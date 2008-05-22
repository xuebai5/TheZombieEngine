//-----------------------------------------------------------------------------
//  phyobjmanager.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/phyobjmanager.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"

//-----------------------------------------------------------------------------
static bool initializedProfilers = false;
static nProfiler profPhysicsManagerRun;

//-----------------------------------------------------------------------------
//  ABC             
//  H*D clock-wise
//  GFE

const vector2 phyObjManager::direction[8] = {
    vector2(phyreal(.7),phyreal(.7)),
    vector2(0,phyreal(1)),
    vector2(phyreal(-.7),phyreal(.7)),
    vector2(phyreal(-1),0),
    vector2(phyreal(-.7),phyreal(-.7)),
    vector2(0,phyreal(-1)),
    vector2(phyreal(.7),phyreal(-.7)),
    vector2(phyreal(1),0)
};
const int phyObjManager::directionIndexes[8][2] = {
    {1,1},
    {0,1},
    {-1,1},
    {-1,0},
    {-1,-1},
    {0,-1},
    {1,-1},
    {1,0}
};

/// distance to activate an indoor
const phyreal phyObjManager::DistanceActivateIndoor(phyreal(45));

namespace 
{
void DrawRectangleArea( nGfxServer2* server, const vector2& offset, nPhysicsAABB& aabb, const vector4& color, bool filled, phyreal ratio )
{
    static const phyreal drawingOffset(10);

    vector2 offsetXY( offset );

    aabb.maxx /= ratio;
    aabb.minx /= ratio;
    aabb.maxz /= ratio;
    aabb.minz /= ratio;

    offsetXY.x /= ratio;
    offsetXY.y /= ratio;

    static vector2 rectangle[5];

    rectangle[0].x = drawingOffset+offsetXY.x;
    rectangle[0].y = drawingOffset+offsetXY.y;

    rectangle[1].x = aabb.maxx-aabb.minx + drawingOffset+offsetXY.x;
    rectangle[1].y = drawingOffset+offsetXY.y;

    rectangle[2].x = rectangle[1].x;
    rectangle[2].y = aabb.maxz-aabb.minz + drawingOffset+offsetXY.y;

    rectangle[3].x = drawingOffset+offsetXY.x;
    rectangle[3].y = rectangle[2].y;

    rectangle[4] = rectangle[0];

    server->BeginLines();

    server->DrawLines2d( rectangle, 5, color ); 

    server->EndLines();    

    if( !filled )
    {
        return;
    }

    // Drawing a cross
    rectangle[1] = rectangle[2];

    server->BeginLines();

    server->DrawLines2d( rectangle, 2, color ); 

    server->EndLines();    

    rectangle[2].x = aabb.maxx-aabb.minx + drawingOffset+offsetXY.x;
    rectangle[2].y = drawingOffset+offsetXY.y;

    server->BeginLines();

    server->DrawLines2d( &rectangle[2], 2, color ); 

    server->EndLines();    
}
}

//-----------------------------------------------------------------------------
/**
    Constructor

    @param world a physics world

    history:
        - 27-Jun-2005   Zombie        created
*/
phyObjManager::phyObjManager( nPhysicsWorld* world ) : 
    managedWorld( world ),
    outdoor(0),
    cellSize(0),
    cellDiagonalSquared(0),
    cellsPerSide(0),
    cellsPerSideMinusOne(0),
    pointDistance(0),
    indoors( NumInitialIndoors, NumGrowthPhysicsObj ),
    cellsLengthInteger(0),
    bitmap(0),
    oldbitmap(0)
{
    if (!initializedProfilers)
    {
        profPhysicsManagerRun.Initialize("profPhysicsManagerRun", true);
        initializedProfilers = true;
    }

    n_assert2( world, "The world it isn't valid." );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 27-Jun-2005   Zombie        created
*/
phyObjManager::~phyObjManager()
{
    if( this->bitmap )
    {
        n_delete_array( this->bitmap );
        this->bitmap = 0;
    }
    if( this->oldbitmap )
    {
        n_delete_array( this->oldbitmap );
        this->oldbitmap = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Initializes the manager.

    @return success

    history:
        - 27-Jun-2005   Zombie        created
*/
bool phyObjManager::Init()
{
#ifndef NGAME
    /// check if it's a manageable world
    if( !this->managedWorld->HasBodies() )
    {
        return false;
    }

#endif
    /// clearing the list of indoors
    this->indoors.Clear();

    /// look for indoors
    const nArray<ncSpatialIndoor*>& indoorSpaces(nSpatialServer::Instance()->GetIndoorSpaces());

    for( int index(0); index < indoorSpaces.Size(); ++index )
    {
        ncPhyIndoor* indoor( indoorSpaces[ index ]->GetComponent<ncPhyIndoor>() );

        this->indoors.PushBack( indoor );
    }

    /// finding the outdoor
    this->outdoor = nSpatialServer::Instance()->GetOutdoorEntity()->GetComponentSafe<ncPhyTerrain>();

    n_assert2( this->outdoor, "Outdoor Needed." );

    /// pre-computing cell size
    ncPhyTerrain* terrain(this->outdoor->GetComponent<ncPhyTerrain>());

    this->pointDistance = terrain->GetHeightMapInfo()->GetGridScale();

    n_assert2( terrain, "Missing component: ncPhyTerrain." );

    this->cellSize = phyreal(terrain->GetLenghtCell());

    this->cellsLengthInteger = int(this->cellSize*this->pointDistance);

    phyreal iValue(this->cellSize * pointDistance );

    this->cellDiagonalSquared = 
        iValue * iValue + 
        iValue * iValue;

    this->cellsPerSide = int(sqrt(phyreal(this->outdoor->GetComponentSafe<ncPhyTerrain>()->GetNumberOfCells())));

    this->cellsPerSideMinusOne = this->cellsPerSide - 1;

#ifndef NGAME
    this->pixelRatio = this->cellsPerSide * this->cellSize / phyreal(128);
#endif

    if( this->bitmap )
    {
        n_delete_array( this->bitmap );
        this->bitmap = 0;
    }

    this->bitmap = n_new_array( char, sizeof( char ) * terrain->GetNumberOfCells() );

    memset( this->bitmap, 0,  sizeof( char ) * terrain->GetNumberOfCells() );

    if( this->oldbitmap )
    {
        n_delete_array( this->oldbitmap );
        this->oldbitmap = 0;
    }

    this->oldbitmap = n_new_array( char, sizeof( char ) * terrain->GetNumberOfCells() );

    memset( this->oldbitmap, 0xFF,  sizeof( char ) * terrain->GetNumberOfCells() );

    return true;
}

//-----------------------------------------------------------------------------
/**
    Manage physics areas.

    history:
        - 24-Jun-2005   Zombie        created
*/
void phyObjManager::ManagePhysicsAreas()
{
#ifndef NGAME
    nPhysicsWorld* world(nPhysicsServer::Instance()->GetDefaultWorld());

    if( !world )
    {
        return;
    }

    if( !world->GetNumPhysicObjs() )
    {
        return;
    }

    /// check if it's a manageable world
    if( !this->managedWorld->HasBodies() )
    {
        return;
    }

    if( !this->outdoor )
    {
        return;
    }

#endif

    // disabling indoor spaces
    for( int index(0); index < this->indoors.Size(); ++index )
    {
        ncPhyIndoor* indoor(this->indoors[ index ]);

        indoor->GetIndoorSpace()->HoldOutOfSpace();
    }

    this->UpdateDynamicObjects();

    this->UpdateBitmapInformation();
}

//-----------------------------------------------------------------------------
/**
    Processes a hot point.

    @param hotpoint world position
    @param dir direction in the playe x/z

    history:
        - 24-Jun-2005   Zombie        created
        - 08-Mar-2006   Zombie        removed direction hot point
        - 10-Aug-2006   Carles Ros    indoor's aabb expanded to check for hot point nearness
*/
void phyObjManager::ProcessRadialHotPoint( const vector3& hotpoint )
{
    // find hotpoint's cell
    float cellxReal(hotpoint.x / this->cellsLengthInteger);

    float cellyReal(hotpoint.z / this->cellsLengthInteger);

    const int cellx( (int)cellxReal );
    const int celly( (int)cellyReal );
    
    if( cellx >= 0 && cellx < this->cellsPerSide )
    {
        if( celly >= 0 && celly < this->cellsPerSide )
        {
            // inside the world
            const int cellIndex(celly * this->cellsPerSide + cellx);
            ncPhyTerrainCell* cell(this->outdoor->GetTerrainCell( cellIndex ));

            this->bitmap[cellIndex] |= StaticSpace;

            if( cell->GetDynamicCellSpace()->GetNumGeometries() )
                this->bitmap[cellIndex] |= DynamicSpace;

            float frestx(floor( cellxReal ));
            float fresty(floor( cellyReal ));

            cellxReal -= frestx;
            cellyReal -= fresty;

            /// around cell
            // abc
            // d e
            // fgh
            if( cellxReal < 0.1 )
            {
                // add (d)
                this->UpdateBitmapCell( cellx - 1, celly );

                if( cellyReal < 0.1 )
                {
                    // add(b)
                    this->UpdateBitmapCell( cellx, celly-1 );
                    // add(a)
                    this->UpdateBitmapCell( cellx-1, celly-1 );
                }
                else if( cellyReal > 0.9 )
                {
                    // add(g)
                    this->UpdateBitmapCell( cellx, celly+1 );
                    // add(f)
                    this->UpdateBitmapCell( cellx-1, celly+1 );
                }
            } 
            else if( cellxReal > 0.9 )
            {
                // add(e)
                if( fresty < 0.1 )
                {
                    // add(b)
                    this->UpdateBitmapCell( cellx, celly-1 );
                    // add(c)
                    this->UpdateBitmapCell( cellx+1, celly-1 );
                }
                else if( cellyReal > 0.9 )
                {
                    // add(g)
                    this->UpdateBitmapCell( cellx, celly+1 );
                    // add(h)
                    this->UpdateBitmapCell( cellx+1, celly+1 );
                }
            }
            else if( cellyReal < 0.1 )
            {
                // add(b)
                this->UpdateBitmapCell( cellx, celly-1 );
            }
            else if( cellyReal > 0.9 )
            {
                // add(g)
                this->UpdateBitmapCell( cellx, celly+1 );                
            }
            
        }
    }

    nPhysicsAABB aabb;

    for( int index(0); index < this->indoors.Size(); ++index )
    {
        ncPhyIndoor* indoor(this->indoors[index]);

        nPhySpace* space( indoor->GetIndoorSpace() );
        
        if( space->SpaceId() != NoValidID )
            continue; // already connected
    
        space->GetAABB( aabb );
        aabb.minx -= DistanceActivateIndoor;
        aabb.miny -= DistanceActivateIndoor;
        aabb.minz -= DistanceActivateIndoor;
        aabb.maxx += DistanceActivateIndoor;
        aabb.maxy += DistanceActivateIndoor;
        aabb.maxz += DistanceActivateIndoor;

        if( aabb.IsInside( hotpoint.x, hotpoint.y, hotpoint.z ) )
        {
            space->ReconnectHoldSpace();
            continue;
        }

    }
}
//-----------------------------------------------------------------------------
/**
    Runs the object manager.

    @return success

    history:
        - 24-Jun-2005   Zombie        created
*/
bool phyObjManager::Run() 
{
    profPhysicsManagerRun.StartAccum();
    this->ManagePhysicsAreas();
    profPhysicsManagerRun.StopAccum();
    return true;
}

//-----------------------------------------------------------------------------
/**
    Restores the world collision.

    history:
        - 24-Jun-2005   Zombie        created
*/
void phyObjManager::Restore()
{
    nPhysicsWorld* world(nPhysicsServer::Instance()->GetDefaultWorld());

    if( !world )
    {
        return;
    }

    if( !world->GetNumPhysicObjs() )
    {
        return;
    }

    if( !this->outdoor )
    {
        return;
    }

    ncPhyTerrainCell* cell;

    for( int index(0); index < this->outdoor->GetNumberOfCells(); ++index )
    {
        cell = this->outdoor->GetTerrainCell(index);

        if( cell->GetCellSpace()->SpaceId() == NoValidID )
        {
            cell->GetCellSpace()->ReconnectHoldSpace();
            cell->GetDynamicCellSpace()->ReconnectHoldSpace();
        }
    }

    ncPhyIndoor* indoor;

    for( int index(0); index < this->indoors.Size(); ++index )
    {
        indoor = this->indoors[ index ];
        
        if( indoor->GetIndoorSpace()->SpaceId() == NoValidID )
        {
            indoor->GetIndoorSpace()->ReconnectHoldSpace();
        }
    }

    memset( this->bitmap, 0,  sizeof( char ) * this->outdoor->GetNumberOfCells() );
    memset( this->oldbitmap, 0xFF,  sizeof( char ) * this->outdoor->GetNumberOfCells() );
}

//-----------------------------------------------------------------------------
/**
    Updates dynamic objects spaces.

    history:
        - 02-Jul-2005   Zombie         created
*/
void phyObjManager::UpdateDynamicObjects()
{
    nPhysicsWorld::tContainerRigidBodies const& container(this->managedWorld->GetRigidBodies());

    for( int index(0); index < this->managedWorld->GetRigidBodies().Size(); ++index )
    {
        nPhyRigidBody* body(container.GetElementAt( index ));

        n_assert2( body, "Data corruption." );

        if( !body->IsEnabled() )
        {
            continue;
        }

        ncPhysicsObj* obj( body->GetPhysicsObj() );

        nPhySpace* space(obj->GetParentSpace());

        n_assert2( space, "Data corruption." );

        if( !space->IsEnabled() )
        {
            continue;
        }

        // auto-insert in a dynamic space
        vector3 position;

        body->GetPosition( position );

        // find hotpoint's cell
        const int cellx(int(position.x) / this->cellsLengthInteger);

        const int celly(int(position.z) / this->cellsLengthInteger);

        if( cellx >= 0 && cellx < this->cellsPerSide )
        {
            if( celly >= 0 && celly < this->cellsPerSide )
            {
                ncPhyTerrainCell* cell(this->outdoor->GetTerrainCell( celly * this->cellsPerSide + cellx ));

                nPhySpace* space(cell->GetDynamicCellSpace());

                if( obj->GetParentSpace() != space )
                {
                    obj->MoveToSpace( space );
                }
            }
            else
            {
                if( obj->GetParentSpace() != obj->GetWorld()->GetSpace() )
                {
                    obj->Alienate();
                }
            }
        }
        else
        {
            if( obj->GetParentSpace() != obj->GetWorld()->GetSpace() )
            {
                obj->Alienate();
            }
        }

        this->ProcessRadialHotPoint( position );
    }    
}

//-----------------------------------------------------------------------------
/**
    Inserts an object to the static spaces.

    @param obj a physic's object

    history:
        - 02-Jul-2005   Zombie         created
*/
void phyObjManager::Insert( ncPhysicsObj* obj )
{
    // auto-inser in a dynamic space
    vector3 position;

    obj->GetPosition( position );

    // find hotpoint's cell
    const int cellx(int(position.x) / this->cellsLengthInteger);

    const int celly(int(position.z) / this->cellsLengthInteger);

    if( cellx >= 0 && cellx < this->cellsPerSide )
    {
        if( celly >= 0 && celly < this->cellsPerSide )
        {
            ncPhyTerrainCell* cell(this->outdoor->GetTerrainCell( celly * this->cellsPerSide + cellx ));

            nPhySpace* space(cell->GetCellSpace());

            if( obj->GetParentSpace() != space )
            {
                obj->MoveToSpace( space );
            }
        }
        else
        {
            if( obj->GetParentSpace() != obj->GetWorld()->GetSpace() )
            {
                obj->Alienate();
            }
        }
    }
    else
    {
        if( obj->GetParentSpace() != obj->GetWorld()->GetSpace() )
        {
            obj->Alienate();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Updates the bitmap information.

    history:
        - 03-Jul-2005   Zombie         created
*/
void phyObjManager::UpdateBitmapInformation()
{
    const char *bitmapPtr( this->bitmap );
    const char *oldbitmapPtr( this->oldbitmap );

    ncPhyTerrainCell* cell;

    nPhySpace* space;

    for( int index(0); index < this->outdoor->GetNumberOfCells(); index+=4 )
    {
        // unrolled loop by 4
        if( *bitmapPtr != *oldbitmapPtr )
        {
            cell = this->outdoor->GetTerrainCell(index);
            
            space = cell->GetCellSpace();

            if( *bitmapPtr & StaticSpace )
            {
                if( space->SpaceId() == NoValidID )
                {
                    space->ReconnectHoldSpace();
                }
            }
            else
            {
                if( space->SpaceId() != NoValidID )
                {
                    space->HoldOutOfSpace();
                }
            }

            space = cell->GetDynamicCellSpace();

            if( *bitmapPtr & DynamicSpace )
            {
                if( space->SpaceId() == NoValidID )
                {
                    space->ReconnectHoldSpace();
                }
            }
            else
            {
                if( space->SpaceId() != NoValidID )
                {
                    space->HoldOutOfSpace();
                }
            }
        }
        
        ++bitmapPtr;
        ++oldbitmapPtr;

        if( *bitmapPtr != *oldbitmapPtr )
        {
            cell = this->outdoor->GetTerrainCell(index+1);
            
            space = cell->GetCellSpace();

            if( *bitmapPtr & StaticSpace )
            {
                if( space->SpaceId() == NoValidID )
                {
                    space->ReconnectHoldSpace();
                }
            }
            else
            {
                if( space->SpaceId() != NoValidID )
                {
                    space->HoldOutOfSpace();
                }
            }

            space = cell->GetDynamicCellSpace();

            if( *bitmapPtr & DynamicSpace )
            {
                if( space->SpaceId() == NoValidID )
                {
                    space->ReconnectHoldSpace();
                }
            }
            else
            {
                if( space->SpaceId() != NoValidID )
                {
                    space->HoldOutOfSpace();
                }
            }
        }
        
        ++bitmapPtr;
        ++oldbitmapPtr;

        if( *bitmapPtr != *oldbitmapPtr )
        {
            cell = this->outdoor->GetTerrainCell(index+2);
            
            space = cell->GetCellSpace();

            if( *bitmapPtr & StaticSpace )
            {
                if( space->SpaceId() == NoValidID )
                {
                    space->ReconnectHoldSpace();
                }
            }
            else
            {
                if( space->SpaceId() != NoValidID )
                {
                    space->HoldOutOfSpace();
                }
            }

            space = cell->GetDynamicCellSpace();

            if( *bitmapPtr & DynamicSpace )
            {
                if( space->SpaceId() == NoValidID )
                {
                    space->ReconnectHoldSpace();
                }
            }
            else
            {
                if( space->SpaceId() != NoValidID )
                {
                    space->HoldOutOfSpace();
                }
            }
        }
        
        ++bitmapPtr;
        ++oldbitmapPtr;

        if( *bitmapPtr != *oldbitmapPtr )
        {
            cell = this->outdoor->GetTerrainCell(index+3);
            
            space = cell->GetCellSpace();

            if( *bitmapPtr & StaticSpace )
            {
                if( space->SpaceId() == NoValidID )
                {
                    space->ReconnectHoldSpace();
                }
            }
            else
            {
                if( space->SpaceId() != NoValidID )
                {
                    space->HoldOutOfSpace();
                }
            }

            space = cell->GetDynamicCellSpace();

            if( *bitmapPtr & DynamicSpace )
            {
                if( space->SpaceId() == NoValidID )
                {
                    space->ReconnectHoldSpace();
                }
            }
            else
            {
                if( space->SpaceId() != NoValidID )
                {
                    space->HoldOutOfSpace();
                }
            }
        }
        
        ++bitmapPtr;
        ++oldbitmapPtr;
    }

    memcpy( this->oldbitmap, this->bitmap, sizeof( char ) * this->outdoor->GetNumberOfCells() );
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws a map of areas enabled/disabled.

    @param server graphics server

    history:
        - 16-Jun-2005   Zombie         created
*/
void phyObjManager::DrawAreas( nGfxServer2* server )
{
    if( ! nSpatialServer::Instance()->GetOutdoorEntity() )
    {
        return;
    }

    // Drawing outdoor
    ncPhyTerrain* outdoor = nSpatialServer::Instance()->GetOutdoorEntity()->GetComponentSafe<ncPhyTerrain>();


    if( !outdoor )
        return;

    nPhysicsAABB outdoorAABB;

    outdoor->GetAABB( outdoorAABB );

    // drawing terrain cells
    nPhysicsAABB cellAABB;

    for( int index(0); index < outdoor->GetNumberOfCells(); ++index )
    {
        ncPhyTerrainCell* cell( outdoor->GetTerrainCell( index ) );

        cell->GetCellSpace()->GetAABB( cellAABB );

        if( cell->GetCellSpace()->SpaceId() )
        {
            // draws cells
            DrawRectangleArea( server, vector2(cellAABB.minx-outdoorAABB.minx,cellAABB.minz-outdoorAABB.minz),cellAABB, phy_color_heightmap, true, this->pixelRatio );
        }
        else
        {
            // draws cells
            // DrawRectangleArea( server, vector2(cellAABB.minx-outdoorAABB.minx,cellAABB.minz-outdoorAABB.minz),cellAABB, phy_color_heightmap_disabled, false, this->pixelRatio );
        }
    }

    nPhysicsAABB indoorAABB;

    // Drawing indoors
    for( int index(0); index < this->indoors.Size(); ++index )
    {
        ncPhyIndoor* indoor( this->indoors[ index ] );

        indoor->GetIndoorSpace()->GetAABB( indoorAABB );

        if( indoor->GetIndoorSpace()->SpaceId() )
        {
            DrawRectangleArea( server, vector2(indoorAABB.minx-outdoorAABB.minx,indoorAABB.minz-outdoorAABB.minz),indoorAABB, phy_color_space, false, this->pixelRatio );
        }
        else
        {
            DrawRectangleArea( server, vector2(indoorAABB.minx-outdoorAABB.minx,indoorAABB.minz-outdoorAABB.minz),indoorAABB, phy_color_space_disabled, false, this->pixelRatio );
        }
    }
}

#endif

void phyObjManager::UpdateBitmapCell( const int cellx, const int celly )
{
    if( cellx < 0 )
        return;
    if( cellx > this->cellsPerSideMinusOne )
        return;

    if( celly < 0 )
        return;
    if( celly > this->cellsPerSideMinusOne )
        return;

    const int cellIndex(celly * this->cellsPerSide + cellx);

    ncPhyTerrainCell* cell(this->outdoor->GetTerrainCell( cellIndex ));

    if( cell->GetCellSpace()->SpaceId() != NoValidID )
        return;

    this->bitmap[cellIndex] |= StaticSpace;

    if( cell->GetDynamicCellSpace()->GetNumGeometries() )
        this->bitmap[cellIndex] |= DynamicSpace;
}

//-----------------------------------------------------------------------------
/**
    Resets the state.
    
    history:
        - 18-Jul-2005   Zombie         created
*/
void phyObjManager::Reset()
{
    if( this->outdoor )
    {
        /// emptying bitmap
        memset( this->bitmap, 0,  sizeof( char ) * this->outdoor->GetNumberOfCells() );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the visible cell.

    @param index cell index
    @param dynamic if the dynamic space has any geometry
    
    history:
        - 18-Jul-2005   Zombie         created
*/
void phyObjManager::SetVisibleCell( const unsigned index, const bool dynamic )
{
    if( dynamic )
    {
        this->bitmap[ index ] = StaticSpace | DynamicSpace;
    }
    else
    {
        this->bitmap[ index ] = StaticSpace;
    }
}
