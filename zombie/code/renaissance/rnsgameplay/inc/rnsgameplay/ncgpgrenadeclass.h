#ifndef NCGPGRENADECLASS_H
#define NCGPGRENADECLASS_H
//------------------------------------------------------------------------------
/**
    @class ncGPGrenadeClass
    @ingroup Entities

    Component Class that represent a Grenade gameplay.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "rnsgameplay/ncgameplayclass.h"

//------------------------------------------------------------------------------
class ncGPGrenadeClass : public ncGameplayClass
{

    NCOMPONENT_DECLARE(ncGPGrenadeClass,ncGameplayClass);

public:
    /// constructor
    ncGPGrenadeClass();
    /// destructor
    ~ncGPGrenadeClass();
    /// entity class persistence
    bool SaveCmds( nPersistServer * ps );

    /// @name Script interface
    //@{
    /// set the launch power of the grenade
    void SetLaunchPower(float);
    /// get the launch power of the grenade
    float GetLaunchPower ();
    /// set the maximum life time of the rockets
    void SetMaxLifeTime(float);
    /// get the maximum life time of the rockets
    float GetMaxLifeTime ();
    //@}

private:
    float launchPower;
    float maxLifeTime;

};

//------------------------------------------------------------------------------
/**
    @param new launch Power
*/
inline
void
ncGPGrenadeClass::SetLaunchPower( float power )
{
    this->launchPower = power;
}

//------------------------------------------------------------------------------
/**
    @returns the launch Power
*/
inline
float
ncGPGrenadeClass::GetLaunchPower( )
{
    return this->launchPower;
}

//------------------------------------------------------------------------------
/**
    @param new Maximum Life Time
*/
inline
void
ncGPGrenadeClass::SetMaxLifeTime( float time )
{
    this->maxLifeTime = time;
}

//------------------------------------------------------------------------------
/**
    @returns the Maximum Life Time
*/
inline
float
ncGPGrenadeClass::GetMaxLifeTime( )
{
    return this->maxLifeTime;
}

//------------------------------------------------------------------------------
#endif//NCGPGRENADECLASS_H
