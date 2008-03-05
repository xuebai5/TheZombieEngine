#ifndef NC_SPATIALQUADTREE_H
#define NC_SPATIALQUADTREE_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialQuadtree
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial quadtree component for outdoor entities.

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "nspatial/ncspatialspace.h"

class ncSpatialQuadtreeCell;

//------------------------------------------------------------------------------
class ncSpatialQuadtree : public ncSpatialSpace
{

    NCOMPONENT_DECLARE(ncSpatialQuadtree, ncSpatialSpace);

public:

    enum
    {
        BOXMARGIN = 70
    };

    /// constructor
    ncSpatialQuadtree();
    /// destructor
    virtual ~ncSpatialQuadtree();

    /// adjust quadtree cells' bounding box with the given offset
    void AdjustBBoxesWith(float);
    /// adjust quadtree cells' bounding box with the current offset
    void AdjustBBoxes();
    /// set the quadtree cells' bounding box offset
    void SetBBoxesOffset(float);

    /// it's called once all the entity information is ready for the components
    virtual void InitInstance(nObject::InitInstanceMsg initType);
    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// flush all the lights in the space
    virtual void FlushAllLights();

    /// destroy all the space
    virtual void DestroySpace();
    /// get the root cell of the quadtree space
    ncSpatialQuadtreeCell *GetRootCell() const;
    /// add the root cell to the quadtree space
    void AddRootCell(ncSpatialQuadtreeCell *cell);
    /// remove the root cell
    bool RemoveRootCell();
    /// add an entity to the space
    virtual bool AddEntity(nEntityObject *entity);
    /// search a cell with a given identifier
    virtual ncSpatialCell* SearchCellById(int cellId) const;
    /// searches the cell that contains the given point
    virtual ncSpatialCell *SearchCellContaining(const vector3 &point, const int flags) const;
    /// search the inner cell in the tree that completely contains the given box
    virtual ncSpatialCell *SearchCellContaining(const bbox3 &box) const;
    /// search the inner cell in the tree that completely contains the given box in 2D
    ncSpatialCell *SearchCellContaining2D(const bbox3 &box) const;
    /// searches the inner cell that completely contains the given box, from the given cell in 2D
    ncSpatialQuadtreeCell *SearchCellContaining2D(ncSpatialQuadtreeCell *cell, const bbox3 &box) const;
    /// search the inner cell in the tree that completely contains the given sphere
    virtual ncSpatialCell *SearchCellContaining(const sphere &sph) const;
    /// searches the cell that contains the given model
    virtual ncSpatialCell *SearchCellContaining(const nSpatialModel *model) const;

    /// searches all the cells intersecting with the given bounding box
    virtual void SearchCellsIntersecting(const bbox3 &box, nArray<ncSpatialCell*> *cells) const;
    /// searches all the cells intersecting with the given sphere
    virtual void SearchCellsIntersecting(const sphere &sph, nArray<ncSpatialCell*> *cells) const;

    /// searches all the elements intersecting with the given bounding box
    virtual void SearchEntitiesIntersecting(const bbox3 &box, nArray<nEntityObject*> *entities) const;
    /// searches all the entities intersecting with the given sphere
    virtual void SearchEntitiesIntersecting(const sphere &sph, nArray<nEntityObject*> *entities) const;
    /// searches all the entities whose position is contained into the given sphere
    virtual void SearchEntitiesContainedByPos(const sphere &sph, nArray<nEntityObject*> *entities) const;

    /// searches the cells that contain the given point 
    virtual void SearchCellsContaining(const vector3 &point, nArray<ncSpatialCell*> *cells, 
        const int flags) const;
    /// searches the cells that contain the given bounding box
    virtual void SearchCellsContaining(const bbox3 &box, nArray<ncSpatialCell*> *cells) const;
    /// searches the cells that contain the given sphere
    virtual void SearchCellsContaining(const sphere &sph, nArray<ncSpatialCell*> *cells) const;

    /// set tree's depth
    void SetDepth(int depth);
    /// get tree's depth
    int GetDepth() const;
    /// get the number of entities in the space
    int GetNumEntities() const;
    /// get the total number of cells in this space
    int GetTotalNumCells() const;
    /// increase the total number of cells
    void IncTotalNumCells();
    /// generate a new cell id
    int GetNewCellId();
    /// check if a point is inside the quadtree space
    bool Contains(const vector3 &point) const;
    /// check if a bouding box is completely inside the quadtree space
    bool Contains(const bbox3 &box) const;
    /// search the leaf cell that contains the parameter point
    ncSpatialQuadtreeCell *SearchLeafCell(const vector3 &point) const;
    /// searches the leaf cell in the parameter cell's subtree containing the given point
    ncSpatialQuadtreeCell *SearchLeafContaining(ncSpatialQuadtreeCell *cell, const vector3 &point) const;
    /// get the array of created leaf cells
    nArray<ncSpatialQuadtreeCell*> *GetLeafCells();

private:

    friend class nVisibleFrustumVisitor;
    friend class nSpatialQuadtreeSpaceBuilder;

    ncSpatialQuadtreeCell *m_rootCell;

    /// total number of cells in the quadtree
    int m_totalNumCells;
    /// number of levels of the tree, without counting the root
    int m_depth;
    /// cell's bounding box offset
    float cellsBBoxOffset;
    
    /// searches the inner cell that completely contains the given point, from the given cell
    ncSpatialQuadtreeCell *SearchCellContaining(ncSpatialQuadtreeCell *cell, const vector3 &point) const;
    /// searches the inner cell that completely contains the given box, from the given cell
    ncSpatialQuadtreeCell *SearchCellContaining(ncSpatialQuadtreeCell *cell, const bbox3 &box) const;
    /// searches the inner cell that completely contains the given sphere, from the given cell
    ncSpatialQuadtreeCell *SearchCellContaining(ncSpatialQuadtreeCell *cell, const sphere &sph) const;

    /// searches the cells in the subtree of the parameter cell that intersect the given bounding box
    void SearchCellsIntersecting(ncSpatialQuadtreeCell *cell, 
                                 const bbox3 &box, 
                                 nArray<ncSpatialCell*> *cells) const;
    /// searches the cells in the subtree of the parameter cell that intersect the given sphere
    void SearchCellsIntersecting(ncSpatialQuadtreeCell *cell, 
                                 const sphere &sph, 
                                 nArray<ncSpatialCell*> *cells) const;

    /// searches the cells in the subtree of the parameter cell that contain the given point
    void SearchCellsContaining(ncSpatialQuadtreeCell *cell, 
                               const vector3 &point, 
                               nArray<ncSpatialCell*> *cells) const;
    /// searches the cells in the subtree of the parameter cell that contain the given bounding box
    void SearchCellsContaining(ncSpatialQuadtreeCell *cell, 
                               const bbox3 &box, 
                               nArray<ncSpatialCell*> *cells) const;
    /// searches the cells in the subtree of the parameter cell that contain the given sphere
    void SearchCellsContaining(ncSpatialQuadtreeCell *cell, 
                               const sphere &sph, 
                               nArray<ncSpatialCell*> *cells) const;

    /// add an entity to the space, and put it in the corresponding cell and category
    bool AddEntityByBBox(nEntityObject *entity, const int flags);
    /// add an entity to the space, and put it in the corresponding cell and category
    bool AddEntityByPos(nEntityObject *entity, const int flags);
    /// add an entity to the space, and put it in the corresponding cell and category
    bool AddEntityByModel(nEntityObject *entity, const int flags);

    /// array of leaf outdoor cells created with the spatial quadtree space builder,
    /// sorted from the top-left corner to the bottom-right corner
    nArray<ncSpatialQuadtreeCell*> m_leafCells;

    /// save all the entities in the space
    virtual bool SaveEntities(const char *fileName);

    /// load all the entities in the space
    virtual bool LoadOld();

#ifdef __NEBULA_STATS__
    /// profiler for the SearchCellContaining2D method
    mutable nProfiler profSpaSCC2D;
    /// profiler to mesure the search of cells intersecting a given box
    mutable nProfiler profSpa_SearchCellsIntBox;
    /// profiler to mesure the search of cells intersecting a given sphere
    mutable nProfiler profSpa_SearchCellsIntSph;
#endif // __NEBULA_STATS__

};

//------------------------------------------------------------------------------
/**
    set tree's depth
*/
inline
void 
ncSpatialQuadtree::SetDepth(int depth)
{
    this->m_depth = depth;
}

//------------------------------------------------------------------------------
/**
    get tree's depth
*/
inline
int 
ncSpatialQuadtree::GetDepth() const
{
    return this->m_depth;
}

//------------------------------------------------------------------------------
/**
    get the total number of cells in this space
*/
inline
int 
ncSpatialQuadtree::GetTotalNumCells() const
{
    return this->m_totalNumCells;
}

//------------------------------------------------------------------------------
/**
    increase the total number of cells
*/
inline
void
ncSpatialQuadtree::IncTotalNumCells()
{
    this->m_totalNumCells++;
}

//------------------------------------------------------------------------------
/**
    generate a new cell id
*/
inline
int
ncSpatialQuadtree::GetNewCellId()
{
    return ++this->m_totalNumCells;
}

//------------------------------------------------------------------------------
/**
    get the array of leaf cells
*/
inline
nArray<ncSpatialQuadtreeCell*> *
ncSpatialQuadtree::GetLeafCells()
{
    return &this->m_leafCells;
}

//------------------------------------------------------------------------------
/**
    set the quadtree cells' bounding box offset
*/
inline
void 
ncSpatialQuadtree::SetBBoxesOffset(float offset)
{
    this->cellsBBoxOffset = offset;
}

#endif // NC_SPATIALQUADTREE_H

