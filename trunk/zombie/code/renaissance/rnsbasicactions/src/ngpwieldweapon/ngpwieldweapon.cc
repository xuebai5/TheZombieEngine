//------------------------------------------------------------------------------
//  ngpwieldweapon.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpwieldweapon/ngpwieldweapon.h"

#include "ncgameplayliving/ncgameplayliving.h"

#include "rnsgameplay/ninventorycontainer.h"

#include "rnsgameplay/ninventoryitem.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPWieldWeapon, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPWieldWeapon)
    NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*,int,int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPWieldWeapon::nGPWieldWeapon()
{
    // emtpy
}

//------------------------------------------------------------------------------
/**
*/
nGPWieldWeapon::~nGPWieldWeapon()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @returns true if basic action is valid
*/
bool
nGPWieldWeapon::Init (nEntityObject* entity, int slot, int weaponSlot )
{
    bool valid = true;

    this->entity = entity;

    // get gameplay
    ncGameplayLiving * gameplayLiving = 0;
    gameplayLiving = this->entity->GetComponent<ncGameplayLiving>();
    valid = gameplayLiving != 0;

    // get inventory container
    nInventoryContainer * inventory = 0;
    if( valid )
    {
        inventory = gameplayLiving->GetInventory();
        valid = inventory != 0;
        n_assert2( valid, "Entity without inventory" );
    }

    // get the weapon
    nEntityObject * weapon = 0;
    if( valid )
    {
        nInventoryItem * item = 0;

        // if we get the weapon from the weapon bar
        if( weaponSlot != int( nInventoryContainer::WST_ANY ) )
        {
            nInventoryContainer::WeaponSlotType finalSlot;
            finalSlot = static_cast<nInventoryContainer::WeaponSlotType>( weaponSlot );

            item = inventory->GetWeaponItemAt( finalSlot );
            if( item && item->IsWeapon() )
            {
                weapon = item->GetEntity();
            }
        }
        // else get the weapon from normal items
        else if( slot != -1 )
        {
            item = inventory->GetItemAt( slot );
            if( item )
            {
                weapon = item->GetEntity();
            }
        }

        valid = weapon != 0;
        n_assert2( valid, "We can not get the weapon" );
    }

    // wield the weapon
    if( valid )
    {
        gameplayLiving->SetCurrentWeapon( weapon );
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPWieldWeapon::Run()
{
    return this->IsDone();
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPWieldWeapon::IsDone()const
{
    return true;
}

//------------------------------------------------------------------------------
