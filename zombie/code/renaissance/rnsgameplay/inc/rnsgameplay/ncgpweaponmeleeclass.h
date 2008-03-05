#ifndef NCGPWEAPONMELEECLASS_H
#define NCGPWEAPONMELEECLASS_H
//------------------------------------------------------------------------------
/**
    @class ncGPWeaponMeleeClass
    @ingroup Entities

    Component class with the data of melee weapons

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "rnsgameplay/ndamagemanager.h"

//------------------------------------------------------------------------------
class ncGPWeaponMeleeClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncGPWeaponMeleeClass,nComponentClass);

public:
    /// constructor
    ncGPWeaponMeleeClass();
    /// destructor
    ~ncGPWeaponMeleeClass();
    /// entity class persistence
    bool SaveCmds( nPersistServer * ps );

    /// @name Script interface
    //@{
    /// set the damage melee of the weapon
    void SetDamageMelee(float);
    /// get the damage melee of the weapon
    float GetDamageMelee ()const;
    /// set the type of the melee damage 
    void SetMeleeDamageType(int);
    /// get the type of the melee damage
    int GetMeleeDamageType () const ;
    /// Get the melee range
    float GetMeleeRange () const;
    /// Set the melee range
    void SetMeleeRange(float);

    //@}

private:
    float damageMelee;
    float meleeRange;
    nDamageManager::DamageType meleeDamageType;

};

//------------------------------------------------------------------------------
/**
@param value new range
*/
inline
void
ncGPWeaponMeleeClass::SetMeleeRange( float value )
{
    this->meleeRange = value;
}

//------------------------------------------------------------------------------
/**
@returns melee range
*/
inline
float
ncGPWeaponMeleeClass::GetMeleeRange() const
{
    return this->meleeRange;
}

//------------------------------------------------------------------------------
/**
    @param value new damage
*/
inline
void
ncGPWeaponMeleeClass::SetDamageMelee( float value )
{
    this->damageMelee = value;
}

//------------------------------------------------------------------------------
/**
    @returns melee damage
*/
inline
float
ncGPWeaponMeleeClass::GetDamageMelee()const
{
    return this->damageMelee;
}

//------------------------------------------------------------------------------
/**
    @param value new Melee Damage Type
*/
inline
void
ncGPWeaponMeleeClass::SetMeleeDamageType( int value )
{
    n_assert( value > 0 && value < int( nDamageManager::DAMAGE_LAST ) );
    this->meleeDamageType = nDamageManager::DamageType( value );
}

//------------------------------------------------------------------------------
/**
    @returns melee damage type
*/
inline
int
ncGPWeaponMeleeClass::GetMeleeDamageType() const
{
    return int( this->meleeDamageType );
}

//------------------------------------------------------------------------------
#endif//NCGPWEAPONMELEECLASS_H
