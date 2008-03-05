#ifndef NCWAYPOINTPATH_H
#define NCWAYPOINTPATH_H

//-----------------------------------------------------------------------------
/**
    @class ncWayPointPath
    @ingroup NebulaSystem
    @brief Representation of a waypoint path.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component ncwaypointpath
    
    @cppclass ncWayPointPath
    
    @superclass nComponentObject

    @classinfo Component representing a waypoint path in the world.

*/

//-----------------------------------------------------------------------------
#include "util/narray.h"
#include "util/nstring.h"
#include "entity/nentity.h"
#include "waypoint/waypoint.h"
//-----------------------------------------------------------------------------

class ncWayPointPath : public nComponentObject
{

    NCOMPONENT_DECLARE(ncWayPointPath,nComponentObject);

public:
    /// constructor
    ncWayPointPath();
    /// destructor
    ~ncWayPointPath();

    /// begin:scripting

    /// creates the path
    void Create( const nString& );

    /// adds a waypoint to the path
    void AddWayPoint( const WayPoint::waypointid );

    /// returns the number of waypoints
    int GetNumWayPoints() const;

    /// returns the name of the path
    const nString& GetName() const;

    /// checks if a waypoint it's in the path
    const bool CheckWayPointInPath(const WayPoint::waypointid);

    /// removes a waypoint from a path
    void RemoveWayPointFromPath(const WayPoint::waypointid);

    /// insert a waypoints to a path
    void InsertWayPointInPath(const WayPoint::waypointid,const WayPoint::waypointid,const WayPoint::waypointid);

    /// returns the path id
    const int GetPathId() const;

    /// Returns closest waypoint in path
    int ClosestWayPoint( const vector3& ) const;

    /// end:scripting

    /// returns the waypoint trough the index
    WayPoint* GetWayPointIndex( const int index ) const;

private:

    /// list waypoints
    nArray< WayPoint* > listWayPoints;

    /// path name
    nString name;

    /// path id
    int id;

    /// init value for the list of waypoints
    static const int InitWayPointsSize = 10;
   
    /// grow value for the list of waypoints
    static const int GrowRationWayPointsSize = 1;

};

#endif

