//------------------------------------------------------------------------------
//  ncgameplayplayerclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "ncgameplayplayer/ncgameplayplayerclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayPlayerClass)
    NSCRIPT_ADDCMD_COMPCLASS('LSLP', void, SetLocalPlayer, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGLP', bool, IsLocalPlayer , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSJV', void, SetJumpVertical, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGJV', float, GetJumpVertical , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSJH', void, SetJumpHorizontal, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGJH', float, GetJumpHorizontal , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSJT', void, SetJumpTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGJT', float, GetJumpTime , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FSMS', void, SetMaxStamina, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FGMS', int, GetMaxStamina, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FSJL', void, SetJumpStaminaLost, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FGJL', int, GetJumpStaminaLost, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FSSL', void, SetSprintStaminaLost, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FGSL', int, GetSprintStaminaLost, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FSTT', void, SetTiredTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FGTT', float, GetTiredTime, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FSSR', void, SetStaminaRecover, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('FGSR', int, GetStaminaRecover, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSPH', void, SetPlayerHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGPH', float, GetPlayerHeight , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSPW', void, SetPlayerWide, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGPW', float, GetPlayerWide, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSSH', void, SetStandHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGSH', float, GetStandHeight , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSCH', void, SetCrouchHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGCH', float, GetCrouchHeight , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSRH', void, SetProneHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGRH', float, GetProneHeight , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSLD', void, SetLeanDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGLD', float, GetLeanDistance , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @retval true if save is ok
*/
bool
ncGameplayPlayerClass::SaveCmds (nPersistServer* ps)
{
    if ( ! ncGameplayLivingClass::SaveCmds(ps) )
    {
        return false;
    }

    ps->Put( this->entityClass, 'LSLP', this->isLocalPlayer );
    ps->Put( this->entityClass, 'LSJH', this->jumpHorizontal );
    ps->Put( this->entityClass, 'LSJV', this->jumpVertical );
    ps->Put( this->entityClass, 'LSJT', this->jumpTime );
    ps->Put( this->entityClass, 'FSMS', this->maxStamina );
    ps->Put( this->entityClass, 'FSSL', this->sprintStamina );
    ps->Put( this->entityClass, 'FSJL', this->jumpStamina );
    ps->Put( this->entityClass, 'FSTT', this->tiredTime );
    ps->Put( this->entityClass, 'FSSR', this->staminaRecover );
    ps->Put( this->entityClass, 'LSPH', this->playerHeight );
    ps->Put( this->entityClass, 'LSPW', this->playerWide );
    ps->Put( this->entityClass, 'LSSH', this->standHeight );
    ps->Put( this->entityClass, 'LSCH', this->crouchHeight );
    ps->Put( this->entityClass, 'LSRH', this->proneHeight );
    ps->Put( this->entityClass, 'LSLD', this->leanDistance );

    return true;
}

//------------------------------------------------------------------------------
