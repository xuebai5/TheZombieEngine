//------------------------------------------------------------------------------
//  ncgameplayplayer_move.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "entity/nentityobjectserver.h"

#include "rnsgameplay/ngameplayutils.h"

#include "ncgameplayplayer/ncgameplayplayer.h"
#include "rnsgameplay/ncgpweapon.h"
#include "rnsgameplay/ncgpweaponclass.h"

#include "nphysics/nphygeomray.h"
#include "nphysics/ncphycharacter.h"
#include "nphysics/ncphysicsobj.h"

#include "ncsoundlayer/ncsoundlayer.h"
#include "nclogicanimator/nclogicanimator.h"

#include "ncsoundlayer/ncsoundlayer.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "rnsgameplay/nrnsentitymanager.h"

#include "ntrigger/ngameevent.h"
#include "ntrigger/nctrigger.h"
#include "ntrigger/ncareaevent.h"

//------------------------------------------------------------------------------
/**
    @param timestamp time of the move
    @param pos last position
    @param forward forward direction
    @param step lateral direction
    @param angle last angle
    @param yaw increment of angle yaw
    @param pitch increment of angle pitch
    @param flags movement flags
*/
void
ncGameplayPlayer::ServerMove( float timestamp, const vector3 & /*pos*/, const int forward, const int step,
                            const vector2 & angle, const float yaw, const float pitch, const int flags )
{
    int realFlags = flags;

    // disable sprinting with backward movement or strafe
    if( ( forward > 0 ) || ( step != 0 ) )
    {
        realFlags &= ~IS_SPRINTING;
    }    

    this->SetMovementState( realFlags, forward, step );

    this->incrementAngles.theta = pitch;
    this->incrementAngles.rho = yaw;

    matrix44 moveMatrix;
    moveMatrix.ident();

    polar2 playerAngles;
    this->GetShotAngles( playerAngles );
    moveMatrix.rotate_y( playerAngles.rho );

    vector3 dirMove( moveMatrix.z_component() );
    vector3 dirStep( moveMatrix.x_component() );

    this->inputVelocity = vector3();
    this->inputVelocity += dirMove*float(forward);
    this->inputVelocity += dirStep*float(step);
    this->inputVelocity.norm();

    this->UpdateVelocity();

    // apply jump movement
    if( realFlags & ncGameplayPlayer::IS_JUMPING )
    {
        this->Jump();
    }

    this->SaveMoveState( timestamp );

    float deltaTime = 0;

    // get actual time
    nNetworkManager * network = nNetworkManager::Instance();
    float actualTime = timestamp;
    if( network )
    {
        actualTime = float( network->GetClock() );
    }

    // adjust angle
    if( this->GetEntityObject() != nRnsEntityManager::Instance()->GetLocalPlayer() )
    {
        this->interpolateAngle = true;

        this->desiredAngles.theta = angle.x;
        this->desiredAngles.rho = angle.y;
 
        deltaTime = n_max( 0, actualTime - timestamp );

        if( deltaTime > 0 )
        {
            this->CalculateAngleMove( this->desiredAngles, this->desiredAngles, deltaTime );
        }   
    }

    this->SendPlayerAdjust();
}

//------------------------------------------------------------------------------
/**
    @param timestamp time of the move
    @param forward forward direction
    @param step lateral direction
    @param yaw increment of angle yaw
    @param pitch increment of angle pitch
    @param flags movement flags
*/
void
ncGameplayPlayer::MoveAutonomous( float timestamp, const int forward, const int step,
                            const float yaw, const float pitch, const int flags )
{
    int realFlags = flags;

    // disable sprinting with backward movement or strafe
    if( ( forward > 0 ) || ( step != 0 ) )
    {
        realFlags &= ~IS_SPRINTING;
    }    

    this->SetMovementState( realFlags, forward, step );

    this->incrementAngles.theta = pitch;
    this->incrementAngles.rho = yaw;

    matrix44 moveMatrix;
    moveMatrix.ident();

    polar2 playerAngles;
    this->GetShotAngles( playerAngles );
    moveMatrix.rotate_y( playerAngles.rho );

    vector3 dirMove( moveMatrix.z_component() );
    vector3 dirStep( moveMatrix.x_component() );

    this->inputVelocity = vector3();
    this->inputVelocity += dirMove*float(forward);
    this->inputVelocity += dirStep*float(step);
    this->inputVelocity.norm();

    this->UpdateVelocity();

    // apply jump movement
    if( realFlags & ncGameplayPlayer::IS_JUMPING )
    {
        this->Jump();
    }

    this->SaveMoveState( timestamp );
}

//------------------------------------------------------------------------------
/*
    @param timestamp time of the move
    @param flags movement flags
    @param position movement position
    @param velocity movement velocity
    @param angle movement angle
    @param angleIncrement movement angle increment
*/
void
ncGameplayPlayer::AdjustPosition( float timestamp, const int /*flags*/, 
                            const vector3 & position, const vector3 & velocity, 
                            const vector2 & angle, const vector2 & angleIncrement )
{
    // remove old moves
    while( this->savedMoves.Size() > 0 && this->savedMoves[ 0 ].timestamp <= timestamp )
    {
        this->savedMoves.Erase( 0 );
    }

    // save adjust move
    SavedMove move;

    move.timestamp = timestamp;
    move.position = position;
    move.velocity = velocity;

    this->savedMoves.Insert( 0, move );

    // update moves after adjust
    float deltaTime = 0;
    for( int i = 1 ; i < this->savedMoves.Size() ; ++i )
    {
        deltaTime = this->savedMoves[i].timestamp - this->savedMoves[i-1].timestamp;

        this->savedMoves[i].position = this->savedMoves[i-1].position + this->savedMoves[i-1].velocity*deltaTime;
    }

    // get actual time
    nNetworkManager * network = nNetworkManager::Instance();
    float actualTime = timestamp;
    if( network )
    {
        actualTime = float( network->GetClock() );
    }

    // adjust angle
    if( this->GetEntityObject() != nRnsEntityManager::Instance()->GetLocalPlayer() )
    {
        this->interpolateAngle = true;

        this->desiredAngles.theta = angle.x;
        this->desiredAngles.rho = angle.y;
 
        this->incrementAngles.theta = angleIncrement.x;
        this->incrementAngles.rho = angleIncrement.y;
 
        deltaTime = n_max( 0, actualTime - timestamp );

        if( deltaTime > 0 )
        {
            this->CalculateAngleMove( this->desiredAngles, this->desiredAngles, deltaTime );
        }
    }

    //this->interpolatePos = true;
    deltaTime = n_max( 0, actualTime - this->savedMoves.Back().timestamp );

    vector3 pos;
    pos = this->savedMoves.Back().position + this->savedMoves.Back().velocity * deltaTime;
    this->SetPhysicPosition( pos );

    this->velocity = this->savedMoves.Back().velocity;
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::SendPlayerAdjust() const
{
    nNetworkManager * network = nNetworkManager::Instance();

    if( network && network->IsServer() )
    {
        int flags = 0;

        flags |= this->IsSprinting() ? IS_SPRINTING : 0;
        flags |= this->isJumping ? IS_JUMPING : 0;

        vector3 position;

        this->GetPhysicPosition( position );
        
        nArg args[ 13 ];

        args[0].Delete(); args[0].SetO( this->GetEntityObject() );
        args[1].Delete(); args[1].SetF( float( network->GetClock() ) );
        args[2].Delete(); args[2].SetI( flags );
        args[3].Delete(); args[3].SetF( position.x );
        args[4].Delete(); args[4].SetF( position.y );
        args[5].Delete(); args[5].SetF( position.z );
        args[6].Delete(); args[6].SetF( this->velocity.x );
        args[7].Delete(); args[7].SetF( this->velocity.y );
        args[8].Delete(); args[8].SetF( this->velocity.z );
        args[9].Delete(); args[9].SetF( this->upperAngles.theta );
        args[10].Delete(); args[10].SetF( this->upperAngles.rho );
        args[11].Delete(); args[11].SetF( this->incrementAngles.theta );
        args[12].Delete(); args[12].SetF( this->incrementAngles.rho );
        network->SendAction( "ngpplayeradjust", 13, args, false );
    }
}

//------------------------------------------------------------------------------
/**
    @param flags movement flags
    @param forward movement in the forward direction
    @param step movement in the lateral direction
*/
void
ncGameplayPlayer::SetMovementState( const int flags, const int forward, const int step )
{
    this->SetSprinting( ( IS_SPRINTING & flags ) != 0 );            
    this->MovementFlag( MF_BLOCKSPRINT, (MF_BLOCKSPRINT & flags) != 0 );

    if( forward != 0 || step != 0 )
    {
        if ( step > 0 )
        {
            this->newDirection = ncLogicAnimator::LA_RIGHTSTRAFE;
        }
        else if ( step < 0 )
        {
            this->newDirection = ncLogicAnimator::LA_LEFTSTRAFE;
        }

        if ( forward < 0 )
        {
            this->newDirection = ncLogicAnimator::LA_FORWARD;
        }
        else if ( forward > 0 )
        {
            this->newDirection = ncLogicAnimator::LA_BACKWARD;
        }
    }
    else
    {
        this->newDirection = ncLogicAnimator::LA_STOPPED;
    }

    if( flags & ncGameplayPlayer::IS_LEFTLEAN )
    {
        this->SetLeftLean(  true );
    }
    else if( flags & ncGameplayPlayer::IS_RIGHTLEAN )
    {
        this->SetRightLean(  true );
    }
    else
    {
        this->SetLeftLean(  false );
        this->SetRightLean( false );
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::Jump()
{
    // apply jump movement
    if( ! this->isFalling && ! this->isTired && ! this->isJumping && !this->isClimbing )
    {
        ncGameplayPlayerClass * playerclass = this->GetClassComponentSafe<ncGameplayPlayerClass>();
        ncLogicAnimator * animator = this->GetComponent<ncLogicAnimator>();
        ncSoundLayer * soundLayer = this->GetComponentSafe<ncSoundLayer>();

        // Consumes stamina
        this->SetStamina(this->GetStamina() - playerclass->GetJumpStaminaLost());         

        this->jumpVelocity.y += playerclass->GetJumpVertical();

        this->velocity.x = this->velocity.x;
        this->velocity.y = this->jumpVelocity.y;
        this->velocity.z = this->velocity.z;

        // If stamina is exhausted jump half
        if (this->GetStamina() > 0)
        {
            this->jumpTime = playerclass->GetJumpTime();
        }
        else
        {
            this->isTired = true;            
            this->lastTiredTime = playerclass->GetTiredTime();
            this->jumpTime = playerclass->GetJumpTime() * 0.5f;
        }        

        if( animator )
        {
            animator->SetIdle();
            this->jumpAnim = animator->SetJumpStart();
            this->SwitchOnMovementEvent( 10 );
        }

        if( soundLayer )
        {
            soundLayer->PlaySound( GP_ACTION_JUMPSTART, false );
        }

        this->isJumping = true;                
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::UpdateVelocity()
{
    if( ! this->isFalling )
    {
        this->velocity = this->inputVelocity * this->GetVelocityFactor() + this->jumpVelocity;
    }
    else
    {
        this->velocity.x = this->velocity.x;
        this->velocity.y = this->jumpVelocity.y;
        this->velocity.z = this->velocity.z;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::UpdatePlayerPosition()
{
    if( this->collisionEntity )
    {
        ncTransform * graphics = this->GetComponent<ncTransform>();

        // update position
        vector3 newPosition = this->collisionTransform->GetPosition();
        newPosition.y -= this->collisionPhysics->GetWide() / 2.0f;
        if( ! this->isFirstCameraMode && graphics )
        {
            // set position in 3rd camera mode
            graphics->SetPosition( newPosition );
        }

        newPosition.y += this->cameraHeight;

        this->diffPosition = this->shotPosition - newPosition;
        this->shotPosition = newPosition + this->fbLean.GetState();
        if( this->isFirstCameraMode && graphics )
        {
            // set position in 1st camera mode
            graphics->SetPosition( this->shotPosition );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param deltaTime increment of time since last move
*/
void
ncGameplayPlayer::MovePlayerPosition( const float deltaTime )
{
    if( this->collisionEntity )
    {
        bool saveMove = false;
        vector3 lastVelocity;

        this->CheckFalling();
        if( this->isJumping && ( this->jumpTime > 0.0f ) )
        {
            float deltaJump = deltaTime < this->jumpTime ? deltaTime : this->jumpTime;
            this->jumpTime -= deltaJump;
        }

        if( this->isJumping && this->jumpTime <= 0.0f )
        {
            this->isJumping = false;

            this->jumpVelocity = vector3();

            this->UpdateVelocity();

            saveMove = true;
        }

        if( ! this->isJumping )
        {
            this->collisionPhysics->GetLinearVelocity( lastVelocity );

            this->velocity.y = lastVelocity.y;
        }


        this->collisionPhysics->SetLinearVelocity( this->velocity );
        this->speed = this->velocity.len();

        if( this->savedMoves.Size() > 0 )
        {
            lastVelocity = this->savedMoves.Back().velocity;
            vector3 diff( lastVelocity - this->velocity );

            saveMove = saveMove || ( diff.lensquared() > 0.0001 );
        }

        if( saveMove )
        {
            nNetworkManager * network = nNetworkManager::Instance();
            if( network )
            {
                this->SaveMoveState( float( network->GetClock() ) );
            }
            this->SendPlayerAdjust();
        }
    }        
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::CheckFalling()
{
    const float MAGNITUDE_BUMPING = 0.05f;
    const float RAY_LENGTH = 10.0f;
    const vector3 VERTICAL( 0, 1, 0 );
    const float MARGIN = 0.01f;

    /* check for falling in the new position */
    vector3 rayPosition;

    rayPosition = this->collisionPhysics->GetEntityObject()->GetComponentSafe<ncTransform>()->GetPosition();
    rayPosition.y += MARGIN;
    
    this->collisionRay->SetLength( RAY_LENGTH );
    this->collisionRay->SetDirection( -VERTICAL );
    this->collisionRay->SetPosition( rayPosition );

    // disable the player
    ncPhysicsObj * physicObj = this->GetComponent<ncPhysicsObj>();

    bool enablePhysic = false;
    if( physicObj )
    {
        enablePhysic = physicObj->IsEnabled();
        physicObj->Disable();
    }

    // check collision with the floor
    int numcontacts( nPhysicsServer::Instance()->Collide( 
        this->collisionRay, ncGameplayPlayer::MAX_CONTACTS, this->contacts ) );


    bool newFalling = true;
    if( numcontacts )
    {
        vector3 terrainPosition;

        // Get the first collision above player
        float minDist = RAY_LENGTH * RAY_LENGTH;
        float dist = 0;
        int ind = -1;
        for( int i = 0 ; i < numcontacts ; ++i )
        {
            this->contacts[ i ].GetContactPosition( terrainPosition );
            dist = ( rayPosition - terrainPosition ).lensquared();
            // check that is near and is above player
            if( ( dist < minDist ) && ( terrainPosition.y < rayPosition.y ) )
            {
                minDist = dist;
                ind = i;
            }
        }

        if( ind != -1 )
        {
            // get contact data
            vector3 terrainNormal;
            this->contacts[ ind ].GetContactPosition( terrainPosition );
            this->contacts[ ind ].GetContactNormal( terrainNormal );

            // check angle with terrain for limit movement
            // fix normal
            if( terrainNormal.dot( rayPosition - terrainPosition ) < 0 )
            {
                terrainNormal *= -1.0;
            }

            // calculate angle of terrain
            vector3 projection = terrainNormal;
            projection.y = 0.0f;

            if( projection.lensquared() < TINY )
            {
                projection.x = 1.0;
            }

            float angle = acos( projection.dot( terrainNormal ) / projection.len() );

            // adjust angle,  0 is horizontal terrain
            angle = N_HALFPI - angle;
            angle = n_rad2deg( angle );

            float difference = ncGameplayPlayer::STEP_HEIGHT;
            difference += ( this->collisionPhysics->GetHeight() + this->collisionPhysics->GetWide() ) * 0.5f;
            if( rayPosition.y - terrainPosition.y <= difference )
            {
                newFalling = false;

                if( this->playerClass )
                {
                    this->isClimbing = angle >= playerClass->GetMaxClimbSlope();
                }
            }
        }

    }

    // double check
    int numContacts( this->GetPlayersPhysics()->Collide(ncGameplayPlayer::MAX_CONTACTS,this->contacts) );

    if( numContacts && !this->isClimbing )
    {
        int indexLowestY(0);

        vector3 contactPosition;

        this->contacts[0].GetContactPosition( contactPosition );

        float ContactsPointInY(contactPosition.y);

        // check for the lowest contact in Y axis
        for( int index(1); index < numContacts; ++index )
        {
            this->contacts[index].GetContactPosition( contactPosition );

            // assumes that Y it's the vertical axis on the game, and the Y it's positive going up
            if( contactPosition.y < ContactsPointInY )
            {
                indexLowestY = index;
                ContactsPointInY = contactPosition.y;
            }

        }

        vector3 normal;

        this->contacts[indexLowestY].GetContactNormal( normal );


        float maxAngle(n_min(playerClass->GetMaxClimbSlope(),90));

        maxAngle /= float(90);

        if( fabs(normal.y) < maxAngle )
        {
            this->isClimbing = true;            
        }
    }

    if( newFalling == true ) 
    {
        vector3 speed;

        this->GetPlayersPhysics()->GetLinearVelocity( speed );

        if( speed.y < 0 ) 
        {

            if( numContacts )
            {
                vector3 contactPosition;

                vector3 playerPosition;

                float const distanceToCap((this->GetPlayersPhysics()->GetHeight() - this->GetPlayersPhysics()->GetWide()) / 2.f);

                this->GetPlayersPhysics()->GetPosition( playerPosition );            
                // check just it's the botton cap from the capsule
                for( int index(0); index < numContacts; ++index )
                {
                    this->contacts[index].GetContactPosition( contactPosition );

                    // assumes that Y it's the vertical axis on the game, and the Y it's positive going up
                    float diffPositionOnY( playerPosition.y - contactPosition.y );

                    if( diffPositionOnY > distanceToCap )
                    {
                        newFalling = false;
                        break;
                    }
                }
            }
        }
    }

    // enable the player
    if( physicObj && enablePhysic )
    {
        physicObj->Enable();
    }

    bool oldFalling = this->isFalling;
    this->isFalling = newFalling;

    // update falling state
    if( oldFalling && ! newFalling )
    {
        // the player meets the floor
        /// @todo ZHEN do falling damage
        ncLogicAnimator * animator = this->GetComponent<ncLogicAnimator>();
        if( animator )
        {
            this->jumpAnim = animator->SetJumpEnd();
            this->SwitchOnMovementEvent( 10 );
            this->actualAnim = GP_ACTION_JUMPSTART;

            // stop moving sound
            ncSoundLayer* soundLayer = this->GetComponent <ncSoundLayer>();
            if ( soundLayer )
            {
                soundLayer->StopSound();
            }
        }

        ncSoundLayer * soundLayer = this->GetComponentSafe<ncSoundLayer>();
        if( soundLayer )
        {
            soundLayer->PlaySound( GP_ACTION_JUMPEND, false );
        }

        this->fbFalling.SetGoal( MAGNITUDE_BUMPING );

        this->UpdateVelocity();

        nNetworkManager * network = nNetworkManager::Instance();
        if( network )
        {
            this->SaveMoveState( float( network->GetClock() ) );
        }

        this->SendPlayerAdjust();
    }

    if( ! oldFalling && newFalling )
    {
        // begin free fall
        /// @todo ZHEN falling animation

        this->UpdateVelocity();

        nNetworkManager * network = nNetworkManager::Instance();
        if( network )
        {
            this->SaveMoveState( float( network->GetClock() ) );
        }

        this->SendPlayerAdjust();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::CalculatePlayerSpeed()
{
    ncGameplayPlayerClass* playerClass = this->GetClassComponentSafe <ncGameplayPlayerClass>();

    if( playerClass )
    {
        bool ironsight = this->IsIronsight();

        if( this->IsCrouching() )
        {
            if( ironsight )
            {
                this->sprintVelocityFactor = playerClass->GetCrouchIronSightSpeed();
                this->runVelocityFactor = playerClass->GetCrouchIronSightSpeed();
            }
            else
            {
                this->sprintVelocityFactor = playerClass->GetCrouchSpeed();
                this->runVelocityFactor = playerClass->GetCrouchSpeed();
            }
        }
        else if( this->IsProne() )
        {
            if( ironsight )
            {
                // In ironsight prone cannot move
                this->sprintVelocityFactor = 0;
                this->runVelocityFactor = 0;
            }
            else
            {
                this->sprintVelocityFactor = playerClass->GetProneSpeed();
                this->runVelocityFactor = playerClass->GetProneSpeed();
            }
        }
        else if ( this->IsSwimming() )
        {
            this->sprintVelocityFactor = playerClass->GetSwimSpeed();
            this->runVelocityFactor = playerClass->GetSwimSpeed();
        }
        else if ( ironsight )
        {
            this->runVelocityFactor = playerClass->GetIronsightSpeed();
            this->sprintVelocityFactor = playerClass->GetRunIronsightSpeed();
        }
        else // The player is standing 
        {
            this->runVelocityFactor = playerClass->GetSpeed();
            this->sprintVelocityFactor = playerClass->GetRunSpeed();
        }
    }
}

//------------------------------------------------------------------------------
/**
    @returns the velocity factor of player
*/
float
ncGameplayPlayer::GetVelocityFactor()const
{
    if( this->IsSprinting() && ! ( this->movementFlags & MF_BLOCKSPRINT ) && ! this->isTired )
    {
        return this->sprintVelocityFactor;
    }
    return this->runVelocityFactor;
}

//------------------------------------------------------------------------------
/**
    @param time time of the move
*/
void
ncGameplayPlayer::SaveMoveState( float time )
{
    if( this->savedMoves.Size() >= ncGameplayPlayer::MAX_SAVEDMOVE_STATES )
    {
        this->savedMoves.Erase( 0 );
    }

    SavedMove move;

    move.timestamp = time;
    this->collisionPhysics->GetPosition( move.position );
    move.velocity = this->velocity;

    this->savedMoves.Append( move );
}

//------------------------------------------------------------------------------
/**
    @param oldAngle last angle
    @param newAngle new angle
    @param deltaTime frame time since last angle to new angle
*/
void
ncGameplayPlayer::CalculateAngleMove( const polar2 & oldAngle, polar2 & newAngle, const float deltaTime ) const
{
    newAngle = oldAngle;

    // recover from recoil
    if( (this->recoilDifference.x > 0.0f ||this->recoilDifference.y > 0.0f ) && this->currentWeapon )
    {
        vector2 recover;
        recover.x = this->recoilRecover.x * deltaTime;
        recover.y = this->recoilRecover.y * deltaTime;
        
        recover.x = n_min( recover.x, this->recoilDifference.x );
        recover.y = n_min( recover.y, this->recoilDifference.y );

        newAngle.rho += recover.x;
        newAngle.theta -= recover.y;
    }

    // calculate angle factor
    const float LOOK_VELOCITY = 2.0f;
    const float LOOK_IRONSIGHT_FACTOR = 0.5f;
    const float LOOK_DIZZY_FACTOR = 0.3f;

    float angleFactor = LOOK_VELOCITY;
    // When ironsight or dizzy then apply a factor for alterate gameplay
    if( this->IsIronsight() )
    {
        angleFactor *= LOOK_IRONSIGHT_FACTOR;
    }

    if ( this->IsDizzy() )
    {
        angleFactor *= LOOK_DIZZY_FACTOR;
    }

    // apply desiree movement
    newAngle.theta -= this->incrementAngles.theta * deltaTime * angleFactor;
    newAngle.rho   += this->incrementAngles.rho * deltaTime * angleFactor;

    float anglelimit;

    // adjust soft limits of elevation angles
    anglelimit = this->angleOrigin.theta + this->elevationMaximum;
    if( newAngle.theta > anglelimit )
    {
        newAngle.theta = anglelimit;
    }

    anglelimit = this->angleOrigin.theta + this->elevationMinimum;
    if( newAngle.theta < anglelimit )
    {
        newAngle.theta = anglelimit;
    }

    // adjust absolute limits of elevation angle
    newAngle.theta = n_clamp( newAngle.theta, MIN_ANGLEELEVATION, MAX_ANGLEELEVATION );

    // normalize turn angle between [ -PI,PI ]
    newAngle.rho = n_normalangle2( newAngle.rho );

    // calculate turn limits
    float minimumLimit = n_normalangle2( this->angleOrigin.rho + this->turnMinimum );
    float maximumLimit = n_normalangle2( this->angleOrigin.rho + this->turnMaximum );

    // adjust turn limits
    if( minimumLimit < maximumLimit )
    {
        newAngle.rho = n_clamp( newAngle.rho, minimumLimit, maximumLimit );
    }
    else if( ( newAngle.rho > maximumLimit ) && ( newAngle.rho < minimumLimit ) )
    {
        float distanceMaximum = n_abs( newAngle.rho - maximumLimit );
        float distanceMinimum = n_abs( newAngle.rho - minimumLimit );

        newAngle.rho = ( distanceMaximum < distanceMinimum ) ? maximumLimit : minimumLimit;
    }
}

//------------------------------------------------------------------------------
/**
    @param deltaTime increment of time since last move
*/
void
ncGameplayPlayer::UpdateRecoil( const float deltaTime )
{
    // recover from recoil
    if( (this->recoilDifference.x > 0.0f ||this->recoilDifference.y > 0.0f ) && this->currentWeapon )
    {
        vector2 recover;

        recover.x = this->recoilRecover.x * deltaTime;
        recover.y = this->recoilRecover.y * deltaTime;

        this->recoilDifference.x -= n_min( recover.x, this->recoilDifference.x );
        this->recoilDifference.y -= n_min( recover.y, this->recoilDifference.y );
    }
}

//------------------------------------------------------------------------------
/**
    @param deltaTime increment of time since last move
*/
void
ncGameplayPlayer::MovePlayerAngle( const float deltaTime )
{
    polar2 newAngle;

    this->CalculateAngleMove( this->upperAngles, newAngle, deltaTime );
    if( this->interpolateAngle )
    {
        this->CalculateAngleMove( this->desiredAngles, this->desiredAngles, deltaTime );

        float lerp = n_min( deltaTime * 2.0f, 1.0f );

        newAngle.lerp( this->desiredAngles, lerp );
    }

    this->UpdateDiffAngles( newAngle );
    this->upperAngles = newAngle;

    this->UpdateRecoil( deltaTime );
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayPlayer::ApplyAngles()
{
    const float maxDiff = 0.7f;

    // constrain lower angle to upper angle
    this->lowerAngles.rho += N_TWOPI;
    this->upperAngles.rho += N_TWOPI;

    if( this->velocity.lensquared() >= N_TINY )
    {
        // equal both angles if moving
        this->lowerAngles.rho = this->upperAngles.rho;

        // calculate look direction
        matrix44 viewMatrix;
        viewMatrix.rotate_y( this->upperAngles.rho );

        vector3 look = viewMatrix * vector3( 0, 0, -1 );

        // calculate relationship between look and velocity
        vector3 vel = this->velocity;
        vel.y = 0;
        look.y = 0;

        vel.norm();
        look.norm();

        float angle = n_acos(vel % look);

        float orientation = (vel*look).y;

        const float MIN_ANGLE_DIFF = 0.3f;
        const float STEP_ANGLE = N_HALFPI * 0.5f;

        // if moving forward + step
        if( n_abs( angle - STEP_ANGLE ) <= MIN_ANGLE_DIFF )
        {
            if( orientation > 0 )
            {
                this->lowerAngles.rho = this->upperAngles.rho - STEP_ANGLE;
            }
            else
            {
                this->lowerAngles.rho = this->upperAngles.rho + STEP_ANGLE;
            }
        }
        // if moving backward + step
        else if( n_abs( angle - ( STEP_ANGLE + N_HALFPI ) ) <= MIN_ANGLE_DIFF )
        {
            if( orientation > 0 )
            {
                this->lowerAngles.rho = this->upperAngles.rho + STEP_ANGLE;
            }
            else
            {
                this->lowerAngles.rho = this->upperAngles.rho - STEP_ANGLE;
            }
        }
    }
    else
    {
        float currentDiff = this->lowerAngles.rho - this->upperAngles.rho;
        if( n_abs( currentDiff ) > N_PI )
        {
            if( this->lowerAngles.rho > this->upperAngles.rho )
            {
                this->lowerAngles.rho -= N_TWOPI;
            }
            else
            {
                this->upperAngles.rho -= N_TWOPI;
            }
            currentDiff = this->lowerAngles.rho - this->upperAngles.rho;
        }

        if( n_abs( currentDiff ) > ( maxDiff + 0.0001f ) )
        {
            this->lowerAngles.rho = this->upperAngles.rho + n_sgn( currentDiff ) * maxDiff;
            ncLogicAnimator * animator = this->GetComponent<ncLogicAnimator>();
            if( animator && ( this->velocity.lensquared() <= N_TINY ))
            {
                if( currentDiff > 0 )
                {
                    animator->SetMovementPose( ncLogicAnimator::LA_RIGHTSTRAFE );
                }
                else
                {
                    animator->SetMovementPose( ncLogicAnimator::LA_LEFTSTRAFE );
                }
                animator->SetRoll();
                this->MovementFlag( MF_BLOCKLOWERANIM, true );
            }
        }
        else
        {
            this->MovementFlag( MF_BLOCKLOWERANIM, false );
        }
    }

    this->lowerAngles.rho = n_normalangle2( this->lowerAngles.rho );
    this->upperAngles.rho = n_normalangle2( this->upperAngles.rho );

    // calculate shot direction
    matrix44 viewMatrix;
    viewMatrix.rotate_x( this->upperAngles.theta );
    viewMatrix.rotate_y( this->upperAngles.rho );

    this->shotDirection = viewMatrix * vector3( 0, 0, -1 );

    // adjust absolute limits of first person angle
    viewMatrix.ident();
    if( this->isProne )
    {
        viewMatrix.rotate_x( n_clamp( this->upperAngles.theta, 0, MAX_ANGLETHIRD ) );
    }
    else
    {
        viewMatrix.rotate_x( n_clamp( this->upperAngles.theta, MIN_ANGLETHIRD, MAX_ANGLETHIRD ) );
    }
    viewMatrix.rotate_y( this->upperAngles.rho );
    vector3 thirdPersonDirection = viewMatrix * vector3( 0, 0, -1 );

    polar2 viewangles;
    polar2 physangles = this->lowerAngles;

    if( this->isFirstCameraMode )
    {
        viewangles = this->upperAngles;
    }
    else
    {
        viewangles = this->lowerAngles;
    }

    viewangles.rho += PI;
    viewangles.theta = -viewangles.theta;
    physangles.rho += PI;
    physangles.theta = -physangles.theta;

    ncTransform * transform = this->GetComponentSafe<ncTransform>();
    if( transform )
    {
        // Physics needs lower angles
        transform->EnableUpdate( ncTransform::cPhysics );
        transform->SetPolar( physangles );

        // Update the others with correct angles
        transform->EnableUpdate( static_cast<ncTransform::compUpdate>(~0) );
        transform->DisableUpdate( ncTransform::cPhysics );
        transform->SetPolar( viewangles );
        transform->EnableUpdate( static_cast<ncTransform::compUpdate>(~0) );
    }

    ncCharacter * animation = this->GetComponentSafe<ncCharacter>();
    if( animation )
    {
        animation->SetJointLookAtVector( 0, thirdPersonDirection );
    }
}

//------------------------------------------------------------------------------
