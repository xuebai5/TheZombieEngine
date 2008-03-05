//-----------------------------------------------------------------------------
//  ncphyterrain_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyterrain.h"
#include "kernel/npersistserver.h"
#include "ngeomipmap/nterrainline.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncPhyTerrain,ncPhysicsObj);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyTerrain)
    NSCRIPT_ADDCMD_COMPOBJECT('DCAH', nTerrainLine*, CreateHole, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DALH', void, AddLastHole, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
ncPhyTerrain::SaveCmds(nPersistServer* /*server*/ )
{

    return true;
}

//-----------------------------------------------------------------------------
/**
    Persist holes.

    history:
        - 12-Jan-2006   David Reyes     Created
*/
void ncPhyTerrain::SaveHoles()
{
#ifndef NGAME
    nString holesPath("level:holes/");

    nFileServer2 *fileServer(nFileServer2::Instance());

    if(!fileServer->DirectoryExists(holesPath.Get()))
    {
        // if the waypoints directory doesn't exist yet, create it
        fileServer->MakePath(holesPath.Get());
    }

    nString realHolesPath(fileServer->ManglePath(holesPath.Get()));

    realHolesPath.Append( "/holes.n2" );

    nPersistServer *server(nKernelServer::Instance()->GetPersistServer());
    
    n_assert2(server, "Not able to access to the persist server.");

    nCmd * cmd = server->GetCmd(this->GetEntityObject(), 'THIS');

    if( server->BeginObjectWithCmd(this->GetEntityObject(), cmd, realHolesPath.Get() ) )
    {
        for( int index(0); index < this->GetNumHoles(); ++index )
        {
            nCmd* cmd(server->GetCmd( this->entityObject, 'DCAH'));

            nTerrainLine* line( this->GetHole( index ) );

            if( server->BeginObjectWithCmd(this->GetEntityObject(), cmd) )
            {
                if( line->SaveCmds( server ) == false )
                {
                    server->EndObject(true);
                    return;
                }

                server->EndObject(true);
            }

            // add a function to close
            cmd = server->GetCmd( this->entityObject, 'DALH');
            server->PutCmd(cmd);
        }

        server->EndObject(true);
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Loads holes.

    history:
        - 12-Jan-2006   David Reyes     Created
*/
void ncPhyTerrain::Load()
{
    // begin loading

    nString holesPath("level:holes/");

    nFileServer2 *fileServer(nFileServer2::Instance());

    nString realHolesPath(fileServer->ManglePath(holesPath.Get()));

    realHolesPath.Append( "/holes.n2" );

    nKernelServer::Instance()->PushCwd(this->GetEntityObject());
    nKernelServer::Instance()->Load(realHolesPath.Get(), false);
    nKernelServer::Instance()->PopCwd();

    // end loading
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
