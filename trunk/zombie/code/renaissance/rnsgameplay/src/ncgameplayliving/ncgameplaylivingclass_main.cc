#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncgameplaylivingclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncgameplayliving/ncgameplaylivingclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncGameplayLivingClass,ncGameplayClass);

//------------------------------------------------------------------------------
/**
*/
ncGameplayLivingClass::ncGameplayLivingClass() : ncGameplayClass(), 
	baseSpeed		            (1.5f),
	multSpeed                   (100), 
	multRunSpeed		        (200),	
	multCrouchSpeed		        (66),
	multProneSpeed		        (33),
    multIronsightSpeed          (66),
	multCrouchIronsightSpeed    (33),
	multProneIronSightSpeed     (0),
	multRunIronsightSpeed       (0),
	multSwimSpeed		        (66),
    turnSpeed       (66),
	turnRadius      (0.5f),
    maxAngularVelocity (0.5f * N_PI),
	jumpHeight      (2.f),
    jumpSpeed       (4.f),
	size            (0.39f),
	maxClimbSlope   (0.5f),
	fallingDamageRatio (0.5f),
	eyeHeight       (1.8f),
	useRange        (2.f),
    stepsAnimCycle  (2),
	inventorySize   (1),
	maxHealth       (1000),
	regenThreshold  (0.5f),
	regenAmount     (0),
	memoryTime      (50),
	commRadius      (10.f),
	commIntensity   (1.f),
	sightRadius     (15.f),
	hearingRadius   (50.f),
	feelingRadius   (2.0f),
	meleeRange      (0.5f),
	attackSpeed     (150),
    breathMax(0),
    breathLossAmount(0),
    breathLossSpeed(0),
    breathRecoverySpeed(0)
{
    // @TODO: Use a script for init all class attributes
    this->SetCanBeIncapacitated (false);
    this->SetCanBeStunned (false);

    // initialize body parts damage modification
    for (int i = 0; i < NUM_BODY_PARTS; i++)
    {
        this->bodyPartsDamage[i] = 1.0f;
    }

    // create a new rings info array
    this->ringsInfo = n_new(nArray<RingInfo>(4, 4));
}

//------------------------------------------------------------------------------
/**
*/
ncGameplayLivingClass::~ncGameplayLivingClass()
{
    ncGameplayClass::~ncGameplayClass();

    // remove the rings array
    n_delete(this->ringsInfo);
}

//------------------------------------------------------------------------------
/**
    Reset rings information
*/
void
ncGameplayLivingClass::ResetRingsInfo() const
{
    this->ringsInfo->Reset();
}

//------------------------------------------------------------------------------
/**
    Set the maximum orientation change in degrees per second
*/
void 
ncGameplayLivingClass::SetMaxAngularVelocityInDegrees( float velocity )
{
    this->maxAngularVelocity = N_PI * velocity / 180.0f;
}

//------------------------------------------------------------------------------
/**
    Get the maximum orientation change in degrees per second
*/
float
ncGameplayLivingClass::GetMaxAngularVelocityInDegrees() const
{
    return 180.0f * this->maxAngularVelocity / N_PI;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------