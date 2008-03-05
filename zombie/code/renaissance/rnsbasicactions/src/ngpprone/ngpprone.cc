#include "precompiled/pchrnsbasicactions.h"
/**
@file nGPProne.cc

(C) 2005 Conjurer Services, S.A.
*/
#include "ngpprone/ngpprone.h"

#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayplayer/ncgameplayplayer.h"
#include "ncgameplayplayer/ncgameplayplayerclass.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncsoundlayer/ncsoundlayer.h"
#include "napplication/napplication.h"
#include "nphysics/ncphycharacter.h"

nNebulaScriptClass(nGPProne, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPProne)
	NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*, bool, bool), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
Constructor
*/
nGPProne::nGPProne():
animator (0),
soundLayer(0),
animIncr(0),
player(0),
finalHeight(0),
gameplay(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPProne::~nGPProne()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
	@brief Init
    @param activate True when proning
    @param combined Indicates crouch and prone actions combined as if they were one
*/
bool
nGPProne::Init (nEntityObject* entity, bool activate, bool combined)
{
    bool valid = entity != 0;

    if ( valid )
    {
        this->gameplay = entity->GetComponent <ncGameplayLiving>();
        this->soundLayer = entity->GetComponent <ncSoundLayer>();
        this->animator = entity->GetComponentSafe<ncLogicAnimator>();
        valid = ( this->gameplay != 0 ) && ( this->soundLayer != 0 ) && ( this->animator != 0 );
        n_assert(valid);
    }

    if ( valid )
    { 
        // determine material under the entity
        const char * materialName = soundLayer->GetMaterialNameUnder();

        if ( activate )
        {
            if ( ! combined )
            {
                this->soundLayer->PlayAloneMaterial("crouchprone", materialName, 1);
            }
        }
        else
        {
            if ( combined )
            {
                this->soundLayer->PlayAloneMaterial("pronestandup", materialName, 1);
            }
        }

        this->gameplay->SetCrouching( false );
        this->gameplay->SetProning( activate );            
        
        this->animIndex = this->animator->SetProne( activate );        
        if ( entity->IsA("neplayer") )
        {                       
            this->initTime = static_cast<float> ( nApplication::Instance()->GetTime() );
            this->totalTime = this->animator->GetStateDuration(this->animIndex, false);
            NLOGCOND(resource, this->totalTime <= 0, ( 0, "ngpProne action has totalTime <= 0", this->totalTime) );

            // Set camera to new height
            ncGameplayPlayerClass* playerClass = entity->GetClassComponentSafe<ncGameplayPlayerClass>();            
            this->player = entity->GetComponentSafe<ncGameplayPlayer>();            

            if (playerClass != 0 && this->player != 0)
            {       
                if ( activate )
                {
                    this->finalHeight = playerClass->GetProneHeight();      
                }
                else
                {
                    this->finalHeight = playerClass->GetCrouchHeight();
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
nGPProne::Run()
{
    bool done = false;

    if ( this->entity->IsA("neplayer") )
    {
        float runTime = static_cast< float > ( nApplication::Instance()->GetTime() ) - this->initTime;
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
nGPProne::End()
{
    float phyHeight(0.0f);
    ncPhyCharacter* collisionPhysics = this->player->GetPlayersPhysics();

    if ( collisionPhysics )
    {
        ncGameplayLiving* ncgameplay = this->entity->GetComponent< ncGameplayLiving >();
        ncGameplayPlayerClass* playerClass = this->entity->GetClassComponent< ncGameplayPlayerClass >();

        if ( ncgameplay != 0 && playerClass != 0 )
        {
            if ( ncgameplay->IsProne() )
            {
                phyHeight = playerClass->GetProneHeight();
            }
            else
            {
                phyHeight = playerClass->GetCrouchHeight();
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
