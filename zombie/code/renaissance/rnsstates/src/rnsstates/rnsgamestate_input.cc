//------------------------------------------------------------------------------
//  rnsgamestate_input.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsstates.h"

#include "rnsstates/rnsgamestate.h"

#include "napplication/napplication.h"
#include "napplication/nappviewport.h"

#include "input/ninputserver.h"

#include "misc/nconserver.h"

#include "ncgameplayplayer/ncgameplayplayer.h"

#include "ngpactionmanager/ngpactionmanager.h"

#include "rnsgameplay/nrnsentitymanager.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "rnsgameplay/ncgpweaponcharsclass.h"

#include "util/nstream.h"

#include "ncgamecamera/ncgamecamera.h"

#include "ncgameplayvehicle/ncgameplayvehicle.h"

#include "nphysics/ncphyvehicle.h"

//------------------------------------------------------------------------------
static struct
{
    const char * button;
    const char * accessory;
} AccessoryChanges [] = {
    { "AddonF5", "Rns_wac_slb" },
    { "AddonF6", "Rns_wac_flb" },
    { "AddonF7", "Rns_wac_rdb" },
    { "AddonF8", "Rns_wac_bip" },
    { "AddonF9", "Rns_wac_agr" },
    { "AddonF10", "Rns_wac_glr" },
    { "AddonF11", "Rns_wac_brs" },
    { "AddonF12", "Rns_wac_scg" },
    { 0, 0 }
};

//------------------------------------------------------------------------------
/**
    @param frameTime time from last call
*/
void
RnsGameState::HandleInput( const float frameTime )
{

    nInputServer* inputServer = nInputServer::Instance();

    // begin: temporary code until we've got the save state
    nEntityObject* localPlayer(this->entityManager->GetLocalPlayer());

    bool playerDriving( false );

    if( localPlayer )
    {
        playerDriving = localPlayer->GetComponent<ncGameplayPlayer>()->GetState() == ncGameplayPlayer::IS_DRIVER;
    }
    // end: temporary code until we've got the save state

    if( inputServer->GetButton("exit") )
    {
        if( !playerDriving )
        {
            if( this->GetClass()->FindCmdByName( "ToggleMenu" ) )
            {
                this->showMenu = ! this->showMenu;
                if( this->showMenu )
                {
                    // show the mouse cursor
                    nGfxServer2::Instance()->SetCursorVisibility( nGfxServer2::System );
                }
                else
                {
                    // hide the mouse cursor
                    nGfxServer2::Instance()->SetCursorVisibility( nGfxServer2::Custom );
                }

                this->Call( "ToggleMenu", 0, 0, this->showMenu );
            }
        }
    }

    if( this->showMenu )
    {
        return;
    }

    if( inputServer->GetButton("show_physics") )
    {
        this->showPhysics = !this->showPhysics;
    }

    if( inputServer->GetButton( "change_camera" ) )
    {
        if( !playerDriving )
        {
            nEntityObject * cameraLookat = this->gameCamera->GetAnchorPoint();
            ncGameplayPlayer * gameplay = 0;
            int index = -1;

            if( cameraLookat )
            {
                // search actual player
                for( int i = 0 ; i < MAX_PLAYERS ; ++i )
                {
                    if( this->players[ i ] )
                    {
                        if( this->players[ i ]->clientEntity == cameraLookat )
                        {
                            index = i;
                            break;
                        }
                    }
                }

                gameplay = cameraLookat->GetComponent<ncGameplayPlayer>();
                if( gameplay )
                {
                    gameplay->SetFirstCameraMode( false );
                }
            }

            nEntityObject * newCameraLookat = 0;

            // get next player
            for( int i = index + 1 ; i < MAX_PLAYERS ; ++i )
            {
                if( this->players[ i ] && this->players[ i ]->clientEntity )
                {
                    newCameraLookat = this->players[ i ]->clientEntity;
                    break;
                }
            }

            if( ! newCameraLookat && ! cameraLookat )
            {
                cameraLookat = this->entityManager->GetLocalPlayer();
            }
            else
            {
                cameraLookat = newCameraLookat;
            }

            this->ChangeCameraAnchor( cameraLookat );

            if( cameraLookat )
            {
                gameplay = cameraLookat->GetComponent<ncGameplayPlayer>();
                if( gameplay )
                {
                    gameplay->SetFirstCameraMode( true );
                }
            }
        }
    }

    // when the camera is free 
    if( this->gameCamera->GetCameraType() == ncGameCamera::free )
    {
        this->FreeCameraInput( frameTime );
    }
    else
    {
        nEntityObject * localPlayer = this->entityManager->GetLocalPlayer();

        if( this->gameCamera->GetAnchorPoint() == localPlayer )
        {
            this->PlayerInput( frameTime );
        }
    }

#ifndef NGAME
    if( inputServer->GetButton("artistview") )
    {
        // switching on/off artist view mode
        this->SwitchArtistMode();
    }

    if( inputServer->GetButton("wheel_velocity_minus") )
    {
        vector3 distance = this->gameCamera->GetCameraOffset();
        vector3 direction = distance;
        direction.norm();
        distance -= direction * 0.5;
        if( distance.lensquared() < N_TINY )
        {
            distance = direction;
        }

        this->gameCamera->SetCameraOffset( distance );
    }

    if( inputServer->GetButton("wheel_velocity_plus") )
    {
        vector3 distance = this->gameCamera->GetCameraOffset();
        vector3 direction = distance;
        direction.norm();
        distance += direction * 0.5;
        this->gameCamera->SetCameraOffset( distance );
    }
#endif
}

//------------------------------------------------------------------------------
/**
    @param frameTime time from last call
*/
void
RnsGameState::PlayerInput( const float frameTime )
{
    // get and check player entity
    nEntityObject * player = this->entityManager->GetLocalPlayer();

    ncGameplayPlayer * gameplay = 0;
    if( player )
    {
        gameplay = player->GetComponent<ncGameplayPlayer>();
    }

    bool valid = ( gameplay != 0 ) && ( this->network != 0 );
    if( ! valid )
    {
        return;
    }

    switch( gameplay->GetState() )
    {
        case ncGameplayPlayer::IS_DRIVER:
            this->PlayerVehicleInput( frameTime );
            break;

        case ncGameplayPlayer::IS_PASSENGER:
            this->PlayerVehicleInput( frameTime );
            this->PlayerMoveInput( gameplay );
            this->PlayerWeaponFireInput( frameTime, gameplay );
            break;

        case ncGameplayPlayer::IS_FROZEN:
            this->PlayerMoveInput( gameplay );
            break;

        case ncGameplayPlayer::IS_NOTHING:
            this->PlayerMoveInput( gameplay );
            this->PlayerWeaponFireInput( frameTime, gameplay );
            this->PlayerNormalInput( gameplay );
            break;

        default:
            n_assert_always();
    }
}

//------------------------------------------------------------------------------
/**
    @param frameTime time from last call
*/
void
RnsGameState::FreeCameraInput( const float frameTime )
{
    nInputServer* inputServer = nInputServer::Instance();

    // update camera
    float lookHori = 0.0f;
    float lookVert = 0.0f;

    lookHori = inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right");
    lookVert = inputServer->GetSlider("slider_down") - inputServer->GetSlider("slider_up");

    const float lookVelocity = 0.25f;
    this->viewerAngles.theta -= lookVert * lookVelocity;
    this->viewerAngles.rho   += lookHori * lookVelocity;

    matrix44 viewMatrix;
    viewMatrix.ident();
    viewMatrix.rotate_x( this->viewerAngles.theta );
    viewMatrix.rotate_y( this->viewerAngles.rho );

    vector3 dirMove = viewMatrix * vector3( 0, 0, 1 );
    vector3 dirStep = viewMatrix * vector3( 1, 0 ,0 );

    if( inputServer->GetButton("Forward") )
    {
        this->viewerPos -= dirMove*frameTime*this->cameraVelocity;
    }

    if( inputServer->GetButton("Backward") )
    {
        this->viewerPos += dirMove*frameTime*this->cameraVelocity;
    }

    if( inputServer->GetButton("StrafeLeft") )
    {
        this->viewerPos -= dirStep*frameTime*this->cameraVelocity;
    }

    if( inputServer->GetButton("StrafeRight") )
    {
        this->viewerPos += dirStep*frameTime*this->cameraVelocity;
    }

    // camera velocity
    if (inputServer->GetButton("wheel_velocity_minus") || inputServer->GetButton("velocity_minus"))
    {
        this->cameraVelocity = this->cameraVelocity / 1.5f;
    }

    if (inputServer->GetButton("wheel_velocity_plus") || inputServer->GetButton("velocity_plus"))
    {
        this->cameraVelocity = this->cameraVelocity * 1.5f;
    }

    this->gameCamera->GetViewPort()->SetViewerAngles(this->viewerAngles);
    this->gameCamera->GetViewPort()->SetViewerPos(this->viewerPos);
}

//-----------------------------------------------------------------------------
/**
    Players vehicle input.

    @param frameTime time passed between frames
    
    history:
        - 27-Jan-2006   Zombie         created
*/
void RnsGameState::PlayerVehicleInput( const float frameTime )
{
    nEntityObject * localPlayer(this->entityManager->GetLocalPlayer());

    localPlayer->GetComponent<ncGameplayPlayer>()->GetSeat()->GetComponent<ncGameplayVehicleSeat>()->ManagePlayer( frameTime );
}

//-----------------------------------------------------------------------------
/**
    @param player entity to apply the input
*/
void
RnsGameState::PlayerMoveInput( const ncGameplayPlayer * player )
{
    if( 0 == player )
    {
        return;
    }

    nInputServer* inputServer = nInputServer::Instance();

    float lookYaw = inputServer->GetSlider("slider_left") - inputServer->GetSlider("slider_right");
    float lookPitch = inputServer->GetSlider("slider_down") - inputServer->GetSlider("slider_up");
    if ( player->IsFlagActivated( ncGameplayPlayer::MF_UPDATEANGLE ) )
    {
        lookYaw *= this->mouseXfactor;
        lookPitch *= this->mouseYfactor;
    }

    int moveForward = 0;
    int moveStep = 0;
    int moveFlags = 0;

    if ( player->IsFlagActivated( ncGameplayPlayer::MF_UPDATEPOSITION ) )
    {
        if( inputServer->GetButton("Forward") )
        {
            moveForward -= 1;
        }

        if( inputServer->GetButton("Backward") )
        {
            moveForward += 1;
        }

        if( inputServer->GetButton("StrafeLeft") )
        {
            moveStep -= 1;
        }

        if( inputServer->GetButton("StrafeRight") )
        {
            moveStep += 1;
        }
        
        if( inputServer->GetButton( "Jump" ) )
        {
            if ( ! ( player->IsProne() || player->IsCrouching() ) )
            {
                moveFlags |= ncGameplayPlayer::IS_JUMPING;
            }        
        }
        bool sprintBlocked = player->IsFlagActivated( ncGameplayPlayer::MF_BLOCKSPRINT );

        if ( sprintBlocked )
        {
            moveFlags |= ncGameplayPlayer::MF_BLOCKSPRINT;
        }

        if( inputServer->GetButton( "RunWalkTogglePressed" ) && !sprintBlocked )
        {
            moveFlags |= ncGameplayPlayer::IS_SPRINTING;
        }

        if( inputServer->GetButton( "RunWalkToggleReleased" ) )
        {
            moveFlags &= ~ncGameplayPlayer::MF_BLOCKSPRINT;
        }

	    if( moveForward != this->lastMoveForward ||
            moveStep != this->lastMoveStep )
	    {
		    if ( player->IsIronsight() && player->IsProne() )
		    {
			    this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
			    this->inputArgs[1].Delete(); this->inputArgs[1].SetB( false );
			    this->network->SendAction( "ngpironsight", 2, this->inputArgs, true );
		    }
	    }

        if ( inputServer->GetButton ("LeftLeanEnable") )
	    {
            moveFlags |= ncGameplayPlayer::IS_LEFTLEAN;
	    }

        if ( inputServer->GetButton ("RightLeanEnable") )
	    {
            moveFlags |= ncGameplayPlayer::IS_RIGHTLEAN;
	    }
    }

    if( moveForward != this->lastMoveForward ||
        moveStep != this->lastMoveStep ||
        lookYaw != this->lastLookYaw ||
        lookPitch != this->lastLookPitch ||
        moveFlags != this->lastMoveFlags )
    {
        vector3 position;
        player->GetPhysicPosition( position );

        polar2 angles;
        player->GetShotAngles( angles );

        this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
        this->inputArgs[1].Delete(); this->inputArgs[1].SetF( float( this->network->GetClock() ) );
        this->inputArgs[2].Delete(); this->inputArgs[2].SetF( position.x );
        this->inputArgs[3].Delete(); this->inputArgs[3].SetF( position.y );
        this->inputArgs[4].Delete(); this->inputArgs[4].SetF( position.z );
        this->inputArgs[5].Delete(); this->inputArgs[5].SetI( moveForward );
        this->inputArgs[6].Delete(); this->inputArgs[6].SetI( moveStep );
        this->inputArgs[7].Delete(); this->inputArgs[7].SetF( angles.theta );
        this->inputArgs[8].Delete(); this->inputArgs[8].SetF( angles.rho );
        this->inputArgs[9].Delete(); this->inputArgs[9].SetF( lookYaw );
        this->inputArgs[10].Delete(); this->inputArgs[10].SetF( lookPitch );
        this->inputArgs[11].Delete(); this->inputArgs[11].SetI( moveFlags );
        this->network->SendAction( "ngpplayermovement", 12, this->inputArgs, false );

        this->lastMoveForward = moveForward;
        this->lastMoveStep = moveStep;
        this->lastMoveFlags = moveFlags;
        this->lastLookYaw = lookYaw;
        this->lastLookPitch = lookPitch;
    }
}

//-----------------------------------------------------------------------------
/**
    @param player entity to apply the input
*/
void
RnsGameState::PlayerWeaponFireInput( const float frameTime, const ncGameplayPlayer * player )
{
    if( 0 == player )
    {
        return;
    }

    if( player->IsDead() )
    {
        return;
    }

    nInputServer* inputServer = nInputServer::Instance();

    if( inputServer->GetButton( "ShowWeaponStats" ) )
    {
        if( this->GetClass()->FindCmdByName( "ToggleWeaponPanel" ) )
        {
            this->Call( "ToggleWeaponPanel", 0, 0, true );
        }
    }

    // reload information
    bool fastReload = true;
    bool executeReload = false;
    bool fullOfAmmo = false;

    if( player->GetCurrentWeapon() )
    {
        ncGPWeapon * weapon = 0;
        weapon = player->GetCurrentWeapon()->GetComponent<ncGPWeapon>();

        if( weapon )
        {
            bool weaponPressed = inputServer->GetButton("PrimaryAttackPressed");
            bool weaponTrigger = inputServer->GetButton("PrimaryAttack");
            if( weaponTrigger != this->lastWeaponTrigger ||
                weaponPressed != this->lastWeaponPressed )
            {
                this->lastWeaponTrigger = weaponTrigger;
                this->lastWeaponPressed = weaponPressed;

                nstream data;
                data.SetWrite( true );
                data.UpdateBool( this->lastWeaponTrigger );
                data.UpdateBool( this->lastWeaponPressed );
                this->network->CallRPCServer( 
                    nRnsEntityManager::WEAPON_TRIGGER, data.GetBufferSize(), data.GetBuffer() );
            }

            if( inputServer->GetButton("RateOfFire") && ! player->IsBusy() )
            {
                if( player->IsIronsight() )
                {
                    this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
                    this->inputArgs[1].Delete(); this->inputArgs[1].SetB( false );
                    this->network->SendQueueAction( "ngpironsight", 2, this->inputArgs, true );
                }
                this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
                this->network->SendQueueAction( "ngpswitchfiremode", 1, this->inputArgs, true );
            }

            // check weapon for reload flags
            fullOfAmmo = weapon->HasFullAmmo();
            fastReload = fastReload && ! weapon->NeedFullReload();
        }
    }

    if( inputServer->GetButton( "ReloadDown" ) && ( player->GetBusy() < ncGameplayLiving::BL_NORMAL )  )
    {
        this->reloadInputTime = 0;
    }

    if( inputServer->GetButton( "ReloadPressed" ) && ( player->GetBusy() < ncGameplayLiving::BL_NORMAL )  )
    {
        this->reloadInputTime += frameTime;
        if( this->reloadInputTime >= this->fullReloadTime )
        {
            // the reload time is set to double at least the key is unset
            // to not allow accidental simultaneous reloads
            this->reloadInputTime = -this->fullReloadTime;
            fastReload = false;
            executeReload = true;
        }
    }

    if( inputServer->GetButton( "ReloadUp" ) && ( player->GetBusy() < ncGameplayLiving::BL_NORMAL )  )
    {
        this->reloadInputTime = 0;
        // the fast reload is the magazine indicated
        executeReload = true;
    }

    if( executeReload && ! fullOfAmmo )
    {
        bool returnIronsight = false;
        if( player->IsIronsight() )
        {
            this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
            this->inputArgs[1].Delete(); this->inputArgs[1].SetB( false );
            this->network->SendAction( "ngpironsight", 2, this->inputArgs, true );
            returnIronsight = fastReload;
        }
        this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
        this->inputArgs[1].Delete(); this->inputArgs[1].SetB( fastReload );
        
        if( player->IsIronsight() )
        {
            this->network->SendQueueAction( "ngpreloadweapon", 2, this->inputArgs, true );
        }
        else
        {
            this->network->SendAction( "ngpreloadweapon", 2, this->inputArgs, true );
        }

        if( returnIronsight )
        {
            this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
            this->inputArgs[1].Delete(); this->inputArgs[1].SetB( true );
            this->network->SendQueueAction( "ngpironsight", 2, this->inputArgs, true );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    @param player entity to apply the input
*/
void
RnsGameState::PlayerNormalInput( const ncGameplayPlayer * player )
{
    if( 0 == player )
    {
        return;
    }

    nInputServer* inputServer = nInputServer::Instance();

    if( inputServer->GetButton( "Suicide" ) )
    {
        nstream data;

        int id = player->GetEntityObject()->GetId();

        data.SetWrite( true );
        data.UpdateInt( id );

        this->network->CallRPCServer( nRnsEntityManager::SUICIDE_PLAYER, data.GetBufferSize(), data.GetBuffer() );
    }

    if( inputServer->GetButton( "health" ) )
    {
        this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
        this->inputArgs[1].Delete(); this->inputArgs[1].SetS( "Medpack" );

        this->network->SendAction( "ngpusemedpack", 2, this->inputArgs, true );
    }

    if( inputServer->GetButton( "flashlight" ) )
    {
        this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );

        this->network->SendAction( "ngpswitchflashlight", 1, this->inputArgs, true );        
    }

    if( inputServer->GetButton( "melee" ) )
    {
        if ( ! player->HasAction( "ngpmelee" ) )
        {
            this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );

            this->network->SendAction( "ngpmelee", 1, this->inputArgs, true );
        }
    }

    if( inputServer->GetButton( "block" ) )
    {
        if ( ! player->HasAction( "ngpblock" ) )
        {
            this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
            this->inputArgs[1].Delete(); this->inputArgs[1].SetI( 2 );
            this->inputArgs[2].Delete(); this->inputArgs[2].SetI( 4000 );

            this->network->SendAction( "ngpblock", 3, this->inputArgs, true );
        }        
    }

    if( inputServer->GetButton("Aim") )
    {
        if( ! player->HasAction( "ngpironsight" ) && ! player->IsBusy() )
        {
            bool flag = ! player->IsIronsight();
            
            this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
            this->inputArgs[1].Delete(); this->inputArgs[1].SetB( flag );
            this->network->SendAction( "ngpironsight", 2, this->inputArgs, true );
        }
    }

    // check for magazine change
    const char * accessoryName = 0;

    if( inputServer->GetButton("AddonR") )
    {
        accessoryName = "Rns_wac_mca";
    }

    // check for accessory change
    if( ! accessoryName && ! player->IsBusy() )
    {
        for( int i = 0 ; AccessoryChanges[ i ].button ; ++i )
        {
            if( inputServer->GetButton( AccessoryChanges[ i ].button ) )
            {
                accessoryName = AccessoryChanges[ i ].accessory;
                break;
            }
        }
    }

    if( accessoryName )
    {
        if( player->GetEntityObject()->GetClass()->FindCmdByName( "ChangeAccesory" ) )
        {
            if( player->IsIronsight() )
            {
                this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
                this->inputArgs[1].Delete(); this->inputArgs[1].SetB( false );
                this->network->SendQueueAction( "ngpironsight", 2, this->inputArgs, true );
            }
            player->GetEntityObject()->Call( "ChangeAccesory", 0, 0, accessoryName ); 
        }
    }

    // Crouch player
    if( inputServer->GetButton( "crouch_start" ) )
    {
        if ( ! player->IsCrouching() || player->IsProne() )
        {
            this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
            this->inputArgs[1].Delete(); this->inputArgs[1].SetB( true );
            this->inputArgs[2].Delete(); this->inputArgs[2].SetB( false );
            this->network->SendAction( "ngpcrouch", 3, this->inputArgs, true );
        }
    }

    if( inputServer->GetButton( "crouch_end" ) )
    {
        if ( player->IsCrouching() && ! player->IsProne() )
        {
            this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
            this->inputArgs[1].Delete(); this->inputArgs[1].SetB( false );
            this->inputArgs[2].Delete(); this->inputArgs[2].SetB( false );
            this->network->SendAction( "ngpcrouch", 3, this->inputArgs, true );
        }
    }

    // Prone player
    if ( inputServer->GetButton( "prone_start" ) )
    {
        if( ! player->HasAction( "ngpprone" ) )
        {
            if ( ! player->IsProne() )
            {
                if ( ! player->IsCrouching() )
                {
                    // stand to crouch
                    this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
                    this->inputArgs[1].Delete(); this->inputArgs[1].SetB( true );
                    this->inputArgs[2].Delete(); this->inputArgs[2].SetB( true );
                    this->network->SendQueueAction( "ngpcrouch", 3, this->inputArgs, true );
                }                
                // crouch to prone
                this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
                this->inputArgs[1].Delete(); this->inputArgs[1].SetB( true );
                this->inputArgs[2].Delete(); this->inputArgs[2].SetB( ! player->IsCrouching() );
                this->network->SendQueueAction( "ngpprone", 3, this->inputArgs, true );
            }         
        }
    }

    if ( inputServer->GetButton( "prone_end" ) )
    {
        if ( ! player->HasAction( "ngpprone") )
        {
            if ( player->IsProne() )
            {
                // prone to crouch
                this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
                this->inputArgs[1].Delete(); this->inputArgs[1].SetB( false );
                this->inputArgs[2].Delete(); this->inputArgs[2].SetB( true );
                this->network->SendQueueAction( "ngpprone", 3, this->inputArgs, true );

                // crouch to stand
                this->inputArgs[0].Delete(); this->inputArgs[0].SetO( player->GetEntityObject() );
                this->inputArgs[1].Delete(); this->inputArgs[1].SetB( false );
                this->inputArgs[2].Delete(); this->inputArgs[2].SetB( true );
                this->network->SendQueueAction( "ngpcrouch", 3, this->inputArgs, true );
            }            
        }        
    }

    /// Use Key (pickup / talk / enter vehicles / use / etc...)
    if( inputServer->GetButton("Activate") )
    {
        /// activate the nearest object
        this->entityManager->ActivateObject( player->GetEntityObject() );
    }
}

//------------------------------------------------------------------------------
