//-----------------------------------------------------------------------------
//  ncphyterraincell_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

#include "nphysics/nphygeomheightmap.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyTerrainCell,ncPhySimpleObj);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 08-Apr-2005   Zombie         created
*/
ncPhyTerrainCell::ncPhyTerrainCell() :
    terrainCell(0),
    cellSpace(0),
    cellDynamicSpace(0),
    index(~0u)
{
    this->SetType( Terrain );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 08-Apr-2005   Zombie         created
*/
ncPhyTerrainCell::~ncPhyTerrainCell()
{
    if( this->cellSpace )
    {
        if( this->cellSpace->SpaceId() == NoValidID )
        {
            this->cellSpace->ReconnectHoldSpace();
        }
        this->cellSpace->Release();

        this->cellSpace = 0;

        if( this->cellDynamicSpace->SpaceId() == NoValidID )
        {
            this->cellDynamicSpace->ReconnectHoldSpace();
        }
        this->cellDynamicSpace->Release();

        this->cellDynamicSpace = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Enables the cell.

    history:
        - 08-Apr-2005   Zombie         created
*/
void ncPhyTerrainCell::Enable()
{
    ncPhySimpleObj::Enable();
}

//-----------------------------------------------------------------------------
/**
    Disables the cell.

    history:
        - 08-Apr-2005   Zombie         created
*/
void ncPhyTerrainCell::Disable()
{
    ncPhySimpleObj::Disable();
}

//-----------------------------------------------------------------------------
/**
    Creates the object.

    @param world it's the world where the object will be come to existance

    history:
        - 08-Apr-2005   Zombie         created
*/
void ncPhyTerrainCell::Create( nPhysicsWorld* world )
{
    n_assert2( world, "Null pointer." );

    nPhyGeomHeightMap* heightmap( 
        static_cast<nPhyGeomHeightMap*>(nKernelServer::Instance()->New( "nphygeomheightmap" ) )
        );

    n_assert2( heightmap, "Failed to create a terrain cell." );

    heightmap->SetLength( this->cellLength );

    heightmap->SetHeightsBuffer( const_cast<phyreal*>(this->dataBuffer) );
    
    heightmap->SetNumNodes( this->cellSizeSize );

    heightmap->SetStep( this->terrainCell->GetTotalNumNodes() );

    heightmap->Create();

    heightmap->SetCategories( nPhysicsGeom::Static );

    heightmap->SetCollidesWith( nPhysicsGeom::Dynamic | nPhysicsGeom::Check );

    this->SetGeometry( heightmap );

    this->CreateSpace();

    terrainCell->GetTerrainSpace()->Add( this->cellSpace );

    this->MoveToSpace( this->cellSpace );
    
    ncPhySimpleObj::Create( world );

    this->SetCollidesWith( heightmap->GetCollidesWith() );
    this->SetCategories( heightmap->GetCategories() );

    // adds dynamic space to the world (may be better to the terrain???)
    this->GetWorld()->GetSpace()->Add(this->GetDynamicCellSpace());
}

//-----------------------------------------------------------------------------
/**
    Sets the terrain where this cell belongs.

    @param terrain terrain

    history:
        - 11-Apr-2005   Zombie         created
*/
void ncPhyTerrainCell::SetTerrain( ncPhyTerrain* terrain )
{
    n_assert2( terrain, "Null pointer." );

    this->terrainCell = terrain;
}

//-----------------------------------------------------------------------------
/**
    Sets the size of the cell.

    @param cellSize cell's size

    history:
        - 11-Apr-2005   Zombie         created
*/
void ncPhyTerrainCell::SetSizeCell( const int cellSize )
{
    n_assert2( cellSize > 0, "Invalid cell size value." );

    this->cellSizeSize = cellSize;
}

//-----------------------------------------------------------------------------
/**
    Sets the cell data buffer.

    @param buffer phyreal's buffer

    history:
        - 11-Apr-2005   Zombie         created
*/
void ncPhyTerrainCell::SetBuffer( const phyreal* buffer )
{
    this->dataBuffer = buffer;
}

//-----------------------------------------------------------------------------
/**
    Sets the length of the cell.

    @param length terrain cell length

    history:
        - 11-Apr-2005   Zombie         created
*/
void ncPhyTerrainCell::SetLength( const phyreal length )
{
    this->cellLength = length;
}

//-----------------------------------------------------------------------------
/**
    Creates the cell's space.

    history:
        - 11-Apr-2005   Zombie         created
*/
void ncPhyTerrainCell::CreateSpace()
{
    this->cellSpace = static_cast<nPhyGeomHashSpace*>(nKernelServer::Instance()->New( "nphygeomhashspace" ));

    n_assert2( this->cellSpace, "Failed to create the cell's space." );

    this->MoveToSpace( this->cellSpace );

    this->cellDynamicSpace = static_cast<nPhyGeomHashSpace*>(nKernelServer::Instance()->New( "nphygeomhashspace" ));

    n_assert2( this->cellDynamicSpace, "Failed to create dynamic cell's space." );

    this->cellDynamicSpace->SetCategories( nPhysicsGeom::Dynamic );
    this->cellDynamicSpace->SetCollidesWith( nPhysicsGeom::All );
}



//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 01-Jun-2005   Zombie         created
*/ 
void ncPhyTerrainCell::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhySimpleObj::InitInstance(initType);
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Updates the bounding box.

    history:
        - 15-Jun-2005   Zombie         created
*/ 
void ncPhyTerrainCell::UpdateAABB()
{
    static_cast<nPhyGeomHeightMap*>(this->GetGeometry())->UpdateAABB();
}

#endif

//-----------------------------------------------------------------------------
/**
    Sets a hole in the given coordinate.

    @param x x coordinate in a plane
    @param z z coordinate in a plane

    history:
        - 28-Jun-2005   Zombie         created
*/
void ncPhyTerrainCell::SetHole( int x, int z )
{
    static_cast<nPhyGeomHeightMap*>(this->GetGeometry())->SetHole( x, z );
}

//-----------------------------------------------------------------------------
/**
    Removes all the holes.

    history:
        - 28-Jun-2005   Zombie         created
*/
void ncPhyTerrainCell::RemoveHoles()
{
    static_cast<nPhyGeomHeightMap*>(this->GetGeometry())->RemoveHoles();
}

//-----------------------------------------------------------------------------
/**
    Gets a hole in the given coordinate.

    @param x x coordinate in a plane
    @param z z coordinate in a plane

    @return if hole present

    history:
        - 13-Jan-2006   Zombie         created
*/
const bool ncPhyTerrainCell::GetHole( const int x, const int z ) const
{
    return static_cast<nPhyGeomHeightMap*>(this->GetGeometry())->GetHole( x, z );
}

//-----------------------------------------------------------------------------
/**
    Returns if the cell has any hole.

    @return if hole present

    history:
    - 01-Feb-2006   Zombie         created
*/
const bool ncPhyTerrainCell::IsThereAnyHole() const
{
    return phyHasTerrainYHoles(this->GetGeometry()->Id());
}

//-----------------------------------------------------------------------------
/**
    Marks the cell as visible.

    history:
    - 18-Jul-2006   Zombie         created
*/
void ncPhyTerrainCell::SetVisible()
{
    if( this->GetIndex() == ~0u )
    {
        return; // leave nothing to do
    }

    this->GetWorld()->GetObjectManager()->SetVisibleCell( this->GetIndex(), bool(this->GetDynamicCellSpace()->GetNumGeometries() > 0) );
}

//-----------------------------------------------------------------------------
/**
    Sets the cell index.

    @param indexCell index associated to this cell

    history:
    - 18-Jul-2006   Zombie         created
*/
void ncPhyTerrainCell::SetIndex( const unsigned indexCell )
{
    this->index = indexCell;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
