//------------------------------------------------------------------------------
//  ncgpweaponmeleeclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpweaponmeleeclass.h"

nNebulaComponentClass(ncGPWeaponMeleeClass,nComponentClass);

//------------------------------------------------------------------------------
/**
*/
ncGPWeaponMeleeClass::ncGPWeaponMeleeClass():
    damageMelee(10),
    meleeDamageType(nDamageManager::DAMAGE_MELEE),
    meleeRange(1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPWeaponMeleeClass::~ncGPWeaponMeleeClass()
{
    // empty
}

//------------------------------------------------------------------------------
