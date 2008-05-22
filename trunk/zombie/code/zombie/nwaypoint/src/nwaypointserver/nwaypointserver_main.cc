//-----------------------------------------------------------------------------
//  nwaypointserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnwaypoint.h"

#include "nwaypointserver/nwaypointserver.h"

#include "kernel/nkernelserver.h"

#include "util/ntag.h"

#include "nlevel/nlevelmanager.h"

#include "ncwaypoint/ncwaypoint.h"

#include "ncwaypointpath/ncwaypointpath.h"

#include "nwaypointserver/routemanager.h"

#include "entity/nentityobjectserver.h"

#include "kernel/nfileserver2.h"

#ifndef NGAME
#include "gfx2/ngfxserver2.h"
#include "gfx2/nlineserver.h"
#endif

//-----------------------------------------------------------------------------

nNebulaScriptClass(nWayPointServer, "nroot");

//-----------------------------------------------------------------------------

nWayPointServer* nWayPointServer::Singleton(0);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 05-Oct-2004   Zombie         created
*/
nWayPointServer::nWayPointServer() :
    wayPointsList( InitNumberOfWayPoints, InitGrowingFactorOfWayPoints ),
    pathsList( InitNumberOfPaths, InitGrowingFactorOfPaths ),
    lastWayPointIdUsed(0),
    loading(false)
#ifndef NGAME
    ,pathCreators(1,1)
    ,lastCreatorId(0)
#endif
{
    n_assert2( !Singleton , "Trying to instanciate a second instance of a singleton" );

    Singleton = this;

    // bind to level creation and loading
    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelCreated, 
                                          this,
                                          &nWayPointServer::OnNewLevel,
                                          0);

    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelLoaded, 
                                          this,
                                          &nWayPointServer::OnLoadLevel,
                                          0);

    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelSaved, 
                                          this,
                                          &nWayPointServer::OnSaveLevel,
                                          0);

    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelUnload, 
                                          this,
                                          &nWayPointServer::Destroy,
                                          0);
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 05-Oct-2004   Zombie         created
*/
nWayPointServer::~nWayPointServer()
{
    // Has to be the last line.
    Singleton = 0;

    this->Destroy();
}

//-----------------------------------------------------------------------------
/**
    get instance pointer

    @return pointer to the only instance of this object

    history:
        - 05-Oct-2004   Zombie         created
*/
nWayPointServer* nWayPointServer::Instance()
{
    n_assert2( Singleton , "Accessing to the waypoint server without instance." );

    return Singleton;
}

//-----------------------------------------------------------------------------
/**
    Creates a way point

    @param wpPosition position of the new point

    @return waypoint id

    history:
        - 05-Oct-2004   Zombie         created
*/
WayPoint::waypointid nWayPointServer::CreateNewWayPoint( const vector3& wpPosition )
{
    WayPoint* newWP( n_new( WayPoint(wpPosition,this->GetNewWayPointId()) ) );

    n_assert2( newWP, "Failed to allocate memory." );

    wayPointsList.Add( newWP->GetId(), newWP );

#ifndef NGAME
    if( this->IsLoading() )
    {
        // create editor entity
        nEntityObject* object(nEntityObjectServer::Instance()->NewLocalEntityObject( "newaypoint" ));

        ncWayPoint* wp( object->GetComponentSafe<ncWayPoint>() );

        wp->SetWayPoint( newWP );

        wp->SetPosition( wpPosition );

        newWP->SetComponent( wp );
    }
#endif

    return newWP->GetId();
}

//-----------------------------------------------------------------------------
/**
    Creates a new path

    @param namePath name new path

    history:
        - 05-Oct-2004   Zombie         created
*/
const int nWayPointServer::CreateNewPath( const nString& namePath )
{
#ifndef NGAME
    n_assert2( namePath.Length(), "The name it's empty." );

    if( !namePath.Length() )
        return -1;

#endif


#ifndef NGAME
    ncWayPointPath* check(this->CheckPathExists(namePath));

    n_assert2( check == 0, "This name it's already in use." );

    if( check )
    {
        return -1;
    }
#endif
    
    // creating new list
    ncWayPointPath* newPath( nEntityObjectServer::Instance()->NewLocalEntityObject( "newaypointpath" )->GetComponent<ncWayPointPath>());

    n_assert2( newPath, "Failed to allocate memory." );

    newPath->Create( namePath );

    // adding the list
    this->pathsList.Add( newPath->GetPathId(), newPath );

#ifndef NGAME
    if( loading )
    {
        this->AddPathToCreator( lastCreatorId, newPath );
    }
    else
    {
        this->AddPathToCreator( nEntityObjectServer::Instance()->GetHighId(), newPath );
    }
#endif

    return newPath->GetPathId();
}

//-----------------------------------------------------------------------------
/**
    Destroyes a path

    @param pathName path name

    history:
        - 10-Jan-2006   Zombie         created
*/
void nWayPointServer::DestroysPath( const nString& pathName )
{
    ncWayPointPath* check(this->CheckPathExists(pathName));

    n_assert2( check != 0, "This name it's already in use." );
#ifndef NGAME

    if( !check )
    {
        // it already exists
        return;
    }
#endif

    const int pathid(nTag( pathName ).KeyMap());

    ncWayPointPath* path(0);

    this->pathsList.Find( pathid, path );
    
    if( !path )
    {
        return;
    }

    // removing the waypoints from the path if any
    while( path->GetNumWayPoints() )
    {
        path->RemoveWayPointFromPath( path->GetWayPointIndex(0)->GetId() );
    }

    this->pathsList.Rem( pathid );
 
#ifndef NGAME

    // removing meta information (extremly slow)

    for( int index(0); index < this->pathCreators.Size(); ++index )
    {
        for( int iindex(0); iindex <  this->pathCreators[ index ]->paths.Size(); ++iindex )
        {
            if( this->pathCreators[ index ]->paths[ iindex ] == path )
            {
                this->pathCreators[ index ]->paths.Erase( iindex );
            }
        }
    }

#endif

   nEntityObjectServer::Instance()->RemoveEntityObject( path->GetEntityObject() );

}

//-----------------------------------------------------------------------------
/**
    Adds a waypoint to the path

    @param id waypoint id
    @param pathName path name

    history:
        - 05-Oct-2004   Zombie         created
*/
void nWayPointServer::AddWayPointToPath( const WayPoint::waypointid id, const nString& pathName )
{
    ncWayPointPath* path(this->CheckPathExists(pathName));

#ifndef NGAME
    n_assert2( path, "This path doesn't exists." );

    if( !path )
    {
        return;
    }
#endif

#ifndef NGAME
    WayPoint* waypoint(this->CheckWayPointExists(id));

    n_assert2( waypoint, "This waypoint doesn't exists." );

    if( !waypoint )
    {
        return;
    }
#endif

    if( this->CheckWayPointInPath( id, nTag(pathName).KeyMap() ) )
    {
        // it's already in the path
        return;
    }

    path->AddWayPoint( id );

#ifndef NGAME
    waypoint->SetPath( nTag(pathName).KeyMap() );
#endif
}

//-----------------------------------------------------------------------------
/**
    Removes a waypoint from a path

    @param id waypoint id
    @param pathName path name

    history:
        - 05-Oct-2004   Zombie         created
*/
void nWayPointServer::RemoveWayPointFromPath(const WayPoint::waypointid id, const nString& pathName )
{
    if( !this->CheckWayPointInPath( id, nTag(pathName).KeyMap() ) )
    {
        // it's not in the path
        return;
    }

    ncWayPointPath* path(this->CheckPathExists(pathName));

    path->RemoveWayPointFromPath( id );
}


//-----------------------------------------------------------------------------
/**
    Checks if a waypoint it's in a path

    @param id waypoint id
    @param pathName path name

    history:
        - 06-Oct-2004   Zombie         created
*/
const bool nWayPointServer::CheckWayPointInPath(const WayPoint::waypointid id,const nString& pathName )
{
    ncWayPointPath* path( this->CheckPathExists( pathName ) );

    if( !path )
    {
        return false;
    }

    return path->CheckWayPointInPath( id );
}

//-----------------------------------------------------------------------------
/**
    Check if a path exists

    @param pathName path name

    @return waypoint path if any, otherwise null pointer

    history:
        - 06-Oct-2004   Zombie         created
*/
ncWayPointPath* nWayPointServer::CheckPathExists( const nString& pathName )
{
    ncWayPointPath* checkPath(0);

    this->pathsList.Find( nTag(pathName).KeyMap(), checkPath );

    return checkPath;
}

//-----------------------------------------------------------------------------
/**
    Check if a path exists

    @param id path name

    @return waypoint path if any, otherwise null pointer

    history:
        - 06-Oct-2004   Zombie         created
*/
ncWayPointPath* nWayPointServer::CheckPathExists( const int id )
{
    ncWayPointPath* checkPath(0);

    this->pathsList.Find( id, checkPath );

    return checkPath;
}


//-----------------------------------------------------------------------------
/**
    Check waypoint exists

    @param id waypoint id

    @return waypoint if any, otherwise null pointer

    history:
        - 06-Oct-2004   Zombie         created
*/
WayPoint* nWayPointServer::CheckWayPointExists( const WayPoint::waypointid id )
{
    WayPoint* checkWP(0);

    this->wayPointsList.Find( id, checkWP );

    return checkWP;
}

//-----------------------------------------------------------------------------
/**
    Returns a new waypoint id

    @param id waypoint id

    @return waypoint if any, otherwise NoValidID

    history:
        - 06-Oct-2004   Zombie         created
*/
const WayPoint::waypointid nWayPointServer::GetNewWayPointId()
{
    return ++this->lastWayPointIdUsed;
}

//-----------------------------------------------------------------------------
/**
    Returns the waypoint itself trough the id

    @param id waypoint id

    @return waypoint if any, otherwise null pointer

    history:
        - 06-Oct-2004   Zombie         created
*/
WayPoint* nWayPointServer::GetWayPoint( const WayPoint::waypointid id )
{
    return this->CheckWayPointExists( id );
}

//-----------------------------------------------------------------------------
/**
    Destroys information (not the server)

    history:
        - 06-Oct-2004   Zombie         created
*/
void nWayPointServer::Destroy()
{
    RouteManager::Instance().DestroyAllRoutes();

    if( Singleton )
    {
        // destroying paths
        for( int index(0); index < this->pathsList.Size(); ++index )
        {
            nEntityObjectServer::Instance()->RemoveEntityObject(this->pathsList[ index ]->GetEntityObject());
        }
    }

    this->pathsList.Clear();

    // destroying waypoints
    for( int index(0); index < this->wayPointsList.Size(); ++index )
    {
        n_delete(this->wayPointsList[index]);
    }

    this->wayPointsList.Clear();

#ifndef NGAME
    // destroying meta information
    
    for( int index(0); index < this->pathCreators.Size(); ++index )
    {
        n_delete(this->pathCreators[ index ]);
    }

    this->pathCreators.Clear();

#endif
    this->lastWayPointIdUsed = 0;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Draws the waypoints

    history:
        - 06-Oct-2004   Zombie         created
        - 08-Feb-2006   Carles Ros     skip deleted/hidden waypoints
        - 14-Feb-2006   Carles Ros     reverted changes to version prior to 08-Feb-2006
*/
void nWayPointServer::Draw()
{
    static float radius(.1f);

    matrix44 model;
    vector4 color;

    model.scale( vector3( radius, radius, radius ) );
    
    nGfxServer2::Instance()->BeginShapes();
    
    // drawing waypoints
    for( int index(0); index < this->wayPointsList.Size(); ++index )
    {
        model.set_translation( this->wayPointsList[ index ]->GetPosition() );

        nGfxServer2::Instance()->DrawShape( nGfxServer2::Sphere, model, vector4(1,0,0,1) );
    }

    nGfxServer2::Instance()->EndShapes();

    matrix44 matV(nGfxServer2::Instance()->GetTransform(nGfxServer2::View));
    matV *= nGfxServer2::Instance()->GetCamera().GetProjection();

    // drawing waypoints names
    for( int index(0); index < this->wayPointsList.Size(); ++index )
    {
        nString wpName;

        if( this->wayPointsList[ index ]->GetPath() )
        {
            ncWayPointPath* path(this->CheckPathExists( this->wayPointsList[ index ]->GetPath() ));

            wpName = path->GetName() + ":";
            if( !this->wayPointsList[ index ]->GetBackward() )
            {
                wpName += "Start:";
            }
            else if( !this->wayPointsList[ index ]->GetForward() )
            {
                wpName += "End:";
            }

            color = path->GetName().MatchPattern("Player*") ?
                vector4(1,1,0,1) : vector4(1,1,1,1);
        }

        wpName.AppendInt( this->wayPointsList[ index ]->GetId() );

        vector4 pos4( this->wayPointsList[ index ]->GetPosition() );
        pos4 = matV * pos4;

        if ((pos4.x < -pos4.w) || (pos4.x > pos4.w) ||
            (pos4.y < -pos4.w) || (pos4.y > pos4.w))
        {
            continue;
        }

        pos4.x = pos4.x / pos4.w;
        pos4.y = -pos4.y / pos4.w;

        nGfxServer2::Instance()->Text( wpName.Get(), color, pos4.x, pos4.y );
    }

    // drawing paths
    static nArray<vector3> pointsArray;
    static nArray<vector4> colorsArray;
    pointsArray.Reset();
    colorsArray.Reset();

    for( int index(0); index < this->pathsList.Size(); ++index )
    {
        color = this->pathsList[index]->GetName().MatchPattern("Player*") ?
            vector4(1,1,0,1) : vector4(0,1,1,1);

        for( int innerIndex(1); innerIndex < this->pathsList[ index ]->GetNumWayPoints(); ++innerIndex )
        {
            pointsArray.Append( this->pathsList[ index ]->GetWayPointIndex( innerIndex - 1 )->GetPosition() );
            pointsArray.Append( this->pathsList[ index ]->GetWayPointIndex( innerIndex )->GetPosition() );

            colorsArray.Append( color );
            colorsArray.Append( color );
        }
    }

    if (!pointsArray.Empty())
    {
        static nLineHandler lineHandler(nGfxServer2::LineList, nMesh2::Coord|nMesh2::Color);
        if (!lineHandler.IsValid())
        {
            lineHandler.SetShader("shaders:line_no_z.fx");
        }
        matrix44 identMatrix;
        lineHandler.BeginLines( identMatrix );
        lineHandler.DrawLines3d( pointsArray.Begin(), 0, colorsArray.Begin(), pointsArray.Size() );
        lineHandler.EndLines();
    }
}

#endif 

//-----------------------------------------------------------------------------
/**
    Creates a path with two waypoints

    @param idA id waypoint
    @param idB id waypoint
    @param namepath path name

    history:
        - 06-Oct-2004   Zombie         created
*/
const int nWayPointServer::CreatePathWithTwoWayPoints( const waypointid idA, const waypointid idB, const nString& pathname )
{
    int id(this->CreateNewPath( pathname ));

    this->AddWayPointToPath( idA, pathname );

    this->AddWayPointToPath( idB, pathname );

#ifndef NGAME
    WayPoint* wpA( this->CheckWayPointExists( idA ) );
    WayPoint* wpB( this->CheckWayPointExists( idB ) );

    wpA->SetForward( wpB );
    wpB->SetBackward( wpA );

    wpA->SetPath( nTag( pathname ).KeyMap() );
    wpB->SetPath( nTag( pathname ).KeyMap() );
#endif

    return id;
}

//-----------------------------------------------------------------------------
/**
    Insert a waypoints to a path

    @param id waypoint to be inserted
    @param idA id waypoint from a path
    @param idB id waypoint from a path

    history:
        - 06-Oct-2004   Zombie         created
*/
void nWayPointServer::InsertWayPointInPath( const WayPoint::waypointid id,const WayPoint::waypointid idA,const WayPoint::waypointid idB )
{
    
    WayPoint* wpA( this->CheckWayPointExists(idA) );
    WayPoint* wpB( this->CheckWayPointExists(idB) );

#ifndef NGAME
    WayPoint* wp( this->CheckWayPointExists(id) );
    
    if( wpA->GetPath() != wpB->GetPath() )
    {
        // they have to belong to the same path
        n_assert2_always( "The two referenced waypoints belong to different paths." );
        return;
    }

    if( !wpA->GetPath() )
    {
        n_assert2_always( "The two referenced waypoints don't belong to a path." );
        return;
    }

    if( wp->GetPath() )
    {
        n_assert2_always( "The waypoint to be inserted it's already in a path." );
        return;
    }

#endif
    ncWayPointPath* path( this->CheckPathExists(wpA->GetPath()) );

    n_assert2( path, "Data corruption." );

#ifndef NGAME
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

    wpA->SetForward(wp);
    wpB->SetBackward(wp);
    wp->SetForward(wpB);
    wp->SetBackward(wpA);
    wp->SetPath(wpB->GetPath());
    
#endif

    path->InsertWayPointInPath( id, wpA->GetId(), wpB->GetId() );

}

//-----------------------------------------------------------------------------
/**
    Remove waypoint from the path

    @param id waypoint to be removed

    history:
        - 06-Oct-2004   Zombie         created
*/
void nWayPointServer::RemoveWayPointFromItsPath( const WayPoint::waypointid id )
{
    WayPoint* wp( this->CheckWayPointExists( id ) );

    if( !wp )
    {
        n_assert2_always( "Waypoint doesn't exists." );
        return;
    }

    ncWayPointPath* path( this->CheckPathExists(wp->GetPath()) );

    if( !path )
    {
        n_assert2_always( "This waypoint doesn't belong to any path." );
        return;
    }

    path->RemoveWayPointFromPath( id );

#ifndef NGAME
    if( wp->GetBackward() )
    {
        wp->GetBackward()->SetForward( wp->GetForward() );
    }
    if( wp->GetForward() )
    {
        wp->GetForward()->SetBackward( wp->GetBackward() );
    }

    wp->SetPath(0);
#endif
}

//-----------------------------------------------------------------------------
/**
    Called when a level is loaded

    history:
        - 07-Oct-2004   Zombie         created
*/
void nWayPointServer::OnNewLevel()
{
    // destroyes the actual info
    this->Destroy();    

    // loading


}

#ifndef NGAME

//-----------------------------------------------------------------------------
/**
    Adds a path to a creator

    @param idCreator id of the path's creator
    @param path a waypoint path

    history:
        - 07-Oct-2004   Zombie         created
*/
void nWayPointServer::AddPathToCreator( const nEntityObjectId idCreator, const ncWayPointPath* path )
{
    tPathsCreator* checkPath(0);
    
    if( !this->pathCreators.Find( idCreator, checkPath ) )
    {
        checkPath = n_new( tPathsCreator );

        n_assert2( checkPath, "Failed to allocate memory." );

        this->pathCreators.Add( idCreator, checkPath );
    }

    // getting the responsable of creating this path
    checkPath->idCreator = idCreator;

    checkPath->paths.PushBack( path );
}

#endif

//-----------------------------------------------------------------------------
/**
    Sets the path creator

    @param idCreator id of the path's creator

    history:
        - 07-Oct-2004   Zombie         created
*/
void nWayPointServer::SetPathCreator( const nEntityObjectId 
#ifndef NGAME
                                     idCreator 
#endif
                                     )
{
#ifndef NGAME
    this->lastCreatorId = idCreator;
#endif
}
//-----------------------------------------------------------------------------
/**
    Inserts a waypoint at the back of a path

    @param id waypoint's id
    @param path path name

    history:
        - 07-Oct-2004   Zombie         created
*/
void nWayPointServer::InsertWPAtTheBackOfPath(const WayPoint::waypointid id, const nString& namePath )
{
    ncWayPointPath* path(this->CheckPathExists(namePath));

    int idLocal(id);

    if( this->IsLoading() )
    {
        idLocal = this->lastWayPointIdUsed;
    }

#ifndef NGAME
    WayPoint* waypoint(this->CheckWayPointExists(idLocal));
#endif

    int size(path->GetNumWayPoints());

    if( !size )
    {
        this->AddWayPointToPath( idLocal, namePath );
        return;
    }

#ifndef NGAME
    WayPoint* prevWP(path->GetWayPointIndex( size - 1 ));

    prevWP->SetForward( waypoint );
    
    waypoint->SetBackward( prevWP );

    waypoint->SetPath( nTag(namePath).KeyMap() );
#endif

    path->AddWayPoint( idLocal );

}

//-----------------------------------------------------------------------------
/**
    Called when a level is loaded

    history:
        - 07-Oct-2004   Zombie         created
*/
void nWayPointServer::OnLoadLevel()
{
    this->Destroy();

    // begin loading
    this->loading = true;

    nString waypointsPath("level:waypoints/");

    nFileServer2 *fileServer(nFileServer2::Instance());

    nString realWayPointsPath(fileServer->ManglePath(waypointsPath));

    realWayPointsPath.Append( "/waypoints.n2" );

    nKernelServer::Instance()->PushCwd(this);
    nKernelServer::Instance()->Load(realWayPointsPath.Get(), false);
    nKernelServer::Instance()->PopCwd();

    // end loading
    this->loading = false;
}

//-----------------------------------------------------------------------------
/**
    Called when a level is saved

    history:
        - 07-Oct-2004   Zombie         created
*/
void nWayPointServer::OnSaveLevel()
{
    nString waypointsPath("level:waypoints/");

    nFileServer2 *fileServer(nFileServer2::Instance());

    if(!fileServer->DirectoryExists(waypointsPath))
    {
        // if the waypoints directory doesn't exist yet, create it
        fileServer->MakePath(waypointsPath);
    }

    nString realWayPointsPath(fileServer->ManglePath(waypointsPath));

    realWayPointsPath.Append( "/waypoints.n2" );

    nPersistServer *server(nKernelServer::Instance()->GetPersistServer());
    
    n_assert2(server, "Not able to access to the persist server.");

    nCmd * cmd = server->GetCmd(this, 'THIS');

    if( server->BeginObjectWithCmd(this, cmd, realWayPointsPath.Get() ) )
    {
        this->SaveCmds( server );

        server->EndObject(true);
    }
}

//-----------------------------------------------------------------------------
/**
    Returns if the server it's loading information

    history:
        - 07-Oct-2004   Zombie         created
*/
const bool nWayPointServer::IsLoading() const
{
    return this->loading;
}

//-----------------------------------------------------------------------------
/**
    Remove waypoint from the list

    @param id waypoint id

    history:
        - 07-Oct-2004   Zombie         created
*/
void nWayPointServer::RemoveWayPointFromList( const WayPoint::waypointid id )
{
    this->wayPointsList.Rem( id );
}

//-----------------------------------------------------------------------------
/**
    Add waypoint to the list

    @param wp waypoint

    history:
        - 07-Oct-2004   Zombie         created
*/
void nWayPointServer::AddWayPointFromList( WayPoint* wp )
{
    n_assert2( wp, "Null pointer." );

    this->wayPointsList.Add( wp->GetId(),  wp );
}

//-----------------------------------------------------------------------------
/**
    Creates a route

    @param path waypoints path
    @param direction values: Forward, Backward
    @param type values: Linear, Bezier3, Bezier4, Bezier
    @param traverse values: Horizontal (default), Vertical

    history:
        - 13-Oct-2004   Zombie         created
*/
const int nWayPointServer::CreateRoute( const nString& path, int direction, int type, float radius )
{
    return RouteManager::Instance().CreateRoute( path, 
        RouteManager::RMTypes( direction ),
        RouteManager::RMTypes( type ),
        radius );
}

//-----------------------------------------------------------------------------
/**
    Destroys a route

    @param routeid route id

    history:
        - 13-Oct-2004   Zombie         created
*/
void nWayPointServer::DestroyRoute( const int routeid )
{
    RouteManager::Instance().DestroyRoute( routeid );
}

//-----------------------------------------------------------------------------
/**
    Follows a route

    @param routeid route id
    @param step step size

    history:
        - 13-Oct-2004   Zombie         created
*/
const bool nWayPointServer::FollowRoute( const int routeid, const float step, const bool stopNextWayPoint )
{
    vector3 newposition;

    return RouteManager::Instance().FollowRoute( routeid, step, newposition, stopNextWayPoint );
}

//-----------------------------------------------------------------------------
/**
    Return route position

    @param id route id
    
    @return position

    history:
        - 13-Oct-2004   Zombie         created
*/
const vector3& nWayPointServer::RoutePosition( const int id ) const
{
    return RouteManager::Instance().GetPositionRoute( id );
}

//-----------------------------------------------------------------------------
/**
    Returns the number of waypoints of a given path

    @param pathName path name
    
    @return num waypoints

    history:
        - 14-Dec-2004   Zombie         created
*/
const int nWayPointServer::GetNumWayPoints( const nString& pathName )
{
    ncWayPointPath* path(this->CheckPathExists(pathName));

    n_assert2( path, "The path name doesn't exists." );

    return path->GetNumWayPoints();
}

//-----------------------------------------------------------------------------
/**
    Returns the number of waypoints of a given path

    @param id path id
    
    @return num waypoints

    history:
        - 14-Dec-2004   Zombie         created
*/
const int nWayPointServer::GetNumWayPoints( const int id )
{
    ncWayPointPath* path(0);
    
    this->pathsList.Find( id, path );

    n_assert2( path, "Wrong path id." );

    return path->GetNumWayPoints();
}

//-----------------------------------------------------------------------------
/**
    Returns the waypoint giving a path

    @param pathid path id    
    @param index waypoint index relative to the path

    @return waypoint

    history:
        - 14-Dec-2004   Zombie         created
*/
WayPoint* nWayPointServer::GetWayPoint( const int pathid, const int index )
{
    ncWayPointPath* path(0);
    
    this->pathsList.Find( pathid, path );

    n_assert2( path, "Wrong path id." );

    return path->GetWayPointIndex(index);
}

//-----------------------------------------------------------------------------
/**
    Returns the waypoint giving a path

    @param path path name
    @param index waypoint index relative to the path

    @return waypoint

    history:
        - 14-Dec-2004   Zombie         created
*/
WayPoint* nWayPointServer::GetWayPoint( const nString& path, const int index )
{
    return this->GetWayPoint( nTag( path ).KeyMap(), index );
}

//-----------------------------------------------------------------------------
/**
    Returns the path id

    @param pathName path name
    @return path id

    history:
        - 10-Jan-2006   Zombie         created
*/
const int nWayPointServer::GetPathId( const nString& pathName ) const
{
    return nTag( pathName ).KeyMap();
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Returns the number of paths

    @return num paths

    history:
        - 14-Dec-2004   Zombie         created
*/
const int nWayPointServer::GetNumPaths() const
{
    return this->pathsList.Size();
}
//-----------------------------------------------------------------------------
/**
    Returns a waypoint from a path and index

    @param pathid path id
    @param index index inside the path
    @return waypoint

    history:
        - 10-Jan-2006   Zombie         created
*/
nEntityObject* nWayPointServer::GetWayPointFromPath( const int pathid, const int index)
{
    WayPoint* wp(this->GetWayPoint( pathid, index ));

    if( !wp )
    {
        return 0;
    }

    return wp->GetComponent()->GetEntityObject();
}

//-----------------------------------------------------------------------------
/**
    Returns the name of a given path

    @return num paths

    history:
        - 14-Dec-2004   Zombie         created
*/
const nString& nWayPointServer::GetPathName( const int index ) const
{
    n_assert2( index < this->GetNumPaths(), "Index out of bounds." );

    return this->pathsList.GetElementAt( index )->GetName();
}

//-----------------------------------------------------------------------------
/**
    Renames a path

*/
void
nWayPointServer::RenamePath( const nString& oldname, const nString& newname )
{
    ncWayPointPath* path = 0;
    int path_oldid = nTag(oldname).KeyMap();
    int path_newid = nTag(newname).KeyMap();

    n_assert_return2(this->pathsList.Find(path_oldid,path),,"non-existent path");

    this->pathsList.Rem(path_oldid);
    path->Create(newname);

    // Change all waypoints in path
    for(int i=0; i < this->wayPointsList.Size(); ++i)
    {
        if( this->wayPointsList[i]->GetPath() == path_oldid )
        {
            this->wayPointsList[i]->SetPath(path_newid);
        }
    }

    this->pathsList.Add(path_newid, path);
}

#endif // NGAME

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
