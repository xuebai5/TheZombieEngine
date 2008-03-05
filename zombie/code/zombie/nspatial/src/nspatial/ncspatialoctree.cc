#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialoctree.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialoctree.h"
#include "kernel/npersistserver.h"

#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialmc.h"
#include "nspatial/nspatialmodels.h"
#include "nspatial/nspatialoctreecell.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialOctree, ncSpatialSpace);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialOctree)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSpatialOctree::ncSpatialOctree()
{
    this->m_spaceType = ncSpatialSpace::N_OCTREE_SPACE;
}

//------------------------------------------------------------------------------
/**
*/
ncSpatialOctree::~ncSpatialOctree()
{
    nSpatialOctreeCell *octreeCell;
    for (nArray<ncSpatialCell*>::iterator cell  = this->m_cellsArray.Begin();
                                          cell != this->m_cellsArray.End();
                                          cell++)
    {
        octreeCell = (nSpatialOctreeCell*)(*cell);
        octreeCell->DestroyCell();
        n_delete(octreeCell);
    }

    this->m_cellsArray.Reset();
    if (this->m_octree.isvalid())
    {
        this->m_octree->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialOctree::SaveCmds(nPersistServer *ps)
{
    if (ncSpatialSpace::SaveCmds(ps))
    {
        return true;
    }
    return false;
}

void 
ncSpatialOctree::SetOctree(nOctree *octree)
{
    this->m_octree = octree;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialOctree::InitializeSpace()
{

    this->m_octree = static_cast<nOctree*>(nKernelServer::Instance()->New("noctree", "/usr/space/octree"));
}

//------------------------------------------------------------------------------
/**
    Get the root cell of the octree space.
    Returns 'NULL' if this space doesn't have a root cell.
*/
nSpatialOctreeCell *
ncSpatialOctree::GetRootCell()
{
    if (this->m_cellsArray.Size() == 1)
    {
        return static_cast<nSpatialOctreeCell*>(this->m_cellsArray.At(0));
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
    remove a cell from the space

    NOTE: this removes the cell from the space and it doesn't destroy the cell.
*/

bool
ncSpatialOctree::RemoveSpatialCell(ncSpatialCell *cell)
{
    /// WARNING!!!!! We also have to remove the cell's nOctreeNode from the octree
    return ncSpatialSpace::RemoveSpatialCell(cell);
}

//------------------------------------------------------------------------------
/**
    remove a cell from the space

    NOTE: this removes the cell from the space and destroys the cell.
*/

bool
ncSpatialOctree::DestroySpatialCell(ncSpatialCell *cell)
{
    /// WARNING!!!!! We also have to remove the cell's nOctreeNode from the octree
    return ncSpatialSpace::DestroySpatialCell(cell);
}

//------------------------------------------------------------------------------
/**
    add an entity to the space
*/
bool 
ncSpatialOctree::AddEntity(nEntityObject *entity)
{
    if (this->m_cellsArray.Size() > 0)
    {
        return this->AddEntityWithCells(entity);
    }
    else
    {
        return this->AddEntityWithoutCells(entity);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialOctree::AddEntityWithCells(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to entity!");
    
    // get the spatial component from the entity
    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "miquelangel.rujula: entity hasn't spatial component!");

    // search the cell (lower in the tree) that completely contains the entity's bounding box
    ncSpatialCell *cell = this->SearchCellContaining(spatialComp->GetBBox());

    if (cell)
    {
        // insert the entity in that cell
        n_verify(cell->AddEntity(entity));

        return true;
    }

    // couldn't find a cell containing the entity
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialOctree::AddEntityWithoutCells(nEntityObject *entity)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to entity!");

    // get the spatial component from the entity
    ncSpatial *spatialComponent = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComponent, "miquelangel.rujula: entity hasn't spatial component!");

    nOctElement *newOctElement = n_new(nOctElement);
    // set element's center and radius
    newOctElement->Set(spatialComponent->GetBBox().center(), spatialComponent->GetBBox().extents().len());
        
    // set pointer to the entity
    newOctElement->SetPtr(entity);

    // set pointer back to the octree element
    //spatialComponent->SetAuxPtr(newOctElement);
    spatialComponent->SetOctElement(newOctElement);
    // insert the new octree element in the octree
    this->m_octree->AddElement(newOctElement);
    // move it to its corresponding node in the tree
    this->MoveObject(newOctElement, spatialComponent->GetBBox().center());

    return true;
}

//------------------------------------------------------------------------------
/**
    remove an entity from the space without destroying it
*/
bool 
ncSpatialOctree::RemoveEntity(nEntityObject *entity)
{
    // get the spatial component from the entity
    ncSpatial *spatialComponent = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComponent, "Error: entity hasn't spatial component!");
    
    nSpatialOctreeCell *octreeCell = static_cast<nSpatialOctreeCell*>(spatialComponent->GetCell());
    if (octreeCell->RemoveEntity(entity))
    {
        if (spatialComponent->GetType() == ncSpatial::N_SPATIAL_MC)
        {
            (static_cast<ncSpatialMC*>(spatialComponent))->RemoveCell(octreeCell);
        }
        else
        {
            spatialComponent->SetCell(0);
        }
        
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    Search the inner cell in the tree that completely contains the given box.
*/
ncSpatialCell *
ncSpatialOctree::SearchCellContaining(const bbox3 &box)
{
    nSpatialOctreeCell *cell = this->SearchCellContaining(static_cast<nSpatialOctreeCell*>(this->m_cellsArray[0]),
                                                          box);
    if (cell)
    {
        return static_cast<ncSpatialCell*>(cell);
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
    Searches the inner cell in the subtree from the parameter octree cell that 
    contains the given bounding box.
*/
nSpatialOctreeCell * 
ncSpatialOctree::SearchCellContaining(nSpatialOctreeCell *cell, 
                                      const bbox3 &box)
{
    n_assert2(cell, "miquelangel.rujula");
    
    if (cell->GetBBox().contains(box))
    {
        // at least the given cell completely contains the bounding box

        // check its subcells
        for (nArray<nSpatialOctreeCell*>::iterator pSubcell  = cell->GetSubcells()->Begin();
                                                   pSubcell != cell->GetSubcells()->End();
                                                   pSubcell++)
        {
            if (this->SearchCellContaining((*pSubcell), box))
            {
                return (*pSubcell);
            }
        }

        // there is no subcell completely containing the box. Return 'cell' that does
        return cell;
    }

    // 'cell' doesn't contain the box
    return NULL;
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    bounding box, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialOctree::SearchCellsIntersecting(const bbox3 &box, nArray<ncSpatialCell*> *cells)
{
    n_assert2(cells, "miquelangel.rujula");

    nSpatialOctreeCell *rootCell = this->GetRootCell();
    
    // check if the box 'touches' this space
    if (!rootCell || !this->m_bbox.intersects(box))
    {
        // the box doesn't 'touch' this space
        return;
    }
    
    // search all the cells in this space that intersect with the bounding box
    this->SearchCellsIntersecting(rootCell, box, cells);
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    sphere, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialOctree::SearchCellsIntersecting(const sphere &sph, nArray<ncSpatialCell*> *cells)
{
    n_assert2(cells, "miquelangel.rujula");

    nSpatialOctreeCell *rootCell = this->GetRootCell();

    // check if the box 'touches' this space
    if (!rootCell || !sph.intersects(this->m_bbox))
    {
        // the box doesn't 'touch' this space
        return;
    }
    
    // search all the cells in this space that intersect with the bounding box
    this->SearchCellsIntersecting(rootCell, sph, cells);
}

//------------------------------------------------------------------------------
/**
    Searches the cells in the subtree from the parameter octree node that 
    intersect the given bounding box.
*/
void 
ncSpatialOctree::SearchCellsIntersecting(nSpatialOctreeCell *cell, 
                                         const bbox3 &box, 
                                         nArray<ncSpatialCell*> *cells)
{
    n_assert2(cell, "miquelangel.rujula");
    n_assert2(cells, "miquelangel.rujula");

    if (cell->GetBBox().intersects(box))
    {
        // the node intersects with the bounding box, append its cell
        cells->Append(cell);

        // check its subcells
        for (nArray<nSpatialOctreeCell*>::iterator pSubcell  = cell->GetSubcells()->Begin();
                                                   pSubcell != cell->GetSubcells()->End();
                                                   pSubcell++)
        {
            this->SearchCellsIntersecting((*pSubcell), box, cells);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches the cells in the subtree of the parameter cell that intersect 
    the given sphere.
*/
void 
ncSpatialOctree::SearchCellsIntersecting(nSpatialOctreeCell *cell, 
                                         const sphere &sph, 
                                         nArray<ncSpatialCell*> *cells)
{
    n_assert2(cell, "miquelangel.rujula");
    n_assert2(cells, "miquelangel.rujula");

    if (sph.intersects(cell->GetBBox()))
    {
        // 'cell' intersects with the sphere, append it
        cells->Append(cell);

        // check its subcells
        for (nArray<nSpatialOctreeCell*>::iterator pSubcell  = cell->GetSubcells()->Begin();
                                                   pSubcell != cell->GetSubcells()->End();
                                                   pSubcell++)
        {
            this->SearchCellsIntersecting((*pSubcell), sph, cells);
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
ncSpatialOctree::SearchEntitiesIntersecting(const bbox3 &box, nArray<nEntityObject*> *entities)
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
                                              pCell++)
        {
            categories = (*pCell)->GetCategories();
            for ( int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
            {
                if ( catIndex == nSpatialTypes::CAT_LIGHTS )
                {
                    // append intersecting lights
                    const nArray<nEntityObject*> &lights = categories[nSpatialTypes::CAT_LIGHTS];
                    for (int i = 0; i < lights.Size(); i++)
                    {
                        if (lights[i]->GetComponentSafe<ncSpatial>()->GetTestModel()->Intersects(box))
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
                    for (int i = 0; i < category.Size(); i++)
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
ncSpatialOctree::SearchEntitiesIntersecting(const sphere &sph, nArray<nEntityObject*> *entities)
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
                                              pCell++)
        {
            categories = (*pCell)->GetCategories();
            for ( int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
            {
                if ( catIndex == nSpatialTypes::CAT_LIGHTS )
                {
                    // append intersecting lights
                    const nArray<nEntityObject*> &lights = categories[nSpatialTypes::CAT_LIGHTS];
                    for (int i = 0; i < lights.Size(); i++)
                    {
                        if (lights[i]->GetComponentSafe<ncSpatial>()->GetTestModel()->Intersects(sph))
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
                    for (int i = 0; i < category.Size(); i++)
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
    check if a point is inside the octree space
*/
bool 
ncSpatialOctree::Contains(const vector3 &point) const
{
    return this->m_bbox.contains(point);
}

//------------------------------------------------------------------------------
/**
    check if a bouding box is completely inside the octree space
*/
bool 
ncSpatialOctree::Contains(const bbox3 &box) const
{
    return this->m_bbox.contains(box);
}

//------------------------------------------------------------------------------
/**
*/
void 
ncSpatialOctree::MoveObject(nOctElement *octElement, const vector3 &newpos)
{
    nEntityObject *entity = static_cast<nEntityObject*>(octElement->GetPtr());
    // get the spatial component from the entity
    ncSpatial *spatialComponent = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComponent, "Error: entity hasn't spatial component!");

    this->m_octree->UpdateElement(octElement, newpos, spatialComponent->GetBBox());
    
}

//------------------------------------------------------------------------------
/**
*/
void 
ncSpatialOctree::DetermineCells(nOctNode *octNode, nSpatialOctreeCell *octreeCell)
{
    // associate the cell to all the elements in the octree node
    nOctElement *oe;
    nEntityObject *entity = 0;
    ncSpatial *spatialComponent = 0;
    for (oe = (nOctElement *) octNode->elm_list.GetHead();
         oe;
         oe = (nOctElement *) oe->GetSucc())
    {
        entity = static_cast<nEntityObject*>(oe->GetPtr());
        spatialComponent = entity->GetComponent<ncSpatial>();
        n_verify(octreeCell->ncSpatialCell::AddEntity(entity));
    }

    // recurse on all children
    for (int i=0; i<8; i++)
    {
        if (!octNode->c[i])
        {
            break;
        }
        // create an octree cell for the child node
        nSpatialOctreeCell *childCell = n_new(nSpatialOctreeCell);
        childCell->SetParentSpace(this);
        childCell->SetOctreeNode(octNode->c[i]);
        // determine the subcells of this child
        this->DetermineCells(octNode->c[i], childCell);
        // append the child cell to the subcells array of the octree cell
        octreeCell->AddSubCell(childCell);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncSpatialOctree::DetermineCells()
{
    // balance the tree to create all the octree nodes, 
    // which will coincide with nSpatialOctreeCell's
    this->m_octree->BalanceTree();
    nOctNode* rootNode = this->m_octree->GetRoot();

    // create an octree cell for the root node
    nSpatialOctreeCell *rootCell = n_new(nSpatialOctreeCell);
    rootCell->SetParentSpace(this);
    rootCell->SetOctreeNode(rootNode);

    // determine all the other cells of the octree
    this->DetermineCells(rootNode, rootCell);
    // append the root cell to the octree cell
    this->m_cellsArray.Append(rootCell);
}

//------------------------------------------------------------------------------
/**
    set the space's size
*/
void 
ncSpatialOctree::SetSpaceSize(bbox3 box)
{
    this->m_bbox = box;
    this->m_octree->SetSize(box.vmin, box.vmax);
}

