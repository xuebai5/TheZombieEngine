#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  nclogicanimator_main.cc
//  (C) Conjurer Services, S.A. 2006
//------------------------------------------------------------------------------
#include "nclogicanimator/nclogicanimator.h"
#include "util/nrandomlogic.h"

#include "ncgameplayliving/ncgameplayliving.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/*
    Animation nomenclature:

    XXX_handle_action

    Where the first X is the direction, sampled to the key usually used to be played.
    So, we've got W, S, A, D, $ for 'joker'
    The second X refers to the body. 'S' for the upper part, 'I' the lower, and '$' 
    for full body animation
    The third X refers to the state. We've got the following states:

        - 'S' for Stand
        - 'I' for Ironsight
        - 'C' for Crouch
        - 'P' for Prone
		- 'Q' for Left lean
		- 'E' for Right lean

    'Handle' refers wether the animation have something at their hands, so, we've got 2a for
    weapons handle with two hands, and 1a for animations with single hand weapons.

    Finally, the action signs what must do the animation, like walk, stand, run, reload, etc...
*/

#define CHAR_JOKER '$'

//------------------------------------------------------------------------------
nNebulaComponentObject(ncLogicAnimator,ncCharacter);

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncLogicAnimator::ncLogicAnimator() : 
    firstPersonIndex(-1),
    upperThirdPersonIndex(-1),
    lowerThirdPersonIndex(-1),
    state (LA_STATE_STAND),
    prevState (LA_STATE_STAND),
    action (GP_ACTION_IDLE),
    changeState (false),
    direction(LA_STOPPED),
    upperBlocked(false),
    lowerBlocked(false),
    fpersonBlocked(false),
    prevAction(GP_ACTION_NONE),
    blockedAction(GP_ACTION_NONE),
    descriptor(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncLogicAnimator::~ncLogicAnimator()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
void 
ncLogicAnimator::InitInstance(nObject::InitInstanceMsg initType)
{
    ncCharacter::InitInstance (initType);

    if ( initType != nObject::NewInstance && initType != nObject::ReloadedInstance )
    {
        ncCharacter::Load();
        this->SetAnimationAction (this->action, false, false, true);
    }
}

//------------------------------------------------------------------------------
/**
    SetFlashlight
*/
int
ncLogicAnimator::SetFlashlight()
{
    return this->SetAnimationAction (GP_ACTION_FLASHLIGHT, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetInjection
*/
int
ncLogicAnimator::SetInjection()
{
    return this->SetAnimationAction (GP_ACTION_INJECTION, false, false, true);
}

//------------------------------------------------------------------------------
/**
    @param direction animation move direction
*/
void
ncLogicAnimator::SetMovementPose( int direction )
{
	this->newDirection = eAnimationDirection ( direction );
}

//------------------------------------------------------------------------------
/**
    SetWalk
*/
int
ncLogicAnimator::SetWalk()
{
    return this->SetAnimationAction (GP_ACTION_WALK, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetRun
*/
int
ncLogicAnimator::SetRun()
{
    return this->SetAnimationAction (GP_ACTION_RUN, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetLookAround
*/
int
ncLogicAnimator::SetLookAround()
{
    return this->SetAnimationAction (GP_ACTION_LOOKAROUND, false, false, true);
}

//------------------------------------------------------------------------------
/**
*/
int
ncLogicAnimator::SetSilencerOn()
{
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_SILENCERON, false, false, false);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
*/
int
ncLogicAnimator::SetSilencerOff()
{
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_SILENCEROFF, false, false, false);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
*/
int
ncLogicAnimator::SetSwitchFireMode()
{
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_SWITCH, false, false, true);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetStrafeLeft
*/
int
ncLogicAnimator::SetStrafeLeft()
{
    return this->SetAnimationAction (GP_ACTION_STRAFELEFT, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetStrafeRight
*/
int
ncLogicAnimator::SetStrafeRight()
{
    return this->SetAnimationAction (GP_ACTION_STRAFERIGHT, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetSprint
*/
int
ncLogicAnimator::SetSprint()
{
    return this->SetAnimationAction (GP_ACTION_SPRINT, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetIdle
*/
int
ncLogicAnimator::SetIdle()
{
    int index = -1;
    index = this->SetAnimationAction (GP_ACTION_IDLE, false, true, true);

    return index;
}

//------------------------------------------------------------------------------
/**
    SetCombatIdle
*/
int
ncLogicAnimator::SetCombatIdle()
{
    int index = -1;
    index = this->SetAnimationAction (GP_ACTION_COMBATIDLE, false, false, true);

    return index;
}

//------------------------------------------------------------------------------
/**
    SetDrivingIdle
*/
int
ncLogicAnimator::SetDrivingIdle()
{
    int index = -1;
    index = this->SetAnimationAction (GP_ACTION_DRIVINGIDLE, false, true, true);

    return index;
}

//------------------------------------------------------------------------------
/**
    SetDie
*/
int
ncLogicAnimator::SetDie()
{
    return this->SetAnimationAction (GP_ACTION_DIE, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetDrive
*/
int
ncLogicAnimator::SetDrive()
{
    return this->SetAnimationAction (GP_ACTION_DRIVE, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetSwim
*/
int
ncLogicAnimator::SetSwim()
{
    return this->SetAnimationAction (GP_ACTION_SWIM, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetDive
*/
int
ncLogicAnimator::SetDive()
{
    return this->SetAnimationAction (GP_ACTION_DIVE, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetShoot
*/
int
ncLogicAnimator::SetShoot()
{
    int index = -1;

    this->prevAction = this->action;
    index = this->SetAnimationAction (GP_ACTION_SHOOT, false, false, false);
    if( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetShootAuto
*/
void
ncLogicAnimator::SetShootAuto()
{
    int index = -1;

    this->prevAction = this->action;
    index = this->SetAnimationAction (GP_ACTION_SHOOTAUTO, false, false, false);
    if( index > -1 )
    {
        this->upperBlocked = true;
    }
}

//------------------------------------------------------------------------------
/**
    SetMeleeAttack
*/
int
ncLogicAnimator::SetMeleeAttack()
{
    int index = -1;

    this->prevAction = this->action;
    index = this->SetAnimationAction (GP_ACTION_MELEEATTACK, false, false, true);
    if( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetMeleeBlock
*/
int
ncLogicAnimator::SetMeleeBlock()
{
    int index = -1;

    index = this->SetAnimationAction (GP_ACTION_MELEEBLOCK, false, false, true);
    this->upperBlocked = true;
    
    return index;
}

//------------------------------------------------------------------------------
/**
    SetImpact
*/
int
ncLogicAnimator::SetImpact()
{
    return this->SetAnimationAction (GP_ACTION_IMPACT, false, false, false);
}

//------------------------------------------------------------------------------
/**
    SetMiniImpact
*/
int
ncLogicAnimator::SetMiniImpact()
{
    return this->SetAnimationAction (GP_ACTION_MINIIMPACT, false, false, false);
}

//------------------------------------------------------------------------------
/**
    SetLeftClawAttack
*/
int
ncLogicAnimator::SetLeftClawAttack()
{
    return this->SetAnimationAction (GP_ACTION_LEFTCLAW, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetRightClawAttack
*/
int
ncLogicAnimator::SetRightClawAttack()
{
    return this->SetAnimationAction (GP_ACTION_RIGHTCLAW, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetBothClawsAttack
*/
int
ncLogicAnimator::SetBothClawsAttack()
{
    return this->SetAnimationAction (GP_ACTION_BOTHCLAWS, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetFly
*/
int
ncLogicAnimator::SetFly()
{
    return this->SetAnimationAction (GP_ACTION_FLY, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetJumpStart
*/
int
ncLogicAnimator::SetJumpStart()
{
    return this->SetAnimationAction (GP_ACTION_JUMPSTART, false, false, true);
}

//------------------------------------------------------------------------------
/**
SetBattleCry
*/
int
ncLogicAnimator::SetBattleCry()
{
    return this->SetAnimationAction (GP_ACTION_BATTLECRY, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetJumpObstacle
*/
int
ncLogicAnimator::SetJumpObstacle()
{
    return this->SetAnimationAction (GP_ACTION_JUMPOBSTACLE, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetJumpAttack
*/
int
ncLogicAnimator::SetJumpAttack()
{
    return this->SetAnimationAction (GP_ACTION_JUMPATTACK, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetJumpEndSuc
*/
int
ncLogicAnimator::SetJumpEndSuc()
{
    return this->SetAnimationAction (GP_ACTION_JUMPENDSUC, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetJumpEndFail
*/
int
ncLogicAnimator::SetJumpEndFail()
{
    return this->SetAnimationAction (GP_ACTION_JUMPENDFAIL, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetJumpMid
*/
int
ncLogicAnimator::SetJumpMid(bool loop = true)
{
    return this->SetAnimationAction (GP_ACTION_JUMPMID, false, loop, true);
}

//------------------------------------------------------------------------------
/**
    SetJumpEnd
*/
int
ncLogicAnimator::SetJumpEnd()
{
    return this->SetAnimationAction (GP_ACTION_JUMPEND, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetMop
*/
int
ncLogicAnimator::SetMop()
{
    return this->SetAnimationAction (GP_ACTION_MOP, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetLeap
*/
int
ncLogicAnimator::SetLeap()
{
    return this->SetAnimationAction (GP_ACTION_LEAP, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetTaunt
*/
int
ncLogicAnimator::SetTaunt()
{
    return this->SetAnimationAction (GP_ACTION_TAUNT, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetBurrow
*/
int
ncLogicAnimator::SetBurrow()
{
    return this->SetAnimationAction (GP_ACTION_BURROW, false, false, true);
}

//------------------------------------------------------------------------------
/**
    SetRoll
*/
int
ncLogicAnimator::SetRoll()
{
    return this->SetAnimationAction (GP_ACTION_ROLL, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetClimb
*/
int 
ncLogicAnimator::SetClimb()
{
    return this->SetAnimationAction (GP_ACTION_CLIMB, false, true, true);
}

//------------------------------------------------------------------------------
/**
    SetFastReload
*/
int
ncLogicAnimator::SetFastFirstReload()
{    
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_FASTFIRSTRELOAD, false, false, false);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetFastReload
*/
int
ncLogicAnimator::SetFastSecondReload()
{    
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_FASTSECONDRELOAD, false, false, false);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetFullReload
*/
int
ncLogicAnimator::SetFullReload()
{    
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_FULLRELOAD, false, false, false);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetFullFirstReload
*/
int
ncLogicAnimator::SetFullFirstReload()
{    
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_FULLFIRSTRELOAD, false, false, false);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetFullSecondReload
*/
int
ncLogicAnimator::SetFullSecondReload()
{    
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_FULLSECONDRELOAD, false, false, false);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetEndReload
*/
int
ncLogicAnimator::SetEndReload()
{
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_ENDRELOAD, false, false, false);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetCockReload
*/
int
ncLogicAnimator::SetCockReload()
{
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_COCKRELOAD, false, false, false);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
	SetHideWeapon
*/
int
ncLogicAnimator::SetHideWeapon()
{
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_HIDEWEAPON, false, false, true);
    if ( index > -1 )
    {
        this->blockedAction = GP_ACTION_IDLE;
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
	SetShowWeapon
*/
int
ncLogicAnimator::SetShowWeapon()
{
    int index = -1;

    this->upperBlocked = false;
	index = this->SetAnimationAction (GP_ACTION_SHOWWEAPON, false, false, true);
    if ( index > -1 )
    {
        this->blockedAction = GP_ACTION_NONE;
        this->upperBlocked = true;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
*/
int
ncLogicAnimator::SetWeaponAcc()
{
    int index = -1;

    this->upperBlocked = false;
    index = this->SetAnimationAction (GP_ACTION_WEAPONACC, false, true, true);
    if ( index > -1 )
    {
        this->upperBlocked = true;
    }

	return index;
}

//------------------------------------------------------------------------------
/**
    SetAnimation

    @params action          - action to play
            backward        - true if animation must be played backwards
            looped          - true if animation must be played looped, false else
            interpolation   - true if animation must be played interpolated to previous animation

    @brief Plays an animation given by the user updating the internal state.
*/
int
ncLogicAnimator::SetAnimationAction (int action, bool backward, bool looped, bool interpolation)
{
    ngpAction newAction = (ngpAction)action;
    int index = -1;

    if( action != this->blockedAction )
    {
        if ( this->action != newAction || this->direction != this->newDirection )
        {
            NLOG( animation, (NLOG3 | 1, "%s -> ncLogicAnimator New Animation '%s' %d %d %d", 
                this->GetEntityClass()->GetName(), nGameplayEnums::GetActionString( newAction ), 
                backward, looped, interpolation ) );
            this->action = newAction;
		    this->direction = this->newDirection;
            index = this->PlayAnimation (newAction, backward, looped, interpolation);
        }
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetIronsight
    
    @params forward - true for put ironsight state, false to cover prevous state
        
    @brief (un)Set ironsight state, playing the proper animation.
*/
int
ncLogicAnimator::SetIronsight (bool forward)
{
    int index = -1;

    if ( forward )
    {
        if ( this->state == LA_STATE_STAND )
        {
            index = this->SetAnimationAction (GP_ACTION_IRONSIGHT, false, false, false);
            this->ChangeState ( LA_STATE_STANDIRONSIGHT, this->state );
        }
        else if ( this->state == LA_STATE_CROUCH )
        {
            index = this->SetAnimationAction (GP_ACTION_IRONSIGHT, false, false, false);
            this->ChangeState ( LA_STATE_CROUCHIRONSIGHT, this->state );
        }
        else if ( this->state == LA_STATE_PRONE )
        {        
            index = this->SetAnimationAction (GP_ACTION_IRONSIGHT, false, false, false);
            this->ChangeState ( LA_STATE_PRONEIRONSIGHT, this->state );
        }
    }
    else // backward
    {
        if ( this->state == LA_STATE_STANDIRONSIGHT )
        {
            this->action = GP_ACTION_NONE;
            index = this->SetAnimationAction (GP_ACTION_IRONSIGHT, true, false, false);
            this->ChangeState ( LA_STATE_STAND, this->prevState );
        }
        else if ( this->state == LA_STATE_CROUCHIRONSIGHT )
        {
            this->action = GP_ACTION_NONE;
            index = this->SetAnimationAction (GP_ACTION_IRONSIGHT, true, false, false);
            this->ChangeState ( LA_STATE_CROUCH, this->prevState );
        }
        else if ( this->state == LA_STATE_PRONEIRONSIGHT ) 
        {
            this->action = GP_ACTION_NONE;
            index = this->SetAnimationAction (GP_ACTION_IRONSIGHT, true, false, false);
            this->ChangeState ( LA_STATE_PRONE, this->state );
        }
    }

    // Block idle for avoid animation overwrite in gameplayplayer
    // @TODO: Open the wc, take this file, throw it inside wc, press water buton, suspire...
    this->blockedAction = GP_ACTION_IDLE;

    this->upperBlocked = true;

    return index;
}

//------------------------------------------------------------------------------
/**
SetLeanIronsight

    @params forward - true for put leanironsight state, false to cover previous state

    @brief (un)Set leanronsight state, playing the proper animation.
*/
int
ncLogicAnimator::SetLeanIronsight (bool forward)
{
    bool backward = !forward;
    int index = -1;

    if ( this->state == LA_STATE_LEANIRONSIGHT && backward )
    {        
        //@TODO: CODE HORROR!!! Spectacular ugly, silly trick for make works the lean ironsight
        this->ChangeState ( this->prevState, LA_STATE_LEANIRONSIGHT );        
        this->action = GP_ACTION_NONE;
        this->firstPersonIndex = -1;        
        index = this->SetAnimationAction( GP_ACTION_IRONSIGHT, true, false, false );                
        this->newDirection = LA_STOPPED;
    }
    else
    if ( this->state != LA_STATE_LEANIRONSIGHT && forward )
    {
        this->ChangeState( LA_STATE_LEANIRONSIGHT, this->state );
        index = this->SetAnimationAction( GP_ACTION_LEANIRONSIGHT, false, false, false );        
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    SetCrouch

    @params forward - true for put crouch state, false to cover previous state

    @brief (un)Set crouch state, playing the proper animation.
*/
int
ncLogicAnimator::SetCrouch (bool forward)
{
    bool backward = !forward;
    int index = -1;
    
    if ( this->state == LA_STATE_CROUCH && backward )
    {
        if ( this->prevState == LA_STATE_PRONE )
        {
            this->prevState = LA_STATE_STAND;
        }
        // Ensure that the action is executed
        this->action = GP_ACTION_NONE;       
        this->state = LA_STATE_STAND;
        index = this->SetAnimationAction (GP_ACTION_IDLE, false, false, true);    
        this->ChangeState (this->LA_STATE_STAND, LA_STATE_CROUCH);
    }
    else
    if ( this->state == LA_STATE_CROUCHIRONSIGHT && backward )
    {
        // Ensure that the action is executed
        this->action = GP_ACTION_NONE;    
        this->state = LA_STATE_STANDIRONSIGHT;
        index = this->SetAnimationAction (GP_ACTION_IDLE, false, false, true);            
        this->ChangeState (LA_STATE_STANDIRONSIGHT, LA_STATE_CROUCHIRONSIGHT);
    }
    else
    if ( this->state != LA_STATE_CROUCH && forward )
    {               
        if ( this->state == LA_STATE_PRONE )
        {            
            index = this->SetAnimationAction (GP_ACTION_PRONE, true, false, false);            
            this->ChangeState (LA_STATE_CROUCH, this->state);            
        }
        else
        if ( this->state == LA_STATE_PRONEIRONSIGHT )
        {
            index = this->SetAnimationAction (GP_ACTION_PRONE, true, false, false);
            this->ChangeState ( LA_STATE_CROUCHIRONSIGHT, this->state );
        }
        else
        if ( this->state == LA_STATE_STAND )
        {
            this->ChangeState (LA_STATE_CROUCH, this->state);
            this->action = GP_ACTION_NONE;
            index = this->SetAnimationAction (GP_ACTION_IDLE, false, false, true);
        }
        else
        if ( this->state == LA_STATE_STANDIRONSIGHT )
        {
            this->ChangeState (LA_STATE_CROUCHIRONSIGHT, this->state );
            this->action = GP_ACTION_NONE;
            index = this->SetAnimationAction (GP_ACTION_IDLE, false, false, true);
        }
        else            
        {
            this->SetIdle();            
        }                
    }    

    return index;
}

//------------------------------------------------------------------------------
/**
    SetProne

    @params forward - true for put prone state, false to cover prevous state

    @brief (un)Set prone state, playing the proper animation.
*/
int
ncLogicAnimator::SetProne (bool forward)
{
    bool backward = !forward;
    int index = -1;

    if ( this->state == LA_STATE_PRONE && backward )
    {
        index = this->SetAnimationAction (GP_ACTION_PRONE, true, false, false);
        this->ChangeState (LA_STATE_CROUCH, LA_STATE_PRONE);
    }
    else
    if ( this->state == LA_STATE_PRONEIRONSIGHT && backward )
    {
        index = this->SetAnimationAction (GP_ACTION_PRONE, true, false, false);
        this->ChangeState (LA_STATE_CROUCHIRONSIGHT, LA_STATE_PRONEIRONSIGHT);
    }
    else
    if ( this->state != LA_STATE_PRONE && forward )
    {
        if ( this->state == LA_STATE_CROUCH )
        {            
            index = this->SetAnimationAction (GP_ACTION_PRONE, false, false, false);            
            this->ChangeState (LA_STATE_PRONE, this->state);            
        }
        else
        if ( this->state == LA_STATE_CROUCHIRONSIGHT )
        {
            index = this->SetAnimationAction (GP_ACTION_PRONE, false, false, false);
            this->ChangeState ( LA_STATE_PRONEIRONSIGHT, this->state );
        }        
        else            
        {
            this->SetIdle();            
        }       
    }

    this->lowerBlocked = true;
    this->upperBlocked = true;

    return index;
}

//------------------------------------------------------------------------------
/**
    SetSeat

    @params forward - true for put seat state, false to cover pervious state

    @brief (un)Set seat state, playing the proper animation.
*/
int
ncLogicAnimator::SetSeat (bool forward)
{
    bool backward = !forward;
    int index = -1;

    if ( this->state == LA_STATE_SEAT && backward )
    {
        index = this->SetAnimationAction (GP_ACTION_SEAT, true, false, false);
        this->ChangeState (this->prevState, LA_STATE_PRONE);
    }
    else
    if ( this->state != LA_STATE_SEAT && forward )
    {
        index = this->SetAnimationAction (GP_ACTION_SEAT, false, false, false);
        this->ChangeState (LA_STATE_SEAT, this->state);
    }

    return index;
}

//------------------------------------------------------------------------------
/**
	SetLean

	@params forward - true for put lean state, false to cover pervious state

	@brief (un)Set left lean state, playing the proper animation.
*/
int
ncLogicAnimator::SetLean (bool forward)
{
	bool backward = !forward;
    int index = -1;
    int unblock = false;

    if ( this->state != LA_STATE_CROUCHIRONSIGHT && this->state != LA_STATE_STANDIRONSIGHT )
    {
        if ( this->state == LA_STATE_LEAN && backward )
        {            
            unblock = true;
            index = this->SetAnimationAction (GP_ACTION_LEAN, true, false, false);
            this->ChangeState (this->prevState, LA_STATE_LEAN);
        }
        else
        if ( this->state != LA_STATE_LEAN && forward )
        {
            index = this->SetAnimationAction (GP_ACTION_LEAN, false, false, false);		
            this->ChangeState (LA_STATE_LEAN, this->state);
        }
    }

    if ( index > -1 )
    {
        if ( unblock )
        {
            this->upperBlocked = false;
        }
        else
        {
            this->upperBlocked = true;
        }
    }	

    return index;
}

//------------------------------------------------------------------------------
/**
    ChangeState

    @params state       - the state to change
            prevState   - previous state

    @brief  Set an transition between the previous and the new state.
*/
void
ncLogicAnimator::ChangeState (eAnimationState state, eAnimationState prevState)
{
    NLOG( animation, (NLOG3|1, "%s -> ncLogicAnimator::ChangeState %d, %d",
        this->GetEntityClass()->GetName(), state, prevState ) );
    this->SetAnimatorState (state);
    this->SetPrevState (prevState);
    this->changeState = true;
}

//------------------------------------------------------------------------------
/**
    PlayAnimation

    @params animation - name of the animation, given by a string
    backward          - true if animation must be played backwards
    looped            - true if animation must be played looped, false else
    interpolation     - true if animation must be played interpolated to previous animation

    @brief Plays an animation given by param.
*/
int
ncLogicAnimator::PlayAnimation (const char* animation, bool backward, bool looped, bool interpolation)
{   
    eAnimationState old = this->state;
    bool fperson = false;
    int index = -1;
    this->descriptor = -1; // no descriptor by default

    if ( backward )
    {
        this->state = this->prevState;
    }
    
    if( this->HasFirstPersonAnimation() && !this->upperBlocked )
    {   
        fperson = true;        
    
        // set first person animation
        index = this->GetIndexRandomState (animation, true);        

        if ( index > -1 && index != this->firstPersonIndex )
        {        
            this->firstPersonIndex = index;
            this->SetFirstPersonActiveStateByIndex (index, backward, looped, interpolation);

            this->savedFirstAnim.index = index;
            this->savedFirstAnim.backward = backward;
            this->savedFirstAnim.looped = looped;
            this->savedFirstAnim.interpolation = interpolation;
        }    

        if ( index == -1 )
        {
            NLOGCOND( animation, index == -1, 
                ( NLOG3|0, "ncLogicAnimator::PlayAnimation, Not Found '%s' for %s", animation, 
                fperson ? "first person" : "third person" ) );

            fperson = false;
            this->firstPersonIndex = -1;
        }
    }    

    // set third person animation

    if ( !fperson )
    {
        index = this->GetIndexRandomState (animation, false);

        if ( index > -1 )
        {            
            this->SetThirdPersonState (index, backward, looped, interpolation);
        }

        if ( index == -1 )
        {
            // as haven't first person animation don't have sense continue
            this->upperThirdPersonIndex = -1;
            this->lowerThirdPersonIndex = -1;
        }
    }
    else
    {
        // Get index to modify
        index = this->GetThirdAnimationReflex( index );
        if ( index > -1 )
        {
            this->SetThirdPersonState ( index, backward, looped, interpolation );                
        }
        else
        {
            if ( !this->changeState )
            {
                this->upperThirdPersonIndex = -1;
                this->lowerThirdPersonIndex = -1;
                NLOG( animation, (NLOG3|0, "%s -> Reflex for 1er person animation '%s' not found!! Maybe is a TRANSITION", 
                    this->GetEntityClass()->GetName(), animation ) );
            }            
        }
    }

    this->state = old;

    return index;
}

//------------------------------------------------------------------------------
/**
    HasFinished

    @params action - Action to request if ended

    @return True if the requested action is finished
*/
bool
ncLogicAnimator::HasFinished( int index ) const
{
    bool finished = true;   
    NLOGCOND( animation, index == -1, 
        ( NLOG3|0, "ncLogicAnimator::HasFinished, Trying to test the finish of a not existent index: '%d'", index ) );
    bool fperson = ( index > -1 ? false : true );
    if ( index > -1 )
    {
        eAnimationBody trainUsed = this->GetUsedTrains( index );
        // Index must be the active one, is not is the active is because other
        // animation with have overwrite it, then return finished
        switch ( trainUsed )
        {
        case LA_BODY_LOWER:
            if ( index == this->lowerThirdPersonIndex )
            {
			    finished = this->CheckEnd( this->lowerThirdPersonIndex, fperson );
            }
	        break;
        case LA_BODY_UPPER:
            if ( index == this->upperThirdPersonIndex )
            {
                finished = this->CheckEnd( this->upperThirdPersonIndex, fperson );
            }
	        break;
        case LA_BODY_FULL:
            if ( this->upperThirdPersonIndex ==  this->lowerThirdPersonIndex )
            {
                if ( index == this->upperThirdPersonIndex )
                {
                    finished = this->CheckEnd( this->upperThirdPersonIndex, fperson );
                }
            }
            else
            {
                if ( index == this->upperThirdPersonIndex || index == this->lowerThirdPersonIndex )
                {
                    finished = this->CheckEnd( this->upperThirdPersonIndex, fperson ) && 
                        this->CheckEnd( this->lowerThirdPersonIndex, fperson );
                }
            }                
            break;
        default:
            NLOG( animation, (NLOG3|0, "%s -> Animation use invalid train: '%d'",
                this->GetEntityClass()->GetName(), trainUsed ) );
        }
    }
    else
    {
        finished = this->CheckEnd( this->firstPersonIndex, fperson );
    }
    
    return finished;
}

//------------------------------------------------------------------------------
/**
    HasFinishedAllAnims

    @return True if all the requested actions have finished or looping
*/
bool
ncLogicAnimator::HasFinishedAllAnims()
{
    bool finished = true;
    int index;

    index = this->firstPersonIndex;
    if( this->HasFirstPersonAnimation() && index > -1 )
    {
        finished = this->IsLoopedByIndex(index) ||
                   (this->GetRemainingTime( index, true ) <= 0);
        if( !finished )
        {
            return false;
        }        
    }

    index = this->lowerThirdPersonIndex;
    if( index > -1 )
    {
        finished = this->IsLoopedByIndex(index) ||
                   (this->GetRemainingTime( index, false ) <= 0);
        if( !finished )
        {
            return false;
        }
    }

    index = this->upperThirdPersonIndex;
    if( index > -1 )
    {
        finished = this->IsLoopedByIndex(index) ||
                   (this->GetRemainingTime( index, false ) <= 0);
    }

    return finished;
}

//------------------------------------------------------------------------------
/**
    GetUsedTrains
    
    @params action - Action to check

    @return return the body trains used for the action
*/
ncLogicAnimator::eAnimationBody
ncLogicAnimator::GetUsedTrains ( int index ) const
{   
    eAnimationBody bodyAnim;
    eAnimationBody inverse;
    nString stateName = this->GetStateName( index );
    bodyAnim = this->GetAnimationBody ( stateName.Get() );
    
    if ( bodyAnim == LA_BODY_FULL )
    {
        return LA_BODY_FULL;
    }

    if ( bodyAnim == LA_BODY_LOWER )
    {
        inverse = LA_BODY_UPPER;
    }
    else
    {
        inverse = LA_BODY_LOWER;
    }

    if ( this->GetInverseTrain( index, inverse ) > -1 )
    {
        return LA_BODY_FULL;
    }
    else
    {
        return bodyAnim;
    }
}

//------------------------------------------------------------------------------
/**
    SetThirdPersonState

    @params index          - index of the animation
            backward       - true if animation must be played backwards
            looped         - true if animation must be played looped, false else
            interpolation  - true if animation must be played interpolated to previous animation

    @brief Ser the third person animation state, cheking trains.
*/
void
ncLogicAnimator::SetThirdPersonState ( int index, bool backward, bool looped, bool interpolation )
{
    nString animName = this->GetStateName(index);
    eAnimationBody animBody = this->GetAnimationBody( animName.Get() );

    if ( animBody == LA_BODY_FULL )
    {
        if ( !this->upperBlocked && !this->lowerBlocked && 
            this->upperThirdPersonIndex != index && this->lowerThirdPersonIndex != index )
        {
            this->SetActiveStateByIndex( index, backward, looped, interpolation );
            // if is full body use two trains        
            this->upperThirdPersonIndex = index;
            this->lowerThirdPersonIndex = index;

            this->savedLowerAnim.index = index;
            this->savedLowerAnim.backward = backward;
            this->savedLowerAnim.looped = looped;
            this->savedLowerAnim.interpolation = interpolation;
            this->savedUpperAnim.index = -1;
        }
        
    }
    else if ( animBody == LA_BODY_UPPER )
    {
        // if is upper train, see if have an associated lower train
        int indexInverseTrain = this->GetInverseTrain ( index, LA_BODY_LOWER );

        if (  indexInverseTrain > -1 && indexInverseTrain != this->lowerThirdPersonIndex ) 
        {
            if ( !this->lowerBlocked )
            {
                this->SetActiveStateByIndex( indexInverseTrain, backward, looped, interpolation );                           
                this->lowerThirdPersonIndex = indexInverseTrain;

                this->savedLowerAnim.index = indexInverseTrain;
                this->savedLowerAnim.backward = backward;
                this->savedLowerAnim.looped = looped;
                this->savedLowerAnim.interpolation = interpolation;
            }
        }

        if ( !this->upperBlocked && this->upperThirdPersonIndex != index )
        {
            this->SetActiveStateByIndex( index, backward, looped, interpolation );
            this->upperThirdPersonIndex = index;        

            this->savedUpperAnim.index = index;
            this->savedUpperAnim.backward = backward;
            this->savedUpperAnim.looped = looped;
            this->savedUpperAnim.interpolation = interpolation;
        }       
    }
    else if ( animBody == LA_BODY_LOWER )
    {
        // if is lower train, see if have an associated upper train
        int indexInverseTrain = this->GetInverseTrain ( index, LA_BODY_UPPER );

        if ( indexInverseTrain > -1 && indexInverseTrain != this->upperThirdPersonIndex ) 
        {
            if ( !this->upperBlocked )
            {
                this->SetActiveStateByIndex( indexInverseTrain, backward, looped, interpolation );
                this->upperThirdPersonIndex = indexInverseTrain;

                this->savedLowerAnim.index = indexInverseTrain;
                this->savedLowerAnim.backward = backward;
                this->savedLowerAnim.looped = looped;
                this->savedLowerAnim.interpolation = interpolation;
            }
        }

        if ( !this->lowerBlocked && index != this->lowerThirdPersonIndex )
        {
            this->SetActiveStateByIndex( index, backward, looped, interpolation );
            this->lowerThirdPersonIndex = index;

            this->savedUpperAnim.index = index;
            this->savedUpperAnim.backward = backward;
            this->savedUpperAnim.looped = looped;
            this->savedUpperAnim.interpolation = interpolation;
        }        
    }
	else
	{
		NLOG( animation,(NLOG3|0, "%s -> Non contempled body train returned by the GetAnimationBody method",
            this->GetEntityClass()->GetName() ) );
		this->lowerThirdPersonIndex = -1;
		this->lowerThirdPersonIndex = -1;
	}
}

//------------------------------------------------------------------------------
/**
    GetInverseTrain

    @params index - Index of the animation
            train - Train to check
    
    @return Index of the inverse train animation or -1 if not found

*/
int
ncLogicAnimator::GetInverseTrain ( int index, int train ) const
{
    nString stateName = this->GetStateName( index );
    nString stateName2(stateName);
    char& searchName = stateName2[LA_CODE_BODY];    

    // Change the body character to corresponded
    if ( train == LA_BODY_UPPER )
    {        
        searchName = (char)tolower( this->GetCharBody(LA_BODY_UPPER) );
    }
    else
    {
        searchName = (char)tolower( this->GetCharBody(LA_BODY_LOWER) );
    }

    int number = this->GetNumberStates();

    // search the reverse state in the state list
    for ( int i=0; i<number; i++ )
    {
        const nString stringName = this->GetStateName(i);
        const char* thirdPersonName = stringName.Get();

        if ( !strcmp( stateName2.Get(), thirdPersonName ) )
        {
            return i;
        }            
    }

    return -1;
}

//------------------------------------------------------------------------------
/**
    PlayAnimation

    @params action - action to play
    backward       - true if animation must be played backwards
    looped         - true if animation must be played looped, false else
    interpolation  - true if animation must be played interpolated to previous animation

    @brief Plays an animation given by the user updating the internal state.
*/
int
ncLogicAnimator::PlayAnimation (ngpAction action, bool backward, bool looped, bool interpolation)
{
    const char* stateName = nGameplayEnums::GetActionString( action );
    n_assert(stateName);
    int index = -1;

    if ( stateName )
    {
        index = this->PlayAnimation (stateName, backward, looped, interpolation);
    }

    return index;
}

//------------------------------------------------------------------------------
/**
   IsAnimationType

   @param animationClass - the class of the animation to be evaluated
          animation      - the animation checked

   @return true if the animation belong to the animationClass, false else
*/
bool
ncLogicAnimator::IsAnimationType (const char* animationClass, const char* animation) const
{
    n_assert(animationClass&&animation);

    bool isAnimation = false;

    if ( animationClass && animation )
    {
        eAnimationState state = this->GetAnimationState (animation);
        int direction = this->GetAnimationDirection ( animation );
        if ( this->IsSpecialState() && ( strcmp(animationClass, "jumpstart") == 0 || 
             strcmp(animationClass, "jumpmid") == 0 || strcmp(animationClass, "jumpend") == 0 ) )
        {
            state = this->state;
        }
        if ( state == this->state && direction == this->direction )
        {
            const char* animName = strrchr( animation, int('_') );           
            nString animName2 (animName);
            nString animNum ( animName2.ExtractRange(animName2.Length() - 2, 2 ) );
            int number = animNum.AsInt();
            if ( number > 0 || animNum == "00" )
            {                
                animName2 = animName2.ExtractRange( 1, animName2.Length() - 3 );
                isAnimation = bool ( strcmp (animationClass, animName2.Get()) == 0 );
            }
            else
            {
                animName2 = animName2.ExtractRange( 1, animName2.Length() - 1 );
                isAnimation = bool ( strcmp ( animationClass, animName2.Get() ) == 0 );
            }
        }                    
    }

    return isAnimation;
}

//------------------------------------------------------------------------------
/**
    GetNumberOfAnimationClass

    @param animationClass - the class of the animation
    @param firstPerson - if the index is for the first person animations

    @return the number of the animations within the class for the current character.
*/
int
ncLogicAnimator::GetNumberOfAnimationClass (const char* animationClass, bool firstPerson) const
{
    int total = 0;
    int number = 0;
    if( firstPerson )
    {
        number = this->GetFirstPersonNumberStates();
    }
    else
    {
        number = this->GetNumberStates();
    }

    for ( int i=0; i<number; i++ )
    {
        const nString stringName = firstPerson ? this->GetFirstPersonStateName(i) : this->GetStateName(i);

        const char* stateName = stringName.Get();
        if ( this->IsAnimationType (animationClass, stateName) )
        {
            total++;
        }
    }

    return total;
}

int 
ncLogicAnimator::GetThirdAnimationReflex ( int fidx )
{
    int number = 0;
    const char* firstPersonName = 0;

    nString stateName ( this->GetFirstPersonStateName( fidx ) );
    firstPersonName = stateName.Get();

    number = this->GetNumberStates();

    for ( int i=0; i<number; i++ )
    {
        const nString stringName = this->GetStateName(i);

        const char* thirdPersonName = stringName.Get();
        eAnimationState state = this->GetAnimationState (thirdPersonName);
        int direction = this->GetAnimationDirection ( thirdPersonName );

        if ( state == this->state && direction == this->direction ) 
        {            
            if ( this->EqualAnimationName( firstPersonName, thirdPersonName ) )
            {
                return i;
            }
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
/** 
    EqualAnimationName

    @params firstAnim - Name of first person animation
            thirdAnim - Name of third person animation

    @return True if the animation name of the arguments strings are identical
*/
bool
ncLogicAnimator::EqualAnimationName ( const char* firstAnim, const char* thirdAnim )
{
    bool isSameAnimation = false;

    // get the "_animname" string
    firstAnim = strrchr( firstAnim, int('_') );
    thirdAnim = strrchr( thirdAnim, int('_') );

    if ( !strcmp( firstAnim, thirdAnim ) )
    {
        isSameAnimation = true;
    }

    return isSameAnimation;
}


//------------------------------------------------------------------------------
/**
    GetStateOrder

    @params animationClass - the class of the animation to look for
            order          - the action desired
            firstPerson    - if the index is for the first person animations

    @return the state index of the proper animation according the state and the class
*/
int
ncLogicAnimator::GetStateOrder (const char* animationClass, int order, bool firstPerson) const
{
    n_assert(animationClass);

    int index = 0;
    int number = 0;

    if( firstPerson )
    {
        number = this->GetFirstPersonNumberStates();
    }
    else
    {
        number = this->GetNumberStates();
    }

    int i;
    for ( i=0; i<number && index < order; i++ )
    {
        const nString stringName = firstPerson ? this->GetFirstPersonStateName(i) : this->GetStateName(i);

        const char* stateName = stringName.Get();

        if ( this->IsAnimationType (animationClass, stateName) )
        {
            index++;
        }
    }

    return (i - 1);
}

//------------------------------------------------------------------------------
/**
    GetIndexRandomState

    @param animationClass - the class where the animation must be founded
    @param firstPerson - if the index is for the first person animations

    @return a random index of the current state and class animation
*/
int
ncLogicAnimator::GetIndexRandomState (const char* animationClass, bool firstPerson )
{
    int total = this->GetNumberOfAnimationClass (animationClass, firstPerson );

    NLOGCOND( animation, total == 0, 
        ( NLOG3|0, "ncLogicAnimator, Not animation found for '%s' in %s", animationClass, 
        firstPerson ? "first person" : "third person" ) );

    int index = -1; 
    int order;

    if ( total > 1 )
    {
        // the first animation will be played the RANDOM_LIMT % of the time
        // the rest of the time % will be linear distributed between the others
        // animations
        const float RANDOM_MAX = 100.0f;
        const float RANDOM_LIMIT = 70.0f;
        const float RANDOM_REST  = RANDOM_MAX - RANDOM_LIMIT;

        float random = n_rand_real( RANDOM_MAX );
        if( random < RANDOM_LIMIT )
        {
            order = 1;
        }
        else
        {
            random = ( random - RANDOM_LIMIT );
            random = random / ( RANDOM_REST / ( total - 1 ) );

            order = int( n_floor( random ) ) + 2;
        }

        bool valid = ( order >= 1 ) && ( order <= total );
        n_assert( valid );
        if( ! valid )
        {
            order = 1;
        }
    }
    else
    {
        order = 1;
    }

    if ( total > 0 )
    {        
        // Now, we must get the right state index
        index = this->GetStateOrder (animationClass, order, firstPerson );
    }
    this->descriptor = order - 1;
    
    return index;
}

//------------------------------------------------------------------------------
/**
    CheckEnd

    @params stateIndex - Animation index to check
            fperson    - If true check the first person index else in third person

    @return true if the requested animation is finished
*/
bool
ncLogicAnimator::CheckEnd( int index, bool fperson ) const
{
    bool finished = true;
    if ( index > -1 )
    {
        finished = this->GetRemainingTime ( index, fperson ) <= 0 ||
            this->LoopJustStarted( index, fperson);         
    }

    return finished;
}

//------------------------------------------------------------------------------
/**
    UpdateCharacter

    @param the char index to be updated

    @brief updates the current state of the character
*/
bool
ncLogicAnimator::UpdateCharacter (int charIndex, nTime curTime)
{
    bool update = ncCharacter::UpdateCharacter(charIndex, curTime);

    bool looped = this->action == GP_ACTION_DIE ? false : true;

    if ( this->lowerThirdPersonIndex > -1 || this->upperThirdPersonIndex > -1 || this->firstPersonIndex > -1)
    {
        bool finishedLower = true;
        bool finishedUpper = true;
        bool finishedFperson = true;

        if ( this->HasFirstPersonAnimation() && this->firstPersonIndex > -1 )
        {
            finishedFperson = this->CheckEnd( this->firstPersonIndex, true );
            if ( finishedFperson )
            {
                this->upperBlocked = false;
                this->lowerBlocked = false;

                if ( this->changeState )
                {
                    // force the idle animation
                    this->action = GP_ACTION_NONE;
                    this->blockedAction = GP_ACTION_NONE;
                    this->upperBlocked = false;
                    this->lowerBlocked = false;
                    //this->SetIdle();
                    if ( this->blockedAction == GP_ACTION_IDLE )
                    {
                        this->blockedAction = GP_ACTION_NONE;
                        this->action = GP_ACTION_IDLE;
                    }
                    this->changeState = false;                    
                }
                else
                {
                    this->PlayAnimation ( this->action, false, looped, true);
                }
                return update;
            }
        }
        else
        {
            finishedLower = this->CheckEnd( this->lowerThirdPersonIndex, false );
            if ( finishedLower )
            {
                if ( !this->changeState )
                {
                    this->lowerBlocked = false;
                    this->PlayAnimation ( this->action, false, looped, true);
                }                
            }
            finishedUpper = this->CheckEnd( this->upperThirdPersonIndex, false );
            if ( finishedUpper )
            {
                if ( !this->changeState )
                {
                    this->upperBlocked = false;
                    this->PlayAnimation ( this->action, false, looped, true);
                }                
            }

            if ( finishedLower || finishedUpper )
            {
                if ( this->changeState )
                {
                    this->blockedAction = GP_ACTION_NONE;
                    this->action = GP_ACTION_NONE;
                    this->upperBlocked = false;
                    this->lowerBlocked = false;
                    //this->SetIdle();
                    this->changeState = false;
                }                
            }
        }
    }
    else
    {
        // force the idle animation
        this->action = GP_ACTION_NONE;
        this->blockedAction = GP_ACTION_NONE;
        this->upperBlocked = false;
        this->lowerBlocked = false;
        //this->SetIdle();
    }

    return update;
}

//------------------------------------------------------------------------------
/**
    SetUpperIdle    

    @brief set the upper idle animation
*/
void
ncLogicAnimator::SetUpperIdle()
{
    int index = this->GetIndexRandomState ("idle", false);
    nString animName = this->GetStateName(index);
    eAnimationBody animBody = this->GetAnimationBody( animName.Get() );

    if ( animBody == LA_BODY_UPPER )
    {
        this->SetActiveStateByIndex( index, false, false, true );
        this->upperThirdPersonIndex = index;
    }
    else
    {
        int indexInverseTrain = this->GetInverseTrain ( index, LA_BODY_LOWER );

        if (  indexInverseTrain > -1 ) 
        {
            this->SetActiveStateByIndex( indexInverseTrain, false, false, true );  
            this->upperThirdPersonIndex = indexInverseTrain;
        }
        else
        {
            NLOG( animation, (NLOG3|0, "%s -> No animation found for 'idle' of index '%d'!!!", 
                this->GetEntityClass()->GetName(), index ) );
        }

    }
}

//------------------------------------------------------------------------------
/**
ForceAnimation

    @param action - the action forced to be played

    @brief forces to play the upper train animation
*/
void
ncLogicAnimator::ForceAnimation( ngpAction action, int train )
{
    const char* animation = nGameplayEnums::GetActionString( action );
    bool fperson = false;
    int index = -1;
    if( this->HasFirstPersonAnimation() )
    {   
        fperson = true;

        // set first person animation
        index = this->GetIndexRandomState (animation, true);

        NLOGCOND( animation, index == -1, 
            ( NLOG3|0, "ncLogicAnimator::ForceAnimation, Not Found '%s' for %s", animation, 
            fperson ? "first person" : "third person" ) );

        if ( index > -1 && index != this->firstPersonIndex )
        {        
            this->firstPersonIndex = index;
            this->SetFirstPersonActiveStateByIndex (index, false, false, true);            
        }

        if ( index == -1 )
        {
            fperson = false;
            this->firstPersonIndex = -1;
        }
    }

    // set third person animation

    if ( !fperson )
    {
        index = this->GetIndexRandomState (animation, false);

        if ( index > -1 )
        {            
            this->SetThirdPersonStateByTrain ( train, index );
        }

        if ( index == -1 )
        {
            // If no exist idle will be put in updatecharacter
            this->upperThirdPersonIndex = -1;
            this->lowerThirdPersonIndex = -1;
        }
    }
    else
    {
        index = this->GetThirdAnimationReflex( index );
        if ( index > -1 )
        {
            // Get index to modify
            n_assert2( index > -1, "Animation for third person not found" );        
            if ( index > -1 )
            {   
                this->SetThirdPersonStateByTrain ( train, index );                
            }
        }
    }
}

void
ncLogicAnimator::SetThirdPersonStateByTrain ( int train, int index )
{
    nString animName = this->GetStateName(index);
    eAnimationBody animBody = this->GetAnimationBody( animName.Get() );
    int* indexChanged = 0;
    if ( train == LA_BODY_UPPER )
    {
        indexChanged = &this->upperThirdPersonIndex;
    }
    else if ( train == LA_BODY_LOWER )
    {
        indexChanged = &this->lowerThirdPersonIndex;
    }
    else
    {
        n_assert2( train != LA_BODY_UPPER && train != LA_BODY_LOWER, "Bad train to check, please reveise tour code" );
    }

    if ( animBody == train )
    {
        this->SetActiveStateByIndex( index, false, false, true );
        *indexChanged = index;
    }
    else
    {
        int bodyTest = 0;
        // Get the inverse body to check
        if ( animBody == LA_BODY_UPPER )
        {
            bodyTest = LA_BODY_LOWER;
        }
        else if ( animBody == LA_BODY_LOWER )
        {
            bodyTest = LA_BODY_UPPER;
        }
        else
        {
            n_assert2( train != LA_BODY_UPPER && train != LA_BODY_LOWER, "Bad train to check, please reveise tour code");
        }
        // If exist the inverse play it
        int indexInverseTrain = this->GetInverseTrain ( index, bodyTest );

        if (  indexInverseTrain > -1 ) 
        {
            this->SetActiveStateByIndex( indexInverseTrain, false, false, true );                           
            *indexChanged = indexInverseTrain;
        }
        else
        {
            this->ForceAnimation( GP_ACTION_IDLE, bodyTest );
        }
    }
}

//------------------------------------------------------------------------------
/**
	GetStringState

	@brief Get the string name for an animation state, (for debug purpose only)
*/
const nString&
ncLogicAnimator::GetStringState() const
{
	static nString string;

	switch (this->state)
	{
	case LA_STATE_STAND:	        string = "Stand";		        break;
    case LA_STATE_STANDIRONSIGHT:   string = "Stand ironsight"; 	break;
    case LA_STATE_PRONE:	        string = "Prone";		        break;
    case LA_STATE_PRONEIRONSIGHT:   string = "Prone ironsight";	    break;
	case LA_STATE_CROUCH:	        string = "Crouch";		        break;
    case LA_STATE_CROUCHIRONSIGHT:	string = "Crouch ironsight";	break;	
	case LA_STATE_LEAN:	            string = "Lean";	            break;
    case LA_STATE_SWIM:	            string = "Swim";	            break;
    case LA_STATE_HANDLADDER:       string = "Hand ladder";      	break;
    case LA_STATE_SEAT:             string = "Seat";             	break;
	default:				        string = "Unknown";		        break;
	}

	return string;
}

//------------------------------------------------------------------------------
/**
GetCharDirection

@brief Get the code for an animation state
*/
char
ncLogicAnimator::GetCharDirection (eAnimationDirection direction) const
{
    char code;

    switch (direction)
    {
    case LA_FORWARD:
        code = 'W';
        break;
    case LA_RIGHTSTRAFE:
        code = 'D';
        break;
    case LA_LEFTSTRAFE:
        code = 'A';
        break;
    case LA_BACKWARD:
        code = 'S';
        break;
    case LA_LEFTLEAN:
        code = 'Q';
        break;
    case LA_RIGHTLEAN:
        code = 'E';
        break;
    default:
        code = CHAR_JOKER;
    }

    return code;
}

//------------------------------------------------------------------------------
/**
    GetCharState

    @brief Get the code for an animation state
*/
char
ncLogicAnimator::GetCharState (eAnimationState state) const
{
    char code;

    switch (state)
    {
    case LA_STATE_STAND:
        code = 'S';
        break;

    case LA_STATE_STANDIRONSIGHT:
        code = 'I';
        break;    

    case LA_STATE_CROUCH:
        code = 'C';
        break;

    case LA_STATE_CROUCHIRONSIGHT:
        code = 'X';
        break;

    case LA_STATE_PRONE:
        code = 'P';
        break;

	case LA_STATE_LEAN:
		code = 'L';
		break;

    case LA_STATE_LEANIRONSIGHT:
        code = 'T';
        break;

    case LA_STATE_HANDLADDER:
        code = 'E';
        break;

    case LA_STATE_SWIM:
        code = 'N';
        break;

    case LA_STATE_SEAT:
        code = 'D';
        break;

    default:
        code = CHAR_JOKER;
    }

    return code;
}

//------------------------------------------------------------------------------
/**
    GetAnimationState

    @brief Look at the third character of the name of the animation what is the 
           code of the state
*/
ncLogicAnimator::eAnimationState 
ncLogicAnimator::GetAnimationState (const char* name) const
{
    n_assert(name);
   
    int len = static_cast<int>(strlen(name));
    n_assert (len>LA_CODE_STATE);

    eAnimationState state = this->state;

    if ( len > LA_CODE_STATE )
    {
        bool found = false;
        char code = (char)toupper (name[LA_CODE_STATE]);

        for ( int i=0; i<(int)LA_MAX_STATES && !found; i++ )
        {
            eAnimationState current = eAnimationState(i);

            if ( this->GetCharState(current) == code )
            {
                state = current;
                found = true;
            }
            else // Joker is never used for IRONSIGHT state but the rest of them
            if ( !this->IsSpecialState() && code == CHAR_JOKER )
            {
                state = this->state;
                found = true;
            }
            else
            if ( this->IsSpecialState() && code == CHAR_JOKER )
            {
                state = LA_STATE_NONE;
                found = true;
            }
        }
    }
    
    return state;
}

//------------------------------------------------------------------------------
/**
GetAnimationState

@brief Look at the third character of the name of the animation what is the 
code of the state
*/
ncLogicAnimator::eAnimationDirection 
ncLogicAnimator::GetAnimationDirection (const char* name) const
{
    n_assert(name);

    int len = static_cast<int>(strlen(name));
    n_assert (len>LA_CODE_DIRECTION);

    eAnimationDirection direction = this->direction;

    if ( len > LA_CODE_DIRECTION )
    {
        bool found = false;
        char code = (char)toupper (name[LA_CODE_DIRECTION]);

        for ( int i=0; i<(int)LA_MAX_DIRECTION && !found; i++ )
        {
            eAnimationDirection current = eAnimationDirection(i);

            if ( this->GetCharDirection(current) == code )
            {
                direction = current;
                found = true;
            }
            else
            if ( code == CHAR_JOKER )
            {
                direction = this->direction;
                found = true;
            }
        }
    }

    return direction;
}

//------------------------------------------------------------------------------
/**
    GetCharHandle

    @brief Get the code for an animation handle state
*/
const char*
ncLogicAnimator::GetStringHandle (eAnimationHandle state) const
{
    const char* string;

    switch (state)
    {
    case LA_HANDLE_1HAND:
        string = "1a";
        break;

    case LA_HANDLE_2HANDS:
        string = "2a";
        break;

    default:
        string = "$$";
    }

    return string;
}

//------------------------------------------------------------------------------
/**
    GetAnimationHandle
*/
ncLogicAnimator::eAnimationHandle
ncLogicAnimator::GetAnimationHandle (const char* name) const
{
    n_assert(name);

    eAnimationHandle handle = LA_HANDLE_1HAND;

    for ( int i=0; i<(ngpAction)LA_MAX_HANDLES; i++ )
    {
        eAnimationHandle currentHandle = (eAnimationHandle)i;

        if ( !strcmp (name, this->GetStringHandle (currentHandle)) )
        {
            handle = currentHandle;
        }
    }

    return handle;
}

//------------------------------------------------------------------------------
/**
    GetCharBody

    @brief Get the code for determine what part of the body is going to play the animation
*/
char
ncLogicAnimator::GetCharBody (eAnimationBody body) const
{
    char code;

    switch (body)
    {
    case LA_BODY_LOWER:
        code = 'I';
        break;

    case LA_BODY_UPPER:
        code = 'S';
        break;

    default:
        code = '$';
    }

    return code;
}

//------------------------------------------------------------------------------
/**
    GetAnimationBody

    @brief Look at the second character of the name of the animation that is the 
           code of the body part to play
*/
ncLogicAnimator::eAnimationBody 
ncLogicAnimator::GetAnimationBody (const char* name) const
{
    n_assert(name);

    int len = static_cast<int>(strlen(name));
    n_assert (len>LA_CODE_BODY);

    eAnimationBody body = LA_BODY_FULL;

    if ( len > LA_CODE_BODY )
    {
        bool found = false;
        char code = (char)toupper (name[LA_CODE_BODY]);

        for ( int i=0; i<(int)LA_MAX_BODY && !found; i++ )
        {
            eAnimationBody current = eAnimationBody(i);

            if ( this->GetCharBody(current) == code )
            {
                body = current;
                found = true;
            }
        }
    }

    return body;
}

//------------------------------------------------------------------------------
/**
    @param parts from where need the index
    @returns the index 
*/
int
ncLogicAnimator::GetActualState( int parts ) const
{
    int index = -1;

    if( ( parts & LA_BODY_LOWER ) == LA_BODY_LOWER )
    {
        index = this->lowerThirdPersonIndex;
    }
    
    if( ( index == -1 ) && ( ( parts & LA_BODY_UPPER ) == LA_BODY_UPPER ) )
    {
        index = this->upperThirdPersonIndex;
    }

    if( ( index == -1 ) && ( ( parts & LA_BODY_FULL ) == LA_BODY_FULL ) )
    {
        index = this->lowerThirdPersonIndex;
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    @param buffer 
*/
void
ncLogicAnimator::UpdateAnims( nstream & buffer )
{
    const int BACKWR_FLAG = (1<<0);
    const int INTERP_FLAG = (1<<1);
    const int LOOPED_FLAG = (1<<2);

    // saved first anim
    int flags = 0;
    if( buffer.GetWrite() )
    {
        flags |= this->savedFirstAnim.backward ? BACKWR_FLAG : 0;
        flags |= this->savedFirstAnim.interpolation ? INTERP_FLAG : 0;
        flags |= this->savedFirstAnim.looped ? LOOPED_FLAG : 0;
    }

    buffer.UpdateInt( this->savedFirstAnim.index );
    buffer.UpdateInt( flags );

    if( ! buffer.GetWrite() )
    {
        this->savedFirstAnim.backward = 0 != ( flags & BACKWR_FLAG );
        this->savedFirstAnim.interpolation = 0 != ( flags & INTERP_FLAG );
        this->savedFirstAnim.looped = 0 != ( flags & LOOPED_FLAG );

        if( this->savedFirstAnim.index != -1 )
        {
            this->SetFirstPersonActiveStateByIndex(
                this->savedFirstAnim.index, this->savedFirstAnim.backward, 
                this->savedFirstAnim.looped, this->savedFirstAnim.interpolation );
        }
    }

    // saved lower anim
    flags = 0;
    if( buffer.GetWrite() )
    {
        flags |= this->savedLowerAnim.backward ? BACKWR_FLAG : 0;
        flags |= this->savedLowerAnim.interpolation ? INTERP_FLAG : 0;
        flags |= this->savedLowerAnim.looped ? LOOPED_FLAG : 0;
    }

    buffer.UpdateInt( this->savedLowerAnim.index );
    buffer.UpdateInt( flags );

    if( ! buffer.GetWrite() )
    {
        this->savedLowerAnim.backward = 0 != ( flags & BACKWR_FLAG );
        this->savedLowerAnim.interpolation = 0 != ( flags & INTERP_FLAG );
        this->savedLowerAnim.looped = 0 != ( flags & LOOPED_FLAG );

        if( this->savedLowerAnim.index != -1 )
        {
            this->SetActiveStateByIndex(
                this->savedLowerAnim.index, this->savedLowerAnim.backward, 
                this->savedLowerAnim.looped, this->savedLowerAnim.interpolation );
        }
    }

    // saved upper anim
    flags = 0;
    if( buffer.GetWrite() )
    {
        flags |= this->savedUpperAnim.backward ? BACKWR_FLAG : 0;
        flags |= this->savedUpperAnim.interpolation ? INTERP_FLAG : 0;
        flags |= this->savedUpperAnim.looped ? LOOPED_FLAG : 0;
    }

    buffer.UpdateInt( this->savedUpperAnim.index );
    buffer.UpdateInt( flags );

    if( ! buffer.GetWrite() )
    {
        this->savedUpperAnim.backward = 0 != ( flags & BACKWR_FLAG );
        this->savedUpperAnim.interpolation = 0 != ( flags & INTERP_FLAG );
        this->savedUpperAnim.looped = 0 != ( flags & LOOPED_FLAG );

        if( this->savedUpperAnim.index != -1 )
        {
            this->SetActiveStateByIndex(
                this->savedUpperAnim.index, this->savedUpperAnim.backward, 
                this->savedUpperAnim.looped, this->savedUpperAnim.interpolation );
        }
    }
}

//------------------------------------------------------------------------------
