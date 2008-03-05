#ifndef NCGAMEPLAYPLAYER_H
#define NCGAMEPLAYPLAYER_H
//------------------------------------------------------------------------------
/**
    @class ncGameplayPlayer
    @ingroup Entities

    Component with the behaviour of a Player

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayplayer/ncgameplayplayerclass.h"
#include "nphysics/nphycollide.h"

#include "util/npfeedbackloop.h"

//------------------------------------------------------------------------------
class polar2;
class nPhyGeomRay;
class ncPhyCharacter;
class ncTransform;

//------------------------------------------------------------------------------

class ncGameplayPlayer : public ncGameplayLiving
{

    NCOMPONENT_DECLARE(ncGameplayPlayer,ncGameplayLiving);

public:
    enum MoveFlags
    {
        IS_JUMPING          = (1<<0),
        IS_SPRINTING        = (1<<1),
        IS_LEFTLEAN         = (1<<2),
        IS_RIGHTLEAN        = (1<<3),

        MF_UPDATEPHYSICS    = (1<<4),
        MF_UPDATEPOSITION   = (1<<5),
        MF_UPDATEANGLE      = (1<<6),
        MF_BLOCKSPRINT      = (1<<7),
        MF_BLOCKLOWERANIM   = (1<<8),

        LAST_FLAG
    };

    typedef enum {
        IS_NOTHING,
        IS_DRIVER,
        IS_PASSENGER,
        IS_FROZEN
    } state;    

    /// Constructor
    ncGameplayPlayer();
    /// Destructor
    ~ncGameplayPlayer();

    /// initialize component data
    void InitInstance(nObject::InitInstanceMsg initType);
    
    /// Persistency
    bool SaveCmds (nPersistServer* ps);

    /// @name Script interface
    //@{
    /// get if is local player
    bool IsLocalPlayer ()const;
    /// Sets/gets the stamina
    void SetStamina(int);
    int GetStamina() const;
    /// Sets/gets 'has stamina' property
    void SetHasStamina(bool);
    bool HasStamina() const;
    /// Sets the special equipment item
    void SetSpecialEquipment(int,bool);
    /// Gets the special equipment item
    bool GetSpecialEquipment(int)const;
    /// Sets the elevation limits of the player
    void SetElevationLimits(const vector2&);
    /// Gets the elevation limits of the player
    void GetElevationLimits(vector2&)const;
    /// Sets the turn limits of the player
    void SetTurnLimits(const vector2&);
    /// Gets the turn limits of the player
    void GetTurnLimits(vector2&)const;
    /// Sets the angle center of player
    void SetAngleCenter(const vector2&);
    /// Gets the angle center of player
    void GetAngleCenter(vector2&)const;
    /// Gets if player is in scope
    bool IsInScope ()const;
    /// Sets the input state
    void SetInputState (const state);
    /// Gets the input state
    state GetInputState()const;
    //@}

    NSIGNAL_DECLARE('DSTC', void, ChangePlayerState, 2, (int, nEntityObject*), 0, ());

    /// runs the gameplay logic 
    virtual void Run ( const float time);

    /// set the ironsight state
    void SetIronsight( bool activate );
    /// set the proning state
    virtual void SetProning( bool activate );
    /// set the crouching state
    virtual void SetCrouching( bool activate );

    /// set the first camera mode state
    void SetFirstCameraMode( bool activate );
    /// get the first camera mode state
    bool IsFirstCameraMode()const;

    /// set the lean of player to left
    void SetLeftLean( bool activate );
    /// set the lean of player to right
    void SetRightLean( bool activate );

    /// get the angle of view of the player
    float GetAngleOfView()const;

    /// get shot position of player
    void GetShotPosition( vector3 & pos )const;
    /// get shot direction of player
    void GetShotDirection( vector3 & dir )const;
    /// get the angles of shot direction of player
    void GetShotAngles( polar2 & angles )const;

    /// Get real position of entity
    void GetPosition( vector3 & pos )const;

    /// set shot ray for weapon shoot
    void SetShotPosition( const vector3 & pos );
    /// set the angles of shot direction
    void SetShotAngles( const polar2 & angles );

    /// get physic position of player
    void GetPhysicPosition( vector3 & pos )const;
    /// set physic position of player
    void SetPhysicPosition( const vector3 & pos );

    /// update camera position
    void UpdateCamera( nTime time );
    /// get the camera position
    void GetCameraPosition( vector3 & pos ) const;

    /// apply deviation for weapon
    void ApplyRecoilDeviation( );

    /// perform server movement locally
    void MoveAutonomous( float timestamp, const int forward, const int step,
                            const float yaw, const float pitch, const int flags );
    /// perform player movement
    void ServerMove( float timestamp, const vector3 & pos, const int forward, const int step,
                            const vector2 & angle, const float yaw, const float pitch, const int flags );
    /// adjust player movement
    void AdjustPosition( float timestamp, const int flags, 
                            const vector3 & position, const vector3 & velocity,
                            const vector2 & angle, const vector2 & angleIncrement );

    /// calculate the player speed
    void CalculatePlayerSpeed();

    /// set the player velocity to running
    void SetCameraHeight (float height);
    float GetCameraHeight ();

    /// Sets primary weapon
    virtual void SetCurrentWeapon( nEntityObject * weapon );

    /// set the objects used in player in Best Lod possible
    void SetFirstCameraLod( bool activate ); 

    /// sets the player state
    void SetState( const state newstate );

    /// gets the player state
    const state GetState() const;

    void DeactivePhysics();

    void ActivatePhysics();

    /// gets the seat where the player is
    nEntityObject* GetSeat() const;

    /// sets the seat where the player is
    void SetSeat( nEntityObject* newseat );

    /// gets the players physics
    ncPhyCharacter* GetPlayersPhysics();

    /// get the player velocity
    float GetSpeed();   

    /// Toggle the movement flags
    void MovementFlag (unsigned int flag, bool activate);

    /// Toggle the movement flags
    bool IsFlagActivated (unsigned int flag) const;

    /// hide the player in scope mode
    void RefreshScopeState();

    /// Add time to the shoot timer 
    void AddShotTimer( const nTime & t );

    /// say if only has shooted one bullet
    bool IsFirstShot() const;

    /// set the weapon trigger state
    void SetWeaponTrigger( bool trigger, bool pressed );

#ifndef NGAME
    /// debug draw of entity
    virtual void DebugDraw( nGfxServer2 * const gfxServer );
#endif//!NGAME

    /// update the position of character using the physic position
    void UpdatePlayerPosition();

private:
    static const float SCOPE_ANGLE;
    static const float IRONSIGHT_ANGLE;
    static const float NORMAL_ANGLE;
    static const float IRONSIGHT_ANGLE_SPEED;
    static const float IRONSIGHT_TURN_ADJUST;
    static const float IRONSIGHT_VELOCITY_FACTOR;
    static const float WEAPON_RECOVER_SPEED;
    static const float WEAPON_VERTICAL_ADJUST;
    static const float WEAPON_VERTICAL_SPEED;
    static const float WEAPON_VERTICAL_BUMP;
    static const float WEAPON_HORIZONTAL_MOVE;
    static const float MAX_ANGLEELEVATION;
    static const float MIN_ANGLEELEVATION;
    static const float MAX_ANGLETHIRD;
    static const float MIN_ANGLETHIRD;
    static const float STEP_HEIGHT;
    static const float JUMP_CONTROL;
    static const int MAX_SAVEDMOVE_STATES = 40;

    /// move the player an amount of time indicated in the parameter
    void MovePlayerPosition( const float deltaReal );
    /// update player orientation
    void MovePlayerAngle( const float deltaReal );
    /// update the difference between new angles and old angles
    void UpdateDiffAngles( const polar2 & angles );
    /// check if player is not touching the floor
    void CheckFalling();
    /// apply player angles to 
    void ApplyAngles();
    /// update player stamina
    void UpdateStamina( const float frameTime );
    /// update player 
    void UpdateGraphicsSound();
    /// get the player velocity
    float GetVelocityFactor()const;
    /// shot the weapon if necessary
    void ShotWeapon();
    /// set the movement state after a move
    void SetMovementState( const int flags, const int forward, const int step );
    /// execute player jump
    void Jump();
    /// update velocity
    void UpdateVelocity();
    /// save current state of movement
    void SaveMoveState( float time );
    /// update the player in clients
    void SendPlayerAdjust()const;
    /// calculate angle change
    void CalculateAngleMove( const polar2 & oldangle, polar2 & newangle, const float deltaTime ) const;
    /// update the recoil angle increment
    void UpdateRecoil( const float deltaReal );

    // variables for player collision
    static const int MAX_CONTACTS = 10;
    nPhyCollide::nContact contacts[ MAX_CONTACTS ];
    nRef<nPhyGeomRay> collisionRay;

    nRef<nEntityObject> collisionEntity;
    ncTransform * collisionTransform;
    ncPhyCharacter * collisionPhysics;
    ncGameplayPlayerClass * playerClass; 

    nArray<int> specialEquipment;

    vector3 shotDirection;
    vector3 shotPosition;
    vector3 diffPosition;

    vector3 velocity;
    vector3 inputVelocity;
    vector3 jumpVelocity;

    vector2 recoilDifference;
    vector2 recoilRecover;
    polar2 upperAngles;
    polar2 lowerAngles;
    polar2 diffAngles;
    polar2 incrementAngles;
    polar2 desiredAngles;

    struct SavedMove {
        float timestamp;
        vector3 position;
        vector3 velocity;
    };

    nArray<SavedMove> savedMoves;

    // angle restrictions
    polar2 angleOrigin;
    float turnMaximum;
    float turnMinimum;
    float elevationMaximum;
    float elevationMinimum;

    nTime shotTime;
    float jumpTime;
    float sprintVelocityFactor;
    float runVelocityFactor;
    float cameraHeight;
    float leanDistance;
    float ironsightDownOffset;
    float ironsightAngle;
    int movementFlags;
    int actualAnim;
    int actualDirection;
    int newDirection;
    int jumpAnim;
    bool isFalling;
    bool isJumping;
    bool isClimbing;
    bool isFirstCameraMode;
    bool isInScopeMode;
    bool interpolateAngle;
    /// variables to indicate if shot or not
    bool isWeaponPressed;
    bool isWeaponTrigger;
    bool firedLastFrame;
    
    /// stores the player state
    state playerState;

    nPFeedbackLoop<float> fbIronsightAngle;
    nPFeedbackLoop<vector3> fbMove;
    nPFeedbackLoop<vector3> fbTurn;
    nPFeedbackLoop<vector3> fbLean;
    nPFeedbackLoop<float> fbFalling;

    // Stamina properties    
    int stamina;
    bool  hasStamina;
    // Tired time control
    float lastTiredTime; // Counter for tired state
    bool isTired;

    float speed;

    bool sprintCancelled;
    
    /// stores the player's seat
    nEntityObject* seat;

};

//------------------------------------------------------------------------------
/**
    @param flag which flag affect
    @param active if true flag is activate else flag is deactivate
*/
inline
void
ncGameplayPlayer::MovementFlag (unsigned int flag, bool activate)
{
    if ( activate )
    {
        this->movementFlags |= flag;
    }
    else
    {
        this->movementFlags &= ~flag;
    }
}

//------------------------------------------------------------------------------
/**
GetSpeed
*/
inline
float
ncGameplayPlayer::GetSpeed()
{
    return this->speed;
}

//------------------------------------------------------------------------------
/**
SetCameraHeight
*/
inline
void
ncGameplayPlayer::SetCameraHeight (float height)
{
    this->cameraHeight = height;
}

//------------------------------------------------------------------------------
/**
GetCameraHeight
*/
inline
float
ncGameplayPlayer::GetCameraHeight ()
{
    return this->cameraHeight;
}

//------------------------------------------------------------------------------
/**
IsSprintCancelled
*/
inline
bool
ncGameplayPlayer::IsFlagActivated( unsigned int flag) const
{
    return (this->movementFlags&flag) != 0;
}

//------------------------------------------------------------------------------
/**
SetHasStamina
*/
inline
void
ncGameplayPlayer::SetHasStamina (bool value)
{
    this->hasStamina = value;
}

//------------------------------------------------------------------------------
/**
HasStamina
*/
inline
bool
ncGameplayPlayer::HasStamina() const
{
    return this->hasStamina;
}
//------------------------------------------------------------------------------
/**
GetStamina
*/
inline
int
ncGameplayPlayer::GetStamina() const
{
    return this->stamina;
}

//------------------------------------------------------------------------------
/**
SetHealth
*/
inline
void
ncGameplayPlayer::SetStamina(int stamina)
{
    if (stamina > 0 && stamina < this->GetClassComponentSafe<ncGameplayPlayerClass>()->GetMaxStamina())
    {
        this->stamina = stamina;
    }
    else
    if (stamina < 1)
    {
        this->stamina = 0;
    }
    else
    {
        this->stamina = this->GetClassComponentSafe<ncGameplayPlayerClass>()->GetMaxStamina();
    }
}

//------------------------------------------------------------------------------
/**
    @returns true if player is the local player
*/
inline
bool
ncGameplayPlayer::IsLocalPlayer()const
{
    return this->GetClassComponentSafe<ncGameplayPlayerClass>()->IsLocalPlayer();
}

//------------------------------------------------------------------------------
/**
    @param pos the returned shot position
*/
inline
void 
ncGameplayPlayer::GetShotPosition( vector3 & pos )const
{
    if( this->isFirstCameraMode )
    {
        this->GetCameraPosition( pos );
    }
    else
    {
        pos = this->shotPosition;
    }
}

//------------------------------------------------------------------------------
/**
    @param dir the returned shot direction
*/
inline
void 
ncGameplayPlayer::GetShotDirection( vector3 & dir )const
{
    dir = this->shotDirection;
}

//------------------------------------------------------------------------------
/**
    @param pos the new shot position
*/
inline
void 
ncGameplayPlayer::SetShotPosition( const vector3 & pos )
{
    vector3 newPosition( pos );
    newPosition.y -= this->cameraHeight;
    n_assert( this->collisionEntity );
    this->collisionTransform->SetPosition( newPosition );
}

//------------------------------------------------------------------------------
/**
    @param pos the returned player position
*/
inline 
void
ncGameplayPlayer::GetPosition( vector3 & pos )const
{
    pos = this->shotPosition;
    if( this->IsLocalPlayer() )
    {
        pos.y -= this->cameraHeight;
    }
}

//------------------------------------------------------------------------------
/**
    @returns the angle of view
*/
inline
float
ncGameplayPlayer::GetAngleOfView()const
{
    return this->fbIronsightAngle.GetState();
}

//------------------------------------------------------------------------------
/**
    @param pos the returned player position
*/
inline 
void
ncGameplayPlayer::GetPhysicPosition( vector3 & pos )const
{
    if( this->collisionTransform )
    {
        pos = this->collisionTransform->GetPosition();
    }
}

//------------------------------------------------------------------------------
/**
    @param pos the returned player position
*/
inline 
void
ncGameplayPlayer::SetPhysicPosition( const vector3 & pos )
{
    if( this->collisionTransform )
    {
        this->collisionTransform->SetPosition( pos );
    }
}

//------------------------------------------------------------------------------
/**
    @returns true if it is in first camera mode
*/
inline
bool 
ncGameplayPlayer::IsFirstCameraMode()const
{
    return this->isFirstCameraMode;
}

//------------------------------------------------------------------------------
/**
    @param set the input state
*/
inline
void
ncGameplayPlayer::SetInputState( const state mode )
{
    this->SetState( mode );
}

//------------------------------------------------------------------------------
/**
    @returns The current input state
*/
inline
ncGameplayPlayer::state
ncGameplayPlayer::GetInputState() const
{
    return this->GetState();
}

//------------------------------------------------------------------------------
N_CMDARGTYPE_NEW_TYPE( ncGameplayPlayer::state, "i", (value = (ncGameplayPlayer::state) cmd->In()->GetI()), (cmd->Out()->SetI(value)) );

//------------------------------------------------------------------------------
#endif//NCGAMEPLAYPLAYER_H
