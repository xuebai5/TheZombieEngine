//------------------------------------------------------------------------------
//  ncgpweaponclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpweaponclass.h"

nNebulaComponentClass(ncGPWeaponClass,ncGPWeaponCharsClass);

//------------------------------------------------------------------------------
/**
*/
ncGPWeaponClass::ncGPWeaponClass():
    rateOfFire(0.2f),
    rateOfTapFire(0.3f),
    cockTime(1),
    damageType(nDamageManager::DAMAGE_MELEE),
    ammoType(AMMO_MELEE),
    ammoChamber(1),
    ammoReq(AMMO_MELEE),
    addonOffsetBone(-1),
    typeShape(SHAPE_INSTANT),
    weaponType(WT_MELEE),
    fireModes(ncGPWeaponClass::FM_SEMI),
    defaultMagazine( "" )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPWeaponClass::~ncGPWeaponClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param value new fire mode
*/
void
ncGPWeaponClass::SetFireModes( int value )
{
    bool valid = ( value >= 0 ) && ( value < ncGPWeaponClass::FM_LAST );

    if( valid )
    {
        this->fireModes = value;
    }
}

//------------------------------------------------------------------------------
