//-----------------------------------------------------------------------------
//  ncphysimpleobj_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphysimpleobj.h"
#include "kernel/npersistserver.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhySimpleObj)
    NSCRIPT_ADDCMD_COMPOBJECT('DGEO', void, SetGeometry, 1, (nPhysicsGeom*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DNOB', nPhysicsGeom*, CreateGeometryObj, 1, (const nString&), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
ncPhySimpleObj::SaveCmds(nPersistServer* server )
{
#ifdef __ZOMBIE_EXPORTER__
    if( this->isInIndoor )
    {
        this->SaveChunk(server);
    }
#endif
    return ncPhysicsObj::SaveCmds(server);
}

//------------------------------------------------------------------------------
/**
    Object chunk persistency.
*/
bool
ncPhySimpleObj::SaveChunk(nPersistServer* server)
{
    if( this->GetGeometry() )
    {

        nCmd* cmd(server->GetCmd( this->entityObject, 'DNOB'));
        
        cmd->In()->SetS( this->GetGeometry()->GetClass()->GetName() );

        n_assert2( cmd, "Error command not found" );

        if (server->BeginObjectWithCmd(this->GetEntityObject(), cmd))
        {
            if( !this->GetGeometry()->SaveCmds( server ) )
            {
                return false;
            }
            server->EndObject(false);
        }
    }

    return ncPhysicsObj::SaveChunk(server);
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
