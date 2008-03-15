#ifndef NCGPWEAPONCLASS_H
#define NCGPWEAPONCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncGPWeaponClass
    @ingroup Entities

    Component Class that represent the gameplay of a weapon.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "rnsgameplay/ncgpweaponcharsclass.h"
#include "rnsgameplay/ndamagemanager.h"

//------------------------------------------------------------------------------
class ncGPWeaponClass : public ncGPWeaponCharsClass
{

    NCOMPONENT_DECLARE(ncGPWeaponClass,ncGPWeaponCharsClass);

public:
    enum AmmoType{
        AMMO_MELEE              = ( 1 << 0 ),
        AMMO_10MM               = ( 1 << 1 ),
        AMMO_SHELL              = ( 1 << 2 ),
        AMMO_SHELL_FLECHETTE    = ( 1 << 3 ),
        AMMO_5M                 = ( 1 << 4 ),
        AMMO_5M_FLECHETTE       = ( 1 << 5 ),
        AMMO_7M                 = ( 1 << 6 ),
        AMMO_12M                = ( 1 << 7 ),
        AMMO_ROCKET             = ( 1 << 8 ),
        AMMO_ROCKET_FLECHETTE   = ( 1 << 9 ),
        AMMO_GRENADE            = ( 1 << 10 ),
        AMMO_GRENADE_FLECHETTE  = ( 1 << 11 ),
        AMMO_ENERGY             = ( 1 << 12 ),
        AMMO_10MM_FLECHETTE     = ( 1 << 13 ),

        AMMO_LAST
    };

    enum TypeShape{
        SHAPE_INSTANT       = ( 1 << 0 ),
        SHAPE_PROJECTILE    = ( 1 << 1 ),
        SHAPE_CONE          = ( 1 << 2 ),
        SHAPE_SPHERE        = ( 1 << 3 ),
        SHAPE_CONTACT       = ( 1 << 4 ),

        SHAPE_LAST
    };

    enum WeaponType{
        WT_MELEE          = ( 1 << 0 ),
        WT_HANDGUN        = ( 1 << 1 ),
        WT_SHOTGUN        = ( 1 << 2 ),
        WT_SUBMACHINE     = ( 1 << 3 ),
        WT_ASSAULT        = ( 1 << 4 ),
        WT_LIGHTMACHINE   = ( 1 << 5 ),
        WT_SNIPER         = ( 1 << 6 ),
        WT_PORTABLE_HEAVY = ( 1 << 7 ),
        WT_MOUNTED_HEAVY  = ( 1 << 8 ),
        
        WT_LAST
    };

    enum FireMode{
        FM_AUTO     = ( 1 << 0 ),
        FM_SEMI     = ( 1 << 1 ),

        FM_LAST     = ( 1 << 2 ),
    };

    /// constructor
    ncGPWeaponClass();
    /// destructor
    ~ncGPWeaponClass();
    /// entity class persistence
    bool SaveCmds( nPersistServer * ps );

    /// @name Script interface
    //@{
    /// Set the rate of fire of the weapon
    void SetRateOfFire(float);
    /// Get the rate of fire of the weapon
    float GetRateOfFire ()const;
    /// Set the rate of tap fire of the weapon
    void SetRateOfTapFire(float);
    /// Get the rate of tap fire of the weapon
    float GetRateOfTapFire ()const;
    /// Set the cock time of the weapon
    void SetCockTime(float);
    /// Get the cock time of the weapon
    float GetCockTime ()const;
    /// Set the damage type of the weapon
    void SetDamageType(int);
    /// Get the damage type of the weapon
    int GetDamageType ()const;
    /// Set the ammo type of the weapon
    void SetAmmoType(int);
    /// Get the ammo type of the weapon
    int GetAmmoType ()const;
    /// Set the type shape of the weapon
    void SetTypeShape(int);
    /// Get the type shape of the weapon
    int GetTypeShape ()const;
    /// Set the ammo chamber space of the weapon
    void SetAmmoChamber(int);
    /// Get the ammo chamber space of the weapon
    int GetAmmoChamber ()const;
    /// Set the list of ammo types this weapon requires to work
    void SetAmmoReq(int);
    /// Get the list of ammo types this weapon requires to work
    int GetAmmoReq ()const;
    /// Set the type of the weapon
    void SetWeaponType(int);
    /// Get the type of the weapon
    int GetWeaponType ()const;
    /// Set the offset of weapon with addons
    void SetAddonOffset(const vector3&);
    /// Get the offset of weapon with addons
    const vector3& GetAddonOffset () const;
    /// Set the bone of the weapon offset
    void SetAddonOffsetBone(int);
    /// Get the bone of the weapon offset
    int GetAddonOffsetBone () const;
    /// Set the fire modes of the weapon
    void SetFireModes(int);
    /// Get the fire modes of the weapon
    int GetFireModes () const;
    /// say if weapon has auto fire mode
    bool HasSemiFireMode ()const;
    /// say if weapon has semi fire mode
    bool HasAutoFireMode ()const;
    //@}

private:

    nString defaultMagazine;
    vector3 addonOffset;
    float rateOfFire;
    float rateOfTapFire;
    float cockTime;
    nDamageManager::DamageType damageType;
    AmmoType ammoType;
    TypeShape typeShape;
    int ammoChamber;
    int ammoReq;
    int addonOffsetBone;
    int fireModes;
    WeaponType weaponType;

};

//------------------------------------------------------------------------------
/**
    @param value new rate of fire
*/
inline
void
ncGPWeaponClass::SetRateOfFire( float value )
{
    this->rateOfFire = value;
}

//------------------------------------------------------------------------------
/**
    @returns the rate of fire
*/
inline
float
ncGPWeaponClass::GetRateOfFire() const
{
    return this->rateOfFire;
}

//------------------------------------------------------------------------------
/**
    @param value new rate of tap fire
*/
inline
void
ncGPWeaponClass::SetRateOfTapFire( float value )
{
    this->rateOfTapFire = value;
}

//------------------------------------------------------------------------------
/**
    @returns the rate of tap fire
*/
inline
float
ncGPWeaponClass::GetRateOfTapFire() const
{
    return this->rateOfTapFire;
}

//------------------------------------------------------------------------------
/**
    @param value new cock time
*/
inline
void
ncGPWeaponClass::SetCockTime( float value )
{
    this->cockTime = value;
}

//------------------------------------------------------------------------------
/**
    @returns the cock time
*/
inline
float
ncGPWeaponClass::GetCockTime() const
{
    return this->cockTime;
}

//------------------------------------------------------------------------------
/**
    @param value new damage type
*/
inline
void
ncGPWeaponClass::SetDamageType( int value )
{
    n_assert( value > 0 && value < int( nDamageManager::DAMAGE_LAST ) );
    this->damageType = nDamageManager::DamageType( value );
}

//------------------------------------------------------------------------------
/**
    @returns the damage type
*/
inline
int
ncGPWeaponClass::GetDamageType() const
{
    return int( this->damageType );
}

//------------------------------------------------------------------------------
/**
    @param value new ammo type
*/
inline
void
ncGPWeaponClass::SetAmmoType( int value )
{
    n_assert( value > 0 && value < int( AMMO_LAST ) );
    this->ammoType = AmmoType( value );
}

//------------------------------------------------------------------------------
/**
    @returns the ammo type
*/
inline
int
ncGPWeaponClass::GetAmmoType() const
{
    return int( this->ammoType );
}

//------------------------------------------------------------------------------
/**
    @param value new type shape
*/
inline
void
ncGPWeaponClass::SetTypeShape( int value )
{
    n_assert( value > 0 && value < int( SHAPE_LAST ) );
    this->typeShape = TypeShape( value );
}

//------------------------------------------------------------------------------
/**
    @returns the type shape
*/
inline
int
ncGPWeaponClass::GetTypeShape() const
{
    return int( this->typeShape );
}

//------------------------------------------------------------------------------
/**
    @param value new amo chamber space
*/
inline
void
ncGPWeaponClass::SetAmmoChamber( int value )
{
    this->ammoChamber = value;
}

//------------------------------------------------------------------------------
/**
    @returns the ammo chamber
*/
inline
int
ncGPWeaponClass::GetAmmoChamber() const
{
    return this->ammoChamber;
}

//------------------------------------------------------------------------------
/**
    @param value new ammo requeriments
*/
inline
void
ncGPWeaponClass::SetAmmoReq( int value )
{
    n_assert( value >= 0 && value < int( AMMO_LAST ) );
    this->ammoReq = value;
}

//------------------------------------------------------------------------------
/**
    @returns the ammo requeriments
*/
inline
int
ncGPWeaponClass::GetAmmoReq() const
{
    return int( this->ammoReq );
}

//------------------------------------------------------------------------------
/**
    @param value new weapon type
*/
inline
void
ncGPWeaponClass::SetWeaponType( int value )
{
    n_assert( value >= 0 && value < int( WT_LAST ) );
    this->weaponType = ncGPWeaponClass::WeaponType( value );
}

//------------------------------------------------------------------------------
/**
    @returns the weapon type
*/
inline
int
ncGPWeaponClass::GetWeaponType() const
{
    return int( this->weaponType );
}

//------------------------------------------------------------------------------
/**
    @param value new addon offset
*/
inline
void
ncGPWeaponClass::SetAddonOffset( const vector3& value )
{
    this->addonOffset = value;
}

//------------------------------------------------------------------------------
/**
    @returns the addon offset
*/
inline
const vector3&
ncGPWeaponClass::GetAddonOffset() const
{
    return this->addonOffset;
}

//------------------------------------------------------------------------------
/**
    @param value new addon offset bone
*/
inline
void
ncGPWeaponClass::SetAddonOffsetBone( int value )
{
    this->addonOffsetBone = value;
}

//------------------------------------------------------------------------------
/**
    @returns the fire modes
*/
inline
int
ncGPWeaponClass::GetFireModes() const
{
    return this->fireModes;
}

//------------------------------------------------------------------------------
/**
    @returns the addon offset bone
*/
inline
int
ncGPWeaponClass::GetAddonOffsetBone() const
{
    return this->addonOffsetBone;
}

//------------------------------------------------------------------------------
/**
    @returns true if has auto semi mode
*/
inline
bool
ncGPWeaponClass::HasSemiFireMode() const
{
    return ( ( this->fireModes & ncGPWeaponClass::FM_SEMI ) != 0 );
}

//------------------------------------------------------------------------------
/**
    @returns true if has auto fire mode
*/
inline
bool
ncGPWeaponClass::HasAutoFireMode() const
{
    return ( ( this->fireModes & ncGPWeaponClass::FM_AUTO ) != 0 );
}

//------------------------------------------------------------------------------
#endif//NCGPWEAPONCLASS_H
