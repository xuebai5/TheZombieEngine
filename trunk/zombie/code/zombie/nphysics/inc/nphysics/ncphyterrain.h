#ifndef NC_NPHYTERRAIN_H
#define NC_NPHYTERRAIN_H
//-----------------------------------------------------------------------------
/**
    @class ncPhyTerrain
    @ingroup NebulaPhysicsSystem
    @brief An abstract terrain geometry

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Terrain.
    
    @cppclass ncPhyTerrain
    
    @superclass ncPhysicsObj

    @classinfo An abstract terrain geometry.

*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphysicsobj.h"
#include "ngeomipmap/nfloatmap.h"
#include "ngeomipmap/nterrainline.h"

//-----------------------------------------------------------------------------

class ncPhyTerrainCell;
class nPersistServer;
class nPhysicsWorld;
class nPhyGeomHashSpace;
class ncTerrainGMMClass;
//-----------------------------------------------------------------------------
class ncPhyTerrain : public ncPhysicsObj
{

    NCOMPONENT_DECLARE(ncPhyTerrain,ncPhysicsObj);

    friend class ncOutdoorBuilder;

public:
    /// constructor
    ncPhyTerrain();

    /// destructor
    ~ncPhyTerrain();

    /// functionality to persist the object
    bool SaveCmds(nPersistServer* ps);

    /// creates the terrain in the world
    void Create( nPhysicsWorld* world );

    /// enables the terrain
    void Enable();

    /// disables the terrain
    void Disable();

    /// sets the categories
    void SetCategories( int categories );

    /// sets the collisions relationship
    void SetCollidesWith( int categories );

    /// begin:scripting
    /// end:scripting

    /// returns the terrain space
    nPhySpace* GetTerrainSpace();

    /// returns the total number of nodes
    int GetTotalNumNodes() const;

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// returns the number of cells
    int GetNumberOfCells() const;

    /// returns a cell
    ncPhyTerrainCell* GetTerrainCell( int index ) const;

    /// returns the bounding box of this geometry
    void GetAABB( nPhysicsAABB& boundingbox );

    /// return a cell's lenght
    int GetLenghtCell() const;

    /// access to heighmap info
    nFloatMap* GetHeightMapInfo() const;

#ifndef NGAME
    /// updates the aabbs when the heightmap has changed.
    void UpdateTerrainBoundingBoxes();

    /// draws the physic object
    virtual void Draw( nGfxServer2* server );
#endif

    /// Adds a hole to the terrain, returning the hole added
    nTerrainLine* AddHole( nTerrainLine* polyLine );

#ifndef NGAME
    /// returns the number of holes
    const int GetNumHoles() const;

    /// returns index of given hole
    int GetIndexOfHole( nTerrainLine * line ) const;

    /// returns a hole by index
    nTerrainLine* GetHole( const int index ) const;

    /// remove hole by index
    void RemoveHole( const int index );
    
    /// removes all the holes
    void RemoveHoles();

    /// remove hole for given line
    void RemoveHoleForLine( nTerrainLine * line );

#endif
    /// returns if there's a hole
    const bool GetHole( const int x, const int z ) const;

    /// sets a hole in the given coordinates
    void SetHole( int x, int z );

    /// creates a hole in the terrain (for persistung pourpose)
    nTerrainLine* CreateHole();
    /// adds the last created hole
    void AddLastHole();

    /// persist holes
    void SaveHoles();

    /// loads holes
    void Load();

private:
#ifndef NGAME
    /// stores the holes
    nArray<nTerrainLine*> holes;
#endif

    /// adds a hole to the cells
    void AddHoleInCells( nTerrainLine* polyLine );

    /// stores the last loaded nTerrainLine
    nTerrainLine* lastTerrainLine;

    /// stores the cells
    ncPhyTerrainCell** terrainCells;

    /// stores the num of cells in the terrain
    int numCellsPerSide;

    /// stores the total number of cells in the terrain
    int numTotalCells;

    /// prepares internal resources
    void Prepare();

    /// stores the length of a cell (squared)
    int lengthCell;

    /// stores the height map component
    nRef<nFloatMap> heightMap;    

    /// returns the main height map buffer
    const phyreal* GetBuffer() const;

    /// prepares the buffer data
    void PrepareBuffer( const phyreal* buffer );

    /// stores the buffer data
    const phyreal* bufferData;

    /// stores the space tha comprehends all the terrain space
    nPhyGeomHashSpace* terrainSpace;

    /// creates the space for this terrain
    void CreateSpace();

    /// loads a terrain
    void SetInfoHM( ncTerrainGMMClass* terrainGMMClass );

};

//-----------------------------------------------------------------------------
/**
    Returns the number of cells.

    @return cell number

    history:
        - 01-Jun-2005   David Reyes    created
*/
inline
int ncPhyTerrain::GetNumberOfCells() const
{
    return this->numTotalCells;
}

//-----------------------------------------------------------------------------
/**
    Returns a cell.

    @param index cell index
    @return terrain cell

    history:
        - 01-Jun-2005   David Reyes    created
*/
inline
ncPhyTerrainCell* ncPhyTerrain::GetTerrainCell( int index ) const
{
    n_assert2( index < this->GetNumberOfCells(), "Index out of bounds." );

#ifndef NDEBUG
    if( !this->terrainCells )
        return 0;
#endif

    return this->terrainCells[ index ];
}

//-----------------------------------------------------------------------------
/**
    Return a cell's lenght.

    @return cell lenght

    history:
        - 27-Jun-2005   David Reyes    created
*/
inline
int ncPhyTerrain::GetLenghtCell() const
{
    return this->lengthCell;
}

//-----------------------------------------------------------------------------
/**
    Access to heighmap info.

    @return associated heightmap

    history:
        - 28-Jun-2005   David Reyes    created
*/
inline
nFloatMap* ncPhyTerrain::GetHeightMapInfo() const
{
    return this->heightMap.get();
}

#endif 
