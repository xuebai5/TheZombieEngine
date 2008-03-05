//-----------------------------------------------------------------------------
//  nphygeomray_cmds.cc
//  (C) 2003 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphygeomray.h"
#include "kernel/npersistserver.h"


//------------------------------------------------------------------------------
/**
    @scriptclass
    nphygeomray

    @cppclass
    nPhyGeomRay

    @superclass
    nObject

    @classinfo
    A ray physics geometry.
*/
NSCRIPT_INITCMDS_BEGIN(nPhyGeomRay)
    NSCRIPT_ADDCMD('DLEN', void, SetLength, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD('DGLE', phyreal, GetLength, 0, (), 0, ());
    NSCRIPT_ADDCMD('DDIR', void, SetDirection, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('DGDI', void, GetDirection, 0, (), 1, (vector3&));
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool
nPhyGeomRay::SaveCmds(nPersistServer* ps)
{
    if( !nPhysicsGeom::SaveCmds(ps) )
        return false;

    nCmd* cmd(ps->GetCmd( this, 'DDIR'));

    n_assert2( cmd, "Error command not found" );

    vector3 direction;

    this->GetDirection(direction);

    /// Setting the direction of the ray
    cmd->In()->SetF( direction.x );
    cmd->In()->SetF( direction.y );
    cmd->In()->SetF( direction.z );

    ps->PutCmd(cmd);

    cmd = ps->GetCmd( this, 'DLEN');
    
    /// Setting the ray length
    cmd->In()->SetF( this->GetLength() );

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
