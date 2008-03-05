#ifndef N_SPATIALQUADTREESPACEBUILDER_H
#define N_SPATIALQUADTREESPACEBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nSpatialQuadtreeSpaceBuilder
    @ingroup NebulaSpatialSystem
    @author Miquel Angel Rujula <>

    @brief nSpatialQuadtreeSpaceBuilder helps to build nSpatialQuadtreeSpace's
    
    (C) 2004  Conjurer Services, S.A.    
*/

#include "nspatial/ncspatialquadtree.h"

class nSpatialQuadtreeSpaceBuilder
{
    
public:

    /// constructor 1
    nSpatialQuadtreeSpaceBuilder();
    /// constructor 2
    nSpatialQuadtreeSpaceBuilder(ncSpatialQuadtree *quadtreeSpace, const bbox3 &quadtreeBox,
        const bbox3 &terrainBox, int treeDepth);
    /// constructor 3
    nSpatialQuadtreeSpaceBuilder(ncSpatialQuadtree *quadtreeSpace);
    /// destructor
    ~nSpatialQuadtreeSpaceBuilder();

    /// build the quadtree space from scratch
    void BuildSpace(const bbox3 &sizeBox, int treeDepth);

    /// set the quadtree space to build. Useful if you want to continue building it
    void SetQuadtreeSpace(ncSpatialQuadtree *quadtreeSpace);
    /// returns the quadtree space. Call it after construction.
    ncSpatialQuadtree *GetQuadtreeSpace();

    /// set the terrain's bounding box
    void SetTerrainBBox(const bbox3 &box);
    /// get the terrain's bounding box
    const bbox3 &GetTerrainBBox() const;

    /// search the cell that completely contains the parameter bounding box
    ncSpatialQuadtreeCell *SearchCell(ncSpatialQuadtreeCell *quadCell, bbox3 &box);

private:

    /// quadtree space we're building
    ncSpatialQuadtree *m_quadtreeSpace;

    /// terrain bbox
    bbox3 terrainBox;

    /// create the subtree of the given cell using the current depth
    ncSpatialQuadtreeCell * CreateSubtree(const bbox3 & treeBox, int curDepth, int bx, int bz);

    /// add a cell to the leaf cells array in its place (array is sorted by position)
    void AddLeafCell(ncSpatialQuadtreeCell* cell);

};

//------------------------------------------------------------------------------
/**
    set the quadtree space to build. Useful if you want to continue building it
*/
inline
void 
nSpatialQuadtreeSpaceBuilder::SetQuadtreeSpace(ncSpatialQuadtree *quadtreeSpace)
{
    n_assert2(quadtreeSpace, "miquelangel.rujula: NULL pointer to quadtree space component!");

    this->m_quadtreeSpace = quadtreeSpace;
}

//------------------------------------------------------------------------------
/**
    returns the quadtree space. Call it after construction.
*/
inline
ncSpatialQuadtree *
nSpatialQuadtreeSpaceBuilder::GetQuadtreeSpace()
{
    if (!this->m_quadtreeSpace)
    {
        return NULL;
    }

    return this->m_quadtreeSpace;
}

//------------------------------------------------------------------------------
/**
    set the terrain's bounding box
*/
inline
void 
nSpatialQuadtreeSpaceBuilder::SetTerrainBBox(const bbox3 &box)
{
    this->terrainBox = box;
}

//------------------------------------------------------------------------------
/**
    get the terrain's bounding box
*/
inline
const bbox3 &
nSpatialQuadtreeSpaceBuilder::GetTerrainBBox() const
{
    return this->terrainBox;
}

#endif

