#ifndef NWAYPOINTSERVER_H
#define NWAYPOINTSERVER_H

//-----------------------------------------------------------------------------
/**
    @class nWayPointServer
    @ingroup NebulaSystem
    @brief A waypoint manager

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass nwaypointserver
    
    @cppclass nWayPointServer
    
    @superclass nRoot

    @classinfo A waypoint manager.
*/    

//-----------------------------------------------------------------------------

#include "kernel/nroot.h"
#include "waypoint/waypoint.h"
#include "util/narray.h"
#include "util/nkeyarray.h"
#include "entity/nentity.h"

//-----------------------------------------------------------------------------
class ncWayPointPath;
//-----------------------------------------------------------------------------

class nWayPointServer : public nRoot 
{
public:
    /// waypoint id type
    typedef int waypointid;
    
    /// get instance pointer
    static nWayPointServer* Instance();

    /// constructor
    nWayPointServer();

    /// destructor
    ~nWayPointServer();

    /// begin:scripting

    /// creates a way point
    waypointid CreateNewWayPoint(const vector3&);

    /// creates a path with two waypoints
    const int CreatePathWithTwoWayPoints(const waypointid,const waypointid, const nString&);

    /// creates a new path
    const int CreateNewPath(const nString&);

    /// destroyes a path
    void DestroysPath(const nString&);

    /// adds a waypoint to the path
    void AddWayPointToPath(const waypointid,const nString&);

    /// removes a waypoint from a path
    void RemoveWayPointFromPath(const waypointid,const nString&);

    /// checks if a waypoint it's in a path
    const bool CheckWayPointInPath(const WayPoint::waypointid,const nString&);

    /// insert a waypoints to a path
    void InsertWayPointInPath(const WayPoint::waypointid,const WayPoint::waypointid,const WayPoint::waypointid);

    /// remove waypoint from the path
    void RemoveWayPointFromItsPath(const WayPoint::waypointid);

    /// sets the path creator
    void SetPathCreator(const nEntityObjectId);

    /// inserts a waypoint at the back of a path
    void InsertWPAtTheBackOfPath(const WayPoint::waypointid, const nString&);

    /// remove waypoint from the list
    void RemoveWayPointFromList(const WayPoint::waypointid);

    /// create a route
    const int CreateRoute(const nString&,int,int,float);

    /// destroys a route
    void DestroyRoute(const int);

    /// follows a route
    const bool FollowRoute(const int,const float,const bool);

    /// return route position
    const vector3& RoutePosition(const int) const;

    /// returns the number of waypoints of a given path
    const int GetNumWayPoints(const int);

    /// returns the path id
    const int GetPathId( const nString& ) const;

#ifndef NGAME
    /// returns the number of paths
    const int GetNumPaths() const;

    /// returns the name of a given path
    const nString& GetPathName(const int) const;

    /// Sets the name of the given path
    void RenamePath( const nString&, const nString& );

    /// returns a waypoint from a path and index
    nEntityObject* GetWayPointFromPath(const int,const int);
#endif

    /// end:scripting

    /// add waypoint to the list
    void AddWayPointFromList( WayPoint* wp );

    /// returns the waypoint itself trough the id
    WayPoint* GetWayPoint( const WayPoint::waypointid id );

    /// returns the waypoint giving a path
    WayPoint* GetWayPoint( const int pathid, const int index );

    /// returns the waypoint giving a path
    WayPoint* GetWayPoint( const nString& path, const int index );

    /// destroys information (not the server)
    void Destroy();

    /// called when a level is new
    void OnNewLevel();

    /// called when a level is loaded
    void OnLoadLevel();

    /// called when a level is saved
    void OnSaveLevel();

#ifndef NGAME
    /// object persistency
    bool SaveCmds(nPersistServer *ps);
#endif

    /// returns the number of waypoints of a given path
    const int GetNumWayPoints( const nString& pathName );

#ifndef NGAME
    /// draws the waypoints
    void Draw();
#endif 

    /// returns if the server it's loading information
    const bool IsLoading() const;

    /// check waypoint exists
    WayPoint* CheckWayPointExists( const WayPoint::waypointid id );

    /// check if a path exists
    ncWayPointPath* CheckPathExists( const nString& pathName );

    /// check if a path exists
    ncWayPointPath* CheckPathExists( const int id );

private:

    /// stores the WayPoints
    nKeyArray< WayPoint* > wayPointsList;

#ifndef NGAME

    /// meta information for persistancy
    typedef struct PathsCreator 
    {
        nEntityObjectId idCreator;
        nArray< const ncWayPointPath* > paths;
    } tPathsCreator;

    nKeyArray< tPathsCreator* > pathCreators;

    /// adds a path to a creator
    void AddPathToCreator( const nEntityObjectId idCreator, const ncWayPointPath* path );

    /// stores the last creator
    nEntityObjectId lastCreatorId;

#endif

    /// sets if the server it's loading info
    bool loading;

    /// list of paths
    nKeyArray< ncWayPointPath* > pathsList;

    /// returns a new waypoint id
    const WayPoint::waypointid GetNewWayPointId();

    /// pointer to the unique instance
    static nWayPointServer* Singleton;

    /// stores the initial space for paths
    static const int InitNumberOfPaths = 100;

    /// stores the growing factor of paths
    static const int InitGrowingFactorOfPaths = 1;

    /// stores the initial space for paths
    static const int InitNumberOfWayPoints = 100;

    /// stores the growing factor of paths
    static const int InitGrowingFactorOfWayPoints = 1;

    /// stores the last waypoint id used
    WayPoint::waypointid lastWayPointIdUsed;
};

#endif 