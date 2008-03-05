#ifndef ROUTEMANAGER_H
#define ROUTEMANAGER_H

//-----------------------------------------------------------------------------
/**
    @class RouteManager
    @ingroup NebulaSystem
    @brief A waypoint routes manager

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------

#include "util/nstring.h"

//-----------------------------------------------------------------------------

class ncWayPointPath;

//-----------------------------------------------------------------------------

class RouteManager 
{
public:

    typedef enum {
        Forward = 0, // goes forward and stops at the end
        Backward, // goes backward and stops at the start
        LoopForward,  // goes in an infinite loop starting from the start path
        LoopBackward, // goes in an infinite loop startung fron the end path
        PingPongForward, // goes and comes in an infinite loop starting at the start of the path
        PingPongBackward, // goes and comes in an infinite loop starting at the end of the path
        Horizontal, // (only bezier*, it's apply in horizontal)
        Vertical, // (only bezier*, it's apply in vertical)
        Linear, // goes following the path as it is
        Bezier // Bezier in the corners
    } RMTypes;

    /// instance
    static RouteManager& Instance();

    /// destructor
    ~RouteManager();

    /// creates a route
    const int CreateRoute( const nString& path, const RMTypes direction, const RMTypes type, const float radius = 0 );

    /// destroys a route
    void DestroyRoute( const int id );

    /// follows a  route
    const bool FollowRoute( const int id, const float step, vector3& newposition, bool waittonextwaypoint = false );

    /// return route position
    const vector3& GetPositionRoute( const int id ) const;

    /// destroys all the allocated routes
    void DestroyAllRoutes();

    /// reverses direction
    void Reverse( const int id );

private:
    /// default constructor
    RouteManager();

    /// stores the max routes available
    static const int MaxRoutesAvaliable = 100;

    /// route node
    struct Route 
    {
        ncWayPointPath* path;
        RouteManager::RMTypes direction;
        RouteManager::RMTypes type;
        float radius;
        float radiusSquared;
        int lastWayPointIndex; // only for bezier 3 and 4
        vector3 lastPosition;
        float BezierStep;
    };

    /// stores all the available routes
    Route routes[MaxRoutesAvaliable];

    /// routes indexes
    int indexes[MaxRoutesAvaliable];      

    /// conts the number of routes used
    int indexLastFree;

    /// follows a route with linear interpolation
    const bool FollowLinear( Route& route, const float step, vector3& newposition );

    /// follows a route with linear interpolation forward
    const bool FollowLinearForward( Route& route, const float step, vector3& newposition );

    /// follows a route with linear interpolation backward
    const bool FollowLinearBackward( Route& route, const float step, vector3& newposition );

    /// follows a route with linear interpolation (in pingpong way)
    const bool FollowPingPongLinear( Route& route, const float step, vector3& newposition );

    /// follows a route with linear interpolation (in loop way)
    const bool FollowLoopLinear( Route& route, const float step, vector3& newposition );

    /// follows a route with linear interpolation (in loop way) goint forward
    const bool FollowLoopLinearForward( Route& route, const float step, vector3& newposition );

    /// follows a route with linear interpolation (in loop way) going backward
    const bool FollowLoopLinearBackward( Route& route, const float step, vector3& newposition );

    /// follows full bezier
    const bool FollowBezier( Route& route, const float step, vector3& newposition );

    /// follows a route with bezier interpolation forward
    const bool FollowBezierForward( Route& route, const float step, vector3& newposition );

    /// follows a route with bezier interpolation backward
    const bool FollowBezierBackward( Route& route, const float step, vector3& newposition );

    /// follows a route with bezier interpolation (in pingpong way)
    const bool FollowPingPongBezier( Route& route, const float step, vector3& newposition );

    /// applicates bezier to the points
    void BezierRoute( Route& route, vector3& newposition );

    /// applicates bezier to the points
    void BezierForward( Route& route, vector3& newposition );

    /// applicates bezier to the points
    void BezierBackward( Route& route, vector3& newposition );

    /// follows a route with bezier interpolation (in loop way) goint forward
    const bool FollowLoopBezierForward( Route& route, const float step, vector3& newposition );

    /// follows a route with bezier interpolation (in loop way) going backward
    const bool FollowLoopBezierBackward( Route& route, const float step, vector3& newposition );

    /// stores if this in emulation mode
    bool inEmulation;

    /// stores if it has to stop at the next waypoint
    bool stopInWayPoint;
};

#endif // ROUTEMANAGER_H