#ifndef NC_SPATIALOCTREE_H
#define NC_SPATIALOCTREE_H
//------------------------------------------------------------------------------
/**
    @class ncSpatialOctree
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief Spatial octree component for octree entities.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "nspatial/ncspatialspace.h"
#include "octree/noctree.h"

//------------------------------------------------------------------------------
class nSpatialOctreeCell;

//------------------------------------------------------------------------------
class ncSpatialOctree : public ncSpatialSpace
{

    NCOMPONENT_DECLARE(ncSpatialOctree, ncSpatialSpace);

    friend class nVisibleFrustumVisitor;

public:

    /// constructor
    ncSpatialOctree();
    /// destructor
    virtual ~ncSpatialOctree();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    // --- loading / saving ---
    /// load an octree space from a file
    virtual bool LoadSpace(nFileServer2* /*fileServer*/){return false;}
    /// save an octree space to a file
    virtual bool SaveSpace(){return false;}

    void SetOctree(nOctree *octree);
    /// initializes the octree space. Creates the nOctree
    void InitializeSpace();
    /// get the root cell of the octree space
    nSpatialOctreeCell *GetRootCell();
    /// remove a cell from the space
    virtual bool RemoveSpatialCell(ncSpatialCell *cell);
    /// remove a cell from the space and destroy it
    virtual bool DestroySpatialCell(ncSpatialCell *cell);
    
    /// add an entity to the space
    virtual bool AddEntity(nEntityObject *entity);
    /// remove an entity from the space
    virtual bool RemoveEntity(nEntityObject *entity);

    /// searches the inner cell in the tree that completely contains the given box
    ncSpatialCell *SearchCellContaining(const bbox3 &box);
    /// searches all the cells intersecting with the given bounding box
    virtual void SearchCellsIntersecting(const bbox3 &box, nArray<ncSpatialCell*> *cells);
    /// searches all the cells intersecting with the given sphere
    virtual void SearchCellsIntersecting(const sphere &sph, nArray<ncSpatialCell*> *cells);
    /// searches all the entities intersecting with the given bounding box
    virtual void SearchEntitiesIntersecting(const bbox3 &box, nArray<nEntityObject*> *entities);
    /// searches all the elements intersecting with the given sphere
    virtual void SearchEntitiesIntersecting(const sphere &sph, nArray<nEntityObject*> *entities);
    /// check if a point is inside the octree space
    bool Contains(const vector3 &point) const;
    /// check if a bouding box is completely inside the octree space
    bool Contains(const bbox3 &box) const;

    /// set the space's size
    void SetSpaceSize(bbox3 box);
    /// calculate the space's cells. This must be called once all spatial elements are inserted in the space.
    void DetermineCells();

private:

    nRef<nOctree> m_octree;

    /// add an entity when the octree has already created the cells
    bool AddEntityWithCells(nEntityObject *entity);
    /// add an entity when the octree hasn't already created the cells
    bool AddEntityWithoutCells(nEntityObject *entity);

    void MoveObject(nOctElement *object, const vector3 &newpos);
    void DetermineCells(nOctNode *octNode, nSpatialOctreeCell *octreeCell);

    /// searches the inner cell that completely contains the given box, from the given cell
    nSpatialOctreeCell * SearchCellContaining(nSpatialOctreeCell *cell, const bbox3 &box);
    /// searches the cells in the subtree from the parameter octree node that intersect the given bounding box
    void SearchCellsIntersecting(nSpatialOctreeCell *cell, const bbox3 &box, nArray<ncSpatialCell*> *cells);
    /// searches the cells in the subtree from the parameter octree node that intersect the given sphere
    void SearchCellsIntersecting(nSpatialOctreeCell *cell, const sphere &sph, nArray<ncSpatialCell*> *cells);
    
};

#endif // NC_SPATIALOCTREE_H
