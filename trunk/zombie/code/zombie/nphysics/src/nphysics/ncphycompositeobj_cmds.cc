//-----------------------------------------------------------------------------
//  ncphycompositeobj_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphycompositeobj.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncPhyCompositeObj,ncPhysicsObj);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyCompositeObj)
    NSCRIPT_ADDCMD_COMPOBJECT('DDDG', void, Add, 1, (nPhysicsGeom*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DEMG', void, Remove, 1, (nPhysicsGeom*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DNOB', nPhysicsGeom*, CreateGeometryObj, 1, (const nString&), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool 
ncPhyCompositeObj::SaveCmds( nPersistServer* server )
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
bool ncPhyCompositeObj::SaveChunk(nPersistServer* server)
{
    int Size(containerGeometries.Size());

    for( int i(0); i < Size; ++i )
    {
        nPhyGeomTrans* trans(
            static_cast<nPhyGeomTrans*>(containerGeometries.GetElementAt( i )));

        n_assert2( trans, "Null pointer" );

        nPhysicsGeom* geometry( trans->GetGeometry() );

        n_assert2( geometry, "Null pointer" );

        nCmd* cmd(server->GetCmd( this->entityObject, 'DNOB'));
        
        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetS( geometry->GetClass()->GetName() );

        if (server->BeginObjectWithCmd(this->GetEntityObject(), cmd)) 
        {
            if( !geometry->SaveCmds( server ) )
            {
                return false;
            }
            server->EndObject(true);
        }
    }    

    return ncPhysicsObj::SaveChunk(server);
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
