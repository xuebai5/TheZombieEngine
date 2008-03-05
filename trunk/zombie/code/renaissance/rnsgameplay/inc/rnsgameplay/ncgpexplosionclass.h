#ifndef NCGPEXPLOSIONCLASS_H
#define NCGPEXPLOSIONCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncGPExplosionClass
    @ingroup Entities
    
    Component Class that represent an explosion.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

//------------------------------------------------------------------------------
class ncGPExplosionClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncGPExplosionClass,nComponentClass);

public:
    /// constructor
    ncGPExplosionClass();
    /// destructor
    ~ncGPExplosionClass();
    /// entity class persistence
    bool SaveCmds( nPersistServer * ps );

    /// @name Script interface
    //@{
    /// set the radius of the explosion
    void SetExplosionRadius(float);
    /// get the radius of the explosion
    float GetExplosionRadius () const;
    /// set the power of the explosion
    void SetExplosionPower(float);
    /// get the power of the explosion
    float GetExplosionPower () const;
    /// set the damage caused
    void SetExplosionDamage(float);
    /// get the damage caused
    float GetExplosionDamage() const;
    //@}

private:
    float explosionRadius;
    float explosionPower;
    float explosionDamage;

};

//------------------------------------------------------------------------------
/**
    @param new Explosion Radius
*/
inline
void
ncGPExplosionClass::SetExplosionRadius( float radius )
{
    this->explosionRadius = radius;
}

//------------------------------------------------------------------------------
/**
    @returns the Explosion Radius
*/
inline
float
ncGPExplosionClass::GetExplosionRadius( )const
{
    return this->explosionRadius;
}

//------------------------------------------------------------------------------
/**
    @param new Explosion Power
*/
inline
void
ncGPExplosionClass::SetExplosionPower( float power )
{
    this->explosionPower = power;
}

//------------------------------------------------------------------------------
/**
    @returns the Explosion Power
*/
inline
float
ncGPExplosionClass::GetExplosionPower( )const
{
    return this->explosionPower;
}

//------------------------------------------------------------------------------
/**
    @param new Explosion Power
*/
inline
void
ncGPExplosionClass::SetExplosionDamage( float damage )
{
    this->explosionDamage = damage;
}

//------------------------------------------------------------------------------
/**
    @returns the Explosion Power
*/
inline
float
ncGPExplosionClass::GetExplosionDamage( )const
{
    return this->explosionDamage;
}

//------------------------------------------------------------------------------
#endif//NCGPEXPLOSIONCLASS_H
