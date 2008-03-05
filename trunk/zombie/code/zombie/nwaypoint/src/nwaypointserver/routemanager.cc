//-----------------------------------------------------------------------------
//  routemanager.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnwaypoint.h"

#include "nwaypointserver/routemanager.h"

#include "ncwaypointpath/ncwaypointpath.h"

#include "nwaypointserver/nwaypointserver.h"

//-----------------------------------------------------------------------------
/**
    Instance

    @return a unique instance

    history:
        - 13-Oct-2004   David Reyes    created
*/
RouteManager& RouteManager::Instance()
{
    static RouteManager onlyOne;
    return onlyOne;
}

//-----------------------------------------------------------------------------
/**
    Default constructor

    history:
        - 13-Oct-2004   David Reyes    created
*/
RouteManager::RouteManager() : 
    indexLastFree(MaxRoutesAvaliable-1)
{
    // preparing free indexes
    this->DestroyAllRoutes();
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 13-Oct-2004   David Reyes    created
*/
RouteManager::~RouteManager()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates a route

    @param path waypoints path
    @param direction values: Forward, Backward
    @param type values: Linear, Bezier3, Bezier4, Bezier
    @param traverse values: Horizontal (default), Vertical

    history:
        - 13-Oct-2004   David Reyes    created
*/
const int RouteManager::CreateRoute( 
                                    const nString& path, 
                                    const RMTypes direction, 
                                    const RMTypes type, 
                                    const float radius )
{
    n_assert2( this->indexLastFree, "Run out of routes, talk to the programmers." );

    Route& newRoute(routes[ this->indexLastFree-- ]);

    newRoute.path = nWayPointServer::Instance()->CheckPathExists( path );

    newRoute.direction = direction;

    newRoute.type = type;

    newRoute.radius = radius;

    newRoute.radiusSquared = radius * radius;

    newRoute.BezierStep = 0;

    if( newRoute.direction == Forward || newRoute.direction == LoopForward || newRoute.direction == PingPongForward )
    {
        newRoute.lastWayPointIndex = 0;
        newRoute.lastPosition = newRoute.path->GetWayPointIndex(0)->GetPosition();
    }
    else
    {
        newRoute.lastWayPointIndex = newRoute.path->GetNumWayPoints() - 1;
        newRoute.lastPosition = newRoute.path->GetWayPointIndex(newRoute.lastWayPointIndex)->GetPosition();
    }

    return this->indexLastFree+1;
}

//-----------------------------------------------------------------------------
/**
    Destroys a route

    @param id id given when created route

    history:
        - 13-Oct-2004   David Reyes    created
*/
void RouteManager::DestroyRoute( const int id )
{
    if( indexLastFree == MaxRoutesAvaliable - 1 )
    {
        return;
    }

    this->indexes[ ++indexLastFree ] = id;
}

//-----------------------------------------------------------------------------
/**
    Follows a  route

    @param id route id
    @param step speed following the route

    @return true - if route ended false otherwise

    history:
        - 13-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowRoute( const int id, const float step, vector3& newposition, bool waittonextwaypoint )
{
    this->stopInWayPoint = waittonextwaypoint;
    
    Route& route(this->routes[ id ]);

    switch( route.type )
    {
    case Linear:
        return FollowLinear( route, step, newposition );
    case Bezier:
        return FollowBezier( route, step, newposition );
    default:
        n_assert2_always( "Not supported yet." );
        break;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Follows a route with linear interpolation

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 13-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowLinear( Route& route, const float step, vector3& newposition )
{
    if( route.direction == Forward )
    {
        return FollowLinearForward( route, step, newposition );
    }

    if( route.direction == PingPongForward )
    {
        return FollowPingPongLinear( route, step, newposition );
    }

    if( route.direction == PingPongBackward )
    {
        return FollowPingPongLinear( route, step, newposition );
    }

    if( route.direction == LoopForward )
    {
        return FollowLoopLinearForward( route, step, newposition );
    }

    if( route.direction == LoopBackward )
    {
        return FollowLoopLinearBackward( route, step, newposition );
    }

    return FollowLinearBackward( route, step, newposition );
}


//-----------------------------------------------------------------------------
/**
    Follows a route with linear interpolation forward

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 13-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowLinearForward( Route& route, const float step, vector3& newposition )
{
    const int lastWayPointIndex( route.path->GetNumWayPoints() -1 );

    const vector3 lastWayPointPosition( route.path->GetWayPointIndex( lastWayPointIndex )->GetPosition() );
    
    vector3 direction( lastWayPointPosition -  route.lastPosition );

    const float stepSquared( step * step );

    // check if the end it's near
    if( direction.lensquared() < stepSquared )
    {
        // it's at the end
        route.lastWayPointIndex = lastWayPointIndex;

        newposition = lastWayPointPosition;

        return true;
    }

    // check next point to be followed
    bool found(false);

    int index( route.lastWayPointIndex + 1 );
    
    while( !found )
    {
        direction = route.path->GetWayPointIndex( index )->GetPosition() -  route.lastPosition;

        if( direction.lensquared() > stepSquared )
        {
            direction.norm();

            route.lastPosition = route.lastPosition + (direction * step);
            newposition = route.lastPosition;
            return false;
        }

        if( this->stopInWayPoint )
        {
            route.lastPosition = route.path->GetWayPointIndex( index )->GetPosition();
            newposition = route.lastPosition;
            route.lastWayPointIndex = index;
            return true;
        }

        route.lastWayPointIndex = index;
        ++index;
    }

    n_assert2_always( "This point shouldn't be reached. " );

    return false;
}

//-----------------------------------------------------------------------------
/**
    Return route position

    @param id route id

    @return position

    history:
        - 13-Oct-2004   David Reyes    created
*/
const vector3& RouteManager::GetPositionRoute( const int id ) const
{
    return this->routes[ id ].lastPosition;
}

//-----------------------------------------------------------------------------
/**
    Follows a route with linear interpolation backward

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 14-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowLinearBackward( Route& route, const float step, vector3& newposition )
{    
    const float stepSquared( step * step );

    const vector3& firstWayPointPosition( route.path->GetWayPointIndex( 0 )->GetPosition() );
    
    vector3 direction( firstWayPointPosition -  route.lastPosition );

    // check if the end it's near
    if( direction.lensquared() < stepSquared )
    {
        // it's at the end
        route.lastWayPointIndex = 0;

        newposition = firstWayPointPosition;
        return true;
    }

    // check next point to be followed
    bool found(false);

    int index( route.lastWayPointIndex - 1 );
    
    while( !found )
    {
        direction = route.path->GetWayPointIndex( index )->GetPosition() -  route.lastPosition;

        if( direction.lensquared() > stepSquared )
        {
            direction.norm();

            route.lastPosition = route.lastPosition + (direction * step);
            newposition = route.lastPosition;
            return false;
        }

        if( this->stopInWayPoint )
        {
            route.lastPosition = route.path->GetWayPointIndex( index )->GetPosition();
            newposition = route.lastPosition;
            route.lastWayPointIndex = index;
            return true;
        }


        route.lastWayPointIndex = index;
        --index;
    }

    n_assert2_always( "This point shouldn't be reached. " );

    return false;
}

//-----------------------------------------------------------------------------
/**
    Follows a route with linear interpolation (in pingpong way)

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 14-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowPingPongLinear( Route& route, const float step, vector3& newposition )
{
    if( route.direction == PingPongForward )
    {
        if( FollowLinearForward( route, step, newposition ) )
        {
            route.direction = PingPongBackward;
        }
    }
    else
    {
        if( FollowLinearBackward( route, step, newposition ) )
        {
            route.direction = PingPongForward;
        }
    }


    return false;
}

//-----------------------------------------------------------------------------
/**
    Follows a route with linear interpolation (in loop way)

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 14-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowLoopLinear( Route& route, const float step, vector3& newposition )
{
    if( route.direction == LoopForward )
    {
        FollowLoopLinearForward( route, step, newposition );
    }
    else
    {
        FollowLoopLinearBackward( route, step, newposition );
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Follows a route with linear interpolation (in loop way) goint forward

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 14-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowLoopLinearForward( Route& route, const float step, vector3& newposition )
{
    // check next point to be followed
    bool found(false);

    int index( route.lastWayPointIndex + 1 );

    const int numWayPoints(route.path->GetNumWayPoints());

    const float stepSquared( step * step );

    vector3 direction;

    index %= numWayPoints;
    
    while( !found )
    {
        direction = route.path->GetWayPointIndex( index )->GetPosition() -  route.lastPosition;

        if( direction.lensquared() > stepSquared )
        {
            direction.norm();

            route.lastPosition = route.lastPosition + (direction * step);
            newposition = route.lastPosition;
            return false;
        }

        if( this->stopInWayPoint )
        {
            route.lastPosition = route.path->GetWayPointIndex( index )->GetPosition();
            newposition = route.lastPosition;
            route.lastWayPointIndex = index;
            return true;
        }

        route.lastWayPointIndex = index;
        ++index;
        index %= numWayPoints;
    }

    n_assert2_always( "This point shouldn't be reached. " );

    return false;
}

//-----------------------------------------------------------------------------
/**
    Follows a route with linear interpolation (in loop way) going backward

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 14-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowLoopLinearBackward( Route& route, const float step, vector3& newposition )
{
    const float stepSquared( step * step );

    // check next point to be followed
    bool found(false);

    int index( route.lastWayPointIndex - 1 );

    const int numWayPoints(route.path->GetNumWayPoints());

    if( index < 0 )
    {
        index = numWayPoints - 1;
    }

    vector3 direction;
    
    while( !found )
    {
        direction = route.path->GetWayPointIndex( index )->GetPosition() -  route.lastPosition;

        if( direction.lensquared() > stepSquared )
        {
            direction.norm();

            route.lastPosition = route.lastPosition + (direction * step);
            newposition = route.lastPosition;
            return false;
        }

        if( this->stopInWayPoint )
        {
            route.lastPosition = route.path->GetWayPointIndex( index )->GetPosition();
            newposition = route.lastPosition;
            route.lastWayPointIndex = index;
            return true;
        }

        route.lastWayPointIndex = index;
        --index;
        if( index  < 0 )
        {
            index = numWayPoints - 1;
        }
    }

    n_assert2_always( "This point shouldn't be reached. " );

    return false;
}

//-----------------------------------------------------------------------------
/**
    Follows full bezier

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 14-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowBezier( Route& route, const float step, vector3& newposition )
{    
    if( route.direction == Forward )
    {
        return FollowBezierForward( route, step, newposition );
    }

    if( route.direction == Backward )
    {
        return FollowBezierBackward( route, step, newposition );
    }

    if( route.direction == PingPongForward )
    {
        return FollowPingPongBezier( route, step, newposition );
    }

    if( route.direction == PingPongBackward )
    {
        return FollowPingPongBezier( route, step, newposition );
    }

    if( route.direction == LoopForward )
    {
        return FollowLoopBezierForward( route, step, newposition );
    }

    if( route.direction == LoopBackward )
    {
        return FollowLoopBezierBackward( route, step, newposition );
    }

    return FollowLinearBackward( route, step, newposition );
}

//-----------------------------------------------------------------------------
/**
    Follows a route with bezier interpolation forward

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 14-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowBezierForward( Route& route, const float step, vector3& newposition )
{
    const int lastIndexWayPointsPath( route.path->GetNumWayPoints() -1 );

    const vector3 PositionLastWayPoint( route.path->GetWayPointIndex( lastIndexWayPointsPath )->GetPosition() );

    // check if the end it's near
    vector3 direction( PositionLastWayPoint -  route.lastPosition );

    if( direction.lensquared() < step*step )
    {
        // it's at the end
        route.lastWayPointIndex = lastIndexWayPointsPath;

        newposition = PositionLastWayPoint;
        return true;
    }

    // check if it's in bezier area
    if( route.lastWayPointIndex + 1 >= lastIndexWayPointsPath )
    {
        // no bezier, it's the last point
        route.BezierStep = 0;
        return FollowLinearForward( route, step, newposition );
    }

    direction = route.path->GetWayPointIndex( route.lastWayPointIndex + 1 )->GetPosition() - route.lastPosition;

    const float ratio( direction.lensquared() / route.radiusSquared );

    if( ratio < float(1) )
    {
        // bezier area
        if( route.BezierStep == 0 )
        {
            // calculate time stamp for bezier
            route.BezierStep = float(1) - ratio;
        }
        else
        {
            route.BezierStep += step / route.radius;
        }

        if( route.BezierStep >= float(1) )
        {
            route.BezierStep = 0;
            ++route.lastWayPointIndex;
            return FollowLinearForward( route, step, newposition );
        }

        BezierRoute( route, newposition );
        return false;
    }
    else
    {
        route.BezierStep = 0;
        return FollowLinearForward( route, step, newposition );
    }

}

//-----------------------------------------------------------------------------
/**
    Applicates bezier to the points

    @param route a route
    @param newposition where to go

    history:
        - 20-Oct-2004   David Reyes    created
*/
void RouteManager::BezierRoute( Route& route, vector3& newposition )
{
    if( route.direction == Forward || route.direction == PingPongForward || route.direction == LoopForward )
    {
        BezierForward( route, newposition );
        return;
    }
    if( route.direction == Backward || route.direction == PingPongBackward || route.direction == LoopBackward )
    {
        BezierBackward( route, newposition );
        return;
    }
}

//-----------------------------------------------------------------------------
/**
    Applicates bezier to the points

    @param route a route
    @param newposition where to go

    history:
        - 20-Oct-2004   David Reyes    created
*/
void RouteManager::BezierForward( Route& route, vector3& newposition )
{
    const vector3& P0Dist(route.path->GetWayPointIndex(route.lastWayPointIndex)->GetPosition());

    const int numWayPoints( route.path->GetNumWayPoints() );

    int index( route.lastWayPointIndex+1 );

    index %= numWayPoints;

    vector3 P1(route.path->GetWayPointIndex(index)->GetPosition());
    
    ++index;

    index %= numWayPoints;

    const vector3& P2Dist(route.path->GetWayPointIndex(index)->GetPosition());

    vector3 P0 = (P0Dist-P1);
    
    P0.norm();

    P0 *= route.radius;

    P0 += P1;

    vector3 P2 = (P2Dist-P1);
    
    P2.norm();

    P2 *= route.radius;

    P2 += P1;

    float oneMinusT( 1.f - route.BezierStep );

    P0 *= oneMinusT * oneMinusT;

    P1 *= 2 * oneMinusT * route.BezierStep;

    P2 *= route.BezierStep * route.BezierStep;

    newposition = route.lastPosition = P0 + P1 + P2;
}

//-----------------------------------------------------------------------------
/**
    Applicates bezier to the points

    @param route a route
    @param newposition where to go

    history:
        - 20-Oct-2004   David Reyes    created
*/
void RouteManager::BezierBackward( Route& route, vector3& newposition )
{
    int index( route.lastWayPointIndex );

    const vector3& P0Dist(route.path->GetWayPointIndex(index)->GetPosition());

    --index;

    if( index < 0 )
    {
        index = route.path->GetNumWayPoints() - 1;
    }

    vector3 P1(route.path->GetWayPointIndex(index)->GetPosition());

    --index;

    if( index < 0 )
    {
        index = route.path->GetNumWayPoints() - 1;
    }

    const vector3& P2Dist(route.path->GetWayPointIndex(index)->GetPosition());

    vector3 P0 = (P0Dist-P1);
    
    P0.norm();

    P0 *= route.radius;

    P0 += P1;

    vector3 P2 = (P2Dist-P1);
    
    P2.norm();

    P2 *= route.radius;

    P2 += P1;

    float oneMinusT( 1.f - route.BezierStep );

    P0 *= oneMinusT * oneMinusT;

    P1 *= 2 * oneMinusT * route.BezierStep;

    P2 *= route.BezierStep * route.BezierStep;

    newposition = route.lastPosition = P0 + P1 + P2;
}

//-----------------------------------------------------------------------------
/**
    Follows a route with bezier interpolation backward

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 20-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowBezierBackward( Route& route, const float step, vector3& newposition )
{

   const vector3 FirstWayPointPosition( route.path->GetWayPointIndex( 0 )->GetPosition() );

    vector3 direction( FirstWayPointPosition -  route.lastPosition );

    // check if the end it's near
    if( direction.lensquared() < step*step )
    {
        // it's at the end
        route.lastWayPointIndex = 0;

        newposition = FirstWayPointPosition;
        return true;
    }

    // check if it's in bezier area
    if( route.lastWayPointIndex - 1 <= 0 )
    {
        // no bezier, it's the last point
        route.BezierStep = 0;
        return FollowLinearBackward( route, step, newposition );
    }

    direction = route.path->GetWayPointIndex( route.lastWayPointIndex - 1 )->GetPosition() - route.lastPosition;

    const float ratio( direction.lensquared() / route.radiusSquared );

    if( ratio < float(1) )
    {
        // bezier area
        if( route.BezierStep == 0 )
        {
            // calculate time stamp for bezier
            route.BezierStep = float(1) - ratio;
        }
        else
        {
            route.BezierStep += step / route.radius;
        }

        if( route.BezierStep >= float(1) )
        {
            route.BezierStep = 0;
            --route.lastWayPointIndex;
            return FollowLinearBackward( route, step, newposition );
        }

        BezierRoute( route, newposition );
        return false;
    }
    else
    {
        route.BezierStep = 0;
        return FollowLinearBackward( route, step, newposition );
    }

}

//-----------------------------------------------------------------------------
/**
    Follows a route with linear interpolation (in pingpong way)

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 20-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowPingPongBezier( Route& route, const float step, vector3& newposition )
{
    if( route.direction == PingPongForward )
    {
        if( FollowBezierForward( route, step, newposition ) )
        {
            route.direction = PingPongBackward;
        }
    }
    else
    {
        if( FollowBezierBackward( route, step, newposition ) )
        {
            route.direction = PingPongForward;
        }
    }


    return false;
}

//-----------------------------------------------------------------------------
/**
    Follows a route with bezier interpolation (in loop way) goint forward

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 20-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowLoopBezierForward( Route& route, const float step, vector3& newposition )
{
    const int numWayPoints(route.path->GetNumWayPoints());
    
    int index ( (route.lastWayPointIndex + 1) %  numWayPoints);      
    
    vector3 direction(route.path->GetWayPointIndex( index )->GetPosition() - route.lastPosition);

    const float ratio( direction.lensquared() / route.radiusSquared );

    if( ratio < float(1) )
    {
        // bezier area
        if( route.BezierStep == 0 )
        {
            // calculate time stamp for bezier
            route.BezierStep = float(1) - ratio;
        }
        else
        {
            route.BezierStep += step / route.radius;
        }

        if( route.BezierStep >= float(1) )
        {
            route.BezierStep = 0;
            ++route.lastWayPointIndex;
            route.lastWayPointIndex %= numWayPoints;
            return FollowLoopLinearForward( route, step, newposition );
        }

        BezierRoute( route, newposition );
        return false;
    }
    else
    {
        route.BezierStep = 0;
        return FollowLoopLinearForward( route, step, newposition );
    }
}

//-----------------------------------------------------------------------------
/**
    Follows a route with bezier interpolation (in loop way) going backward

    @param route a route
    @param step speed following the route
    @param newposition where to go

    @return true - if route ended false otherwise

    history:
        - 20-Oct-2004   David Reyes    created
*/
const bool RouteManager::FollowLoopBezierBackward( Route& route, const float step, vector3& newposition )
{
    int index ( route.lastWayPointIndex - 1 );      

    if( index < 0 )
    {
        index = route.path->GetNumWayPoints() - 1;
    }
    
    vector3 direction(route.path->GetWayPointIndex( index )->GetPosition() - route.lastPosition);

    const float ratio( direction.lensquared() / route.radiusSquared );

    if( ratio < float(1) )
    {
        // bezier area
        if( route.BezierStep == 0 )
        {
            // calculate time stamp for bezier
            route.BezierStep = float(1) - ratio;
        }
        else
        {
            route.BezierStep += step / route.radius;
        }

        if( route.BezierStep >= float(1) )
        {
            route.BezierStep = 0;
            --route.lastWayPointIndex;
            if( route.lastWayPointIndex < 0 )
            {
                route.lastWayPointIndex = route.path->GetNumWayPoints() - 1;
            }
            return FollowLoopLinearBackward( route, step, newposition );
        }

        BezierRoute( route, newposition );
        return false;
    }
    else
    {
        route.BezierStep = 0;
        return FollowLoopLinearBackward( route, step, newposition );
    }
}

//-----------------------------------------------------------------------------
/**
    Destroys all the allocated routes

    history:
        - 20-Oct-2004   David Reyes    created
*/
void RouteManager::DestroyAllRoutes()
{
    // preparing free indexes
    for( int index(0); index < MaxRoutesAvaliable; ++index )
    {
        indexes[ index ] = 0;
    }    

    indexLastFree = MaxRoutesAvaliable - 1;
}

//-----------------------------------------------------------------------------
/**
    Reverses direction

    history:
        - 23-Nov-2005   David Reyes    created
*/
void RouteManager::Reverse( const int id )
{
    Route& route(this->routes[ id ]);

    switch( route.direction )
    {
        case Forward:
            route.direction = Backward;
            break;
        case Backward:
            route.direction = Forward;
            break;
        case LoopForward:
            route.direction = LoopBackward;
            break;
        case LoopBackward:
            route.direction = LoopForward;
            break;
        case PingPongForward:
            route.direction = PingPongBackward;
            break;
        case PingPongBackward:
            route.direction = PingPongForward;
            break;
    }
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
