#ifndef N_SPATIALOCTREECELL_H
#define N_SPATIALOCTREECELL_H
//------------------------------------------------------------------------------
/**
    @class nSpatialOctreeCell
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief nSpatialOctreeCell represents a division of a space. 
    It contains spatial elements in an octree.
    
    (C) 2004  Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nspatial/ncspatialcell.h"
#include "octree/noctree.h"

//------------------------------------------------------------------------------
class nEntityObject;

//------------------------------------------------------------------------------
class nSpatialOctreeCell : public ncSpatialCell
{
public:
    /// constructor
    nSpatialOctreeCell();
    /// destructor
    ~nSpatialOctreeCell();

    /// destroy the cell and all its elements
    virtual void DestroyCell();
    /// adds an entity to the cell's array
    virtual bool AddEntity(nEntityObject *entity);
    /// add a subcell
    bool AddSubCell(nSpatialOctreeCell *subCell);
    /// removes an entity from the cell
    virtual bool RemoveEntity(nEntityObject *entity);
    /// get subcells array
    nArray<nSpatialOctreeCell*> *GetSubcells();
    /// set the octree node
    void SetOctreeNode(nOctNode *octreeNode);

private:

    nOctNode *m_octreeNode;

    // subcells array
    nArray<nSpatialOctreeCell*> m_cellsArray;
};

//------------------------------------------------------------------------------
/**
    constructor
*/
inline
nSpatialOctreeCell::nSpatialOctreeCell()
{
    this->m_cellType = ncSpatialCell::N_OCTREE_CELL;
}

//------------------------------------------------------------------------------
/**
    Adds a subcell to the subcells array.
*/
inline
bool 
nSpatialOctreeCell::AddSubCell(nSpatialOctreeCell *subCell)
{
    n_assert2(subCell, "miquelangel.rujula");
    
    if ( this->m_cellsArray.Size() > 7 )
    {
        return false;
    }
    
    this->m_cellsArray.Append(subCell);
    
    return true;
}

//------------------------------------------------------------------------------
/**
    get subcells array
*/
inline
nArray<nSpatialOctreeCell*> *
nSpatialOctreeCell::GetSubcells()
{
    return &this->m_cellsArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSpatialOctreeCell::SetOctreeNode(nOctNode *octreeNode)
{
    n_assert2(octreeNode, "miquelangel.rujula: NULL pointer to octree node!");

    this->m_octreeNode = octreeNode;
    this->m_bbox.begin_extend();
    this->m_bbox.extend(octreeNode->minCorner);
    this->m_bbox.extend(octreeNode->maxCorner);
}

#endif
