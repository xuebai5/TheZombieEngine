#ifndef NCGPROCKETCLASS_H
#define NCGPROCKETCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncGPRocketClass
    @ingroup Entities

    Component Class that represent a Rocket Propelled Grenade gameplay.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "rnsgameplay/ncgameplayclass.h"
 
//------------------------------------------------------------------------------
class ncGPRocketClass : public ncGameplayClass
{

    NCOMPONENT_DECLARE(ncGPRocketClass,ncGameplayClass);

public:
    /// constructor
    ncGPRocketClass();
    /// destructor
    ~ncGPRocketClass();
    /// entity class persistence
    bool SaveCmds( nPersistServer * ps );

    /// @name Script interface
    //@{
    /// set the initial power of the rockets
    void SetInitialThrustPower(float);
    /// get the initial power of the rockets
    float GetInitialThrustPower ();
    /// set the maximum thrust time of the rockets
    void SetMaxThrustTime(float);
    /// get the maximum thrust time of the rockets
    float GetMaxThrustTime ();
    /// set the maximum life time of the rockets
    void SetMaxLifeTime(float);
    /// get the maximum life time of the rockets
    float GetMaxLifeTime ();
    /// set the maximum meters of the rockets
    void SetMaxMeters(float);
    /// get the maximum meters of the rockets
    float GetMaxMeters ();
    //@}

private:
    float initialThrustPower;
    float maxThrustTime;
    float maxLifeTime;
    float maxMeters;

};

//------------------------------------------------------------------------------
/**
    @param power Initial Thrust Power
*/
inline
void
ncGPRocketClass::SetInitialThrustPower( float power )
{
    this->initialThrustPower = power;
}

//------------------------------------------------------------------------------
/**
    @returns the Initial Thrust Power
*/
inline
float
ncGPRocketClass::GetInitialThrustPower( )
{
    return this->initialThrustPower;
}

//------------------------------------------------------------------------------
/**
    @param time Maximum Thrust Time
*/
inline
void
ncGPRocketClass::SetMaxThrustTime( float time )
{
    this->maxThrustTime = time;
}

//------------------------------------------------------------------------------
/**
    @returns the Maximum Thrust Time
*/
inline
float
ncGPRocketClass::GetMaxThrustTime( )
{
    return this->maxThrustTime;
}

//------------------------------------------------------------------------------
/**
    @param time Maximum Life Time
*/
inline
void
ncGPRocketClass::SetMaxLifeTime( float time )
{
    this->maxLifeTime = time;
}

//------------------------------------------------------------------------------
/**
    @returns the Maximum Life Time
*/
inline
float
ncGPRocketClass::GetMaxLifeTime( )
{
    return this->maxLifeTime;
}

//------------------------------------------------------------------------------
/**
    @param meters Maximum Meters
*/
inline
void
ncGPRocketClass::SetMaxMeters( float meters )
{
    this->maxMeters = meters;
}

//------------------------------------------------------------------------------
/**
    @returns the Maximum Meters
*/
inline
float
ncGPRocketClass::GetMaxMeters( )
{
    return this->maxMeters;
}

//------------------------------------------------------------------------------
#endif//NCGPROCKETCLASS_H
