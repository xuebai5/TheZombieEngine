//-----------------------------------------------------------------------------
//  ncphycharacter_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

#include "nphysics/ncphycharacter.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyCharacter)
    NSCRIPT_ADDCMD_COMPOBJECT('DSEH', void, SetHeight, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGEH', phyreal, GetHeight, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSEW', void, SetWide, 1, (phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGEW', phyreal, GetWide, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool ncPhyCharacter::SaveCmds(nPersistServer* ps)
{
    if( !ncPhyCompositeObj::SaveCmds( ps ) )
    {
        return false;
    }

    if( ps->GetSaveType() != nPersistServer::SAVETYPE_PERSIST )
    {
        return true;
    }

    nCmd* cmd(ps->GetCmd( this->entityObject, 'DSEH'));

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF(this->height);

    ps->PutCmd(cmd);

    cmd = ps->GetCmd( this->entityObject, 'DSEW');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF(this->wideness);

    ps->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
