//------------------------------------------------------------------------------
//  ncgameplayplayer_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "entity/nentityobjectserver.h"

#include "ncgameplayplayer/ncgameplayplayer.h"
#include "rnsgameplay/ngameplayutils.h"
#include "rnsgameplay/nrnsentitymanager.h"

#include "misc/nwatched.h"
#include "util/nrandomlogic.h"
#include "util/nstream.h"

#include "nphysics/nphysicsserver.h"
#include "nphysics/nphygeomray.h"
#include "nphysics/nphygeomsphere.h"
#include "nphysics/ncphycharacter.h"
#include "nphysics/ncphycharacterobj.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "animcomp/nccharacter.h"

#include "nscene/ncscenelod.h"

#include "nclogicanimator/nclogicanimator.h"

#include "ncsoundlayer/ncsoundlayer.h"

#include "zombieentity/ncdictionary.h"

#include "ntrigger/ngameevent.h"
#include "ntrigger/nctrigger.h"
#include "ntrigger/ncareaevent.h"

#ifndef NGAME
#include "ndebug/ndebugserver.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGameplayPlayer,ncGameplayLiving);

//------------------------------------------------------------------------------
const float ncGameplayPlayer::NORMAL_ANGLE = 65.0f;
const float ncGameplayPlayer::IRONSIGHT_ANGLE = 40.0f;
const float ncGameplayPlayer::SCOPE_ANGLE = 20.0f;
const float ncGameplayPlayer::IRONSIGHT_ANGLE_SPEED = -20.0f;
const float ncGameplayPlayer::IRONSIGHT_TURN_ADJUST = 0.6f;
const float ncGameplayPlayer::IRONSIGHT_VELOCITY_FACTOR = 0.5f;
const float ncGameplayPlayer::WEAPON_RECOVER_SPEED = -6.0f;
const float ncGameplayPlayer::WEAPON_VERTICAL_SPEED = -10.0f;
const float ncGameplayPlayer::WEAPON_VERTICAL_ADJUST = 0.5f;
const float ncGameplayPlayer::WEAPON_VERTICAL_BUMP = 0.05f;
const float ncGameplayPlayer::WEAPON_HORIZONTAL_MOVE = 0.01f;
const float ncGameplayPlayer::MAX_ANGLEELEVATION =  n_deg2rad( 80.0f );
const float ncGameplayPlayer::MIN_ANGLEELEVATION = -n_deg2rad( 80.0f );
const float ncGameplayPlayer::MAX_ANGLETHIRD =  n_deg2rad( 30.0f );
const float ncGameplayPlayer::MIN_ANGLETHIRD = -n_deg2rad( 45.0f );
const float ncGameplayPlayer::STEP_HEIGHT = 0.15f;
const float ncGameplayPlayer::JUMP_CONTROL = 0.01f;

//------------------------------------------------------------------------------

NSIGNAL_DEFINE(ncGameplayPlayer, ChangePlayerState);

//------------------------------------------------------------------------------
/**
*/
ncGameplayPlayer::ncGameplayPlayer():
    recoilDifference( 0,0 ), 
    angleOrigin( 0,0 ),
    lowerAngles( 0,0 ),
    upperAngles( 0,0 ),
    turnMaximum( N_PI ),
    turnMinimum( -N_PI ),
    elevationMaximum( ncGameplayPlayer::MAX_ANGLEELEVATION ),
    elevationMinimum( ncGameplayPlayer::MIN_ANGLEELEVATION ),
    velocity( 0, 0, 0 ),
    playerClass( 0 ),
    collisionPhysics( 0 ), collisionTransform( 0 ),
    isFalling( false ), isJumping( false ),
    isClimbing( false ), isFirstCameraMode( false ),
    isInScopeMode( false ),
    movementFlags( MF_UPDATEPHYSICS | MF_UPDATEPOSITION | MF_UPDATEANGLE ),
    shotTime( 0 ),
    jumpTime( 0 ),
    cameraHeight( 0 ),
    leanDistance( 0 ),
    sprintVelocityFactor( 0.0f ),
    runVelocityFactor( 0.0f ),
    ironsightDownOffset( 0.0f ),
    ironsightAngle( ncGameplayPlayer::IRONSIGHT_ANGLE ),
    incrementAngles( 0, 0 ),
    hasStamina(true), stamina(5000),
    lastTiredTime(0), isTired(false),
    playerState( IS_NOTHING ),
    actualAnim(GP_ACTION_IDLE),
    jumpAnim( -1 ),
    seat(0),
    newDirection(ncLogicAnimator::LA_STOPPED),
    actualDirection(ncLogicAnimator::LA_STOPPED),
    sprintCancelled( false ),
    isWeaponTrigger( false ),
    isWeaponPressed( false ),
    firedLastFrame( false ),
    interpolateAngle( false )
{
    // put human sound as default noisy event
    this->SetMovementEvent( nGameEvent::HUMAN_SOUND );
}

//------------------------------------------------------------------------------
/**
*/
ncGameplayPlayer::~ncGameplayPlayer()
{
    if( this->collisionRay )
    {
        this->collisionRay->Release();
    }

    if( this->collisionEntity.isvalid() )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->collisionEntity );
    }
}

//------------------------------------------------------------------------------
/**
    @param loaded say if the instance is loaded or created
*/
void
ncGameplayPlayer::InitInstance(nObject::InitInstanceMsg initType)
{
    ncGameplayLiving::InitInstance( initType );

    if (initType != nObject::ReloadedInstance)
    {
        this->collisionEntity = nEntityObjectServer::Instance()->NewLocalEntityObject( "nephycharacter" );
    }

    if( this->collisionEntity )
    {
        this->collisionPhysics = this->collisionEntity->GetComponent<ncPhyCharacter>();
        this->collisionTransform = this->collisionEntity->GetComponent<ncTransform>();

        bool valid = ( this->collisionPhysics != 0 ) && (this->collisionTransform != 0);
        n_assert2( valid, "Can not get components" );
        if( valid )
        {
            ncGameplayPlayerClass * playerClass = this->GetClassComponentSafe<ncGameplayPlayerClass>();
            if( playerClass )
            {
                float wide = playerClass->GetPlayerWide();
                float height = playerClass->GetPlayerHeight();
                height = (height > wide) ? (height - wide) : ( N_TINY );
                this->collisionPhysics->SetWide( wide );
                this->collisionPhysics->SetHeight( height );
            }

            // Check for player material
            if( ! nKernelServer::Instance()->Lookup("/usr/physics/playermaterial") )
            {
                // creating the player material
                nPhyMaterial* mat( static_cast<nPhyMaterial*>( nKernelServer::Instance()->New( "nphymaterial", "/usr/physics/playermaterial" ) ) );

                n_assert2( mat, "Failed to create the player material." );
                if( mat )
                {
                    mat->SetCoeficientPyramidFriction(phyreal(0));
                    mat->SetCoeficientBounce(phyreal(.0001));
                    mat->SetCoeficientSoftnessERP( phyreal(1) );
                    mat->SetCoeficientSoftnessCFM(0);
                    mat->SetCoeficientSlip(phyreal(0));
                    mat->Update();
                    this->collisionPhysics->SetMaterial( mat->Id() );
                }
            }

            this->collisionPhysics->InsertInTheWorld();
        }
        else
        {
            this->collisionEntity->Release();
            this->collisionEntity = 0;
            this->collisionPhysics = 0;
            this->collisionTransform = 0;
        }
    }

    // create variables needed in collision
    if (initType != nObject::ReloadedInstance)
    {
        this->collisionRay = static_cast<nPhyGeomRay*>( nKernelServer::Instance()->New( "nphygeomray" ) );
    }
    n_assert2( this->collisionRay, "Can not create Collision Ray" );
    this->collisionRay->SetCollidesWith( ~(nPhysicsGeom::Fluid|nPhysicsGeom::Check|nPhysicsGeom::Stairs) );
    this->collisionRay->SetCategories( nPhysicsGeom::Check );

    this->fbIronsightAngle.SetState( ncGameplayPlayer::NORMAL_ANGLE );
    this->fbIronsightAngle.SetGain( ncGameplayPlayer::IRONSIGHT_ANGLE_SPEED );

    this->fbTurn.SetGain( ncGameplayPlayer::WEAPON_RECOVER_SPEED );
    this->fbMove.SetGain( ncGameplayPlayer::WEAPON_VERTICAL_SPEED );

    this->fbFalling.SetGoal( 0 );
    this->fbFalling.SetState( 0 );
    this->fbFalling.SetGain( ncGameplayPlayer::WEAPON_VERTICAL_SPEED );

    this->fbLean.SetGoal( vector3( 0, 0, 0 ) );
    this->fbLean.SetState( vector3( 0, 0, 0 ) );
    this->fbLean.SetGain( ncGameplayPlayer::WEAPON_VERTICAL_SPEED );

    this->SetIronsight( false );
    this->CalculatePlayerSpeed();

    this->SetShotPosition( vector3( 0,2.0f,0 ) );
    this->SetShotAngles( polar2( 0, 0 ) );

    // init first camera mode
    this->SetFirstCameraMode( this->isFirstCameraMode );
    ncGameplayPlayerClass * playerclass = this->GetClassComponentSafe<ncGameplayPlayerClass>();
    if( playerclass )
    {
        this->cameraHeight = playerclass->GetStandHeight();
    }

    this->playerClass = this->GetClassComponent<ncGameplayPlayerClass>();
}

//------------------------------------------------------------------------------
/**
    @param angles new orientation angles
*/
void
ncGameplayPlayer::SetShotAngles( const polar2 & angles )
{
    this->upperAngles = angles;
    this->lowerAngles.rho = angles.rho;
    this->lowerAngles.theta = 0.0f;

    this->ApplyAngles();
}

//------------------------------------------------------------------------------
/**
    @param angles player orientation angles
*/
void
ncGameplayPlayer::GetShotAngles( polar2 & angles ) const
{
    angles = this->upperAngles;
}

//------------------------------------------------------------------------------
/**
    @param angles new angles
*/
void
ncGameplayPlayer::UpdateDiffAngles( const polar2 & angles )
{
    this->diffAngles.theta = this->upperAngles.theta - angles.theta;

    this->diffAngles.rho = this->upperAngles.rho - angles.rho;
    while( this->diffAngles.rho > N_PI )
    {
        this->diffAngles.rho -= 2*N_PI;
    }
    while( this->diffAngles.rho < -N_PI )
    {
        this->diffAngles.rho += 2*N_PI;
    }
}

//------------------------------------------------------------------------------
/**
    @param time actual application time
*/
void
ncGameplayPlayer::UpdateCamera( nTime time )
{
    const float MAGNITUDE_MAX = 0.10f;
    const float MAGNITUDE_MIN = 0.05f;
    const float MAGNITUDE_FACTOR = 10.0f;
    const float TURN_FACTOR = 1.0f;
    const float BUMP_EPSILON = 0.005f;
    const vector3 VERTICAL( 0, 1, 0 );

    vector3 direction;
    this->GetShotDirection( direction );
    vector3 dirStep = direction * VERTICAL;
    dirStep.norm();

    float turnFactor = TURN_FACTOR;

    vector3 movement;

    float velocityMagnitude = this->velocity.lensquared();

    bool fullIronsight = this->IsIronsight() && ! ( velocityMagnitude > 0 && this->isSprint );

    if( fullIronsight )
    {
        turnFactor = ncGameplayPlayer::IRONSIGHT_TURN_ADJUST;
    }
    else
    {
        // calculate movement camera adjust
        if( velocityMagnitude > 0 )
        {
            float magnitude = 0.0f;
            // calculate the movement for the terrain bumps
            magnitude = n_clamp( n_abs( this->diffPosition.y ), MAGNITUDE_MIN, MAGNITUDE_MAX ) * MAGNITUDE_FACTOR;
            // apply the player movement down weapon move
            movement.y = ncGameplayPlayer::WEAPON_VERTICAL_BUMP * magnitude;

            // projection over the step direction
            float stepmove = -( dirStep.dot( this->velocity ) / dirStep.len() );

            magnitude = n_clamp( stepmove, -MAGNITUDE_MAX, MAGNITUDE_MAX ) * MAGNITUDE_FACTOR;
            movement += dirStep * ncGameplayPlayer::WEAPON_HORIZONTAL_MOVE * magnitude;
        }
    }

    if( ! this->IsInScope() )
    {
        if( this->velocity.lensquared() > 0 && this->isSprint && this->IsIronsight() )
        {
            this->fbIronsightAngle.SetGoal( ncGameplayPlayer::NORMAL_ANGLE );
        }
        else if( (this->velocity.lensquared() <= TINY || ! this->isSprint) && this->IsIronsight() )
        {
            this->fbIronsightAngle.SetGoal( this->ironsightAngle );
        }
    }

    this->fbIronsightAngle.Update( time );

    this->fbFalling.Update( time );

    float fallingGoal = this->fbFalling.GetGoal();
    if( fallingGoal > 0 )
    {   
        if( n_abs( fallingGoal - this->fbFalling.GetState() ) < BUMP_EPSILON )
        {
            this->fbFalling.SetGoal( 0 );
        }
    }

    this->fbMove.SetGoal( movement );
    this->fbMove.Update( time );

    // calculate turn camera adjust
    vector3 turn;
    if( n_abs( this->diffAngles.rho ) > 0 )
    {
        turn += dirStep * diffAngles.rho * turnFactor;
    }

    if( n_abs( this->diffAngles.theta ) > 0 )
    {
        turnFactor *= ncGameplayPlayer::WEAPON_VERTICAL_ADJUST;
        turn -= VERTICAL * diffAngles.theta * turnFactor;
    }

    float turnLen = turn.len();
    if( turnLen > MAGNITUDE_MAX )
    {
        turn = ( turn / turnLen ) * MAGNITUDE_MAX;
    }
    this->fbTurn.SetGoal( turn );
    this->fbTurn.Update( time );

    // calculate lean camera adjust
    if( n_abs( this->leanDistance ) > N_TINY )
    {
        this->fbLean.SetGoal( 
            dirStep * this->leanDistance + vector3(0,-1,0)*0.1f*n_abs(this->leanDistance) );
    }
    this->fbLean.Update( time );
}

//------------------------------------------------------------------------------
/**
    @param position 
*/
void
ncGameplayPlayer::GetCameraPosition( vector3 & position ) const
{
    position = this->shotPosition;

    position += this->fbTurn.GetState();
    position += this->fbMove.GetState();
    position.y += this->fbFalling.GetState();
    position.y += this->ironsightDownOffset;
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::ApplyRecoilDeviation()
{
    n_assert( this->currentWeapon );

    const float MIN_RECOIL_TIME = 0.1f;

    polar2 angles( this->upperAngles );
    
    ncGPWeapon * weapon = this->currentWeapon->GetComponentSafe<ncGPWeapon>();
    n_assert( weapon );
    this->recoilDifference.x = weapon->GetRecoilDeviationX() * n_rand_real( 1.0f );
    this->recoilDifference.y = weapon->GetRecoilDeviationY() * n_rand_real( 1.0f );

    float recoilTime = weapon->GetRecoilTime();
    if( recoilTime <= 0.0f )
    {
        recoilTime = MIN_RECOIL_TIME;
    }

    this->recoilRecover.x = this->recoilDifference.x / recoilTime;
    this->recoilRecover.y = this->recoilDifference.y / recoilTime;

    angles.rho -= this->recoilDifference.x;
    angles.theta += this->recoilDifference.y;

    this->UpdateDiffAngles( angles );
    this->upperAngles = angles;
    this->ApplyAngles();
}

//------------------------------------------------------------------------------
/**
    @param deltaTime time from last update
*/
void
ncGameplayPlayer::Run (const float deltaTime)
{
    ncGameplayLiving::Run (deltaTime);

    // update player position between physic and graphic
    if( this->movementFlags & MF_UPDATEPHYSICS )
    {
        this->UpdatePlayerPosition();
    }

    // move player angles
    if( this->movementFlags & MF_UPDATEANGLE )
    {
        this->MovePlayerAngle( deltaTime );
        this->ApplyAngles();
    }

    // move player position
    if( this->movementFlags & MF_UPDATEPOSITION )
    {
        this->MovePlayerPosition( deltaTime );
    }

    // update player stamina
    this->UpdateStamina( deltaTime );

    // undate representation of player
    this->UpdateGraphicsSound();

    // update shot timer
    this->shotTime -= deltaTime;
    if( this->shotTime < 0 )
    {
        this->shotTime = 0;
    }

    // shot if necessary
    this->ShotWeapon();
}

//------------------------------------------------------------------------------
/** 
*/
void
ncGameplayPlayer::UpdateStamina( const float deltaTime )
{
    // Update stamina
    ncGameplayPlayerClass * playerclass = this->GetClassComponentSafe<ncGameplayPlayerClass>();

    if ( this->hasStamina )
    {
        this->SetStamina(this->stamina + int( deltaTime*playerclass->GetStaminaRecover() ) );
    }

    // If tired, wait to recover
    if (this->isTired)
    {
        this->lastTiredTime -= deltaTime;
        if (this->lastTiredTime <= 0 )
        {
            this->isTired = false;
        }
    }
    else
    {
        if ( this->isSprint )
        {
            // If the player is in the air after a jump, sprinting doesn't consume stamina
            if ( this->inputVelocity.lensquared() > 0 && ! this->isFalling && ! this->isJumping )
            {
                this->SetHasStamina(false);
                this->SetStamina(this->GetStamina() - int( deltaTime*playerclass->GetSprintStaminaLost() ) );            
               
                if ( ! this->GetStamina() )
                {
                    // player is tired, stop the sprinting
                    this->isTired = true;  
                    this->SetHasStamina( true );
                    this->lastTiredTime = playerclass->GetTiredTime();
                }   
            }
            else
            {
                this->SetHasStamina(true);
            }
        }
        else
        {
            this->SetHasStamina( true );
        }
    }

}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::UpdateGraphicsSound()
{
    ncLogicAnimator * animator = this->GetComponent<ncLogicAnimator>();
    ncSoundLayer * soundLayer = this->GetComponentSafe<ncSoundLayer>();

    if ( soundLayer && animator )
    {
        float duration = animator->GetComponentSafe<ncCharacter>()->GetStateDuration( animator->GetStateIndex(), false );

        // update sound material
        soundLayer->UpdateSoundMaterial( duration );

        bool isInthefloor = ! this->isFalling && ! this->isClimbing;

        int newAnim = this->actualAnim; 
        if( this->jumpAnim == -1 )
        {
            if( isInthefloor )
            {
                // calculate direction of horizontal movement
                vector3 horizontal( this->velocity.x, 0, this->velocity.z );

                int soundEvent = newAnim;
                if( horizontal.lensquared() > 0.01 )    // 0.1 m/s squared is the minimal velocity
                {
                    if( this->newDirection == ncLogicAnimator::LA_STOPPED )
                    {
                        // calculate turn
                        float cosa = horizontal.dot( nGameplayUtils::AngleBase ) / horizontal.len();

                        float angle = acos( cosa );
                        if( horizontal.x > 0.0f )
                        {
                            angle = -angle;
                        }

                        // check if player go forward or backward
                        if( n_abs( this->lowerAngles.rho - angle ) > N_HALFPI )
                        {
                            this->newDirection = ncLogicAnimator::LA_BACKWARD;
                        }
                        else
                        {
                            this->newDirection = ncLogicAnimator::LA_FORWARD;
                        }
                    }

                    if( this->IsSprinting() && !this->isTired && !this->IsProne() && !this->IsCrouching() )
                    {
                        newAnim = GP_ACTION_RUN;
                        this->SwitchOnMovementEvent();
                    }
                    else
                    {
                        newAnim = GP_ACTION_WALK;

                        if ( this->IsProne() )
                        {
                            soundEvent = GP_ACTION_WALKPRONE;
                        }
                        else
                        {
                            soundEvent = GP_ACTION_WALK;
                        }

                        // Set the movement event
                        if( this->IsProne() || this->IsCrouching() ) {
                            this->SwitchOffMovementEvent();
                        }
                        else
                        {
                            this->SwitchOnMovementEvent();
                        }
                    }
                }
                else
                {
                    newAnim = GP_ACTION_IDLE;
                    this->SwitchOffMovementEvent();
               }

                if( this->actualDirection != this->newDirection )
                {
                    animator->SetMovementPose( this->newDirection );
                    this->actualDirection = this->newDirection;
                }
                if( ! ( this->movementFlags & MF_BLOCKLOWERANIM ) && animator->GetAction() != GP_ACTION_DRIVINGIDLE )
                {
                    animator->SetAnimationAction( newAnim, false, true, true );
                }
                this->actualAnim = newAnim;
                
                // apply lean
                if( this->actualDirection == ncLogicAnimator::LA_LEFTLEAN || 
                    this->actualDirection == ncLogicAnimator::LA_RIGHTLEAN )
                {   
                    if( this->IsIronsight() )
                    {
                        animator->SetLeanIronsight( true );
                    }
                    else
                    {
                        animator->SetLean( true );
                    }
                }

                // Launch anim sound
                if ( animator->IsValidStateIndex( animator->GetStateIndex() ) )
                {
                    if ( newAnim != GP_ACTION_IDLE )
                    {
                        float duration = animator->GetComponentSafe<ncCharacter>()->GetStateDuration( animator->GetStateIndex(), false );

                        // determine material under the player
                        const char * materialName = soundLayer->GetMaterialNameUnder();
                        int stepsPerAnimCycle = this->GetClassComponentSafe<ncGameplayPlayerClass>()->GetStepsAnimCycle();

                        if ( stepsPerAnimCycle > 0 )
                        {
                            soundLayer->PlaySoundTimeMaterial( newAnim, 0, materialName, duration / stepsPerAnimCycle, true );
                        }
                        else
                        {
                            soundLayer->PlaySoundTimeMaterial( newAnim, 0, materialName, duration, true );
                        }

                        soundLayer->SetSoundPositionOffset( vector3( 0.0f, - this->GetClassComponentSafe<ncGameplayPlayerClass>()->GetStandHeight(), -0.5f) );
                    }
                    else
                    {
                        // Stop movement sounds
                        soundLayer->StopSound();
                    }
                }
            }
            else
            {
                animator->SetJumpMid(true);
            }
        }

        if( this->jumpAnim != -1 )
        {
            if( animator->HasFinished( this->jumpAnim ) )
            {
                this->jumpAnim = -1;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param activate put the ironsight state of the player
*/
void
ncGameplayPlayer::SetIronsight( bool activate )
{
    ncGameplayLiving::SetIronsight( activate );
    
    this->CalculatePlayerSpeed();

    if( this->IsIronsight() )
    {
        this->fbIronsightAngle.SetGoal( ncGameplayPlayer::IRONSIGHT_ANGLE );

        if( this->currentWeapon )
        {
            ncGPWeapon * weapon = this->currentWeapon->GetComponent<ncGPWeapon>();
            if( weapon )
            {
                this->ironsightDownOffset = weapon->GetDownOffset();
            }
        }
    }
    else
    {
        this->fbIronsightAngle.SetGoal( ncGameplayPlayer::NORMAL_ANGLE );
        this->ironsightDownOffset = 0;
    }
    this->fbMove.SetGoal( vector3( 0, 0, 0 ) );
    this->fbMove.SetState( vector3( 0, 0, 0 ) );
    this->fbTurn.SetGoal( vector3( 0, 0, 0 ) );
    this->fbTurn.SetState( vector3( 0, 0, 0 ) );
}

//------------------------------------------------------------------------------
/**
    @param activate if true the player will prone
*/
void
ncGameplayPlayer::SetProning( bool activate )
{
    ncGameplayLiving::SetProning( activate );
    this->CalculatePlayerSpeed();
}

//------------------------------------------------------------------------------
/**
    @param activate if true the player will crouch
*/
void
ncGameplayPlayer::SetCrouching( bool activate )
{
    ncGameplayLiving::SetCrouching( activate );
    this->CalculatePlayerSpeed();
}

//------------------------------------------------------------------------------
/**
    @param activate if true, put the player in first camera mode 
*/
void
ncGameplayPlayer::SetFirstCameraMode( bool activate )
{
    ncCharacter * character = this->GetEntityObject()->GetComponent<ncCharacter>();

    if( character )
    {
        character->SetFirstPersonActive( activate );
    }

    this->SetFirstCameraLod( activate );

    this->isFirstCameraMode = activate;

    this->RefreshScopeState();
}

//------------------------------------------------------------------------------
/**
    @param weapon new current weapon
*/
void
ncGameplayPlayer::SetCurrentWeapon( nEntityObject * weapon )
{
    ncGameplayLiving::SetCurrentWeapon( weapon  );

    this->SetFirstCameraLod( this->isFirstCameraMode );
}

//------------------------------------------------------------------------------
/**
    @param activate indicate if activate
*/
void
ncGameplayPlayer::SetFirstCameraLod( bool activate )
{
    if( this->currentWeapon )
    {
        // fix weapon lod
        ncSceneLod * scene = this->currentWeapon->GetComponent<ncSceneLod>();

        if( scene )
        {
            scene->SetLockedLevel( activate, 0 );
        }

/** ZOMBIE REMOVE
        // fix weapon addons lod
        ncGPWeapon * weaponGameplay = this->currentWeapon->GetComponent<ncGPWeapon>();
        if( weaponGameplay )
        {
            nEntityObject * addonEntity = 0;
            for( int i = 0 ; i < weaponGameplay->GetNumSlots(); ++i )
            {
                addonEntity = weaponGameplay->GetAddonAt( i );
                if( addonEntity )
                {
                    scene = addonEntity->GetComponent<ncSceneLod>();
                    if( scene )
                    {
                        scene->SetLockedLevel( activate, 0 );
                    }
                }
            }
        }
*/
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::RefreshScopeState()
{
    nEntityObject * wpn = this->GetCurrentWeapon();
    ncGPWeapon * weapon = 0;

    if( wpn )
    {
        weapon = wpn->GetComponent<ncGPWeapon>();
    }

    if( weapon )
    {
/** ZOMBIE REMOVE
        this->isInScopeMode = weapon->HasTrait( ncGPWeaponCharsClass::MOD_ENABLE_SCOPE );
        this->isInScopeMode = this->isInScopeMode && this->IsIronsight();
        bool hide = this->isInScopeMode && this->IsFirstCameraMode();

        weapon->SetScopeState( hide );

        ncScene * scene = this->GetEntityObject()->GetComponent<ncScene>();
        if( scene )
        {
            scene->SetHiddenEntity( hide );
        }
*/
        if( this->IsIronsight() )
        {
/** ZOMBIE REMOVE
            if( weapon->HasTrait( ncGPWeaponCharsClass::MOD_ENABLE_SCOPE ) )
            {
                this->ironsightAngle = ncGameplayPlayer::SCOPE_ANGLE;
            }
            else*/
            {
                this->ironsightAngle = ncGameplayPlayer::IRONSIGHT_ANGLE;
            }

            this->fbIronsightAngle.SetGoal( this->ironsightAngle );
        }
        else
        {
            this->fbIronsightAngle.SetGoal( ncGameplayPlayer::NORMAL_ANGLE );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncGameplayPlayer::IsInScope() const
{
    return this->isInScopeMode;
}

//------------------------------------------------------------------------------
/**
    @param activate indicate when activate the lean or not
*/
void
ncGameplayPlayer::SetLeftLean( bool activate )
{
    if( activate )
    {
        ncGameplayPlayerClass * playerClass = this->GetClassComponentSafe<ncGameplayPlayerClass>();
        if( playerClass )
        {
            this->leanDistance = - playerClass->GetLeanDistance();
        }

        this->newDirection = ncLogicAnimator::LA_LEFTLEAN;

        ncSoundLayer * soundLayer = this->GetComponentSafe<ncSoundLayer>();

        if( soundLayer )
        {
            soundLayer->PlaySound( GP_ACTION_LEAN );
        }
    }
    else
    {
        ncLogicAnimator * animator = this->GetComponent<ncLogicAnimator>();
        if( animator )
        {
            if( this->IsIronsight() )
            {
                animator->SetLeanIronsight( false );
            }
            else
            {
                animator->SetLean( false );
            }
        }

        this->leanDistance = 0.0f;
        this->fbLean.SetGoal( vector3( 0, 0, 0 ) );
    }
}

//------------------------------------------------------------------------------
/**
    @param activate indicate when activate the lean or not
*/
void
ncGameplayPlayer::SetRightLean( bool activate )
{
    if( activate )
    {
        ncGameplayPlayerClass * playerClass = this->GetClassComponentSafe<ncGameplayPlayerClass>();
        if( playerClass )
        {
            this->leanDistance = playerClass->GetLeanDistance();
        }

        this->newDirection = ncLogicAnimator::LA_RIGHTLEAN;

        ncSoundLayer * soundLayer = this->GetComponentSafe<ncSoundLayer>();

        if( soundLayer )
        {
            soundLayer->PlaySound( GP_ACTION_LEAN );
        }
    }
    else
    {
        ncLogicAnimator * animator = this->GetComponent<ncLogicAnimator>();
        if( animator )
        {
            if( this->IsIronsight() )
            {
                animator->SetLeanIronsight( false );
            }
            else
            {
                animator->SetLean( false );
            }
        }

        this->leanDistance = 0.0f;
        this->fbLean.SetGoal( vector3( 0, 0, 0 ) );
    }
}

//------------------------------------------------------------------------------
/**
    Sets the player state.

    @param newstate new player's state

    history:
     - 27-Jan-2006   David Reyes    created

*/
void ncGameplayPlayer::SetState( const state newstate )
{
    // NOTE: Assumes it's syncronous
    this->SignalChangePlayerState(this->GetEntityObject(),int(newstate),this->GetEntityObject());

    this->playerState = newstate;

    if( this->playerState == ncGameplayPlayer::IS_DRIVER )
    {
        this->movementFlags &= ~ ( MF_UPDATEPOSITION | MF_UPDATEANGLE );
    }
    else if( this->playerState == ncGameplayPlayer::IS_PASSENGER || this->playerState == ncGameplayPlayer::IS_FROZEN )
    {
        this->movementFlags &= ~ ( MF_UPDATEPOSITION );
        this->movementFlags |=   ( MF_UPDATEANGLE );
    }
    else
    {
        this->movementFlags |= ( MF_UPDATEPOSITION | MF_UPDATEANGLE );
    }
}

//------------------------------------------------------------------------------
/**
    Gets the player state.

    @return player's state

    history:
     - 27-Jan-2006   David Reyes    created
   
*/
const ncGameplayPlayer::state ncGameplayPlayer::GetState() const
{
    return this->playerState;
}

//------------------------------------------------------------------------------
/**
    Deactivates player's physics.

    history:
     - 27-Jan-2006   David Reyes    created
   
*/
void ncGameplayPlayer::DeactivePhysics()
{
    this->GetComponent<ncPhyCharacterObj>()->Deactivate();
    this->collisionPhysics->Deactivate();
}

//------------------------------------------------------------------------------
/**
    Activates player's physics.

    history:
     - 27-Jan-2006   David Reyes    created
   
*/
void ncGameplayPlayer::ActivatePhysics()
{
    this->GetComponent<ncPhyCharacterObj>()->Activates();
    this->collisionPhysics->Activates();
}

//------------------------------------------------------------------------------
/**
    Gets the seat where the player is.

    @return the seat entity

    history:
     - 27-Jan-2006   David Reyes    created
   
*/
nEntityObject* ncGameplayPlayer::GetSeat() const
{
    return this->seat;
}

//------------------------------------------------------------------------------
/**
    Sets the vehicle where the player is.

    @param newseat a seat entity

    history:
     - 27-Jan-2006   David Reyes    created
   
*/
void ncGameplayPlayer::SetSeat( nEntityObject* newseat )
{
    this->seat = newseat;
}

//------------------------------------------------------------------------------
/**
    @param equipment identifier of special equipment
    @param activate if put or remove the equipment
*/
void 
ncGameplayPlayer::SetSpecialEquipment( int equipment, bool activate )
{
    int index = this->specialEquipment.FindIndex( equipment );

    if( activate )
    {
        if( index == -1 )
        {
            this->specialEquipment.PushBack( equipment );
        }
    }
    else
    {
        if( index != -1 )
        {
            this->specialEquipment.EraseQuick( index );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param equipment identifier of special equipment
    @param activate if put or remove the equipment
*/
bool
ncGameplayPlayer::GetSpecialEquipment( int equipment ) const
{
    int index = this->specialEquipment.FindIndex( equipment );
    
    if( index != -1 )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Gets the players physics.

    @return the players physics object (not collision). 

    history:
     - 27-Jan-2006   David Reyes    created
   
*/
ncPhyCharacter* ncGameplayPlayer::GetPlayersPhysics()
{
    return this->collisionPhysics;
}

//------------------------------------------------------------------------------
/**
    @param limits vector with ( minimum limit, maximum limit ) of elevation
*/
void
ncGameplayPlayer::SetElevationLimits( const vector2& limits )
{
    this->elevationMinimum = n_clamp( limits.x, MIN_ANGLEELEVATION, 0 );
    this->elevationMaximum = n_clamp( limits.y, 0, MAX_ANGLEELEVATION );
}

//------------------------------------------------------------------------------
/**
    @param limits [OUT] vector with ( minimum limit, maximum limit ) of elevation
*/
void
ncGameplayPlayer::GetElevationLimits( vector2& limits ) const
{
    limits.x = this->elevationMinimum;
    limits.y = this->elevationMaximum;
}

//------------------------------------------------------------------------------
/**
    @param limits vector with ( minimum limit, maximum limit ) of turn
*/
void
ncGameplayPlayer::SetTurnLimits( const vector2& limits )
{
    this->turnMinimum = n_clamp( limits.x, -N_PI,    0 );
    this->turnMaximum = n_clamp( limits.y,     0, N_PI );
}

//------------------------------------------------------------------------------
/**
    @param limits [OUT] vector with ( minimum limit, maximum limit ) of turn
*/
void
ncGameplayPlayer::GetTurnLimits( vector2& limits ) const
{
    limits.x = this->turnMinimum;
    limits.y = this->turnMaximum;
}

//------------------------------------------------------------------------------
/**
    @param center new player angle center
*/
void
ncGameplayPlayer::SetAngleCenter( const vector2& center )
{
    // normalize turn center between [ -PI,PI ]
    this->angleOrigin.rho = n_normalangle2( center.x );

    // adjust maximum elevation center
    this->angleOrigin.theta = n_clamp( center.y, MIN_ANGLEELEVATION, MAX_ANGLEELEVATION );
}

//------------------------------------------------------------------------------
/**
    @param center [OUT] angle of player center
*/
void
ncGameplayPlayer::GetAngleCenter( vector2& center ) const
{
    center.x = this->angleOrigin.rho;
    center.y = this->angleOrigin.theta;
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::AddShotTimer( const nTime & t )
{
    this->shotTime += t;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncGameplayPlayer::IsFirstShot() const
{
    float rateOfFire = 0;
    if( this->currentWeapon )
    {
        ncGPWeapon * weapon = this->currentWeapon->GetComponentSafe<ncGPWeapon>();
        if( weapon )
        {
            rateOfFire = weapon->GetRateOfFire();
        }
    }

    return ( this->shotTime < rateOfFire );
}

//------------------------------------------------------------------------------
/**
    @param trigger weapon was shooted
    @param pressed weapon trigger is pressed
*/
void
ncGameplayPlayer::SetWeaponTrigger( bool trigger, bool pressed )
{
    this->isWeaponTrigger = trigger;
    this->isWeaponPressed = pressed;

    nNetworkManager * network = nNetworkManager::Instance();
    if( network && network->IsServer() )
    {
        int id = this->GetEntityObject()->GetId();
        nstream data;
        data.SetWrite( true );
        data.UpdateInt( id );
        data.UpdateBool( this->isWeaponTrigger );
        data.UpdateBool( this->isWeaponPressed );
        network->CallRPCAll( nRnsEntityManager::WEAPON_BURST,
            data.GetBufferSize(), data.GetBuffer() );
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::ShotWeapon()
{
    if( this->currentWeapon )
    {
        ncGPWeapon * weapon = 0;
        weapon = this->currentWeapon->GetComponent<ncGPWeapon>();

        nNetworkManager * network = nNetworkManager::Instance();

        if( weapon )
        {
            nArg args[ 2 ];

            bool fire = false;
            switch( weapon->GetFireMode() )
            {
            case ncGPWeaponClass::FM_AUTO:
                fire = this->isWeaponPressed;
                break;
            case ncGPWeaponClass::FM_SEMI:
                fire = this->isWeaponTrigger;
                break;
            }

            ncSoundLayer * soundLayer = this->currentWeapon->GetComponentSafe<ncSoundLayer>();

/** ZOMBIE REMOVE
            bool silencerPut = weapon->HasTrait( ncGPWeaponCharsClass::MOD_ENABLE_SILENCER );
*/
            if( fire )
            {
                if( weapon->GetAmmo() )
                {
                    if( ! this->IsBusy() )
                    {
                        if( soundLayer && ! this->firedLastFrame &&
                            weapon->GetFireMode() == ncGPWeaponClass::FM_AUTO )
                        {
/** ZOMBIE REMOVE
                            if ( ! silencerPut )
*/
                            {
                                soundLayer->PlaySoundTime( GP_ACTION_SHOOTBURST, 10, 0.0f, true );
                            }
/** ZOMBIE REMOVE
                            else
                            {
                                soundLayer->PlaySoundTime( GP_ACTION_SHOOTBURSTSILENCER, 10, 0.0f, true );
                            }
*/
                        }

                        this->firedLastFrame = true;

                        if( network && network->IsServer() )
                        {
                            args[0].Delete(); args[0].SetO( this->GetEntityObject() );            
                            network->SendAction( "ngpshoot", 1, args, true );
                        }
                    }
                }
                else if( this->GetBusy() < ncGameplayLiving::BL_NORMAL )
                {
                    if( network && network->IsServer() )
                    {
                        if( ! weapon->HasFullAmmo() )
                        {
                            bool fastReload = ! weapon->NeedFullReload();
                            bool returnIronsight = false;
                            if( this->IsIronsight() )
                            {
                                args[0].Delete(); args[0].SetO( this->GetEntityObject() );
                                args[1].Delete(); args[1].SetB( false );
                                network->SendAction( "ngpironsight", 2, args, true );
                                returnIronsight = fastReload;
                            }
                            args[0].Delete(); args[0].SetO( this->GetEntityObject() );
                            args[1].Delete(); args[1].SetB( fastReload );
                            
                            if( this->IsIronsight() )
                            {
                                network->SendQueueAction( "ngpreloadweapon", 2, args, true );
                            }
                            else
                            {
                                network->SendAction( "ngpreloadweapon", 2, args, true );
                            }

                            if( returnIronsight )
                            {
                                args[0].Delete(); args[0].SetO( this->GetEntityObject() );
                                args[1].Delete(); args[1].SetB( true );
                                network->SendQueueAction( "ngpironsight", 2, args, true );
                            }
                        }
                    }
                }
            }
            else if( this->firedLastFrame )
            {
                this->firedLastFrame = false;

                if( soundLayer )
                {
                    // stop burst loop sound
                    nSoundScheduler::SoundComponentInfo * sound = 0;
                    int index = InvalidSoundIndex;

                    index = soundLayer->GetSoundIndex( "shootburst" );
                    if( index != InvalidSoundIndex )
                    {
                        sound = nSoundScheduler::Instance()->FindPlayingSound( index );
                        if( sound )
                        {
                            sound->Stop();
                        }
                    }

                    index = soundLayer->GetSoundIndex( "shootburstsilencer" );
                    if( index != InvalidSoundIndex )
                    {
                        sound = nSoundScheduler::Instance()->FindPlayingSound( index );
                        if( sound )
                        {
                            sound->Stop();
                        }
                    }

                    // put release sound
/** ZOMBIE REMOVE
                    if ( ! silencerPut )
                    {
*/
                        soundLayer->PlayAlone( "shootrelase", 1 );
/** ZOMBIE REMOVE
                    }
                    else
                    {
                        soundLayer->PlayAlone( "shootreleasesilencer", 1 );
                    }
*/
                }
            }
        }
    }

    this->isWeaponTrigger = false;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @param gfxserver graphics server
*/
void
ncGameplayPlayer::DebugDraw( nGfxServer2 * const gfxServer )
{
    ncGameplayLiving::DebugDraw( gfxServer );

    if( ! gfxServer )
    {
        return;
    }

    bool debugGraphics = nDebugServer::Instance()->GetFlagEnabled( "rnsview", "debuggraphics" );
    if( debugGraphics )
    {
        nArray<vector3> points;

        for( int i = 0 ; i < this->savedMoves.Size() ; ++i )
        {
            points.Append( this->savedMoves[ i ].position );
        }

        vector3 actualPos;
        this->collisionPhysics->GetPosition( actualPos );
        points.Append( actualPos );

        if( points.Size() > 1 )
        {
            gfxServer->BeginLines();
            gfxServer->DrawLines3d( points.Begin(), points.Size(), vector4( 1.0f, 1.0f, 1.0f, 0.5f ) );
            gfxServer->EndLines();
        }
    }
}
#endif//!NGAME

//------------------------------------------------------------------------------
