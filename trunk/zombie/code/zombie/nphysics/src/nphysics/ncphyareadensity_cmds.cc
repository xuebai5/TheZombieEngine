//-----------------------------------------------------------------------------
//  ncphyareadensity_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyareadensity.h"
#include "kernel/npersistserver.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyAreaDensity)
    NSCRIPT_ADDCMD_COMPOBJECT('DSRF', void, SetResistance, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGRF', phyreal, GetResistance, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool 
ncPhyAreaDensity::SaveCmds(nPersistServer* ps)
{
    if( !ncPhySimpleObj::SaveCmds(ps) )
    {
        return false;
    }

    nCmd* cmd(ps->GetCmd( this->entityObject, 'DSRF'));

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF(this->GetResistance());

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
