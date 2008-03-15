#include "precompiled/pchrnsgameplay.h"
//-----------------------------------------------------------------------------
//  ncgameplayliving_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "ncgameplayliving/ncgameplayliving.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayLiving)
    NSCRIPT_ADDCMD_COMPOBJECT('ISHH', void, SetHealth, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGHH', int, GetHealth, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISGS', void, SetGPState, 1, (unsigned int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGGS', unsigned int, GetGPState, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IACA', void, AddCommandAura, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IRCA', void, RemoveCommandAura, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISCA', void, SetCommandAura, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGCA', int, GetCommandAura, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIHY', bool, IsHealthy, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIWD', bool, IsWounded, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIIN', bool, IsIncapacitated, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISSN', void, SetStunned, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IISN', bool, IsStunned, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSDD', void, SetDead, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIDD', bool, IsDead, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISIV', void, SetInvulnerable, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIIV', bool, IsInvulnerable, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISZZ', void, SetDizzy, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIZZ', bool, IsDizzy, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISRT', void, SetRegenRate, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGRT', int, GetRegenRate, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSRH', void, SetRecoverHealth, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGRH', int, GetRecoverHealth, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSRF', void, SetRegenFactor, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGRF', float, GetRegenFactor, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISBH', void, SetBreath, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGBH', float, GetBreath, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISAE', void, SetAttackSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGAE', int, GetAttackSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISBS', void, SetBrainStopped, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIBS', bool, IsBrainStopped, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_SF', void, SetFrozen, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('I_IF', bool, IsFrozen, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISSD', void, SetSensoryDisabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IISD', bool, IsSensoryDisabled, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISBD', void, SetBlind, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIBD', bool, IsBlind, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISDF', void, SetDeaf, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIDF', bool, IsDeaf, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISIB', void, SetInvisible, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIIB', bool, IsInvisible, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISSL', void, SetSilent, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IISL', bool, IsSilent, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISCE', void, SetCombatEngage, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIIC', bool, IsInCombat, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISHD', void, SetHeld, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IIHD', bool, IsHeld, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSBS', void, SetBlocking, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RIBS', bool, IsBlocking, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSRR', void, SetRecovering, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FIRR', bool, IsRecovering, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSCW', void, SetCurrentWeapon, 1, (nEntityObject *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGCW', nEntityObject *, GetCurrentWeapon , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LAPD', void, ApplyDamage, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGIR', bool, IsIronsight , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISCB', void, SetCarriedByID, 1, (unsigned int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGCB', unsigned int, GetCarriedByID, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RIMR', bool, IsInMeleeRange, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('IIPT', bool, IsSprinting, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISPG', void, SetSprinting, 1, (bool), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISCU', bool, IsCrouching, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISCH', void, SetCrouching, 1, (bool), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISCW', bool, IsProne, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ICRA', void, SetProning, 1, (bool), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISWM', bool, IsSwimming, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPOBJECT('ISWG', void, SetSwimming, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSHR', void, SetHasRegen, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('F_HR', bool, HasRegen, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSRI', void, SetPathId, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGRI', int, GetPathId, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSLP', void, SetLastWayPoint, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGLP', int, GetLastWayPoint, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGIN', nInventoryContainer *, GetInventory , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSBY', void, SetBusy, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGBY', int, GetBusy , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LIBY', bool, IsBusy , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSME', void, SetMovementEvent , 1, (nGameEvent::Type), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGME', nGameEvent::Type, GetMovementEvent , 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncGameplayLiving::SaveCmds (nPersistServer* ps)
{
    if ( ! ncGameplay::SaveCmds(ps) )
    {
        return false;        
    }

    // -------------------- Route state --------------------------
    // -- Route
    if ( ! ps->Put (this->entityObject, 'FSRI', this->pathId) )
    {
        return false;
    }
    // -- Last waypoint
    if ( ! ps->Put (this->entityObject, 'FSLP', this->lastWayPoint) )
    {
        return false;
    }

    // -------------------- Health state --------------------------
    // -- state
    if ( ! ps->Put (this->entityObject, 'ISGS', this->gpState) )
    {
        return false;
    }
    // -- health
    if ( ! ps->Put (this->entityObject, 'ISHH', this->health) )
    {
        return false;
    }
    // -- turnRadius
    if ( ! ps->Put (this->entityObject, 'ISRT', this->regenRate) )
    {
        return false;
    }
    // -- Is recovering
    if ( ! ps->Put (this->entityObject, 'FSRR', this->isRecovering) )
    {
        return false;
    }
    // -- recoverHealth
    if ( ! ps->Put (this->entityObject, 'FSRH', this->recoverHealth) )
    {
        return false;
    }
    // -- has regen
    if ( ! ps->Put (this->entityObject, 'FSHR', this->hasRegen) )
    {
        return false;
    }

    // -------------------- Action states -----------------------
    // -- isSprint
    if ( ! ps->Put (this->entityObject, 'ISPG', this->isSprint) )
    {
        return false;
    }
    // -- isCrouch
    if ( ! ps->Put (this->entityObject, 'ISCH', this->isCrouch) )
    {
        return false;
    }
    // -- isProne
    if ( ! ps->Put (this->entityObject, 'ICRA', this->isProne) )
    {
        return false;
    }
    // -- isSwimming
    if ( ! ps->Put (this->entityObject, 'ISWM', this->isSwimming) )
    {
        return false;
    }

    // -- breath
    if ( ! ps->Put (this->entityObject, 'ISBH', this->breath) )
    {
        return false;
    }

    // ------------------- Combat attributes ----------------------
    // -- attackSpeed
    if ( ! ps->Put (this->entityObject, 'ISAE', this->attackSpeed) )
    {
        return false;
    }

    // ------------------ Game play attributes --------------------
    // -- movementEvent
     if ( ! ps->Put (this->entityObject, 'FSME', this->movementEvent) )
    {
        return false;
    }
    // -- commandAura
    if ( ! ps->Put (this->entityObject, 'ISCA', this->commandAura) )
    {
        return false;
    }

    // -- carriedBy
    unsigned int idCarriedBy = this->GetCarriedByID();
    if (!ps->Put (this->entityObject, 'ISCB', idCarriedBy))
    {
        return false;
    }

    //this->InitRefs();
   
    return true;
}