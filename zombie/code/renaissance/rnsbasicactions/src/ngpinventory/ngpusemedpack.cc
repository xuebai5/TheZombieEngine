//------------------------------------------------------------------------------
//  ngpusemedpack.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpinventory/ngpusemedpack.h"

#include "nclogicanimator/nclogicanimator.h"

#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayplayer/ncgameplayplayer.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "zombieentity/ncdictionaryclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPUseMedpack, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPUseMedpack)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*,const char *), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPUseMedpack::nGPUseMedpack():
    gameplayLiving( 0 ),
    animator( 0 ),
    finalHealth( 0 ),
    animIndex( -1 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPUseMedpack::~nGPUseMedpack()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param entity entity that change the weapon
    @param name name of the medpack to use
    @returns true if basic action is valid
*/
bool
nGPUseMedpack::Init( nEntityObject* entity, const char * name )
{
    bool valid = true;

    this->entity = entity;

    // get gameplay
    this->gameplayLiving = this->entity->GetComponent<ncGameplayLiving>();
    this->animator = entity->GetComponent<ncLogicAnimator>();
    valid = (this->gameplayLiving != 0) && (this->animator != 0);
    n_assert( valid );

    // get inventory container
    nInventoryContainer * inventory = 0;
    if( valid )
    {
        inventory = this->gameplayLiving->GetInventory();
        valid = inventory != 0;
        n_assert2( valid, "Entity without inventory" );
    }

    // get actual health of living entity
    int actualHealth = 0;
    if( valid )
    {
        actualHealth = this->gameplayLiving->GetHealth();
    }

    // get health from medpack
    int medPackHealth = 0;
    if( valid )
    {
        valid = inventory->GetNumEquipment( name ) != 0;
        if( valid )
        {
            nEntityObject * entity = inventory->GetEntityWith( name );
            valid = entity != 0;
            n_assert( valid );

            if( valid )
            {
                ncDictionaryClass * dictionary = entity->GetClassComponentSafe<ncDictionaryClass>();
                if( dictionary )
                {
                    medPackHealth = dictionary->GetIntVariable( "ItemMedpackHealth" );
                }
            }

            if( valid )
            {
                inventory->RemoveEquipment( name, 1 );
            }
        }
    }

    if( valid )
    {
        this->finalHealth = actualHealth + medPackHealth;
        this->animIndex = this->animator->SetInjection();

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
nGPUseMedpack::Run()
{
    bool done = this->IsDone();

    if( done )
    {
        if( nNetworkManager::Instance()->IsServer() )
        {
            this->gameplayLiving->SetRecoverHealth( this->finalHealth );
        }
		this->gameplayLiving->SetDizzy( true );
    }   
    
    return done;
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPUseMedpack::IsDone()const
{
    n_assert( this->init );

    return !this->animator || this->animator->HasFinished( this->animIndex );
}

//------------------------------------------------------------------------------
