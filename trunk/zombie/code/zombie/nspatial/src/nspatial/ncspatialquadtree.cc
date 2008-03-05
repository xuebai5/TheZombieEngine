#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  ncspatialquadtree.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/ncspatialquadtree.h"
#include "kernel/npersistserver.h"
#include "nspatial/ncspatial.h"
#include "nspatial/nspatialmodels.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/nspatialtypes.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/nctransform.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSpatialQuadtree, ncSpatialSpace);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSpatialQuadtree)
    NSCRIPT_ADDCMD_COMPOBJECT('RSCB', void, AdjustBBoxesWith, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSAB', void, AdjustBBoxes, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSOF', void, SetBBoxesOffset, 1, (float), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSpatialQuadtree::ncSpatialQuadtree():
m_rootCell(0),
m_totalNumCells(0),
m_depth(0),
#ifdef __NEBULA_STATS__
profSpaSCC2D("profSpa_QuadSearchCCont2D", true),
profSpa_SearchCellsIntBox("profSpa_SearchCellsIntBox", true),
profSpa_SearchCellsIntSph("profSpa_SearchCellsIntSph", true),
#endif // __NEBULA_STATS__
cellsBBoxOffset(40.f)
{
    this->m_spaceType = ncSpatialSpace::N_QUADTREE_SPACE;
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncSpatialQuadtree::~ncSpatialQuadtree()
{
}

//------------------------------------------------------------------------------
/**
    flush all the lights in the space
*/
void
ncSpatialQuadtree::FlushAllLights()
{
    n_assert_return(this->m_rootCell, );

    this->m_rootCell->FlushAllLights();
}

//------------------------------------------------------------------------------
/**
    it's called once all the entity information is ready for the components
*/
void 
ncSpatialQuadtree::InitInstance(nObject::InitInstanceMsg /*initType*/) 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destroys all the cells contained in the space.
    Call it just before deleting the space.
*/
void 
ncSpatialQuadtree::DestroySpace()
{
    if (this->m_rootCell)
    {
        this->m_rootCell->DestroyCell();
        if ( this->m_rootCell->GetEntityObject() )
        {
            this->m_rootCell->GetEntityObject()->Release();
            nEntityObjectServer::Instance()->RemoveEntityObject(this->m_rootCell->GetEntityObject());
        }
        
        this->m_rootCell = 0;
    }

    this->m_totalNumCells = 0;
}

//------------------------------------------------------------------------------
/**
    add a cell to the space
*/
void
ncSpatialQuadtree::AddRootCell(ncSpatialQuadtreeCell *cell)
{
    this->m_rootCell = cell;
    this->m_bbox = cell->GetBBox();
    cell->SetParentSpace(this);
    if ( cell->GetEntityObject() )
    {
        cell->GetEntityObject()->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
    Get the root cell of the quadtree space.
    Returns 'NULL' if this space doesn't have a root cell.
*/
ncSpatialQuadtreeCell *
ncSpatialQuadtree::GetRootCell() const
{
    return this->m_rootCell;
}

//------------------------------------------------------------------------------
/**
    remove the root cell from the quadtree
*/
bool 
ncSpatialQuadtree::RemoveRootCell()
{
    this->m_rootCell->DestroyCell();
    this->m_rootCell = 0;

    return true;
}

//------------------------------------------------------------------------------
/**
    add an entity to the space
*/
bool 
ncSpatialQuadtree::AddEntity(nEntityObject *entity)
{
    n_assert2(entity, "Null pointer to entity!");
    
    int flags = 0;
    flags |= nSpatialTypes::SPF_ONE_CELL;
    flags |= nSpatialTypes::SPF_CONTAINING;
    flags |= nSpatialTypes::SPF_2D;

    return this->AddEntityByBBox(entity, flags);
}

//------------------------------------------------------------------------------
/**
    Searches a cell with a given identifier.
*/
ncSpatialCell* 
ncSpatialQuadtree::SearchCellById(int cellId) const
{
    n_assert2( cellId > -1, "miquelangel.rujula: wrong cell identifier!");

    if ( cellId == 0 )
    {
        //@todo ma.garcias -check where this is used in exporter
        return this->m_rootCell;
    }

    return this->m_rootCell->SearchCellById(cellId);
}

//------------------------------------------------------------------------------
/**
    searches the cell that contains the given point
*/
ncSpatialCell *
ncSpatialQuadtree::SearchCellContaining(const vector3 &point, const int /*flags*/) const
{
    return static_cast<ncSpatialCell*>(this->SearchCellContaining(this->GetRootCell(), point));
}

//------------------------------------------------------------------------------
/**
    Searches the inner cell in the subtree from the parameter quadtree cell that 
    contains the given point.
*/
ncSpatialQuadtreeCell * 
ncSpatialQuadtree::SearchCellContaining(ncSpatialQuadtreeCell *cell, 
                                        const vector3 &point) const
{
    n_assert2(cell, "miquelangel.rujula");
    
    if (cell->Contains(point))
    {
        if (!cell->IsLeaf())
        {
            // has subcells, check them
            ncSpatialQuadtreeCell**subcells = cell->GetSubcells();
            ncSpatialQuadtreeCell *contCell = this->SearchCellContaining(subcells[0], point);
            if (contCell)
            {
                return contCell;
            }

            contCell = this->SearchCellContaining(subcells[1], point);
            if (contCell)
            {
                return contCell;
            }
            
            contCell = this->SearchCellContaining(subcells[2], point);
            if (contCell)
            {
                return contCell;
            }

            contCell = this->SearchCellContaining(subcells[3], point);
            if (contCell)
            {
                return contCell;
            }
        }

        // there is no subcell containing the point. Return 'cell' that does
        return cell;
    }

    // 'cell' doesn't contain the box
    return 0;
}

//------------------------------------------------------------------------------
/**
    Search the inner cell in the tree that completely contains the given box.
*/
ncSpatialCell *
ncSpatialQuadtree::SearchCellContaining(const bbox3 &box) const
{
    ncSpatialQuadtreeCell *cell = this->SearchCellContaining(this->GetRootCell(),
        box);
    if (cell)
    {
        return static_cast<ncSpatialCell*>(cell);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    Searches the inner cell in the subtree from the parameter quadtree cell that 
    contains the given bounding box.
*/
ncSpatialQuadtreeCell * 
ncSpatialQuadtree::SearchCellContaining(ncSpatialQuadtreeCell *cell, 
                                        const bbox3 &box) const
{
    n_assert2(cell, "miquelangel.rujula");
    
    if (cell->GetBBox().contains(box))
    {
        if (!cell->IsLeaf())
        {
            // has subcells, check them
            ncSpatialQuadtreeCell **subcells = cell->GetSubcells();
            ncSpatialQuadtreeCell *containingCell = this->SearchCellContaining(subcells[0], box);
            if (containingCell)
            {
                return containingCell;
            }

            containingCell = this->SearchCellContaining(subcells[1], box);
            if (containingCell)
            {
                return containingCell;
            }

            containingCell = this->SearchCellContaining(subcells[2], box);
            if (containingCell)
            {
                return containingCell;
            }

            containingCell = this->SearchCellContaining(subcells[3], box);
            if (containingCell)
            {
                return containingCell;
            }
        }

        // there is no subcell completely containing the box. Return 'cell' that does
        return cell;
    }

    // 'cell' doesn't contain the box
    return 0;
}

//------------------------------------------------------------------------------
/**
    Search the inner cell in the tree that completely contains the given box, in
    two dimensions (X, Z).
*/
ncSpatialCell *
ncSpatialQuadtree::SearchCellContaining2D(const bbox3 &box) const
{
#ifdef __NEBULA_STATS__
    this->profSpaSCC2D.StartAccum();
#endif // __NEBULA_STATS__
    ncSpatialQuadtreeCell *cell = this->SearchCellContaining2D(this->GetRootCell(),
        box);
    if (cell)
    {
        #ifdef __NEBULA_STATS__
        this->profSpaSCC2D.StopAccum();
        #endif // __NEBULA_STATS__
        return static_cast<ncSpatialCell*>(cell);
    }

    #ifdef __NEBULA_STATS__
    this->profSpaSCC2D.StopAccum();
    #endif // __NEBULA_STATS__
    return 0;
}

//------------------------------------------------------------------------------
/**
    Searches the inner cell in the subtree from the parameter quadtree cell that 
    contains the given bounding box in 2 dimensions (X, Z).
*/
ncSpatialQuadtreeCell * 
ncSpatialQuadtree::SearchCellContaining2D(ncSpatialQuadtreeCell *cell, 
                                          const bbox3 &box) const
{
    n_assert2(cell, "miquelangel.rujula");
    
    if (cell->GetBBox().containsXZ(box))
    {
        if (!cell->IsLeaf())
        {
            // has subcells, check them
            ncSpatialQuadtreeCell**subcells = cell->GetSubcells();
            ncSpatialQuadtreeCell *containingCell = this->SearchCellContaining2D(subcells[0], box);
            if (containingCell)
            {
                return containingCell;
            }

            containingCell = this->SearchCellContaining2D(subcells[1], box);
            if (containingCell)
            {
                return containingCell;
            }

            containingCell = this->SearchCellContaining2D(subcells[2], box);
            if (containingCell)
            {
                return containingCell;
            }

            containingCell = this->SearchCellContaining2D(subcells[3], box);
            if (containingCell)
            {
                return containingCell;
            }
        }

        // there is no subcell completely containing the box. Return 'cell' that does
        return cell;
    }

    // 'cell' doesn't contain the box
    return 0;
}

//------------------------------------------------------------------------------
/**
    search the inner cell in the tree that completely contains the given sphere
*/
ncSpatialCell *
ncSpatialQuadtree::SearchCellContaining(const sphere &sph) const
{
    ncSpatialQuadtreeCell *cell = this->SearchCellContaining(this->GetRootCell(),
        sph);
    if (cell)
    {
        return static_cast<ncSpatialCell*>(cell);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    searches the inner cell that completely contains the given sphere, from the given cell
*/
ncSpatialQuadtreeCell *
ncSpatialQuadtree::SearchCellContaining(ncSpatialQuadtreeCell *cell, const sphere &sph) const
{
    n_assert2(cell, "Null pointer to a quadtree cell!");

    if (sph.isContainedIn(cell->GetBBox()))
    {
        if (!cell->IsLeaf())
        {
            // has subcells, check them
            ncSpatialQuadtreeCell**subcells = cell->GetSubcells();
            if (this->SearchCellContaining(subcells[0], sph))
            {
                return subcells[0];
            }

            if (this->SearchCellContaining(subcells[1], sph))
            {
                return subcells[1];
            }

            if (this->SearchCellContaining(subcells[2], sph))
            {
                return subcells[2];
            }

            if (this->SearchCellContaining(subcells[3], sph))
            {
                return subcells[3];
            }
        }

        // there is no subcell completely containing the box. Return 'cell' that does
        return cell;
    }

    // 'cell' doesn't contain the box
    return 0;
}

//------------------------------------------------------------------------------
/**
    Searches the cell that contains the given model.
    Returns NULL if no cell contains it.

    NOTE: it only tests if the bounding box of any cell completely contains the
    given model, and returns the first one that does it.

    NOTE2: by now, it only checks sphere models.
*/
ncSpatialCell *
ncSpatialQuadtree::SearchCellContaining(const nSpatialModel *model) const
{
    n_assert2((model->GetType() == nSpatialModel::SPATIAL_MODEL_SPHERE), 
              "Error: only sphere spatial models are supported!");

    nSpatialSphereModel *sphereModel = (nSpatialSphereModel*)model;
    sphere sph(sphereModel->GetSphere());

    return this->SearchCellContaining(sph);
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    bounding box, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialQuadtree::SearchCellsIntersecting(const bbox3 &box, nArray<ncSpatialCell*> *cells) const
{
    n_assert(cells);

#ifdef __NEBULA_STATS__
    this->profSpa_SearchCellsIntBox.StartAccum();
#endif // __NEBULA_STATS__

    ncSpatialQuadtreeCell *rootCell = this->GetRootCell();

    // check if the box 'touches' this space
    if (!rootCell || !this->m_bbox.intersects(box))
    {
        // the box doesn't 'touch' this space
        #ifdef __NEBULA_STATS__
        this->profSpa_SearchCellsIntBox.StopAccum();
        #endif // __NEBULA_STATS__
        return;
    }

    // search all the cells in this space that intersect with the bounding box
    this->SearchCellsIntersecting(rootCell, box, cells);

#ifdef __NEBULA_STATS__
    this->profSpa_SearchCellsIntBox.StopAccum();
#endif // __NEBULA_STATS__
}

//------------------------------------------------------------------------------
/**
    Searches all the cells whose bounding box intersects with the given 
    sphere, and fills the parameter array with them.
    It doesn't reset the parameter array.
*/
void 
ncSpatialQuadtree::SearchCellsIntersecting(const sphere &sph, nArray<ncSpatialCell*> *cells) const
{
    n_assert(cells);

#ifdef __NEBULA_STATS__
    this->profSpa_SearchCellsIntSph.StartAccum();
#endif // __NEBULA_STATS__

    ncSpatialQuadtreeCell *rootCell = this->GetRootCell();

    // check if the box 'touches' this space
    if (!rootCell || !sph.intersects(this->m_bbox))
    {
        // the box doesn't 'touch' this space
        #ifdef __NEBULA_STATS__
        this->profSpa_SearchCellsIntSph.StopAccum();
        #endif // __NEBULA_STATS__
        return;
    }

    // search all the cells in this space that intersect with the bounding box
    this->SearchCellsIntersecting(rootCell, sph, cells);

#ifdef __NEBULA_STATS__
    this->profSpa_SearchCellsIntSph.StopAccum();
#endif // __NEBULA_STATS__
}

//------------------------------------------------------------------------------
/**
    Searches the cells in the subtree of the parameter cell that intersect 
    the given bounding box.
*/
void 
ncSpatialQuadtree::SearchCellsIntersecting(ncSpatialQuadtreeCell *cell, 
                                           const bbox3 &box, 
                                           nArray<ncSpatialCell*> *cells) const
{
    n_assert(cell);
    n_assert(cells);

    if (cell->GetBBox().intersects(box))
    {
        // 'cell' intersects with the bounding box, append it
        cells->Append(cell);

        if (!cell->IsLeaf())
        {
            // check its subcells
            ncSpatialQuadtreeCell** subcells = cell->GetSubcells();
            this->SearchCellsIntersecting(subcells[0], box, cells);
            this->SearchCellsIntersecting(subcells[1], box, cells);
            this->SearchCellsIntersecting(subcells[2], box, cells);
            this->SearchCellsIntersecting(subcells[3], box, cells);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches the cells in the subtree of the parameter cell that intersect 
    the given sphere.
*/
void 
ncSpatialQuadtree::SearchCellsIntersecting(ncSpatialQuadtreeCell *cell, 
                                           const sphere &sph, 
                                           nArray<ncSpatialCell*> *cells) const
{
    n_assert(cell);
    n_assert(cells);

    if (sph.intersects(cell->GetBBox()))
    {
        // 'cell' intersects with the sphere, append it
        cells->Append(cell);

        if (!cell->IsLeaf())
        {
            // check its subcells
            ncSpatialQuadtreeCell** subcells = cell->GetSubcells();
            this->SearchCellsIntersecting(subcells[0], sph, cells);
            this->SearchCellsIntersecting(subcells[1], sph, cells);
            this->SearchCellsIntersecting(subcells[2], sph, cells);
            this->SearchCellsIntersecting(subcells[3], sph, cells);
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
ncSpatialQuadtree::SearchEntitiesIntersecting(const bbox3 &box, 
                                              nArray<nEntityObject*> *entities) const
{
    n_assert2(entities, "Null pointer to entities array!");    

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
            for ( int catIndex(0); catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
            {
                if ( catIndex == nSpatialTypes::CAT_LIGHTS )
                {
                    // append intersecting lights
                    const nArray<nEntityObject*> &lights = categories[nSpatialTypes::CAT_LIGHTS];
                    nSpatialModel* testModel = 0;
                    for (int i(0); i < lights.Size(); ++i)
                    {
                        testModel = lights[i]->GetComponentSafe<ncSpatial>()->GetTestModel();
                        if (testModel && testModel->Intersects(box))
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
                    for (int i(0); i < category.Size(); ++i)
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
    It doesn't reset the parameter array.
*/
void 
ncSpatialQuadtree::SearchEntitiesIntersecting(const sphere &sph,
                                              nArray<nEntityObject*> *entities) const
{
    n_assert2(entities, "Null pointer to entities array!");    

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
            for ( int catIndex(0); catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++catIndex)
            {
                if ( catIndex == nSpatialTypes::CAT_LIGHTS )
                {
                    // append intersecting lights
                    const nArray<nEntityObject*> &lights = categories[nSpatialTypes::CAT_LIGHTS];
                    nSpatialModel* testModel = 0;
                    for (int i(0); i < lights.Size(); ++i)
                    {
                        testModel = lights[i]->GetComponentSafe<ncSpatial>()->GetTestModel();
                        if (testModel && testModel->Intersects(sph))
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
                    for (int i(0); i < category.Size(); ++i)
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
ncSpatialQuadtree::SearchEntitiesContainedByPos(const sphere &sph, 
                                                nArray<nEntityObject*> *entities) const
{
    n_assert2(entities, "Null pointer to entities array.");

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
                if (catIndex == nSpatialTypes::CAT_NAVMESHNODES)
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
    get the number of entities in the space
*/
int 
ncSpatialQuadtree::GetNumEntities() const
{
    n_assert2(this->m_rootCell, "miquelangel.rujula: quadtree space has no cells!");

    return this->m_rootCell->GetTotalNumEntities();
}

//------------------------------------------------------------------------------
/**
    check if a point is inside the quadtree space
*/
bool 
ncSpatialQuadtree::Contains(const vector3 &point) const
{
    n_assert2(this->m_rootCell, "miquelangel.rujula: quadtree space has no cells!");

    return this->m_rootCell->GetBBox().contains(point);
}

//------------------------------------------------------------------------------
/**
    check if a bouding box is completely inside the quadtree space
*/
bool 
ncSpatialQuadtree::Contains(const bbox3 &box) const
{
    n_assert2(this->m_rootCell, "miquelangel.rujula: quadtree space has no cells!");

    return this->m_rootCell->GetBBox().contains(box);
}

//------------------------------------------------------------------------------
/**
    Search the leaf cell that contains the parameter point.
    Returns 'NULL' if the point is outside the quadtree space.
*/
ncSpatialQuadtreeCell *
ncSpatialQuadtree::SearchLeafCell(const vector3 &point) const
{
    n_assert2(this->m_rootCell, "miquelangel.rujula: quadtree space has no cells!");

    if (!this->Contains(point))
    {
        return 0;
    }

    return this->SearchLeafContaining(this->m_rootCell, point);
}

//------------------------------------------------------------------------------
/**
    Searches the leaf cell in the parameter cell's subtree containing the given point.
    It is supposed that the parameter cell contains the point.
*/
ncSpatialQuadtreeCell *
ncSpatialQuadtree::SearchLeafContaining(ncSpatialQuadtreeCell *cell, 
                                        const vector3 &point) const
{
    n_assert2(cell, "miquelangel.rujula");

    if (!cell->IsLeaf())
    {
        // has subcells, check them
        ncSpatialQuadtreeCell** subcells = cell->GetSubcells();
        if (subcells[0]->GetBBox().contains(point))
        {
            return this->SearchLeafContaining(subcells[0], point);
        }

        if (subcells[1]->GetBBox().contains(point))
        {
            return this->SearchLeafContaining(subcells[1], point);
        }

        if (subcells[2]->GetBBox().contains(point))
        {
            return this->SearchLeafContaining(subcells[2], point);
        }

        if (subcells[3]->GetBBox().contains(point))
        {
            return this->SearchLeafContaining(subcells[3], point);
        }
    }

    // is a leaf
    return cell;
}

//------------------------------------------------------------------------------
/**
    Searches the cells that contain the given point.
*/
void
ncSpatialQuadtree::SearchCellsContaining(const vector3 &point, 
                                         nArray<ncSpatialCell*> *cells,
                                         const int /*flags*/) const
{
    n_assert2(cells, "Null pointer to spatial cells array!");

    if (!this->m_bbox.contains(point))
    {
        // the point is surely outside this space
        return;
    }

    ncSpatialQuadtreeCell *rootCell = this->GetRootCell();

    this->SearchCellsContaining(rootCell, point, cells);
}

//------------------------------------------------------------------------------
/**
    searches the cells in the subtree of the parameter cell that contain the
    given point
*/
void 
ncSpatialQuadtree::SearchCellsContaining(ncSpatialQuadtreeCell *cell, 
                                         const vector3 &point, 
                                         nArray<ncSpatialCell*> *cells) const
{
    n_assert2(cell, "Null pointer to a quadtree cell!");

    if (cell->GetBBox().contains(point))
    {
        // the cell contains the box
        cells->Append(cell);

        // let's which of its childs contain the box
        if (!cell->IsLeaf())
        {
            ncSpatialQuadtreeCell** subcells = cell->GetSubcells();
            this->SearchCellsContaining(subcells[0], point, cells);
            this->SearchCellsContaining(subcells[1], point, cells);
            this->SearchCellsContaining(subcells[2], point, cells);
            this->SearchCellsContaining(subcells[3], point, cells);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Searches the cells whose bounding boxes completely contain the given bounding box
*/
void 
ncSpatialQuadtree::SearchCellsContaining(const bbox3 &box, nArray<ncSpatialCell*> *cells) const
{
    n_assert2(cells, "Error: null pointer to cells array.");

    // First check if the parameter bounding box 'touches' this space.
    if (!this->m_bbox.intersects(box))
    {
        return;
    }

    ncSpatialQuadtreeCell *rootCell = this->GetRootCell();

    this->SearchCellsContaining(rootCell, box, cells);
}

//------------------------------------------------------------------------------
/**
    searches the cells in the subtree of the parameter cell that contain the given bounding box
*/
void 
ncSpatialQuadtree::SearchCellsContaining(ncSpatialQuadtreeCell *cell, 
                                         const bbox3 &box, 
                                         nArray<ncSpatialCell*> *cells) const
{
    n_assert2(cell, "Null pointer to a quadtree cell!");

    if (cell->GetBBox().contains(box))
    {
        // the cell contains the box
        cells->Append(cell);

        // let's which of its childs contain the box
        if (!cell->IsLeaf())
        {
            ncSpatialQuadtreeCell** subcells = cell->GetSubcells();
            this->SearchCellsContaining(subcells[0], box, cells);
            this->SearchCellsContaining(subcells[1], box, cells);
            this->SearchCellsContaining(subcells[2], box, cells);
            this->SearchCellsContaining(subcells[3], box, cells);
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
ncSpatialQuadtree::SearchCellsContaining(const sphere &sph, nArray<ncSpatialCell*> *cells) const
{
    n_assert2(cells, "Error: null pointer to cells array.");

    // First check if the parameter sphere 'touches' this space.
    if (!sph.intersects(this->m_bbox))
    {
        return;
    }

    ncSpatialQuadtreeCell *rootCell = this->GetRootCell();

    this->SearchCellsContaining(rootCell, sph, cells);
}

//------------------------------------------------------------------------------
/**
    searches the cells in the subtree of the parameter cell that contain the 
    given sphere
*/
void 
ncSpatialQuadtree::SearchCellsContaining(ncSpatialQuadtreeCell *cell, 
                                         const sphere &sph, 
                                         nArray<ncSpatialCell*> *cells) const
{
    n_assert2(cell, "Null pointer to a quadtree cell!");

    if (sph.isContainedIn(cell->GetBBox()))
    {
        // the cell contains the box
        cells->Append(cell);

        // let's which of its childs contain the box
        if (!cell->IsLeaf())
        {
            ncSpatialQuadtreeCell** subcells = cell->GetSubcells();
            this->SearchCellsContaining(subcells[0], sph, cells);
            this->SearchCellsContaining(subcells[1], sph, cells);
            this->SearchCellsContaining(subcells[2], sph, cells);
            this->SearchCellsContaining(subcells[3], sph, cells);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell and category.
*/
bool 
ncSpatialQuadtree::AddEntityByBBox(nEntityObject *entity, 
                                   const int flags)
{ 
    n_assert2(entity, "Null pointer to an entity!");

    bool result(false);

    // get entity's spatial component
    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "Entity hasn't spatial component!");
    const bbox3 &entityBox = spatialComp->GetBBox();

    // insert the entity depending on the flags
    if (flags & nSpatialTypes::SPF_ONE_CELL)
    {
        // insert it in only one cell
        if (flags & nSpatialTypes::SPF_CONTAINING)
        {
            ncSpatialQuadtreeCell *containingCell = 0;

            // search the cell that contains the element's bounding box
            if (flags & nSpatialTypes::SPF_2D)
            {
                containingCell = static_cast<ncSpatialQuadtreeCell*>(this->SearchCellContaining2D(entityBox));
            }
            else
            {
                containingCell = static_cast<ncSpatialQuadtreeCell*>(this->SearchCellContaining(entityBox));
            }

            if (containingCell)
            {
                n_verify(containingCell->AddEntity(entity));
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
            result = false;
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
            result = false;
        }
    }


    return result;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space, and put it in the corresponding cell if the
    SPF_ONE_CELL flag is set, or in all the cells whose bounding boxes contain
    the point if SPF_ALL_CELLS is set. 
*/
bool 
ncSpatialQuadtree::AddEntityByPos(nEntityObject *entity,
                                  const int flags)
{
    n_assert2(entity, "miquelangel.rujula: NULL pointer to an entity!");
    n_assert2(!(flags & nSpatialTypes::SPF_USE_POSITION), "Wrong spatial insertion flag!");

    // get the entity's spatial component
    ncSpatial *spatialComp = entity->GetComponent<ncSpatial>();
    n_assert2(spatialComp, "miquelangel.rujula: entity hasn't spatial component!");

    const vector3 &pos = spatialComp->GetBBox().center();
    bool result(false);

    if (flags & nSpatialTypes::SPF_ONE_CELL)
    {
        // search the cell that contains the spatial component's position
        ncSpatialQuadtreeCell *containingCell = static_cast<ncSpatialQuadtreeCell*>(this->SearchCellContaining(pos, flags));
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
    }

    return result;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the space using its model, and put it in the corresponding 
    cell and category.
*/
bool 
ncSpatialQuadtree::AddEntityByModel(nEntityObject *entity, 
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

    bool result(false);

    // by now, we'll only use the sphere model
    n_assert2(spatialCompModel && (spatialCompModel->GetType() != nSpatialModel::SPATIAL_MODEL_SPHERE), 
              "miquelangel.rujula: wrong spatial model while inserting entity in a quadtree space.");
    

    nSpatialSphereModel *sphereModel = static_cast<nSpatialSphereModel*>(spatialCompModel);
    sphere sph(sphereModel->GetSphere());

    // insert the entity depending on the flags
    if (flags & nSpatialTypes::SPF_ONE_CELL)
    {
        // insert it in only one cell
        if (flags & nSpatialTypes::SPF_CONTAINING)
        {
            // insert it in the cell that contains the sphere
            ncSpatialQuadtreeCell *containingCell = static_cast<ncSpatialQuadtreeCell*>(this->SearchCellContaining(sphereModel));
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
                    // its bounding box
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
    adjust quadtree cells' bounding box with the given offset
*/
void 
ncSpatialQuadtree::AdjustBBoxesWith(float offset)
{
    this->cellsBBoxOffset = offset;
    this->m_rootCell->AdjustBBox(offset);
}

//------------------------------------------------------------------------------
/**
    adjust quadtree cells' bounding box with the current offset
*/
void 
ncSpatialQuadtree::AdjustBBoxes()
{
    this->m_rootCell->AdjustBBox(this->cellsBBoxOffset);
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSpatialQuadtree::SaveCmds(nPersistServer *ps)
{
    if (!ncSpatialSpace::SaveCmds(ps))
    {
        return false;
    }
    
    if (!ps->Put(this->GetEntityObject(), 'RSOF', this->cellsBBoxOffset))
    {
        return false;
    }

    // --- setbbox ---
    vector3 boxCenter(this->m_bbox.center());
    vector3 boxExtents(this->m_bbox.extents());
    if (!ps->Put(this->GetEntityObject(), 'RSBX', boxCenter.x, boxCenter.y, 
        boxCenter.z, boxExtents.x, boxExtents.y, boxExtents.z))
    {
        return false;
    }
    
    return true;
}

//------------------------------------------------------------------------------
/**
    save the entities contained in the cell
*/
bool 
ncSpatialQuadtree::SaveEntities(const char *fileName)
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
        this->m_rootCell->Save(wizards);

        int curCellId = -1;

        // write wizards and the entities in each one
        for (int wiz(0); wiz < wizards.Size(); ++wiz)
        {
            wizards[wiz].QSort(this->EntitiesSorter);

            ps->Put(this->GetEntityObject(), 'RSWZ', (wiz << 24));

            //N_IFDEF_ASSERTS(nEntityObjectId prevId = nEntityObjectServer::IDINVALID);
            nEntityObjectId prevId = nEntityObjectServer::IDINVALID;
            for (int i(0); i < wizards[wiz].Size() ; ++i)
            {
                if (wizards[wiz][i].cellId != curCellId)
                {
                    //--- setcellid ---
                    curCellId = wizards[wiz][i].cellId;
                    ps->Put(this->GetEntityObject(), 'MSCC', curCellId);
                }

                n_assert_if(prevId != wizards[wiz][i].objectId)
                {
                    //N_IFDEF_ASSERTS(prevId = wizards[wiz][i].objectId);
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
    load all the entities in the space
*/
bool 
ncSpatialQuadtree::LoadOld()
{
    char longCharBuff[50];

    nString spatialPath("level:spatial/");
    sprintf(longCharBuff, "%x", this->GetEntityObject()->GetId());
    spatialPath.Append(longCharBuff);
    spatialPath.Append("/");

    return this->m_rootCell->Load(spatialPath.Get());
}
