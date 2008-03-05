//-----------------------------------------------------------------------------
//  nnavutils_main.cc
//  (C) 2006 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnnavmesh.h"
#include "nnavmesh/nnavutils.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphysicsgeom.h"
#include "nphysics/ncphysicsobj.h"
#include "nspatial/nspatialserver.h"
#include "ngeomipmap/ncterraingmmclass.h"

//-----------------------------------------------------------------------------
namespace nNavUtils
{

//-----------------------------------------------------------------------------
// To cast the ray used to found the ground height below a position, this offset is added to the position
// as the start point of the ray so ground bump and small obstacles are climbed correctly
// (but the value should not be too high or otherwise there's the risk to "climb up" to the top floor)
// History:
//     - 2006/03/15    1.0 used as initial ground height offset value
//     - 2006/05/08    1.0 has proven to be a way too much and can cause agents
//                     to climb obstacles too high for them. Changed to 0.5
//     - 2006/08/08    Moved to global variable server as
//                     -GroundSeekStartDistance
//const float GroundHeightOffset( 0.5f );
// Max distance to look for the ground below a position
// History:
//     - 2006/03/15    It's recommended that ray offset above floor is greater
//                     than below so if the agent accidentally "climbs down"
//                     to a lower floor when walking over a small hole, it has
//                     a chance to return back.
//                     Value = 0.5 + ground height offset
//     - 2006/05/08    With the addition of physics ramp category now it's
//                     rather desired to have a greater offset value below the
//                     floor to allow agent to "climb down" and let designer
//                     handle special cases caused by some small holes.
//                     Value increased to 1.0 + ground height offset
//     - 2006/05/16    Value increased to 4.0 + ground height offset
//                     This value forces aliens to climb down when they
//                     accidentally climb up ramps due to pathfinding not
//                     considering the alien body size yet.
//     - 2006/08/08    Moved to global variable server as
//                     GroundSeekEndDistance - GroundSeekStartDistance
//const float GroundSeekDistance( 4.0f + GroundHeightOffset );

//------------------------------------------------------------------------------
/**
    Get the height of the closest ground below the give point

    The ground is any static or ramp physics surface found by a ray with origin
    slightly above the given point and casted down with some predefined length.
    The highest contact point is the returned height.

    If no ground is found, the height of the given point is returned. Anyway,
    don't assume this to be the default behavior forever, it's just a
    convenience value that can change in a future (even to a value so different
    such as -FLT_MAX for instance). So rather check the boolean value returned
    for a more robust usage.

    Global variables GroundSeekStartDistance and GroundSeekEndDistance are used
    to build the ray pointing downwards. Distances are relative to the given
    point's y:

        ray start = point.y - GroundSeekStartDistance,
        ray end = point.y + GroundSeekEndDistance

    @param point Point for which a ground is looked for
    @retval groundHeight Height of the closest ground found
    @return True if a ground has been found, false otherwise
*/
bool
GetGroundHeight( const vector3& point, float& groundHeight )
{
    // Cast a ray slightly above the given position and towards the ground
    // to find the height of the ground below the position
    const float GroundSeekStart = nVariableServer::Instance()->GetFloatVariable("GroundSeekStartDistance");
    const float GroundSeekEnd = nVariableServer::Instance()->GetFloatVariable("GroundSeekEndDistance");
    const float GroundSeekDistance = GroundSeekEnd - GroundSeekStart;
    vector3 origin(point);
    origin.y -= GroundSeekStart;
    const int MaxContacts( 10 );
    nPhyCollide::nContact contact[ MaxContacts ];
    int contactsNumber( nPhysicsServer::Collide( origin, vector3(0,-1,0), GroundSeekDistance,
        MaxContacts, contact, nPhysicsGeom::Static | nPhysicsGeom::Ramp ) );
    if ( contactsNumber == 0 )
    {
        // No ground found -> return given height as ground level
        groundHeight = point.y;
        return false;
    }

    // Take the closest contact as the ground level
    float closestHeight( -FLT_MAX );
    for ( int c(0); c < contactsNumber; ++c )
    {
        vector3 contactPos;
        contact[c].GetContactPosition( contactPos );
        if ( contactPos.y > closestHeight )
        {
            closestHeight = contactPos.y;
        }
    }
    groundHeight = closestHeight;
    return true;
}

//------------------------------------------------------------------------------
/**
    Get the height of the closest ground physics geometry below the given point

    This method works like GetGroundHeight, but skiping those objects that
    aren't marked as ground. The terrain is considered as a ground too.

    @param point Point for which a ground is looked for
    @retval groundHeight Height of the closest ground found
    @return True if a ground has been found, false otherwise
    @see GetGroundHeight
    @todo Mix this and GetGroundHeight into a single method to simplify mantenaince
*/
bool
GetGroundLevelHeight( const vector3& point, float& groundHeight )
{
    // Cast a ray slightly above the given position and pointing down
    // to find the height of the ground below the position
    const float GroundSeekStart = nVariableServer::Instance()->GetFloatVariable("GroundSeekStartDistance");
    const float GroundSeekEnd = nVariableServer::Instance()->GetFloatVariable("GroundSeekEndDistance");
    const float GroundSeekDistance = GroundSeekEnd - GroundSeekStart;
    vector3 origin(point);
    origin.y -= GroundSeekStart;
    const int MaxContacts( 10 );
    nPhyCollide::nContact contact[ MaxContacts ];
    int contactsNumber( nPhysicsServer::Collide( origin, vector3(0,-1,0),
        GroundSeekDistance, MaxContacts, contact, nPhysicsGeom::Static ) );

    // Take the closest ground contact as the ground level,
    // ignoring non ground objects
    bool groundFound( false );
    float closestHeight( -FLT_MAX );
    for ( int c(0); c < contactsNumber; ++c )
    {
        if ( contact[c].GetGeometryA()->HasAttributes( nPhysicsGeom::ground ) ||
             contact[c].GetGeometryA()->Type() == nPhysicsGeom::HeightMap ||
             contact[c].GetGeometryB()->HasAttributes( nPhysicsGeom::ground ) ||
             contact[c].GetGeometryB()->Type() == nPhysicsGeom::HeightMap )
        {
            vector3 contactPos;
            contact[c].GetContactPosition( contactPos );
            if ( contactPos.y > closestHeight )
            {
                closestHeight = contactPos.y;
                groundFound = true;
            }
        }
    }

    // Return closest ground found and if a ground has been found at all
    if ( !groundFound )
    {
        // No ground found -> return given height as ground level
        groundHeight = point.y;
    }
    else
    {
        // Ground found -> return height of closest ground
        groundHeight = closestHeight;
    }
    return groundFound;
}

//------------------------------------------------------------------------------
/**
    Check if there's a clear LoS between two points

    Only static, ramps and stairs objects (excluding the terrain) can block LoS.
*/
bool
IsLineOfSight( const vector3& source, const vector3& target )
{
    // Build LoS
    vector3 los( target - source );
    float losLength( los.len() );
    if ( losLength <= TINY )
    {
        // No need for LoS test if 0 long ray
        return true;
    }
    los.norm();

    // Test for LoS
    const int MaxContacts( 10 );
    nPhyCollide::nContact contact[ MaxContacts ];
    int numContacts( nPhysicsServer::Collide( source, los, losLength,
        1, contact, nPhysicsGeom::Static | nPhysicsGeom::Ramp | nPhysicsGeom::Stairs ) );
    for ( int i(0); i < numContacts; ++i )
    {
        // Get collider
        nPhysicsGeom* collider( contact[i].GetGeometryA() );
        n_assert( collider );
        if ( collider->Type() == nPhysicsGeom::Ray )
        {
            collider = contact[i].GetGeometryB();
            n_assert( collider );
        }

        // If found an obstacle other than the terrain there's no LoS
        if ( collider->Type() != nPhysicsGeom::HeightMap )
        {
            return false;
        }
    }

    // No obstacle found, there's a clear LoS
    return true;
}

//------------------------------------------------------------------------------
/**
    Check if an entity can walk between two points (unidirectional check)

    A LoS check is done between the two given points, moved slightly up to
    prevent that the ground blocks the LoS but to detect those obstacles near
    the ground.

    An additional check to detect holes is performed, but it hasn't an accurate
    resolution, so don't use this method to detect small or narrow holes for now.
    
    Given entity's gameplay component is completely ignored by now.

    @todo Consider entity attributes like body height, body size,
          max walkable slope, max climb up/down height, etc.
    @todo Detect holes more accurately and rather faster
*/
bool
IsWalkable( const vector3& source, const vector3& target, const ncGameplayClass* /*entityConfig*/ )
{
    // Project given points over ground
    vector3 groundSource( source );
    if ( !nNavUtils::GetGroundHeight( groundSource, groundSource.y ) )
    {
        // No ground found near, and no ground means you cannot walk
        return false;
    }
    vector3 groundTarget( target );
    if ( !nNavUtils::GetGroundHeight( groundTarget, groundTarget.y ) )
    {
        // No ground found near, and no ground means you cannot walk
        return false;
    }

    // Move up given points a bit over ground
    const float GroundOffset( 0.5f );
    vector3 sourceUp( groundSource );
    sourceUp.y += GroundOffset;
    vector3 targetUp( groundTarget );
    targetUp.y += GroundOffset;

    // Test for LoS
    if ( !nNavUtils::IsLineOfSight( sourceUp, targetUp ) )
    {
        return false;
    }

    // Look for holes by sampling the ground height below each 'step' towards the end point
    const float stepSize( 0.5f );
    const float heightTolerance( 0.5f );
    vector3 travelDir( groundTarget - groundSource );
    float travelDistance( travelDir.len() );
    travelDir.norm();
    for ( float distance( stepSize ); distance < travelDistance; distance += stepSize )
    {
        vector3 pos( groundSource + travelDir * distance );
        float groundHeight;
        if ( !nNavUtils::GetGroundHeight( pos, groundHeight ) )
        {
            return false;
        }
        if ( pos.y - groundHeight > heightTolerance )
        {
            return false;
        }
    }

    // All tests have successfully passed, the surface from start to end point is walkable
    return true;
}

//------------------------------------------------------------------------------
/**
    Get the terrain height map
*/
nFloatMap*
GetHeightMap()
{
    nEntityObject* outdoor( nSpatialServer::Instance()->GetOutdoorEntity() );
    n_assert( outdoor );
    ncTerrainGMMClass* terrainGMM( outdoor->GetClassComponentSafe< ncTerrainGMMClass >() );
    nFloatMap* heightMap( terrainGMM->GetHeightMap() );
    n_assert( heightMap );
    return heightMap;
}

//-----------------------------------------------------------------------------
} // namespace nNavUtils
