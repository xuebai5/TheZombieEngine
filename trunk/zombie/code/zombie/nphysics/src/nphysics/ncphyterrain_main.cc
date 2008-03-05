//-----------------------------------------------------------------------------
//  ncphyterrain_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyterrain.h"
#include "resource/nresourceserver.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "entity/nentityclass.h"
#include "nphysics/nphysicsserver.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nlevel/nlevelmanager.h"

#ifndef NGAME
#include "ngeomipmap/nterrainline.h"
#include "gfx2/nlineserver.h"
#endif

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
*/
ncPhyTerrain::ncPhyTerrain() :
    terrainCells(0),
    bufferData(0),
    terrainSpace(0),
    lengthCell(0),
    lastTerrainLine(0)
#ifndef NGAME
    ,holes(1,1)
#endif
{
    this->SetType( Terrain );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
*/
ncPhyTerrain::~ncPhyTerrain()
{
    /// destroying data copy
    if( this->bufferData )
    {
        this->bufferData = 0;
    }

    /// destroying terrain cels
    if( this->terrainCells )
    {
        n_delete_array( this->terrainCells );

        this->terrainCells = 0;
    }

    if( this->GetTerrainSpace() )
    {
        this->GetTerrainSpace()->Release();
        this->terrainSpace = 0;
    }

#ifndef NGAME
    // destroying holes editor information
    for( int index(0); index < this->GetNumHoles(); ++index )
    {
        this->GetHole( index )->Release();
    }

#endif
}

//-----------------------------------------------------------------------------
/**
    Loads a terrain

    @param terrainGMMClass terrain object

    history:
*/
void ncPhyTerrain::SetInfoHM( ncTerrainGMMClass* terrainGMMClass )
{
    n_assert2( terrainGMMClass, "Null pointer" );

    // retrieving the hightmap
    this->heightMap = terrainGMMClass->GetHeightMap();

    n_assert2( this->heightMap, "Failed to locate the height map." );

    // setting block's size
    this->lengthCell = terrainGMMClass->GetBlockSize()-1;
    
    // Prepares the terrain
    this->Prepare();
}

//-----------------------------------------------------------------------------
/**
    Prepares internal resources

    history:
*/
void ncPhyTerrain::Prepare()
{
    // computing number of cells by side (is assumed it's squared)
    this->numCellsPerSide = (this->heightMap->GetSize()-1) / lengthCell;

    int cellSizeSide(lengthCell);

    // check there's at least one cell
    if( !this->numCellsPerSide )
    {
        ++this->numCellsPerSide;
        cellSizeSide = this->heightMap->GetSize()-1;
        lengthCell = this->heightMap->GetSize()-1;
    }

    // computing number of totall cells
    this->numTotalCells = this->numCellsPerSide * this->numCellsPerSide;

    // allocating space for pointers to the cells
    this->terrainCells = n_new_array( ncPhyTerrainCell*, this->numTotalCells );

    n_assert2( this->terrainCells, "Error allocating space for terrain cells." );

    // prepares the memory buffer info
    this->PrepareBuffer( this->heightMap->GetHeightMap() );

    // obtaining spatial component
    ncSpatialQuadtree* spatial( this->GetComponent<ncSpatialQuadtree>());

    n_assert2( spatial, "Failed to locate the spatial component." );

    nArray<ncSpatialQuadtreeCell*>& aOutdoorCells( *spatial->GetLeafCells() );

    // creating cells
    for( int y(0), counter(0); y < this->numCellsPerSide; ++y )
    {
        for( int x(0); x < this->numCellsPerSide; ++x, ++counter )
        {       
            
            // creating an outdoor cell entity
            nEntityObject* ocell = aOutdoorCells[counter]->GetEntityObject();

            n_assert2( ocell, "Failed to create an outdoorcell entity." );

            // getting the physics component
            this->terrainCells[ counter ] = 
                ocell->GetComponent<ncPhyTerrainCell>();                            

            n_assert2( this->terrainCells[ counter ], "Failed to create cells." );

            // setting size of the cell
            this->terrainCells[ counter ]->SetSizeCell( lengthCell );

            // setting cell's terrain
            this->terrainCells[ counter ]->SetTerrain( this );

            // setting the start buffer
            this->terrainCells[ counter ]->SetBuffer( this->GetBuffer() + (x*lengthCell) + (y*this->heightMap->GetSize()*lengthCell) );

            // setting cell's length
            this->terrainCells[ counter ]->SetLength( this->heightMap->GetGridScale() * (cellSizeSide) );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the terrain in the world.

    @param world it's the world where the object will be come to existance

    history:
*/
void ncPhyTerrain::Create( nPhysicsWorld* world )
{
    n_assert2( world, "Null pointer" );
    
    // creating terrain space
    this->CreateSpace();

    // adding terrain space to the world's space
    world->GetSpace()->Add( this->GetTerrainSpace() );

    vector3 position(0,0,0);

    /// adding all the cells to the world
    for( int y(0), index(0); y < this->numCellsPerSide; ++y, position.z += lengthCell * this->heightMap->GetGridScale() )
    {
        for( int x(0); x < this->numCellsPerSide; ++x, ++index, position.x += lengthCell * this->heightMap->GetGridScale() )
        {
            /// adding the cells to the world
            world->Add( this->terrainCells[ index ]->GetEntityObject() );

            /// setting in position
            this->terrainCells[ index ]->SetPosition( position );

            /// setting the proper material for the cell
            this->terrainCells[ index ]->SetMaterial( this->GetMaterial() );

            /// sets the index for this cell
            this->terrainCells[ index ]->SetIndex( index );
        }

        position.x = 0;
    }

    this->SetCategories( this->terrainCells[ 0 ]->GetCategories() );
    this->SetCollidesWith( this->terrainCells[ 0 ]->GetCollidesWith() );

    ncPhysicsObj::Create( world );

    /// initing the object manager
    world->GetObjectManager()->Init();

#ifndef NGAME
    UpdateTerrainBoundingBoxes();
#endif
}


//-----------------------------------------------------------------------------
/**
    Returns the main height map buffer.

    @return phyreal's buffer

    history:
*/
const phyreal* ncPhyTerrain::GetBuffer() const
{
    return this->bufferData;
}

//-----------------------------------------------------------------------------
/**
    Prepares the buffer data.
    
    @param buffer phyreal's buffer

    history:
*/
void ncPhyTerrain::PrepareBuffer( const phyreal* buffer )
{    
    n_assert2( buffer, "Failed to prepare the buffer." );

    this->bufferData = buffer;
}

//-----------------------------------------------------------------------------
/**
    Creates the space for this terrain.

    history:
        - 19-May-2005   David Reyes     created
*/
void ncPhyTerrain::CreateSpace()
{
    this->terrainSpace = static_cast<nPhyGeomHashSpace*>(nKernelServer::Instance()->New( "nphygeomhashspace" ));

    n_assert2( this->terrainSpace, "Failed to create terrain space." );

    // Updating the world's space

    nPhyGeomQuadTreeSpace* worldSpace(
        static_cast<nPhyGeomQuadTreeSpace*>(nPhysicsServer::Instance()->GetDefaultWorld()->GetSpace()));

    if( worldSpace )
    {
        worldSpace->Release();
    }

    worldSpace = static_cast<nPhyGeomQuadTreeSpace*>(nKernelServer::Instance()->New( "nphygeomquadtreespace" ));

    nPhysicsServer::Instance()->GetDefaultWorld()->SetSpace( worldSpace );

    if( worldSpace->Id() == NoValidID )
    {
        ncSpatialQuadtree *quadtree = this->GetComponentSafe<ncSpatialQuadtree>();
        vector3 extents(quadtree->GetBBox().extents() * 2.f);

        extents *= phyreal(1.1); // increment a 10%

        static int depth( 4 );

        worldSpace->SetUp( quadtree->GetBBox().center(), extents, depth );

        worldSpace->Create();
    }
}


//-----------------------------------------------------------------------------
/**
    Returns the terrain space.

    history:
        - 19-May-2005   David Reyes     created
*/
nPhySpace* ncPhyTerrain::GetTerrainSpace()
{
    return this->terrainSpace;
}


//-----------------------------------------------------------------------------
/**
    Returns the total number of nodes.

    @return num nodes

    history:
        - 20-May-2005   David Reyes     Created
*/
int ncPhyTerrain::GetTotalNumNodes()  const
{
    return this->heightMap->GetSize();
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 20-May-2005   David Reyes     Created
*/
void ncPhyTerrain::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType != nObject::ReloadedInstance)
    {
        nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelSaved, 
                                            this,
                                            &ncPhyTerrain::SaveHoles,
                                            0);
    }

    ncPhysicsObj::InitInstance( initType );

    nEntityClass* outdoorClass( this->GetEntityClass() );

    n_assert2( outdoorClass, "Failed to initialize the component ncPhyTerrain." );

    ncTerrainGMMClass* terrainGMMClass(outdoorClass->GetComponent<ncTerrainGMMClass>());

    n_assert2( terrainGMMClass, "Failed to initialize the component ncPhyTerrain." );
}

//-----------------------------------------------------------------------------
/**
    Returns the bounding box of this geometry.

    @param boundingbox axis aligned bounding box

    history:
        - 02-Jun-2005   David Reyes     Created
*/
void ncPhyTerrain::GetAABB( nPhysicsAABB& boundingbox )
{
    n_assert2( this->GetNumberOfCells(), "Outdoor without data." );
        

    this->terrainCells[0]->GetAABB( boundingbox );

    for( int index(1); index < this->GetNumberOfCells(); ++index )
    {
        nPhysicsAABB AABB;

        this->terrainCells[index]->GetAABB( AABB );

        boundingbox += AABB;
    }
}

//-----------------------------------------------------------------------------
/**
    Enables the terrain.

    history:
        - 03-Jun-2005   David Reyes     Created
*/
void ncPhyTerrain::Enable()
{
    if( this->GetTerrainSpace() )
    {
        this->GetTerrainSpace()->Enable();
    }

    ncPhysicsObj::Enable();
}

//-----------------------------------------------------------------------------
/**
    Disables the terrain.

    history:
        - 03-Jun-2005   David Reyes     Created
*/
void ncPhyTerrain::Disable()
{
    if( this->GetTerrainSpace() )
    {
        this->GetTerrainSpace()->Disable();
    }

    ncPhysicsObj::Disable();
}


//-----------------------------------------------------------------------------
/**
    Sets the categories.

    @param categories categories flags

    history:
        - 03-Jun-2005   David Reyes     Created
*/
void ncPhyTerrain::SetCategories( int categories )
{
    for( int index(0); index < this->GetNumberOfCells(); ++index )
    {
        this->GetTerrainCell( index )->SetCategories( categories );
    }

    ncPhysicsObj::SetCategories( categories );
}

//-----------------------------------------------------------------------------
/**
    Sets the collisions relationship.

    @param categories categories flags

    history:
        - 03-Jun-2005   David Reyes     Created
*/
void ncPhyTerrain::SetCollidesWith( int categories )
{
    for( int index(0); index < this->GetNumberOfCells(); ++index )
    {
        this->GetTerrainCell( index )->SetCollidesWith( categories );
    }
    ncPhysicsObj::SetCollidesWith( categories );
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Updates the aabbs when the heightmap has changed.

    history:
        - 15-Jun-2005   David Reyes     Created
*/
void ncPhyTerrain::UpdateTerrainBoundingBoxes()
{
    for( int index(0); index < this->GetNumberOfCells(); ++index )
    {
        this->GetTerrainCell( index )->UpdateAABB();
    }
}
#endif

//-----------------------------------------------------------------------------
/**
    Adds a hole to the terrain.

    @param polyLine line defining an area

    @return polyLine actually added to the holes array if NGAME, 
            otherwise the polyLine passed as a param

    history:
        - 21-Jul-2005   David Reyes     Created
*/
nTerrainLine*
ncPhyTerrain::AddHole( nTerrainLine* polyLine )
{
    this->AddHoleInCells( polyLine );

#ifndef NGAME
    // copying hole
    nTerrainLine* clone( static_cast<nTerrainLine*>(polyLine->Clone()) );

    this->holes.Append( clone );

    this->GetEntityObject()->SetObjectDirty( true );

    return clone;
#endif

    return polyLine;
}

//-----------------------------------------------------------------------------
/**
    Adds a hole to the cells.

    @param polyLine line defining an area

    history:
        - 13-Jan-2006   David Reyes     Created
*/
void ncPhyTerrain::AddHoleInCells( nTerrainLine* polyLine )
{
    n_assert2( polyLine, "Null pointer." );

    bbox3 holeBB;

    polyLine->GetBBinXZ( holeBB ); 

    int firstPointX( int(holeBB.vmin.x / this->heightMap->GetGridScale()) );
    int lastPointX( int(holeBB.vmax.x / this->heightMap->GetGridScale()) );

    int firstPointZ( int(holeBB.vmin.z / this->heightMap->GetGridScale()) );
    int lastPointZ( int(holeBB.vmax.z / this->heightMap->GetGridScale()) );

    // checking the cells involved

    vector2 position(firstPointX * this->heightMap->GetGridScale(),0);

    for( int x(firstPointX); x < lastPointX + 1; ++x )
    {
        position.y = firstPointZ * this->heightMap->GetGridScale();

        for( int z(firstPointZ); z < lastPointZ + 1; ++z )
        {
            if( polyLine->IsPointInside( position ) )
            {
                this->SetHole( x,z );
            }

            position.y += this->heightMap->GetGridScale();
        }

        position.x += this->heightMap->GetGridScale();
    }

}

//-----------------------------------------------------------------------------
/**
    Sets a hole in the given coordinates.

    @param x x coordinate in a plane
    @param z z coordinate in a plane

    history:
        - 28-Jun-2005   David Reyes     Created
*/
void ncPhyTerrain::SetHole( int x, int z )
{
    n_assert2( x >= 0, "Out of range." );
    n_assert2( z >= 0, "Out of range." );
    n_assert2( x < this->GetTotalNumNodes(), "Out of range." );
    n_assert2( z < this->GetTotalNumNodes(), "Out of range." );

    /// finding cell
    int cellx( x / this->lengthCell);
    int cellz( z / this->lengthCell);

    this->GetTerrainCell( cellx + (cellz*this->numCellsPerSide) )->SetHole( x % this->lengthCell, z % this->lengthCell );

    /// checking overlaped areas
    int xMod(x % this->lengthCell);
    int zMod(z % this->lengthCell);

    /// checking horizontal
    if( !xMod )
    {
        if( cellx > 0 )
        {
            this->GetTerrainCell( (cellx-1) + (cellz*this->numCellsPerSide))->SetHole( this->lengthCell, zMod );
        }
    }

    /// checking horizontal
    if( !zMod )
    {
        if( cellz > 0 )
        {
            this->GetTerrainCell( cellx + ((cellz-1)*this->numCellsPerSide))->SetHole( xMod, this->lengthCell );
        }
    }

    /// checking diagonal
    if( !xMod && !zMod )
    {
        if( cellx > 0 && cellz > 0 )
        {
            this->GetTerrainCell( (cellx-1) + ((cellz-1)*this->numCellsPerSide))->SetHole( this->lengthCell, this->lengthCell );
        }
    }
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Removes all the holes.

    history:
        - 28-Jun-2005   David Reyes     Created
*/
void ncPhyTerrain::RemoveHoles()
{
    for( int index(0); index < this->GetNumberOfCells(); ++index )
    {
        this->GetTerrainCell( index )->RemoveHoles();
    }    
}

//-----------------------------------------------------------------------------
/**
    Returns the number of holes.

    @return num of holes in the terrain

    history:
        - 11-Jan-2006   David Reyes     Created
*/
const int ncPhyTerrain::GetNumHoles() const
{
    return this->holes.Size();
}

//-----------------------------------------------------------------------------
/**
    Returns a hole by index.

    @return a line defining a hole

    @param index index to the list of holes

    history:
        - 11-Jan-2006   David Reyes     Created
*/
nTerrainLine* ncPhyTerrain::GetHole( const int index ) const
{
    n_assert2( index < this->holes.Size(), "Index out of bounds." );

    return this->holes[ index ];
}

//-----------------------------------------------------------------------------
/**
    Remove hole by index.

    @param index index to the list of holes

    history:
        - 11-Jan-2006   David Reyes     Created
*/
void ncPhyTerrain::RemoveHole( const int index )
{
    n_assert2( index < this->holes.Size(), "Index out of bounds." );

    this->holes.Erase( index );

    for( int iindex(0); iindex < this->GetNumberOfCells(); ++iindex )
    {
        this->GetTerrainCell( iindex )->RemoveHoles();
    }

    for( int iindex(0); iindex < this->GetNumHoles(); ++iindex  )
    {
        this->AddHoleInCells( this->GetHole( iindex ) );
    }
}

//-----------------------------------------------------------------------------
/**
    Remove the hole represented by the given line.

    @param polyLine defining the hole

*/
void ncPhyTerrain::RemoveHoleForLine( nTerrainLine * line )
{
    n_assert2( line, "No line given." ); 

    int indexOfHole = this->GetIndexOfHole( line );

    n_assert2 ( indexOfHole != -1, "Hole not found" );

    this->RemoveHole( indexOfHole );
}
    
#endif

//-----------------------------------------------------------------------------
/**
    Creates a hole in the terrain (for persisting pourpose).

    @return a new terrain line object.

    history:
        - 12-Jan-2006   David Reyes     Created
*/
nTerrainLine* ncPhyTerrain::CreateHole()
{
    this->lastTerrainLine = static_cast<nTerrainLine*>(nKernelServer::Instance()->New( "nterrainline" ));

    nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(this->lastTerrainLine, nObject::LoadedInstance);

    return this->lastTerrainLine;
}

//-----------------------------------------------------------------------------
/**
    Adds the last created hole.

    history:
        - 12-Jan-2006   David Reyes     Created
*/
void ncPhyTerrain::AddLastHole()
{
    this->AddHole( this->lastTerrainLine );
    this->lastTerrainLine->Release();
    this->lastTerrainLine = 0;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the physic object.

    history:
        - 13-Jan-2006   David Reyes     Created
*/
void ncPhyTerrain::Draw( nGfxServer2* /*server*/ )
{
    // drawing holes
    if( !(nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyHeightMap) )
        return;

    static nLineHandler lineh( nGfxServer2::LineList, nMesh2::Coord | nMesh2::Color);

    static bool once(false);

    if( !once )
    {
        lineh.SetShader( "shaders:line_solid.fx" );
        once = true;
    }

    matrix44 modeltransform;

    static const int numLines(100);

    vector3 lines[numLines*2];

    for( int index(0); index < this->GetNumHoles(); ++index )
    {
        nTerrainLine* line( this->GetHole( index ) );

        n_assert2( line->GetNumVertices() < numLines*2-1, "Not enough buffer." );

        int counter = 0;

        for( int iindex(0); iindex < line->GetNumVertices(); ++iindex )
        {
            lines[ counter++ ] = line->GetVertexBuffer()[iindex];
            if( iindex + 1 >= line->GetNumVertices() )
                lines[ counter++ ] = line->GetVertexBuffer()[0];
            else
                lines[ counter++ ] = line->GetVertexBuffer()[iindex+1];
        }

        lineh.BeginLines(modeltransform);

        lineh.DrawLines3d(lines, 0, counter, phy_color_terrain_hole );

        lineh.EndLines();
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the index of the hole for the given line, 
    or -1 if no match is found
*/

int ncPhyTerrain::GetIndexOfHole( nTerrainLine * line ) const
{
    n_assert2( line, "No line given." );

    for( int index(0); index < this->GetNumHoles(); ++index )
    {
        if ( this->holes[index] == line )
        {
            return index;
        }
    }

    return -1;
}

#endif

/// returns if there's a hole
const bool ncPhyTerrain::GetHole( const int x, const int z ) const
{
    n_assert2( x >= 0, "Out of range." );
    n_assert2( z >= 0, "Out of range." );
    n_assert2( x < this->GetTotalNumNodes(), "Out of range." );
    n_assert2( z < this->GetTotalNumNodes(), "Out of range." );

    /// finding cell
    int cellx( x / this->lengthCell);
    int cellz( z / this->lengthCell);

    return this->GetTerrainCell( cellx + (cellz*this->numCellsPerSide) )->GetHole( x % this->lengthCell, z % this->lengthCell );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

