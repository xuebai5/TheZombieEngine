#ifndef N_PHYGEOMHEIGHTMAP_H
#define N_PHYGEOMHEIGHTMAP_H

//-----------------------------------------------------------------------------
/**
    @class nPhyGeomHeightMap
    @ingroup NebulaPhysicsSystem
    @brief A geometry representing a height map.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Height Map
    
    @cppclass nPhyGeomHeightMap
    
    @superclass nPhysicsGeom

    @classinfo A geometry representing a height map.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsgeom.h"

//-----------------------------------------------------------------------------
class nPhyGeomHeightMap : public nPhysicsGeom 
{
public:
    /// constructor
    nPhyGeomHeightMap();
   
    /// destructor
    virtual ~nPhyGeomHeightMap();

    /// sets the buffer of heights
    void SetHeightsBuffer( phyreal* buffer );

    /// sets the legnth of the height map
    void SetLength( phyreal length );

    /// sets the number of nodes of the height map
    void SetNumNodes( int numnodes );

    /// sets the step to be jump for line in the memory buffer
    void SetStep( int stepjump );

    /// creates the geometry
    void Create();

    /// sets a hole in the given coordinates
    void SetHole( int x, int z );

    /// gets a hole in the given coordinates
    const bool GetHole( const int x, const int z ) const;

    /// removes all the holes
    void RemoveHoles();

#ifndef NGAME
    /// draws the geometry for debug pourposes
    void Draw( nGfxServer2* server );
#endif

#ifndef NGAME
    /// updates the bounding box
    void UpdateAABB();
#endif

private:
    /// buffer storing the heights
    phyreal* heights;

    /// stores the length of the height map
    phyreal length;

    /// stores the nodes of the height map
    int numNodes;

    /// stores the line step
    int step;
};

//-----------------------------------------------------------------------------
/**
    Sets the step to be jump for line in the memory buffer.

    @param stepjump bytes to be jumper per line

    history:
        - 11-Apr-2005   Zombie         created
*/
inline
void nPhyGeomHeightMap::SetStep( int stepjump )
{
    n_assert2( stepjump >= 0, "Invalid value for step." );

    this->step = stepjump;
}

#endif 
