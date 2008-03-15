#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpswitchflashlight.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngpswitchflashlight/ngpswitchflashlight.h"

#include "ncaimovengine/ncaimovengine.h"
#include "rnsgameplay/ngameplayutils.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncgpperception/ncgpsight.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "rnsgameplay/ncgpweapon.h"

#include "mathlib/cone.h"
#include "zombieentity/ncdictionary.h"

#include "nspatial/ncspatiallight.h"
#include "rnsgameplay/ncgpweaponcharsclass.h"

nNebulaScriptClass(nGPSwitchFlashlight, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPSwitchFlashlight)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPSwitchFlashlight::nGPSwitchFlashlight()
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
bool
nGPSwitchFlashlight::Init (nEntityObject * entity)
{
    bool valid = entity != 0;   // Entity must exists
    ncGPWeapon * weapon = 0;

    if ( valid )
    {
        nEntityObject* weaponObject = entity->GetComponentSafe<ncGameplayLiving>()->GetCurrentWeapon();
        if ( weaponObject )
        {
            weapon = weaponObject->GetComponentSafe<ncGPWeapon>();
        }

        valid = ( weaponObject != 0 ) && ( weapon != 0 );
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPSwitchFlashlight::IsDone() const
{
    n_assert(this->init);

    return true;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
