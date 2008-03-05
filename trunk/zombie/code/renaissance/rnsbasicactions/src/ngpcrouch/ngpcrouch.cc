#include "precompiled/pchrnsbasicactions.h"
/**
	@file nGPCrouch.cc
	
(C) 2005 Conjurer Services, S.A.
*/
#include "ngpcrouch/ngpcrouch.h"

#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayplayer/ncgameplayplayer.h"
#include "ncgameplayplayer/ncgameplayplayerclass.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncsoundlayer/ncsoundlayer.h"
#include "napplication/napplication.h"
#include "nphysics/ncphycharacter.h"
#include "ncaimovengine/ncaimovengine.h"


nNebulaScriptClass(nGPCrouch, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
	Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPCrouch)
	NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*, bool, bool), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
	Constructor
*/
nGPCrouch::nGPCrouch():
	animator (0),
	soundLayer(0),
    animIncr(0),
    player(0),
    finalHeight(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
	Destructor
*/
nGPCrouch::~nGPCrouch()
{
	this->End();
}

//------------------------------------------------------------------------------
/**
	@brief Init
    @param activate True when crouching from stand up or from prone, false when going crouch position in any case.
    @param combined Indicates crouch and prone actions combined as if they were one
*/
bool
nGPCrouch::Init (nEntityObject* entity, bool activate, bool combined)
{
	bool valid = entity != 0;
	ncGameplayLiving* gameplay = 0;

	if ( valid )
	{
		gameplay = entity->GetComponent <ncGameplayLiving>();
		this->soundLayer = entity->GetComponent <ncSoundLayer>();
        this->animator = entity->GetComponentSafe<ncLogicAnimator>();
		valid = ( gameplay != 0 ) && ( this->soundLayer != 0 ) && ( this->animator != 0 );
		n_assert(valid);
	}

	if ( valid )
	{ 
        // determine material under the entity
        const char * materialName = soundLayer->GetMaterialNameUnder();

		if ( activate )
        {
            if ( combined )
            {
                this->soundLayer->PlayAloneMaterial("prone", materialName, 1);
            }
            else
            {
                if ( ! gameplay->IsProne() )
                {
                    this->soundLayer->PlayAloneMaterial("crouch", materialName, 1);
                }
                else
                {
                    this->soundLayer->PlayAloneMaterial("pronecrouch", materialName, 1);
                }
            }
        }
        else
        {
            if ( ! combined )
            {
                this->soundLayer->PlayAloneMaterial("crouchstandup", materialName, 1);
            }
        }

        gameplay->SetProning( false );
        gameplay->SetCrouching( activate );               
        this->animIndex = this->animator->SetCrouch( activate ); 

        if ( entity->IsA("neplayer") )
        {
            this->initTime = static_cast<float> ( nApplication::Instance()->GetTime() );
            // @TODO: Get total time from player class
            // Magic Number: 0.5f is the time (in seconds) that player use for crouch
            this->totalTime = 0.5f;

            // Set camera to new height
            ncGameplayPlayerClass* playerClass = entity->GetClassComponentSafe<ncGameplayPlayerClass>();
            this->player = entity->GetComponentSafe<ncGameplayPlayer>();
            if (playerClass != 0 && this->player != 0)
            {
                //this->player->MovementFlag ( ncGameplayPlayer::MF_UPDATEPOSITION, false );

                
                if ( activate )
                {
                    this->finalHeight = playerClass->GetCrouchHeight();                                    
                }
                else
                {
                    this->finalHeight = playerClass->GetStandHeight();                    
                }
                this->height = this->player->GetCameraHeight();
           }

            valid = ( this->player != 0 ) && ( playerClass != 0 );
        }        

        this->entity = entity;
	}

    if( valid )
    {
        if( this->player->IsSprinting() )
        {
            this->player->MovementFlag( ncGameplayPlayer::MF_BLOCKSPRINT, true );
        }
    }

	this->init = valid;

	return valid;
}

//------------------------------------------------------------------------------
/**
	Run
*/
bool
nGPCrouch::Run()
{
    bool done = false;

    if ( this->entity->IsA("neplayer") )
    {
        float runTime = static_cast< float > ( nApplication::Instance()->GetTime() ) - this->initTime;
        n_assert2(this->totalTime > 0, "Division by cero...Put a total time for crouch action");
        if ( runTime < this->totalTime && this->totalTime > 0 )
        {
            float currentHeight = ( this->finalHeight - this->height ) * (runTime / this->totalTime ) + this->height;
            this->player->SetCameraHeight( currentHeight );
        }
        else
        {
            done = true;
        }        
    }

	return done;
}

//------------------------------------------------------------------------------
/**
    End
*/
void
nGPCrouch::End()
{
    float phyHeight(0.0f);
    ncPhyCharacter* collisionPhysics = this->player->GetPlayersPhysics();

    if ( collisionPhysics )
    {
        ncGameplayLiving* ncgameplay = this->entity->GetComponent<ncGameplayLiving>();
        ncGameplayPlayerClass* playerClass = this->entity->GetClassComponent<ncGameplayPlayerClass>();

        if ( ncgameplay != 0 && playerClass != 0 )
        {
            if ( ncgameplay->IsCrouching() )
            {
                phyHeight = playerClass->GetCrouchHeight();
            }
            else
            {
                phyHeight = playerClass->GetPlayerHeight();
            }

            collisionPhysics->ChangeHeight( phyHeight );        
        }
        else
        {
            n_assert2(ncgameplay, "Cannot get ncgameplayliving");
            n_assert2(playerClass, "Cannot get ncgameplayplayerclass");
        }
    }                        
    else
    {
        n_assert2( collisionPhysics, "Cannot found character physics" );
    }
}

//------------------------------------------------------------------------------
