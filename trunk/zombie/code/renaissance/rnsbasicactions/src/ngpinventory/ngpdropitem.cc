//------------------------------------------------------------------------------
//  ngpdropitem.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpinventory/ngpdropitem.h"

#include "ncgameplayliving/ncgameplayliving.h"

#include "rnsgameplay/ninventoryitem.h"
#include "rnsgameplay/ninventorymanager.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPDropItem, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPDropItem)
    NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*,bool,int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPDropItem::nGPDropItem()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPDropItem::~nGPDropItem()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    @param entity entity that change the weapon
    @returns true if basic action is valid
*/
bool
nGPDropItem::Init( nEntityObject* entity, bool weaponBar, int slot )
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

    nInventoryItem * item = 0;
    // check that the item isn't in use
    if( valid )
    {
        if( weaponBar )
        {
            item = inventory->GetWeaponItemAt( nInventoryContainer::WeaponSlotType( slot ) );
        }
        else
        {
            item = inventory->GetItemAt( slot );
        }

        valid = item != 0;
        if( valid )
        {
            valid = ! item->IsUsed();
        }
    }

    // get item
    if( valid )
    {
        if( weaponBar )
        {
            item = inventory->RemoveWeaponItemAt( nInventoryContainer::WeaponSlotType( slot ) );
        }
        else
        {
            item = inventory->RemoveItemAt( slot );
        }

        valid = item != 0;
    }

    // drop the item
    if( valid )
    {
        nInventoryManager * inventoryManager = nInventoryManager::Instance();
        n_assert( inventoryManager );
        if( inventoryManager )
        {
            nEntityObject * droppedItem = 0;
            droppedItem = inventoryManager->CreateDroppedItem( item );
            n_assert( droppedItem );

            if( droppedItem )
            {
                // situate the item
                ncTransform * entityPosition = this->entity->GetComponentSafe<ncTransform>();
                ncTransform * itemPosition = droppedItem->GetComponentSafe<ncTransform>();
                if( itemPosition && entityPosition )
                {
                    matrix44 moveMatrix;
                    moveMatrix.ident();

                    polar2 entityAngles;
                    gameplayLiving->GetShotAngles( entityAngles );
                    moveMatrix.rotate_y( entityAngles.rho );

                    vector3 dirMove( moveMatrix.z_component() );

                    vector3 position;
                    gameplayLiving->GetShotPosition( position );
                    position = position - dirMove * 2.0f;
                    itemPosition->SetPosition( position );
                }
            }
        }
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPDropItem::Run()
{
    return this->IsDone();
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPDropItem::IsDone()const
{
    return true;
}

//------------------------------------------------------------------------------
