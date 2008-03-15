#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncgameplaylivingclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncgameplayliving/ncgameplaylivingclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGameplayLivingClass)
	NSCRIPT_ADDCMD_COMPCLASS('ISXX', void, SetBaseSpeed, 1, (float), 0, ());
	NSCRIPT_ADDCMD_COMPCLASS('IGXX', float, GetBaseSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F001', void, SetMultSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F002', int, GetMultSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGSP', float, GetSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F004', void, SetMultRunSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F005', int, GetMultRunSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGRS', float, GetRunSpeed, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPCLASS('F006', void, SetMultIronsightSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F007', int, GetMultIronsightSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGIS', float, GetIronsightSpeed, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPCLASS('F008', void, SetMultRunIronsightSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F009', int, GetMultRunIronsightSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGIR', float, GetRunIronsightSpeed, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPCLASS('F010', void, SetMultCrouchSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F011', int, GetMultCrouchSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGCH', float, GetCrouchSpeed, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPCLASS('F012', void, SetMultProneSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F013', int, GetMultProneSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGCL', float, GetProneSpeed, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPCLASS('F014', void, SetMultCrouchIronSightSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F015', int, GetMultCrouchIronSightSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGHI', float, GetCrouchIronSightSpeed, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPCLASS('F016', void, SetMultProneIronSightSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F017', int, GetMultProneIronSightSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGLI', float, GetProneIronSightSpeed, 0, (), 0, ());
	NSCRIPT_ADDCMD_COMPCLASS('F018', void, SetMultSwimSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('F019', int, GetMultSwimSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGWE', float, GetSwimSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RSTS', void, SetTurnSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGTS', float, GetTurnSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISTR', void, SetTurnRadius, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGTR', float, GetTurnRadius, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ESMT', void, SetMaxAngularVelocityInDegrees, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGMT', float, GetMaxAngularVelocityInDegrees, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISJH', void, SetJumpHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGJH', float, GetJumpHeight, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RSJS', void, SetJumpSpeed, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGJS', float, GetJumpSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISSZ', void, SetSize, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGSZ', float, GetSize, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISMC', void, SetMaxClimbSlope, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGMC', float, GetMaxClimbSlope, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISFD', void, SetFallingDamageRatio, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGFD', float, GetFallingDamageRatio, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISEH', void, SetEyeHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGEH', float, GetEyeHeight, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISUR', void, SetUseRange, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGUR', float, GetUseRange, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JSSC', void, SetStepsAnimCycle, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JGSC', int, GetStepsAnimCycle, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISCC', void, SetCanBeCarried, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ICBC', bool, CanBeCarried, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISPU', void, SetPickUp, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGPU', bool, GetPickUp, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISIS', void, SetInventorySize, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGIS', int, GetInventorySize, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISMH', void, SetMaxHealth, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGMH', int, GetMaxHealth, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISRT', void, SetRegenThreshold, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGRT', float, GetRegenThreshold, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISRA', void, SetRegenAmount, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGRA', float, GetRegenAmount, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISGS', void, SetGameplayLivingState, 1, (unsigned int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGST', unsigned int, GetGameplayLivingState, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISCS', void, SetCanBeStunned, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ICBS', bool, CanBeStunned, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISCB', void, SetCanBeIncapacitated, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ICBI', bool, CanBeIncapacitated, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISBK', void, SetCanBeKilled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ICCK', bool, CanBeKilled, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISNA', void, SetNeedsAir, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGNA', bool, GetNeedsAir, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISBM', void, SetBreathMax, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGBM', float, GetBreathMax, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISLA', void, SetBreathLossAmount, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGLA', float, GetBreathLossAmount, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISLS', void, SetBreathLossSpeed, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGLS', float, GetBreathLossSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISBR', void, SetBreathRecoverySpeed, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGBR', float, GetBreathRecoverySpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISSR', void, SetSightRadius, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGSR', float, GetSightRadius, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISFA', void, SetFOVAngle, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGFA', float, GetFOVAngle, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISER', void, SetHearingRadius, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGER', float, GetHearingRadius, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISFR', void, SetFeelingRadius, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGFR', float, GetFeelingRadius, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISMT', void, SetMemoryTime, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGMT', int, GetMemoryTime, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISCR', void, SetCommRadius, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGCR', float, GetCommRadius, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISCI', void, SetCommIntensity, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGCI', float, GetCommIntensity, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISMR', void, SetMeleeRange, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGMR', float, GetMeleeRange, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ISAS', void, SetAttackSpeed, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('IGAS', float, GetAttackSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RSDM', void, SetDamageModification, 2, (ncGameplayLivingClass::BodyPart, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RGDM', float, GetDamageModification, 1, (ncGameplayLivingClass::BodyPart), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RRSR', void, ResetRingsInfo, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RAFR', void, AddFightRing, 2, (float, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RRFR', void, RemoveFightRing, 2, (float, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('RRRI', void, RemoveFightRingIndex, 1, (int), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncGameplayLivingClass::SaveCmds (nPersistServer* ps)
{
    if ( ncGameplayClass::SaveCmds(ps) )
    {
		// -- baseSpeed
		ps->Put (this->entityClass, 'ISXX', this->baseSpeed);

		// -- speed
        ps->Put (this->entityClass, 'F001', this->multSpeed);
        // -- runSpeed
        ps->Put (this->entityClass, 'F004', this->multRunSpeed);
        // -- ironsightSpeed
		if (!ps->Put (this->entityClass, 'F006', this->multIronsightSpeed))
        {
            return false;
        }
		// -- runIronsightSpeed
		ps->Put (this->entityClass, 'F008', this->multRunIronsightSpeed);
		// -- crouchSpeed
		ps->Put (this->entityClass, 'F010', this->multCrouchSpeed);
		// -- proneSpeed
		ps->Put (this->entityClass, 'F012', this->multProneSpeed);
		// -- crouchIronsightSpeed
		ps->Put (this->entityClass, 'F014', this->multCrouchIronsightSpeed);
		// -- proneIronSightSpeed
		ps->Put (this->entityClass, 'F016', this->multProneIronSightSpeed);
		// -- swimSpeed
		ps->Put (this->entityClass, 'F018', this->multSwimSpeed);
        // -- turnSpeed
        if (!ps->Put (this->entityClass, 'RSTS', this->turnSpeed))
        {
            return false;
        }
        // -- turnRadius
        ps->Put (this->entityClass, 'ISTR', this->turnRadius);
        // -- maxAngularVelocity
        ps->Put (this->entityClass, 'ESMT', this->GetMaxAngularVelocityInDegrees());
        // -- jumpHeight
        ps->Put (this->entityClass, 'ISJH', this->jumpHeight);
        // -- jumpSpeed
        if (!ps->Put (this->entityClass, 'RSJS', this->jumpSpeed))
        {
            return false;
        }
        // -- size
        ps->Put (this->entityClass, 'ISSZ', this->size);
        // -- maxClimbSlope
        ps->Put (this->entityClass, 'ISMC', this->maxClimbSlope);
        // -- fallingDamageRatio
        ps->Put (this->entityClass, 'ISFD', this->fallingDamageRatio);
        // -- eyeHeight
        ps->Put (this->entityClass, 'ISEH', this->eyeHeight);
        // -- useRange
        ps->Put (this->entityClass, 'ISUR', this->useRange);
        
        // ---------------------------------- Inventory attributes --        
        // -- inventorySize
        ps->Put (this->entityClass, 'ISIS', this->inventorySize);

        // ------------------------------------- Health attributes --        
        // -- maxHealth        
        ps->Put (this->entityClass, 'ISMH', this->maxHealth);
        // -- regenThreshold
        ps->Put (this->entityClass, 'ISRT', this->regenThreshold);
        // -- regenAmount
        ps->Put (this->entityClass, 'ISRA', this->regenAmount);
        
        // ------------------------------------- Breath attributes --
        // -- breathMax
        ps->Put (this->entityClass, 'ISBM', this->breathMax);
        // -- breathLossAmount
        ps->Put (this->entityClass, 'ISLA', this->breathLossAmount);
        // -- breathLossSpeed
        ps->Put (this->entityClass, 'ISLS', this->breathLossSpeed);
        // -- breathRecoverySpeed
        ps->Put (this->entityClass, 'ISBR', this->breathRecoverySpeed);

        // --------------------------------- Perception attributes --        
        // -- sightRadius
        ps->Put (this->entityClass, 'ISSR', this->sightRadius);
        // -- FOVAngle
        ps->Put (this->entityClass, 'ISFA', this->FOVAngle);
        // -- hearingRadius
        ps->Put (this->entityClass, 'ISER', this->hearingRadius);
        // -- feelingRadius
        ps->Put (this->entityClass, 'ISFR', this->feelingRadius);
        // -- memoryTime
        ps->Put (this->entityClass, 'ISMT', this->memoryTime);

        // ------------------------------ Communication attributes --
        // -- commRadius
        ps->Put (this->entityClass, 'ISCR', this->commRadius);
        // -- commIntensity
        ps->Put (this->entityClass, 'ISCI', this->commIntensity);

        // ------------------------------------------- Class state --
        // -- state
        ps->Put (this->entityClass, 'ISGS', this->state);

        // ------------------------------------- Combat attributes --
        // -- meleeRange
        ps->Put (this->entityClass, 'ISMR', this->meleeRange);         
        // -- attackSpeed
        ps->Put (this->entityClass, 'ISAS', this->attackSpeed);

        // reset rings information
        if (!ps->Put(this->entityClass, 'RRSR'))
        {
            return false;
        }

        // -- rings information
        for (int i = 0; i < this->ringsInfo->Size(); i++)
        {
            if (!ps->Put(this->entityClass, 'RAFR', this->ringsInfo->At(i).radius, this->ringsInfo->At(i).size))
            {
                return false;
            }
        }
    }
    
    return true;
}