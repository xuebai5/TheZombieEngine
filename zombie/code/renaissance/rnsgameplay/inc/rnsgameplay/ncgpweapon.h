#ifndef NCGPWEAPON_H
#define NCGPWEAPON_H
//------------------------------------------------------------------------------
/**
    @class ncGPWeapon
    @ingroup Entities

    Component Object that represent the gameplay of a weapon.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "rnsgameplay/ncgameplay.h"

#include "nphysics/nphysicsgeom.h"

#include "rnsgameplay/ncgpweaponclass.h"

//------------------------------------------------------------------------------
class ncGPWeapon : public ncGameplay
{

    NCOMPONENT_DECLARE(ncGPWeapon,ncGameplay);

public:
    /// constructor
    ncGPWeapon();
    /// destructor
    virtual ~ncGPWeapon();

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// @name Script interface
    //@{
    /// Get the accuracy of the weapon
    float GetAccuracy ()const;
    /// Get the recoil deviation in the horizontal of the weapon
    float GetRecoilDeviationX ()const;
    /// Get the recoil deviation in the vertical of the weapon
    float GetRecoilDeviationY ()const;
    /// Get the time of recover from weapon recoil
    float GetRecoilTime ()const;
    /// Get the range of the weapon
    float GetRange ()const;
    /// Get the damage that weapon do
    float GetDamage ()const;
    /// Get the clip size of the weapon magazine
    int GetClipSize ()const;

    /// Get the weapon ammo type
    int GetAmmoType ()const;
    /// Get the actual ammo in the weapon (primary magazine)
    void SetAmmo(int);
    /// Set the actual ammo in the weapon (primary magazine)
    int GetAmmo ()const;
    /// Get the actual ammo in the weapon (secundary magazine)
    void SetAmmoExtra(int);
    /// Set the actual ammo in the weapon (secundary magazine)
    int GetAmmoExtra ()const;
    /// apply accuracy loss for shoot
    void ApplyAccuracyLoss ();
    /// apply accuracy recover in time
    void ApplyAccuracyRecover(float);
    /// Set state ironsight
    void SetIronsight(bool);
    /// Get state ironsight
    bool GetIronsight ()const;
    /// Set state moving
    void SetMoving(bool);
    /// Get state moving
    bool GetMoving ()const;
    /// Set state crouch
    void SetCrouch(bool);
    /// Get state crouch
    bool GetCrouch ()const;
    /// Set state prone
    void SetProne(bool);
    /// Get state prone
    bool GetProne ()const;
    /// get the rate of fire
    float GetRateOfFire ()const;
    ///
    void SetFireMode(int);
    /// get the fire mode
    int GetFireMode ()const;
    /// get the down offset
    float GetDownOffset ()const;

    /// say if the weapon have all magazines full
    bool HasFullAmmo ()const;
    //@}

    nArray<nEntityObject*> GetAddonsByTrait ( int trait);

    void SetScopeState( bool hideValue );

    /// say if weapon need / or only can do the full reload
    bool NeedFullReload() const;

    /// get the default magazine
    nEntityObject * GetDefaultMagazine() const;

private:
    /// calculate acual chars of the weapon
    void CalculateWeaponChars();

    int max_slots;
    int * slots_types;
    nEntityObject ** slots_addons;
    nEntityObject * defaultMag;

    ncGPWeaponClass::FireMode fireMode;

    float actualAccuracy;
    float weaponAccuracyMax;
    float weaponAccuracyMin;
    float weaponAccuracyLoss;
    float weaponAimSpeed;
    float weaponRecoilTime;
    
    float weaponRecoilDeviationX;
    float weaponRecoilDeviationY;

    float weaponDamage;
    float weaponRange;

    int weaponClipSize;

    int ammo;
    int ammoExtra;

    //float actualAccuracyMax;

    bool isMoving;
    bool isCrouch;
    bool isIronsight;
    bool isProne;

};

//------------------------------------------------------------------------------
/**
    @returns the weapon accuracy
*/
inline
float
ncGPWeapon::GetAccuracy()const
{
    return this->actualAccuracy;
}

//------------------------------------------------------------------------------
/**
    @returns the weapon recoil deviation in X
*/
inline
float
ncGPWeapon::GetRecoilDeviationX()const
{
    return this->weaponRecoilDeviationX;
}

//------------------------------------------------------------------------------
/**
    @returns the weapon recoil deviation in Y
*/
inline
float
ncGPWeapon::GetRecoilDeviationY()const
{
    return this->weaponRecoilDeviationY;
}

//------------------------------------------------------------------------------
/**
    @returns the weapon recoil recover time
*/
inline
float
ncGPWeapon::GetRecoilTime()const
{
    return this->weaponRecoilTime;
}

//------------------------------------------------------------------------------
/**
    @returns the weapon damage
*/
inline
float
ncGPWeapon::GetDamage()const
{
    return this->weaponDamage;
}

//------------------------------------------------------------------------------
/**
    @returns the weapon range
*/
inline
float
ncGPWeapon::GetRange()const
{
    return this->weaponRange;
}

//------------------------------------------------------------------------------
/**
    @returns the weapon clip size
*/
inline
int
ncGPWeapon::GetClipSize()const
{
    return this->weaponClipSize;
}

//------------------------------------------------------------------------------
/**
    @param value new weapon primary ammo
*/
inline
void
ncGPWeapon::SetAmmo( int value )
{
    this->ammo = value;
}

//------------------------------------------------------------------------------
/**
    @returns the weapon primary ammo
*/
inline
int
ncGPWeapon::GetAmmo()const
{
    return this->ammo;
}

//------------------------------------------------------------------------------
/**
    @param value new weapon secundary ammo
*/
inline
void
ncGPWeapon::SetAmmoExtra( int value )
{
    this->ammoExtra = value;
}

//------------------------------------------------------------------------------
/**
    @returns the weapon secundary ammo
*/
inline
int
ncGPWeapon::GetAmmoExtra()const
{
    return this->ammoExtra;
}

//------------------------------------------------------------------------------
/**
    @param value new weapon ironsight flag
*/
inline
void
ncGPWeapon::SetIronsight( bool value )
{
    this->isIronsight = value;
    this->CalculateWeaponChars();
}

//------------------------------------------------------------------------------
/**
    @returns true if weapon is in ironsight
*/
inline
bool
ncGPWeapon::GetIronsight()const
{
    return this->isIronsight;
}

//------------------------------------------------------------------------------
/**
    @param value new weapon moving flag
*/
inline
void
ncGPWeapon::SetMoving( bool value )
{
    this->isMoving = value;
    this->CalculateWeaponChars();
}

//------------------------------------------------------------------------------
/**
    @returns true if weapon is moving
*/
inline
bool
ncGPWeapon::GetMoving()const
{
    return this->isMoving;
}

//------------------------------------------------------------------------------
/**
    @param value new weapon crouch flag
*/
inline
void
ncGPWeapon::SetCrouch( bool value )
{
    this->isCrouch = value;
    this->CalculateWeaponChars();
}

//------------------------------------------------------------------------------
/**
    @returns true if weapon is in crouch
*/
inline
bool
ncGPWeapon::GetCrouch()const
{
    return this->isCrouch;
}

//------------------------------------------------------------------------------
/**
    @param value new weapon prone flag
*/
inline
void
ncGPWeapon::SetProne( bool value )
{
    this->isProne = value;
    this->CalculateWeaponChars();
}

//------------------------------------------------------------------------------
/**
    @returns true if weapon is in prone
*/
inline
bool
ncGPWeapon::GetProne()const
{
    return this->isProne;
}

//------------------------------------------------------------------------------
/**
    @returns the fire mode
*/
inline
int
ncGPWeapon::GetFireMode()const
{
    return this->fireMode;
}

//------------------------------------------------------------------------------
#endif//NCGPWEAPON_H
