#include "precompiled/pchrnsgameplay.h"

/**
    @file ncgameplayvehicleseatclass_cmds.cc

    (C) Conjurer Services, S.A. 2005
*/

#include "ncgameplayvehicleseat/ncgameplayvehicleseatclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayVehicleSeatClass)
    NSCRIPT_ADDCMD_COMPCLASS('DSST', void, SetSeatType, 1, (const ncGameplayVehicleSeatClass::type), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGST', const ncGameplayVehicleSeatClass::type, GetSeatType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSMY', void, SetMaxYaw, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGMY', const float, GetMaxYaw, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSIY', void, SetMinYaw, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGIY', const float, GetMinYaw, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSMP', void, SetMaxPitch, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGMP', const float, GetMaxPitch, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DSIP', void, SetMinPitch, 1, (const float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DGIP', const float, GetMinPitch, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncGameplayVehicleSeatClass::SaveCmds (nPersistServer* ps)
{
    if ( nComponentClass::SaveCmds(ps) == false )
    {
        return false;
    }

    nCmd* cmd(0);

    // persisting type of seat
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSST');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetI( this->GetSeatType() );

    ps->PutCmd(cmd);        

    // persisting max Yaw
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSMY');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMaxYaw() );

    ps->PutCmd(cmd);        

    // persisting min Yaw
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSIY');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMinYaw() );

    ps->PutCmd(cmd);        

    // persisting max Pitch
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSMP');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMaxPitch() );

    ps->PutCmd(cmd);        

    // persisting min Pitch
    cmd = ps->GetCmd( this->GetEntityClass(), 'DSIP');

    n_assert2( cmd, "Error command not found" );

    cmd->In()->SetF( this->GetMinPitch() );

    ps->PutCmd(cmd);        

    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
