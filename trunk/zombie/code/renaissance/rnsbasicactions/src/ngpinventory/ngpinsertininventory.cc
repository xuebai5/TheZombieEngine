//------------------------------------------------------------------------------
//  ngpinsertininventory.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpinventory/ngpinsertininventory.h"

#include "ncgameplayliving/ncgameplayliving.h"

#include "rnsgameplay/ninventorycontainer.h"
#include "rnsgameplay/ninventoryitem.h"
#include "rnsgameplay/ninventorymanager.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPInsertInInventory, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPInsertInInventory)
    NSCRIPT_ADDCMD('INIT', bool, Init, 4, (nEntityObject*,const char *,int,int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPInsertInInventory::nGPInsertInInventory()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPInsertInInventory::~nGPInsertInInventory()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    @param entity entity that change the weapon
    @param itemName name of the item to insert
    @param slot where to insert the item
    @returns true if basic action is valid
*/
bool
nGPInsertInInventory::Init (nEntityObject* entity, const char * itemName, int slot, int amount )
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

    // create the item
    nInventoryItem * item = 0;
    nInventoryManager * manager = nInventoryManager::Instance();
    if( valid )
    {
        item = manager->NewItem( itemName );

        valid = item != 0;
        n_assert2( valid, "Can not create item" );
    }

    // put amount in item
    if( valid )
    {
        if( item->IsStackable() && amount > 0 )
        {
            item->SetStack( amount );
        }
    }

    // insert item into inventory
    if( valid )
    {
        valid = inventory->InsertItem( item, static_cast<nInventoryContainer::WeaponSlotType>(slot) );
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
nGPInsertInInventory::Run()
{
    return this->IsDone();
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPInsertInInventory::IsDone()const
{
    return true;
}

//------------------------------------------------------------------------------
