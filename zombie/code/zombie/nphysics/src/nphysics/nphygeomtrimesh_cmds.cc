//-----------------------------------------------------------------------------
//  nphygeomtrimesh_cmds.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomtrimesh.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomtrimesh

    @cppclass
    nPhyGeomTriMesh

    @superclass
    nPhysicsGeom

    @classinfo
    A triangle mesh (TriMesh) represents an arbitrary collection of triangles.
*/
NSCRIPT_INITCMDS_BEGIN(nPhyGeomTriMesh)
    NSCRIPT_ADDCMD('DSTF', void, SetFile, 1, (const nString&), 0, ());
NSCRIPT_INITCMDS_END()
//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhyGeomTriMesh::SaveCmds(nPersistServer* ps)
{
    if( !nPhysicsGeom::SaveCmds(ps) )
        return false;

    nCmd* cmd(ps->GetCmd( this, 'DSTF'));

    n_assert2( cmd, "Error command not found" );

    /// Storing the file to be loaded
    cmd->In()->SetS( GetFileName().Get() );

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
