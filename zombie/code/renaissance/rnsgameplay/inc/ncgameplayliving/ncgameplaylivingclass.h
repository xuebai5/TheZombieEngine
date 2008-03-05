/**
    @file ncgameplaylivingclass.h

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#ifndef NCGAMEPLAYLIVINGCLASS_H
#define NCGAMEPLAYLIVINGCLASS_H

//------------------------------------------------------------------------------
#include "rnsgameplay/ncgameplayclass.h"

//------------------------------------------------------------------------------
class nPersistServer;

//------------------------------------------------------------------------------
class ncGameplayLivingClass : public ncGameplayClass
{

    NCOMPONENT_DECLARE(ncGameplayLivingClass,ncGameplayClass);

    enum 
    {
        HEALTH_HASREGEN             = 1<<0,
        HEALTH_CANBESTUNNED         = 1<<1,
        HEALTH_CANBEINCAPACITATED   = 1<<2,
        HEALTH_CANBEKILLED          = 1<<3,
        HEALTH_NEEDSAIR             = 1<<4,

        MOVEMENT_CANBECARRIED       = 1<<5,
        MOVEMENT_PICKUP             = 1<<6, // ??
    };

public:

    enum BodyPart
    {
        BP_HEAD = 0,
        BP_ARM,
        BP_LEG,
        BP_TORSO,

        NUM_BODY_PARTS
    };

    typedef struct
    {
        float radius;
        int size;
    } RingInfo;

    /// constructor
    ncGameplayLivingClass();
    /// destructor
    ~ncGameplayLivingClass();

    /// Set the maximum orientation change
    void SetMaxAngularVelocity( float velocity );
    /// Get the maximum orientation change
    float GetMaxAngularVelocity() const;

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);	

	/// Sets/gets the base speed
 void SetBaseSpeed(float);
 float GetBaseSpeed() const;

	/// Sets/gets the multiplicator speed of the component
    void SetMultSpeed(int);
    int GetMultSpeed() const;

    /// Get real speed
    float GetSpeed() const;

    /// Sets/gets the multiplicator run speed of the component
    void SetMultRunSpeed(int);
    int GetMultRunSpeed() const;

    /// Get the real run speed
    float GetRunSpeed() const;

    /// Sets/gets the ironsight speed of the component
 void SetMultIronsightSpeed(int);
    int GetMultIronsightSpeed() const;

    /// Get the real iron sight speed
    float GetIronsightSpeed() const;

	/// Sets/gets the multiplicator run speed while is in iron sight mode
 void SetMultRunIronsightSpeed(int);
    int GetMultRunIronsightSpeed() const;

    /// gets the real run iron sight speed
    float GetRunIronsightSpeed() const;

	/// Sets/gets the crouch speed of the component
 void SetMultCrouchSpeed(int);
    int GetMultCrouchSpeed() const;

    /// gets the crouch speed
    float GetCrouchSpeed() const;

	/// Sets/gets the prone speed
 void SetMultProneSpeed(int);
    int GetMultProneSpeed() const;

    /// Gets the real prone speed
    float GetProneSpeed() const;

	/// Sets/gets the crouch speed when iron sight mode
 void SetMultCrouchIronSightSpeed(int);
    int GetMultCrouchIronSightSpeed() const;

    ///Gets the real crouch iron sight speed
    float GetCrouchIronSightSpeed() const;

	/// Sets/gets the prone speed when iron sight mode
 void SetMultProneIronSightSpeed(int);
    int GetMultProneIronSightSpeed() const;

    /// Gets the real prone iron sight speed
    float GetProneIronSightSpeed() const;

	/// Sets/gets the swim speed
 void SetMultSwimSpeed(int);
    int GetMultSwimSpeed() const;

    /// Gets the real swim speed
    float GetSwimSpeed() const;

    /// Sets/gets the turnSpeed of the component
    void SetTurnSpeed(int);
    float GetTurnSpeed() const;
    /// Sets/gets the turnRadius of the component
    void SetTurnRadius(float);
    float GetTurnRadius() const;
    /// Sets/gets the maximum orientation change in degrees per second
    void SetMaxAngularVelocityInDegrees(float);
    float GetMaxAngularVelocityInDegrees() const;
    /// Sets/gets the jump height
    void SetJumpHeight(float);
    float GetJumpHeight() const;

    /// Sets/gets the jump speed
    void SetJumpSpeed(float);
    float GetJumpSpeed() const;
    
    /// Sets/gets the size of the component
    void SetSize(float);
    float GetSize() const;
    /// Sets/gets the max climb slope
    void SetMaxClimbSlope(float);
    float GetMaxClimbSlope() const;
    /// Sets/gets the falling damage ratio
    void SetFallingDamageRatio(float);
    float GetFallingDamageRatio() const;
    /// Sets/gets the eye height
    void SetEyeHeight(float);
    float GetEyeHeight() const;
    /// Sets/gets the 'use' range
    void SetUseRange(float);
    float GetUseRange() const;
    /// Sets/gets steps per anim cycle
    void SetStepsAnimCycle(int);
    int GetStepsAnimCycle() const;
    /// Sets/gets if can be carried
    void SetCanBeCarried(bool);
    bool CanBeCarried() const;
    /// Sets/gets pickup property
    void SetPickUp(bool);
    bool GetPickUp() const;
    /// Sets/gets the size inventory
    void SetInventorySize(int);
    int GetInventorySize() const;
    // -- Health methods
    /// Sets/gets the value for the max health
    void SetMaxHealth(int);
    int GetMaxHealth() const;
    /// Sets/gets the value for the regen threshold
    void SetRegenThreshold(float);
    float GetRegenThreshold() const;
    /// Sets/gets the value for the regent amount
    void SetRegenAmount(float);
    float GetRegenAmount() const;
    /// Sets/gets the state (for persistence only)
    void SetGameplayLivingState(unsigned int);
    unsigned int GetGameplayLivingState() const;
    /// Sets/gets 'can be stunned' property
    void SetCanBeStunned(bool);
    bool CanBeStunned() const;
    /// Sets/gets 'can be incapatitated' property
    void SetCanBeIncapacitated(bool);
    bool CanBeIncapacitated() const;
    /// Sets/gets 'can be killed' property
    void SetCanBeKilled(bool);
    bool CanBeKilled() const;
    //-- Breath methods
    /// Sets/gets if the entity needs air
    void SetNeedsAir(bool);
    bool GetNeedsAir() const;
    /// Sets/gets breathMax
    void SetBreathMax(float);
    float GetBreathMax() const;
    /// Sets/gets breathLossAmount
    void SetBreathLossAmount(float);
    float GetBreathLossAmount() const;
    /// Sets/gets breathLossSpeed
    void SetBreathLossSpeed(float);
    float GetBreathLossSpeed() const;
    /// Sets/gets breathRecoverySpeed
    void SetBreathRecoverySpeed(float);
    float GetBreathRecoverySpeed() const;
    //-- Perception methods
    /// Sets/gets sight radius
    void SetSightRadius(float);
    float GetSightRadius() const;
    /// Sets/gets FOV angle
    void SetFOVAngle(float);
    float GetFOVAngle() const;
    /// Sets/gets hearing radius
    void SetHearingRadius(float);
    float GetHearingRadius() const;
    /// Sets/gets feeling radius
    void SetFeelingRadius(float);
    float GetFeelingRadius() const;
    /// Sets/gets memory time
    void SetMemoryTime(int);
    int GetMemoryTime() const;

    //-- Communication methods
    /// Sets/gets communication radius
    void SetCommRadius(float);
    float GetCommRadius() const;
    /// Sets/gets communication intensity
    void SetCommIntensity(float);
    float GetCommIntensity() const;

    //-- Combat methods
    /// Sets/gets melee range
    void SetMeleeRange(float);
    float GetMeleeRange() const;

    /// Sets/gets attack speed
    void SetAttackSpeed(int);
    float GetAttackSpeed() const;
    /// Sets/gets damage modification for a concrete body part
    void SetDamageModification(BodyPart, float);
    float GetDamageModification(BodyPart);

    /// Reset rings information
    void ResetRingsInfo() const;
    /// Add a fight ring information
    void AddFightRing(float, int);
    /// Remove a fight ring information
    void RemoveFightRing(float, int);
    /// Remove a fight ring information using an index
    void RemoveFightRingIndex(int);
    
																
    /// Get fight rings information array
    nArray<ncGameplayLivingClass::RingInfo>* GetFightRingsInfo() const;

protected:

	float	baseSpeed;

    // Real speeds are calculated through this percentages
    int	multSpeed;
    int	multRunSpeed;
    int	multIronsightSpeed;
    int	multRunIronsightSpeed;		
    int	multCrouchSpeed;			
    int	multProneSpeed;				
    int	multCrouchIronsightSpeed;	
    int	multProneIronSightSpeed;
    int	multSwimSpeed;
    int     turnSpeed;

private:
    /// Auxiliary methods for flag management
    void ToggleFlag (unsigned int flag, bool on);

    /// Attributes of the game play class
    float   turnRadius;
    float   maxAngularVelocity;
    float   jumpHeight;
    float   jumpSpeed;
    float   size;
    float   maxClimbSlope;
    float   fallingDamageRatio;
    float   eyeHeight;
    float   useRange;               // Range at which this agent can "use" other objects around it
    int     stepsAnimCycle;         // steps per anim cycle, used for sound

    /// Inventory attributes ----------------------------
    int     inventorySize;

    /// Health attributes -------------------------------
    int     maxHealth;
    float   regenThreshold;         // Really, only can get values at the range 0..1 (it's just a percentage)
    float   regenAmount;

    /// Breath attributes -------------------------------
    float   breathMax;
    float   breathLossAmount;
    float   breathLossSpeed;
    float   breathRecoverySpeed;

    /// Perception attributes ---------------------------
    float   sightRadius;
    float   FOVAngle;
    float   hearingRadius;
    float   feelingRadius;
    int     memoryTime;
    
    /// Communication attributes ------------------------
    float   commRadius;
    float   commIntensity;

    /// Combat attributes -------------------------------
    float   meleeRange;
    int		attackSpeed;

    /// Body parts damage modification
    float bodyPartsDamage[NUM_BODY_PARTS];

    /// State attributes
    unsigned int state;

    /// Fight rings information array
    nArray<ncGameplayLivingClass::RingInfo> *ringsInfo;

};

//------------------------------------------------------------------------------
/**
	SetBaseSpeed
*/
inline
void 
ncGameplayLivingClass::SetBaseSpeed (float speed)
{
	this->baseSpeed = speed;
}

//------------------------------------------------------------------------------
/**
	GetBaseSpeed
*/
inline
float
ncGameplayLivingClass::GetBaseSpeed() const
{
	return this->baseSpeed;
}

//------------------------------------------------------------------------------
/**
	SetMultIronsightSpeed
*/
inline
void
ncGameplayLivingClass::SetMultIronsightSpeed (int speed)
{
	this->multIronsightSpeed = speed;
}

//------------------------------------------------------------------------------
/**
    GetMultIronsightSpeed
*/
inline
int
ncGameplayLivingClass::GetMultIronsightSpeed() const
{
    return this->multIronsightSpeed;
}

//------------------------------------------------------------------------------
/**
	GetIronsightSpeed
*/
inline
float
ncGameplayLivingClass::GetIronsightSpeed() const
{
	return static_cast<float>(this->multIronsightSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
    SetMultSpeed
*/
inline
void
ncGameplayLivingClass::SetMultSpeed (int speed)
{
    this->multSpeed = speed;
}

//------------------------------------------------------------------------------
/**
    GetMultSpeed
*/
inline
int
ncGameplayLivingClass::GetMultSpeed() const
{
    return this->multSpeed;
}

//------------------------------------------------------------------------------
/**
    GetSpeed
*/
inline
float
ncGameplayLivingClass::GetSpeed() const
{
    return static_cast<float>(this->multSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
    SetMultRunSpeed
*/
inline
void
ncGameplayLivingClass::SetMultRunSpeed (int speed)
{
    this->multRunSpeed = speed;
}

//------------------------------------------------------------------------------
/**
    GetMultRunSpeed
*/
inline
int
ncGameplayLivingClass::GetMultRunSpeed() const
{
    return this->multRunSpeed;
}

//------------------------------------------------------------------------------
/**
    GetRunSpeed
*/
inline
float
ncGameplayLivingClass::GetRunSpeed() const
{
    return static_cast<float>(this->multRunSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
	SetMultRunIronsightSpeed
*/
inline
void
ncGameplayLivingClass::SetMultRunIronsightSpeed (int speed)
{
	this->multRunIronsightSpeed = speed;
}

//------------------------------------------------------------------------------
/**
    GetRunIronsightSpeed
*/
inline
int
ncGameplayLivingClass::GetMultRunIronsightSpeed() const
{
    return this->multRunIronsightSpeed;
}

//------------------------------------------------------------------------------
/**
	GetRunIronsightSpeed
*/
inline
float
ncGameplayLivingClass::GetRunIronsightSpeed() const
{
	return static_cast<float>(this->multRunIronsightSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
	SetCrouchSpeed
*/
inline
void 
ncGameplayLivingClass::SetMultCrouchSpeed (int speed)
{
	this->multCrouchSpeed = speed;
}

//------------------------------------------------------------------------------
/**
    GetMultCrouchSpeed
*/
inline
int
ncGameplayLivingClass::GetMultCrouchSpeed() const
{
    return this->multCrouchSpeed;
}

//------------------------------------------------------------------------------
/**
	GetCrouchSpeed
*/
inline
float
ncGameplayLivingClass::GetCrouchSpeed() const
{
	return static_cast<float>(this->multCrouchSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
	SetMultProneSpeed
*/
inline
void 
ncGameplayLivingClass::SetMultProneSpeed (int speed)
{
	this->multProneSpeed = speed;
}

//------------------------------------------------------------------------------
/**
    GetMultProneSpeed
*/
inline
int
ncGameplayLivingClass::GetMultProneSpeed() const
{
    return this->multProneSpeed;
}

//------------------------------------------------------------------------------
/**
	GetProneSpeed
*/
inline
float
ncGameplayLivingClass::GetProneSpeed() const
{
	return static_cast<float>(this->multProneSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
	SetMultCrouchIronsightSpeed
*/
inline
void
ncGameplayLivingClass::SetMultCrouchIronSightSpeed (int speed)
{
	this->multCrouchIronsightSpeed = speed;
}

//------------------------------------------------------------------------------
/**
	GetMultCrouchIronsightSpeed
*/
inline
int
ncGameplayLivingClass::GetMultCrouchIronSightSpeed() const
{
	return this->multCrouchIronsightSpeed;
}

//------------------------------------------------------------------------------
/**
    GetCrouchIronsightSpeed
*/
inline
float
ncGameplayLivingClass::GetCrouchIronSightSpeed() const
{
    return static_cast<float>(this->multCrouchIronsightSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
	SetMultProneIronsightSpeed
*/
inline
void
ncGameplayLivingClass::SetMultProneIronSightSpeed (int speed)
{
	this->multProneIronSightSpeed = speed;
}

//------------------------------------------------------------------------------
/**
GetProneIronsightSpeed
*/
inline
int
ncGameplayLivingClass::GetMultProneIronSightSpeed() const
{
    return this->multProneIronSightSpeed;

}

//------------------------------------------------------------------------------
/**
	GetProneIronsightSpeed
*/
inline
float
ncGameplayLivingClass::GetProneIronSightSpeed() const
{
	return static_cast<float>(this->multProneIronSightSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
	SetMultSwimSpeed
*/
inline
void
ncGameplayLivingClass::SetMultSwimSpeed (int speed)
{
	this->multSwimSpeed = speed;
}

//------------------------------------------------------------------------------
/**
    GetMultSwimSpeed
*/
inline
int
ncGameplayLivingClass::GetMultSwimSpeed() const
{
    return this->multSwimSpeed;
}

//------------------------------------------------------------------------------
/**
	GetSwimSpeed
*/
inline
float
ncGameplayLivingClass::GetSwimSpeed() const
{
	return static_cast<float>(this->multSwimSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
    Sets the turnSpeed of the component
*/
inline
void 
ncGameplayLivingClass::SetTurnSpeed(int turnSpeed)
{
    this->turnSpeed = turnSpeed;
}

//------------------------------------------------------------------------------
/**
    Gets the turnSpeed of the component
*/
inline
float
ncGameplayLivingClass::GetTurnSpeed() const
{
    return static_cast<float>(this->turnSpeed) * this->GetBaseSpeed() * 0.01f;
}

//------------------------------------------------------------------------------
/**
    SetTurnRadius
*/
inline
void
ncGameplayLivingClass::SetTurnRadius (float turnRadius)
{
    this->turnRadius = turnRadius;
}

//------------------------------------------------------------------------------
/**
    GetTurnRadius
*/
inline
float
ncGameplayLivingClass::GetTurnRadius() const
{
    return this->turnRadius;
}

//------------------------------------------------------------------------------
/**
    Set the maximum orientation change
*/
inline
void 
ncGameplayLivingClass::SetMaxAngularVelocity( float velocity )
{
    this->maxAngularVelocity = velocity;
}

//------------------------------------------------------------------------------
/**
    Get the maximum orientation change
*/
inline
float
ncGameplayLivingClass::GetMaxAngularVelocity() const
{
    return this->maxAngularVelocity;
}

//------------------------------------------------------------------------------
/**
    SetJumpHeight
*/
inline
void
ncGameplayLivingClass::SetJumpHeight (float jumpHeight)
{
    this->jumpHeight = jumpHeight;
}

//------------------------------------------------------------------------------
/**
    GetJumpHeight
*/
inline
float
ncGameplayLivingClass::GetJumpHeight() const
{
    return this->jumpHeight;
}

//------------------------------------------------------------------------------
/**
    SetEyeHeight
*/
inline
void
ncGameplayLivingClass::SetEyeHeight (float eyeHeight)
{
    this->eyeHeight = eyeHeight;
}

//------------------------------------------------------------------------------
/**
    GetEyeHeight
*/
inline
float
ncGameplayLivingClass::GetEyeHeight() const
{
    return this->eyeHeight;
}

//------------------------------------------------------------------------------
/**
    SetUseRange
*/
inline
void
ncGameplayLivingClass::SetUseRange (float useRange)
{
    this->useRange = useRange;
}

//------------------------------------------------------------------------------
/**
    GetUseRange
*/
inline
float
ncGameplayLivingClass::GetUseRange() const
{
    return this->useRange;
}

//------------------------------------------------------------------------------
/**
    SetStepsAnimCycle
*/
inline
void
ncGameplayLivingClass::SetStepsAnimCycle (int stepsAnimCycle)
{
    this->stepsAnimCycle = stepsAnimCycle;
}

//------------------------------------------------------------------------------
/**
    GetStepsAnimCycle
*/
inline
int
ncGameplayLivingClass::GetStepsAnimCycle() const
{
    return this->stepsAnimCycle;
}

//------------------------------------------------------------------------------
/**
    SetInventorySize
*/
inline
void
ncGameplayLivingClass::SetInventorySize (int inventorySize)
{
    this->inventorySize = inventorySize;
}

//------------------------------------------------------------------------------
/**
    GetInventorySize
*/
inline
int
ncGameplayLivingClass::GetInventorySize() const
{
    return this->inventorySize;
}

//------------------------------------------------------------------------------
/**
    SetCanBeCarried
*/
inline
void
ncGameplayLivingClass::SetCanBeCarried (bool value)
{
    this->ToggleFlag (MOVEMENT_CANBECARRIED, value);
}

//------------------------------------------------------------------------------
/**
    CanBeCarried
*/
inline
bool
ncGameplayLivingClass::CanBeCarried() const
{
    return (this->state&MOVEMENT_CANBECARRIED) != 0;
}

//------------------------------------------------------------------------------
/**
    SetPickUp
*/
inline
void
ncGameplayLivingClass::SetPickUp (bool value)
{
    this->ToggleFlag (MOVEMENT_PICKUP, value);
}

//------------------------------------------------------------------------------
/**
    GetPickUp
*/
inline
bool
ncGameplayLivingClass::GetPickUp() const
{
    return (this->state&MOVEMENT_PICKUP) != 0;
}

//------------------------------------------------------------------------------
/**
    SetSize
*/
inline
void
ncGameplayLivingClass::SetSize (float size)
{
    this->size = size;
}

//------------------------------------------------------------------------------
/**
    GetSize
*/
inline
float
ncGameplayLivingClass::GetSize() const
{
    return this->size;
}

//------------------------------------------------------------------------------
/**
    Sets the jump speed
*/
inline
void 
ncGameplayLivingClass::SetJumpSpeed(float speed)
{
    this->jumpSpeed = speed;
}

//------------------------------------------------------------------------------
/**
    Sets the jump speed
*/
inline
float 
ncGameplayLivingClass::GetJumpSpeed() const
{
    return this->jumpSpeed;
}

//------------------------------------------------------------------------------
/**
    SetMaxClimbSlope
*/
inline
void
ncGameplayLivingClass::SetMaxClimbSlope (float maxClimbSlope)
{
    this->maxClimbSlope = maxClimbSlope;
}

//------------------------------------------------------------------------------
/**
    GetMaxClimbSlope
*/
inline
float
ncGameplayLivingClass::GetMaxClimbSlope() const
{
    return this->maxClimbSlope;
}

//------------------------------------------------------------------------------
/**
    SetFallingDamageRatio
*/
inline
void
ncGameplayLivingClass::SetFallingDamageRatio (float ratio)
{
    this->fallingDamageRatio = ratio;
}

//------------------------------------------------------------------------------
/**
    GetFallingDamageRatio
*/
inline
float
ncGameplayLivingClass::GetFallingDamageRatio() const
{
    return this->fallingDamageRatio;
}

//------------------------------------------------------------------------------
/**
    SetMaxHealth
*/
inline
void
ncGameplayLivingClass::SetMaxHealth (int value)
{
    this->maxHealth = value;
}

//------------------------------------------------------------------------------
/**
    GetMaxHealth
*/
inline
int
ncGameplayLivingClass::GetMaxHealth() const
{
    return this->maxHealth;
}

//------------------------------------------------------------------------------
/**
    SetRegenThreshold
*/
inline
void
ncGameplayLivingClass::SetRegenThreshold (float value)
{
    this->regenThreshold = value;
}

//------------------------------------------------------------------------------
/**
    GetRegenThreshold
*/
inline
float
ncGameplayLivingClass::GetRegenThreshold() const
{
    return this->regenThreshold;
}

//------------------------------------------------------------------------------
/**
    SetRegenAmount
*/
inline
void
ncGameplayLivingClass::SetRegenAmount (float value)
{
    this->regenAmount = value;
}

//------------------------------------------------------------------------------
/**
    GetRegenAmount
*/
inline
float
ncGameplayLivingClass::GetRegenAmount() const
{
    return this->regenAmount;
}

//------------------------------------------------------------------------------
/**
    SetGameplayLivingState
*/
inline
void
ncGameplayLivingClass::SetGameplayLivingState (unsigned int state)
{
    this->state = state;
}

//------------------------------------------------------------------------------
/**
    GetGameplayLivingState
*/
inline
unsigned int
ncGameplayLivingClass::GetGameplayLivingState() const
{
    return this->state;
}

//------------------------------------------------------------------------------
/**
    ToggleFlag
*/
inline
void
ncGameplayLivingClass::ToggleFlag (unsigned int flag, bool on)
{
    if ( on )
    {
        this->state |= flag;
    }
    else
    {
        this->state &= flag;
    }
}

//------------------------------------------------------------------------------
/**
    SetCanBeStunned
*/
inline
void
ncGameplayLivingClass::SetCanBeStunned (bool value) 
{
    this->ToggleFlag (HEALTH_CANBESTUNNED, value);
}

//------------------------------------------------------------------------------
/**
    CanBeStunned
*/
inline
bool
ncGameplayLivingClass::CanBeStunned() const
{
    return (this->state&HEALTH_CANBESTUNNED) !=0;
}

//------------------------------------------------------------------------------
/**
    SetCanBeIncapacitated
*/
inline
void
ncGameplayLivingClass::SetCanBeIncapacitated (bool value)
{
    this->ToggleFlag (HEALTH_CANBEINCAPACITATED, value);
}

//------------------------------------------------------------------------------
/**
    CanBeIncapacitated
*/
inline
bool
ncGameplayLivingClass::CanBeIncapacitated() const
{
    return (this->state&HEALTH_CANBEINCAPACITATED) != 0;
}

//------------------------------------------------------------------------------
/**
    SetCanBeKilled
*/
inline
void
ncGameplayLivingClass::SetCanBeKilled (bool value)
{
    this->ToggleFlag (HEALTH_CANBEKILLED, value);
}

//------------------------------------------------------------------------------
/**
    CanBeKilled
*/
inline
bool
ncGameplayLivingClass::CanBeKilled() const
{
    return (this->state&HEALTH_CANBEKILLED) != 0;
}

//------------------------------------------------------------------------------
/**
    SetNeedsAir
*/
inline
void
ncGameplayLivingClass::SetNeedsAir (bool value)
{
    this->ToggleFlag (HEALTH_NEEDSAIR, value);
}

//------------------------------------------------------------------------------
/**
    GetNeedsAir
*/
inline
bool
ncGameplayLivingClass::GetNeedsAir() const
{
    return (this->state&HEALTH_NEEDSAIR) != 0;
}

//------------------------------------------------------------------------------
/**
    SetBreathMax
*/
inline
void
ncGameplayLivingClass::SetBreathMax (float breathMax)
{
    this->breathMax = breathMax;
}

//------------------------------------------------------------------------------
/**
    GetBreathMax
*/
inline
float
ncGameplayLivingClass::GetBreathMax() const
{
    return this->breathMax;
}

//------------------------------------------------------------------------------
/**
    SetSightRadius
*/
inline
void
ncGameplayLivingClass::SetSightRadius (float radius)
{
    this->sightRadius = radius;
}

//------------------------------------------------------------------------------
/**
    GetSightRadius
*/
inline
float
ncGameplayLivingClass::GetSightRadius() const
{
    return this->sightRadius;
}

//------------------------------------------------------------------------------
/**
    SetBreathLossAmount
*/
inline
void
ncGameplayLivingClass::SetBreathLossAmount (float breathLossAmount)
{
    this->breathLossAmount = breathLossAmount;
}

//------------------------------------------------------------------------------
/**
    GetBreathLossAmount
*/
inline
float
ncGameplayLivingClass::GetBreathLossAmount() const
{
    return this->breathLossAmount;
}

//------------------------------------------------------------------------------
/**
    SetBreathLossSpeed
*/
inline
void
ncGameplayLivingClass::SetBreathLossSpeed (float breathLossSpeed)
{
    this->breathLossSpeed = breathLossSpeed;
}

//------------------------------------------------------------------------------
/**
    GetBreathLossSpeed
*/
inline
float
ncGameplayLivingClass::GetBreathLossSpeed() const
{
    return this->breathLossSpeed;
}

//------------------------------------------------------------------------------
/**
    SetBreathRecoverySpeed
*/
inline
void
ncGameplayLivingClass::SetBreathRecoverySpeed (float breathRecoverySpeed)
{
    this->breathRecoverySpeed = breathRecoverySpeed;
}

//------------------------------------------------------------------------------
/**
    GetBreathRecoverySpeed
*/
inline
float
ncGameplayLivingClass::GetBreathRecoverySpeed() const
{
    return this->breathRecoverySpeed;
}

//------------------------------------------------------------------------------
/**
    SetFOVAngle
*/
inline
void
ncGameplayLivingClass::SetFOVAngle (float angle)
{
    this->FOVAngle = angle;
}

//------------------------------------------------------------------------------
/**
    GetFOVAngle
*/
inline
float
ncGameplayLivingClass::GetFOVAngle() const
{
    return this->FOVAngle;
}

//------------------------------------------------------------------------------
/**
    SetHearingRadius
*/
inline
void
ncGameplayLivingClass::SetHearingRadius (float radius)
{
    this->hearingRadius = radius;
}

//------------------------------------------------------------------------------
/**
    GetHearingRadius
*/
inline
float
ncGameplayLivingClass::GetHearingRadius() const
{
    return this->hearingRadius;
}

//------------------------------------------------------------------------------
/**
    SetMemoryTime
*/
inline
void
ncGameplayLivingClass::SetMemoryTime(int time)
{
    this->memoryTime = time;
}

//------------------------------------------------------------------------------
/**
    GetMemoryTime
*/
inline
int 
ncGameplayLivingClass::GetMemoryTime() const
{
    return this->memoryTime;
}

//------------------------------------------------------------------------------
/**
    SetFeelingRadius
*/
inline
void
ncGameplayLivingClass::SetFeelingRadius (float radius)
{
    this->feelingRadius = radius;
}

//------------------------------------------------------------------------------
/**
    GetFeelingRadius
*/
inline
float
ncGameplayLivingClass::GetFeelingRadius() const
{
    return this->feelingRadius;
}

//------------------------------------------------------------------------------
/**
    SetCommRadius
*/
inline
void
ncGameplayLivingClass::SetCommRadius (float radius)
{
    this->commRadius = radius;
}

//------------------------------------------------------------------------------
/**
    GetCommRadius
*/
inline
float
ncGameplayLivingClass::GetCommRadius() const
{
    return this->commRadius;
}

//------------------------------------------------------------------------------
/**
    SetCommIntensity
*/
inline
void
ncGameplayLivingClass::SetCommIntensity (float intensity)
{
    this->commIntensity = intensity;
}

//------------------------------------------------------------------------------
/**
    GetCommIntensity
*/
inline
float
ncGameplayLivingClass::GetCommIntensity() const
{
    return this->commIntensity;
}

//------------------------------------------------------------------------------
/**
    SetMeleeRange
*/
inline
void
ncGameplayLivingClass::SetMeleeRange (float range)
{
    this->meleeRange = range;
}

//------------------------------------------------------------------------------
/**
    GetMeleeRange
*/
inline
float
ncGameplayLivingClass::GetMeleeRange() const
{
    return this->meleeRange;
}

//------------------------------------------------------------------------------
/**
    SetAttackSpeed
*/
inline
void
ncGameplayLivingClass::SetAttackSpeed (int speed)
{
    this->attackSpeed = speed;
}

//------------------------------------------------------------------------------
/**
    GetAttackSpeed
*/
inline
float
ncGameplayLivingClass::GetAttackSpeed() const
{
    return float(this->attackSpeed) * this->GetBaseSpeed() / 100.f;
}

//------------------------------------------------------------------------------
/**
    SetDamageModification
*/
inline
void
ncGameplayLivingClass::SetDamageModification(BodyPart bPart, float damageModif)
{
    this->bodyPartsDamage[bPart] = damageModif;
}

//------------------------------------------------------------------------------
/**
    SetDamageModification
*/
inline
float
ncGameplayLivingClass::GetDamageModification(BodyPart bPart)
{
    return this->bodyPartsDamage[bPart];
}

//------------------------------------------------------------------------------
/**
    Add a fight ring information
*/
inline
void 
ncGameplayLivingClass::AddFightRing(float rad, int siz)
{
    ncGameplayLivingClass::RingInfo newRingInfo;
    newRingInfo.radius = rad;
    newRingInfo.size = siz;
    this->ringsInfo->Append(newRingInfo);
}

//------------------------------------------------------------------------------
/**
    Remove a fight ring information
*/
inline
void 
ncGameplayLivingClass::RemoveFightRing(float rad, int siz)
{
    for (int i = 0; i < this->ringsInfo->Size(); i++)
    {
        if ( (this->ringsInfo->At(i).radius == rad) && 
             (this->ringsInfo->At(i).size == siz) )
        {
            this->ringsInfo->EraseQuick(i);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Remove a fight ring information using an index
*/
inline
void 
ncGameplayLivingClass::RemoveFightRingIndex(int index)
{
    this->ringsInfo->EraseQuick(index);
}

//------------------------------------------------------------------------------
/**
    Get fight rings information array
*/
inline
nArray<ncGameplayLivingClass::RingInfo>*
ncGameplayLivingClass::GetFightRingsInfo() const
{
    return this->ringsInfo;
}

//------------------------------------------------------------------------------

N_CMDARGTYPE_NEW_TYPE(ncGameplayLivingClass::BodyPart, "i", (value = (ncGameplayLivingClass::BodyPart) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );

//------------------------------------------------------------------------------

#endif
