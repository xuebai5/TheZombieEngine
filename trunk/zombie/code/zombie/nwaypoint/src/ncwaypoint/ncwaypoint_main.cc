//-----------------------------------------------------------------------------
//  ncwaypoint.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnwaypoint.h"

#ifndef NGAME

#include "ncwaypoint/ncwaypoint.h"
#include "zombieentity/nctransform.h"
#include "nwaypointserver/nwaypointserver.h"
#include "ncwaypointpath/ncwaypointpath.h"
#include "nspatial/ncspatial.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncWayPoint,ncPhyPickableObj);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncWayPoint)
    NSCRIPT_ADDCMD_COMPOBJECT('DGWI', int, GetWayPointId, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGWP', const int, GetPathId, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 06-Oct-2004   Zombie         created
*/
ncWayPoint::ncWayPoint() :
    waypoint(0)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 06-Oct-2004   Zombie         created
*/
ncWayPoint::~ncWayPoint()
{
    // Empty
}


//-----------------------------------------------------------------------------
/**
    Sets the position of this object and the waypoint

    @param newposition world position

    history:
        - 06-Oct-2004   Zombie         created
*/
void ncWayPoint::SetPosition( const vector3& newposition )
{
    if( !this->waypoint )
    {
        return;
    }

    this->waypoint->SetNewPosition( newposition );

    ncPhyPickableObj::SetPosition( newposition );
}

//-----------------------------------------------------------------------------
/**
    Sets the real waypoint

    @param realwaypoint a waypoint reference

    history:
        - 06-Oct-2004   Zombie         created
*/
void ncWayPoint::SetWayPoint( WayPoint* realwaypoint )
{
    this->waypoint = realwaypoint;
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 17-May-2005   Zombie         created
*/
void ncWayPoint::InitInstance(nObject::InitInstanceMsg initType)
{
    // update Spatial bb
    this->GetComponentSafe<ncSpatial>()->SetBBox(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    //ncPhyPickableObj::InitInstance(false);
    ncPhyPickableObj::InitInstance(nObject::NewInstance);

#ifndef NGAME
    if (initType != nObject::ReloadedInstance)
    {
        if( !nWayPointServer::Instance()->IsLoading() )
        {
            ncTransform* transform( this->GetComponent<ncTransform>() );

            n_assert2( transform, "Missing component ncTransform." );

            WayPoint::waypointid id(nWayPointServer::Instance()->CreateNewWayPoint( 
                transform->GetPosition() ) );

            this->SetWayPoint( nWayPointServer::Instance()->GetWayPoint( id ) );

            nWayPointServer::Instance()->GetWayPoint( id )->SetComponent( this );        
        }
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Returns the id of the waypoint.

    @return waypoint id

    history:
        - 10-Oct-2005   Zombie         created
*/
int ncWayPoint::GetWayPointId() const
{
    return this->waypoint->GetId();
}

//-----------------------------------------------------------------------------
/**
    Moves the object to limbo.

    history:
        - 10-Oct-2005   Zombie         created
*/
void ncWayPoint::YouShallDwellIntoTheLimbo()
{
    ncPhysicsObj::YouShallDwellIntoTheLimbo();


    if( this->waypoint->GetPath() )
    {

        ncWayPointPath* path(nWayPointServer::Instance()->CheckPathExists( this->waypoint->GetPath() ));

        if( path )
        {
            nWayPointServer::Instance()->RemoveWayPointFromItsPath( this->waypoint->GetId() );
        }
    }

    nWayPointServer::Instance()->RemoveWayPointFromList( this->waypoint->GetId() );

}

//-----------------------------------------------------------------------------
/**
    Recovers an object from the limbo.

    history:
        - 10-Oct-2005   Zombie         created
*/
void ncWayPoint::YourSoulMayComeBackFromLimbo()
{
    ncPhysicsObj::YourSoulMayComeBackFromLimbo();

    if( !this->waypoint->GetPath() )
    {
        return;
    }

    ncWayPointPath* path(nWayPointServer::Instance()->CheckPathExists( this->waypoint->GetPath() ));

    if( !path )
    {
        return;
    }

    int idForward(0);

    int idBackward(0);

    if( this->waypoint->GetForward() )
    {
        idForward = this->waypoint->GetForward()->GetId();

    }

    if( this->waypoint->GetBackward() )
    {
        idBackward = this->waypoint->GetBackward()->GetId();
    }

    nWayPointServer::Instance()->AddWayPointFromList( this->waypoint );

    path->InsertWayPointInPath( this->waypoint->GetId(), idBackward, idForward );

    if( this->waypoint->GetForward() )
    {
        this->waypoint->GetForward()->SetBackward( this->waypoint );
    }

    if( this->waypoint->GetBackward() )
    {
        this->waypoint->GetBackward()->SetForward( this->waypoint );
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the waypoint path.

    @return path id

    history:
        - 10-Jan-2005   Zombie         created
*/
const int ncWayPoint::GetPathId() const
{
    return this->waypoint->GetPath();
}

#else

class nClassComponentObject * 
n_init_ncWayPoint(char const *,class nComponentObjectServer *)
{
    return 0;
}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
