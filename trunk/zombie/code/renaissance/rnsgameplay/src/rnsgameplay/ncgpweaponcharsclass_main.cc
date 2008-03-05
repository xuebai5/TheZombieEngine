//------------------------------------------------------------------------------
//  ncgpweaponcharsclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpweaponcharsclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncGPWeaponCharsClass, ncGameplayClass);

//------------------------------------------------------------------------------
const float ncGPWeaponCharsClass::DEVIATION_DISTANCE = 50.0f;
const float ncGPWeaponCharsClass::MAX_DEVIATION = 5.0f;

const ncGPWeaponCharsClass::SlotInfo ncGPWeaponCharsClass::slot_names[] = {
    { SLOT_TOP              , "top" },
    { SLOT_OVERBARREL       , "overbarrel" },
    { SLOT_UNDERBARREL      , "underbarrel" },
    { SLOT_UNDERBARREL_NOG  , "underbarrel_nog" },
    { SLOT_MOUNT            , "mount" },
    { SLOT_SIDERAIL         , "siderail" },
    { SLOT_SIDERAIL_NOG     , "siderail_nog" },
    { SLOT_MUZZLE_SMALL     , "muzzle_small" },
    { SLOT_MUZZLE_SMALL_NOG , "muzzle_small_nog" },
    { SLOT_MUZZLE_BIG       , "muzzle_big" },
    { SLOT_MUZZLE_BIG_NOG   , "muzzle_big_nog" },
    { SLOT_MAG_HICAP        , "mag_hicap" },
    { SLOT_MAG_CLAMP        , "mag_clamp" },
    { SLOT_AMMO             , "ammo" },
    {0,0}
};

//------------------------------------------------------------------------------
/**
*/
const char *
ncGPWeaponCharsClass::GetSlotName( int type )
{
    int index = 0;

    while( slot_names[ index ].name && slot_names[ index ].type != type)
    {
        ++index;
    }

    return slot_names[ index ].name;
}

//------------------------------------------------------------------------------
/**
*/
ncGPWeaponCharsClass::ncGPWeaponCharsClass():
    accuracyMin(0.7f),
    accuracyMax(0.9f),
    accuracyLoss(0.2f),
    accuracyProne(1.30f),
    accuracyCrouch(1.10f),
    accuracyMove(0.90f),
    accuracyRun(0.50f),
    accuracyAngle(0.35f),
    accuracyIronsight(1.0f),
    recoilDeviationX(0.1f),
    recoilDeviationY(0.2f),
    recoilTime(0.3f),
    recoilDeviationProneX(0.02f),
    recoilDeviationProneY(0.05f),
    aimSpeed(0.1f),
    aimSpeedProne(0.1f),
    range(50),
    minRange(0),
    damage(50),
    damageHalfRange(40),
    shootVolume(10),
    clipSize(30), 
    slots( 0 ),
    traits( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPWeaponCharsClass::~ncGPWeaponCharsClass()
{
    // empty
}

//------------------------------------------------------------------------------
