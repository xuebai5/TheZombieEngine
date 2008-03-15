#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncaistate_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncaistate/ncaistate.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncAIState)
    NSCRIPT_ADDCMD_COMPOBJECT('ISTG', void, SetTargetID, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGTG', nEntityObjectId, GetTargetID, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGTG', nEntityObject*, GetTarget, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EHTG', bool, HasTarget, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSAT', void, SetAbandonCurrentTarget, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGAT', bool, GetAbandonCurrentTarget, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ISAW', void, SetActiveWeapon, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('IGAW', nEntityObjectId, GetActiveWeaponID, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSCA', void, SetCallingAllyId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGCA', nEntityObjectId, GetCallingAllyId, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RTCA', bool, ThereIsCallingAlly, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSFR', void, SetFightRingIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RGFR', int, GetFightRingIndex, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RSLS', void, SetLeapSuccess, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('RILS', bool, IsLeapSuccess, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSDP', void, SetDestPoint, 1, (vector3), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGDP', vector3, GetDestPoint, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FSTP', void, SetTargetPoint, 1, (vector3), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGTP', vector3, GetTargetPoint, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
ncAIState::SaveCmds (nPersistServer* ps)
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // -- target
        int idTarget = this->GetTargetID();
        if (!ps->Put (this->entityObject, 'ISTG', idTarget))
        {
            return false;
        }

        // -- active weapon
        int idWeapon = this->GetActiveWeaponID();
        if (!ps->Put (this->entityObject, 'ISAW', idWeapon))
        {
            return false;
        }

        // -- fightRingIdx
        if (!ps->Put (this->entityObject, 'RSFR', this->fightRingIdx))
        {
            return false;
        }
    }

    return true;
}
