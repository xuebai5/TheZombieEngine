//------------------------------------------------------------------------------
//  ngpuseobject.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpinventory/ngpuseobject.h"

#include "entity/nentityobjectserver.h"

#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayplayer/ncgameplayplayer.h"

#include "rnsgameplay/ninventoryitem.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPUseObject, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPUseObject)
    NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*,int,int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
static const char * UseObjectFunction = "UseItemObject";

//------------------------------------------------------------------------------
/**
*/
nGPUseObject::nGPUseObject()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPUseObject::~nGPUseObject()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param entity entity that change the weapon
    @param slot slot in the entity inventory
    @param object independient object to use
    @returns true if basic action is valid
*/
bool
nGPUseObject::Init( nEntityObject* entity, int slot, int objectId )
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

    nEntityObject * object = 0;
    if( objectId != nEntityObjectServer::IDINVALID )
    {
        object = nEntityObjectServer::Instance()->GetEntityObject( objectId );
    }

    if( slot != -1 )
    {
        nInventoryItem * item = 0;
        // get item
        if( valid )
        {
            item = inventory->GetItemAt( slot );

            valid = item != 0;
        }

        // use the item
        if( valid )
        {
            if( ! item->IsUsed() && item->IsEquipment() )
            {
                bool isExecuted = false;
                nEntityObject * entityItem = item->GetEntity();
                if( entityItem )
                {
                    if( entityItem->GetClass()->FindCmdByName( UseObjectFunction ) )
                    {
                        entityItem->Call( UseObjectFunction, 0, 0, this->entity ); 
                        isExecuted = true;
                    }
                }

                if( ! isExecuted )
                {
                    if( item->GetClass()->FindCmdByName( UseObjectFunction ) )
                    {
                        item->Call( UseObjectFunction, 0, 0, this->entity );
                    }
                }
            }
        }
    }
    else if( object )
    {
        if( object->GetClass()->FindCmdByName( UseObjectFunction ) )
        {
            object->Call( UseObjectFunction, 0, 0, this->entity ); 
        }
    }
    else
    {
        valid = false;
    }

    if( valid )
    {
        ncGameplayPlayer * player = this->entity->GetComponent<ncGameplayPlayer>();
        if( player && player->IsSprinting() )
        {
            player->MovementFlag( ncGameplayPlayer::MF_BLOCKSPRINT, true );
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
nGPUseObject::Run()
{
    return this->IsDone();
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPUseObject::IsDone()const
{
    return true;
}

//------------------------------------------------------------------------------
