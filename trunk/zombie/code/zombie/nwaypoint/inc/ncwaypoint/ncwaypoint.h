#ifndef NCWAYPOINT_H
#define NCWAYPOINT_H

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    @class ncWayPoint
    @ingroup NebulaSystem
    @brief Representation of a waypoint during develping time.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component ncwaypoint
    
    @cppclass ncWayPointPath
    
    @superclass ncPhyPickableObj

    @classinfo Component representing a waypoint in the world.

*/

//-----------------------------------------------------------------------------
#include "nphysics/ncphypickableobj.h"
#include "waypoint/waypoint.h"
//-----------------------------------------------------------------------------

class ncWayPoint : public ncPhyPickableObj
{

    NCOMPONENT_DECLARE(ncWayPoint,ncPhyPickableObj);

public:
    /// constructor
    ncWayPoint();
    /// destructor
    ~ncWayPoint();

    /// sets the position of this object and the waypoint
    void SetPosition( const vector3& newposition );

    /// sets the real waypoint
    void SetWayPoint( WayPoint* realwaypoint );
    
    /// user init instance code.
    void InitInstance(nObject::InitInstanceMsg initType);

    /// begin:scripting

    /// returns the id of the waypoint
    int GetWayPointId() const;

    /// returns the waypoint path id
    const int GetPathId() const;

    /// end:scripting

    /// moves the object to limbo
    virtual void YouShallDwellIntoTheLimbo();

    /// recovers an object from the limbo
    virtual void YourSoulMayComeBackFromLimbo();

private:
    
    /// reference to the real waypoint
    WayPoint *waypoint;

};

#endif

#endif 