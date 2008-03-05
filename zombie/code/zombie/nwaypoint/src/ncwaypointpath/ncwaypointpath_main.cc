//-----------------------------------------------------------------------------
//  ncwaypointpath_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnwaypoint.h"


#include "ncwaypointpath/ncwaypointpath.h"
#include "nwaypointserver/nwaypointserver.h"
#include "ncwaypoint/ncwaypoint.h"
#include "util/ntag.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncWayPointPath,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncWayPointPath)
    NSCRIPT_ADDCMD_COMPOBJECT('DCRE', void, Create, 1, ( const nString& ), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DAWP', void, AddWayPoint, 1, ( const WayPoint::waypointid ), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DNWP', int, GetNumWayPoints, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGPN', const nString&, GetName, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DCWP', const bool, CheckWayPointInPath, 1, (const WayPoint::waypointid), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DRWP', void, RemoveWayPointFromPath, 1, (const WayPoint::waypointid), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DIWP', void, InsertWayPointInPath, 3, (const WayPoint::waypointid,const WayPoint::waypointid,const WayPoint::waypointid), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGPI', const int, GetPathId, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DCLW', int, ClosestWayPoint, 1, ( const vector3& ), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 10-Oct-2004   David Reyes    created
*/
ncWayPointPath::ncWayPointPath() :
    listWayPoints(InitWayPointsSize,GrowRationWayPointsSize),
    name( "Unnamed" ),
    id(-1)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 10-Oct-2004   David Reyes    created
*/
ncWayPointPath::~ncWayPointPath()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Creates the path

    @param name path name

    history:
        - 10-Oct-2004   David Reyes    created
*/
void ncWayPointPath::Create( const nString& name )
{
    this->name = name;
        
    this->id = nTag( name ).KeyMap();
}

//-----------------------------------------------------------------------------
/**
    Adds a waypoint to the path

    @param id waypoint id

    history:
        - 10-Oct-2004   David Reyes    created
*/
void ncWayPointPath::AddWayPoint( const WayPoint::waypointid id )
{
    WayPoint* waypoint( nWayPointServer::Instance()->CheckWayPointExists( id ) );

    if( !waypoint )
    {
        return;
    }

    this->listWayPoints.PushBack( waypoint );
}

//-----------------------------------------------------------------------------
/**
    Returns the number of waypoints

    @return num waypoints

    history:
        - 10-Oct-2004   David Reyes    created
*/
int ncWayPointPath::GetNumWayPoints() const
{
    return this->listWayPoints.Size();
}

//-----------------------------------------------------------------------------
/**
    Returns the name of the path

    @return path name

    history:
        - 10-Oct-2004   David Reyes    created
*/
const nString& ncWayPointPath::GetName() const
{
    return this->name;
}

//-----------------------------------------------------------------------------
/**
    Checks if a waypoint it's in the path

    @return true/false

    history:
        - 10-Oct-2004   David Reyes    created
*/
const bool ncWayPointPath::CheckWayPointInPath( const WayPoint::waypointid id )
{
    WayPoint* waypoint( nWayPointServer::Instance()->CheckWayPointExists( id ) );

    if( !waypoint )
    {
        return false;
    }

    return this->listWayPoints.Find( waypoint ) != this->listWayPoints.End();
}

//-----------------------------------------------------------------------------
/**
    Removes a waypoint from a path

    @param id waypoint id

    history:
        - 10-Oct-2004   David Reyes    created
*/
void ncWayPointPath::RemoveWayPointFromPath( const WayPoint::waypointid id )
{
    WayPoint* waypoint( nWayPointServer::Instance()->CheckWayPointExists( id ) );

    if( !waypoint )
    {
        return;
    }

    for( nArray< WayPoint* >::iterator it(this->listWayPoints.Begin());
        it != this->listWayPoints.End(); ++it )
    {
        if( *it == waypoint )
        {
            // removing
            this->listWayPoints.Erase( it );
            break;
        }
    }

#ifndef NGAME
    waypoint->SetPath(0);
#endif
}

//-----------------------------------------------------------------------------
/**
    Returns the waypoint trough the index

    @param index index

    history:
        - 10-Oct-2004   David Reyes    created
*/
WayPoint* ncWayPointPath::GetWayPointIndex( const int index ) const
{
    n_assert2( index < this->listWayPoints.Size(), "Index out of bounds." );

    return this->listWayPoints[ index ];
}

//-----------------------------------------------------------------------------
/**
    Insert a waypoints to a path

    @param id waypoint to be inserted
    @param idA id waypoint from a path
    @param idB id waypoint from a path


    history:
        - 10-Oct-2004   David Reyes    created
*/
void ncWayPointPath::InsertWayPointInPath( const WayPoint::waypointid id,const WayPoint::waypointid idA,const WayPoint::waypointid 
#ifndef NGAME
                                          /*idB*/
#endif
                                          )
{
    WayPoint* wp( nWayPointServer::Instance()->CheckWayPointExists(id) );
    
    WayPoint* wpA( nWayPointServer::Instance()->CheckWayPointExists(idA) );

#if 0
//#ifndef NGAME
    WayPoint* wpB( nWayPointServer::Instance()->CheckWayPointExists(idB) );

    if( !wpA && !wpB )
    {
        this->AddWayPoint( id );
        return;
    }

    if( !wpA && wpB )
    {
        // first in the list
        this->listWayPoints.Insert( 0, wp );
        return;
    }

    if( !wpB )
    {
        // last in the list
        this->listWayPoints.PushBack( wp );
        return;
    }

    if( wpA->GetForward() == wpB )
    {
        // allright
    }
    else if( wpA->GetBackward() == wpB )
    {
        WayPoint* swap( wpA );
        wpA = wpB;
        wpB = swap;
    }
    else
    {
        n_assert2_always( "The two referenced waypoints are not consecutive." );
        return;
    }
#endif

    int index(this->listWayPoints.FindIndex( wpA ));

    this->listWayPoints.Insert( index + 1, wp );
}

//-----------------------------------------------------------------------------
/**
    Returns the path id

    @return id

    history:
        - 14-Dec-2004   David Reyes    created
*/
const int ncWayPointPath::GetPathId() const
{
    return this->id;
}

//-----------------------------------------------------------------------------
/**
    Returns closest waypoint in path

    @param position 

    @return index
*/
int
ncWayPointPath::ClosestWayPoint( const vector3& position ) const
{
    int closest = -1;
    float minDistance = FLT_MAX;

    for(int i = 0; i < this->GetNumWayPoints(); ++i )
    {
        float sqrtDist = (this->GetWayPointIndex(i)->GetPosition() - position).lensquared();
        if( sqrtDist < minDistance )
        {
            minDistance = sqrtDist;
            closest = i;
        }
    }
    return closest;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
