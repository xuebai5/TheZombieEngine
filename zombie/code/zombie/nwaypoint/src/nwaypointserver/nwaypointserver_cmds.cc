//-----------------------------------------------------------------------------
//  nwaypointserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnwaypoint.h"
#include "nwaypointserver/nwaypointserver.h"
#include "ncwaypointpath/ncwaypointpath.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nwaypointserver

    @cppclass
    nWayPointServer

    @superclass
    nRoot

    @classinfo
    A waypoint manager.
*/
NSCRIPT_INITCMDS_BEGIN(nWayPointServer)
    NSCRIPT_ADDCMD('DNWP', nWayPointServer::waypointid, CreateNewWayPoint, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('DCTW', const int, CreatePathWithTwoWayPoints, 3, (const nWayPointServer::waypointid,const nWayPointServer::waypointid, const nString&), 0, ());
    NSCRIPT_ADDCMD('DCPW', const int, CreateNewPath, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('DDPW', void, DestroysPath, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('DAWP', void, AddWayPointToPath, 2, (const nWayPointServer::waypointid,const nString&), 0, ());
    NSCRIPT_ADDCMD('DRWP', void, RemoveWayPointFromPath, 2, (const nWayPointServer::waypointid,const nString&), 0, ());
    NSCRIPT_ADDCMD('DCWP', const bool, CheckWayPointInPath, 2, (const WayPoint::waypointid,const nString&), 0, ());
    NSCRIPT_ADDCMD('DIWP', void, InsertWayPointInPath, 3, (const WayPoint::waypointid,const WayPoint::waypointid,const WayPoint::waypointid), 0, ());
    NSCRIPT_ADDCMD('DRIP', void, RemoveWayPointFromItsPath, 1, (const WayPoint::waypointid), 0, ());
    NSCRIPT_ADDCMD('DSPC', void, SetPathCreator, 1, (const nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('DIWB', void, InsertWPAtTheBackOfPath, 2, (const WayPoint::waypointid, const nString&), 0, ());
    NSCRIPT_ADDCMD('DRWL', void, RemoveWayPointFromList, 1, (const WayPoint::waypointid), 0, ());
    NSCRIPT_ADDCMD('DCRO', const int, CreateRoute, 4, (const nString&,int,int,float), 0, ());
    NSCRIPT_ADDCMD('DDRO', void, DestroyRoute, 1, (const int), 0, ());
    NSCRIPT_ADDCMD('DFRO', const bool, FollowRoute, 3, (const int,const float,const bool), 0, ());
    NSCRIPT_ADDCMD('DRPO', const vector3&, RoutePosition, 1, (const int), 0, ());
    NSCRIPT_ADDCMD('DGNW', const int, GetNumWayPoints, 1, (const int), 0, ());
    NSCRIPT_ADDCMD('DGPI', const int, GetPathId, 1, ( const nString& ), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('DGNP', const int, GetNumPaths, 0, (), 0, ());
    NSCRIPT_ADDCMD('DGPN', const nString&, GetPathName, 1, (const int), 0, ());
    NSCRIPT_ADDCMD('DGWP', nEntityObject*, GetWayPointFromPath, 2, (const int,const int), 0, ());
    NSCRIPT_ADDCMD('DRNM', void, RenamePath, 2, (const nString&, const nString&), 0, ());
#endif
NSCRIPT_INITCMDS_END()


#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Object persistency

    @param server persist server

    history:
        - 07-Oct-2004   David Reyes    created
*/
bool nWayPointServer::SaveCmds(nPersistServer* server)
{
    // persistance by paths
    for( int index(0); index < (1 << nEntityObjectServer::IDHIGHBITS); ++index )
    {
        tPathsCreator* creatorsWork(0);
        this->pathCreators.Find( index << nEntityObjectServer::IDLOWBITS, creatorsWork );

        // persisting creator's id
        nCmd* cmd( server->GetCmd( this, 'DSPC' ) );

        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetI( index << nEntityObjectServer::IDLOWBITS );

        server->PutCmd(cmd);

        if( !creatorsWork )
        {
            continue;
        }

        for( int inner(0); inner < creatorsWork->paths.Size(); ++inner )
        {
            const ncWayPointPath* path( creatorsWork->paths[ inner ] );

            cmd = server->GetCmd( this, 'DCPW' );

            cmd->In()->SetS( path->GetName().Get() );

            server->PutCmd(cmd);

            // persisting waypoints
            for( int iWP(0); iWP < path->GetNumWayPoints(); ++iWP )
            {
                cmd = server->GetCmd( this, 'DNWP' );

                cmd->In()->SetF(  path->GetWayPointIndex( iWP )->GetPosition().x );
                cmd->In()->SetF(  path->GetWayPointIndex( iWP )->GetPosition().y );
                cmd->In()->SetF(  path->GetWayPointIndex( iWP )->GetPosition().z );

                server->PutCmd(cmd);

                cmd = server->GetCmd( this, 'DIWB' );

                cmd->In()->SetI( 0 );

                cmd->In()->SetS( path->GetName().Get() );

                server->PutCmd(cmd);
            }
        }       
    }
    return true;
}

#endif
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------