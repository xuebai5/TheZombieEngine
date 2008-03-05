//-----------------------------------------------------------------------------
//  nphygeomsphere_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomsphere.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomsphere

    @cppclass
    nPhyGeomSphere

    @superclass
    nPhysicsGeom

    @classinfo
    An sphere physics geometry.
*/
NSCRIPT_INITCMDS_BEGIN(nPhyGeomSphere)
    NSCRIPT_ADDCMD('DRAD', void, SetRadius, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD('DGRA', phyreal, GetRadius, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhyGeomSphere::SaveCmds(nPersistServer* ps)
{
    if( !nPhysicsGeom::SaveCmds(ps) )
        return false;

    nCmd* cmd(ps->GetCmd( this, 'DRAD'));

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetRadius() );

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
