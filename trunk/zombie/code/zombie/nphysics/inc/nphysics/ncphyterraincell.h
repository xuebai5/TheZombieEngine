#ifndef NC_PHYTERRAINCELL_H
#define NC_PHYTERRAINCELL_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyTerrainCell
    @ingroup NebulaPhysicsSystem
    @brief Represents a cell from a terrain object.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Terrain Cell.
    
    @cppclass ncPhyTerrainCell
    
    @superclass ncPhySimpleObj

    @classinfo Represents a cell from a terrain object.

*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphyterrain.h"
#include "nphysics/ncphysimpleobj.h"
#include "nphysics/nphygeomhashspace.h"
#include "nphysics/nphygeomspace.h"

//-----------------------------------------------------------------------------
class ncPhyTerrainCell : public ncPhySimpleObj
{

    NCOMPONENT_DECLARE(ncPhyTerrainCell,ncPhySimpleObj);

public:
    /// constructor
    ncPhyTerrainCell();

    /// destructor
    ~ncPhyTerrainCell();

    /// enables the cell
    void Enable();

    /// disables the cell
    void Disable();

    /// creates the object
    void Create( nPhysicsWorld* world );

    /// sets the terrain where this cell belongs
    void SetTerrain( ncPhyTerrain* terrain );

    /// sets the size of the cell
    void SetSizeCell( const int cellSize );

    /// sets the cell data buffer
    void SetBuffer( const phyreal* buffer );

    /// sets the length of the cell
    void SetLength( const phyreal length );

    /// returns the cell space
    nPhySpace* GetCellSpace();

    /// returns the cell space
    nPhySpace* GetDynamicCellSpace();

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// sets a hole in the given coordinates
    void SetHole( int x, int z );

    /// gets a hole in the given coordinates
    const bool GetHole( const int x, const int z ) const;

    /// removes all the holes
    void RemoveHoles();

    /// returns if the cell has any hole
    const bool IsThereAnyHole() const;

    /// marks the cell as visible
    void SetVisible();

    /// sets the cell index
    void SetIndex( const unsigned indexCell );

    /// gets the cell idnex
    const unsigned GetIndex() const;

#ifndef NGAME
    /// updates the bounding box
    void UpdateAABB();
#endif

private:

    /// stores the terrain where the cell belongs
    ncPhyTerrain* terrainCell;

    /// stores the cell size side
    int cellSizeSize;

    /// stores the buffer of data
    const phyreal* dataBuffer;

    /// stores the length of the cell in the world
    phyreal cellLength;

    /// stores the terrain cell space
    nPhyGeomHashSpace* cellSpace;

    /// stores the terrain cell space for dynamic objects
    nPhySpace* cellDynamicSpace;

    /// creates the cell's space
    void CreateSpace();

    /// stores the cell index
    unsigned index;
};

//-----------------------------------------------------------------------------
/**
    Returns the cell space.

    @return space

    history:
        - 01-Jun-2005   David Reyes    created
        - 01-Jul-2005   David Reyes    inlined
*/
inline
nPhySpace* ncPhyTerrainCell::GetCellSpace()
{
    return this->cellSpace;
}

//-----------------------------------------------------------------------------
/**
    Returns the cell's dynamic space.

    @return space

    history:
        - 01-Jul-2005   David Reyes    created
*/
inline
nPhySpace* ncPhyTerrainCell::GetDynamicCellSpace()
{
    return this->cellDynamicSpace;
}

//-----------------------------------------------------------------------------
/**
    Gets the cell index.

    @return associated index

    history:
        - 17-Jul-2006   David Reyes    created
*/
inline
const unsigned ncPhyTerrainCell::GetIndex() const
{
    return this->index;
}

#endif