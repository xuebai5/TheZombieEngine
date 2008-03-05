#include "precompiled/pchrnsbasicactions.h"
/**
   @file ngpironsight.cc
   @author Luis Jose Cabellos Gomez

   (C) 2005 Conjurer Services, S.A.
*/
#include "ngpironsight/ngpironsight.h"

#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayplayer/ncgameplayplayer.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncsoundlayer/ncsoundlayer.h"

nNebulaScriptClass(nGPIronsight, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPIronsight)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, bool), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPIronsight::nGPIronsight():
    animator (0),
	soundLayer(0),
    gameplay(0),
    setIronsight( false )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPIronsight::~nGPIronsight()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param entity entity that change ironsight state
    @returns true if action is invalid
*/
bool
nGPIronsight::Init (nEntityObject* entity, bool activate )
{
    bool valid = entity != 0;
    
    if ( valid )
    {
        this->gameplay = entity->GetComponent <ncGameplayLiving>();
        this->animator = entity->GetComponent <ncLogicAnimator>();
		this->soundLayer = entity->GetComponent <ncSoundLayer>();
        valid = ( this->gameplay != 0 ) && ( this->animator != 0 ) && ( this->soundLayer != 0 );
        n_assert( valid );
    }

    if ( valid )
    {
        // activate == true  , from normal to ironsight
        // activate == false , from ironsight to normal
        this->animIndex = this->animator->SetIronsight (activate);
        if ( activate )
        {
		    this->soundLayer->PlayAlone("ironsight", 1);
        }
        else
        {
            this->soundLayer->PlayAlone("ironsightoff", 1);
        }

        if( ! activate )
        {
            this->gameplay->SetIronsight( activate );
        }

        this->setIronsight = activate;
        this->entity = entity;        
    }

    this->init = valid;

    if( valid )
    {
        this->gameplay->SetBusy( ncGameplayLiving::BL_NORMAL );

        ncGameplayPlayer * player = this->entity->GetComponent<ncGameplayPlayer>();
        if( player )
        {
            // unhide weapon in the begin
            if( ! activate )
            {
                player->RefreshScopeState();
            }

            if( player->IsSprinting() )
            {
                player->MovementFlag( ncGameplayPlayer::MF_BLOCKSPRINT, true );
            }
        }
    }

    return valid;
}

//------------------------------------------------------------------------------
/**
    @returns true if action is done
*/
bool
nGPIronsight::Run()
{
    bool done = this->IsDone();

    if( done )
    {
        if( this->setIronsight )
        {
            this->gameplay->SetIronsight( this->setIronsight );
        }

        ncGameplayPlayer * player = this->entity->GetComponent<ncGameplayPlayer>();
        if( player )
        {
            player->RefreshScopeState();
        }

        this->gameplay->SetBusy( ncGameplayLiving::BL_NONE );
    }

    return done;
}

//------------------------------------------------------------------------------
/**
    @returns true if action is done
*/
bool
nGPIronsight::IsDone()const
{
    n_assert(this->init);

    return this->animator->HasFinished( this->animIndex );
}

//------------------------------------------------------------------------------
