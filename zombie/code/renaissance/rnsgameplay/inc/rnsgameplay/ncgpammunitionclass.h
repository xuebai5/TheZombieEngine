#ifndef NCGPAMMUNITIONCLASS_H
#define NCGPAMMUNITIONCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncGPAmmunitionClass
    @ingroup Entities

    Component class with the data of ammunition

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"

#include "rnsgameplay/ncgpweaponclass.h"

//------------------------------------------------------------------------------
class ncGPAmmunitionClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncGPAmmunitionClass,nComponentClass);

public:
    /// constructor
    ncGPAmmunitionClass();
    /// destructor
    ~ncGPAmmunitionClass();
    /// entity class persistence
    bool SaveCmds( nPersistServer * ps );

    /// @name Script interface
    //@{
    /// Set the ammo type of the weapon
    void SetAmmoType(int);
    /// Get the ammo type of the weapon
    int GetAmmoType ()const;
    //@}

private:

    ncGPWeaponClass::AmmoType ammoType;

};

//------------------------------------------------------------------------------
/**
    @param value new ammo type
*/
inline
void
ncGPAmmunitionClass::SetAmmoType( int value )
{
    n_assert( value > 0 && value < int( ncGPWeaponClass::AMMO_LAST ) );
    this->ammoType = ncGPWeaponClass::AmmoType( value );
}

//------------------------------------------------------------------------------
/**
    @returns the ammo type
*/
inline
int
ncGPAmmunitionClass::GetAmmoType() const
{
    return int( this->ammoType );
}

//------------------------------------------------------------------------------
#endif//NCGPAMMUNITIONCLASS_H
