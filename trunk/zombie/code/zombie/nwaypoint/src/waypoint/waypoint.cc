//-----------------------------------------------------------------------------
//  waypoint.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnwaypoint.h"
#include "waypoint/waypoint.h"
#include "ncwaypoint/ncwaypoint.h"
#ifndef NGAME
#include "entity/nentityobjectserver.h"
#endif

//-----------------------------------------------------------------------------
/**
    Constructor

    @param position point int the world

    @param id the new waypoint id

    history:
        - 06-Oct-2004   David Reyes    created
*/
WayPoint::WayPoint( const vector3& position, const waypointid id ) :
    point( position ),
    identifier( id )


#ifndef NGAME
    ,forward(0)
    ,backward(0)
    ,idPath(0)
    ,componentWP(0)
#endif
{
    // Empty
}


//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 13-Oct-2004   David Reyes    created
*/
WayPoint::~WayPoint()
{
#ifndef NGAME
    if( this->componentWP )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->componentWP->GetEntityObject() );
        this->componentWP = 0;
    }
#endif
}


//-----------------------------------------------------------------------------
/**
    Returns the waypoint id

    @return waypoint id

    history:
        - 06-Oct-2004   David Reyes    created
*/
const WayPoint::waypointid WayPoint::GetId() const
{
    return this->identifier;
}

//-----------------------------------------------------------------------------
/**
    Returns the position of the waypoint

    @return position

    history:
        - 06-Oct-2004   David Reyes    created
*/
const vector3& WayPoint::GetPosition() const
{
    return this->point;
}

//-----------------------------------------------------------------------------
/**
    Sets a new position to the waypoint

    @param position new way point position

    history:
        - 06-Oct-2004   David Reyes    created
*/
void WayPoint::SetNewPosition( const vector3& position )
{
    this->point = position;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Sets a name for the waypoint

    @param newname new waypoint name

    history:
        - 06-Oct-2004   David Reyes    created
*/
void WayPoint::SetName( const nString& newname )
{
    this->name = newname;
}

//-----------------------------------------------------------------------------
/**
    Gets the name of this waypoint

    @return name

    history:
        - 06-Oct-2004   David Reyes    created
*/
const nString& WayPoint::GetName() const
{
    return this->name;
}

//-----------------------------------------------------------------------------
/**
    Sets the brotherA

    @param fward waypoint

    history:
        - 06-Oct-2004   David Reyes    created
*/
void WayPoint::SetForward( WayPoint* fward )
{
    this->forward = fward;
}

//-----------------------------------------------------------------------------
/**
    Sets the brotherB

    @param bward waypoint

    history:
        - 06-Oct-2004   David Reyes    created
*/
void WayPoint::SetBackward( WayPoint* bward )
{
    this->backward = bward;
}

//-----------------------------------------------------------------------------
/**
    Gets the brotherA

    @return waypoint

    history:
        - 06-Oct-2004   David Reyes    created
*/
WayPoint* WayPoint::GetForward() const
{
    return this->forward;
}

//-----------------------------------------------------------------------------
/**
    Gets the brotherB

    @return waypoint

    history:
        - 06-Oct-2004   David Reyes    created
*/
WayPoint* WayPoint::GetBackward() const
{
    return this->backward;
}

//-----------------------------------------------------------------------------
/**
    Sets the path

    @param id path id

    history:
        - 06-Oct-2004   David Reyes    created
*/
void WayPoint::SetPath( int id )
{
    this->idPath = id;
}
#endif

//-----------------------------------------------------------------------------
/**
    Gets the path id

    @param id path id

    history:
        - 06-Oct-2004   David Reyes    created
*/
const int WayPoint::GetPath() const
{
    return this->idPath;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Returns the component

    @return waypoint component

    history:
        - 10-Oct-2004   David Reyes    created
*/
ncWayPoint* WayPoint::GetComponent() const
{
    return this->componentWP;
}

//-----------------------------------------------------------------------------
/**
    Sets the component

    @param component waypoint component

    history:
        - 10-Oct-2004   David Reyes    created
*/
void WayPoint::SetComponent( ncWayPoint* component )
{
    n_assert( !this->componentWP );
    this->componentWP = component;
}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------


