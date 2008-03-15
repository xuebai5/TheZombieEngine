#ifndef NC_AIMOVENGINE_H
#define NC_AIMOVENGINE_H

//------------------------------------------------------------------------------
/**
    @class ncAIMovEngine

    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "mathlib/plane.h"

//------------------------------------------------------------------------------
class quaternion;
class nGfxServer2;
class nPersistServer;
class nEntityObject;
class ncTransform;
class ncLogicAnimator;
class ncSoundLayer;
class nPhyGeomRay;
class ncSpatialCell;

//------------------------------------------------------------------------------
class ncAIMovEngine : public nComponentObject
{

    NCOMPONENT_DECLARE(ncAIMovEngine,nComponentObject);

    enum eEngineState
    {
        ST_STOPPED  = 0,
        ST_PAUSED,   
        ST_RUNNING,
        ST_TURNING,
        ST_FACING,
        ST_JUMPING,
        ST_DROPPING
    };

    struct movStretch 
    {
        vector3 start, goal;    // Sign the current stretch
        plane goalPlane;        // Crossing this plane equals to reaching the goal point when avoiding obstacles
        vector3 facingPoint;    // The point to facing whilst moving
        quaternion final;        // Used for facing    
        bool facing;            // Says if we must face the entity to other point
        float jumpAngle;        // Says the angle of jump
        float jumpTime;            // Says the current time jumping
        float xspeed;           // Jump horizontal speed
        float yspeed;           // Jump vertical speed
        float jumpHeight;       // height achieved with jump
        vector3 direction;      // Direction vector used to jump
        nTime maxTimeToGoal;    // Max time allowed to complete this stretch
    };

public:
    
    enum eMovDirection
    {
        MD_FORWARD = 0,
        MD_RIGHTSTRAFE,
        MD_LEFTSTRAFE,
        MD_BACKWARD,

        MD_MAX_DIRECTION
    };

    enum eMovMode
    {
        MOV_WALK,
        MOV_RUN,
        MOV_WALKPRONE,
    };

    /// Constructor
    ncAIMovEngine();
    /// Destructor
    ~ncAIMovEngine();

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Moves the entity to a given position
    bool MoveToFacing(const vector3&, const vector3&);
    bool MoveTo(const vector3&);
    bool MoveToZigZag(const vector3&, const vector3&, bool, nArray<bool> &);
    bool Move(const vector3&, const vector3&);
    bool MoveZigZag(const vector3&, const vector3&, nArray<bool> &);
    /// Jump
    bool Jump();
    /// get the total jump distance of the actual animation
    vector3 GetJumpDistance() const;
    /// get the jump distance of the actual animation without apply factor
    vector3 GetOriginalJumpDistance() const;
    /// set the factor applied to jump displacement
    void SetJumpFactor(const vector3&);
    /// get the factor applied to jump displacement
    vector3 GetJumpFactor () const;

    /// Drop
    bool DropTo(const vector3&);

    /// Stops the entity
    void Stop();
    /// Face the entity to a given position
    bool FaceTo(const vector3&);
    /// Face the entity to a given direction
    bool Face(const quaternion&);
    
    /// Set the type of movement
    void SetStyle(int);
    /// Get the type of movement
    int GetStyle();

    /// Set the direction
    void SetMovDirection(int);
    
    /// Set the movement mode
    void SetMovMode(int mode, float speedFactor = 1.f);
    /// Get the movement mode
    int GetMovMode() const;

    /// Set the state of the engine
    void SetMovEngineState(int);
    /// Get the state of the engine
    int GetMovEngineState () const;
    /// Says if there are some actions running in the engine
    bool IsActioned() const;
    /// Says if the engine is running
    bool IsMoving() const;
    /// Says if the engine is taking some kind of turn
    bool IsTurning() const;
    /// Says if the engine if facing to some target
    bool IsFacing() const;
    /// Says if the engine is stopped
    bool IsStopped() const;
    /// Says if the engine is jumping
    bool IsJumping() const;
    /// Says if the engine is dropping
    bool IsDropping() const;

    // Get the speed of the entity
    float GetEntitySpeed() const;

    /// Get the current speed
    float GetSpeed() const;
    void SetSpeed(float);
    /// Get the jump range
    float GetJumpRange() const;

    /// Pause the engine
    void SetPause(bool);
    /// Says if the engine is paused
    bool IsPaused() const;

    /// Says if is approaching to a position
    bool IsApproaching(const vector3&) const;
    /// Says if a position is at the floor
    bool IsAtFloor(const vector3&) const;

    /// Get the final goal
    const vector3& GetGoal() const;
    /// Set the goal (for persistency only)
    void SetGoal(const vector3&);
    /// Set the start (for persistency only)
    void SetStart(const vector3&);

    // -- Obstacle avoidance
    /// Get the maximum avoidance turn inc allowed
    float GetMaxAvoidanceTurnInc() const;
    /// Set the maximum avoidance turn inc allowed
    void SetMaxAvoidanceTurnInc(float);
    /// Get the maximum avoidance turn acceleration allowed
    float GetMaxAvoidanceTurnAccel() const;
    /// Set the maximum avoidance turn acceleration allowed
    void SetMaxAvoidanceTurnAccel(float);
    /// Get the look ahead avoidance time
    float GetAvoidanceTime() const;
    /// Set the look ahead avoidance time
    void SetAvoidanceTime(float);
    /// Get the maximum avoidance throttle acceleration allowed
    float GetMaxAvoidanceThrottleAccel() const;
    /// Set the maximum avoidance throttle acceleration allowed
    void SetMaxAvoidanceThrottleAccel(float);
    /// Says if the current position is at the end of the current stretch
    bool IsAtEndStretch() const;

    // -- Spatial queries
    /// Get the dynamic obstacles near this entity
    void GetNearDynamicObstacles( bool updateCells, nArray<nEntityObject*>& entities );
    /// Set the time when the dynamic obstacles cells list must be updated
    void SetAvoidanceCellsUpdateTime( int interval );
    /// Get the time when the dynamic obstacles cells list must be updated
    int GetAvoidanceCellsUpdateTime() const;

    /// Run the logic of the movement engine
    void Run(const float deltaTime);
    
    /// Say if the current position is at the end of the movement
    bool IsAtEnd() const; 

    /// Update the height of a given position
    void UpdateHeight (vector3* position) const;
    /// Move the given point to the ground by applying gravity over time
    void UpdateHeightSmoothly( vector3& position );

    /// Get move direction
    void GetMoveDirection( vector3& dir ) const;

    /// Get current stretch's goal
    const vector3& GetCurrStretchGoal() const;

    /// Get the look ahead obstacles avoidance distance for current speed
    float GetAvoidanceDistance() const;

#ifndef NGAME
    /// Draws the engine (for debug only)
    void Draw (nGfxServer2* server);
    /// Enable draw of most motion info
    void EnableDrawMotion( bool enable );
    /// Enable draw of common info for obstacles avoidance
    void EnableDrawAvoid( bool enable );
    /// Enable draw of specific info for static obstacles avoidance
    void EnableDrawStaticAvoid( bool enable );
    /// Enable/disable obstacles avoidance for all entities
    static void EnableObstaclesAvoidance( bool enable );
    /// Enable/disable throttle for obstacles avoidance for all entities
    static void EnableThrottleSteering( bool enable );
    /// Enable/disable non-penetration constraint for all entities
    static void EnableNonPenetrationConstraint( bool enable );
    /// Enable/disable adjusment of position due to animation displacement
    static void EnableAnimDisplacement( bool enable );
    /// Enable/disable simulation of constant elapsed time between consecutive motion updates
    static void EnableConstantTime( bool enable );
    /// Enable/disable rotation smoothing for all entities
    static void EnableRotationSmoothing( bool enable );
#endif
    /// Enable/disable containment constraint for all entities
    static void EnableContainmentConstraint( bool enable );

private:    
    /// Auxiliary functions for main run
    bool RunMovement();
    //bool RunTurning();
    bool RunFacing();
    bool RunDropping();
    bool RunJumping();

    /// Get the current stretch
    void GetCurrentStretch (const vector3& position);
    ///// Calculates the turn position between two segments
    //void CalculateTurnPosition();

    /// Update the entity position
    void UpdateEntity ();

    /// Adjust a given position to a valid position
    bool AdjustPosition (vector3* position) const;

    /// Says if is at the end of a curve
    bool IsAtEndCurve() const;

    /// Get the current orientation and position for the unit
    void GetSituation (vector3* position, quaternion* direction);

    /// Find the index of a point in the path
    int FindPoint (const vector3& point) const;

    /// Get next point
    vector3 GetNextPoint (const vector3& start) const;

    /// Draw particular path of the entity
    void DrawPath (nGfxServer2* server);
    void DrawCross (nGfxServer2* server, const vector3& point);

    /// Smooth the current path
    void SmoothCurrentPath();

    // Calculate a quaternion direction from the current position and a final position
    quaternion GetFinalDirection (const vector3& position, const vector3& finalPos) const;

    /// Look for immediate obstacles and modify the given movement direction to avoid them
    void AvoidObstacles( vector3& moveStep, quaternion& orientation );
    void ApplyAvoidanceSteering( vector3& moveStep, quaternion& orientation );
    void AvoidStaticObstacles( const vector3& moveDir );
    void AvoidDynamicObstacles( const vector3& moveDir );
    /// Prevent entities from crossing each other
    void AvoidPenetration( vector3& moveStep );
    /// Prevent entity from going out of the navigable space
    void ForceContainment( vector3& moveStep );
    /// Prevent entity from changing their orientation abruptly
    void SmoothRotation( quaternion& orientation );

    /// Says if an action can be done (movement, jump, face... etc)
    bool CanDoAction() const;

    /// update saved animation index
    void UpdateDisplacementIndex();
    /// get animation displacement of last game frame
    void GetAnimDisplacement( vector3 & desplacement ) const;

    /// Get the gameplay body size of an agent
    float GetBodySize( const nEntityObject* entity ) const;

    /// Complete current path
    nArray<vector3> path;
    /// Say the part of the path smoothed
    int indexSmooth;

    /// Current stretch of movement
    movStretch current;

    /// State of the engine
    eEngineState state;
    eEngineState pausedState;

    /// Goal of movement
    vector3 start, goal;

    /// Debug info
    vector4 entityAreaColor;

    /// Other AI components relationed with the movement engine
    ncTransform* transform;
    ncLogicAnimator* animator;
    ncSoundLayer* soundLayer;

    int dispIndex;
    bool useAnimDisplacement;
    vector3 dispFactor;

    ///// last time
    //unsigned int lastTime;
    /// delta time in gameplay units
    float deltaGameplay;

    int style;
    float speed;    // The current speed for the engine
    float speedFactor; // The current speed factor for the engine

    /// Engine's current moving direction
    int direction;

    /// Engine's current movement mode
    int movMode;

    /// Previous position (used as a way to determine direction not dependent on movement mode)
    vector3 previousPosition;

    /// Downside velocity due to not have been touching the ground for a while (applied to walking/running)
    float fallingVelocity;

    // -- Obstacle avoidance
    /// Accumulated turn radius over the desired movement vector
    float avoidanceTurnInc;
    /// Direction towards where to turn to avoid expected obstacles
    float avoidanceTurnDir;
    /// Max avoidance turn inc allowed
    float maxAvoidanceTurnInc;
    /// Max avoidance turn acceleration allowed
    float maxAvoidanceTurnAccel;
    /// Look ahead time for obstacles avoidance
    float avoidanceTime;
    /// Some collision is expected if no avoidance steering is applied?
    bool collisionExpected;
    /// Expected distance (squared) to collision if no avoidance steering is applied
    float distToCollisionSquared;
    /// Acceleration or brake factor over the desired movement vector
    float avoidanceThrottle;
    /// Braking or accelerating?
    float avoidanceThrottleDir;
    /// Max avoidance throttle "acceleration" allowed
    float maxAvoidanceThrottleAccel;
#ifndef NGAME
    /// Rays used to detect potencial static obstacles while moving
    nArray< line3 > avoidanceRays;
    // Expected point of collision if no obstacle avoidance is applied
    vector3 expectedCollisionPoint;
#endif

    // -- Spatial queries
    /// Time till next updating of the dynamic obstacles cells list
    int avoidanceCellsUpdateTime;
    /// Near cells for dynamic obstacles faster queries
    nArray<ncSpatialCell*> avoidanceCells;

    // -- Enables
#ifndef NGAME
    // Debug draw enables
    bool drawMotion;
    bool drawAvoidance;
    bool drawStaticAvoidance;
    // Entities should try to avoid obstacles?
    static bool avoidObstacles;
    // Entities should accelerate/brake to avoid obstacles?
    static bool useThrottle;
    // Entities cannot pass through each other?
    static bool dontPenetrate;
    // Animation must adjust entity position?
    static bool displaceAnim;
    // Use a constant delta time for motion update?
    static bool constantTime;
    // Smooth abrupt direction changes?
    static bool smoothRotations;
#endif
    // Entities cannot go out of the navigable space?
    // Temporary allowed to disable containment in game for the demo so
    // it can be set/unset at last moment if performance requires it
    // (and even maybe in just one of the two levels of the demo)
    static bool forceContainment;

};

//------------------------------------------------------------------------------
/**
    SetMovEngineState
*/
inline
void
ncAIMovEngine::SetMovEngineState (int state)
{
    this->state = (eEngineState) state;
}

//------------------------------------------------------------------------------
/**
    @returns the movEngine state
*/
inline
int
ncAIMovEngine::GetMovEngineState() const
{
    return static_cast<int>( this->state );
}

//------------------------------------------------------------------------------
/**
    IsActioned
*/
inline
bool
ncAIMovEngine::IsActioned() const
{
    return this->IsMoving() || this->IsFacing() || this->IsJumping() || this->IsDropping();
}

//------------------------------------------------------------------------------
/**
    IsMoving
*/
inline
bool
ncAIMovEngine::IsMoving() const
{
    return bool (this->state == ST_RUNNING);
}

//------------------------------------------------------------------------------
/**
    IsTurning
*/
inline
bool
ncAIMovEngine::IsTurning() const
{
    return bool (this->state == ST_TURNING);
}

//------------------------------------------------------------------------------
/**
    IsFacing
*/
inline
bool
ncAIMovEngine::IsFacing() const
{
    return bool (this->state == ST_FACING);
}

//------------------------------------------------------------------------------
/**
    IsJumping
*/
inline
bool
ncAIMovEngine::IsJumping() const
{
    return bool (this->state == ST_JUMPING);
}

//------------------------------------------------------------------------------
/**
    IsDropping
*/
inline
bool
ncAIMovEngine::IsDropping() const
{
    return bool (this->state == ST_DROPPING);
}

//------------------------------------------------------------------------------
/**
    IsStopped
*/
inline
bool
ncAIMovEngine::IsStopped() const
{
    return bool (this->state == ST_STOPPED);
}

//------------------------------------------------------------------------------
/**
    IsPaused
*/
inline
bool
ncAIMovEngine::IsPaused() const
{
    return bool (this->state == ST_PAUSED);
}

//------------------------------------------------------------------------------
/**
    Get current stretch's goal
*/
inline
const vector3&
ncAIMovEngine::GetCurrStretchGoal() const
{
    return this->current.goal;
}

//------------------------------------------------------------------------------
/**
    GetGoal
*/
inline
const vector3&
ncAIMovEngine::GetGoal() const
{
    return this->goal;
}

//------------------------------------------------------------------------------
/**
    SetGoal
*/
inline
void
ncAIMovEngine::SetGoal (const vector3& goal)
{
    this->goal = goal;
}

//------------------------------------------------------------------------------
/**
    SetStart
*/
inline
void
ncAIMovEngine::SetStart (const vector3& start)
{
    this->start = start;
}

//------------------------------------------------------------------------------
/**
    SetStyle
*/
inline
void
ncAIMovEngine::SetStyle (int style)
{
    this->style = style;
}

//------------------------------------------------------------------------------
/**
    GetStyle
*/
inline
int
ncAIMovEngine::GetStyle ()
{
    return this->style;
}

//------------------------------------------------------------------------------
/**
    CanDoAction
*/
inline
bool
ncAIMovEngine::CanDoAction() const
{
    return !this->IsJumping() && !this->IsDropping();
}

//------------------------------------------------------------------------------
/**
    GetSpeed
*/
inline
float
ncAIMovEngine::GetSpeed() const
{
    return this->speed;
}

//------------------------------------------------------------------------------
/**
    SetSpeed
*/
inline
void
ncAIMovEngine::SetSpeed (float speed)
{
    this->speed = speed;
}

//------------------------------------------------------------------------------
/**
    Set the movement mode
*/
inline
void 
ncAIMovEngine::SetMovMode(int mode, float speedFactor)
{
    this->movMode = mode;
    this->speedFactor = speedFactor;
}

//------------------------------------------------------------------------------
/**
    Get the movement mode
*/
inline
int 
ncAIMovEngine::GetMovMode() const
{
    return this->movMode;
}

//------------------------------------------------------------------------------
/**
    Get the look ahead obstacles avoidance distance for current speed
*/
inline
float
ncAIMovEngine::GetAvoidanceDistance() const
{
    return this->avoidanceTime * this->GetSpeed() * this->speedFactor * 60.f;
}

//------------------------------------------------------------------------------
/**
    Set the time when the dynamic obstacles cells list must be updated
*/
inline
void
ncAIMovEngine::SetAvoidanceCellsUpdateTime( int time )
{
    this->avoidanceCellsUpdateTime = time;
}

//------------------------------------------------------------------------------
/**
    Get the time when the dynamic obstacles cells list must be updated
*/
inline
int
ncAIMovEngine::GetAvoidanceCellsUpdateTime() const
{
    return this->avoidanceCellsUpdateTime;
}

//------------------------------------------------------------------------------
#endif
