#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialindoor.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialindoor.h"
#include "kernel/npersistserver.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatial.h"
#include "nspatial/nspatialindoorcell.h"
#include "nspatial/nspatialmodels.h"
#include "nspatial/ncspatialindoorclass.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialoccluder.h"
#include "entity/nentityobjectserver.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/nctransform.h"
#include "entity/nobjectinstancer.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif // !NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialIndoor, ncSpatialSpace);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialIndoor)
    NSCRIPT_ADDCMD_COMPOBJECT('R_DI', void, Disconnect, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RDIT', void, DisconnectTemporary, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RDFI', void, DisconnectFromIndoors, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('R_CI', void, Connect, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RCTO', void, ConnectToOutdoor, 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('R_RT', void, RemoveTemporary, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RRFT', void, RestoreFromTempArray, 0, (), 0, ());
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSpatialIndoor::ncSpatialIndoor():
m_facade(0),
m_transformFacade(true),
#ifndef NGAME
enabled(true),
#endif // !NGAME
m_occluders(0)
{
    this->m_spaceType = ncSpatialSpace::N_INDOOR_SPACE;
}

//------------------------------------------------------------------------------
/**
*/
ncSpatialIndoor::~ncSpatialIndoor()
{
    if (this->m_occluders)
    {
        this->m_occluders->Reset();
        n_delete(this->m_occluders);
        this->m_occluders = 0;
    }

    if (this->m_facade)
    {
#ifndef NGAME
        if ( !this->m_facade->IsInLimbo() )
        {
#endif // !NGAME
        n_verify2( nEntityObjectServer::Instance()->RemoveEntityObject(this->m_facade), 
            "miquelangel.rujula: can't destroy facade entity when destroying an indoor!");
#ifndef NGAME
        }
#endif // !NGAME
    }

#ifndef NGAME
    if (this->GetEntityObject()->IsInLimbo())
    {
#endif // !NGAME
        this->DestroySpace();
#ifndef NGAME
    }
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
    Initialize the entity instance
*/
void 
ncSpatialIndoor::InitInstance(nObject::InitInstanceMsg initType)
{
    n_assert2(!this->m_facade, "miquelangel.rujula: spatial indoor already has a facade!");
    ncSpatialIndoorClass *indoorClass = this->GetClassComponent<ncSpatialIndoorClass>();
    n_assert2(indoorClass, "miquelangel.rujula: indoor entity has no spatial indoor class component!");

    if (initType != nObject::ReloadedInstance)
    {
        nString facadeName = indoorClass->GetShellName();
        if (!facadeName.IsEmpty())
        {
            this->m_facade = nEntityObjectServer::Instance()->NewLocalEntityObject(facadeName.Get());
            n_assert2(this->m_facade, "miquelangel.rujula: can't create facade entity for the indoor!");
            ncSpatial *spatialFacade = this->m_facade->GetComponentSafe<ncSpatial>();
            spatialFacade->SetIndoorSpace(this);
            spatialFacade->SetTransformIndoor(false);
            this->TransformFacade(this->GetComponentSafe<ncTransform>()->GetTransform());
            spatialFacade->SetTransformIndoor(true);
            int flags(nSpatialTypes::SPF_ONE_CELL | 
                nSpatialTypes::SPF_CONTAINING |
                nSpatialTypes::SPF_USE_BBOX |
                nSpatialTypes::SPF_2D);
            n_verify(nSpatialServer::Instance()->InsertEntityOutdoors(this->m_facade, flags));
        }
    }

#ifndef NGAME
    if( initType != nObject::ReloadedInstance )
    {
        // Register to EnterLimbo signal
        this->entityObject->BindSignal( NSIGNAL_OBJECT(nEntityObject, EnterLimbo), this, &ncSpatialIndoor::DoEnterLimbo, 0 );
        // Register to ExitLimbo signal
        this->entityObject->BindSignal( NSIGNAL_OBJECT(nEntityObject, ExitLimbo), this, &ncSpatialIndoor::DoLeaveLimbo, 0 );
    }
#endif

    if (initType != nObject::ReloadedInstance)
    {
        // if this class has occluders, create and store them
        if (indoorClass->GetHasOccluders())
        {
            // create the occluders array
            this->m_occluders = n_new(nArray<nEntityObject*>(2, 2));

            // load the occluders in the instancer
            ncLoaderClass *loaderClass = indoorClass->GetComponentSafe<ncLoaderClass>();
            nString fileName = loaderClass->GetResourceFile();
            fileName.Append("/spatial/occluders.n2");
            nObjectInstancer *instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->Load(fileName.Get()));
            n_assert(instancer);
            if (instancer)
            {
                // get matrix information to also transform the occluders
                const matrix44 &mat = this->GetComponent<ncTransform>()->GetTransform();
                vector3 scale;
                quaternion quat;
                vector3 pos;
                mat.get(scale, quat, pos);
                ncTransform *trComp = 0;
                nEntityObject *occluder = 0;
                for (int i = 0; i < instancer->Size(); ++i)
                {
                    occluder = static_cast<nEntityObject*>(instancer->At(i));
                    // add the occluder to the array
                    this->m_occluders->Append(occluder);

                    // update the occluder's transform 
                    trComp = this->m_occluders->At(i)->GetComponent<ncTransform>();
                    trComp->DisableUpdate(ncTransform::cSpatial);
                    trComp->SetPosition(pos);
                    trComp->SetScale(scale);
                    trComp->EnableUpdate(ncTransform::cSpatial);
                    trComp->SetQuat(quat);
                }
                instancer->Release();
            }
        }
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    remove the space from the spatial server
*/
void
ncSpatialIndoor::DoEnterLimbo()
{
    nSpatialServer::Instance()->RemoveIndoorSpace(this);
    this->DisconnectTemporary();
    if (this->m_occluders)
    {
        for (int i = 0; i < this->m_occluders->Size(); ++i)
        {
            this->entityObject->SendToLimbo((*this->m_occluders)[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    register the space to the spatial server
*/
void
ncSpatialIndoor::DoLeaveLimbo()
{
    nSpatialServer::Instance()->RegisterSpace(this);
    this->Connect();
    if (this->m_occluders)
    {
        nEntityObject *occluder = 0;
        for (int i = 0; i < this->m_occluders->Size(); ++i)
        {
            occluder = (*this->m_occluders)[i];
            occluder->GetComponent<ncTransform>()->SetPosition(occluder->GetComponent<ncTransform>()->GetPosition());
        }
    }
}
#endif // !NGAME

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialIndoor::SaveCmds(nPersistServer *ps)
{
    if (ncSpatialSpace::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Destroys all the cells contained in the space and disconnects it.
    Call it just before deleting the space.
*/
void 
ncSpatialIndoor::DestroySpace()
{
    this->Disconnect();
    ncSpatialSpace::DestroySpace();
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell.
    Assuming that the entity's spatial component is in world coordinates (its position), 
    because it comes from outside.
*/
bool 
ncSpatialIndoor::AddEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to entity!");

    // get entity's spatial component
    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "miquelangel.rujula: entity hasn't spatial component!");
    
    nSpatialIndoorCell *containingCell = static_cast<nSpatialIndoorCell*>(this->SearchCellContaining(spatialComp->GetBBox().center(), 0));
    if ( containingCell )
    {
        return containingCell->AddEntity(entity);
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell and category.

    Assuming that the entity's spatial component is in space coordinates (its bounding box).
*/
bool 
ncSpatialIndoor::AddEntityByBBox(nEntityObject *entity, 
                                 const int flags)
{ 
    n_assert2(entity, "miquelangel.rujula: NULL pointer to entity!");

    // get entity's spatial component
    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "miquelangel.rujula: entity hasn't spatial component!");
    const bbox3 &entityBox = spatialComp->GetBBox();

    bool result = false;

    // insert the entity depending on the flags
    if (flags & nSpatialTypes::SPF_ONE_CELL)
    {
        // insert it in only one cell
        if (flags & nSpatialTypes::SPF_CONTAINING)
        {
            // search the cell that contains the element's bounding box
            nSpatialIndoorCell *containingCell = static_cast<nSpatialIndoorCell*>(this->SearchCellContaining(entityBox));
            if (containingCell)
            {
                containingCell->AddEntity(entity);
                result = true;
            }
            else
            {
                result = false;
            }
        }
        else if (flags & nSpatialTypes::SPF_INTERSECTING)
        {
            nArray<ncSpatialCell*> cellsArray;
            this->SearchCellsIntersecting(entityBox, &cellsArray);
            for (nArray<ncSpatialCell*>::iterator pCell  = cellsArray.Begin();
                                                  pCell != cellsArray.End();
                                                  ++pCell)
            {
                if((*pCell)->AddEntity(entity))
                {
                    // the entity has been inserted in the first cell intersecting 
                    // the entity's bounding box
                    result = true;
                    break;
                }
            }
        }
        else
        {
            n_message("Error: incorrect spatial insertion flags. Don't know how to insert the entity!");
            return false;
        }

    }
    else if (flags & nSpatialTypes::SPF_ALL_CELLS)
    {
        // insert it in all the cells
        if (flags & nSpatialTypes::SPF_CONTAINING)
        {
            // insert it in all the cells containing the bounding box
            nArray<ncSpatialCell*> cellsArray;
            this->SearchCellsContaining(entityBox, &cellsArray);
            for (nArray<ncSpatialCell*>::iterator pCell  = cellsArray.Begin();
                                                  pCell != cellsArray.End();
                                                  ++pCell)
            {
                if((*pCell)->AddEntity(entity))
                {
                    result = true;
                }
            }
        }
        else if (flags & nSpatialTypes::SPF_INTERSECTING)
        {
            // insert it in all the cells intersecting the bounding box
            nArray<ncSpatialCell*> cellsArray;
            this->SearchCellsIntersecting(entityBox, &cellsArray);
            for (nArray<ncSpatialCell*>::iterator pCell  = cellsArray.Begin();
                                                  pCell != cellsArray.End();
                                                  ++pCell)
            {
                if((*pCell)->AddEntity(entity))
                {
                    result = true;
                }
            }
        }
        else
        {
            n_message("Error: incorrect spatial insertion flags. Don't know how to insert the entity!");
            return false;
        }
    }

    
    return result;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell and category.

    Assuming that the entity is in space coordinates (its model).
*/
bool 
ncSpatialIndoor::AddEntityByModel(nEntityObject *entity,
                                      const int flags)
{ 
    n_assert2(entity, "miquelangel.rujula: NULL pointer to an entity!");

    // get the entity's spatial component
    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "miquelangel.rujula: entity hasn't spatial component!");

    nSpatialModel *spatialCompModel = spatialComp->GetTestModel();

    if (!spatialCompModel)
    {
        // the spatial component has no model
        return false;
    }

    bool result = false;

    // by now, we'll only use the sphere model
    n_assert2(spatialCompModel && (spatialCompModel->GetType() != nSpatialModel::SPATIAL_MODEL_SPHERE), 
              "miquelangel.rujula: wrong spatial model while inserting entity in an indoor space.");
    

    nSpatialSphereModel *sphereModel = static_cast<nSpatialSphereModel*>(spatialCompModel);
    sphere sph = sphereModel->GetSphere();

    // insert the entity depending on the flags
    if (flags & nSpatialTypes::SPF_ONE_CELL)
    {
        // insert it in only one cell
        if (flags & nSpatialTypes::SPF_CONTAINING)
        {
            // insert it in the cell that contains the sphere
            nSpatialIndoorCell *containingCell = static_cast<nSpatialIndoorCell*>(this->SearchCellContaining(sphereModel));
            if (containingCell)
            {
                n_verify(containingCell->ncSpatialCell::AddEntity(entity));
                result = true;
            }
            else
            {
                result = false;
            }
        }
        else if (flags & nSpatialTypes::SPF_INTERSECTING)
        {
            nArray<ncSpatialCell*> cellsArray;
            this->SearchCellsIntersecting(sph, &cellsArray);
            for (nArray<ncSpatialCell*>::iterator pCell  = cellsArray.Begin();
                                                 pCell != cellsArray.End();
                                                 ++pCell)
            {
                if((*pCell)->AddEntity(entity))
                {
                    // the element has been inserted in the first cell intersecting 
                    // the element's bounding box
                    result = true;
                    break;
                }
            }
        }
        else
        {
            n_message("Error: incorrect spatial insertion flags. Don't know how to insert the entity.");
            return false;
        }

    }
    else if (flags & nSpatialTypes::SPF_ALL_CELLS)
    {
        // insert it in all the cells
        if (flags & nSpatialTypes::SPF_CONTAINING)
        {
            // insert it in all the cells containing the bounding box
            nArray<ncSpatialCell*> cellsArray;
            this->SearchCellsContaining(sph, &cellsArray);
            for (nArray<ncSpatialCell*>::iterator pCell  = cellsArray.Begin();
                                                  pCell != cellsArray.End();
                                                  ++pCell)
            {
                if((*pCell)->AddEntity(entity))
                {
                    result = true;
                }
            }
        }
        else if (flags & nSpatialTypes::SPF_INTERSECTING)
        {
            // insert it in all the cells intersecting the bounding box
            nArray<ncSpatialCell*> cellsArray;
            this->SearchCellsIntersecting(sph, &cellsArray);
            for (nArray<ncSpatialCell*>::iterator pCell  = cellsArray.Begin();
                                                  pCell != cellsArray.End();
                                                  ++pCell)
            {
                if((*pCell)->AddEntity(entity))
                {
                    result = true;
                }
            }
        }
        else
        {
            n_message("Error: incorrect spatial insertion flags. Don't know how to insert the entity.");
            return false;
        }
    }


    return result;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell if the
    SPF_ONE_CELL flag is set, or in all the cells whose bounding boxes contain
    the center of the entity's bounding box if SPF_ALL_CELLS is set. 
*/
bool 
ncSpatialIndoor::AddEntityByPos(nEntityObject *entity,
                                const int flags)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to an entity!");
    n_assert2((flags & nSpatialTypes::SPF_USE_POSITION), "Wrong spatial insertion flag!");

    // get the entity's spatial component
    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "miquelangel.rujula: entity hasn't spatial component!");

    const vector3 &pos = spatialComp->GetBBox().center();
    bool result = false;

    if (flags & nSpatialTypes::SPF_ONE_CELL)
    {
        // search the cell that contains the spatial component's position
        nSpatialIndoorCell *containingCell = static_cast<nSpatialIndoorCell*>(this->SearchCellContaining(pos, flags));
        if (containingCell)
        {
            n_verify(containingCell->ncSpatialCell::AddEntity(entity));
            result = true;
        }
        else
        {
            result = false;
        }
    }
    else if (flags & nSpatialTypes::SPF_ALL_CELLS)
    {
        if ((flags & nSpatialTypes::SPF_CONTAINING) ||
            (flags & nSpatialTypes::SPF_INTERSECTING))
        {
            // insert it in all the cells containing the entity's position in space coordinates
            nArray<ncSpatialCell*> cellsArray;
            this->SearchCellsContaining(pos, &cellsArray, flags);
            for (nArray<ncSpatialCell*>::iterator pCell  = cellsArray.Begin();
                                                  pCell != cellsArray.End();
                                                  ++pCell)
            {
                if((*pCell)->AddEntity(entity))
                {
                    result = true;
                }
            }
        }
        else 
        {
            for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                                  pCell != this->m_cellsArray.End();
                                                  ++pCell)
            {
                if((*pCell)->AddEntity(entity))
                {
                    result = true;
                }
            }
        }
    }

    return result;
}

//------------------------------------------------------------------------------
/**
    Searches the cell that contains the given point.
    Point has to be in space coordinates.
    Returns NULL if no cell contains it.
*/
ncSpatialCell *
ncSpatialIndoor::SearchCellContaining(const vector3 &point, const int flags) const
{
    if (!this->m_bbox.contains(point))
    {
        // the point is surely outside this space
        return NULL;
    }

    // this array contains all the cells that can contain the point
    nArray<nSpatialIndoorCell*> possibleCells;
    for (nArray<ncSpatialCell*>::iterator cell  = this->m_cellsArray.Begin();
                                          cell != this->m_cellsArray.End();
                                        ++cell)
    {
        nSpatialIndoorCell *indoorCell = static_cast<nSpatialIndoorCell*>((*cell));
                
        if (indoorCell->GetBBox().contains(point))
        {
            possibleCells.Append(indoorCell);
        }
    }

    if (possibleCells.Empty())
    {
        // no cell in the space contains the element
        return 0;
    }

    if ( flags & nSpatialTypes::SPF_USE_CELL_BBOX )
    {
        return possibleCells[0];
    }
    else
    {
        // use cells' wrapper mesh to determine which one contains it
        for (nArray<nSpatialIndoorCell*>::iterator cell  = possibleCells.Begin();
                                                   cell != possibleCells.End();
                                                 ++cell)
        {
            if ((*cell)->Contains(point))
            {
                // we've found the cell that contains the point
                return (*cell);
            }
        }
    }

    // no cell contains the point
    return 0;
}

//------------------------------------------------------------------------------
/**
    Searches the cell that contains the given bounding box.
    Returns NULL if no cell contains it.

    NOTE: it only tests if the bounding box of any cell completely contains the
    given bounding box, and returns the first one that does it. It doesn't 
    determine if the cell's wrapper mesh completely contains it.
*/
ncSpatialCell *
ncSpatialIndoor::SearchCellContaining(const bbox3 &box) const
{
    if (!this->m_bbox.contains(box))
    {
        // the box isn't completely contained in the space's box, and then
        // there isn't a cell in this space doing it
        return NULL;
    }

    for (nArray<ncSpatialCell*>::iterator cell  = this->m_cellsArray.Begin();
                                          cell != this->m_cellsArray.End();
                                          ++cell)
    {
        nSpatialIndoorCell *indoorCell = static_cast<nSpatialIndoorCell*>((*cell));
        
        if (indoorCell->Contains(box))
        {
            return indoorCell;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
    search the inner cell in the tree that completely contains the given sphere
*/
ncSpatialCell *
ncSpatialIndoor::SearchCellContaining(const sphere &sph) const
{
    if (!sph.isContainedIn(this->m_bbox))
    {
        // the box isn't completely contained in the space's box, and then
        // there isn't a cell in this space doing it
        return NULL;
    }

    for (nArray<ncSpatialCell*>::iterator cell  = this->m_cellsArray.Begin();
                                          cell != this->m_cellsArray.End();
                                          ++cell)
    {
        nSpatialIndoorCell *indoorCell = static_cast<nSpatialIndoorCell*>((*cell));
        
        if (indoorCell->Contains(sph))
        {
            return indoorCell;
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
    Searches the cell that contains the given model.
    Returns NULL if no cell contains it.

    NOTE: it only tests if the bounding box of any cell completely contains the
    given model, and returns the first one that does it. It doesn't 
    determine if the cell's wrapper mesh completely contains it.

    NOTE2: by now, it only checks sphere models.
*/
ncSpatialCell *
ncSpatialIndoor::SearchCellContaining(const nSpatialModel *model) const
{
    n_assert2((model->GetType() == nSpatialModel::SPATIAL_MODEL_SPHERE), 
              "Error: only sphere spatial models are supported!");

    nSpatialSphereModel *sphereModel = (nSpatialSphereModel*)model;
    sphere sph = sphereModel->GetSphere();

    return this->SearchCellContaining(sph);
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    bounding box, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialIndoor::SearchCellsIntersecting(const bbox3 &box, nArray<ncSpatialCell*> *cells) const
{
    n_assert2(cells, "Error: null pointer to cells array.");

    // First check if the parameter bounding box 'touches' this space.
    if (!this->m_bbox.intersects(box))
    {
        return;
    }
    
    // this array contains all the cells that intersect the bounding box
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        if ((*pCell)->GetBBox().intersects(box))
        {
            cells->Append((*pCell));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    sphere, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialIndoor::SearchCellsIntersecting(const sphere &sph, nArray<ncSpatialCell*> *cells) const
{
    n_assert2(cells, "Error: null pointer to cells array.");

    // First check if the parameter sphere 'touches' this space.
    if (!sph.intersects(this->m_bbox))
    {
        return;
    }

    // this array contains all the cells that intersect the bounding box
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        if (sph.intersects((*pCell)->GetBBox()))
        {
            cells->Append((*pCell));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches all the entities whose bounding box intersects with the 
    given bounding box, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialIndoor::SearchEntitiesIntersecting(const bbox3 &box, nArray<nEntityObject*> *entities) const
{
    n_assert2(entities, "miquelangel.rujula: NULL pointer to entities array!");    

    // get all the cells in this space that intersect the box
    nArray<ncSpatialCell*> intersectingCells;
    this->SearchCellsIntersecting(box, &intersectingCells);

    if (intersectingCells.Size() > 0)
    {
        ncSpatial *spatialComp = 0;
        const nArray<nEntityObject*> *categories = 0;
        // append the intersecting entities of each cell
        for (nArray<ncSpatialCell*>::iterator pCell  = intersectingCells.Begin();
                                              pCell != intersectingCells.End();
                                              ++pCell)
        {
            categories = (*pCell)->GetCategories();
            for ( int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
            {
                if ( catIndex == nSpatialTypes::CAT_LIGHTS )
                {
                    // append intersecting lights
                    const nArray<nEntityObject*> &lights = categories[nSpatialTypes::CAT_LIGHTS];
                    for (int i = 0; i < lights.Size(); ++i)
                    {
                        if (lights[i]->GetComponentSafe<ncSpatialLight>()->GetTestModel()->Intersects(box))
                        {
                            entities->Append(lights[i]);
                        }
                    }
                }
                else
                {
                    // append intersecting entities
                    const nArray<nEntityObject*> &category = categories[catIndex];
                    nEntityObject *entity = 0;
                    for (int i = 0; i < category.Size(); ++i)
                    {
                        entity = category[i];
                        spatialComp = entity->GetComponent<ncSpatial>();
                        n_assert2(spatialComp, "miquelangel.rujula: entity has to have spatial component!");

                        if (spatialComp->GetBBox().intersects(box))
                        {
                            entities->Append(entity);
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches all the entities whose bounding box intersects with the 
    given sphere, and fills the parameter array with them.
    'sph' has to be in world coordinates.
    It doesn't reset the parameter array.
*/
void 
ncSpatialIndoor::SearchEntitiesIntersecting(const sphere &sph, nArray<nEntityObject*> *entities) const
{
    n_assert2(entities, "miquelangel.rujula: NULL pointer to entities array!");    

    // get all the cells in this space that intersect the sph
    nArray<ncSpatialCell*> intersectingCells;
    this->SearchCellsIntersecting(sph, &intersectingCells);

    if (intersectingCells.Size() > 0)
    {
        ncSpatial *spatialComp = 0;
        const nArray<nEntityObject*> *categories = 0;
        // append the intersecting entities of each cell
        for (nArray<ncSpatialCell*>::iterator pCell  = intersectingCells.Begin();
                                              pCell != intersectingCells.End();
                                              ++pCell)
        {
            categories = (*pCell)->GetCategories();
            for ( int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
            {
                if ( catIndex == nSpatialTypes::CAT_LIGHTS )
                {
                    // append intersecting lights
                    const nArray<nEntityObject*> &lights = categories[nSpatialTypes::CAT_LIGHTS];
                    for (int i = 0; i < lights.Size(); ++i)
                    {
                        if (lights[i]->GetComponentSafe<ncSpatialLight>()->GetTestModel()->Intersects(sph))
                        {
                            entities->Append(lights[i]);
                        }
                    }
                }
                else
                {
                    // append intersecting entities
                    const nArray<nEntityObject*> &category = categories[catIndex];
                    nEntityObject *entity = 0;
                    for (int i = 0; i < category.Size(); ++i)
                    {
                        entity = category[i];
                        spatialComp = entity->GetComponent<ncSpatial>();
                        n_assert2(spatialComp, "miquelangel.rujula: entity has to have spatial component!");

                        if (sph.intersects(spatialComp->GetBBox()))
                        {
                            entities->Append(entity);
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches all the entities whose position is contained into the given sphere
*/
void 
ncSpatialIndoor::SearchEntitiesContainedByPos(const sphere &sph, nArray<nEntityObject*> *entities) const
{
    n_assert2(entities, "miquelangel.rujula: NULL pointer to entities array.");

    // First check if the parameter sphere 'touches' this space.
    if (!sph.intersects(this->m_bbox))
    {
        return;
    }

    // search the cells that can have the contained entities
    nArray<ncSpatialCell*> intersectingCells;
    this->SearchCellsIntersecting(sph, &intersectingCells);

    if (intersectingCells.Size() > 0)
    {
        // append the contained entities of each cell
        const nArray<nEntityObject*> *categories = 0;
        for (nArray<ncSpatialCell*>::iterator pCell  = intersectingCells.Begin();
                                              pCell != intersectingCells.End();
                                              ++pCell)
        {
            categories = (*pCell)->GetCategories();
            ncTransform *trComp = 0;
            for ( int catIndex(0); catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
            {
                if ( catIndex == nSpatialTypes::CAT_NAVMESHNODES )
                {
                    continue;
                }

                // append contained entities
                const nArray<nEntityObject*> &category = categories[catIndex];
                nEntityObject *entity = 0;

                for (int i(0); i < category.Size(); ++i)
                {
                    entity = category[i];
                    trComp = entity->GetComponent<ncTransform>();
                    n_assert2(trComp, 
                                "miquelangel.rujula: there's an entity in the spatial system without transform component!");
                    if (sph.contains(trComp->GetPosition()))
                    {
                        entities->Append(entity);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches the cells that contain the given point.
*/
void
ncSpatialIndoor::SearchCellsContaining(const vector3 &point, 
                                       nArray<ncSpatialCell*> *cells, 
                                       const int flags) const
{
    n_assert2(cells, "miquelangel.rujula: NULL pointer to spatial cells array!");

    if (!this->m_bbox.contains(point))
    {
        // the point is surely outside this space
        return;
    }

    nSpatialIndoorCell *indoorCell;
    // append all the cells that contain the point
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                        ++pCell)
    {
        indoorCell = static_cast<nSpatialIndoorCell*>((*pCell));
        if ( indoorCell->GetBBox().contains(point) )
        {
            if ( !(flags & nSpatialTypes::SPF_USE_CELL_BBOX) && !indoorCell->Contains(point) )
            {
                continue;
            }

            cells->Append((*pCell));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches the cells whose bounding boxes completely contain the given bounding box
*/
void 
ncSpatialIndoor::SearchCellsContaining(const bbox3 &box, nArray<ncSpatialCell*> *cells) const
{
    n_assert2(cells, "Error: null pointer to cells array.");

    // First check if the parameter bounding box 'touches' this space.
    if (!this->m_bbox.intersects(box))
    {
        return;
    }
    
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        if ((*pCell)->Contains(box))
        {
            cells->Append((*pCell));
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    sphere, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialIndoor::SearchCellsContaining(const sphere &sph, nArray<ncSpatialCell*> *cells) const
{
    n_assert2(cells, "Error: null pointer to cells array.");

    // First check if the parameter sphere 'touches' this space.
    // Remember that space's bounding box is already in world coordinates
    if (!sph.intersects(this->m_bbox))
    {
        return;
    }

    // this array contains all the cells that intersect the sphere
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        if (sph.isContainedIn((*pCell)->GetBBox()))
        {
            cells->Append((*pCell));
        }
    }
}

//------------------------------------------------------------------------------
/**
    get the all the indoor brushes into this indoor
*/
void 
ncSpatialIndoor::GetIndoorBrushes(nArray<nEntityObject*> &indoorBrushes) const
{
    nSpatialIndoorCell *indoorCell = 0;
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        indoorCell = static_cast<nSpatialIndoorCell*>(*pCell);
        indoorCell->GetIndoorBrushes(indoorBrushes);
    }
}

//------------------------------------------------------------------------------
/**
    get the all the portals into this indoor
*/
void 
ncSpatialIndoor::GetPortals(nArray<ncSpatialPortal*> &portals) const
{
    nSpatialIndoorCell *indoorCell = 0;
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        indoorCell = static_cast<nSpatialIndoorCell*>(*pCell);
        indoorCell->GetPortals(portals);
    }
}

//------------------------------------------------------------------------------
/**
    Get the entities that are not indoor brushes nor portals. It fills the given
    array with the entities. It doesn't reset the array.
*/
void 
ncSpatialIndoor::GetDynamicEntities(nArray<nEntityObject*> &entitiesArray) const
{
    nSpatialIndoorCell *indoorCell = 0;
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        indoorCell = static_cast<nSpatialIndoorCell*>(*pCell);
        indoorCell->GetDynamicEntities(entitiesArray);
    }
}

//------------------------------------------------------------------------------
/**
    get the portals that point to another indoor space
*/
void
ncSpatialIndoor::GetOtherIndoorsPortals(nArray<ncSpatialPortal*> &otherIndoorPortals) const
{
    ncSpatialSpace *otherSideSpace = NULL;
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        const nArray<nEntityObject*> &portals = (*pCell)->GetCategories()[nSpatialTypes::CAT_PORTALS];
        ncSpatialPortal* portal = 0;
        for (int i = 0; i < portals.Size(); ++i)
        {
            portal = portals[i]->GetComponentSafe<ncSpatialPortal>();
            if ( portal->GetOtherSideCell() )
            {
                otherSideSpace = portal->GetOtherSideCell()->GetParentSpace();
                if ( otherSideSpace && 
                    (otherSideSpace != this) && 
                    (otherSideSpace->GetType() == ncSpatialSpace::N_INDOOR_SPACE) )
                {
                    otherIndoorPortals.Append(portal);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    get the portals that point to the outdoor
*/
void
ncSpatialIndoor::GetOutdoorPortals(nArray<ncSpatialPortal*> &outdoorPortals) const
{
    ncSpatialSpace *otherSideSpace = NULL;
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        const nArray<nEntityObject*> &portals = (*pCell)->GetCategories()[nSpatialTypes::CAT_PORTALS];
        ncSpatialPortal* portal = 0;
        for (int i = 0; i < portals.Size(); ++i)
        {
            portal = portals[i]->GetComponentSafe<ncSpatialPortal>();
            if (portal->GetOtherSideCell())
            {
                otherSideSpace = portal->GetOtherSideCell()->GetParentSpace();
                if ( otherSideSpace && (otherSideSpace->GetType() == ncSpatialSpace::N_QUADTREE_SPACE))
                {
                    outdoorPortals.Append(portal);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    get the portals that point to the global space
*/
void
ncSpatialIndoor::GetGlobalPortals(nArray<ncSpatialPortal*> &globalPortals) const
{
    ncSpatialSpace *otherSideSpace = NULL;
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        const nArray<nEntityObject*> &portals = (*pCell)->GetCategories()[nSpatialTypes::CAT_PORTALS];
        ncSpatialPortal* portal = 0;
        for (int i = 0; i < portals.Size(); ++i)
        {
            portal = portals[i]->GetComponentSafe<ncSpatialPortal>();
            if (portal->GetOtherSideCell())
            {
                otherSideSpace = portal->GetOtherSideCell()->GetParentSpace();
                if ( otherSideSpace && (otherSideSpace->GetType() == ncSpatialSpace::N_GLOBAL_SPACE))
                {
                    globalPortals.Append(portal);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    get the portals that are disconnected
*/
void 
ncSpatialIndoor::GetDisconnectedPortals(nArray<ncSpatialPortal*> &disconnectedPortals) const
{
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          ++pCell)
    {
        const nArray<nEntityObject*> &portals = (*pCell)->GetCategories()[nSpatialTypes::CAT_PORTALS];
        ncSpatialPortal* portal = 0;
        for (int i = 0; i < portals.Size(); ++i)
        {
            portal = portals[i]->GetComponentSafe<ncSpatialPortal>();
            if ((portal->GetOtherSideCellId() == 0) && !portal->GetTwinPortal())
            {
                disconnectedPortals.Append(portal);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    disconnect the indoor space temporary, this is, of the outdoors and 
    the other indoors and mantein the other indoors connected
*/
void 
ncSpatialIndoor::DisconnectTemporary()
{
    // disconnect the portals that point to the outdoors and destroy their twins
    nArray<ncSpatialPortal*> outdoorPortals;
    this->GetOutdoorPortals(outdoorPortals);
    bool result = false;
    for (nArray<ncSpatialPortal*>::iterator pPortal  = outdoorPortals.Begin();
                                            pPortal != outdoorPortals.End();
                                            ++pPortal)
    {
        result = (*pPortal)->GetOtherSideCell()->RemoveEntity((*pPortal)->GetTwinPortal()->GetEntityObject());
        n_assert2(result, 
                  "miquelangel.rujula: can't disconnect indoor, because I can't remove an outdoor portal!");
        nEntityObjectServer::Instance()->RemoveEntityObject((*pPortal)->GetTwinPortal()->GetEntityObject());
        (*pPortal)->SetTwinPortal(0);
    }

    // disconnect the portals that point to the global space and destroy their twins
    nArray<ncSpatialPortal*> globalPortals;
    this->GetGlobalPortals(globalPortals);
    for (nArray<ncSpatialPortal*>::iterator pPortal  = globalPortals.Begin();
                                            pPortal != globalPortals.End();
                                            ++pPortal)
    {
        result = (*pPortal)->GetOtherSideCell()->RemoveEntity((*pPortal)->GetTwinPortal()->GetEntityObject());
        n_assert2(result, 
                  "miquelangel.rujula: can't disconnect indoor, because I can't remove a global portal!");
        nEntityObjectServer::Instance()->RemoveEntityObject((*pPortal)->GetTwinPortal()->GetEntityObject());
        (*pPortal)->SetTwinPortal(0);
    }

    // disconnect the portals that point to other indoors, but don't destroy their twins
    nArray<ncSpatialPortal*> otherIndoorsPortals;
    nArray<ncSpatialSpace*> otherIndoors;
    ncSpatialCell *twinCell;
    int index = -1;
    this->GetOtherIndoorsPortals(otherIndoorsPortals);
    for (nArray<ncSpatialPortal*>::iterator pPortal  = otherIndoorsPortals.Begin();
                                            pPortal != otherIndoorsPortals.End();
                                            ++pPortal)
    {
        twinCell = (*pPortal)->GetTwinPortal()->GetCell();
        if ( twinCell )
        {
            index = otherIndoors.FindIndex(twinCell->GetParentSpace());
            if ( index == -1 )
            {
                // append the other indoor
                otherIndoors.Append(twinCell->GetParentSpace());
            }
        }

        (*pPortal)->GetTwinPortal()->SetTwinPortal(0);
        (*pPortal)->SetTwinPortal(0);
    }

    int i;
    for ( i = 0; i < otherIndoors.Size(); ++i )
    {
        static_cast<ncSpatialIndoor*>(otherIndoors[i])->Connect();
    }
}

//------------------------------------------------------------------------------
/**
    disconnect the indoor space completely, this is, of the outdoors and 
    the other indoors
*/
void 
ncSpatialIndoor::Disconnect()
{
    // disconnect the portals that point to the outdoors and destroy their twins
    nArray<ncSpatialPortal*> outdoorPortals;
    this->GetOutdoorPortals(outdoorPortals);
    bool result = false;
    for (nArray<ncSpatialPortal*>::iterator pPortal  = outdoorPortals.Begin();
                                            pPortal != outdoorPortals.End();
                                            ++pPortal)
    {
        result = (*pPortal)->GetOtherSideCell()->RemoveEntity((*pPortal)->GetTwinPortal()->GetEntityObject());
        n_assert2(result, 
                  "miquelangel.rujula: can't disconnect indoor, because I can't remove an outdoor portal!");
        nEntityObjectServer::Instance()->RemoveEntityObject((*pPortal)->GetTwinPortal()->GetEntityObject());
        (*pPortal)->SetTwinPortal(0);
    }

    // disconnect the portals that point to other indoors, but don't destroy their twins
    nArray<ncSpatialPortal*> otherIndoorsPortals;
    this->GetOtherIndoorsPortals(otherIndoorsPortals);
    for (nArray<ncSpatialPortal*>::iterator pPortal  = otherIndoorsPortals.Begin();
                                            pPortal != otherIndoorsPortals.End();
                                            ++pPortal)
    {
        (*pPortal)->GetTwinPortal()->SetTwinPortal(0);
        (*pPortal)->SetTwinPortal(0);
    }
}

//------------------------------------------------------------------------------
/**
    disconnect the indoor space of the other indoors
*/
void 
ncSpatialIndoor::DisconnectFromIndoors()
{
    // disconnect the portals that point to other indoors, but don't destroy their twins
    nArray<ncSpatialPortal*> otherIndoorsPortals;
    this->GetOtherIndoorsPortals(otherIndoorsPortals);
    for (nArray<ncSpatialPortal*>::iterator pPortal  = otherIndoorsPortals.Begin();
                                            pPortal != otherIndoorsPortals.End();
                                            pPortal++)
    {
        (*pPortal)->GetTwinPortal()->SetTwinPortal(0);
        (*pPortal)->SetTwinPortal(0);
    }

    // connect the indoor to the outdoors
    this->ConnectToOutdoor();
}

//------------------------------------------------------------------------------
/**
    connect the indoor space to other spaces (indoors and outdoor)
*/
void
ncSpatialIndoor::Connect()
{
    this->ConnectToOutdoor();
    nSpatialServer::Instance()->ConnectIndoors();
}

//------------------------------------------------------------------------------
/**
    connect the indoor space to the outdoors
*/
void 
ncSpatialIndoor::ConnectToOutdoor()
{
    nSpatialServer::Instance()->ConnectSpace(this);
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    remove temporary
*/
void 
ncSpatialIndoor::RemoveTemporary()
{
    nSpatialIndoorCell *indoorCell = 0;
    for (int i = 0; i < this->m_cellsArray.Size(); ++i)
    {
        indoorCell = static_cast<nSpatialIndoorCell*>(this->m_cellsArray[i]);
        indoorCell->RemoveTemporary();
    }
    this->SetEnable(false);
}

//------------------------------------------------------------------------------
/**
    restore this indoor's entities from the temporal array
*/
void 
ncSpatialIndoor::RestoreFromTempArray()
{
    nSpatialIndoorCell *indoorCell = 0;
    for (int i = 0; i < this->m_cellsArray.Size(); ++i)
    {
        indoorCell = static_cast<nSpatialIndoorCell*>(this->m_cellsArray[i]);
        indoorCell->RestoreTempArray();
    }
    this->SetEnable(true);
}
#endif // !NGAME

//------------------------------------------------------------------------------
/**
    update space
*/
void 
ncSpatialIndoor::Update(const matrix44 &matrix)
{
    ncSpatialSpace::Update(matrix);

    // transform indoor's facade
    if (this->m_transformFacade)
    {
        this->TransformFacade(matrix);
    }

    // if this entity has occluders, move them too
    if (this->m_occluders)
    {
        // get matrix information to also transform the occluders
        vector3 scale;
        quaternion quat;
        vector3 pos;
        matrix.get(scale, quat, pos);

        ncTransform *trComp = 0;
        for (int i = 0; i < this->m_occluders->Size(); i++)
        {
            trComp = this->m_occluders->At(i)->GetComponent<ncTransform>();
            trComp->DisableUpdate(ncTransform::cSpatial);
            trComp->SetPosition(pos);
            trComp->SetScale(scale);
            trComp->EnableUpdate(ncTransform::cSpatial);
            trComp->SetQuat(quat);
        }
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    set the layer id to all the entities into this space
*/
void 
ncSpatialIndoor::SetLayerId(int layerId)
{
    ncSpatialSpace::SetLayerId(layerId);

    if (this->m_facade)
    {
        this->m_facade->GetComponentSafe<ncEditor>()->SetLayerId(layerId);
    }
}
#endif // !NGAME

//------------------------------------------------------------------------------
/**
*/
bool 
ncSpatialIndoor::ReadCellsStaticInfo(const TiXmlElement *spaceElem)
{
    n_assert2(spaceElem, "miquelangel.rujula");

    // cells
    TiXmlElement *cellElem = spaceElem->FirstChildElement("Cell");

    while(cellElem)
    {
        nSpatialIndoorCell *newCell = n_new(nSpatialIndoorCell);
        newCell->SetParentSpace(this);
        if (!newCell->ReadStaticInfo(cellElem))
        {
            return false;
        }

        // add the new cell to the space
        this->AddSpatialCell(newCell);

        // get next cell
        cellElem = cellElem->NextSiblingElement("Cell");
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    transform indoor's facade
*/
void 
ncSpatialIndoor::TransformFacade(const matrix44 &matrix)
{
    if (!this->m_facade || !this->m_transformFacade)
    {
        return;
    }

    this->m_facade->GetComponentSafe<ncSpatial>()->SetTransformIndoor(false);

    // get the increments
    ncTransform *trComp = this->m_facade->GetComponent<ncTransform>();
    n_assert2(trComp, "miquelangel.rujula: indoor's facade has no transform component!");

    vector3 incPos(matrix.pos_component() - trComp->GetPosition());
    quaternion incQuat = matrix.get_quaternion() - trComp->GetQuat();

    trComp->DisableUpdate(ncTransform::cSpatial);
    trComp->SetPosition(trComp->GetPosition() + incPos);
    trComp->EnableUpdate(ncTransform::cSpatial);
    quaternion newQuat = trComp->GetTransform().get_quaternion() + incQuat;
    newQuat.normalize();
    trComp->SetQuat(newQuat);

    this->m_facade->GetComponentSafe<ncSpatial>()->SetTransformIndoor(true);
}
