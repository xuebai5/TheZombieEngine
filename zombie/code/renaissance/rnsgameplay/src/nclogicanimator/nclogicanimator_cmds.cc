#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  nclogicanimator_cmds.cc
//  (C) Conjurer Services, S.A. 2006
//------------------------------------------------------------------------------

#include "nclogicanimator/nclogicanimator.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncLogicAnimator)
    NSCRIPT_ADDCMD_COMPOBJECT('ISAS', void, SetAnimatorState, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISAC', void, SetAction, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISPV', void, SetPrevState, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSPS', void, SetMovementPose, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_SW', int, SetWalk , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_SR', int, SetRun , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSSL', int, SetStrafeLeft, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSSR', int, SetStrafeRight, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISPT', int, SetSprint, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_SI', int, SetIdle, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSCE', int, SetCombatIdle, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSDI', int, SetDrivingIdle, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_SD', int, SetDie, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISDR', int, SetDrive, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISSW', int, SetSwim, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISDV', int, SetDive, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISSH', int, SetShoot, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSSA', void, SetShootAuto, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSMA', int, SetMeleeAttack, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSMB', int, SetMeleeBlock, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSIA', int, SetImpact, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSMI', int, SetMiniImpact, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RLCA', int, SetLeftClawAttack, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RRCA', int, SetRightClawAttack, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RBCA', int, SetBothClawsAttack, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IFLY', int, SetFly, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSJS', int, SetJumpStart, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LJMM', int, SetJumpMid, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LJME', int, SetJumpEnd, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSJO', int, SetJumpObstacle, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSJA', int, SetJumpAttack, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RJES', int, SetJumpEndSuc, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RJEF', int, SetJumpEndFail, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSBC', int, SetBattleCry, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IMOP', int, SetMop, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_SL', int, SetLeap, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('R_ST', int, SetTaunt, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_SB', int, SetBurrow, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IRLL', int, SetRoll, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_SC', int, SetClimb, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISQF', int, SetFastFirstReload, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISQS', int, SetFastSecondReload, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISLR', int, SetFullReload, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISFF', int, SetFullFirstReload, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISFS', int, SetFullSecondReload, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISER', int, SetEndReload, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISCR', int, SetCockReload, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISHW', int, SetHideWeapon, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISHO', int, SetShowWeapon, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LWCC', int, SetWeaponAcc, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSSI', int, SetInjection, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSLA', int, SetLookAround, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSSO', int, SetSilencerOn, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSSF', int, SetSilencerOff, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSFM', int, SetSwitchFireMode, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISIS', int, SetIronsight, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISLS', int, SetLeanIronsight, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISCS', int, SetCrouch, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISPR', int, SetProne, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISSS', int, SetSeat, 1, (bool), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISLE', int, SetLean, 1, (bool), 0, ());
	/*NSCRIPT_ADDCMD_COMPOBJECT('ISRL', int, SetRightLean, 1, (bool), 0, ());*/
    NSCRIPT_ADDCMD_COMPOBJECT('FSFL', int, SetFlashlight, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_HF', bool, HasFinished, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGRA', int, GetIndexRandomState, 2, (const char*,bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISAA', int, SetAnimationAction, 4, (int, bool, bool, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_FA', bool, HasFinishedAllAnims, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGAD', int, GetRandomIndexAnimation, 0, (), 0, ());

NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncLogicAnimator::SaveCmds (nPersistServer* ps)
{
    if ( ncCharacter::SaveCmds(ps) )
    {
        // -- state
        ps->Put (this->entityObject, 'ISAS', this->state);
        // -- action
        ps->Put (this->entityObject, 'ISAC', this->action);
        // -- prev state
        ps->Put (this->entityObject, 'ISPV', this->prevState);

        return true;
    }

    return false;
}