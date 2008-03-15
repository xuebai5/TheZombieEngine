#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  naimovengine.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ncaimovengine/ncaimovengine.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#include "npathfinder/npathfinder.h"
#include "zombieentity/nctransform.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncsoundlayer/ncsoundlayer.h"
#include "nnavmesh/nnavmesh.h"
#include "gfx2/ngfxserver2.h"
#include "nspatial/nspatialserver.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "entity/ncomponentobject.h"
#include "nspatial/ncspatialquadtree.h"
#include "nnavmesh/nnavutils.h"
#include "util/nrandomlogic.h"
#include "animcomp/nccharacter.h"

#include "nphysics/nphygeomray.h"
#include "nphysics/nphysicsserver.h"
#include "nspatial/ncspatial.h"
#include "ncaistate/ncaistate.h"
#include "ncnavnode/ncnavnode.h"

#include "napplication/napplication.h"
#include "variable/nvariableserver.h"

// @todo Remove when moving motion related time intervals out from the trigger server
#include "ntrigger/ntriggerserver.h"

#define EPSILON_FACING      0.1f
#define GRAVITY             9.8f
#define DOUBLEGRAVITY       19.6f
#define HALFGRAVITY         4.9f

//------------------------------------------------------------------------------
nNebulaComponentObject(ncAIMovEngine,nComponentObject);

//------------------------------------------------------------------------------
#ifndef NGAME
bool ncAIMovEngine::avoidObstacles( true );
bool ncAIMovEngine::useThrottle( false ); // Excluded for demo, but available to try
bool ncAIMovEngine::dontPenetrate( true );
bool ncAIMovEngine::displaceAnim( true );
bool ncAIMovEngine::constantTime( false );
bool ncAIMovEngine::smoothRotations( true );
#endif
bool ncAIMovEngine::forceContainment( true );

namespace
{
#ifdef __NEBULA_STATS__
    nProfiler profMotion;
    nProfiler profPathfinding;
    nProfiler profPathfollowing;
    nProfiler profAvoidance;
    nProfiler profDynamicAvoidance;
    nProfiler profStaticAvoidance;
    nProfiler profAntipenetration;
    nProfiler profContainment;
#endif

    // Three rays: left, middle, right
    const int StaticAvoidanceRaysNumber( 3 );
}

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncAIMovEngine::ncAIMovEngine() : 
    transform (0),
    animator (0),
    soundLayer (0),
    direction(MD_FORWARD),
    movMode(MOV_WALK),
    style (nPathFinder::PATH_STRAIGHT),
    deltaGameplay(0.f),
    avoidanceTurnInc(0.f),
    avoidanceThrottle(0.f),
    maxAvoidanceTurnInc(N_PI / 2),
    maxAvoidanceTurnAccel(N_PI / 2),
    maxAvoidanceThrottleAccel(2.0f),
    avoidanceTime(1.f),
    fallingVelocity(0),
    dispIndex( -1 ),
    dispFactor( 1,1,1 ),
    collisionExpected(false),
    useAnimDisplacement(false),
    state(ST_STOPPED),
    pausedState(ST_STOPPED)
{
#ifndef NGAME
    this->avoidanceRays.SetFixedSize( StaticAvoidanceRaysNumber );
#endif

#ifdef __NEBULA_STATS__
    if ( !profMotion.IsValid() )
    {
        profMotion.Initialize( "profAIMotion", true );
        profPathfinding.Initialize( "profAIMotionPathfinding", true );
        profPathfollowing.Initialize( "profAIMotionPathfollowing", true );
        profAvoidance.Initialize( "profAIMotionAvoidance", true );
        profDynamicAvoidance.Initialize( "profAIMotionDynamicAvoidance", true );
        profStaticAvoidance.Initialize( "profAIMotionStaticAvoidance", true );
        profAntipenetration.Initialize( "profAIMotionAntipenetration", true );
        profContainment.Initialize( "profAIMotionContainment", true );
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncAIMovEngine::~ncAIMovEngine()
{
    this->transform = 0;
}

//------------------------------------------------------------------------------
/**
    InitInstance
*/
void
ncAIMovEngine::InitInstance(nObject::InitInstanceMsg initType)
{
    n_assert(this->entityObject);

    if ( this->entityObject )
    {
        this->transform  = this->entityObject->GetComponent <ncTransform>();
        this->animator     = this->entityObject->GetComponent <ncLogicAnimator>();
        this->soundLayer = this->entityObject->GetComponent <ncSoundLayer>();

        this->speed = this->GetEntitySpeed();
        this->previousPosition = this->transform->GetPosition();
    }

    if (initType == nObject::ReloadedInstance)
    {
        this->Stop();
    }
}

//------------------------------------------------------------------------------
/**
    Run
*/
void
ncAIMovEngine::Run( const float deltaTime )
{
#ifdef __NEBULA_STATS__
    profMotion.StartAccum();
#endif

#ifndef NGAME
    static float FakeDelta( 0.05f );
    if ( this->constantTime )
    {
        this->deltaGameplay = FakeDelta;
    }
    else
#endif
    {
        this->deltaGameplay = deltaTime;
    }
    vector3 pos( this->transform->GetPosition() );

    if (deltaGameplay == 0.f)
    {
#ifdef __NEBULA_STATS__
        profMotion.StopAccum();
#endif
        return;
    }

    if (this->deltaGameplay > 0.3f)
    {
        this->deltaGameplay = 0.3f;
    }

    if ( this->IsActioned() )
    {
        this->UpdateEntity();
    }

    // Update height
    if( this->transform )
    {
        vector3 newPosition = this->transform->GetPosition();
        this->UpdateHeightSmoothly( newPosition );
        this->transform->SetPosition( newPosition );
    }

    this->previousPosition = pos;

#ifdef __NEBULA_STATS__
    profMotion.StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Jump
*/
bool
ncAIMovEngine::Jump()
{
    this->current.start = this->transform->GetPosition();

    this->UpdateDisplacementIndex();

    this->current.jumpHeight = 0.0f;

    this->SetMovEngineState (ST_JUMPING);

    return true; 
}

//------------------------------------------------------------------------------
/**
    DropTo
*/
bool
ncAIMovEngine::DropTo (const vector3& point)
{
    n_assert(this->transform);

    nPathFinder* pathFinder = nPathFinder::Instance();
    n_assert(pathFinder);
    bool valid = this->CanDoAction();

    if ( valid )
    {
        vector3 testPoint(point);
        nNavUtils::GetGroundHeight( testPoint, testPoint.y );
        valid = pathFinder->GetMesh()->GetNode (testPoint) != 0;
    }

    if ( valid )
    {
        this->current.start = this->transform->GetPosition();
        this->current.goal = point;
        this->current.jumpTime = 0.f;

        // Calculate the angle for the jump
        ncGameplayLivingClass* livingClass = this->GetClassComponentSafe<ncGameplayLivingClass>();
        float jumpSpeed = livingClass->GetJumpSpeed();
        this->current.jumpAngle = 0.f;
        this->current.direction = this->current.goal - this->current.start;
        this->current.direction.norm();
        this->current.xspeed = jumpSpeed;

        this->SetMovEngineState (ST_DROPPING);
    }

    return valid; 
}

//------------------------------------------------------------------------------
/**
    MoveToFacing
*/
bool 
ncAIMovEngine::MoveToFacing (const vector3& goal, const vector3& facingPoint)    
{
    n_assert(this->transform);
    vector3 start( this->transform->GetPosition() );
    vector3 finalGoal( goal );

    this->UpdateHeight (&finalGoal);
    this->current.facingPoint = facingPoint;
    this->current.facing = true;

    return this->Move (start, finalGoal);
}

//------------------------------------------------------------------------------
/**
    Move
*/
bool
ncAIMovEngine::MoveTo (const vector3& goal)
{
#ifdef __NEBULA_STATS__
    profMotion.StartAccum();
#endif

    n_assert(this->transform);
    vector3 start( this->transform->GetPosition() );
    vector3 finalGoal( goal );

    this->UpdateHeight (&finalGoal);
    this->current.facing = false;

    bool pathFound( this->Move(start, finalGoal) );

#ifdef __NEBULA_STATS__
    profMotion.StopAccum();
#endif

    return pathFound;
}

//------------------------------------------------------------------------------
/**
    MoveToZigZag
*/
bool
ncAIMovEngine::MoveToZigZag (const vector3& goal, const vector3& facingPoint, bool face, nArray<bool> &dirs)
{
    n_assert(this->transform);
    vector3 start(this->transform->GetPosition());
    vector3 finalGoal(goal);

    this->UpdateHeight (&finalGoal);
    if (face)
    {
        this->current.facingPoint = facingPoint;
        this->current.facing = true;
    }
    else
    {
        this->current.facing = false;
    }

    return this->MoveZigZag (start, finalGoal, dirs);
}

//------------------------------------------------------------------------------
/**
    Move
*/
bool
ncAIMovEngine::Move (const vector3& start, const vector3& goal)
{
    n_assert (nPathFinder::Instance());

    nPathFinder* pathFinder = nPathFinder::Instance();
    bool possible = false; 

    if ( this->CanDoAction() )
    {
#ifdef __NEBULA_STATS__
        profPathfinding.StartAccum();
#endif
        if ( pathFinder )
        {
            vector3 realStart( start );
            vector3 realGoal( goal );

//            realStart.y = realGoal.y = 0.f;
            this->UpdateHeight (&realStart);
            this->UpdateHeight (&realGoal);

            possible = pathFinder->FindPathStraight (realStart, 
                                                     realGoal, 
                                                     &this->path, 
                                                     this->entityObject);
        }
#ifdef __NEBULA_STATS__
        profPathfinding.StopAccum();
#endif

        if ( possible )
        {
            this->start = start;
            this->goal = goal;

            this->UpdateHeight (&this->start);
            this->UpdateHeight (&this->goal);

            vector3 oldDir( this->current.goal - this->current.start );
            oldDir.norm();

            this->GetCurrentStretch (start);

            // Adapt current obstacle avoidance steering to new movement direction
            // Turning and throttle avoidance amounts are decreased proportionally to angle between
            // new and old directions, to the point to be mirrored if directions are opposite
            vector3 newDir( this->current.goal - this->current.start );
            newDir.norm();
            float straight( oldDir % newDir );
            if ( straight < 0 )
            {
                this->avoidanceTurnDir = -this->avoidanceTurnDir;
                this->avoidanceThrottle = -this->avoidanceThrottle;
                straight = -straight;
            }
            this->avoidanceTurnInc *= straight;
            this->avoidanceThrottle *= straight;
        }
        else
        {
            this->Stop();
        }
    }
    
    return possible;
}

//------------------------------------------------------------------------------
/**
    Move ZigZag
*/
bool
ncAIMovEngine::MoveZigZag(const vector3& start, const vector3& goal, nArray<bool> &dirs)
{
    n_assert (nPathFinder::Instance());

    nPathFinder* pathFinder = nPathFinder::Instance();
    bool possible = false; 

    if ( this->CanDoAction() )
    {
/*        if ( goal.isequal (this->goal, N_TINY) )
        { 
            possible = false; 
        }
        else*/
        if ( pathFinder )
        {
            vector3 realStart( start );
            vector3 realGoal( goal );

//            realStart.y = realGoal.y = 0.f;
            this->UpdateHeight (&realStart);
            this->UpdateHeight (&realGoal);

            // @TODO: Ugly hack for try to hide a bug while trying debug another bug...
            // This do-while block must be deleted and test why pathfinder sometime fails.
            int i = 0;
            do 
            {
                possible = pathFinder->FindPathZigZag (realStart, 
                    realGoal, 
                    &this->path, 
                    dirs,
                    this->entityObject);
                i++;

            } while ( !possible && i < 3 );
		}

        if ( possible )
        {
            this->start = start;
            this->goal = goal;

            this->UpdateHeight (&this->start);
            this->UpdateHeight (&this->goal);

            this->GetCurrentStretch (start);  
        }    
        else
        {
            this->Stop();
        }
    }
    
    return possible;
}

//------------------------------------------------------------------------------
/**
    Stop
*/
void
ncAIMovEngine::Stop()
{
    this->state = ST_STOPPED;
    this->goal = this->transform->GetPosition();
    this->deltaGameplay = 0.f;
    this->dispIndex = -1;

    if ( this->soundLayer )
    {
        this->soundLayer->PlaySound (GP_ACTION_IDLE, false);
    }
}

//------------------------------------------------------------------------------
/**
    FaceTo
*/
bool
ncAIMovEngine::FaceTo (const vector3& position)
{
    n_assert(this->transform);
    const vector3 &start( this->transform->GetPosition() );
    vector3 finalGoal( position );

    nNavUtils::GetGroundHeight(finalGoal, finalGoal.y);

    quaternion direction = this->GetFinalDirection (start, finalGoal);
    direction.normalize();

    return this->Face(direction);
}

//------------------------------------------------------------------------------
/**
    Face
*/
bool
ncAIMovEngine::Face (const quaternion& direction)
{
    n_assert(this->transform);

    bool possible = this->CanDoAction();

    if ( possible )
    {
        if ( this->transform )
        {
            this->current.final = direction;
        }

        n_assert(this->entityObject);
        n_assert(this->animator);

        if ( this->entityObject )
        {    
            this->SetMovEngineState (ST_FACING);
            this->animator->SetRoll();

            // determine material under the player
            const char * materialName = soundLayer->GetMaterialNameUnder();

            if ( this->soundLayer )
            {
                this->soundLayer->PlaySoundMaterial (GP_ACTION_ROLL, materialName, true);
            }
        }
    }

    return possible;
}

//------------------------------------------------------------------------------
/**
    FindPoint
*/
int 
ncAIMovEngine::FindPoint (const vector3& point) const
{
    int index = -1;

    for ( int i=0; i<this->path.Size() && index == -1; i++ )
    {
        const vector3 &element( this->path[i] );

        //if ( element.isequal (point, FLT_EPSILON) )
//        if ( element.isequal (point, N_TINY) )
        if ( n_abs( element.x - point.x ) <= N_TINY && n_abs( element.z - point.z ) <= N_TINY )
        {
            index = i;
        }
    }

    return index;
}

//------------------------------------------------------------------------------
/**
    GetNextPoint
*/
vector3
ncAIMovEngine::GetNextPoint (const vector3& start) const
{
    //vector3 pathStart = vector3 (start.x, 0.f, start.z);
    vector3 next;
    int index = this->FindPoint (start/*pathStart*/);

    if ( this->path.Size() > 0 )
    {
        if ( index != -1 && index < this->path.Size() - 1 )
        {
            next = this->path[index+1];
        }

        if ( index == this->path.Size() - 1 )
        {
            next = this->path[this->path.Size()-1];
        }
    }
    else
    {
        n_assert_always();
        next = this->start;
    }

    return next;
}

//------------------------------------------------------------------------------
/**
    GetCurrentStretch
*/
void
ncAIMovEngine::GetCurrentStretch (const vector3& position)
{
    // First, pick up the current segment
    this->current.start = position;
//    this->current.start.y = 0;
    this->UpdateHeight( &this->current.start );
    this->current.goal  = this->GetNextPoint (position);
//    this->current.goal.y = 0;
    this->UpdateHeight( &this->current.goal );
    this->SetMovEngineState (ST_RUNNING);

    // Set the goal plane as the plane perpendicular to start position
    vector3 planeNorm( this->current.start - this->current.goal );
    planeNorm.y = 0;
    planeNorm.norm();
    vector3 goal( this->current.goal );
    goal.y = 0;
    float d( planeNorm.x * goal.x + planeNorm.y * goal.y + planeNorm.z * goal.z );
    this->current.goalPlane.set( planeNorm.x, planeNorm.y, planeNorm.z, -d );

    const float MinAgentSpeed = nVariableServer::Instance()->GetFloatVariable("MinAgentsSpeed");
    this->current.maxTimeToGoal = nTimeServer::Instance()->GetTime() +
        ( this->current.goal - this->current.start ).len() / MinAgentSpeed;

    //// Now, prepare the turning angle to the next segment
    //this->CalculateTurnPosition();
}

////------------------------------------------------------------------------------
///**
//    CalculateTurnPosition
//*/
//void
//ncAIMovEngine::CalculateTurnPosition()
//{
//    n_assert(this->entityObject);
//    n_assert(this->animator);
//
//    if ( this->entityObject )
//    {    
//        this->SetMovEngineState (ST_RUNNING);
//        this->animator->SetWalk();
//
//        if ( this->soundLayer )
//        {
//            this->soundLayer->PlaySound (GP_ACTION_WALK, true);
//        }
//    }
//}

//------------------------------------------------------------------------------
/**
    UpdateEntity
*/
void
ncAIMovEngine::UpdateEntity()
{
    n_assert(this->transform);
    if ( this->transform )
    {
        switch (this->state)
        {
        case ST_RUNNING:    // Movement
            if ( this->RunMovement() )
            {
                this->Stop();
            }
            break;

        case ST_FACING:    // Facing
            if ( this->RunFacing() )
            {
                this->Stop();
            }
            break;

        case ST_DROPPING:   // Free fallin'
            if ( this->RunDropping() )
            {
                this->Stop();
            }
            break;

        case ST_JUMPING:    // Jump!
            if ( this->RunJumping() )
            {
                this->Stop();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    RunMovement
*/
bool
ncAIMovEngine::RunMovement()
{
#ifdef __NEBULA_STATS__
    profPathfollowing.StartAccum();
#endif

    n_assert(this->transform);

    bool done = false;

    if ( this->transform )
    {    
        vector3 position( this->transform->GetPosition() );

        // If is necessary, smooth the path
        this->SmoothCurrentPath();

        while ( this->IsAtEndStretch() && !this->IsAtEnd() )
        {
            this->GetCurrentStretch (this->current.goal);
        }

        if ( !this->IsAtEnd() )    
        {
            int animStateIndex = 0;
            if ( this->animator )
            {
                switch (this->movMode)
                {
                    case MOV_WALK:
                        //empty
                    case MOV_WALKPRONE:
                        animStateIndex = this->animator->SetWalk();
                        this->animator->SetSpeedFactor(this->animator->GetStateIndex(), this->speedFactor);
                        this->animator->SetSpeedFactor(this->animator->GetUpperStateIndex(), this->speedFactor);
                        break;
        
                    case MOV_RUN:
                        animStateIndex = this->animator->SetRun();
                        this->animator->SetSpeedFactor(this->animator->GetStateIndex(), this->speedFactor);
                        this->animator->SetSpeedFactor(this->animator->GetUpperStateIndex(), this->speedFactor);
                        break;
                }
            }

            this->UpdateDisplacementIndex();

            quaternion direction;

            this->GetSituation (&position, &direction);

            this->transform->SetQuat (direction);        
            this->transform->SetPosition (position);

            // play movement sound depending on animation duration and steps per animation cycle
            if ( this->soundLayer && this->animator && animator->IsValidStateIndex(animStateIndex))
            {
                float duration = this->animator->GetComponentSafe<ncCharacter>()->GetStateDuration( animStateIndex, false);
                int stepsPerAnimCycle = this->GetClassComponentSafe<ncGameplayLivingClass>()->GetStepsAnimCycle();
                
                // determine material under the player
                const char * materialName = soundLayer->GetMaterialNameUnder();

                if ( stepsPerAnimCycle == 0 )
                {
                    switch (this->movMode)
                    {
                        case MOV_WALK:
                            this->soundLayer->PlaySoundTimeMaterial (GP_ACTION_WALK, 0, materialName, duration, true);
                            break;
            
                        case MOV_RUN:
                            this->soundLayer->PlaySoundTimeMaterial (GP_ACTION_RUN, 0, materialName, duration, true);
                            break;

                        case MOV_WALKPRONE:
                            this->soundLayer->PlaySoundTimeMaterial (GP_ACTION_WALKPRONE, 0, materialName, duration , true);
                            break;
                    }
                }
                else
                {
                    switch (this->movMode)
                    {
                        case MOV_WALK:
                            this->soundLayer->PlaySoundTimeMaterial (GP_ACTION_WALK, 0, materialName, duration / stepsPerAnimCycle, true);
                            break;
            
                        case MOV_RUN:
                            this->soundLayer->PlaySoundTimeMaterial (GP_ACTION_WALK, 0, materialName, duration / stepsPerAnimCycle, true);
                            break;

                        case MOV_WALKPRONE:
                            this->soundLayer->PlaySoundTimeMaterial (GP_ACTION_WALKPRONE, 0, materialName, duration / stepsPerAnimCycle, true);
                            break;
                    }
                }
            }
        }
        else
        {
            done = true;
        }

        // Recalculate path to goal if the next subgoal hasn't been reached in some more time than expected
        if ( !done && nTimeServer::Instance()->GetTime() > this->current.maxTimeToGoal )
        {
            if ( !this->Move( this->transform->GetPosition(), this->goal ) )
            {
                // Path to goal no longer possible, so finish movement
                done = true;
            }
        }
    }
    else
    {
        done = true;
    }

#ifdef __NEBULA_STATS__
    profPathfollowing.StopAccum();
#endif

    return done;
}

//------------------------------------------------------------------------------
/**
    RunFacing
*/
bool
ncAIMovEngine::RunFacing()
{
    n_assert(this->transform);    
    bool done = false;
    quaternion current;

    if ( this->transform )
    {
        current = this->transform->GetQuat();
        current.norm();
    }

    if ( !this->current.final.isequal (current, 0.01f) )
    {
        quaternion increase;

        increase.slerp (current, this->current.final, 0.5f);
        this->transform->SetQuat (increase);
    }
    else
    {
        done = true;
    }

    return done;
}

//------------------------------------------------------------------------------
/**
    RunDropping
*/
bool
ncAIMovEngine::RunDropping()
{
    n_assert(this->transform);

    vector3 position( this->transform->GetPosition() );
    bool done = this->current.jumpTime > 0 && this->IsAtFloor (position);

    this->current.jumpTime += this->deltaGameplay;

    if ( !done )
    {
        position = this->current.start;
        vector3 inc( this->current.direction );
        inc *= (this->current.xspeed * this->current.jumpTime); // v0x * t

        // Calculate height
        inc.y = -(GRAVITY * this->current.jumpTime * this->current.jumpTime * 0.5f);
                                    
        position += inc;

        if ( this->IsAtFloor (position) )
        {
            done = true;
        }
    }
    
    if ( done )
    {
        this->UpdateHeight (&position);
    }

    this->transform->SetPosition (position);

    return done;
}

//------------------------------------------------------------------------------
/**
    RunJumping
*/
bool
ncAIMovEngine::RunJumping()
{
    n_assert(this->transform);

    bool valid = this->useAnimDisplacement && this->transform;

    if( valid )
    {
        vector3 position(this->transform->GetPosition());

        quaternion direction(this->transform->GetQuat());

        vector3 displacement;
        this->GetAnimDisplacement( displacement );

        if (displacement.isequal(vector3(0.f, 0.f, 0.f), N_TINY) && this->animator->HasFinished(this->dispIndex))
        {
            return true;
        }

        displacement = direction.rotate( displacement );

        this->current.jumpHeight += ( displacement.y > 0 ) ? displacement.y : 0;

        position += displacement;

        if( this->current.jumpHeight > 0.1f && displacement.y < 0 && this->IsAtFloor( position ) )
        {
            nNavUtils::GetGroundHeight( position, position.y );
            valid = false;
        }

        this->transform->SetPosition( position );

        if( valid && this->animator->HasFinished( this->dispIndex ) )
        {
            valid = false;
        }
    }

    return !valid;
}

//------------------------------------------------------------------------------
/**
    GetSituation

    @brief Calculates the current situation for the unit, this is the position    
    and final orientation (direction)
*/
void 
ncAIMovEngine::GetSituation (vector3* position, quaternion* direction)
{
    n_assert(position && direction && this->transform);

    if ( position && direction && this->transform)
    {
        vector3 currentPos( this->transform->GetPosition() );

        // Check if we've reached the end of the curve
        if ( this->IsAtEndCurve() )
        {
            this->SetMovEngineState (ST_RUNNING);
        }

        vector3 step( 0.0f, 0.0f, 0.0f );

        if ( ! this->IsTurning() )
        {
            // calculate speed using animation
            float increment;
            if( this->useAnimDisplacement N_IFNDEF_NGAME( && this->displaceAnim ) )
            {
                vector3 animDisplacement;
                this->GetAnimDisplacement( animDisplacement );
                increment = animDisplacement.len();
            }
            else
            {
//                currentPos.y = this->current.goal.y;
                increment = this->GetSpeed() * this->deltaGameplay * 60.f;
            }

            increment *= this->speedFactor;
            step = this->current.goal - currentPos;
            step.y = 0;
            step.norm();
            step *= increment;

            vector3 finalDirection;

            if ( this->current.facing )
            {
                finalDirection = this->current.facingPoint;
            }
            else
            {
                finalDirection = this->current.goal;
            }

            *direction = this->GetFinalDirection (currentPos, finalDirection);
        }

#ifdef __NEBULA_STATS__
        profPathfollowing.StopAccum();
#endif

        this->AvoidObstacles( step, *direction );
        this->AvoidPenetration( step );
        this->ForceContainment( step );

#ifdef __NEBULA_STATS__
        profPathfollowing.StartAccum();
#endif

        *position = step + currentPos;

        bool valid = this->AdjustPosition (position);

        // Avoid abrupt orientation changes
        this->SmoothRotation( *direction );

        if ( valid )
        {
            this->entityAreaColor = vector4(0.f, 1.f, 0.5f, 1.f);
        }
        else
        {
            this->entityAreaColor = vector4(0.7f, 0.f, 0.5f, 1.f);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Set the direction
*/
void 
ncAIMovEngine::SetMovDirection(int dir)
{
    if ( this->animator )
    {
        this->animator->SetMovementPose(dir);
    }
}

//------------------------------------------------------------------------------
/**
    GetFinalDirection
*/
quaternion
ncAIMovEngine::GetFinalDirection (const vector3& position, const vector3& finalPos) const
{
    vector3 finalDirection( finalPos - position );
    float angle = 0.f;
    float sinAngle, cosAngle;

    finalDirection.y = 0.f;

    if ( finalDirection.z != 0.f )
    {
        angle = n_atan (finalDirection.x / finalDirection.z);
    }
    else
    {
        if( finalDirection.x > 0)
        {
            angle = N_PI * 0.5f;
        }
        else
        {
            angle = -N_PI * 0.5f;
        }
    }

    if ( finalDirection.z < 0 )
    {
        angle -= N_PI;
    }

    sinAngle = n_sin (angle*0.5f);
    cosAngle = n_cos (angle*0.5f);

    return quaternion (0, sinAngle, 0, cosAngle);
}

//------------------------------------------------------------------------------
/**
    GetMoveDirection
*/
void 
ncAIMovEngine::GetMoveDirection( vector3& dir ) const
{
    dir = this->transform->GetPosition() - this->previousPosition;
    dir.norm();
}

//------------------------------------------------------------------------------
/**
    AvoidObstacles
*/
void
ncAIMovEngine::AvoidObstacles( vector3& moveStep, quaternion& orientation )
{
#ifdef __NEBULA_STATS__
    profAvoidance.StartAccum();
#endif

#ifndef NGAME
    if ( this->avoidObstacles )
#endif
    {
        // Steer to avoid obstacles using correction from previous frame
        this->ApplyAvoidanceSteering( moveStep, orientation );

        // Calculate steer correction for next frame
        vector3 moveDir( moveStep );
        moveDir.norm();
        this->collisionExpected = false;
        this->distToCollisionSquared = this->GetAvoidanceDistance() * this->GetAvoidanceDistance();
        this->AvoidStaticObstacles( moveDir );
        this->AvoidDynamicObstacles( moveDir );
    }

#ifdef __NEBULA_STATS__
    profAvoidance.StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    ApplyAvoidanceSteering
*/
void
ncAIMovEngine::ApplyAvoidanceSteering( vector3& moveStep, quaternion& orientation )
{
    // Adapt turn increment to avoid the expected obstacle
    if ( this->collisionExpected )
    {
        // If a collision is expected, try to avoid it by turning
        // Turn magnitude is proportional (squared) to collision proximity
        float avoidanceDist( this->GetAvoidanceDistance() );
        float proximity( 1 - (this->distToCollisionSquared / (avoidanceDist*avoidanceDist)) );
        float turnInc( this->avoidanceTurnDir * proximity * this->maxAvoidanceTurnAccel * this->deltaGameplay );
        this->avoidanceTurnInc += turnInc;

        // Prevent overturning
        if ( this->avoidanceTurnInc > this->maxAvoidanceTurnInc )
        {
            this->avoidanceTurnInc = this->maxAvoidanceTurnInc;
        }
        else if ( this->avoidanceTurnInc < -this->maxAvoidanceTurnInc )
        {
            this->avoidanceTurnInc = -this->maxAvoidanceTurnInc;
        }

        // Also, try to avoid collision by accelerating/braking
        // Throttle increment is proportional (squared) to collision poximity
        if ( this->avoidanceThrottleDir < 0 )
        {
            this->avoidanceThrottleDir *= -this->avoidanceThrottleDir * proximity;
        }
        float throttleInc( this->avoidanceThrottleDir * proximity * this->maxAvoidanceThrottleAccel * this->deltaGameplay );
        this->avoidanceThrottle += throttleInc;

        // Prevent overthrotting
        if ( this->avoidanceThrottle > 1 )
        {
            this->avoidanceThrottle = 1;
        }
        else if ( this->avoidanceThrottle < -1 )
        {
            this->avoidanceThrottle = -1;
        }
    }
    else
    {
        // Otherwise, return to desired movement vector
        // "De-turn" at the maximum turn acceleration allowed
        float turnInc( this->maxAvoidanceTurnAccel * this->deltaGameplay );
        if ( this->avoidanceTurnInc > 0 )
        {
            this->avoidanceTurnInc -= turnInc;

            // Prevent flicking
            if ( this->avoidanceTurnInc < 0 )
            {
                this->avoidanceTurnInc = 0;
            }
        }
        else if ( this->avoidanceTurnInc < 0 )
        {
            this->avoidanceTurnInc += turnInc;

            // Prevent flicking
            if ( this->avoidanceTurnInc > 0 )
            {
                this->avoidanceTurnInc = 0;
            }
        }

        // Also, "de-throttle" at the maximum throttle acceleration allowed
        float throttleInc( this->maxAvoidanceThrottleAccel * this->deltaGameplay );
        if ( this->avoidanceThrottle > 0 )
        {
            this->avoidanceThrottle -= throttleInc;

            // Prevent flicking
            if ( this->avoidanceThrottle < 0 )
            {
                this->avoidanceThrottle = 0;
            }
        }
        else if ( this->avoidanceThrottle < 0 )
        {
            this->avoidanceThrottle += throttleInc;

            // Prevent flicking
            if ( this->avoidanceThrottle > 0 )
            {
                this->avoidanceThrottle = 0;
            }
        }
    }

    // Rotate the given movement vector to avoid expected obstacles
    quaternion turn;
    turn.set_rotate_y( this->avoidanceTurnInc );
    moveStep = turn.rotate( moveStep );

    // Rotate orientation to match movement turn if the entity is looking forward
    if ( !this->current.facing )
    {
        orientation *= turn;
    }

    // Shorten/enlarge movement vector to avoid expected obstacles
#ifndef NGAME
    if ( this->useThrottle )
#endif
#ifndef NGAME
    // Excluded from demo because "it looks ugly" to have agents breaking to near stop
    // and still moving legs like mad
    {
        moveStep *= 1 + this->avoidanceThrottle * 0.9f;
    }
#endif
}

//------------------------------------------------------------------------------
/**
    AvoidStaticObstacles
*/
void
ncAIMovEngine::AvoidStaticObstacles( const vector3& moveDir )
{
#ifdef __NEBULA_STATS__
    profStaticAvoidance.StartAccum();
#endif

    // Get the side vector
    quaternion turn;
    turn.set_rotate_y( N_PI / 2 );
    vector3 leftDir( turn.rotate( moveDir ) );

    // Approximate the agent as a 2D disk, whose radius is its gameplay body size
    float boundaryRadius( this->GetBodySize( this->GetEntityObject() ) );

    // Calculate the avoidance rays origins
    const float BottomHeight = 0.1f;
    const vector3& center( this->transform->GetPosition() );
    vector3 origin[ StaticAvoidanceRaysNumber ];
    // ray from bottom-center
    origin[0] = center + moveDir * boundaryRadius;
    origin[0].y += BottomHeight;
    // ray from bottom-left
    origin[1] = center + leftDir * boundaryRadius;
    origin[1].y += BottomHeight;
    // ray from bottom-right
    origin[2] = center - leftDir * boundaryRadius;
    origin[2].y += BottomHeight;

    // Calculate the ray lengths for the desired look ahead obstacle avoidance
    float avoidanceDistance( this->GetAvoidanceDistance() );

    // Look for the closest collision for all the avoidance rays
    for ( int i(0); i < StaticAvoidanceRaysNumber; ++i )
    {
#ifndef NGAME
        // Store avoidance rays for debug purposes
        this->avoidanceRays[i].set( origin[i], origin[i] + moveDir * avoidanceDistance );
#endif

        // Get potential obstacle contacts
        const int MaxContactsPerRay( 10 );
        nPhyCollide::nContact contact[ MaxContactsPerRay ];
        int contactsNumber( nPhysicsServer::Collide( origin[i], moveDir, avoidanceDistance,
            MaxContactsPerRay, contact, nPhysicsGeom::Dynamic ) );

        // Look for a closer contact point than any of the ones found so far by previous rays
        // (itself and the target entity shoudln't be considered as obstacles)
        for ( int c(0); c < contactsNumber; ++c )
        {
            vector3 contactPos;
            contact[c].GetContactPosition( contactPos );
            float distSquared( (contactPos - origin[i]).lensquared() );
            if ( distSquared < this->distToCollisionSquared )
            {
                // Collision distance
                this->collisionExpected = true;
                this->distToCollisionSquared = distSquared;

                // Get collider center
                nPhysicsGeom* collider( contact[c].GetGeometryA() );
                n_assert( collider );
                if ( collider->Type() == nPhysicsGeom::Ray )
                {
                    collider = contact[c].GetGeometryB();
                    n_assert( collider );
                }
                vector3 colliderCenter;
                collider->GetAbsolutePosition( colliderCenter );

                // Expected best turn direction to prevent collision
                vector3 centersVector( colliderCenter - center );
                if ( leftDir % centersVector >= 0 )
                {
                    this->avoidanceTurnDir = -1;
                }
                else
                {
                    this->avoidanceTurnDir = 1;
                }

#ifndef NGAME
                this->expectedCollisionPoint = contactPos;
#endif
            }
        }
    }

#ifdef __NEBULA_STATS__
    profStaticAvoidance.StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    AvoidDynamicObstacles
*/
void
ncAIMovEngine::AvoidDynamicObstacles( const vector3& moveDir3D )
{
#ifdef __NEBULA_STATS__
    profDynamicAvoidance.StartAccum();
#endif

    // Get near mobile entities
    nArray<nEntityObject*> entities;
    int updateInterval = nTriggerServer::Instance()->GetAvoidanceCellsUpdateInterval();
    this->avoidanceCellsUpdateTime = (this->avoidanceCellsUpdateTime + 1) % updateInterval;
    this->GetNearDynamicObstacles( this->avoidanceCellsUpdateTime == 0, entities );

    // Look for the sooner expected collision
    nEntityObject* self( this->GetEntityObject() );
    nEntityObject* target( this->GetComponentSafe<ncAIState>()->GetTarget() );
    for ( int i(0); i < entities.Size(); ++i )
    {
        nEntityObject* entity( entities[i] );

        // Skip self and target entity
        if ( entity == self || entity == target )
        {
            continue;
        }

        // -- Check if the entity is in a collision route

        // Approximate both entities as a 2D disk, whose radius is their gameplay body size
        float radiuses( ( this->GetBodySize(this->GetEntityObject()) + this->GetBodySize(entity) ) );

        // Get the relative movement for the desired look ahead avoidance
        vector2 moveDir1( moveDir3D.x, moveDir3D.z );
        vector2 relMove( moveDir1 * this->GetAvoidanceDistance() );
        ncAIMovEngine* moveEngine( entity->GetComponent<ncAIMovEngine>() );
        vector2 moveDir2( 0, 0 );
        if ( moveEngine )
        {
            vector3 dir;
            moveEngine->GetMoveDirection( dir );
            moveDir2 = vector2( dir.x, dir.z );
            relMove -= moveDir2 * moveEngine->GetAvoidanceDistance();
        }

        // If there's no relative movement, there's no need for steering
        float relMoveLength( relMove.len() );
        if ( relMoveLength <= TINY )
        {
            continue;
        }

        // Get closest approach of both entities along the relative movement direction
        vector3 center3D( this->transform->GetPosition() );
        vector2 center1( center3D.x, center3D.z );
        center3D = entity->GetComponentSafe<ncTransform>()->GetPosition();
        vector2 center2( center3D.x, center3D.z );
        vector2 centersVector( center2 - center1 );
        vector2 relDir( relMove );
        relDir.norm();
        float distToClosestPoint( relDir % centersVector );
        if ( distToClosestPoint <= 0 )
        {
            // Entities are separating, skip this pair
            continue;
        }
        else if ( distToClosestPoint >= relMoveLength + radiuses )
        {
            // Entities are far away, skip this pair
            continue;
        }
        vector2 closestPoint( center1 + relDir * distToClosestPoint );

        // Use the the closest approach point as an approximation of the collision proximity
        float closestDist( (closestPoint - center2).len() );
        float penetration( radiuses - closestDist );
        if ( penetration <= 0 )
        {
            continue;
        }

        // Collision distance
        float approxDistToCollision( distToClosestPoint - radiuses );
        float distSquared( approxDistToCollision * approxDistToCollision );
        if ( distSquared >= this->distToCollisionSquared )
        {
            continue;
        }

        // -- Sooner collision so far -> calculate the new obtacle avoidance steering

        this->collisionExpected = true;
        this->distToCollisionSquared = distSquared;

        // Get the side vector
        vector2 leftDir1( moveDir1 );
        leftDir1.rotate( -N_PI / 2 );

        // Expected best turn direction to prevent collision
        if ( leftDir1 % centersVector >= 0 )
        {
            this->avoidanceTurnDir = -1;
        }
        else
        {
            this->avoidanceTurnDir = 1;
        }

        // Calculate throttle/brake amount to try to avoid collision
        if ( relMoveLength > TINY )
        {
            // Approximate the estimated position where entities will collide if no steering is applied
            float approxCollisionTime( approxDistToCollision / relMoveLength );
            vector2 futurePos1( center1 + moveDir1 * approxCollisionTime );
            vector2 futurePos2( center2 + moveDir2 * approxCollisionTime );

            // Accelerate if the collision is expected at the back
            // Brake if the collision is expected at the front
            vector2 futureCentersDir( futurePos1 - futurePos2 );
            futureCentersDir.norm();
            this->avoidanceThrottleDir = futureCentersDir % moveDir1;
        }

#ifndef NGAME
        // Gather some data for debug purposes
        vector2 collisionPoint( center1 + relDir * ( distToClosestPoint - radiuses ) );
        this->expectedCollisionPoint = vector3( collisionPoint.x, this->transform->GetPosition().y, collisionPoint.y );
#endif
    }

#ifdef __NEBULA_STATS__
    profDynamicAvoidance.StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    Get the dynamic obstacles near this entity
*/
void
ncAIMovEngine::GetNearDynamicObstacles( bool updateCells, nArray<nEntityObject*>& entities )
{
    // Build proximity bounding
    float avoidanceDist( this->GetAvoidanceDistance() );
    float height( this->GetComponentSafe<ncSpatial>()->GetBBox().extents().y );
    bbox3 bbox( this->transform->GetPosition(), vector3(avoidanceDist, height, avoidanceDist) );

    // Get near dynamic obstacles
    if ( updateCells )
    {
        // Get spatial cells too to speed up future queries
        this->avoidanceCells.Clear();
        nSpatialServer::Instance()->GetEntitiesCellsCategory(
            bbox, nSpatialTypes::CAT_AGENTS, nSpatialTypes::SPF_OUTDOORS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_POSITION,
            entities, this->avoidanceCells );
        nSpatialServer::Instance()->GetEntitiesCellsCategory(
            bbox, nSpatialTypes::CAT_AGENTS, nSpatialTypes::SPF_ALL_INDOORS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_POSITION,
            entities, this->avoidanceCells );
    }
    else if ( !this->avoidanceCells.Empty() )
    {
        // Search only in spatial cells found on previous queries
        nSpatialServer::Instance()->GetEntitiesUsingCellsCategory(
            bbox, nSpatialTypes::CAT_AGENTS, nSpatialTypes::SPF_OUTDOORS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_POSITION,
            entities, this->avoidanceCells );
        nSpatialServer::Instance()->GetEntitiesUsingCellsCategory(
            bbox, nSpatialTypes::CAT_AGENTS, nSpatialTypes::SPF_ALL_INDOORS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_POSITION,
            entities, this->avoidanceCells );
    }
}

//------------------------------------------------------------------------------
/**
    AvoidPenetration
*/
void
ncAIMovEngine::AvoidPenetration( vector3& moveStep )
{
#ifdef __NEBULA_STATS__
    profAntipenetration.StartAccum();
#endif

#ifndef NGAME
    if ( this->dontPenetrate )
#endif
    {
        // Approximate entity as a sphere whose radius is its gameplay body size
        // Place the sphere where it's expected to be if no penetration is found
        const vector3 pos( this->transform->GetPosition() + moveStep );
        float radius( this->GetBodySize( this->GetEntityObject() ) );
        sphere s( pos, radius );

        // Get mobile entities that occupy the next place where to go
        nArray<nEntityObject*> entities;
        nSpatialServer::Instance()->GetEntitiesCategory( s, nSpatialTypes::CAT_AGENTS,
            nSpatialTypes::SPF_OUTDOORS | nSpatialTypes::SPF_INTERSECTING |
            nSpatialTypes::SPF_USE_BBOX, entities );
        nSpatialServer::Instance()->GetEntitiesCategory( s, nSpatialTypes::CAT_AGENTS,
            nSpatialTypes::SPF_ALL_INDOORS | nSpatialTypes::SPF_INTERSECTING |
            nSpatialTypes::SPF_USE_BBOX, entities );

        // Push the movement vector out of any penetration
        nEntityObject* self( this->GetEntityObject() );
        for ( int i(0); i < entities.Size(); ++i )
        {
            nEntityObject* entity( entities[i] );

            // Skip itself
            if ( entity == self )
            {
                continue;
            }

            // Approximate entity as a sphere whose radius is its gameplay body size
            float radius2( this->GetBodySize(entity) );
            vector3 pos2( entity->GetComponentSafe<ncTransform>()->GetPosition() );

            // Skip non penetrating entities
            float centersDistSquared( (pos - pos2).lensquared() );
            float radiuses( radius + radius2 );
            float radiusesSquared( radiuses * radiuses );
            float penetrationSquared = radiusesSquared - centersDistSquared;
            if ( penetrationSquared <= 0 )
            {
                continue;
            }

            // Push the movement vector out of penetration
            // Push force is proportional to penetration amount
            // @todo Consider all the simulatenous penetrations and make some push average instead of pushing iteratively
            // @todo Push towards the movement direction instead of centers direction
            vector3 centersDir( pos - pos2 );
            centersDir.norm();
            float penetration( penetrationSquared / radiusesSquared );
//            const float antipenetrationStrength( 0.2f ); // 1 = push completely out of penetration in a single frame, but may cause visible flickering
//            moveStep = moveStep * ( 1 - penetration ) + ( centersDir * radiuses ) * penetration * antipenetrationStrength;
            if ( ( pos2 - this->transform->GetPosition() ) % moveStep <= 0 )
            {
                // Skip if already going away from penetration
                continue;
            }
            vector3 pushOut( centersDir * radiuses * penetration );
            if ( pushOut.lensquared() > moveStep.lensquared() )
            {
                // Prevent flickering (but not entirely, it's still visible if radiuses are big)
                pushOut.norm();
                pushOut *= moveStep.len();
            }
            moveStep += pushOut;
        }
    }

#ifdef __NEBULA_STATS__
    profAntipenetration.StopAccum();
#endif
}

//------------------------------------------------------------------------------
/**
    ForceContainment
*/
void
ncAIMovEngine::ForceContainment( vector3& moveStep )
{
#ifdef __NEBULA_STATS__
    profContainment.StartAccum();
#endif

//#ifndef NGAME
    if ( this->forceContainment )
//#endif
    {
        nNavMesh* navMesh( nPathFinder::Instance()->GetMesh() );
        n_assert( navMesh );

        vector3 pos( this->transform->GetPosition() );
        if ( !navMesh->GetNode( pos ) )
        {
            // Already out of the navigation mesh, let motion continue to try to reenter
            goto exit;
        }

        vector3 targetPos( pos + moveStep );
        line3 moveRay( pos, targetPos );
        vector3 crossPoint;
        vector3 crossNormal;
        if ( !navMesh->GetBoundaryPoint( moveRay, crossPoint, crossNormal ) )
        {
            // Not leaving the navegable space, so move freely
            goto exit;
        }

        // Crossing the boundary of the navigation mesh -> Steer parallely to the boundary,
        // but still containing the movement within the navegable space
        vector3 sideDir( crossNormal );
        sideDir.rotate( vector3(0,1,0), N_HALFPI * 1.1f );
        // Follow the previous movement direction
        float previousDir( ( pos - this->previousPosition ) % moveStep > 0 ? 1.0f : -1.0f );
        float dir( moveStep % sideDir > 0 ? previousDir : -previousDir );
        moveStep = sideDir * moveStep.len() * dir;
        // If the steer movement is also leaving the navegable space, flip movement direction
        moveRay.set( pos, pos + moveStep );
        if ( navMesh->GetBoundaryPoint( moveRay, crossPoint, crossNormal ) )
        {
            moveStep *= -1;
            // If even the reversed steering leaves the navegable space, stop movement
            moveRay.set( pos, pos + moveStep );
            if ( navMesh->GetBoundaryPoint( moveRay, crossPoint, crossNormal ) )
            {
                moveStep *= 0;
            }
        }
    }

exit:

#ifdef __NEBULA_STATS__
    profContainment.StopAccum();
#else
    // this is to make the compiler compile
    ;
#endif
}

//------------------------------------------------------------------------------
/**
    SmoothRotation
*/
void
ncAIMovEngine::SmoothRotation( quaternion& orientation )
{
#ifndef NGAME
    if ( ncAIMovEngine::smoothRotations )
#endif
    {
        // Get the max allowed orientation change
        ncGameplayLivingClass* gameplay( this->GetClassComponent<ncGameplayLivingClass>() );
        if ( !gameplay )
        {
            return;
        }
        const float maxAngularVelocity( gameplay->GetMaxAngularVelocity() );

        // Get the desired angle increment over the current orientation
        quaternion oldOrientation( this->transform->GetQuat() );
        float angleInc( n_abs( acos( oldOrientation.dot(orientation) ) ) );

        // Clip the orientation change to the max allowed orientation angle
        if ( angleInc > TINY )
        {
            float maxAngle( maxAngularVelocity * this->deltaGameplay );
            if ( maxAngle < angleInc )
            {
                orientation.slerp( oldOrientation, orientation, maxAngle / angleInc );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    IsAtTheEndCurve
*/
bool
ncAIMovEngine::IsAtEndCurve() const
{
    n_assert(this->entityObject);

    bool end = false;

    if ( this->entityObject )
    {
        // @TODO
    }

    return end;
}

//------------------------------------------------------------------------------
/**
    IsAtTheEndStretch
*/
bool
ncAIMovEngine::IsAtEndStretch() const
{
    n_assert(this->transform);
    bool end = true;

    if ( this->transform )
    {
        const vector3& position( this->transform->GetPosition() );
/*        float epsilon = this->speed * this->deltaGameplay * 30.f; 

        end = n_abs (position.x - this->current.goal.x) < epsilon &&
              n_abs (position.z - this->current.goal.z) < epsilon;*/

        /*if ( !end )
        {*/
        // When goal plane is crossed the current goal is considered to be reached
        // This prevents circling on a goal point when there's an obstacle on it
        vector3 pos2d( position.x, 0, position.z );
        end = this->current.goalPlane.distance( pos2d ) < this->GetBodySize( this->GetEntityObject() );
        //}
    }

    return end;
}

//------------------------------------------------------------------------------
/**
    GetEntitySpeed
*/
float
ncAIMovEngine::GetEntitySpeed() const
{
    n_assert (this->entityObject);

    float speed = 0.f;

    if ( this->entityObject )
    {
        ncGameplayLivingClass* gpClass = this->entityObject->GetEntityClass()->GetComponent <ncGameplayLivingClass>();
        n_assert(gpClass);

        if ( gpClass )
        {
            speed = gpClass->GetSpeed();
        }
    }

    return speed;
}

//------------------------------------------------------------------------------
/**
    GetJumpRange
*/
float
ncAIMovEngine::GetJumpRange() const
{
    n_assert(this->entityObject);

    float range = 0.f;

    if ( this->entityObject )
    {
        ncGameplayLivingClass* gpClass = this->entityObject->GetEntityClass()->GetComponent <ncGameplayLivingClass>();
        n_assert(gpClass);

        if ( gpClass )
        {
            range = gpClass->GetJumpHeight();
        }
    }

    return range;
}

//------------------------------------------------------------------------------
/**
    AdjustPosition
*/
bool
ncAIMovEngine::AdjustPosition (vector3* position) const
{
    n_assert(nPathFinder::Instance());

    nPathFinder* pathFinder = nPathFinder::Instance();
    bool valid = true;

    if ( pathFinder && pathFinder->GetMesh() )
    {
        valid = pathFinder->GetMesh()->AdjustPosition (position, 0.3f); //this->entityObject->GetSize());
    }

    return valid;
}

//------------------------------------------------------------------------------
/**
    IsAtEnd
*/
bool
ncAIMovEngine::IsAtEnd() const
{
/*    n_assert (this->transform);
    const vector3& position = this->transform->GetPosition();

    float epsilon = this->speed * this->deltaGameplay * 30.f; // speed * delta * 60fps * 0.5
    
    return bool ( fabs(position.x - this->goal.x) < epsilon && 
                  fabs(position.z - this->goal.z) < epsilon );*/
    vector3 start( this->current.start );
    vector3 goal( this->current.goal );
    start.y = goal.y = 0.f;
    return start.isequal( goal, TINY );
}

//------------------------------------------------------------------------------
/**
    EnableContainmentConstraint
*/
void
ncAIMovEngine::EnableContainmentConstraint( bool enable )
{
    ncAIMovEngine::forceContainment = enable;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    EnableObstacleAvoidance
*/
void
ncAIMovEngine::EnableObstaclesAvoidance( bool enable )
{
    ncAIMovEngine::avoidObstacles = enable;
}

//------------------------------------------------------------------------------
/**
    EnableThrottleSteering
*/
void
ncAIMovEngine::EnableThrottleSteering( bool enable )
{
    ncAIMovEngine::useThrottle = enable;
}

//------------------------------------------------------------------------------
/**
    EnableNonPenetrationConstraint
*/
void
ncAIMovEngine::EnableNonPenetrationConstraint( bool enable )
{
    ncAIMovEngine::dontPenetrate = enable;
}

//------------------------------------------------------------------------------
/**
    EnableAnimDisplacement
*/
void
ncAIMovEngine::EnableAnimDisplacement( bool enable )
{
    ncAIMovEngine::displaceAnim = enable;
}

//------------------------------------------------------------------------------
/**
    EnableConstantTime
*/
void
ncAIMovEngine::EnableConstantTime( bool enable )
{
    ncAIMovEngine::constantTime = enable;
}

//------------------------------------------------------------------------------
/**
    EnableRotationSmoothing
*/
void
ncAIMovEngine::EnableRotationSmoothing( bool enable )
{
    ncAIMovEngine::smoothRotations = enable;
}

//------------------------------------------------------------------------------
/**
    EnableDrawMotion
*/
void
ncAIMovEngine::EnableDrawMotion( bool enable )
{
    this->drawMotion = enable;
}

//------------------------------------------------------------------------------
/**
    EnableDrawAvoid
*/
void
ncAIMovEngine::EnableDrawAvoid( bool enable )
{
    this->drawAvoidance = enable;
}

//------------------------------------------------------------------------------
/**
    EnableDrawStaticAvoid
*/
void
ncAIMovEngine::EnableDrawStaticAvoid( bool enable )
{
    this->drawStaticAvoidance = enable;
}

//------------------------------------------------------------------------------
/**
    Draw
*/
void
ncAIMovEngine::Draw (nGfxServer2* server)
{
    n_assert(server);

    matrix44 m0;
    m0.ident();
    server->SetTransform( nGfxServer2::Model, m0 );

    vector4 color (0.f, 0.3f, 0.9f, 1.f);
    vector3 buffer[256];

    vector3 pos( this->transform->GetPosition() );
    float high = /*-10.f;*/ pos.y; // + 0.4f;

    server->BeginLines();

    if ( this->drawStaticAvoidance )
    {
        // Obstacle avoidance rays
        for ( int i(0); i < StaticAvoidanceRaysNumber; ++i )
        {
            buffer[0] = this->avoidanceRays[i].start();
            buffer[1] = this->avoidanceRays[i].end();
            server->DrawLines3d( buffer, 2, vector4(1,1,0,1) );
        }
    }

    if ( this->drawAvoidance )
    {
        // Movement direction and magnitude
        vector3 moveDir( this->transform->GetPosition() - this->previousPosition );
        moveDir.norm();
        buffer[0] = this->transform->GetPosition();
        buffer[0].y += 0.5f;
        buffer[1] = buffer[0] + moveDir * ( 1 + 10 * this->speed * this->speedFactor );
        server->DrawLines3d( buffer, 2, vector4(0,1,0,1) );

        // Obstacle avoidance turn direction and magnitude
        if ( this->collisionExpected )
        {
            quaternion turnLeft;
            turnLeft.set_rotate_y( N_PI / 2 );
            vector3 leftDir( moveDir );
            leftDir = turnLeft.rotate( moveDir );
            buffer[1] = buffer[0] + leftDir * ( this->avoidanceTurnDir + 5 * this->avoidanceTurnInc / this->maxAvoidanceTurnInc );
            server->DrawLines3d( buffer, 2, vector4(1,0,0,1) );

            // Expected collision point
            buffer[1] = this->expectedCollisionPoint;
            buffer[1].y += 0.5f;
            server->DrawLines3d( buffer, 2, vector4(0.95f,0.9f,0.85f,1) );
        }
    }

    if ( !this->drawMotion )
    {
        server->EndLines();
        return;
    }
    
    // Direction
    quaternion orientation = this->transform->GetQuat();    
    vector3 vertex0, vertex1, vertex2, aux;
    matrix44 transform(orientation);
    transform.set_translation(pos);

    vertex0 = vector3 (0, 0, 3.5f);
    aux = vertex0;
    aux.norm();
    aux *= 0.2f;

    vertex1.x = -aux.z;
    vertex1.z = aux.x;
    vertex2.x = aux.z;
    vertex2.z = -aux.x;    

    transform.mult (vertex0, buffer[0]);
    transform.mult (vertex1, buffer[1]);
    transform.mult (vertex2, buffer[2]);
    buffer[3] = buffer[0];

    int i;
    for ( i=0; i<4; i++ )
    {
        buffer[i].y = high;
    }

    server->DrawLines3d (buffer, 4, color);

    // Orientation objetive
    vertex0 = vector3 (0, 0, 0);
    vertex1 = vector3 (0, 0, 3.5f);

    transform.set (this->current.final);
    transform.set_translation(pos);
    transform.mult(vertex0, buffer[0]);
    transform.mult(vertex1, buffer[1]);

    for ( i=0; i<3; i++ )
    {
        buffer[i].y = high;
    }

    server->DrawLines3d (buffer, 2, vector4 (1.f, 1.f, 1.f, 1.f));

    // Axis
    buffer[0] = vector3 (0.5f, high, 0) + pos;
    buffer[1] = pos;
    buffer[0].y = buffer[1].y = high;
    server->DrawLines3d (buffer, 2, vector4(1.f, 0, 0, 1.f));

    buffer[0] = vector3 (0, high, 0.5f) + pos;
    buffer[1] = pos;
    buffer[0].y = buffer[1].y = high;
    server->DrawLines3d (buffer, 2, vector4(0, 0, 1.f, 1.f));

    // Direction and goal vectors
    vector3 direction, goal;

    direction = vector3 (0.f, 0.f, 0.5f);

    transform.mult (direction, buffer[0]);
    buffer[1] = pos;

    buffer[0].y = buffer[1].y = high;

    goal = this->current.goal - this->current.start;
    goal.norm();
    goal *= 0.5f;
    goal += pos;

    server->DrawLines3d (buffer, 2, vector4(1.f, 0, 0, 1.f));

    buffer[0] = goal;
    buffer[1] = pos;

    buffer[0].y = buffer[1].y = high;

    server->DrawLines3d (buffer, 2, vector4(0, 1.f, 0, 1.f));

    // Jump position
    if ( this->IsJumping() )
    {
        buffer[0] = pos;
        buffer[1] = this->current.goal;

        server->DrawLines3d (buffer, 2, vector4 (1.f, 1.f, 1.f, 1.f));
    }

    // Size of the entity
    float size = 0.3f; //this->transform->GetSize();
    i=0;

    for ( float f=0; f<N_TWOPI; f +=0.1f, i++ )
    {
        vector3 circPos( pos + vector3 (n_cos(f)*size, 0.f, n_sin(f)*size) );
        buffer[i] = circPos;
    }

    server->DrawLines3d (buffer, i, this->entityAreaColor);
    server->EndLines();

    this->DrawPath (server);
    // Draw a cross at the goal (always)
    this->DrawCross (server, this->current.goal);
}

//------------------------------------------------------------------------------
/**
    DrawPath
*/
void
ncAIMovEngine::DrawPath (nGfxServer2* server)
{
    n_assert(server);

    if ( server )
    {   
        void* p = (void*)this;
        size_t i = reinterpret_cast<size_t>(p); 

        int col1 = int(i) & 0x0f;
        int col2 = (int(i) & 0xf0) >> 4;
        vector4 green (0.f, 0.7f, 0.f, 1.f);
        vector4 red (1.0f, 0.f, 0.f, 1.f);
        vector4 color (0.0f, col1/16.0f, col2/16.0f, 1.0f);
        vector3 line[256];    

        // Smooth path
        for ( int i=0; i<this->path.Size(); i++ )
        {
            line[i] = this->path[i];
            this->UpdateHeight (&line[i]);            
            line[i].y += 0.5f;
        } 

        server->BeginLines();

        // Path
        // Only draw the path when exist
        if ( this->path.Size() != 0 )
        {
            server->DrawLines3d (line, this->path.Size(), color);
        }

        server->EndLines();
    }
}

//------------------------------------------------------------------------------
/**
    DrawCross
*/
void
ncAIMovEngine::DrawCross (nGfxServer2* server, const vector3& point)
{
    vector4 color (1.f, 1.f, 1.f, 1.f);
    vector3 vertical[2];
    vector3 horizontal[2];
    float length = 0.3f;
    float margin = 0.001f;

    vertical[0]     = point + vector3 (-length, 0, 0);
    vertical[1]     = point + vector3 (+length, 0, 0);
    horizontal[0]   = point + vector3 (0, 0, -length);
    horizontal[1]   = point + vector3 (0, 0, +length);

    this->UpdateHeight (&vertical[0]);
    this->UpdateHeight (&vertical[1]);
    this->UpdateHeight (&horizontal[0]);
    this->UpdateHeight (&horizontal[1]);

    server->BeginLines();
    server->DrawLines3d (vertical, 2, color);
    server->DrawLines3d (horizontal, 2, color);

    vertical[0]     = point + vector3 (-length, 0, -margin);
    vertical[1]     = point + vector3 (+length, 0, -margin);
    horizontal[0]   = point + vector3 (-margin, 0, -length);
    horizontal[1]   = point + vector3 (-margin, 0, +length);

    this->UpdateHeight (&vertical[0]);
    this->UpdateHeight (&vertical[1]);
    this->UpdateHeight (&horizontal[0]);
    this->UpdateHeight (&horizontal[1]);

    server->DrawLines3d (vertical, 2, color);
    server->DrawLines3d (horizontal, 2, color);

    server->EndLines();
}
#endif // !NGAME

//------------------------------------------------------------------------------
/**
    IsApproaching
*/
bool
ncAIMovEngine::IsApproaching (const vector3& position) const
{
    n_assert(this->transform);

    bool approaching = false;

    if ( this->transform && !this->IsStopped() )
    {
        vector3 direction( this->goal - this->transform->GetPosition() );
        vector3 target( position - this->transform->GetPosition() );

        approaching = direction.dot (target) > 0;
    }

    return approaching;
}

//------------------------------------------------------------------------------
/**
    UpdateHeight
*/
void
ncAIMovEngine::UpdateHeight (vector3* position) const
{
    n_assert( position );
    nNavUtils::GetGroundHeight( *position, position->y );
}

//------------------------------------------------------------------------------
/**
    Move the given point to the ground by applying gravity over time
*/
void
ncAIMovEngine::UpdateHeightSmoothly( vector3& position )
{
    // Get the ground height below the given position
    float groundHeight;
    if ( !nNavUtils::GetGroundHeight( position, groundHeight ) )
    {
        // Return to previous position if no ground is found. This is usually the case
        // when the physics on the space where the agent is in have been deactivated.
        // Of course, if for any reason the agent tries to fall from a distance higher
        // than what GetGroundHeight can detect the agent will stay floating on the air
        // or stuck on the border of a cliff trying to move forward.
        position = this->previousPosition;
        return;
    }

    // If close to the ground, just move the point to it to avoid 'silly stepping'
    bool onGround( false );
    const float MinHeightToStartFalling( 0.1f );
    if ( position.y < groundHeight + MinHeightToStartFalling )
    {
        onGround = true;
    }
    // Otherwise, simulate gravity to move down the position
    else
    {
        const float MaxFallDist = nVariableServer::Instance()->GetFloatVariable("GroundSeekEndDistance");
        float fallInc = this->fallingVelocity * this->deltaGameplay - 9.8f * this->deltaGameplay * this->deltaGameplay * 0.5f;
        if ( fallInc < -MaxFallDist )
        {
            fallInc = -MaxFallDist;
        }
        position.y += fallInc;
        this->fallingVelocity -= 9.8f * this->deltaGameplay;
        onGround = position.y < groundHeight;
    }

    // Stop falling when reaching the ground
    if ( onGround )
    {
        position.y = groundHeight;
        this->fallingVelocity = 0;
    }
}

//------------------------------------------------------------------------------
/**
    SmoothCurrentPath
*/
void
ncAIMovEngine::SmoothCurrentPath()
{
    // empty 
}

//------------------------------------------------------------------------------
/**
    SetPause
*/
void
ncAIMovEngine::SetPause (bool pause)
{
    if ( pause )
    {
        this->pausedState = this->state;
        this->state = ST_PAUSED;
    }
    else
    {
        this->state = this->pausedState;
    }

    // (un)pause the animator too!!
    if ( this->animator )
    {
        this->animator->Pause (pause);
    }
}

//------------------------------------------------------------------------------
/**
    IsAtFloor
*/
bool
ncAIMovEngine::IsAtFloor (const vector3& position) const
{
    float groundHeight;
    if ( !nNavUtils::GetGroundHeight( position, groundHeight ) )
    {
        return false;
    }
    else
    {        
        return position.y <= groundHeight + N_TINY;
    }
}

//------------------------------------------------------------------------------
/**
    GetMaxAvoidanceTurnInc
*/
float
ncAIMovEngine::GetMaxAvoidanceTurnInc() const
{
    return this->maxAvoidanceTurnInc;
}

//------------------------------------------------------------------------------
/**
    SetMaxAvoidanceTurnInc
*/
void
ncAIMovEngine::SetMaxAvoidanceTurnInc( float maxInc )
{
    this->maxAvoidanceTurnInc = maxInc;
}

//------------------------------------------------------------------------------
/**
    GetMaxAvoidanceTurnAccel
*/
float
ncAIMovEngine::GetMaxAvoidanceTurnAccel() const
{
    return this->maxAvoidanceTurnAccel;
}

//------------------------------------------------------------------------------
/**
    SetMaxAvoidanceTurnAccel
*/
void
ncAIMovEngine::SetMaxAvoidanceTurnAccel( float maxAccel )
{
    this->maxAvoidanceTurnAccel = maxAccel;
}

//------------------------------------------------------------------------------
/**
    GetAvoidanceTime
*/
float
ncAIMovEngine::GetAvoidanceTime() const
{
    return this->avoidanceTime;
}

//------------------------------------------------------------------------------
/**
    SetAvoidanceTime
*/
void
ncAIMovEngine::SetAvoidanceTime( float time )
{
    this->avoidanceTime = time;
}

//------------------------------------------------------------------------------
/**
    GetMaxAvoidanceThrottleAccel
*/
float
ncAIMovEngine::GetMaxAvoidanceThrottleAccel() const
{
    return this->maxAvoidanceThrottleAccel;
}

//------------------------------------------------------------------------------
/**
    SetMaxAvoidanceThrottleAccel
*/
void
ncAIMovEngine::SetMaxAvoidanceThrottleAccel( float maxAccel )
{
    this->maxAvoidanceThrottleAccel = maxAccel;
}

//------------------------------------------------------------------------------
/**
    @param newIndex new animation index
*/
void
ncAIMovEngine::UpdateDisplacementIndex()
{
    int newIndex = this->animator->GetActualState( 
        ncLogicAnimator:: LA_BODY_FULL | ncLogicAnimator::LA_BODY_LOWER );

    if( newIndex != -1 )
    {
        this->dispIndex = newIndex;

        this->useAnimDisplacement = this->animator->HasDisplacement( this->dispIndex );
    }
}

//------------------------------------------------------------------------------
/**
    @param displacement [OUT] displacement of animation in last frame
*/
void
ncAIMovEngine::GetAnimDisplacement( vector3 & displacement ) const
{
    if( this->dispIndex != -1 )
    {
        float time = static_cast<float>( nApplication::Instance()->GetTime() );

        displacement = this->animator->GetDisplacement( this->dispIndex, time - this->deltaGameplay, time );
        displacement.x *= this->dispFactor.x;
        displacement.y *= this->dispFactor.y;
        displacement.z *= this->dispFactor.z;
    }
}

//------------------------------------------------------------------------------
/**
    @returns the distance of jump
*/
vector3
ncAIMovEngine::GetOriginalJumpDistance() const
{
    vector3 distance( 0.0f, 0.0f, 0.0f );

    if( this->dispIndex != - 1 )
    {
        float time = static_cast<float>( nApplication::Instance()->GetTime() );

        float fromTime = time - this->animator->GetElapsedTime( this->dispIndex, false );
        float toTime = time + this->animator->GetRemainingTime( this->dispIndex, false );

        distance = this->animator->GetDisplacement( this->dispIndex, fromTime, toTime );
    }

    return distance;
}

//------------------------------------------------------------------------------
/**
    @returns the distance of jump
*/
vector3
ncAIMovEngine::GetJumpDistance() const
{
    vector3 distance( this->GetOriginalJumpDistance() );

    distance.x *= this->dispFactor.x;
    distance.y *= this->dispFactor.y;
    distance.z *= this->dispFactor.z;

    return distance;
}
//------------------------------------------------------------------------------
/**
    @param factor new factor
*/
void
ncAIMovEngine::SetJumpFactor( const vector3 & factor )
{
    this->dispFactor = factor;
}

//------------------------------------------------------------------------------
/**
    @returns the jump factor
*/
vector3
ncAIMovEngine::GetJumpFactor() const
{
    return this->dispFactor;
}

//------------------------------------------------------------------------------
/**
    Get the gameplay body size of an agent
*/
float
ncAIMovEngine::GetBodySize( const nEntityObject* entity ) const
{
    const float DefaultBodySize( 0.4f );

    const ncGameplayLivingClass* living( entity->GetClassComponent<ncGameplayLivingClass>() );
    if ( living )
    {
        return living->GetSize();
    }
    else
    {
        return DefaultBodySize;
    }
}

//------------------------------------------------------------------------------
