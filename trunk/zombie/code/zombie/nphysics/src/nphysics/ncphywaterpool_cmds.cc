//-----------------------------------------------------------------------------
//  ncphywaterpool_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphywaterpool.h"
#include "kernel/npersistserver.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyWaterPool)
    NSCRIPT_ADDCMD_COMPOBJECT('DSPL', void, SetPoolsLength, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGPL', const vector3&, GetPoolsLength, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool 
ncPhyWaterPool::SaveCmds(nPersistServer* ps)
{
    // area density
    nCmd* cmd(ps->GetCmd( this->entityObject, 'DARR'));

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF(this->GetResistance());

    ps->PutCmd(cmd);

    // this entity
    cmd = ps->GetCmd( this->entityObject, 'DSPL');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF(this->lengthsPool.x);
    cmd->In()->SetF(this->lengthsPool.y);
    cmd->In()->SetF(this->lengthsPool.z);

    ps->PutCmd(cmd);

    // nphysicsobj
    cmd = ps->GetCmd( this->entityObject, 'DITW');

    n_assert2( cmd, "Error command not found" );

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

