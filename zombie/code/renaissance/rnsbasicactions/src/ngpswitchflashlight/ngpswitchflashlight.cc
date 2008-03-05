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
/** ZOMBIE REMOVE
#include "ncgameplaystrider/ncgameplaystrider.h"
*/
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

/** ZOMBIE REMOVE
    if ( valid )
    {
        nArray<nEntityObject*> addons = weapon->GetAddonsByTrait( ncGPWeaponCharsClass::MOD_ENABLE_LIGHT );
        for ( int i=0; i < addons.Size(); i++)
        {            
            // switch all light in weapon (any addon with lights)
            nEntityObject* light = 0;
            ncGPWeaponAddon* addon = addons[ i ]->GetComponentSafe<ncGPWeaponAddon>();
            if ( addon )
            {
                light = addon->GetFlashlight();
            }
            n_assert2(light, "Addon don't have nelight entity");
            if ( light )
            {
                bool active = !addon->GetEnabled();
                ncDictionary* dict = addons[ i ]->GetComponentSafe<ncDictionary>();
                if ( active && dict )
                {
                    dict->SetIntVariable( "lightstate", 0 );
                    light->GetComponentSafe<ncSpatialLight>()->TurnOn();
                }
                else
                {
                    dict->SetIntVariable( "lightstate", 1 );
                    light->GetComponentSafe<ncSpatialLight>()->TurnOff();
                }
                addon->SetEnabled( active );
            }

            valid = ( light != 0 );            
        }
    }    
*/
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
