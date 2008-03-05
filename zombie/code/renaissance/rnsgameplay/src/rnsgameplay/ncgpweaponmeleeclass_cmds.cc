//------------------------------------------------------------------------------
//  ncgpweaponmeleeclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpweaponmeleeclass.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPWeaponMeleeClass)
    NSCRIPT_ADDCMD_COMPCLASS('LSEE', void, SetDamageMelee, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGEE', float, GetDamageMelee , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSMT', void, SetMeleeDamageType, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGMT', int, GetMeleeDamageType , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LGGI', float, GetMeleeRange , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('LSGI', void, SetMeleeRange, 1, (float), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    @param ps the persist Server to save the commands
    @retval true if save is ok
*/
bool
ncGPWeaponMeleeClass::SaveCmds( nPersistServer * ps )
{
    if( ! nComponentClass::SaveCmds(ps) )
    {
        return false;
    }

    ps->Put( this->GetEntityClass(), 'LSEE', this->GetDamageMelee() );
    ps->Put( this->GetEntityClass(), 'LSMT', this->GetMeleeDamageType() );

    return true;
}

//------------------------------------------------------------------------------
