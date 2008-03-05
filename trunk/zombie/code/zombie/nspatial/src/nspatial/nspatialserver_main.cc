#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nspatialserver_main.cc
//  (C) 2004 Conjurer Services, S.A.
//  @author Miquel Angel Rujula <>
//------------------------------------------------------------------------------

#include "kernel/ntimeserver.h"
#include "mathlib/sphere.h"

#include "nspatial/nspatialserver.h"
#include "nspatial/nvisiblefrustumvisitor.h"
#include "nspatial/nspatialvisitor.h"
#include "nspatial/nspatialquadtreespacebuilder.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/nspatialmodels.h"
#include "nspatial/ncspatialglobalspace.h"
#include "nspatial/ncspatialglobalcell.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialoctree.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialbatch.h"
#include "nscene/ncscene.h"

#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"

#include "zombieentity/nctransform.h"

#include <math.h>

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif //NGAME

nNebulaScriptClass(nSpatialServer, "nroot");

nSpatialServer* nSpatialServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nSpatialServer::nSpatialServer():
m_frameId(-1),
m_totalNumEntities(0),
m_useOctrees(false),
#ifndef NGAME
execMode(EDITOR),
m_editIndoorMode(false),
seeAllSelectedIndoors(false),
#endif // !NGAME
profVis("profVis", true),
profVisGlobalEnts("profVis_Globals", true)
{
    n_assert(0 == Singleton);
    Singleton = this;

    // create spatial visitor
    this->spatialVisitor = n_new(nSpatialVisitor);

    //create visibility visitor
    this->visibilityVisitor = n_new(nVisibleFrustumVisitor);

    this->spatialVisitor->SetSpatialServer(this);
    this->visibilityVisitor->SetSpatialServer(this);
    this->m_globalSpace = n_new(ncSpatialGlobalSpace);
    this->m_globalSpace->AddSpatialCell(n_new(ncSpatialGlobalCell));
    this->m_worldBox.begin_extend();

    // bind to signal of delete entity
    nEntityObjectServer::Instance()->BindSignal( nEntityObjectServer::SignalEntityDeleted, 
                                                 this, 
                                                 &nSpatialServer::EntityDeleted, 
                                                 0 );

    // initialize the categories that have to be used to determine visibility for
    this->m_visCategoriesMap.Append(nSpatialTypes::CAT_DEFAULT);
    this->m_visCategoriesMap.Append(nSpatialTypes::CAT_GRAPHBATCHES);
    this->m_visCategoriesMap.Append(nSpatialTypes::CAT_PORTALS);
    this->m_visCategoriesMap.Append(nSpatialTypes::CAT_FACADES);
    this->m_visCategoriesMap.Append(nSpatialTypes::CAT_BRUSHES);
    this->m_visCategoriesMap.Append(nSpatialTypes::CAT_INDOOR_BRUSHES);
    this->m_visCategoriesMap.Append(nSpatialTypes::CAT_AGENTS);
    this->m_visCategoriesMap.Append(nSpatialTypes::CAT_WEAPONS);
    this->m_visCategoriesMap.Append(nSpatialTypes::CAT_VEHICLES);
        
    // set the visibility categories map to the visibility visitor
    this->visibilityVisitor->SetVisCategoriesMap();

#ifndef  __ZOMBIE_EXPORTER__
    // bind to level creation and loading
    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelLoaded, 
                                          this,
                                          &nSpatialServer::OnLevelLoaded,
                                          0);
#endif
}

//------------------------------------------------------------------------------
/**
*/
nSpatialServer::~nSpatialServer()
{
    // destroy all the spaces
    this->ReleaseSpaces();

    this->m_spatialCameras.Reset();

    // destroy global space
    this->m_globalSpace->DestroySpace();
    n_delete(this->m_globalSpace);

    // release spatial visitor
    n_delete(this->spatialVisitor);

    // release visibility visitor
    n_delete(this->visibilityVisitor);

    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    callback for the level loaded
*/
void
nSpatialServer::OnLevelLoaded()
{
    this->FlushAllLights();
}

//------------------------------------------------------------------------------
/**
    flush all the lights in the world
*/
void
nSpatialServer::FlushAllLights()
{
    if ( this->m_outdoorSpace.isvalid() )
    {
        ncSpatialQuadtree *quadtree = this->GetQuadtreeSpace();
        quadtree->FlushAllLights();
    }

    for ( int i(0); i < this->m_indoorSpaces.Size(); ++i )
    {
        this->m_indoorSpaces[i]->FlushAllLights();
    }
}

//------------------------------------------------------------------------------
/**
    Insert the entity in the correponding cell and space.

    Assuming that the entity's spatial component is in world coordinates.

    First, it finds the cell where the entity is, searching arround all the spaces
    registered in the spatial server. After that, if it could find the cell, inserts
    the entity and returns true. If not, it returns false.
*/
bool
nSpatialServer::InsertEntity(nEntityObject *entity)
{
    n_assert2(entity, "Error: NULL pointer to entity trying to insert it in the Spatial Server!");

    ncSpatial *spatialComponent = entity->GetComponent<ncSpatial>();

    n_assert2(spatialComponent, "miquelangel.rujula: entity has no spatial component!");

    // this array has all the spaces that can contain the entity
    nArray<ncSpatialIndoor*> possibleSpaces;

    vector3 point;
    ncSpatialPortal *portalComp = entity->GetComponent<ncSpatialPortal>();
    if ( portalComp )
    {
        point = portalComp->GetClipCenter();
    }
    else
    {
        point = spatialComponent->GetBBoxCenter();
    
        // first we look if it's in any indoor space
        for (nArray<ncSpatialIndoor*>::iterator indoorSpace  = this->m_indoorSpaces.Begin();
                                                indoorSpace != this->m_indoorSpaces.End();
                                                ++indoorSpace)
        {
#ifndef NGAME
            if (!(*indoorSpace)->IsEnabled())
            {
                continue;
            }
#endif // !NGAME
            if ((*indoorSpace)->GetBBox().contains(point))
            {
                possibleSpaces.Append((*indoorSpace));
            }
        }

        // at least one indoor space contains the entity
        if (possibleSpaces.Size() > 0)
        {
            // we'll try to insert it in all the spaces, one by one, until we
            // find the one that really contains it. If it can't find it, let's see
            // if it's in the outdoors
            for (nArray<ncSpatialIndoor*>::iterator indoorSpace  = possibleSpaces.Begin();
                                                    indoorSpace != possibleSpaces.End();
                                                  ++indoorSpace)
            {
                if ((*indoorSpace)->AddEntity(entity))
                {
                    // remove entity from global space just in case
                    this->RemoveGlobalEntity(entity);
                    // the entity has been inserted in this indoor space
                    ++this->m_totalNumEntities;
                    return true;
                }
            }
        }
    }

    // there isnt't an indoor space containing the entity. Let's see if the outdoor space does.
    if (this->m_outdoorSpace.isvalid())
    {
        ncSpatialQuadtree * outdoorSpace = this->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>();

        if (spatialComponent->GetType() == ncSpatial::N_SPATIAL_CAMERA)
        {
            // try to remove camera from global space first just in case
            this->RemoveGlobalEntity(entity);
            // add the camera to the outdoor root cell
            spatialComponent->SetCell( outdoorSpace->GetRootCell() );
            return true;
        }

        if (outdoorSpace->GetBBox().contains(point))
        {
            // the entity is in the outdoor. Let's insert it.
            if (outdoorSpace->AddEntity(entity))
            {
                // the entity has been inserted in the quadtree space
                ++this->m_totalNumEntities;
                return true;
            }
        }
    }

    if (this->m_useOctrees && this->m_octreeSpace.isvalid())
    {
        ncSpatialOctree * octreeSpace = this->m_octreeSpace->GetComponentSafe<ncSpatialOctree>();

        if (octreeSpace->AddEntity(entity))
        {
            // try to remove entity from global space just in case
            this->RemoveGlobalEntity(entity);
            return true;
        }
        return false;
    }

    // the entity it's outside all the spaces, let's insert it in the global entities array
    this->InsertGlobalEntity(entity);

    return false;
}

//------------------------------------------------------------------------------
/**
    insert a global entity
*/
bool 
nSpatialServer::InsertGlobalEntity(nEntityObject *entity, bool alwaysVisible)
{
    n_assert2(entity, 
              "miquelangel.rujula: NULL pointer to entity trying to insert it to the global entities array!");
    n_assert2(this->m_globalSpace->GetCellsArray()[0], 
              "miquelangel.rujula: global space's cell not created!");

    if ( alwaysVisible )
    {
        this->m_globalSpace->AddAlwaysVisibleEntity(entity);
    }
    else
    {
        (this->m_globalSpace->GetCellsArray())[0]->AddEntity(entity);
        ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
        if (spatialComp && (spatialComp->GetType() == ncSpatial::N_SPATIAL_CAMERA) && spatialComp->GetCell())
        {
            spatialComp->GetCell()->RemoveEntity(entity);
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    remove a global entity
*/
bool 
nSpatialServer::RemoveGlobalEntity(nEntityObject *entity)
{
    n_assert2(entity, 
              "miquelangel.rujula: NULL pointer to entity trying to insert it to the global entities array!");
    n_assert2(this->m_globalSpace->GetCellsArray()[0], 
              "miquelangel.rujula: global space's cell not created!");

    if (this->GetGlobalAlwaysVisibleEntities().Find(entity))
    {
        return this->m_globalSpace->RemoveAlwaysVisibleEntity(entity);
    }
    else
    {
        return this->m_globalSpace->GetCellsArray()[0]->RemoveEntity(entity);
    }
}

//------------------------------------------------------------------------------
/**
    get the global entities array
*/
void
nSpatialServer::GetAllGlobalEntities(nArray<nEntityObject*> &globalEntities) const
{
    const nArray<nEntityObject*> *categories = this->m_globalSpace->GetCellsArray()[0]->GetCategories();
    for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
    {
        const nArray<nEntityObject*> &category = categories[catIndex];
        for (int i = 0; i < category.Size(); ++i)
        {
            globalEntities.Append(category[i]);
        }
    }

    const nArray<nEntityObject*> &alwaysVisibles = this->m_globalSpace->GetAlwaysVisibleEntities();
    for ( int i(0); i < alwaysVisibles.Size(); ++i )
    {
        globalEntities.Append(alwaysVisibles[i]);
    }
}

//------------------------------------------------------------------------------
/**
    get the global common entities array
*/
void
nSpatialServer::GetCommonGlobalEntities(nArray<nEntityObject*> &globalEntities) const
{
    const nArray<nEntityObject*> *categories = this->m_globalSpace->GetCellsArray()[0]->GetCategories();
    for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
    {
        const nArray<nEntityObject*> &category = categories[catIndex];
        for (int i = 0; i < category.Size(); ++i)
        {
            globalEntities.Append(category[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    get the global always visible entities array
*/
const nArray<nEntityObject*> &
nSpatialServer::GetGlobalAlwaysVisibleEntities() const
{
    return this->m_globalSpace->GetAlwaysVisibleEntities();
}

//------------------------------------------------------------------------------
/**
    insert the entity in the first indoor space that contains or 
    intersects with the entity, depending on the given flags
*/
bool
nSpatialServer::InsertEntityOneIndoor(nEntityObject *entity, 
                                      const int flags)
{
    for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_indoorSpaces.Begin();
                                            pIndoorSpace != this->m_indoorSpaces.End();
                                            ++pIndoorSpace)
    {
        if ((*pIndoorSpace)->ncSpatialSpace::AddEntity(entity, flags))
        {
            // the entity has been inserted in this indoor space
            ++this->m_totalNumEntities; //////// RUJU, INCREMENT IT?????????
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    insert the entity in all the indoor spaces that contain or intersect with
    the given entity, depending on the given flags
*/
bool
nSpatialServer::InsertEntityAllIndoors(nEntityObject *entity, 
                                       const int flags)
{
    bool inserted = false;
    for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_indoorSpaces.Begin();
                                            pIndoorSpace != this->m_indoorSpaces.End();
                                            ++pIndoorSpace)
    {
        if ((*pIndoorSpace)->ncSpatialSpace::AddEntity(entity, flags))
        {
            // the entity has been inserted in this indoor space
            ++this->m_totalNumEntities; //////// RUJU, INCREMENT IT?????????
            inserted = true;
        }
    }

    return inserted;
}

//------------------------------------------------------------------------------
/**
    insert the given entity in the outdoors (if it's registered) depending on 
    the given flags
*/
bool
nSpatialServer::InsertEntityOutdoors(nEntityObject *entity, 
                                     const int flags)
{
    if (this->m_outdoorSpace.isvalid())
    {
        ncSpatialQuadtree * outdoorSpace = this->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>();
        return outdoorSpace->ncSpatialSpace::AddEntity(entity, flags);
    }
    else
    {
        // there is no quadtree space
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    insert the given entity into the octree space (if it's registered) depending
    on the flags
*/
bool
nSpatialServer::InsertEntityOctree(nEntityObject *entity, 
                                   const int flags)
{
    if (this->m_octreeSpace.isvalid())
    {
        return this->m_octreeSpace->GetComponentSafe<ncSpatialSpace>()->AddEntity(entity, flags);
    }
    else
    {
        // there is no octree space
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
nSpatialServer::InsertEntity(nEntityObject *entity, int flags)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to entity!");

    if (flags & nSpatialTypes::SPF_ONE_INDOOR)
    {
        return this->InsertEntityOneIndoor(entity, flags);
    }
    else if (flags & nSpatialTypes::SPF_ALL_INDOORS)
    {
        return this->InsertEntityAllIndoors(entity, flags);
    }
    
    if (flags & nSpatialTypes::SPF_OUTDOORS)
    {
        return this->InsertEntityOutdoors(entity, flags);
    }
    
    if (flags & nSpatialTypes::SPF_OCTREE)
    {
        return this->InsertEntityOctree(entity, flags);
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    move an entity from a cell to another one. The entity's spatial component 
    contains the origin cell
*/
bool 
nSpatialServer::MoveEntity(nEntityObject *entity, ncSpatialCell *destCell)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to entity!");
    n_assert2(destCell, "miquelangel.rujula: trying to move an entity to a cell whose pointer is NULL!");

    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "miquelangel.rujula: entity has no spatial component. Can't move it!");
    ncSpatialCell *origCell = spatialComp->GetCell();
    if (!origCell)
    {
        return false;
    }
    
    int category = entity->GetComponentSafe<ncSpatial>()->GetSpatialCategory();

    if (!origCell->RemoveEntity(entity, category))
    {
        return false;
    }
    
    n_verify(destCell->AddEntity(entity, category));

    return true;
}

//------------------------------------------------------------------------------
/**
    adjust quadtree's cells' bounding boxes with the given offset
*/
void 
nSpatialServer::AdjustQuadtreeBBoxes(float offset)
{
    nEntityObject *outdoor = this->GetOutdoorEntity();
    if (outdoor)
    {
        outdoor->GetComponentSafe<ncSpatialQuadtree>()->AdjustBBoxesWith(offset);
    }
}

//------------------------------------------------------------------------------
/**
    get all the disconnected portals in the registered indoor spaces
*/
void 
nSpatialServer::GetDisconnectedPortals(nArray<ncSpatialPortal*> *portalsArray) const
{
    for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_indoorSpaces.Begin();
                                     pIndoorSpace != this->m_indoorSpaces.End();
                                     ++pIndoorSpace)
    {
        nArray<ncSpatialPortal*> disconnectedPortals;
        (*pIndoorSpace)->GetDisconnectedPortals(disconnectedPortals);
        for (nArray<ncSpatialPortal*>::iterator pOuterPortal  = disconnectedPortals.Begin();
                                         pOuterPortal != disconnectedPortals.End();
                                         ++pOuterPortal)
        {
            portalsArray->Append((*pOuterPortal));
            
        }
    }
}

//------------------------------------------------------------------------------
/**
    Get the entities in the bounding box using the spatial collector.
*/
void 
nSpatialServer::GetEntities(const bbox3 &box, nSpatialCollector *collector)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->Visit(box, collector);
}

//------------------------------------------------------------------------------
/**
    Get the entities in the sphere using the spatial collector.
*/
void 
nSpatialServer::GetEntities(const sphere &sph, nSpatialCollector *collector)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->Visit(sph, collector);
}

//------------------------------------------------------------------------------
/**
    get the entities whose position is in the sphere using the spatial collector
*/
void 
nSpatialServer::GetEntitiesByPos(const sphere &sph, nSpatialCollector *collector)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesByPos(sph, collector);
}

//------------------------------------------------------------------------------
/**
    get the entities whose position is in the sphere and add them to the given
    array
*/
void 
nSpatialServer::GetEntitiesByPos(const sphere &sph, nArray<nEntityObject*> *entities)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesByPos(sph, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags and a collector
*/
void 
nSpatialServer::GetEntitiesCategory(const bbox3 &box, 
                                    int category, 
                                    const int flags, 
                                    nSpatialCollector *collector)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesCategory(box, category, flags, collector);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags and a collector
*/
void 
nSpatialServer::GetEntitiesCategory(const sphere &sph,  
                                    int category, 
                                    const int flags, 
                                    nSpatialCollector *collector)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesCategory(sph, category, flags, collector);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags and a collector
*/
void 
nSpatialServer::GetEntitiesCategory(const vector3 &point, 
                                    int category, 
                                    const int flags, 
                                    nSpatialCollector *collector)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesCategory(point, category, flags, collector);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
void 
nSpatialServer::GetEntitiesCategory(const bbox3 &box, 
                                    int category, 
                                    const int flags, 
                                    nArray<nEntityObject*> &entities)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesCategory(box, category, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
void 
nSpatialServer::GetEntitiesCategory(const sphere &sph,  
                                    int category, 
                                    const int flags, 
                                    nArray<nEntityObject*> &entities)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesCategory(sph, category, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
void 
nSpatialServer::GetEntitiesCategory(const vector3 &point, 
                                    int category, 
                                    const int flags, 
                                    nArray<nEntityObject*> &entities)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesCategory(point, category, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities various categories using the given flags
*/
void 
nSpatialServer::GetEntitiesCategories(const bbox3 &box, 
                                      const nArray<int> &categories,
                                      const int flags, 
                                      nArray<nEntityObject*> &entities)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesCategories(box, categories, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
void 
nSpatialServer::GetEntitiesCategories(const sphere &sph,  
                                      const nArray<int> &categories,
                                      const int flags, 
                                      nArray<nEntityObject*> &entities)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesCategories(sph, categories, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
void 
nSpatialServer::GetEntitiesCategories(const vector3 &point, 
                                      const nArray<int> &categories,
                                      const int flags, 
                                      nArray<nEntityObject*> &entities)
{
    n_assert(this->spatialVisitor);
    this->spatialVisitor->GetEntitiesCategories(point, categories, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialServer::GetEntitiesCellsCategory(const bbox3 &box, 
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesCellsCategory(box, category, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialServer::GetEntitiesCellsCategory(const sphere &sph,  
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesCellsCategory(sph, category, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialServer::GetEntitiesCellsCategory(const vector3 &point, 
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesCellsCategory(point, category, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities various categories using the given flags
*/
bool 
nSpatialServer::GetEntitiesCellsCategories(const bbox3 &box, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesCellsCategories(box, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialServer::GetEntitiesCellsCategories(const sphere &sph,  
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesCellsCategories(sph, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialServer::GetEntitiesCellsCategories(const vector3 &point, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesCellsCategories(point, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool
nSpatialServer::GetEntitiesUsingCellsCategory(const bbox3 &box, 
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesUsingCellsCategory(box, category, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialServer::GetEntitiesUsingCellsCategory(const sphere &sph,  
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesUsingCellsCategory(sph, category, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialServer::GetEntitiesUsingCellsCategory(const vector3 &point, 
    int category, 
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesUsingCellsCategory(point, category, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities various categories using the given flags
*/
bool 
nSpatialServer::GetEntitiesUsingCellsCategories(const bbox3 &box, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesUsingCellsCategories(box, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialServer::GetEntitiesUsingCellsCategories(const sphere &sph,  
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesUsingCellsCategories(sph, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of a determined category using the given flags
*/
bool 
nSpatialServer::GetEntitiesUsingCellsCategories(const vector3 &point, 
    const nArray<int> &categories,
    const int flags, 
    nArray<nEntityObject*> &entities,
    const nArray<ncSpatialCell*> &cells) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetEntitiesUsingCellsCategories(point, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the nearest entity to a given position
*/
nEntityObject *
nSpatialServer::GetNearestEntityTo(const vector3 &pos, nEntityObjectId id) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetNearestEntityTo(pos, id);
}

//------------------------------------------------------------------------------
/**
    get the nearest entity to a given position, inside the given sphere
*/
nEntityObject *
nSpatialServer::GetNearestEntityTo(const sphere &sph, nEntityObjectId id) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->GetNearestEntityTo(sph, id);
}

//------------------------------------------------------------------------------
/**
    get the category the class belongs to
*/
int 
nSpatialServer::GetCategory(nEntityObject *entity)
{
    if (entity->IsA("nebrush") || entity->IsA("newalkablebrush"))
    {
        return nSpatialTypes::CAT_BRUSHES;
    }
    else if (entity->IsA("nemirage"))
    {
        ncSpatial* spComp = entity->GetComponentSafe<ncSpatial>();
        if ( spComp->GetIndoorSpace() )
        {
            return nSpatialTypes::CAT_FACADES;
        }
        else
        {
            return nSpatialTypes::CAT_BRUSHES;
        }
    }
    else if (entity->IsA("neoccluder"))
    {
        return nSpatialTypes::CAT_OCCLUDERS;
    }
    else if (entity->IsA("nevehicle") || entity->IsA("newheel"))
    {
        return nSpatialTypes::CAT_VEHICLES;
    }
    else if (entity->IsA("nernswaypoint") || entity->IsA("newaypoint"))
    {
        return nSpatialTypes::CAT_WAYPOINTS;
    }
    else if (entity->IsA("neagententity") || entity->IsA("neplayer"))
    {
        return nSpatialTypes::CAT_AGENTS;
    }
    else if (entity->IsA("neweapon"))
    {
        return nSpatialTypes::CAT_WEAPONS;
    }
    else if (entity->IsA("neweaponaddon"))
    {
        return nSpatialTypes::CAT_WEAPONS;
    }
    else if (entity->IsA("neindoorbrush"))
    {
        return nSpatialTypes::CAT_INDOOR_BRUSHES;      
    }
    else if (entity->IsA("neportal"))
    {
        return nSpatialTypes::CAT_PORTALS;
    }
    else if (entity->IsA("nelight") || entity->IsA("neoutlight") || entity->IsA("nedotlaser") )
    {
        return nSpatialTypes::CAT_LIGHTS;
    }
	else if (entity->IsA("nesoundsource"))
    {
        return nSpatialTypes::CAT_SOUNDSSOURCES;
    }
    else if (entity->IsA("necamera"))
    {
        return nSpatialTypes::CAT_CAMERAS;
    }
    else if (entity->IsA("nenavmeshnode"))
    {
        return nSpatialTypes::CAT_NAVMESHNODES;
    }
    else if (entity->IsA("nenavmeshobstacle"))
    {
        return nSpatialTypes::CAT_NAVMESHOBSTACLES;
    }
    else if (entity->IsA("nestaticbatch"))
    {
        return nSpatialTypes::CAT_GRAPHBATCHES;
    }
    else if (entity->IsA("nescene"))
    {
        return nSpatialTypes::CAT_SCENE;
    }
    else if (entity->IsA("nespawner") || entity->IsA("nespawnpoint") || 
        entity->IsA("neareatrigger"))
    {
        return nSpatialTypes::CAT_NON_VISIBLES;
    }
    else if (entity->IsA("neareaevent"))
    {
        return nSpatialTypes::CAT_AREA_EVENTS;
    }

    // the class is not categorized
    return nSpatialTypes::CAT_DEFAULT;
}

//------------------------------------------------------------------------------
/**
    tell if a point is approximately inside some indoor

    The bboxes of the indoor's cells are used to check if a point is inside an
    indoor. This makes sure that the function return true for any point that's
    inside an indoor, but can return true also for some points slightly outside
    the indoor.
*/
bool 
nSpatialServer::IsInsideIndoorApprox( const vector3& point ) const
{
    for ( int i(0); i < this->m_indoorSpaces.Size(); ++i )
    {
        if ( this->m_indoorSpaces[i]->SearchCellContaining( point, nSpatialTypes::SPF_USE_CELL_BBOX ) )
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    toggle the octree visibility flag
*/
void 
nSpatialServer::ToggleUseOctrees()
{
    if (this->m_useOctrees)
    {
        this->SetRecalculateCamPos(true);
        this->m_useOctrees = false;
    }
    else
    {
        this->m_useOctrees = true;
    }

    ncTransform *trComp = 0;
    for (nArray<ncSpatialCamera*>::iterator pCamera  = this->m_spatialCameras.Begin();
                                            pCamera != this->m_spatialCameras.End();
                                            ++pCamera)
    {
        trComp = (*pCamera)->GetEntityObject()->GetComponent<ncTransform>();
        n_assert2(trComp, "miquelangel.rujula: camera has no transformation component!");
        trComp->SetPosition(trComp->GetPosition());
    }
}

//------------------------------------------------------------------------------
/**
    set the use octree visibility flag
*/
void 
nSpatialServer::SetUseOctrees(bool flag)
{
    this->m_useOctrees = flag;

    if (this->m_useOctrees)
    {
        this->SetRecalculateCamPos(true);
    }
    
    ncTransform *trComp = 0;
    for (nArray<ncSpatialCamera*>::iterator pCamera  = this->m_spatialCameras.Begin();
                                            pCamera != this->m_spatialCameras.End();
                                            ++pCamera)
    {
        trComp = (*pCamera)->GetEntityObject()->GetComponent<ncTransform>();
        n_assert2(trComp, "miquelangel.rujula: camera has no transformation component!");
        trComp->SetPosition(trComp->GetPosition());
    }
}

//------------------------------------------------------------------------------
/**
    connect the indoor spaces to the outdoor space. Searches all the portals of
    the indoor spaces that connect to an outer space (portal->m_otherSideCell == 0),
    it clones them and put them in the outdoor space.
*/
bool
nSpatialServer::ConnectSpaces()
{
    if (!this->m_outdoorSpace.isvalid())
    {
        return false;
    }

    for (nArray<ncSpatialIndoor*>::iterator pIndoorSpace  = this->m_indoorSpaces.Begin();
                                            pIndoorSpace != this->m_indoorSpaces.End();
                                            ++pIndoorSpace)
    {
        this->ConnectSpace((*pIndoorSpace));
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    connect the parameter indoor space to the outdoor space. Searches all the portals of
    the indoor space that connect to an outer space (portal->m_otherSideCell == 0),
    it clones them and put them into the outdoor space.
*/
bool
nSpatialServer::ConnectSpace(ncSpatialIndoor* indoor)
{
    if (!this->m_outdoorSpace.isvalid())
    {
        return false;
    }

    nArray<ncSpatialPortal*> disconnectedPortals;
    indoor->GetDisconnectedPortals(disconnectedPortals);
    ncSpatialPortal* outerPortal;
    for (nArray<ncSpatialPortal*>::iterator pOuterPortal  = disconnectedPortals.Begin();
                                            pOuterPortal != disconnectedPortals.End();
                                            ++pOuterPortal)
    {
        outerPortal = (*pOuterPortal);
        // create a clone of the outer portal
        nEntityObject * clonedEntity = static_cast<nEntityObject*> (outerPortal->GetEntityObject()->Clone());
        ncSpatialPortal* clonePortal = clonedEntity->GetComponentSafe<ncSpatialPortal>();
        
        // copy the content of the outer portal
        *(clonePortal->GetComponent<ncSpatialPortal>()) = *(outerPortal->GetComponent<ncSpatialPortal>());

//        clonePortal->ncSpatialPortal::SetPosition(clonePortal->GetBBox().center());
#ifndef NGAME
        n_assert2(outerPortal->GetComponentSafe<ncEditor>()->IsSetEditorKey("layerId"), 
                  "miquelangel.rujula: trying to clone a portal to connect an indoor space, and can't put layer id!");
        clonePortal->GetComponentSafe<ncEditor>()->SetLayerId( outerPortal->GetComponentSafe<ncEditor>()->GetEditorKeyInt("layerId") );
#endif // !NGAME
        clonePortal->SetCell(0);

        // make the clone portal and its original point between them
        clonePortal->SetTwinPortal(outerPortal);
        clonePortal->SetOriginalPlane(-outerPortal->GetPlane().a, 
                                      -outerPortal->GetPlane().b, 
                                      -outerPortal->GetPlane().c, 
                                      -outerPortal->GetPlane().d);
        clonePortal->SetPlane(clonePortal->GetOriginalPlane());

        // reverse the original portal vertices
        const vector3 *portalVertices = outerPortal->GetOriginalVertices();
        vector3 reversedVertices[4];
        reversedVertices[0] = portalVertices[1];
        reversedVertices[1] = portalVertices[0];
        reversedVertices[2] = portalVertices[3];
        reversedVertices[3] = portalVertices[2];
        clonePortal->SetOriginalVertices(reversedVertices);

        // reverse the portal vertices
        portalVertices = outerPortal->GetVertices();
        reversedVertices[0] = portalVertices[1];
        reversedVertices[1] = portalVertices[0];
        reversedVertices[2] = portalVertices[3];
        reversedVertices[3] = portalVertices[2];
        clonePortal->SetVertices(reversedVertices);

#if 0 // this is handled by the loader server
        // load the clone portal's resources
        ncLoader* cloneLoader = clonePortal->GetComponentSafe<ncLoader>();
        cloneLoader->LoadComponents();
#endif

        // insert the clone portal into the outdoor space
        if (!this->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>()->AddEntity(clonePortal->GetEntityObject()))
        {
            // can't insert the clone portal into the outdoor space. Try to insert it into the global space
            if (!this->InsertGlobalEntity(clonePortal->GetEntityObject()))
            {
                clonePortal->GetEntityObject()->Release();
                return false;
            }
        }

        ++this->m_totalNumEntities;

        outerPortal->SetTwinPortal(clonePortal);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    connect indoors that have a common portal
*/
void 
nSpatialServer::ConnectIndoors()
{
    //unsigned int connectedIndoors = 0;
    int i;
    nArray<ncSpatialPortal*> outdoorPortals1;
    ncSpatialIndoor* indoor1 = 0;
    ncSpatialIndoor* indoor2 = 0;
    for (i = 0; i < (this->m_indoorSpaces.Size() - 1); ++i)
    {
        // get indoor 'i' outer portals
        indoor1 = this->m_indoorSpaces.At(i);
        indoor1->GetOutdoorPortals(outdoorPortals1);
        int op1;
        ncSpatialPortal *portal1 = 0;
        bool connected = false;
        for (op1 = 0; op1 < outdoorPortals1.Size(); ++op1)
        {
            // try to connect portal 'op1' to all the outer portals of the other
            // indoors that haven't been connected yet
            portal1 = outdoorPortals1.At(op1);
            int j;
            for (j = i + 1; j < this->m_indoorSpaces.Size(); ++j)
            {
                // if it's not the same indoor, if it wasn't connected yet,
                // and if both indoors are touching, try to connect them
                indoor2 = this->m_indoorSpaces.At(j);
                nArray<ncSpatialPortal*> outdoorPortals2;
                indoor2->GetOutdoorPortals(outdoorPortals2);
                int op2;
                ncSpatialPortal *portal2 = 0;
                for (op2 = 0; op2 < outdoorPortals2.Size(); ++op2)
                {
                    portal2 = outdoorPortals2.At(op2);
                    if ( portal1->Touches(portal2) )
                    {
                        if ( portal1->ConnectTo(portal2) )
                        {
                            connected = true;
                        }
                        break;
                    }
                }

                if ( connected )
                {
                    break;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    register an space in the spatial server to manage it
*/
void 
nSpatialServer::RegisterSpace(ncSpatialSpace *space)
{
    n_assert2(space, "miquelangel.rujula: NULL pointer to space!");

    switch(space->GetType())
    {
        case ncSpatialSpace::N_INDOOR_SPACE:

            this->RegisterSpace(space->GetComponentSafe<ncSpatialIndoor>());
            break;
        
        case ncSpatialSpace::N_QUADTREE_SPACE:

            this->RegisterSpace(space->GetComponentSafe<ncSpatialQuadtree>());
            break;

        case ncSpatialSpace::N_OCTREE_SPACE:

            this->RegisterSpace(space->GetComponentSafe<ncSpatialOctree>());
            break;
    }
    
}


//------------------------------------------------------------------------------
/**
    register an indoor space in the spatial server to manage it
*/
void 
nSpatialServer::RegisterSpace(ncSpatialIndoor* indoorSpace)
{
    n_assert2(indoorSpace, "miquelangel.rujula");
    this->m_indoorSpaces.Append(indoorSpace);
    indoorSpace->GetEntityObject()->AddRef();

    if (!this->m_outdoorSpace.isvalid())
    {
        this->m_worldBox.begin_extend();
        for (int i = 0; i < this->m_indoorSpaces.Size(); ++i)
        {
            this->m_worldBox.extend(this->m_indoorSpaces[i]->GetBBox());
        }
    }
}

//------------------------------------------------------------------------------
/**
    register an outdoor space in the spatial server to manage it
*/
void 
nSpatialServer::RegisterSpace(ncSpatialQuadtree *outdoorSpace)
{
    n_assert2(outdoorSpace, "miquelangel.rujula");

    // if there is already a quadtree space, release it
    this->ReleaseOutdoorSpace();

    this->m_outdoorSpace = outdoorSpace->GetEntityObject();
    this->m_outdoorSpace->AddRef();

    this->m_worldBox = outdoorSpace->GetBBox();
}

//------------------------------------------------------------------------------
/**
    register an octree space in the spatial server to manage it
*/
void 
nSpatialServer::RegisterSpace(ncSpatialOctree* octreeSpace)
{
    n_assert2(octreeSpace, "miquelangel.rujula");

    // if there is already an octree space, release it
    this->ReleaseOctreeSpace();
    
    this->m_octreeSpace = octreeSpace->GetEntityObject();
    this->m_octreeSpace->AddRef();
}

//------------------------------------------------------------------------------
/**
    Release all the registered spaces. 
    
    It also turns off all the registered spatial cameras. If you use this 
    method, and after that you load a new scene and register new spaces, 
    remember to turn on the correponding cameras.
*/
void 
nSpatialServer::ReleaseSpaces()
{
    // release all the registered spaces
    this->ReleaseOctreeSpace();
    this->ReleaseIndoorSpaces();
    this->ReleaseOutdoorSpace();
    this->ReleaseGlobalSpace();

    // turn off all the cameras, because there are no spaces
    for (nArray<ncSpatialCamera*>::iterator pSpatialCamera  = this->m_spatialCameras.Begin();
                                            pSpatialCamera != this->m_spatialCameras.End();
                                            ++pSpatialCamera)
    {
        (*pSpatialCamera)->SetCell(0);
        (*pSpatialCamera)->TurnOff();
    }

    // all the entities have been destroyed
    this->m_totalNumEntities = 0;
}

//------------------------------------------------------------------------------
/**
    release the octree space
*/
void
nSpatialServer::ReleaseOctreeSpace()
{
    if (this->m_octreeSpace.isvalid())
    {
        ncSpatialOctree *octreeComp = this->m_octreeSpace->GetComponentSafe<ncSpatialOctree>();
        octreeComp->DestroySpace();
        this->m_octreeSpace->Release();
        this->m_octreeSpace.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    release all the indoor spaces
*/
void
nSpatialServer::ReleaseIndoorSpaces()
{
    for (nArray<ncSpatialIndoor*>::iterator indoorSpace  = this->m_indoorSpaces.Begin();
                                            indoorSpace != this->m_indoorSpaces.End();
                                            ++indoorSpace)
    {
        (*indoorSpace)->DestroySpace();
        (*indoorSpace)->GetEntityObject()->Release();
    }

    this->m_indoorSpaces.Reset();
}

//------------------------------------------------------------------------------
/**
    release an indoor space
*/
void
nSpatialServer::ReleaseIndoorSpace(ncSpatialIndoor *indoor)
{
    n_assert2(indoor, "miquelangel.rujula: NULL pointer to indoor space entity!");

    int index = this->m_indoorSpaces.FindIndex(indoor);
    n_assert2(index != -1, "miquelangel.rujula: trying to release an unregistered indoor space!");
    
    this->m_indoorSpaces.EraseQuick(index);
    indoor->DestroySpace();
    indoor->GetEntityObject()->Release();
}

//------------------------------------------------------------------------------
/**
    release the quadtree space
*/
void
nSpatialServer::ReleaseOutdoorSpace()
{
    if (this->m_outdoorSpace.isvalid())
    {
        ncSpatialQuadtree * spatialQuadtree = this->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>();

        int quadNumEntities = spatialQuadtree->GetNumEntities();
        this->m_totalNumEntities = this->m_totalNumEntities - quadNumEntities;
        spatialQuadtree->DestroySpace();
        this->m_outdoorSpace->Release();
        this->m_outdoorSpace.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    release the global space
*/
void
nSpatialServer::ReleaseGlobalSpace()
{
    if (this->m_globalSpace)
    {
        this->m_globalSpace->GetCellsArray().At(0)->DestroyCell();
    }
}

//------------------------------------------------------------------------------
/**
    remove an indoor space from the array (but it doesn't destroy it)
*/
void 
nSpatialServer::RemoveIndoorSpace(ncSpatialIndoor* indoor)
{
    int index = this->m_indoorSpaces.FindIndex(indoor);
    if ( index != -1 )
    {
        this->m_indoorSpaces.EraseQuick(index);
        indoor->GetEntityObject()->Release();
    }
}

//------------------------------------------------------------------------------
/**
    create a new camera entity and introduce it in its corresponding space and 
    cell if it's on
*/
nEntityObject *
nSpatialServer::CreateCamera(const nCamera2 &camera, const matrix44 &viewMatrix, bool on)
{
    // create the camera entity
    nEntityObject *newCamera = nEntityObjectServer::Instance()->NewLocalEntityObject("necamera");
    ncSpatialCamera * spatialCamera = newCamera->GetComponentSafe<ncSpatialCamera>();

    // transformation matrix in world coordinates
    spatialCamera->SetViewMatrix(viewMatrix);
    spatialCamera->SetCamera(camera);
    spatialCamera->SetNearPlaneDist(camera.GetNearPlane());

    if (on)
    {
        // turn on the camera
        spatialCamera->TurnOn();

        if (!this->m_octreeSpace.isvalid())
        {
            // if there's no octree space, then try to insert it in the other spaces
            if(!this->InsertEntity(newCamera))
            {
                //n_message("Error: Can't situate camera during its creation!!");
            }
        }
    }
    else
    {
        // turn off the camera
        spatialCamera->TurnOff();
    }

    // store this camera
    this->m_spatialCameras.Append(spatialCamera);

    return newCamera;
}

//------------------------------------------------------------------------------
/**
    destroy a camera
*/
bool  
nSpatialServer::DestroyCamera(ncSpatialCamera *camera)
{
    n_assert2(camera, "miquelangel.rujula");

    // search the camera
    int i = this->m_spatialCameras.FindIndex(camera);
    if ( i != -1 )
    {   
        camera->RemoveFromSpaces();
        nEntityObjectServer::Instance()->RemoveEntityObject(this->m_spatialCameras[i]->GetEntityObject());
        this->m_spatialCameras.EraseQuick(i);
        return true;
    }

    // the camera doesn't exist. Couldn't erase it.
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
nSpatialServer::DetermineVisibility(ncSpatialCamera* camera, nSceneGraph* sceneGraph)
{
    #ifdef __NEBULA_STATS__
    this->profVis.StartAccum();
    #endif

    // reset the camera
    camera->Reset();

    // set scene graph
    this->visibilityVisitor->SetSceneGraph(sceneGraph);

    if (!camera->IsOn())
    {
        #ifdef __NEBULA_STATS__
        this->profVis.StopAccum();
        #endif
        return;
    }

    if (this->m_octreeSpace.isvalid() && this->m_useOctrees)
    {
        this->visibilityVisitor->SetCamera(camera);
        this->visibilityVisitor->Visit(this->m_octreeSpace->GetComponentSafe<ncSpatialOctree>());
    }
    else
    {
        ncSpatialCell *cameraCell = camera->GetCell();
        camera->ResetVisibleLightsArray();

        // if the camera is not in any space, try to make at least outdoor visibility
        if (!cameraCell)
        {
            if (this->m_outdoorSpace.isvalid())
            {
                this->visibilityVisitor->SetCamera(camera);
                ncSpatialCell *rootCell = this->m_outdoorSpace->GetComponentSafe<ncSpatialQuadtree>()->GetRootCell();
                this->visibilityVisitor->nVisibleFrustumVisitor::Visit(rootCell);
            }
        }
        else
        {
            this->visibilityVisitor->nVisibleFrustumVisitor::Visit(camera);
        }
    }

    #ifdef __NEBULA_STATS__
    this->profVisGlobalEnts.StartAccum();
    #endif
#ifndef NGAME
    if (camera->GetAttachGlobalEntities())
    {
#endif // !NGAME
        this->m_globalSpace->ResetVisEntitiesArray();
        if ( this->visibilityVisitor->IsOutdoorVisible() )
        {
            // test the global entities
            static nArray<nEntityObject*> globalEntities;
            globalEntities.Reset();
            this->GetCommonGlobalEntities(globalEntities);

            this->visibilityVisitor->VisitGlobalEntities(globalEntities);
        }

        ncScene *sceneComp;
        const nArray<nEntityObject*> &alwaysVis = this->GetGlobalAlwaysVisibleEntities();
        for ( int i(0); i < alwaysVis.Size(); ++i )
        {
            if (!alwaysVis[i]->GetComponent<ncSpatialCamera>())
            {
                sceneComp = alwaysVis[i]->GetComponent<ncScene>();
                if (sceneComp)
                {
                    sceneComp->SetFlag(ncScene::ShapeVisible, true);
                    if (sceneGraph)
                    {
                        // HACK ma.garcias - check that the component has not been rendered this frame
                        if (sceneComp->GetFrameId() != (uint) this->GetFrameId())
                        {
                            sceneComp->SetTime(nTimeServer::Instance()->GetFrameTime());
                            sceneComp->SetFrameId(this->GetFrameId());
                            sceneComp->Render(sceneGraph);
                        }
                    }
                }

                if (camera->GetUseCameraCollector())
                {
                    camera->AppendVisibleEntity(alwaysVis[i]);
                }
            }
        }

#ifndef NGAME
    }
#endif // !NGAME

    #ifdef __NEBULA_STATS__
    this->profVisGlobalEnts.StopAccum();
    this->profVis.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
    remove the entity with the given id from all the spaces
*/
void 
nSpatialServer::EntityDeleted(int id)
{
    nEntityObject * obj = nEntityObjectServer::Instance()->GetEntityObject(id);
    n_assert2( obj, "EntityObject not found" );
    if( obj )
    {
        bool result = true;

        const nArray<nEntityObject*> &alwaysVis = this->GetGlobalAlwaysVisibleEntities();
        int index( alwaysVis.FindIndex(obj) );
        if (index != -1)
        {
            this->RemoveGlobalEntity( obj );
            return;
        }

        ncSpatial *spatialComp = obj->GetComponent<ncSpatial>();
        if (spatialComp)
        {
            if (spatialComp->IsBatched())
            {
                spatialComp->LeaveBatch();
            }

            result = spatialComp->RemoveFromSpaces();
        }

        n_assert2(result, "Can't remove entity from all the spaces!");
    }
}

//------------------------------------------------------------------------------
/**
    search the cell containing the point in all the spaces
*/
ncSpatialCell *
nSpatialServer::SearchCell(const vector3 &point) const
{
    n_assert(this->spatialVisitor);
    return this->spatialVisitor->SearchCell(point);
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    enable or disable quadtree cells painting
*/
void 
nSpatialServer::SetTerrainVisible(bool flag)
{
    this->visibilityVisitor->SetTerrainVisible(flag);
}

//------------------------------------------------------------------------------
/**
*/
bool
nSpatialServer::IsTerrainVisible()
{
    return this->visibilityVisitor->IsTerrainVisible();
}

#endif // !NGAME

//------------------------------------------------------------------------------
/**
    enable or disable horizon culling
*/
void 
nSpatialServer::SetDoHorizonCulling(bool flag)
{
    this->visibilityVisitor->SetDoHorizonCulling(flag);
}

//------------------------------------------------------------------------------
/**
    get if it's doing horizon culling or not
*/
bool 
nSpatialServer::IsDoingHorizonCulling() const
{
    return this->visibilityVisitor->IsDoingHorizonCulling();
}

//------------------------------------------------------------------------------
/**
    get the octree space
*/
ncSpatialOctree*
nSpatialServer::GetOctreeSpace()
{
    return this->m_octreeSpace.isvalid() ? this->m_octreeSpace.get()->GetComponentSafe<ncSpatialOctree>() : 0;
}

//------------------------------------------------------------------------------
/**
    get the outdoor space
*/
ncSpatialQuadtree* 
nSpatialServer::GetQuadtreeSpace()
{
    return this->m_outdoorSpace.isvalid() ? this->m_outdoorSpace.get()->GetComponentSafe<ncSpatialQuadtree>() : 0;
}

//------------------------------------------------------------------------------
/**
    get a space given its id
*/
ncSpatialSpace *
nSpatialServer::GetSpaceById(nEntityObjectId id) const
{
    if ( this->m_outdoorSpace.isvalid() && (this->m_outdoorSpace->GetId() == id) )
    {
        return this->m_outdoorSpace.get()->GetComponentSafe<ncSpatialSpace>();
    }
    else
    {
        for ( int i(0); i < this->m_indoorSpaces.Size(); ++i )
        {
            if ( this->m_indoorSpaces[i]->GetEntityObject()->GetId() == id )
            {
                return this->m_indoorSpaces[i];
            }
        }
    }

    return 0;
}
