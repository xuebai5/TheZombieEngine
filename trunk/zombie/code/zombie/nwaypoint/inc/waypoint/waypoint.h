#ifndef WAYPOINT_H
#define WAYPOINT_H

//-----------------------------------------------------------------------------
/**
    @class WayPoint
    @ingroup NebulaSystem
    @brief A waypoint

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------

#include "mathlib/vector.h"
#include "util/nstring.h"

//-----------------------------------------------------------------------------
#ifndef NGAME
class ncWayPoint;
#endif
//-----------------------------------------------------------------------------
class WayPoint 
{
public:
    
    /// waypoint id type
    typedef int waypointid;
    
    /// constructor
    WayPoint( const vector3& position, const waypointid id );

    /// destructor
    ~WayPoint();

    /// returns the waypoint id
    const waypointid GetId() const;

    /// returns the position of the waypoint
    const vector3& GetPosition() const;

    /// sets a new position to the waypoint
    void SetNewPosition( const vector3& position );

#ifndef NGAME

    /// sets a name for the waypoint
    void SetName( const nString& newname );

    /// gets the name of this waypoint
    const nString& GetName() const;

    /// sets the brotherA
    void SetForward( WayPoint* fward );

    /// sets the brotherB
    void SetBackward( WayPoint* bward );

    /// gets the brotherA
    WayPoint* GetForward() const;

    /// gets the brotherB
    WayPoint* GetBackward() const;

    /// sets the path
    void SetPath( int id );

    /// returns the component
    ncWayPoint* GetComponent() const;

    /// sets the component
    void SetComponent( ncWayPoint* component );

#endif

    /// gets the path id
    const int GetPath() const;

private:

    /// default constructor (not entitled to be used)
    WayPoint();

    /// waypoint point
    vector3 point;

    /// stores the waypoint id
    waypointid identifier;

#ifndef NGAME
    /// stores a name for the waypoint only for displaying help
    nString name;

    /// stores the brothers
    WayPoint* forward;       
    WayPoint* backward;       
#endif
    /// stores path id
    int idPath;

#ifndef NGAME
    /// pointer to the entity
    ncWayPoint* componentWP;
#endif
};

#endif