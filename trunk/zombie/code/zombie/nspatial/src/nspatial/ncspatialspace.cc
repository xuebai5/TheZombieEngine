#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialspace.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialspace.h"
#include "kernel/npersistserver.h"
#include "kernel/ndirectory.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialindoorclass.h"
#include "entity/nentityobjectserver.h"
#include "zombieentity/nctransform.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialSpace,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialSpace)
    NSCRIPT_ADDCMD_COMPOBJECT('MSCC', void, SetCellId, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RAEI', void, AddEntityId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSWZ', void, SetWizard, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSBX', void, SetBBox, 6, (float, float, float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGBX', void, GetBBox, 0, (), 2, (vector3&, vector3&));
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSpatialSpace::ncSpatialSpace():
m_spaceType(N_SPATIAL_SPACE),
m_spatialPath(0),
currentCell(0),
isLoaded(false)
{
    this->m_originalBBox.begin_extend();
}

//------------------------------------------------------------------------------
/**
*/
ncSpatialSpace::~ncSpatialSpace()
{
    n_assert2(this->m_cellsArray.Size() == 0, "miquelangel.rujula");
}

//------------------------------------------------------------------------------
/**
    flush all the lights in the space
*/
void
ncSpatialSpace::FlushAllLights()
{
    for ( int i(0); i < this->m_cellsArray.Size(); ++i )
    {
        this->m_cellsArray[i]->FlushAllLights();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialSpace::SaveCmds(nPersistServer *ps)
{
    if (nComponentObject::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Destroys all the cells contained in the space.
    Call it just before deleting the space.
*/
void 
ncSpatialSpace::DestroySpace()
{
    for (nArray<ncSpatialCell*>::iterator cell  = this->m_cellsArray.Begin();
                                          cell != this->m_cellsArray.End();
                                          cell++)
    {
        (*cell)->DestroyCell();
        n_delete((*cell));
    }

    this->m_cellsArray.Reset();
}

//------------------------------------------------------------------------------
/**
    add a cell to the space
*/
void
ncSpatialSpace::AddSpatialCell(ncSpatialCell *cell)
{
    n_assert2(cell, "miquelangel.rujula");

    this->m_cellsArray.Append(cell);
    cell->SetParentSpace(this);
    this->m_originalBBox.extend(cell->GetOriginalBBox());
}

//------------------------------------------------------------------------------
/**
    remove a cell from the space
*/
bool 
ncSpatialSpace::RemoveSpatialCell(ncSpatialCell *cell)
{
    int index = this->m_cellsArray.FindIndex(cell);
    this->m_cellsArray.EraseQuick(index);
    return true;
}

//------------------------------------------------------------------------------
/**
    remove a cell from the space and destroys it 
    Warning: This method erases and destroys the cell, and all inside it!!
*/
bool 
ncSpatialSpace::DestroySpatialCell(ncSpatialCell *cell)
{
    int index = this->m_cellsArray.FindIndex(cell);
    this->m_cellsArray.Erase(index);
    return true;
}

//------------------------------------------------------------------------------
/**
    add an entity to the space in the corresponding cell and category, depending
    on the flags
*/
bool 
ncSpatialSpace::AddEntity(nEntityObject *entity, int flags)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to an entity!");

    if (flags & nSpatialTypes::SPF_USE_BBOX)
    {
        return this->AddEntityByBBox(entity, flags);
    }
    else if (flags & nSpatialTypes::SPF_USE_POSITION)
    {
        return this->AddEntityByPos(entity, flags);
    }
    else if (flags & nSpatialTypes::SPF_USE_MODEL)
    {
        return this->AddEntityByModel(entity, flags);
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell and category.
*/
bool 
ncSpatialSpace::AddEntityByBBox(nEntityObject * /*entity*/, 
                                const int /*flags*/)
{ 
    return false;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell and category.
    Rewrite it in subclasses.
*/
bool 
ncSpatialSpace::AddEntityByModel(nEntityObject * /*entity*/, 
                                const int /*flags*/)
{ 
    return false;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell if the
    SPF_ONE_CELL flag is set, or in all the cells whose bounding boxes contain
    the point if SPF_ALL_CELLS is set. The entity is inserted in the given 
    category.
    
    Rewrite it in subclasses.
*/
bool 
ncSpatialSpace::AddEntityByPos(nEntityObject * /*entity*/,
                              const int /*flags*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    add an entity to the space, and puts it in the corresponding cell
    Rewrite it in subclasses
*/
bool
ncSpatialSpace::AddEntity(nEntityObject* /*entity*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    remove an entity from the space
*/
bool 
ncSpatialSpace::RemoveEntity(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula");

    ncSpatial *spatialComponent = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComponent, "Error: entity hasn't spatial component!");
    ncSpatialCell *cell = spatialComponent->GetCell();
    if (!cell)
    {
        return false;
    }
    return cell->RemoveEntity(entity);
}

//------------------------------------------------------------------------------
/**
    move an entity from a cell to another one. The entity's spatial component 
    contains the origin cell
*/
bool 
ncSpatialSpace::MoveEntity(nEntityObject *entity, ncSpatialCell *destCell)
{
    n_assert2(entity, "miquelangel.rujula");
    n_assert2(destCell, "miquelangel.rujula");

    ncSpatial *spatialComponent = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComponent, "Error: entity hasn't spatial component!");

    ncSpatialCell *origCell = spatialComponent->GetCell();
    if ( !origCell )
    {
        return false;
    }

    if ( (origCell == destCell) || !origCell->RemoveEntity(entity) )
    {
        return false;
    }

    n_verify(destCell->AddEntity(entity));

    return true;
}

//------------------------------------------------------------------------------
/**
    get the number of entities in the space
*/
int 
ncSpatialSpace::GetNumEntities() const
{
    int numEntities= 0;
    for (nArray<ncSpatialCell*>::iterator cell  = this->m_cellsArray.Begin();
                                          cell != this->m_cellsArray.End();
                                          cell++)
    {
        numEntities += (*cell)->GetNumEntities();
    }

    return numEntities;
}

//------------------------------------------------------------------------------
/**
    Searches the cell that contains the given point.
    Rewrite it in subclasses.
*/
ncSpatialCell *
ncSpatialSpace::SearchCellContaining(const vector3 &/*point*/, const int /*flags*/) const
{
    // do nothing. Rewrite it in subclasses.
    return NULL;
}

//------------------------------------------------------------------------------
/**
    Search the cell in the space that completely contains the given box.
*/
ncSpatialCell *
ncSpatialSpace::SearchCellContaining(const bbox3 &/*box*/) const
{
    // do nothing. Rewrite it in subclasses.
    return NULL;
}

//------------------------------------------------------------------------------
/**
    search the inner cell in the tree that completely contains the given sphere
*/
ncSpatialCell *
ncSpatialSpace::SearchCellContaining(const sphere &/*sph*/) const
{
    // do nothing. Rewrite it in subclasses.
    return NULL;
}

//------------------------------------------------------------------------------
/**
    Searches the cell that contains the given model.
*/
ncSpatialCell *
ncSpatialSpace::SearchCellContaining(const nSpatialModel * /*model*/) const
{
    // do nothing. Rewrite it in subclasses.
    return NULL;
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    bounding box, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialSpace::SearchCellsIntersecting(const bbox3 &/*box*/, 
                                        nArray<ncSpatialCell*> * /*cells*/) const
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    sphere, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialSpace::SearchCellsIntersecting(const sphere &/*sph*/, 
                                        nArray<ncSpatialCell*> * /*cells*/) const
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    Searches all the entities whose bounding box intersects with the 
    given bounding box, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialSpace::SearchEntitiesIntersecting(const bbox3 &/*box*/, 
                                           nArray<nEntityObject*> * /*entities*/) const
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    Searches all the entities whose bounding box intersects with the 
    given sphere, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialSpace::SearchEntitiesIntersecting(const sphere &/*sph*/, 
                                           nArray<nEntityObject*> * /*entities*/) const
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    searches all the entities whose position is contained into the given sphere
*/
void 
ncSpatialSpace::SearchEntitiesContainedByPos(const sphere & /*sph*/, 
                                             nArray<nEntityObject*> * /*entities*/) const
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    Searches the cells that contain the given point
*/
void
ncSpatialSpace::SearchCellsContaining(const vector3 &/*point*/, 
                                      nArray<ncSpatialCell*> * /*cells*/,
                                      const int /*flags*/) const
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    Searches the cells that contain the given bounding box
*/
void 
ncSpatialSpace::SearchCellsContaining(const bbox3 &/*box*/, 
                                      nArray<ncSpatialCell*> * /*cells*/) const
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    Searches the cells that contain the given sphere
*/
void 
ncSpatialSpace::SearchCellsContaining(const sphere &/*sph*/, 
                                      nArray<ncSpatialCell*> * /*cells*/) const
{
    // do nothing. Rewrite it in subclasses.
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the box and the flags
*/
bool 
ncSpatialSpace::GetEntitiesCategory(const bbox3 &box,
                                    int category, 
                                    const int flags, 
                                    nArray<nEntityObject*> &entities) const
{
    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesCategories(box, categories, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the sphere and the flags
*/
bool 
ncSpatialSpace::GetEntitiesCategory(const sphere &sph,
                                    int category, 
                                    const int flags, 
                                    nArray<nEntityObject*> &entities) const
{
    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesCategories(sph, categories, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the point and the flags
*/
bool 
ncSpatialSpace::GetEntitiesCategory(const vector3 &point,
                                    int category, 
                                    const int flags, 
                                    nArray<nEntityObject*> &entities) const
{
    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesCategories(point, categories, flags, entities);
}

//------------------------------------------------------------------------------
/**
    get all the entities of various categories, using the box and the flags
*/
bool 
ncSpatialSpace::GetEntitiesCategories(const bbox3 &box,
                                      const nArray<int> &categories,
                                      const int flags, 
                                      nArray<nEntityObject*> &entities) const
{
    // search all the cells touching the box
    nArray<ncSpatialCell*> cells;
    this->SearchCellsIntersecting(box, &cells);

    if (cells.Empty())
    {
        return false;
    }

    // search the entities in these cells
    return this->GetEntitiesUsingCellsCategories(box, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get all the entities of various categories, using the sphere and the flags
*/
bool 
ncSpatialSpace::GetEntitiesCategories(const sphere &sph,
                                      const nArray<int> &categories,
                                      const int flags, 
                                      nArray<nEntityObject*> &entities) const
{
    // search all the cells touching the box
    nArray<ncSpatialCell*> cells;
    this->SearchCellsIntersecting(sph, &cells);

    if (cells.Empty())
    {
        return false;
    }

    // search the entities in these cells
    return this->GetEntitiesUsingCellsCategories(sph, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get all the entities of various categories, using the point and the flags
*/
bool 
ncSpatialSpace::GetEntitiesCategories(const vector3 &point,
                                      const nArray<int> &categories,
                                      const int flags, 
                                      nArray<nEntityObject*> &entities) const
{
    if (!this->m_bbox.contains(point))
    {
        return false;
    }

    nArray<ncSpatialCell*> cells;
    if (flags & nSpatialTypes::SPF_ONE_CELL)
    {
        // search the cell that contain the given point
        ncSpatialCell *containingCell = this->SearchCellContaining(point, flags);
        if ( containingCell )
        {
            cells.Append(containingCell);
            return this->GetEntitiesUsingCellsCategories(point, categories, flags, entities, cells);
        }
    }
    else if (flags & nSpatialTypes::SPF_ALL_CELLS)
    {
        this->SearchCellsContaining(point, &cells, flags);
        if ( cells.Empty() )
        {
            return false;
        }
        else
        {
            return this->GetEntitiesUsingCellsCategories(point, categories, flags, entities, cells);
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the box and the flags. 
    It returns the intersecting cells (it doesn't reset the cells array)
*/
bool 
ncSpatialSpace::GetEntitiesCellsCategory(const bbox3 &box,
                                         int category, 
                                         const int flags, 
                                         nArray<nEntityObject*> &entities,
                                         nArray<ncSpatialCell*> &cells) const
{
    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesCellsCategories(box, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the sphere and the flags. 
    It returns the intersecting cells
*/
bool 
ncSpatialSpace::GetEntitiesCellsCategory(const sphere &sph,
                                         int category, 
                                         const int flags, 
                                         nArray<nEntityObject*> &entities,
                                         nArray<ncSpatialCell*> &cells) const
{
    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesCellsCategories(sph, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the point and the flags. 
    It returns the intersecting cells
*/
bool 
ncSpatialSpace::GetEntitiesCellsCategory(const vector3 &point,
                                         int category, 
                                         const int flags, 
                                         nArray<nEntityObject*> &entities,
                                         nArray<ncSpatialCell*> &cells) const
{
    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesCellsCategories(point, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags. It returns the intersecting cells
*/
bool 
ncSpatialSpace::GetEntitiesCellsCategories(const bbox3 &box, 
                                           const nArray<int> &categories,
                                           const int flags, 
                                           nArray<nEntityObject*> &entities,
                                           nArray<ncSpatialCell*> &cells) const
{
    if (!this->m_bbox.intersects(box))
    {
        return false;
    }

    // search all the cells touching the box
    this->SearchCellsIntersecting(box, &cells);

    if ( cells.Empty() )
    {
        return false;
    }

    // get the entities using the found cells
    return this->GetEntitiesUsingCellsCategories(box, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags. 
    It returns the intersecting cells
*/
bool 
ncSpatialSpace::GetEntitiesCellsCategories(const sphere &sph, 
                                           const nArray<int> &categories,
                                           const int flags, 
                                           nArray<nEntityObject*> &entities,
                                           nArray<ncSpatialCell*> &cells) const
{
    if (!sph.intersects(this->m_bbox))
    {
        // the sphere is not touching this space
        return false;
    }

    // search all the cells touching the box
    this->SearchCellsIntersecting(sph, &cells);

    if ( cells.Empty() )
    {
        return false;
    }

    // get the entities using the found cells
    return this->GetEntitiesUsingCellsCategories(sph, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags. 
    It returns the containing cells
*/
bool 
ncSpatialSpace::GetEntitiesCellsCategories(const vector3 &point, 
                                      const nArray<int> &categories,
                                      const int flags, 
                                      nArray<nEntityObject*> &entities,
                                      nArray<ncSpatialCell*> &cells) const
{
    if (!this->m_bbox.contains(point))
    {
        return false;
    }

    if (flags & nSpatialTypes::SPF_ONE_CELL)
    {
        // search the cell that contains the given point
        ncSpatialCell *containingCell = this->SearchCellContaining(point, flags);
        if ( containingCell )
        {
            cells.Append(containingCell);
        }
    }
    else if (flags & nSpatialTypes::SPF_ALL_CELLS)
    {
        // search the cells that contain the given point
        this->SearchCellsContaining(point, &cells, flags);
    }

    if ( cells.Empty() )
    {
        return false;
    }

    return this->GetEntitiesUsingCellsCategories(point, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the box and the flags and
    the given cells
*/
bool 
ncSpatialSpace::GetEntitiesUsingCellsCategory(const bbox3 &box,
                                              int category, 
                                              const int flags, 
                                              nArray<nEntityObject*> &entities,
                                              const nArray<ncSpatialCell*> &cells) const
{
    n_assert(!cells.Empty());

    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesUsingCellsCategories(box, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the sphere and the flags and
    the given cells
*/
bool 
ncSpatialSpace::GetEntitiesUsingCellsCategory(const sphere &sph,
                                              int category, 
                                              const int flags, 
                                              nArray<nEntityObject*> &entities,
                                              const nArray<ncSpatialCell*> &cells) const
{
    n_assert(!cells.Empty());

    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesUsingCellsCategories(sph, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get all the entities of a given category, using the point and the flags and
    the given cells
*/
bool 
ncSpatialSpace::GetEntitiesUsingCellsCategory(const vector3 &point,
                                              int category, 
                                              const int flags, 
                                              nArray<nEntityObject*> &entities,
                                              const nArray<ncSpatialCell*> &cells) const
{
    n_assert(!cells.Empty());

    nArray<int> categories(1, 1);
    categories.Append(category);

    return this->GetEntitiesUsingCellsCategories(point, categories, flags, entities, cells);
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags. It returns the intersecting cells
*/
bool 
ncSpatialSpace::GetEntitiesUsingCellsCategories(const bbox3 &box, 
                                                const nArray<int> &categories,
                                                const int flags, 
                                                nArray<nEntityObject*> &entities,
                                                const nArray<ncSpatialCell*> &cells) const
{
    n_assert(!cells.Empty());

    if (!this->m_bbox.intersects(box))
    {
        return false;
    }

    int initSize(entities.Size());

    if (flags & nSpatialTypes::SPF_CONTAINING)
    {
        if (flags & nSpatialTypes::SPF_USE_BBOX)
        {
            // determine which entities of the given category are contained in the box
            // using their boxes
            for (int c(0); c < cells.Size(); ++c)
            {
                for (int cati(0); cati < categories.Size(); ++cati)
                {
                    const nArray<nEntityObject*> &catArray = cells[c]->GetCategory(categories[cati]);
                    for (int i(0); i < catArray.Size(); ++i)
                    {
                        if (box.contains(catArray[i]->GetComponentSafe<ncSpatial>()->GetBBox()))
                        {
                            entities.Append(catArray[i]);
                        }
                    }
                }
            }
        }
        else if (flags & nSpatialTypes::SPF_USE_POSITION)
        {
            // determine which entities of the given category are contained in the box
            // using their positions
            for (int c(0); c < cells.Size(); ++c)
            {
                for (int cati(0); cati < categories.Size(); ++cati)
                {
                    const nArray<nEntityObject*> &catArray = cells[c]->GetCategory(categories[cati]);
                    for (int i = 0; i < catArray.Size(); i++)
                    {
                        if (box.contains(catArray[i]->GetComponentSafe<ncTransform>()->GetPosition()))
                        {
                            entities.Append(catArray[i]);
                        }
                    }
                }
            }
        }
        else if (flags & nSpatialTypes::SPF_USE_MODEL)
        {
            n_assert2_always("GetEntitiesCategory using spatial model not implemented yet!");
        }
    }
    else if (flags & nSpatialTypes::SPF_INTERSECTING)
    {
        // determine which entities of the given category are intersecting the box
        for (int c(0); c < cells.Size(); ++c)
        {
            for (int cati(0); cati < categories.Size(); ++cati)
            {
                const nArray<nEntityObject*> &catArray = cells[c]->GetCategory(categories[cati]);
                for (int i = 0; i < catArray.Size(); i++)
                {
                    if (box.intersects(catArray[i]->GetComponentSafe<ncSpatial>()->GetBBox()))
                    {
                        entities.Append(catArray[i]);
                    }
                }
            }
        }
    }

    // if we've added at least one entity return true, otherwise return false
    return initSize < entities.Size();
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags. 
    It returns the intersecting cells
*/
bool 
ncSpatialSpace::GetEntitiesUsingCellsCategories(const sphere &sph, 
                                                const nArray<int> &categories,
                                                const int flags, 
                                                nArray<nEntityObject*> &entities,
                                                const nArray<ncSpatialCell*> &cells) const
{
    n_assert(!cells.Empty());

    if (!sph.intersects(this->m_bbox))
    {
        // the sphere is not touching this space
        return false;
    }

    int initSize(entities.Size());

    if (flags & nSpatialTypes::SPF_CONTAINING)
    {
        if (flags & nSpatialTypes::SPF_USE_BBOX)
        {
            // determine which entities of the given category are contained in the sphere,
            // using their bounding boxes
            for (int c = 0; c < cells.Size(); c++)
            {
                for (int cati(0); cati < categories.Size(); ++cati)
                {
                    const nArray<nEntityObject*> &catArray = cells[c]->GetCategory(categories[cati]);
                    for (int i = 0; i < catArray.Size(); i++)
                    {
                        if (sph.clipstatus(catArray[i]->GetComponentSafe<ncSpatial>()->GetBBox()) == sphere::Inside)
                        {
                            entities.Append(catArray[i]);
                        }
                    }
                }
            }
        }
        else if (flags & nSpatialTypes::SPF_USE_POSITION)
        {
            // determine which entities of the given category are contained in the sphere,
            // using their positions
            for (int c = 0; c < cells.Size(); c++)
            {
                for (int cati(0); cati < categories.Size(); ++cati)
                {
                    const nArray<nEntityObject*> &catArray = cells[c]->GetCategory(categories[cati]);
                    for (int i = 0; i < catArray.Size(); i++)
                    {
                        if (sph.contains(catArray[i]->GetComponent<ncTransform>()->GetPosition()))
                        {
                            entities.Append(catArray[i]);
                        }
                    }
                }
            }
        }
        else if (flags & nSpatialTypes::SPF_USE_MODEL)
        {
            n_assert2_always("GetEntitiesCategory using spatial model not implemented yet!");
        }
    }
    else if (flags & nSpatialTypes::SPF_INTERSECTING)
    {
        // determine which entities of the given category are intersecting the sphere
        for (int c(0); c < cells.Size(); ++c)
        {
            for (int cati = 0; cati < categories.Size(); cati++)
            {
                const nArray<nEntityObject*> &catArray = cells[c]->GetCategory(categories[cati]);
                for (int i = 0; i < catArray.Size(); i++)
                {
                    if (sph.intersects(catArray[i]->GetComponentSafe<ncSpatial>()->GetBBox()))
                    {
                        entities.Append(catArray[i]);
                    }
                }
            }
        }
    }

    // if we've added at least one entity return true, otherwise return false
    return initSize < entities.Size();
}

//------------------------------------------------------------------------------
/**
    get the entities of various categories using the given flags. 
    It returns the containing cells
*/
bool 
ncSpatialSpace::GetEntitiesUsingCellsCategories(const vector3 &point, 
                                                const nArray<int> &categories,
                                                const int flags, 
                                                nArray<nEntityObject*> &entities,
                                                const nArray<ncSpatialCell*> &cells) const
{
    n_assert(!cells.Empty());

    if (!this->m_bbox.contains(point))
    {
        return false;
    }

    bool result(false);

    if (flags & nSpatialTypes::SPF_ONE_CELL)
    {
        // search the cell that contains the given point
        const ncSpatialCell *containingCell = cells.Front();

        // collect all the entities of the desired categories
        for (int cati(0); cati < categories.Size(); ++cati)
        {
            const nArray<nEntityObject*> &catArray = containingCell->GetCategory(categories[cati]);
            for (int i(0); i < catArray.Size(); ++i)
            {
                entities.Append(catArray[i]);
            }
            result = true;
        }
    }
    else if (flags & nSpatialTypes::SPF_ALL_CELLS)
    {
        for (int c(0); c < cells.Size(); ++c)
        {
            // collect all the entities of the desired categories
            for (int cati(0); cati < categories.Size(); ++cati)
            {
                const nArray<nEntityObject*> &catArray = cells[c]->GetCategory(categories[cati]);
                for (int i(0); i < catArray.Size(); ++i)
                {
                    entities.Append(catArray[i]);
                }
                result = true;
            }
        }
    }

    return result;
}

// --- loading / saving ---

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialSpace::ReadStaticInfo(const TiXmlElement *spaceElem)
{
    n_assert2(spaceElem, "miquelangel.rujula");

    // catch the information corresponding to the cells contained in spaceElem
    return this->ReadCellsStaticInfo(spaceElem);    
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncSpatialSpace::ReadCellsStaticInfo(const TiXmlElement *spaceElem)
{
    n_assert2(spaceElem, "miquelangel.rujula: NULL pointer to TiXmlElement!");

    // cells
    TiXmlElement *cellElem = spaceElem->FirstChildElement("Cell");
    // a space has to have at least one cell
    if (!cellElem)
    {
        return false;
    }

    while (cellElem)
    {
        ncSpatialCell *newCell = n_new(ncSpatialCell);
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
*/
bool 
ncSpatialSpace::WriteStaticInfo(TiXmlElement *spaceElem)
{
    n_assert2(spaceElem, "miquelangel.rujula");

    char longCharBuff[50];

    // type of the class. Useful for checking during load
    nClass *thisClass = this->GetEntityObject()->GetClass();
    const char *spaceType = thisClass->GetProperName();
    spaceElem->SetAttribute("Type", spaceType);

    // space's original bounding box
    TiXmlElement bboxElem("OriginalBBox");
    sprintf(longCharBuff, "%f", this->m_originalBBox.vmin.x);
    bboxElem.SetAttribute("VminX", longCharBuff);
    sprintf(longCharBuff, "%f", this->m_originalBBox.vmin.y);
    bboxElem.SetAttribute("VminY", longCharBuff);
    sprintf(longCharBuff, "%f", this->m_originalBBox.vmin.z);
    bboxElem.SetAttribute("VminZ", longCharBuff);
    sprintf(longCharBuff, "%f", this->m_originalBBox.vmax.x);
    bboxElem.SetAttribute("VmaxX", longCharBuff);
    sprintf(longCharBuff, "%f", this->m_originalBBox.vmax.y);
    bboxElem.SetAttribute("VmaxY", longCharBuff);
    sprintf(longCharBuff, "%f", this->m_originalBBox.vmax.z);
    bboxElem.SetAttribute("VmaxZ", longCharBuff);

    spaceElem->InsertEndChild(bboxElem);
    
    // cells
    if (!this->AddXmlElementsForCells(spaceElem))
    {
        return false;
    }
    
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncSpatialSpace::AddXmlElementsForCells(TiXmlElement *spaceElem)
{
    n_assert2(spaceElem, "miquelangel.rujula");

    for (nArray<ncSpatialCell*>::iterator pSpatialCell  = this->m_cellsArray.Begin();
                                          pSpatialCell != this->m_cellsArray.End();
                                          pSpatialCell++)
    {
        TiXmlElement cellElem("Cell");
        // fill the TiXmlElement corresponding to the cell with its info
        if (!(*pSpatialCell)->WriteStaticInfo(&cellElem))
        {
            return false;
        }
        spaceElem->InsertEndChild(cellElem);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    save all the entities in the space
*/
bool 
ncSpatialSpace::Save()
{
    nString spatialPath;
    spatialPath.Format("level:spatial/%x/", this->GetEntityObject()->GetId());

    nFileServer2 *fileServer = nFileServer2::Instance();

    // check if the spatial directory already exists
    if (!fileServer->DirectoryExists(spatialPath.Get()))
    {
        // if the spatial directory doesn't exist yet, create it
        fileServer->MakePath(spatialPath.Get());
    }
    
    spatialPath.Append("entities.n2");

    return this->SaveEntities(spatialPath.Get());
}

//------------------------------------------------------------------------------
/**
    save the entities contained in the cell
*/
bool
ncSpatialSpace::SaveEntities(const char *fileName)
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // save entities with cmd BeginNewObjectConfig cmd
    nCmd * cmd = ps->GetCmd(this->GetEntityObject(), 'THIS');
    if (ps->BeginObjectWithCmd(this->GetEntityObject(), cmd, fileName)) 
    {
        ncSpatialCell::WizardEntityArray wizards;
        wizards.SetFixedSize( 0x100 );

        // collect all the entities in this space, classified by wizard
        int i;
        for ( i = 0; i < this->m_cellsArray.Size(); i++ )
        {
            this->m_cellsArray.At(i)->Save(wizards);
        }

        int curCellId = -1;

        // write wizards and the entities in each one
        for (int wiz = 0; wiz < wizards.Size(); ++wiz)
        {
            wizards[wiz].QSort(this->EntitiesSorter);

            ps->Put(this->GetEntityObject(), 'RSWZ', (wiz << 24) );

            //N_IFDEF_ASSERTS(nEntityObjectId prevId = nEntityObjectServer::IDINVALID);
            nEntityObjectId prevId = nEntityObjectServer::IDINVALID;
            for (int i = 0 ; i < wizards[wiz].Size() ; ++i)
            {
                if (wizards[wiz][i].cellId != curCellId)
                {
                    //--- setcellid ---
                    curCellId = wizards[wiz][i].cellId;
                    ps->Put(this->GetEntityObject(), 'MSCC', curCellId);
                }

                n_assert_if(prevId != wizards[wiz][i].objectId)
                {
                    prevId = wizards[wiz][i].objectId;
                    //--- setentityid ---
                    ps->Put(this->GetEntityObject(), 'RAEI', wizards[wiz][i].objectId);
                }
            }
        }

        ps->EndObject(true);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    sort hook for SaveEntities: sort entities by cell and entity id.
*/
int
__cdecl
ncSpatialSpace::EntitiesSorter(const void* elm0, const void* elm1)
{
    const ncSpatialCell::CellAndEntityId& item1 = *static_cast<const ncSpatialCell::CellAndEntityId*>(elm0);
    const ncSpatialCell::CellAndEntityId& item2 = *static_cast<const ncSpatialCell::CellAndEntityId*>(elm1);

    if (item1.cellId == item2.cellId)
    {
        return item1.objectId - item2.objectId;
    }

    return item1.cellId - item2.cellId;
}

//------------------------------------------------------------------------------
/**
    load the entities contained in the space
*/
bool
ncSpatialSpace::Load()
{ 
    char longCharBuff[50];

    nString spatialPath("level:spatial/");
    sprintf(longCharBuff, "%x", this->GetEntityObject()->GetId());
    spatialPath.Append(longCharBuff);
    spatialPath.Append("/entities.n2");
    
    nFileServer2 *fileServer = nFileServer2::Instance();

    /// @obsolete try to load spaces by cells
    if (!fileServer->FileExists(spatialPath.Get()))
    {
        return this->LoadOld();
    }

    if (this->isLoaded)
    {
        return true;
    }

    this->m_entitiesIdArray.Reset();
    this->currentCell = 0; //force all entities with no cell into local array

    nKernelServer::Instance()->PushCwd(this->GetEntityObject());
    nKernelServer::Instance()->Load(spatialPath.Get(), false);
    nKernelServer::Instance()->PopCwd();

    // automatically load entity ids for space-range entities (lights, batches, etc.)
    N_IFDEF_NLOG(int index = 0);
    N_IFDEF_NLOG(int prevPercent = -1);
    N_IFDEF_NLOG(int numEntities = this->m_entitiesIdArray.Size());

    ncSpatial *spatialComp;

    // ask to the entity server to load all the entities using their ids
    for (nArray<nEntityObjectId>::iterator entityId  = this->m_entitiesIdArray.Begin();
                                           entityId != this->m_entitiesIdArray.End();
                                           entityId++)
    {
        N_IFDEF_NLOG(int curPercent = n_fchop(((float) index / (float) numEntities) * 100));
        N_IFDEF_NLOG(if (curPercent != prevPercent))
        {
            NLOG(resource, (NLOGUSER, "loading entities in space: %s (%u%%)", this->GetEntityClass()->GetName(), curPercent));
        }
        N_IFDEF_NLOG(prevPercent = curPercent);
        N_IFDEF_NLOG(++index);

        nEntityObject *entity = nEntityObjectServer::Instance()->GetEntityObject(*entityId);

        if ( entity )
        {
#ifndef NGAME
            if ( nSpatialServer::Instance()->GetExecutionMode() == nSpatialServer::EDITOR )
            {
                ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
                if ( spatialComp && !spatialComp->DetermineSpace() )
                {
                    ncSpatialSpace *space = nSpatialServer::Instance()->GetSpaceById(spatialComp->GetSpaceId());
                    n_assert_if(space && (space == this) )
                    {
                        ncSpatialCell *cell = space->SearchCellById(spatialComp->GetCellId());
                        n_assert_if(cell)
                        {
                            n_verify(cell->AddEntity(entity));
                        }
                    }
                }
                else
                {
                    n_verify(this->AddEntity(entity));
                }
            }
            else
#endif // !NGAME
            {
#ifndef NGAME
                n_assert( nSpatialServer::Instance()->GetExecutionMode() == nSpatialServer::GAME );
#endif // !NGAME
                int cellId(entity->GetComponentSafe<ncSpatial>()->GetCellId());
                ncSpatialCell *cell = this->SearchCellById(cellId);
                n_assert_if(cell)
                {
                    n_verify(cell->AddEntity(entity));
                }
            }

            ncSpatialLight *light = entity->GetComponent<ncSpatialLight>();
            if ( light )
            {
                light->Flush();
            }
            else
            {
                spatialComp = entity->GetComponent<ncSpatial>();
                if ( spatialComp )
                {
                    spatialComp->CalculateLightLinks();
                }
            }
        }
        else
        {
            n_message("Can't load entity %x, that is supposed to be in space %x",
                    *entityId, this->GetEntityObject()->GetId());
        }
    }

    // empty the list of ids after loading all entities
    this->m_entitiesIdArray.Clear();

    this->isLoaded = true;

    return true;
}

//------------------------------------------------------------------------------
/**
    load all the entities in the space
*/
bool 
ncSpatialSpace::LoadOld()
{
    nString spatialPath;
    spatialPath.Format("level:spatial/%x/", this->GetEntityObject()->GetId());

    if (!nFileServer2::Instance()->DirectoryExists(spatialPath))
    {
        return true;
    }

    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          pCell++)
    {
        if (!(*pCell)->Load(spatialPath.Get()))
        {
            return false;
        }
    }

    return true;    
}

//------------------------------------------------------------------------------
/**
    change active cell to add entity ids (only used during loading time)
*/
void
ncSpatialSpace::SetCellId(int cellId)
{
    if (cellId > -1)
    {
        this->currentCell = this->SearchCellById(cellId);
    }
    else
    {
        this->currentCell = 0;
    }
}

//------------------------------------------------------------------------------
/**
    add an entity id to the space (only used during loading time)
*/
void
ncSpatialSpace::AddEntityId(nEntityObjectId entityId)
{
    if (this->currentCell)
    {
        this->currentCell->AddEntityId(entityId);
    }
    else
    {
        this->m_entitiesIdArray.Append(entityId);
    }
}

//------------------------------------------------------------------------------
/**
    save the cells structure
*/
bool
ncSpatialSpace::SaveCellsStructure(const char *path)
{
    this->m_spatialPath = path;

    nFileServer2 *fileServer = nFileServer2::Instance();
    nString fileName("cellsstructure.xml");

    // check if the spatial directory already exists
    if (!fileServer->DirectoryExists(path))
    {
        // if the spatial directory doesn't exist yet, create it
        fileServer->MakePath(path);
    }

    nString pathName(path);
    pathName.Append(fileName);
    nString realPathName(fileServer->ManglePath(pathName.Get(), false));
    TiXmlDocument doc(realPathName.Get());
    
    // main element
    TiXmlElement spaceElem("Space");

    // fill the TiXmlElement corresponding to the space with space's info
    if (this->WriteStaticInfo(&spaceElem))
    {    
        // insert the space in the document
        doc.InsertEndChild(spaceElem);

        // save the file
        return doc.SaveFile();
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
    load the cells structure
*/
bool 
ncSpatialSpace::LoadCellsStructure(const char *path)
{
    this->m_spatialPath = path;

    nFileServer2 *fileServer = nFileServer2::Instance();
    nString fileName("cellsstructure.xml");

    // check if the spatial directory exists
    if (!fileServer->DirectoryExists(path))
    {
        n_assert2_always("miquelangel.rujula: there's no spatial directory in the indoor's asset!");
        // if the spatial directory doesn't exist
        return false;
    }

    nString pathName(path);
    pathName.Append(fileName);
    nString realPathName(fileServer->ManglePath(pathName.Get(), false));
    TiXmlDocument doc(realPathName.Get());
    
    if (doc.LoadFile())
    {
        // file loaded and parsed. Proceed to catch spatial info from doc
        TiXmlElement *spaceElem = doc.FirstChildElement("Space");
        return this->ReadStaticInfo(spaceElem);
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Searches a cell with a given identifier.
*/
ncSpatialCell* 
ncSpatialSpace::SearchCellById(int cellId) const
{
    n_assert2( cellId > -1, "miquelangel.rujula");

    if ( cellId == 0 )
    {
        return NULL;
    }

    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          pCell++)
    {
        if ((*pCell)->GetId() == cellId)
        {
            return (*pCell);
        }
    }
    
    return NULL;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    set the layer id to all the entities into this space
*/
void 
ncSpatialSpace::SetLayerId(int layerId)
{
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          pCell++)
    {
        (*pCell)->SetLayerId(layerId);
    }
}
#endif

//------------------------------------------------------------------------------
/**
    update space
*/
void 
ncSpatialSpace::Update(const matrix44 &matrix)
{
    // get the increments
    vector3 incPos(matrix.pos_component() - this->m_transformationMatrix.pos_component());
    quaternion incQuat = matrix.get_quaternion() - this->m_transformationMatrix.get_quaternion();

    // set the new transformation matrix of the space
    this->SetTransformMatrix(matrix);

    // update the space's bounding box
    this->m_bbox = this->m_originalBBox;
    this->m_bbox.transform(matrix);

    // update the position of the entities in the space
    for (nArray<ncSpatialCell*>::iterator pCell  = this->m_cellsArray.Begin();
                                          pCell != this->m_cellsArray.End();
                                          pCell++)
    {
        (*pCell)->Update(incPos, incQuat, matrix);
    }
}

//------------------------------------------------------------------------------
/**
    add a portal that needs to solve its other side cell pointer
*/
void 
ncSpatialSpace::AddUnsolvedPortal(ncSpatialPortal* portal)
{
    this->m_portalsArray.Append(portal);
}

//------------------------------------------------------------------------------
/**
    assign the pointer to the m_otherSide variable (cell the portal is pointing to) 
    of the portals in m_portalsArray, using cell's id contained in each portal
*/
void
ncSpatialSpace::SolvePortalsPointers()
{
    ncSpatialCell *cell = 0;
    for (nArray<ncSpatialPortal*>::iterator pPortal  = this->m_portalsArray.Begin();
                                            pPortal != this->m_portalsArray.End();
                                            pPortal++)
    {
        if ((*pPortal)->GetOtherSideCellId() == 0)
        {
            continue;
        }

        // search the cell with the id contained in the portal
        cell = this->SearchCellById((*pPortal)->GetOtherSideCellId());

        if (cell)
        {
            // search twin portal
            const nArray<nEntityObject*> &portalsArray = cell->GetCategory(nSpatialTypes::CAT_PORTALS);
            ncSpatialPortal* otherSidePortal = 0;
            for (int i = 0; i < portalsArray.Size(); i++)
            {
                otherSidePortal = portalsArray[i]->GetComponentSafe<ncSpatialPortal>();
                if ((*pPortal)->IsTwin(otherSidePortal))
                {
                    (*pPortal)->SetTwinPortal(otherSidePortal);
                    otherSidePortal->SetTwinPortal(*pPortal);
                    break;
                }
            }
        }
    }
}
