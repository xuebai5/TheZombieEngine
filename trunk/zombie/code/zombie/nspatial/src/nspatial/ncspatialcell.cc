#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialcell.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialcell.h"
#include "kernel/npersistserver.h"
#include "nspatial/nvisiblefrustumvisitor.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialmc.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialspace.h"
#include "nspatial/nspatialcellsaver.h"
#include "nspatial/nlightregion.h"
#include "nspatial/nspatialmodels.h"
#include "tinyxml/tinyxml.h"
#include "zombieentity/nctransform.h"
#include "entity/nentityobjectserver.h"
#include "nspatial/nspatialvisitor.h"
#ifndef NGAME
#include "ndebug/nceditor.h"
#endif //NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialCell, nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialCell)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSpatialCell::ncSpatialCell():
m_cellId(-1),
m_cellType(N_SPATIAL_CELL),
m_parentSpace(NULL),
m_visibleFrameId(-1),
m_visitFrameId(-1),
lightRegions(0, 2),
shadowModels(0),
m_lastVisibleCam(nEntityObjectServer::IDINVALID),
m_lastVisitCam(nEntityObjectServer::IDINVALID)
{

}

//------------------------------------------------------------------------------
/**
*/
ncSpatialCell::~ncSpatialCell()
{
    n_assert2((this->GetNumEntities() == 0), 
              "miquelangel.rujula: trying to destroy a cell that has entities!");
}

//------------------------------------------------------------------------------
/**
    flush all the lights in the cell
*/
void 
ncSpatialCell::FlushAllLights()
{
    const nArray<nEntityObject*> &lights = this->GetCategory(nSpatialTypes::CAT_LIGHTS);
    for ( int i(0); i < lights.Size(); ++i )
    {
        lights[i]->GetComponentSafe<ncSpatialLight>()->Flush();
    }
}

//------------------------------------------------------------------------------
/**
    destroy the cell and all its entities
*/
void
ncSpatialCell::DestroyCell()
{
    // delete all the entities
    this->DestroyAllEntities();

    // destroy all the light regions
    this->DestroyLightRegions();

    // destroy all the shadows
    this->DestroyShadowModels();
}

//------------------------------------------------------------------------------
/**
    destroy all the regions of this light
*/
void
ncSpatialCell::DestroyLightRegions()
{
    while ( !this->lightRegions.Empty() )
    {
        n_delete(this->lightRegions.Back());
    }
}

//------------------------------------------------------------------------------
/**
    destroy all the entities in this cell (not portals neither lights)
*/
void
ncSpatialCell::DestroyAllEntities()
{
    #ifndef NGAME
    // restore all the entities in the temporal invisibles array
    while (!this->m_categories[nSpatialTypes::CAT_TEMP_INVISIBLES].Empty())
    {
        this->RestoreTempEntity(m_categories[nSpatialTypes::CAT_TEMP_INVISIBLES].Back());
    }
    n_assert(this->m_categories[nSpatialTypes::CAT_TEMP_INVISIBLES].Empty());
    #endif // !NGAME

    nArray<nEntityObject*> *category = 0;
    for ( int i = 0; i < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++i)
    {
        category = &(this->m_categories[i]);
        while(!category->Empty())
        {
            this->RemoveEntity(*category->Begin());
        }

        n_assert2(category->Size() == 0, 
                  "miquelangel.rujula: a category is not empty after removing all of its entities!");
    }
}

//------------------------------------------------------------------------------
/**
    adds an entity to the cell
*/
bool 
ncSpatialCell::AddEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to an entity!");
    
    int category(entity->GetComponentSafe<ncSpatial>()->GetSpatialCategory());
    
    return this->AddEntity(entity, category);
}

//------------------------------------------------------------------------------
/**
    adds an entity to the cell in a category
*/
bool 
ncSpatialCell::AddEntity(nEntityObject *entity, int category)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to an entity!");

    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "miquelangel.rujula: entity hasn't spatial component!");
    ncSpatialCell *currentCell = spatialComp->GetCell();
    
    if ( currentCell )
    {
        if ( currentCell == this )
        {
            // the entity is already inserted into this cell
            return true;
        }
        else
        {
            n_verify(currentCell->RemoveEntity(entity));
        }
    }

    n_assert(!spatialComp->GetCell());

    n_assert2((category > -1), 
              "miquelangel.rujula: trying to insert an entity in the spatial system that is uncategorized!");
    if (category < 0)
    {
        return false;
    }

    bool addref(true);
    if (category == nSpatialTypes::CAT_CAMERAS)
    {
        addref = false;
    }

    // add the entity to its corresponding category
    if (addref)
    {
        n_assert_return(this->m_categories[category].FindIndex(entity) == -1, true);
        this->m_categories[category].Append(entity);
        entity->AddRef();
        /// @todo miquelangel.rujula review the next line
        //this->GetBBox().extend(spatialComp->GetBBox());
    }
    spatialComp->SetCell(this);

    return true;
}

//------------------------------------------------------------------------------
/**
    removes an entity from the cell and doesn't destroy it

    history
        - 15-jun-2005   ma.garcias  updated to remove global entities.
*/
bool 
ncSpatialCell::RemoveEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to an entity!");

    int category(entity->GetComponentSafe<ncSpatial>()->GetSpatialCategory());
    
    return this->RemoveEntity(entity, category);
}

//------------------------------------------------------------------------------
/**
    removes an entity from the cell and a category without
*/
bool 
ncSpatialCell::RemoveEntity(nEntityObject *entity, int category)
{
    n_assert2(entity, "NULL pointer to an entity, trying to remove it from a cell!");

    n_assert2((category > -1), "Trying to insert an entity in the spatial system that is uncategorized!");
    if (category < 0)
    {
        return false;
    }

    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "miquelangel: entity hasn't spatial component!");

    bool release(true);
    if (category == nSpatialTypes::CAT_CAMERAS)
    {
        release = false;
    }

    // remove the entity from its category
    if (release)
    {
        int index(this->m_categories[category].FindIndex(entity));
        if (index == -1)
        {
            return false;
        }
        this->m_categories[category].EraseQuick(index);
        entity->Release();
    }

    n_assert(this->m_categories[category].FindIndex(entity) == -1);
    if (spatialComp->GetType() == ncSpatial::N_SPATIAL_MC)
    {
        (static_cast<ncSpatialMC*>(spatialComp))->RemoveCell(this);
    }
    else
    {
        spatialComp->SetCell(0);
    }
    
    return true;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    removes an entity temporary from the cell
*/
bool 
ncSpatialCell::RemoveTempEntity(nEntityObject *entity)
{
    n_assert2(entity, "NULL pointer to an entity, trying to remove it from a cell!");

    int category(entity->GetComponentSafe<ncSpatial>()->GetSpatialCategory());
    n_assert2((category > -1), "Wrong spatial category!");

    if (category < 0)
    {
        return false;
    }
    
    int index(this->m_categories[category].FindIndex(entity));
    if (index == -1)
    {
        return false;
    }

    // remove the entity from its category
    this->m_categories[category].EraseQuick(index);

    // add the entity to the temporary invisibles array
    this->m_categories[nSpatialTypes::CAT_TEMP_INVISIBLES].Append(entity);

    return true;
}

//------------------------------------------------------------------------------
/**
    restore an entity that was temporary removed
*/
bool 
ncSpatialCell::RestoreTempEntity(nEntityObject *entity)
{
    n_assert2(entity, "NULL pointer to an entity, trying to remove it from a cell!");

    int index( this->m_categories[nSpatialTypes::CAT_TEMP_INVISIBLES].FindIndex(entity) );
    if (index == -1)
    {
        return false;
    }

    int category( entity->GetComponentSafe<ncSpatial>()->GetSpatialCategory() );
    n_assert2((category > -1), "Wrong spatial category!");
    if (category < 0)
    {
        return false;
    }

    // restore the entity to its corresponding array
    this->m_categories[nSpatialTypes::CAT_TEMP_INVISIBLES].EraseQuick(index);

    // append the entity to its category
    this->m_categories[category].Append(entity);

    return true;
}
#endif // !NGAME

//------------------------------------------------------------------------------
/**
    get neighbor cells
*/
void 
ncSpatialCell::GetNeighbors(nArray<ncSpatialCell*> &neighbors)
{
    const nArray<nEntityObject*> &portals = this->GetCategory(nSpatialTypes::CAT_PORTALS);
    ncSpatialPortal *portal = 0;
    ncSpatialPortal *twin = 0;
    for (int i(0); i < portals.Size(); ++i)
    {
        portal = portals[i]->GetComponentSafe<ncSpatialPortal>();
        twin = portal->GetTwinPortal();
        if ( twin && twin->GetCell() )
        {
            neighbors.Append(twin->GetCell());
        }
    }
}

//------------------------------------------------------------------------------
/**
    get the lights that only affect this cell
*/
void 
ncSpatialCell::GetOnlyCellLights(nArray<ncSpatialLight*> &onlyCellLights)
{
    const nArray<nEntityObject*> &lightsArray = this->GetCategory(nSpatialTypes::CAT_LIGHTS);

    ncSpatialLight* light = 0;
    for (int i(0); i < lightsArray.Size(); ++i)
    {
        light = lightsArray[i]->GetComponentSafe<ncSpatialLight>();
        if ( light->AffectsOnlyACell() )
        {
            onlyCellLights.Append(light);
        }
    }
}

//------------------------------------------------------------------------------
/**
    determine if a given point belongs to this cell
*/
bool 
ncSpatialCell::Contains(const vector3& point) const
{
    return this->m_bbox.contains(point);
}

//------------------------------------------------------------------------------
/**
    determine if a given bbox belongs to this cell
*/
bool 
ncSpatialCell::Contains(const bbox3& box) const
{
    return this->m_bbox.contains(box);
}

//------------------------------------------------------------------------------
/**
    determine if a given sphere belongs to this cell
*/
bool 
ncSpatialCell::Contains(const sphere& sph) const
{
    return sph.isContainedIn(this->m_bbox);
}

//------------------------------------------------------------------------------
/**
    returns the portal in this cell that contains the given point
*/
ncSpatialPortal*
ncSpatialCell::IsInPortals(const vector3 &point)
{
    nArray<nEntityObject*> &portals = this->m_categories[nSpatialTypes::CAT_PORTALS];
    ncSpatialPortal *portal = 0;
    for (int i(0); i < portals.Size(); ++i)
    {
        portal = portals[i]->GetComponentSafe<ncSpatialPortal>();
        if (portal->GetOBBox().contains(point))
        {
            return portal;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    adds a light region to this cell
*/
void 
ncSpatialCell::AddLightRegion(nLightRegion *lightRegion)
{
    n_assert(this->lightRegions.FindIndex(lightRegion) == -1);

    this->lightRegions.Append(lightRegion);
    n_assert(lightRegion->GetCells().FindIndex(this) == -1);
    lightRegion->AddCell(this);
}

//------------------------------------------------------------------------------
/**
    remove a light region from this cell
*/
void 
ncSpatialCell::RemoveLightRegion(nLightRegion *lightRegion)
{
    int index(this->lightRegions.FindIndex(lightRegion));
    if ( index != -1 )
    {
        this->lightRegions.EraseQuick(index);
    }
}

//------------------------------------------------------------------------------
/**
    recalculate the light regions of the lights in this cell
*/
void 
ncSpatialCell::RefreshLights()
{
    const nArray<nEntityObject*> &lights = this->GetCategory(nSpatialTypes::CAT_LIGHTS);
    for ( int i(0); i < lights.Size(); ++i )
    {
        lights[i]->GetComponentSafe<ncSpatialLight>()->Flush();
    }
}

//------------------------------------------------------------------------------
/**
    adds a shadow model to this cell
*/
void 
ncSpatialCell::AddShadowModel(nSpatialShadowModel *shadowModel)
{
    if ( !this->shadowModels )
    {
        this->shadowModels = n_new(nArray<nSpatialShadowModel*>(2, 2));
    }

    /// @todo miquelangel.rujula
    //n_assert2(this->shadowModels->FindIndex(shadowModel) == -1, "Trying to insert a shadow twice in the same cell!");
    if ( this->shadowModels->FindIndex(shadowModel) != -1 )
    {
        return;
    }

    this->shadowModels->Append(shadowModel);
    n_assert(shadowModel->GetCells().FindIndex(this) == -1);
    shadowModel->AddCell(this);
}

//------------------------------------------------------------------------------
/**
    removes a shadow model from this cell
*/
void 
ncSpatialCell::RemoveShadowModel(nSpatialShadowModel *shadowModel)
{
    n_assert(this->shadowModels);

    int index(this->shadowModels->FindIndex(shadowModel));
    if ( index != -1 )
    {
        this->shadowModels->EraseQuick(index);
    }
}

//------------------------------------------------------------------------------
/**
    get the shadow models array
*/
nArray<nSpatialShadowModel*> *
ncSpatialCell::GetShadowModels()
{
    return this->shadowModels;
}

//------------------------------------------------------------------------------
/**
    destroy all the shadow models in this cell
*/
void 
ncSpatialCell::DestroyShadowModels()
{
    if ( this->shadowModels )
    {
        while ( !this->shadowModels->Empty() )
        {
            this->shadowModels->Back()->RemoveFromCell(this);
        }

        n_delete(this->shadowModels);
        this->shadowModels = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncSpatialCell::ReadStaticInfo(const TiXmlElement *cellElem)
{
    n_assert2(cellElem, "miquelangel.rujula");

    // cell's type
    TiXmlElement *cellTypeElem = cellElem->FirstChildElement("Type");
    if (!cellTypeElem)
    {
        return false;
    }
    this->m_cellType = static_cast<nSpatialCellType> (atoi(cellTypeElem->Attribute("Value")));

    // cell's id
    TiXmlElement *cellIdElem = cellElem->FirstChildElement("Id");
    if (!cellIdElem)
    {
        return false;
    }
    this->m_cellId = atoi(cellIdElem->Attribute("Value"));

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncSpatialCell::WriteStaticInfo(TiXmlElement *cellElem)
{
    n_assert2(cellElem, "miquelangel.rujula");

    // cell's type
    TiXmlElement cellTypeElem("Type");
    cellTypeElem.SetAttribute("Value", this->m_cellType);
    cellElem->InsertEndChild(cellTypeElem);

    // cell's id
    TiXmlElement cellIdElem("Id");
    cellIdElem.SetAttribute("Value", this->m_cellId);
    cellElem->InsertEndChild(cellIdElem);

    return true;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    set the layer id to all the entities into this cell
*/
void 
ncSpatialCell::SetLayerId(int layerId)
{
    nArray<nEntityObject*> &portalsArray = this->m_categories[nSpatialTypes::CAT_PORTALS];
    ncSpatialPortal *portal = 0;
    for (int i(0); i < portalsArray.Size(); ++i)
    {
        n_assert2(portalsArray[i]->GetComponent<ncSpatialPortal>(), 
                  "miquelangel.rujula: portal entity expected in the PORTALS category!");
        portal = portalsArray[i]->GetComponentSafe<ncSpatialPortal>();
        portal->GetComponentSafe<ncEditor>()->SetLayerId(layerId);
        if (portal->GetTwinPortal())
        {
            portal->GetTwinPortal()->GetComponentSafe<ncEditor>()->SetLayerId(layerId);
        }
    }
}
#endif

//------------------------------------------------------------------------------
/**
    visibility visitor processing for a basic spatial cell.
*/
void 
ncSpatialCell::Accept(nVisibleFrustumVisitor &visitor)
{
    visitor.Visit(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialCell::SaveCmds(nPersistServer *ps)
{
    if (nComponentObject::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    save the entities contained into this cell
    @todo ma.garcias- if the cell has not been loaded, use current list of ids
*/
void
ncSpatialCell::Save(WizardEntityArray &wizards)
{
    // if the cell has not been loaded, use current list of ids
    if (!this->m_entitiesIdArray.Empty())
    {
        nArray<nEntityObjectId>::iterator idIter = this->m_entitiesIdArray.Begin();
        while (idIter != this->m_entitiesIdArray.End())
        {
            nEntityObjectId entityId = (*idIter);
            int index = (entityId & nEntityObjectServer::IDHIGHMASK) >> 24;
            wizards[index].Append(CellAndEntityId(this->GetId(), entityId));

            ++idIter;
        }
    }

    // classify common entities
    const nArray<nEntityObject*> *categories = this->GetCategories();
    nEntityObject *entity = 0;
    for (int catIndex(0); catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
    {
        const nArray<nEntityObject*> &category = categories[catIndex];
        for (int i(0); i < category.Size(); ++i)
        {
            entity = category[i];

            if (nEntityObjectServer::Instance()->GetEntityObjectType(entity->GetId()) != nEntityObjectServer::Normal)
            {
                continue;
            }

            // entities in some categories need to be loaded directly by the space
            // do not use catIndex, they could be hidden by layer
            int cellId = this->GetId();
            int entityCat = nSpatialServer::Instance()->GetCategory(entity);
            switch (entityCat)
            {
            //case CAT_DEFAULT: check which entities fall in the default category
            case nSpatialTypes::CAT_LIGHTS:
            case nSpatialTypes::CAT_GRAPHBATCHES:
                cellId = -1;
                break;
            }

            int index = (entity->GetId() & nEntityObjectServer::IDHIGHMASK ) >> 24 ;
            n_assert2(( index >= 0 ) && ( index < 0x100 ), "miquelangel.rujula: wrong wizard!");
            wizards[index].Append(CellAndEntityId(cellId, entity->GetId()));
        }
    }
}

//------------------------------------------------------------------------------
/**
    load the entities contained into this cell
    @deprecated keep for backwards compatibility only
*/
bool
ncSpatialCell::Load(const char *spatialPath)
{
    n_assert(spatialPath);

    /// create a new spatial cell saver
    nSpatialCellSaver *cellSaver = static_cast<nSpatialCellSaver*>(nKernelServer::Instance()->New("nspatialcellsaver"));
    cellSaver->SetCell(this);

    /// load the entities ids contained into this cell using the saver
    nString fileName(spatialPath);
    fileName.Append("/Cell_");
    fileName.AppendInt(this->m_cellId);
    fileName.Append(".n2");
    if (!cellSaver->Load(fileName.Get()))
    {
        return false;
    }

    cellSaver->Release();

    // ma.garcias- automatically load entities in the cell for old-style cells
    return this->LoadEntities();
}

//------------------------------------------------------------------------------
/**
    load the entities contained into this cell.
    this uses the list of ids loaded in the initial loading of the space
    and clears it after the process-
*/
bool
ncSpatialCell::LoadEntities()
{
    ncSpatial *spatialComp;

    // ask to the entity server to load all the entities using their ids
    for (nArray<nEntityObjectId>::iterator entityId = this->m_entitiesIdArray.Begin();
                                           entityId != this->m_entitiesIdArray.End();
                                         ++entityId)
    {
        nEntityObject *entity = nEntityObjectServer::Instance()->GetEntityObject(*entityId);
        if (entity)
        {
            n_verify(this->AddEntity(entity));
            spatialComp = entity->GetComponent<ncSpatial>();
            if ( spatialComp )
            {
                spatialComp->CalculateLightLinks();
            }
        }
        else
        {
            n_message("Can't load entity %x, that is supposed to be in cell %d from space %x",
                      *entityId, this->m_cellId, this->m_parentSpace->GetEntityObject()->GetId());
        }
    }

    // empty the list of ids after loading all entities
    this->m_entitiesIdArray.Clear();

    return true;
}

//------------------------------------------------------------------------------
/**
    update the transformation of all the entities into the cell
*/
void 
ncSpatialCell::Update(vector3 &incPos, quaternion &incQuat, const matrix44 &matrix)
{
    // transform the cell's bounding box
    this->m_bbox = this->m_originalBBox;
    this->m_bbox.transform(matrix);

    // get bbox center and extents
    this->bboxCenter = this->m_bbox.center();
    this->bboxExtents = this->m_bbox.extents();

    // transform all the entities into this cell
    ncTransform *transfComp = 0;
    nEntityObject *entity = 0;
    ncSpatial *spatialComp = 0;
    nArray<nEntityObject*> &indoorBrushes = this->m_categories[nSpatialTypes::CAT_INDOOR_BRUSHES];
    for (int i(0); i < indoorBrushes.Size(); ++i)
    {
        entity = indoorBrushes[i];

        n_assert2(entity->IsA("neindoorbrush"), 
                  "miquelangel.rujula: there's an entity that is not an indoor brush in the INDOOR_BRUSHES category!");

        spatialComp = entity->GetComponent<ncSpatial>();
        transfComp = entity->GetComponent<ncTransform>();
        n_assert2(transfComp, "miquelangel.rujula: trying to transform an entity without tranform component!");
        transfComp->DisableUpdate(ncTransform::cSpatial);
        transfComp->SetPosition(transfComp->GetTransform().pos_component() + incPos);
        transfComp->EnableUpdate(ncTransform::cSpatial);
        quaternion newQuat = transfComp->GetTransform().get_quaternion() + incQuat;
        newQuat.normalize();
        transfComp->SetQuat(newQuat);   
    }

    nArray<nEntityObject*> &portals = this->m_categories[nSpatialTypes::CAT_PORTALS];
    ncSpatialPortal* portal = 0;
    // transform all the portals into this cell
    for (int i(0); i < portals.Size(); ++i)
    {
        portal = portals[i]->GetComponentSafe<ncSpatialPortal>();
        n_assert2(portal, "miquelangel.rujula: there's an entity that is not a portal in the PORTALS category!");
        transfComp = portal->GetComponent<ncTransform>();
        n_assert2(transfComp, "miquelangel.rujula: trying to transform a portal without tranform component!");
        transfComp->DisableUpdate(ncTransform::cSpatial);
        transfComp->SetPosition(transfComp->GetTransform().pos_component() + incPos);
        transfComp->EnableUpdate(ncTransform::cSpatial);
        quaternion newQuat = transfComp->GetTransform().get_quaternion() + incQuat;
        newQuat.normalize();
        transfComp->SetQuat(newQuat);   

        // if this portal's twin is in another space, also has to be transformed
        if (portal->GetTwinPortal() && 
           (!portal->GetTwinPortal()->GetCell() || 
             portal->GetCell()->GetType() != portal->GetTwinPortal()->GetCell()->GetType()))
        {
            transfComp = portal->GetTwinPortal()->GetComponent<ncTransform>();
            n_assert2(transfComp, "miquelangel.rujula: trying to transform a portal without tranform component!");
            transfComp->DisableUpdate(ncTransform::cSpatial);
            transfComp->SetPosition(transfComp->GetTransform().pos_component() + incPos);
            transfComp->EnableUpdate(ncTransform::cSpatial);
            quaternion newQuat = transfComp->GetTransform().get_quaternion() + incQuat;
            newQuat.normalize();
            transfComp->SetQuat(newQuat);   
        }
    }
}
