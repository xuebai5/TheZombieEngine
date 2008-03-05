//------------------------------------------------------------------------------
//  ncgameplayplayer_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "ncgameplayplayer/ncgameplayplayer.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayPlayer)
    NSCRIPT_ADDCMD_COMPOBJECT('LGLP', bool, IsLocalPlayer , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSST', void, SetStamina, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGST', int, GetStamina, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSHS', void, SetHasStamina, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FHHS', bool, HasStamina, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSSQ', void, SetSpecialEquipment, 2, (int,bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGSQ', bool, GetSpecialEquipment, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSEL', void, SetElevationLimits, 1, (const vector2&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGEL', void, GetElevationLimits, 0, (), 1, (vector2&));
    NSCRIPT_ADDCMD_COMPOBJECT('LSTL', void, SetTurnLimits, 1, (const vector2&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGTL', void, GetTurnLimits, 0, (), 1, (vector2&));
    NSCRIPT_ADDCMD_COMPOBJECT('LSAC', void, SetAngleCenter, 1, (const vector2&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGAC', void, GetAngleCenter, 0, (), 1, (vector2&));
    NSCRIPT_ADDCMD_COMPOBJECT('LGIC', bool, IsInScope , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGSI', void, SetInputState , 1, (ncGameplayPlayer::state), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGGI', ncGameplayPlayer::state, GetInputState , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @retval true if save is ok
*/
bool
ncGameplayPlayer::SaveCmds (nPersistServer* ps)
{
    if ( ! ncGameplayLiving::SaveCmds(ps) )
    {
        return false;
    }
    ps->Put( this->entityObject, 'FSST', this->stamina );

    return true;
}

//------------------------------------------------------------------------------
