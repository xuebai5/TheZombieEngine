//------------------------------------------------------------------------------
//  ngppickupitem.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpinventory/ngppickupitem.h"

#include "ncgameplayliving/ncgameplayliving.h"

#include "rnsgameplay/ninventoryitem.h"
#include "rnsgameplay/ninventorymanager.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPPickupItem, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPPickupItem)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*,int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPPickupItem::nGPPickupItem()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPPickupItem::~nGPPickupItem()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    @param entity entity that pickup an item
    @param itemID item to pickup
    @returns true if basic action is valid
*/
bool
nGPPickupItem::Init( nEntityObject* entity, int itemID )
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

    // get inventory manager
    nInventoryManager * manager = nInventoryManager::Instance();

    valid = manager != 0;

    // get item
    nInventoryItem * item = 0;
    if( valid )
    {
        item = manager->GetItem( itemID );
        n_assert2( item, "Item not found" );
        if( item )
        {
            valid = item->IsDropped();
        }
    }

    // erase dropped item
    if( valid )
    {
        manager->EraseDroppedItem( item );
    }

    // insert item into inventory
    if( valid )
    {
        valid = inventory->InsertItem( item, nInventoryContainer::WST_ANY );
        n_assert2( valid, "Can insert item in the inventory" );

        if( ! valid )
        {
            manager->DeleteItem( item );
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
nGPPickupItem::Run()
{
    return this->IsDone();
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPPickupItem::IsDone()const
{
    return true;
}

//------------------------------------------------------------------------------
