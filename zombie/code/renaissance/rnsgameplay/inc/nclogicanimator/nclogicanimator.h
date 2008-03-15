#ifndef NCLOGICANIMATOR_H
#define NCLOGICANIMATOR_H

//------------------------------------------------------------------------------
/**
    @class ncLogicAnimator

    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "animcomp/nccharacter.h"
#include "rnsgameplay/ngameplayenums.h"
#include "util/nmaptabletypes.h"
#include "util/nstream.h"

//------------------------------------------------------------------------------
class ncLogicAnimator : public ncCharacter
{

    NCOMPONENT_DECLARE(ncLogicAnimator,ncCharacter);

    enum eAnimationHandle
    {
        LA_HANDLE_1HAND,
        LA_HANDLE_2HANDS,

        LA_MAX_HANDLES
    };

    enum eAnimationCode
    {
        LA_CODE_DIRECTION = 0,
        LA_CODE_BODY,
        LA_CODE_STATE,

        LA_CODE_ARMS
    };    

public:
    enum eAnimationBody
    {
        LA_BODY_FULL,
        LA_BODY_LOWER,
        LA_BODY_UPPER,

        LA_MAX_BODY
    };
    
    enum eAnimationState 
    {
        LA_STATE_NONE = -1,
        LA_STATE_STAND,
        LA_STATE_STANDIRONSIGHT,
        LA_STATE_LEANIRONSIGHT,
        LA_STATE_CROUCH,
        LA_STATE_CROUCHIRONSIGHT,
        LA_STATE_PRONE,
        LA_STATE_PRONEIRONSIGHT,
        LA_STATE_SEAT,
        LA_STATE_LEAN,
        LA_STATE_HANDLADDER,
        LA_STATE_SWIM,

        LA_MAX_STATES           // Number of states
    };

    enum eAnimationDirection
    {
        LA_FORWARD = 0,
        LA_RIGHTSTRAFE,
        LA_LEFTSTRAFE,
        LA_BACKWARD,
        LA_LEFTLEAN,
        LA_RIGHTLEAN,
		LA_STOPPED,

        LA_MAX_DIRECTION

    };

    /// Constructor
    ncLogicAnimator();
    /// Destructor
    ~ncLogicAnimator();

    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

    /// Warning!!! Only por persistency (don't use)
    void SetAnimatorState(int);
    void SetAction(int);
    void SetPrevState(int);
    int GetAction()const;
    /// Set the pose of animation
    void SetMovementPose(int);
    /// Set 'walk' animation
    int SetWalk ();
    /// Set 'run' animation
    int SetRun ();
    /// Set 'strafe left' animation
    int SetStrafeLeft();
    /// Set 'strafe right' animation
    int SetStrafeRight();
    /// Set 'sprint' animation
    int SetSprint();
    /// Set 'idle' animation
    int SetIdle();
    /// Set 'combat idle' animation
    int SetCombatIdle();
    /// Set 'driving idle' animation
    int SetDrivingIdle();
    /// Set 'die' animation
    int SetDie();
    // Set 'drive' animation
    int SetDrive();
    /// Set 'swim' animation
    int SetSwim();
    /// Set 'dive' animation
    int SetDive();
    /// Set 'shoot' animation
    int SetShoot();
    /// Set 'shootauto' animation
    void SetShootAuto();
    /// Set 'melee attack' animation
    int SetMeleeAttack();
    /// Set 'melee block' animation
    int SetMeleeBlock();
    /// Set 'impact' animation
    int SetImpact();
    /// Set 'mini impact' animation
    int SetMiniImpact();
    /// Set 'left claw attack' animation
    int SetLeftClawAttack();
    /// Set 'right claw attack' animation
    int SetRightClawAttack();
    /// Set 'both claws attack' animation
    int SetBothClawsAttack();
    /// Set 'fly' animation
    int SetFly();
    /// Set 'jump start' animation
    int SetJumpStart();
     /// Set 'jump' animation
    int SetJumpMid(bool);
    /// Set 'jumpend' animation
    int SetJumpEnd();
    /// Set 'jumpobstacle' animation
    int SetJumpObstacle();
    /// Set 'jumpattack' animation
    int SetJumpAttack();
    /// Set 'jump end success' animation
    int SetJumpEndSuc();
    /// Set 'jump end fail' animation
    int SetJumpEndFail();
    /// Set 'battlecry' animation
    int SetBattleCry();
    /// Set 'mop' animation
    int SetMop();
    /// Set 'leap' animation
    int SetLeap();
    /// Set 'taunt' animation
    int SetTaunt();
    /// Set 'burrow,' animation
    int SetBurrow();
    /// Set 'roll' animation
    int SetRoll();
    /// Set 'climb' animation
    int SetClimb();
    /// Set 'fast first reload' animation
    int SetFastFirstReload();
    /// Set 'fast second reload' animation
    int SetFastSecondReload();
    /// Set 'full reload' animation
    int SetFullReload();
    /// Set 'full first reload' animation
    int SetFullFirstReload();
    /// Set 'full second reload' animation
    int SetFullSecondReload();
    /// Set 'end reload' animation
    int SetEndReload();
    /// Set 'cock reload' animation
    int SetCockReload();
	/// Set 'hide weapon' animation
 int SetHideWeapon();
	/// Set 'show weapon' animation
    int SetShowWeapon();
    /// Set 'weapon acc' animation
    int SetWeaponAcc();
    /// Set 'apply injection' animation
    int SetInjection();
    /// Set 'look around' animation
    int SetLookAround();
    /// Set 'silenceron' animation
    int SetSilencerOn();
    /// Set 'silenceroff' animation
    int SetSilencerOff();
    /// Set 'switch' animation
    int SetSwitchFireMode();
    /// Set transition to/from 'ironsight' animation
    int SetIronsight(bool);
    /// Set transition to/from 'leanironsight' animation
    int SetLeanIronsight(bool);
    /// Set transition to/from 'crouch' animation
    int SetCrouch(bool);
    /// Set transition to/from 'prone' animation
    int SetProne(bool);
    /// Set transition to/from 'seat' animation
    int SetSeat(bool);
	/// Set transition to/from 'left lean' animation
 int SetLean(bool);
	/// Set transition to/from 'right lean' animation
// int SetRightLean(bool);
    /// Set transition to/from 'Flashlight' animation
    int SetFlashlight();
    /// Say if the requested action has finished
    bool HasFinished(int) const;
    /// Whether all requested actions have finished
    bool HasFinishedAllAnims();
    /// Get a random animation of a type
    int GetIndexRandomState(const char*,bool);
    /// Set an animation action
    int SetAnimationAction(int, bool, bool, bool);

    // Update character
    bool UpdateCharacter( int charIndex, nTime curTime );

    /// Force a new animation of a state
    void ForceAnimation (ngpAction action, int train );

	/// Get the string of the current state
	const nString& GetStringState() const;

    /// get the internal state index
    int GetStateIndex() const;

	/// get the internal state index
    int GetUpperStateIndex() const;

    /// get the internal first person state index
    int GetFirstPersonStateIndex() const;

    /// get the current animation state
    int GetAnimatorState () const;

    /// get the animation index for one state
    int GetActualState( int parts ) const;

    /// descriptor is used externally for know witch animation has been selected in the case of 
    /// random selection animations (idles, taunts, claws...). Utilities can go from launch a
    /// determine sound to apply different damage points to gameplay entities.
    int GetRandomIndexAnimation () const;

    void UpdateAnims( nstream & buffer );
private:
    /// Says the number of animations has an animation class
    int GetNumberOfAnimationClass (const char* animationClass, bool firstPerson) const;
    /// Get the index of the animator for a given animation class and order
    int GetStateOrder (const char* animationClass, int order, bool firstPerson) const;
    /// Say if an animation is used at the current state
    bool IsAnimationType (const char* animationClass, const char* animation) const;

    /// Update the stateIndex and play an animation
    int PlayAnimation (const char* animation, bool backward = false, bool looped = false, bool interpolation = false);
    int PlayAnimation (ngpAction action, bool backward = false, bool looped = false, bool interpolation = false);    

    /// Checks if the requested animation is finished
    bool CheckEnd( int index, bool fperson ) const;

    /// Conversion between state and strings
    char GetCharState (eAnimationState state) const;
    eAnimationState GetAnimationState (const char* name) const;
    /// Conversion handle state and strings
    const char* GetStringHandle (eAnimationHandle handle) const;
    eAnimationHandle GetAnimationHandle (const char* name) const;
    /// Conversion body part and strings
    char GetCharBody (eAnimationBody body) const;
    eAnimationBody GetAnimationBody (const char* name) const;
    /// Conversion between direction and strings
    char GetCharDirection (eAnimationDirection state) const;
    eAnimationDirection GetAnimationDirection (const char* name) const;

	/// Says if the current state is a special state
	bool IsSpecialState() const;

    /// Change a state
    void ChangeState (eAnimationState state, eAnimationState prevState);

    /// Return the reflexed third person animation index from a fperson index
    int GetThirdAnimationReflex( int fidx );

    /// Test if the name of two animations are identical
    bool EqualAnimationName( const char* firstAnim, const char* thirdAnim );

    /// Set the third person state
    void SetThirdPersonState( int index, bool backward, bool looped, bool interpolation );

    /// Get the inverse train index if any
    int GetInverseTrain ( int index, int train ) const;

    /// Set idle for upper train
    void SetIdleUpper();

    /// Set idle for lower train
    void SetIdleLower();

    /// Set the  specific train animation for third person index
    void SetThirdPersonStateByTrain ( int train, int index );

    /// Set the idle animation for upper train
    void SetUpperIdle ();

    eAnimationBody GetUsedTrains ( int index ) const;

    eAnimationState  state;             // current state
    eAnimationState  prevState;         // previous state
    ngpAction		 action;            // current action running
    ngpAction		 prevAction;        // current action running
    ngpAction        blockedAction;     // block animation action
    eAnimationDirection direction;		// Actual direction animation
	eAnimationDirection newDirection;	// New direction for the animation

    int              firstPersonIndex;  // internal index of the upper first person state

    int              lowerThirdPersonIndex;        // internal index of the lower third person state
    int              upperThirdPersonIndex;        // internal index of the lower first person state

    bool			 changeState;       // says if right now is changing a state

    bool             upperBlocked;      // true if upper train is blocked
    bool             lowerBlocked;      // true if lower train is blocked
    bool             fpersonBlocked;    // true if the animation for fperson is blocked
    
    int              descriptor;        // descriptor for random animation selection

    struct SavedAnim
    {
        int index;
        bool backward;
        bool looped;
        bool interpolation;

        SavedAnim():index(-1),backward(false),looped(false),interpolation(false){}
    };

    SavedAnim savedFirstAnim;
    SavedAnim savedLowerAnim;
    SavedAnim savedUpperAnim;

    nMapTableTypes<int>::Int animData;

};
 
//------------------------------------------------------------------------------
/**
    GetRandomIndexAnimation
*/
inline
int
ncLogicAnimator::GetRandomIndexAnimation() const
{
    return this->descriptor;
}

//------------------------------------------------------------------------------
/**
    SetAnimatorState
*/
inline
int
ncLogicAnimator::GetAnimatorState() const
{
    return this->state;
}
//------------------------------------------------------------------------------
/**
    SetAnimatorState
*/
inline
void
ncLogicAnimator::SetAnimatorState (int state)
{
    this->state = eAnimationState (state);
}

//------------------------------------------------------------------------------
/**
    SetAction
*/
inline
void
ncLogicAnimator::SetAction (int action)
{
    this->action = ngpAction (action);
}

//------------------------------------------------------------------------------
/**
    GetAction
*/
inline
int
ncLogicAnimator::GetAction ()const
{
    return this->action;
}

//------------------------------------------------------------------------------
/**
    SetPrevState
*/
inline
void
ncLogicAnimator::SetPrevState (int prevState)
{
    this->prevState = eAnimationState (prevState);
}

//------------------------------------------------------------------------------
/**
	IsSpetialState
*/
inline
bool
ncLogicAnimator::IsSpecialState() const
{
	return  this->state == LA_STATE_STANDIRONSIGHT ||
            this->state == LA_STATE_CROUCHIRONSIGHT ||
            this->state == LA_STATE_PRONEIRONSIGHT ||
            this->state == LA_STATE_LEAN ||
            this->state == LA_STATE_LEANIRONSIGHT;
}

//------------------------------------------------------------------------------
/**
	GetStateIndex
*/
inline
int
ncLogicAnimator::GetStateIndex() const
{
    return this->lowerThirdPersonIndex;
}

//------------------------------------------------------------------------------
/**
	GetStateIndex
*/
inline
int
ncLogicAnimator::GetUpperStateIndex() const
{
    return this->upperThirdPersonIndex;
}

//------------------------------------------------------------------------------
/**
	GetFirstPersonStateIndex
*/
inline
int
ncLogicAnimator::GetFirstPersonStateIndex() const
{
    return this->firstPersonIndex;
}

#endif
