#ifndef NCGAMEPLAYLIVING_H
#define NCGAMEPLAYLIVING_H

//------------------------------------------------------------------------------
#include "rnsgameplay/ncgameplay.h"
#include "rnsgameplay/ncgpweapon.h"
#include "rnsgameplay/ninventorycontainer.h"
#include "zombieentity/nctransform.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#include "ntrigger/ngameevent.h"

//------------------------------------------------------------------------------
class nPersistServer;
class ncGPHearing;
class ncGPFeeling;
class ncGPSight;
class ncAIState;
/** ZOMBIE REMOVE
class nFightRingManager;
*/
//------------------------------------------------------------------------------
class ncGameplayLiving : public ncGameplay
{

    NCOMPONENT_DECLARE(ncGameplayLiving,ncGameplay);

protected:
    enum
    {
        // Health states
        HEALTH_HEALTHY              = 1 << 0,   // Plenty of health
        HEALTH_WOUNDED              = 1 << 1,   // Touched 
        HEALTH_INCAPACITATED        = 1 << 2,   // Like Darth Vader after fighting with Obi-Wan
        HEALTH_STUNNED              = 1 << 3,   // Dummy state
        HEALTH_DEAD                 = 1 << 4,   // Sorry, try it again 
        HEALTH_INVULNERABLE         = 1 << 5,   // I'm invencible!!
        HEALTH_DIZZY                = 1 << 6,   // Feeling little bad
        // Perception states
        PERCEPTION_BLIND            = 1 << 7,   // Unable to see, but able to hear and feel
        PERCEPTION_DEAF             = 1 << 8,   // Unable to hear, but able to see and feel
        PERCEPTION_INVISIBLE        = 1 << 9,   // Entity not able to be seen
        PERCEPTION_SILENT           = 1 << 10,  // Entity not able to be heard
        PERCEPTION_SENSORYDISABLED  = 1 << 11,   // Perception disabled
        // Common AI state
        GP_BRAINSTOPPED             = 1 << 12,  // Thinking disabled
        GP_FROZEN                   = 1 << 13,  // Move and thinking disabled        
        GP_COMBATENGAGE             = 1 << 14,  // In the middle of a fight
        GP_HELD                     = 1 << 15,  // Unable to move
        // Special AI state
        GP_BLOCKING                 = 1 << 16,
        GP_CANBEKILLED              = 1 << 17,  // Squad member
        GP_TACRANDOM                = 1 << 18
    } eGPState;

public:
    enum BusyLevel 
    {
        BL_NONE = 0,
        BL_LOW,
        BL_NORMAL,
        BL_HIGH,
    };
    /// Constructor
    ncGameplayLiving();
    /// Destructor
    ~ncGameplayLiving();

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Gameplay properties for entities
    // -- Health attributes
    /// Sets/gets the current health
    void SetHealth(int);
    int GetHealth() const;
    // -- Gameplay properties
    /// Sets/gets the state (for persitence only)
    void SetGPState(unsigned int);
    unsigned int GetGPState() const;
    /// Increment/decrement references to auras
    void AddCommandAura();
    void RemoveCommandAura();
    /// Sets/Gets command aura
    void SetCommandAura(int);
    int GetCommandAura() const;
    /// Gets health state        
    bool IsHealthy() const;
    bool IsWounded() const;
    bool IsIncapacitated() const;
    /// Sets the stunned state
    void SetStunned(bool);
    /// Gets the stunned state
    bool IsStunned() const;
    /// Sets the entity dead
    void SetDead();
    /// Gets the entity dead state
    bool IsDead() const;
    /// Sets/gets invulnerability state
    void SetInvulnerable(bool);
    bool IsInvulnerable() const;
    /// Sets/gets dizzy state
    void SetDizzy(bool);
    bool IsDizzy() const;
    /// Sets/gets regen time
    void SetRegenRate(int);
    int GetRegenRate() const;
    /// Sets/gets amount recover health
    void SetRecoverHealth(int);
    int GetRecoverHealth() const;
    /// Sets/gets recover factor, this factor makes the regeneration slower (0 < value > 1) or faster (value > 1)
    void SetRegenFactor(float);
    float GetRegenFactor() const;
    /// Sets/gets breath
    void SetBreath(float);
    float GetBreath() const;
    // -- Combat attributes
    // Sets/gets attack speed
    void SetAttackSpeed(int);
    int GetAttackSpeed() const;
    // -- AI attributes
    /// Sets/gets brain state :D
    void SetBrainStopped(bool);
    bool IsBrainStopped() const;
    /// Sets/gets frozen estate
    void SetFrozen(bool);
    bool IsFrozen() const;
    /// Sets/gets sensory disabled state
    void SetSensoryDisabled(bool);
    bool IsSensoryDisabled() const;
    /// Sets/gets blind state
    void SetBlind(bool);
    bool IsBlind() const;
    /// Sets/gets deaf state
    void SetDeaf(bool);
    bool IsDeaf() const;
    /// Sets/gets invisible state
    void SetInvisible(bool);
    bool IsInvisible() const;
    /// Sets/gets silent state
    void SetSilent(bool);
    bool IsSilent() const;
    /// Sets/gets combat engage
    void SetCombatEngage(bool);
    bool IsInCombat() const;
    /// Sets/gets held state
    void SetHeld(bool);
    bool IsHeld() const;
    /// Sets/gets blocking state
    void SetBlocking(bool);
    bool IsBlocking() const;
    /// Sets/gets recovering state
    void SetRecovering(bool);
    bool IsRecovering() const;
    /// Gets/Sets primary weapon
    virtual void SetCurrentWeapon(nEntityObject *);
    nEntityObject * GetCurrentWeapon ()const;

    /// apply damage to the living entity
    virtual void ApplyDamage(int);
    /// Sets/Gets ironsight mode
    virtual void SetIronsight(bool);
    bool IsIronsight ()const;       
    /// Set/gets entity ID carried by
    void SetCarriedByID(unsigned int);
    unsigned int GetCarriedByID() const;

    /// Says if it's in melee range
    bool IsInMeleeRange() const;

	/// Say if the entity is sprinting
    bool IsSprinting() const;
    void SetSprinting(bool);
	/// Say if the entity is crouching
    bool IsCrouching() const;
    virtual void SetCrouching(bool);
	/// Say if the entity is crawling
    bool IsProne() const;
    virtual void SetProning(bool);
	/// Say if the entity is swimming
    bool IsSwimming() const;
    void SetSwimming(bool);
/** ZOMBIE REMOVE
    /// Make the entity to leave its current fight ring
    void LeaveFightRing();

    /// Create the rings manager
    void CreateRingsManager();
    /// Destroy the rings manager
    void DestroyRingsManager();
    /// Return true if have space in inner rings
    bool HasSpaceInnerRings();
*/
    /// Sets/gets 'has regen' property
    void SetHasRegen(bool);
    bool HasRegen() const;

    /// Sets/gets the waypoint path id that will follow the entity
    void SetPathId(int);
    int GetPathId() const;

    /// Sets/gets the waypoint path id that will follow the entity
    void SetLastWayPoint(int);
    int GetLastWayPoint() const;

    /// Get inventory of entity
    nInventoryContainer * GetInventory ();

    /// Set busy level
    void SetBusy(int);
    /// Get busy level
    int GetBusy ()const;
    /// Is busy
    bool IsBusy ()const;
    /// Set the event launched when moving
    void SetMovementEvent ( nGameEvent::Type );
    nGameEvent::Type GetMovementEvent() const;
    /// Launch noise movement event
    void SwitchOnMovementEvent( int time=0);
    /// Stops noise movement event
    void SwitchOffMovementEvent();
														
    /// runs the gameplay logic 
    virtual void Run ( const float deltaTime);

    /// get shot ray for weapon shoot
    virtual void GetShotPosition( vector3 & pos )const;
    virtual void GetShotDirection( vector3 & dir )const;
    /// get the angles of shot direction of living
    virtual void GetShotAngles( polar2 & angles )const;

    /// Sets/gets entity carried by
    void SetCarriedBy (nEntityObject* entity);
    nEntityObject* GetCarriedBy() const;

    /// get body part
    ncGameplayLivingClass::BodyPart GetBodyPart(geomid geomId);
/** ZOMBIE REMOVE
    /// Get the fighting rings
    nFightRingManager* GetRingsManager() const;

    /// Reset all the entities in the rings to redetermine its rings
    void ResetRings();
*/
    /// Set the entity as impacted or not
    void SetImpacted(bool flag);
    /// Says if the entity has been impacted
    bool IsImpacted() const;

    /// Set the impact's animation index
    void SetImpactAnimIndex(int index);
    /// Set the impact's animation index
    int GetImpactAnimIndex() const;

#ifndef NGAME
    /// debug draw of entity
    virtual void DebugDraw( nGfxServer2 * const gfxServer );
#endif//!NGAME

protected:
    /// Auxiliary method for handle flags
    void ToggleFlag (unsigned int flag, bool activate);
    bool ExistFlag (unsigned int flag) const;

    nEntityObject * currentWeapon;
    float dizzyTime;
	bool isSprint;
	bool isCrouch;
	bool isProne;
	bool isSwimming;
    int busyLevel;
    nInventoryContainer * inventory;

private:
    static const int NUM_INVENTORY_ITEMS = 10;

    /// Init refs
    void InitRefs();

    /// Set health state
    void SetHealthState (unsigned int);
    void ResetHealth();

    /// Update the entity's memory
    void UpdateMemory();
    /// Run the recover routine
    void RunRecover( const nTime & frameTime );
    /// Run regenrating
    void RunRegenerate( const nTime & frameTime );
    /// Place the dead event
    virtual void PlaceDeadEvent(); 

    // -- Gameplay properties
    int commandAura;            // Number of auras that affect the entity
    ncAIState* aiState;
    nEntityObject* carriedBy;

    // -- Health properties
    int health;
    int regenRate;
    int recoverHealth;          // Health that will have after apply a recover    
    bool isRecovering;
    // For health recovering (inject nanobots)
    float regenFactor;
    bool hasRegen;

    float breath;               // Amount of air remaining in its lungs

    // -- Combat properties
    int attackSpeed;

    // -- gameplay state
    unsigned int gpState;
/** ZOMBIE REMOVE
    /// manager that controlles the fight rings around this entity
    nFightRingManager* ringsManager;
*/
    /// id of the path associated to the entity
    int pathId;
    /// The last waypoint where the entity is moving
    int lastWayPoint;

    /// True if the entity is impacted
    bool impacted;

    /// Impact's animation index
    int impactAnimIndex;

    /// Event launched when move noisy
    nGameEvent::Type movementEvent;
};

//------------------------------------------------------------------------------
/**
    SetMovementEvent
*/
inline
void
ncGameplayLiving::SetMovementEvent( nGameEvent::Type event )
{
    this->movementEvent = event;
}

//------------------------------------------------------------------------------
/**
    GetMovementEvent
*/
inline
nGameEvent::Type
ncGameplayLiving::GetMovementEvent() const
{
    return this->movementEvent;
}
//------------------------------------------------------------------------------
/**
SetLastWayPoint
*/
inline
void
ncGameplayLiving::SetLastWayPoint (int index)
{
    this->lastWayPoint = index;
}

//------------------------------------------------------------------------------
/**
GetLastWayPoint
*/
inline
int
ncGameplayLiving::GetLastWayPoint() const
{
    return this->lastWayPoint;
}

//------------------------------------------------------------------------------
/**
SetPathId
*/
inline
void
ncGameplayLiving::SetPathId (int id)
{
    if ( id != this->pathId)
    {
        this->pathId = id;
#ifndef NGAME
        this->GetEntityObject()->SetObjectDirty();
#endif
    }    
}

//------------------------------------------------------------------------------
/**
GetPathId
*/
inline
int
ncGameplayLiving::GetPathId() const
{
    return this->pathId;
}

//------------------------------------------------------------------------------
/**
SetHasRegen
*/
inline
void
ncGameplayLiving::SetHasRegen (bool value)
{
    this->hasRegen = value;
}

//------------------------------------------------------------------------------
/**
HasRegen
*/
inline
bool
ncGameplayLiving::HasRegen() const
{
    return this->hasRegen;
}
//------------------------------------------------------------------------------
/**
    GetHealth
*/
inline
int
ncGameplayLiving::GetHealth() const
{
    return this->health;
}

//------------------------------------------------------------------------------
/**
    ToggleFlag
*/
inline
void
ncGameplayLiving::ToggleFlag (unsigned int flag, bool activate)
{
    if ( activate )
    {
        this->gpState |= flag;
    }
    else
    {
        this->gpState &= ~flag;
    }
}

//------------------------------------------------------------------------------
/**
    ExistFlag
*/
inline
bool
ncGameplayLiving::ExistFlag (unsigned int flag) const
{
    return (this->gpState&flag) != 0;
}

//------------------------------------------------------------------------------
/**
    ResetHealth
*/
inline
void
ncGameplayLiving::ResetHealth()
{
    this->ToggleFlag (HEALTH_HEALTHY, false);
    this->ToggleFlag (HEALTH_WOUNDED, false);
    this->ToggleFlag (HEALTH_INCAPACITATED, false);
    this->ToggleFlag (HEALTH_STUNNED, false);    
    this->ToggleFlag (HEALTH_DEAD, false);
}
//------------------------------------------------------------------------------
/**
    AddCommandAura
*/
inline
void
ncGameplayLiving::AddCommandAura()
{
    this->commandAura++;
}

//------------------------------------------------------------------------------
/**
    RemoveCommandAura
*/
inline
void
ncGameplayLiving::RemoveCommandAura()
{
    this->commandAura--;
}

//------------------------------------------------------------------------------
/**
    SetCommandAura
*/
inline
void
ncGameplayLiving::SetCommandAura (int commandAura)
{
    this->commandAura = commandAura;
}

//------------------------------------------------------------------------------
/**
    GetCommandAura
*/
inline
int
ncGameplayLiving::GetCommandAura() const
{
    return this->commandAura;
}

//------------------------------------------------------------------------------
/**
    SetGPState
*/
inline
void
ncGameplayLiving::SetGPState (unsigned int state)
{
    this->gpState = state;
}

//------------------------------------------------------------------------------
/**
    GetGPState
*/
inline
unsigned int
ncGameplayLiving::GetGPState() const
{
    return this->gpState;
}

//------------------------------------------------------------------------------
/**
SetRecoverHealth
*/
inline
void
ncGameplayLiving::SetRecoverHealth (int health)
{
    int maxHealth = this->GetClassComponentSafe<ncGameplayLivingClass>()->GetMaxHealth();
    this->recoverHealth = this->health + health;
    if ( this->recoverHealth > maxHealth )
    {
        this->recoverHealth = maxHealth;
    }

    this->isRecovering = true;
}

//------------------------------------------------------------------------------
/**
GetRecoverHealth
*/
inline
int
ncGameplayLiving::GetRecoverHealth() const
{
    return this->recoverHealth;
}

//------------------------------------------------------------------------------
/**
SetRegenFactor
*/
inline
void
ncGameplayLiving::SetRegenFactor (float factor)
{
    this->regenFactor = factor;
}

//------------------------------------------------------------------------------
/**
GetRegenFactor
*/
inline
float
ncGameplayLiving::GetRegenFactor() const
{
    return this->regenFactor;
}

//------------------------------------------------------------------------------
/**
    SetHealthState
*/
inline
void
ncGameplayLiving::SetHealthState(unsigned int healthState)
{
    this->ResetHealth();
    this->ToggleFlag (healthState, true);
}

//------------------------------------------------------------------------------
/**
    IsHealthy
*/
inline
bool
ncGameplayLiving::IsHealthy() const
{
    return this->ExistFlag (HEALTH_HEALTHY);
}

//------------------------------------------------------------------------------
/**
    IsWounded
*/
inline
bool
ncGameplayLiving::IsWounded() const
{
    return this->ExistFlag (HEALTH_WOUNDED);
}

//------------------------------------------------------------------------------
/**
    IsIncapacitated
*/
inline
bool 
ncGameplayLiving::IsIncapacitated() const
{
    return this->ExistFlag (HEALTH_INCAPACITATED);
}

//------------------------------------------------------------------------------
/**
SetStunned
*/
inline
void
ncGameplayLiving::SetStunned (bool stunned)
{
    this->ToggleFlag (HEALTH_STUNNED, stunned);
}

//------------------------------------------------------------------------------
/**
    IsStunned
*/
inline
bool
ncGameplayLiving::IsStunned() const
{
    return this->ExistFlag (HEALTH_STUNNED);
}

//------------------------------------------------------------------------------
/**
    IsDead
*/
inline
bool
ncGameplayLiving::IsDead() const
{
    return this->ExistFlag (HEALTH_DEAD);
}

//------------------------------------------------------------------------------
/**
    SetInvulnerable
*/
inline
void
ncGameplayLiving::SetInvulnerable (bool invulnerable)
{
    this->ToggleFlag (HEALTH_INVULNERABLE, invulnerable);
}

//------------------------------------------------------------------------------
/**
    IsInvulnerable
*/
inline
bool
ncGameplayLiving::IsInvulnerable() const
{
    return this->ExistFlag (HEALTH_INVULNERABLE);
}

//------------------------------------------------------------------------------
/**
    SetDizzy
*/
inline
void
ncGameplayLiving::SetDizzy (bool dizzy)
{
    this->ToggleFlag (HEALTH_DIZZY, dizzy);
}

//------------------------------------------------------------------------------
/**
    IsDizzy
*/
inline
bool
ncGameplayLiving::IsDizzy() const
{
    return this->ExistFlag (HEALTH_DIZZY);
}

//------------------------------------------------------------------------------
/**
    SetRegenRate
*/
inline
void
ncGameplayLiving::SetRegenRate (int rate)
{
    this->regenRate = rate;
}

//------------------------------------------------------------------------------
/**
    GetRegenRate
*/
inline
int
ncGameplayLiving::GetRegenRate() const
{
    return this->regenRate;
}
//------------------------------------------------------------------------------
/**
    SetBreath
*/
inline
void
ncGameplayLiving::SetBreath (float breath)
{
    this->breath = breath;
}

//------------------------------------------------------------------------------
/**
    GetBreath
*/
inline
float
ncGameplayLiving::GetBreath() const
{
    return this->breath;
}

//------------------------------------------------------------------------------
/**
    SetAttackSpeed
*/
inline
void
ncGameplayLiving::SetAttackSpeed (int attackSpeed)
{
    this->attackSpeed = attackSpeed;
}

//------------------------------------------------------------------------------
/**
    GetAttackSpeed
*/
inline
int
ncGameplayLiving::GetAttackSpeed() const
{
    return this->attackSpeed;
}

//------------------------------------------------------------------------------
/**
    SetBrainStopped
*/
inline
void
ncGameplayLiving::SetBrainStopped (bool value)
{
    this->ToggleFlag (GP_BRAINSTOPPED, value);
}

//------------------------------------------------------------------------------
/**
    IsBrainStopped
*/
inline
bool
ncGameplayLiving::IsBrainStopped() const
{
    return this->ExistFlag (GP_BRAINSTOPPED);
}

//------------------------------------------------------------------------------
/**
    SetFrozen
*/
inline
void
ncGameplayLiving::SetFrozen (bool value)
{
    this->ToggleFlag (GP_FROZEN, value);
}

//------------------------------------------------------------------------------
/**
    IsFrozen
*/
inline
bool
ncGameplayLiving::IsFrozen() const
{
    return this->ExistFlag (GP_FROZEN);
}

//------------------------------------------------------------------------------
/**
    SetSensoryDisabled
*/
inline
void
ncGameplayLiving::SetSensoryDisabled (bool disable)
{
    this->ToggleFlag (PERCEPTION_SENSORYDISABLED, disable);
}

//------------------------------------------------------------------------------
/**
    IsSensoryDisabled
*/
inline
bool
ncGameplayLiving::IsSensoryDisabled() const
{
    return this->ExistFlag (PERCEPTION_SENSORYDISABLED);
}

//------------------------------------------------------------------------------
/**
    SetBlind
*/
inline
void
ncGameplayLiving::SetBlind (bool blind)
{
    this->ToggleFlag (PERCEPTION_BLIND, blind);
}

//------------------------------------------------------------------------------
/**
    IsBlind
*/
inline
bool
ncGameplayLiving::IsBlind() const
{
    return this->ExistFlag (PERCEPTION_BLIND);
}

//------------------------------------------------------------------------------
/**
    SetDeaf
*/
inline
void
ncGameplayLiving::SetDeaf (bool deaf)
{
    this->ToggleFlag (PERCEPTION_DEAF, deaf);
}

//------------------------------------------------------------------------------
/**
    IsDeaf
*/
inline
bool
ncGameplayLiving::IsDeaf() const
{
    return this->ExistFlag (PERCEPTION_DEAF);
}
//------------------------------------------------------------------------------
/**
SetRecovering
*/
inline
void
ncGameplayLiving::SetRecovering (bool recovering)
{
    this->isRecovering = recovering;
}

//------------------------------------------------------------------------------
/**
IsRecovering
*/
inline
bool
ncGameplayLiving::IsRecovering() const
{
    return this->isRecovering;
}
//------------------------------------------------------------------------------
/**
    SetInvisible
*/
inline
void
ncGameplayLiving::SetInvisible (bool invisible)
{
    this->ToggleFlag (PERCEPTION_INVISIBLE, invisible);
}

//------------------------------------------------------------------------------
/**
    IsInvisible
*/
inline
bool
ncGameplayLiving::IsInvisible() const
{
    return this->ExistFlag (PERCEPTION_INVISIBLE);
}

//------------------------------------------------------------------------------
/**
    SetSilent
*/
inline
void
ncGameplayLiving::SetSilent (bool silent)
{
    this->ToggleFlag (PERCEPTION_SILENT, silent);
}

//------------------------------------------------------------------------------
/**
    IsSilent
*/
inline
bool
ncGameplayLiving::IsSilent() const
{
    return this->ExistFlag (PERCEPTION_SILENT);
}

//------------------------------------------------------------------------------
/**
    SetCombatEngage
*/
inline
void
ncGameplayLiving::SetCombatEngage (bool value)
{
    this->ToggleFlag (GP_COMBATENGAGE, value);
}

//------------------------------------------------------------------------------
/**
    IsInCombat
*/
inline
bool
ncGameplayLiving::IsInCombat() const
{
    return this->ExistFlag (GP_COMBATENGAGE);
}

//------------------------------------------------------------------------------
/**
    SetHeld
*/
inline
void
ncGameplayLiving::SetHeld (bool value)
{
    this->ToggleFlag (GP_HELD, value);
}

//------------------------------------------------------------------------------
/**
    IsHeld
*/
inline
bool
ncGameplayLiving::IsHeld() const
{
    return this->ExistFlag (GP_HELD);
}

//------------------------------------------------------------------------------
/**
    SetBlocking
*/
inline
void
ncGameplayLiving::SetBlocking(bool value)
{
    this->ToggleFlag (GP_BLOCKING, value);
}

//------------------------------------------------------------------------------
/**
    IsBlocking
*/
inline
bool
ncGameplayLiving::IsBlocking() const
{
    return this->ExistFlag (GP_BLOCKING);
}

//------------------------------------------------------------------------------
/**
*/
inline
nEntityObject *
ncGameplayLiving::GetCurrentWeapon()const
{
    return this->currentWeapon;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
ncGameplayLiving::ApplyDamage( int damage )
{
    int life = this->health - damage;
    if( life < 0 )
    {
        life = 0;
    }
    this->SetHealth( life );
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
ncGameplayLiving::SetIronsight( bool flag )
{
    if( this->currentWeapon )
    {
        n_assert( this->currentWeapon->GetComponent<ncGPWeapon>() );
        this->currentWeapon->GetComponent<ncGPWeapon>()->SetIronsight( flag );
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncGameplayLiving::IsIronsight() const
{
    bool ironsight = false;

    if( this->currentWeapon )
    {
        n_assert( this->currentWeapon->GetComponent<ncGPWeapon>() );
        ironsight = this->currentWeapon->GetComponent<ncGPWeapon>()->GetIronsight();
    }

    return ironsight;
}

//------------------------------------------------------------------------------
/**
    SetCarriedBy
*/
inline
void
ncGameplayLiving::SetCarriedBy (nEntityObject* entity)
{
    this->carriedBy = entity;
}

//------------------------------------------------------------------------------
/**
    GetCarriedBy
*/
inline
nEntityObject*
ncGameplayLiving::GetCarriedBy() const
{
    return this->carriedBy;
}

//------------------------------------------------------------------------------
/**
	IsSprinting
*/
inline
bool
ncGameplayLiving::IsSprinting() const
{
	return this->isSprint; 
}

//------------------------------------------------------------------------------
/**
	SetSprinting
*/
inline
void
ncGameplayLiving::SetSprinting (bool sprint)
{
	this->isSprint = sprint;
}

//------------------------------------------------------------------------------
/**
	IsCrouching
*/
inline
bool
ncGameplayLiving::IsCrouching() const
{
	return this->isCrouch;
}

//------------------------------------------------------------------------------
/**
	SetCrouching
*/
inline
void
ncGameplayLiving::SetCrouching (bool crouch)
{
	this->isCrouch = crouch;

    if( this->currentWeapon )
    {
        ncGPWeapon * weapon = this->currentWeapon->GetComponentSafe<ncGPWeapon>();
        if( weapon )
        {
            weapon->SetCrouch( crouch );
        }
    }
}

//------------------------------------------------------------------------------
/**
	IsCrawling
*/
inline
bool
ncGameplayLiving::IsProne() const
{
	return this->isProne;
}

//------------------------------------------------------------------------------
/**
	SetProning
*/
inline
void
ncGameplayLiving::SetProning (bool prone)
{
	this->isProne = prone;
    if( this->currentWeapon )
    {
        ncGPWeapon * weapon = this->currentWeapon->GetComponentSafe<ncGPWeapon>();
        if( weapon )
        {
            weapon->SetProne( prone );
        }
    }
}

//------------------------------------------------------------------------------
/**
	IsSwimming
*/
inline
bool
ncGameplayLiving::IsSwimming() const
{
	return this->isSwimming;
}

//------------------------------------------------------------------------------
/**
	SetSwimming
*/
inline
void
ncGameplayLiving::SetSwimming (bool swimming)
{
	this->isSwimming = swimming;
}

//------------------------------------------------------------------------------
/**
    Get the fighting rings
*/
/** ZOMBIE REMOVE
inline
nFightRingManager* 
ncGameplayLiving::GetRingsManager() const
{
    return this->ringsManager;
}
*/
//------------------------------------------------------------------------------
/**
    @param value new busy level
*/
inline
void
ncGameplayLiving::SetBusy( int value )
{
    this->busyLevel = value;
}

//------------------------------------------------------------------------------
/**
    @returns the busy level
*/
inline
int
ncGameplayLiving::GetBusy()const
{
    return this->busyLevel;
}

//------------------------------------------------------------------------------
/**
    @returns true if living entity is busy
*/
inline
bool
ncGameplayLiving::IsBusy()const
{
    return ( this->busyLevel != 0 );
}

//------------------------------------------------------------------------------
/**
    Set the entity as impacted or not
*/
inline
void 
ncGameplayLiving::SetImpacted(bool flag)
{
    this->impacted = flag;
}

//------------------------------------------------------------------------------
/**
    Says if the entity has been impacted
*/
inline
bool 
ncGameplayLiving::IsImpacted() const
{
    return this->impacted;
}

//------------------------------------------------------------------------------
/**
    Set the impact's animation index
*/
inline
void
ncGameplayLiving::SetImpactAnimIndex(int index)
{
    this->impactAnimIndex = index;
}

//------------------------------------------------------------------------------
/**
    Set the impact's animation index
*/
inline
int
ncGameplayLiving::GetImpactAnimIndex() const
{
    return this->impactAnimIndex;
}

#endif
