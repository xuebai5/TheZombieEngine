#include "precompiled/pchrnsgameplay.h"
//-----------------------------------------------------------------------------
//  file ncgameplayliving_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncaimovengine/ncaimovengine.h"
#include "ncaistate/ncaistate.h"
#include "rnsgameplay/ngameplayutils.h"
#include "ncfsm/ncfsm.h"

#include "ncgpperception/ncgpfeeling.h"
#include "ncgpperception/ncgphearing.h"
#include "ncgpperception/ncgpsight.h"
#include "ncagentmemory/ncagentmemory.h"
#include "ntrigger/nctrigger.h"
#include "ntrigger/ntriggerserver.h"
#include "ntrigger/ncareaevent.h"
#include "zombieentity/ncdictionary.h"

#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "nworldinterface/nworldinterface.h"

#include "animcomp/nccharacter.h"
#include "animcomp/nccharacterclass.h"
#include "animcomp/ncskeletonclass.h"

#include "entity/nentityobjectserver.h"
#include "nnetworkmanager/nnetworkmanager.h"

#ifndef NGAME
#include "ndebug/ndebugserver.h"
#include "napplication/nappviewport.h"
#endif//!NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGameplayLiving,ncGameplay);

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGameplayLiving::ncGameplayLiving() : 
    commandAura	(0), 
    carriedBy	(0), 
    currentWeapon(0), 
    inventory( 0 ),
    regenRate(90),
	isSprint	(false),
    isCrouch	(false),
	isProne		(false),
	isSwimming	(false),
    isRecovering (false),
    busyLevel(0),
    recoverHealth  (0),
    dizzyTime(1.0),
    breath(0),
    attackSpeed(0),
    gpState(0),
    regenFactor(1),
    pathId(0),
    lastWayPoint(0),
    health(1000),
    impacted(false),
    impactAnimIndex(-1),
    movementEvent( nGameEvent::HUMAN_SOUND )
{    
    ncGameplay::ncGameplay();    

    this->ToggleFlag (HEALTH_DIZZY, false);
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGameplayLiving::~ncGameplayLiving()
{
    this->carriedBy = 0;
    this->currentWeapon = 0;

    if( this->inventory )
    {
        this->inventory->Release();
        this->inventory = 0;
    }

    ncGameplay::~ncGameplay();
}

//------------------------------------------------------------------------------
/**
    instance initialization
*/
void
ncGameplayLiving::InitInstance(nObject::InitInstanceMsg initType)
{
    ncGameplay::InitInstance (initType);

    this->InitRefs();

    ncGameplayLivingClass * livingClass = this->GetClassComponentSafe<ncGameplayLivingClass>();

    if ( initType == nObject::NewInstance )
    {
        this->SetHealth (livingClass->GetMaxHealth());
    }

    // initialize the inventary
    if (initType != nObject::ReloadedInstance)
    {
        this->inventory = static_cast<nInventoryContainer*>( 
            nKernelServer::Instance()->New( "ninventorycontainer" ) );
        n_assert( this->inventory );
        if( this->inventory && livingClass )
        {
            this->inventory->SetSize( livingClass->GetInventorySize() );
        }
    }

    this->SetBlocking(false);
}

//------------------------------------------------------------------------------
/**
    Initialices references to other components
*/
void
ncGameplayLiving::InitRefs ()
{
    this->currentWeapon = 0;
}

//------------------------------------------------------------------------------
/**
    SetHealth
*/
void
ncGameplayLiving::SetHealth (int value)
{
    if ( !this->IsInvulnerable() )
    {
        ncGameplayLivingClass *gpLivingClass = this->GetClassComponentSafe<ncGameplayLivingClass>();
        // In function the value we must update the health state        
        n_assert(gpLivingClass);

        // calculate ranges and be aware for regeneration
        if ( gpLivingClass )
        {
            // calculate ranges
            if (value > gpLivingClass->GetMaxHealth() )
            {
                this->health = gpLivingClass->GetMaxHealth();
            }
            else if ( value < 0 )
            {
                this->health = 0;
            }
            else
            {
                this->health = value;
            }       

            // update health state
            if ( value > 0)
            {
                // the character is still alive, then can be healty or wounded
                if ( value > gpLivingClass->GetMaxHealth() * gpLivingClass->GetRegenThreshold() )
                {
                    this->SetHealthState (HEALTH_HEALTHY);
                    this->SetHasRegen(true);
                }
                else if ( value < gpLivingClass->GetMaxHealth() * gpLivingClass->GetRegenThreshold() )
                {                
                    this->SetHealthState (HEALTH_WOUNDED);
                    this->SetHasRegen(false);
                }

            }            
            else 
            {
                // without health, character can be incapacitated or died
                if ( gpLivingClass->CanBeIncapacitated())
                {
                    this->SetHealthState (HEALTH_INCAPACITATED);
                    this->SetHasRegen(false);
                }
                else if ( !gpLivingClass->CanBeIncapacitated())
                {
                    nNetworkManager * network = nNetworkManager::Instance();
                    if ( network && network->IsServer() )
                    {
                        this->SetDead();
                    }
                }
            }                 
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayLiving::SetDead()
{
    this->SetHealthState (HEALTH_DEAD);
    this->AbortCurrentAction();

    // finish FSM execution
    ncFSM* fsm = this->GetComponent<ncFSM>();
    if ( fsm )
    {
        fsm->Reset();
    }

    // stop move engine
    ncAIMovEngine * movengine = this->GetComponent<ncAIMovEngine>();
    if( movengine && !movengine->IsJumping() )
    {
        movengine->Stop();
    }

    // stop emitting events
    ncAreaEvent* events = this->GetComponent<ncAreaEvent>();
    if ( events )
    {
        events->DeleteAllEvents();
    }

    // stop receiving events
    ncTrigger* trigger = this->GetComponent<ncTrigger>();
    if ( trigger )
    {
        nTriggerServer::Instance()->RemoveTrigger( this->GetEntityObject() );
    }

    // put dead ragdoll
    /// @todo ragdoll
    ncLogicAnimator * animator = this->GetComponent<ncLogicAnimator>();
    if( animator )
    {
        animator->SetDie();
    }

    // place a dead event where the entity has died
    // @todo bind it with the radgoll or make the rangdoll emit it,
    //       so when the ragdoll is removed the event is removed as well
    if ( nNetworkManager::Instance()->IsServer() )
    {
        this->PlaceDeadEvent();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ncGameplayLiving::GetShotPosition( vector3 & pos )const
{
    ncTransform * transform = this->GetEntityObject()->GetComponent<ncTransform>();
    n_assert( transform );
    if( transform )
    {
        pos = transform->GetPosition();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ncGameplayLiving::GetShotDirection( vector3 & dir )const
{
    ncTransform * transform = this->GetEntityObject()->GetComponent<ncTransform>();
    n_assert( transform );
    if( transform )
    {
        matrix44 mat44 = transform->GetTransform();
        quaternion q = mat44.get_quaternion();
        q.normalize();

        dir = q.rotate( vector3( 0, 0, 1 ) );
        dir.norm();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ncGameplayLiving::GetShotAngles( polar2 & angles )const
{
    ncTransform * transform = this->GetEntityObject()->GetComponent<ncTransform>();
    n_assert( transform );
    if( transform )
    {
        angles = transform->GetPolar();
    }
}

//------------------------------------------------------------------------------
/**
    Run
*/
void
ncGameplayLiving::Run ( const float deltaTime)
{    
    ncGameplay::Run( deltaTime );

    if( this->currentWeapon )
    {
        this->currentWeapon->GetComponentSafe<ncGPWeapon>()->ApplyAccuracyRecover ( deltaTime );
    }    

    this->UpdateMemory();

    if ( this->IsRecovering() )
    {
        // still have injection efects
        this->RunRecover( deltaTime );
    }
    else if (this->IsHealthy() && this->HasRegen())
    {
        // natural regeneration
        if ( this->health < this->GetClassComponentSafe<ncGameplayLivingClass>()->GetMaxHealth() )
        {
            this->RunRegenerate( deltaTime );
        }
    }
    
    if ( this->IsDizzy() )
    {
        // player is dizzy for a while
        this->dizzyTime -= deltaTime;
        if (this->dizzyTime <= 0)
        {
            this->SetDizzy( false );
            this->dizzyTime = 1.0f;
        }
    }
}

//------------------------------------------------------------------------------
/**
    RunRecover
*/
void
ncGameplayLiving::RunRecover( const nTime & frameTime )
{
    int addHealth = static_cast<int>( frameTime * this->regenRate );
    int currentHealth = this->GetHealth() + addHealth;
    
    ncGameplayLivingClass *gpLivingClass = this->GetClassComponentSafe<ncGameplayLivingClass>();
    if (currentHealth > gpLivingClass->GetMaxHealth())
    {
        this->SetHealth( gpLivingClass->GetMaxHealth() );
        this->SetRecovering( false );
    }
    else if (currentHealth < this->recoverHealth)
    {
        this->SetHealth(currentHealth);
    }
    else
    {
        this->SetHealth(this->recoverHealth);        
        this->SetRecovering( false );
    }
}

//------------------------------------------------------------------------------
/**
RunRegenerate
    @param frameTime second transcurred from last call
*/
void
ncGameplayLiving::RunRegenerate( const nTime & frameTime )
{
    double factorizedHealth = this->GetClassComponentSafe<ncGameplayLivingClass>()->GetRegenAmount();
    factorizedHealth *= this->GetRegenFactor();
    factorizedHealth *= frameTime;
    int newHealth = this->health  + static_cast<int>( n_ceil( factorizedHealth ) );
    this->SetHealth(newHealth);
}

//------------------------------------------------------------------------------
/**
    Update the entity's memory
*/
void
ncGameplayLiving::UpdateMemory()
{
    ncAgentMemory* memory = this->GetComponent<ncAgentMemory>();
    if ( memory )
    {
        memory->UpdateMemory();
    }
}

//------------------------------------------------------------------------------
/**
    SetCarriedByID
*/
void
ncGameplayLiving::SetCarriedByID (unsigned int idEntity)
{
    nEntityObjectServer* entityServer = nEntityObjectServer::Instance();
    n_assert(entityServer);

    if ( entityServer && idEntity != 0 )
    {
        this->carriedBy = entityServer->GetEntityObject (idEntity);
    }
}

//------------------------------------------------------------------------------
/**
    GetCarriedByID
*/
unsigned int 
ncGameplayLiving::GetCarriedByID() const
{
    unsigned int idEntity = 0;

    if ( this->carriedBy )
    {
        idEntity = this->carriedBy->GetId();
    }

    return idEntity;
}

//------------------------------------------------------------------------------
/**
*/
void
ncGameplayLiving::SetCurrentWeapon( nEntityObject * weapon )
{
    this->currentWeapon = weapon;
    if( this->currentWeapon )
    {
        if( this->currentWeapon->GetComponentSafe<ncGPWeapon>() )
        {
            ncCharacter * character = this->GetComponentSafe<ncCharacter>();
            if( character )
            {
                character->Attach( "tgr_m4", this->currentWeapon->GetId() );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @returns inventory of gameplay living
*/
nInventoryContainer*
ncGameplayLiving::GetInventory()
{
    return this->inventory;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @param gfxserver graphics server
*/
void
ncGameplayLiving::DebugDraw( nGfxServer2 * const gfxServer ) 
{
    n_assert( gfxServer );
    if( ! gfxServer )
    {
        return;
    }

    ncGameplay::DebugDraw( gfxServer );

    bool debugTexts = nDebugServer::Instance()->GetFlagEnabled( "rnsview", "debugtexts" );
    if( debugTexts )
    {
        // show inventory
        if( this->inventory )
        {
            this->inventory->DebugDraw( gfxServer );
        }

        vector3 pos = this->GetComponent<ncTransform>()->GetPosition();

        pos.y += 2.0f;
        
        nAppViewport * viewport = static_cast<nAppViewport*>( nKernelServer::ks->Lookup( "/usr/rnsview" ) );
        n_assert( viewport );
        if( ! viewport )
        {
            return;
        }

        matrix44 matV = viewport->GetViewMatrix();
        matV.invert();
        matV *= nCamera2( viewport->GetCamera() ).GetProjection();
        vector4 pos4;
        pos4 = pos;
        pos4 = matV * pos4;

        if( pos4.w <= 0 )
        {
            return;
        }

        pos4.x = pos4.x / pos4.w;
        pos4.y = -pos4.y / pos4.w;

        nString text;
        text = "life: ";
        text.AppendInt( this->GetHealth() );
        gfxServer->Text( text.Get(), vector4(0.5f,1,0.5f,0.5f), pos4.x, pos4.y );
    }
}
#endif//!NGAME

//-----------------------------------------------------------------------------
/**
    FIXME antonia.tugores, update it when moving joint names from ncskeletonclass to ncharjoint
*/
ncGameplayLivingClass::BodyPart
ncGameplayLiving::GetBodyPart( geomid geomId )
{
    ncCharacter* characterComp = this->GetComponentSafe<ncCharacter>();
    
    ncCharacterClass* characterClass = characterComp->GetClassComponent<ncCharacterClass>();
    ncSkeletonClass* physicsSkeletonClass = characterClass->GetSkeletonClassPointer( characterComp->GetPhysicsSkelIndex() );

    // get geometry if we have geomid
    nPhysicsGeom* geometry = static_cast<nPhysicsGeom*>(phyRetrieveDataGeom( geomId ));

    // get joint name
    nString jointName = physicsSkeletonClass->GetJointName(geometry->GetIdTag());
    jointName.ToLower();
    
    // compare joint name with different part names
    if (jointName.MatchPattern("*head*"))
    {
        return ncGameplayLivingClass::BP_HEAD;
    }

    if( jointName.MatchPattern("*arm*")    || 
        jointName.MatchPattern("*finger*") || 
        jointName.MatchPattern("*hand*") )
    {
        return ncGameplayLivingClass::BP_ARM;
    }

    if( jointName.MatchPattern("*calf*")  || 
        jointName.MatchPattern("*thigh*") || 
        jointName.MatchPattern("*toe*") )
    {
        return ncGameplayLivingClass::BP_LEG;
    }

    return ncGameplayLivingClass::BP_TORSO;
}

//-----------------------------------------------------------------------------
/**
    Says if it's in melee range
*/
bool
ncGameplayLiving::IsInMeleeRange() const
{
    const ncAIState* state = this->GetComponentSafe<ncAIState>();
    if (state)
    {
        const nEntityObject* target = state->GetTarget();
        if (target)
        {
            const ncTransform* trComp = this->GetComponentSafe<ncTransform>();
            const ncTransform* targetTrComp = target->GetComponentSafe<ncTransform>();
            if (trComp && targetTrComp)
            {
                vector3 targetPos(targetTrComp->GetPosition());
                this->GetComponentSafe<ncAIMovEngine>()->UpdateHeight(&targetPos);
                vector3 distVect(trComp->GetPosition() - targetPos);
                float meleeRange = this->GetClassComponentSafe<ncGameplayLivingClass>()->GetMeleeRange();
                meleeRange += 0.6f;
                return distVect.lensquared() <= (meleeRange * meleeRange);
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Create and place a dead event where the entity is
*/
void
ncGameplayLiving::PlaceDeadEvent()
{
    //empty: Implemented in derived classes for launch died events
}

//------------------------------------------------------------------------------
/**
    Place the gameplay event emitted when made noise movement
*/
void
ncGameplayLiving::SwitchOnMovementEvent( int time )
{
    ncAreaEvent* areaEvent = this->GetComponentSafe<ncAreaEvent>();
    n_assert( areaEvent );
    if ( areaEvent )
    {
        nGameEvent::Id eventId = areaEvent->GetEventByType( this->movementEvent );
        if ( eventId == nGameEvent::INVALID_ID)
        {
            this->GetComponentSafe<ncAreaEvent>()->EmitGameEvent(this->movementEvent, this->GetEntityObject(), time, 0);
        }
    }    
}

//------------------------------------------------------------------------------
/**
    Stops the gameplay event emitted when made noise movement
*/
void
ncGameplayLiving::SwitchOffMovementEvent()
{
    ncAreaEvent* areaEvent = this->GetComponentSafe<ncAreaEvent>();
    n_assert( areaEvent );
    if ( areaEvent )
    {
        nGameEvent::Id eventId = areaEvent->GetEventByType( this->movementEvent );
        if ( eventId != nGameEvent::INVALID_ID )
        {
            areaEvent->StopGameEvent( eventId );
        }
    }    
}
