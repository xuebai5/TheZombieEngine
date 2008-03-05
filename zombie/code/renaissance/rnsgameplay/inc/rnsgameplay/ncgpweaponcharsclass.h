#ifndef NCGPWEAPONCHARSCLASS_H
#define NCGPWEAPONCHARSCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncGPWeaponCharsClass
    @ingroup Entities

    Component Class that represent the characteristics of weapons.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "rnsgameplay/ncgameplayclass.h"

//------------------------------------------------------------------------------
class ncGPWeaponCharsClass : public ncGameplayClass
{

    NCOMPONENT_DECLARE(ncGPWeaponCharsClass,ncGameplayClass);

public:
    enum SlotType{
        SLOT_TOP                    = ( 1 << 0 ),
        SLOT_OVERBARREL             = ( 1 << 1 ),
        SLOT_UNDERBARREL            = ( 1 << 2 ),
        SLOT_UNDERBARREL_NOG        = ( 1 << 3 ),
        SLOT_MOUNT                  = ( 1 << 4 ),
        SLOT_SIDERAIL               = ( 1 << 5 ),
        SLOT_SIDERAIL_NOG           = ( 1 << 6 ),
        SLOT_MUZZLE_SMALL           = ( 1 << 7 ),
        SLOT_MUZZLE_SMALL_NOG       = ( 1 << 8 ),
        SLOT_MUZZLE_BIG             = ( 1 << 9 ),
        SLOT_MUZZLE_BIG_NOG         = ( 1 << 10 ),
        SLOT_MAG_HICAP              = ( 1 << 11 ),
        SLOT_MAG_CLAMP              = ( 1 << 12 ),
        SLOT_AMMO                   = ( 1 << 13 ),

        SLOT_LAST
    };

    enum ModType{
        MOD_ENABLE_IRONSIGHT	    = ( 1 << 0 ),
        MOD_ENABLE_DOTSIGHT	        = ( 1 << 1 ),
        MOD_ENABLE_SCOPE	        = ( 1 << 2 ),
        MOD_ENABLE_LIGHT	        = ( 1 << 3 ),
        MOD_ENABLE_GRENADE	        = ( 1 << 4 ),
        MOD_ENABLE_QUICK_RELOAD	    = ( 1 << 5 ),
        MOD_ENABLE_TRACKING	        = ( 1 << 6 ),
		MOD_ENABLE_SILENCER			= ( 1 << 7 ),

        MOD_LAST
    };

    static const float DEVIATION_DISTANCE;
    static const float MAX_DEVIATION;

    /// constructor
    ncGPWeaponCharsClass();
    /// destructor
    ~ncGPWeaponCharsClass();
    /// entity class persistence
    bool SaveCmds( nPersistServer * ps );

    static const char * GetSlotName( int type );

    /// @name Script interface
    //@{
    /// Set minimum weapon accuracy
    void SetAccuracyMin(float);
    /// Get minimum weapon accuracy
    float GetAccuracyMin ();

    /// Set maximum weapon accuracy
    void SetAccuracyMax(float);
    /// Get maximum weapon accuracy
    float GetAccuracyMax ();

    /// Set accuracy loss of the weapon
    void SetAccuracyLoss(float);
    /// Get accuracy loss of the weapon
    float GetAccuracyLoss ();

    /// Set accuracy when prone of the weapon
    void SetAccuracyProne(float);
    /// Get accuracy when prone of the weapon
    float GetAccuracyProne ();

    /// Set accuracy when crouch of the weapon
    void SetAccuracyCrouch(float);
    /// Get accuracy when crouch of the weapon
    float GetAccuracyCrouch ();

    /// Set accuracy when move of the weapon
    void SetAccuracyMove(float);
    /// Get accuracy when move of the weapon
    float GetAccuracyMove ();

    /// Set accuracy when run of the weapon
    void SetAccuracyRun(float);
    /// Get accuracy when run of the weapon
    float GetAccuracyRun ();

    /// Set accuracy when ironsight of the weapon
    void SetAccuracyIronsight(float);
    /// Get accuracy when ironsight of the weapon
    float GetAccuracyIronsight ();

    /// Set accuracy angle of the weapon
    void SetAccuracyAngle(float);
    /// Get accuracy angle of the weapon
    float GetAccuracyAngle ();

    /// Set recoil deviation in horizontal of the weapon
    void SetRecoilDeviationX(float);
    /// Get recoil deviation in horizontal of the weapon
    float GetRecoilDeviationX ();

    /// Set recoil deviation in vertical of the weapon
    void SetRecoilDeviationY(float);
    /// Get recoil deviation in vertical of the weapon
    float GetRecoilDeviationY ();

    /// Set recoil recover time of the weapon
    void SetRecoilTime(float);
    /// Get recoil recover time of the weapon
    float GetRecoilTime ();

    /// Set recoil deviation in horizontal when prone of the weapon
    void SetRecoilDeviationProneX(float);
    /// Get recoil deviation in horizontal when prone of the weapon
    float GetRecoilDeviationProneX ();

    /// Set recoil deviation in vertical when prone of the weapon
    void SetRecoilDeviationProneY(float);
    /// Get recoil deviation in vertical when prone of the weapon
    float GetRecoilDeviationProneY ();

    /// Set aim speed of the weapon
    void SetAimSpeed(float);
    /// Get aim speed of the weapon
    float GetAimSpeed ();

    /// Set aim speed when prone of the weapon
    void SetAimSpeedProne(float);
    /// Get aim speed when prone of the weapon
    float GetAimSpeedProne ();

    /// Set range of the weapon
    void SetRange(float);
    /// Get range of the weapon
    float GetRange ();

    /// Set minimum range of the weapon
    void SetMinRange(float);
    /// Get minimum range of the weapon
    float GetMinRange ();

    /// Set damage of the weapon
    void SetDamage(float);
    /// Get damage of the weapon
    float GetDamage ();

    /// Set damage at half range of the weapon
    void SetDamageHalfRange(float);
    /// Get damage at half range of the weapon
    float GetDamageHalfRange ();

    /// Set shoot volume of the weapon
    void SetShootVolume(float);
    /// Get shoot volume of the weapon
    float GetShootVolume ();

    /// Set clip size of the weapon
    void SetClipSize(int);
    /// Get clip size of the weapon
    int GetClipSize ();

    /// set the traits of the weapon
    void SetTraits(int);
    /// get the traits of the weapon
    int GetTraits () const;
    /// Add a behaviour to the weapon
    void AddTrait(int);
    /// remove a behavior of the weapon
    void RemoveTrait(int);
    /// Say if a trait is allowed by the weapon characteristics
    bool AllowTrait(int) const;

    /// set the slots of the weapon
    void SetSlots(int);
    /// get the slots of the weapon
    int GetSlots ()const;
    /// add a type to the slot
    void AddSlot(int);
    /// say if addon allow a type of slot
    bool AllowType(int) const;
    /// remove a slot of the weapon
    void RemoveSlot(int);
    //@}

private:
    struct SlotInfo
    {
        int type;
        const char * name;
    };

    static const SlotInfo slot_names[];

    float accuracyMin;
    float accuracyMax;
    float accuracyLoss;
    float accuracyProne;
    float accuracyCrouch;
    float accuracyMove;
    float accuracyRun;
    float accuracyIronsight;
    float accuracyAngle;
    float recoilDeviationX;
    float recoilDeviationY;
    float recoilTime;
    float recoilDeviationProneX;
    float recoilDeviationProneY;
    float aimSpeed;
    float aimSpeedProne;
    float range;
    float minRange;
    float damage;
    float damageHalfRange;
    float shootVolume;
    int clipSize;

    int traits;
    int slots;

};

//------------------------------------------------------------------------------
/**
    @param value new minimum accuracy
*/
inline
void
ncGPWeaponCharsClass::SetAccuracyMin( float value )
{
    this->accuracyMin = value;
}

//------------------------------------------------------------------------------
/**
    @returns minimum accuracy
*/
inline
float
ncGPWeaponCharsClass::GetAccuracyMin()
{
    return this->accuracyMin;
}

//------------------------------------------------------------------------------
/**
    @param value new maximum accuracy
*/
inline
void
ncGPWeaponCharsClass::SetAccuracyMax( float value )
{
    this->accuracyMax = value;
}

//------------------------------------------------------------------------------
/**
    @returns maximum accuracy
*/
inline
float
ncGPWeaponCharsClass::GetAccuracyMax()
{
    return this->accuracyMax;
}

//------------------------------------------------------------------------------
/**
    @param value new accuracy loss
*/
inline
void
ncGPWeaponCharsClass::SetAccuracyLoss( float value )
{
    this->accuracyLoss = value;
}

//------------------------------------------------------------------------------
/**
    @returns accuracy loss
*/
inline
float
ncGPWeaponCharsClass::GetAccuracyLoss()
{
    return this->accuracyLoss;
}

//------------------------------------------------------------------------------
/**
    @param value new accuracy prone
*/
inline
void
ncGPWeaponCharsClass::SetAccuracyProne( float value )
{
    this->accuracyProne = value;
}

//------------------------------------------------------------------------------
/**
    @returns accuracy prone
*/
inline
float
ncGPWeaponCharsClass::GetAccuracyProne()
{
    return this->accuracyProne;
}

//------------------------------------------------------------------------------
/**
    @param value new accuracy when crouch
*/
inline
void
ncGPWeaponCharsClass::SetAccuracyCrouch( float value )
{
    this->accuracyCrouch = value;
}

//------------------------------------------------------------------------------
/**
    @returns accuracy crouch
*/
inline
float
ncGPWeaponCharsClass::GetAccuracyCrouch()
{
    return this->accuracyCrouch;
}

//------------------------------------------------------------------------------
/**
    @param value new accuracy when move
*/
inline
void
ncGPWeaponCharsClass::SetAccuracyMove( float value )
{
    this->accuracyMove = value;
}

//------------------------------------------------------------------------------
/**
    @returns accuracy when move
*/
inline
float
ncGPWeaponCharsClass::GetAccuracyMove()
{
    return this->accuracyMove;
}

//------------------------------------------------------------------------------
/**
    @param value new accuracy when run
*/
inline
void
ncGPWeaponCharsClass::SetAccuracyRun( float value )
{
    this->accuracyRun = value;
}

//------------------------------------------------------------------------------
/**
    @returns accuracy when run
*/
inline
float
ncGPWeaponCharsClass::GetAccuracyRun()
{
    return this->accuracyRun;
}

//------------------------------------------------------------------------------
/**
    @param value new accuracy when ironsight
*/
inline
void
ncGPWeaponCharsClass::SetAccuracyIronsight( float value )
{
    this->accuracyIronsight = value;
}

//------------------------------------------------------------------------------
/**
    @returns accuracy when ironsight
*/
inline
float
ncGPWeaponCharsClass::GetAccuracyIronsight()
{
    return this->accuracyIronsight;
}

//------------------------------------------------------------------------------
/**
    @param value new accuracy angle
*/
inline
void
ncGPWeaponCharsClass::SetAccuracyAngle( float value )
{
    this->accuracyAngle = value;
}

//------------------------------------------------------------------------------
/**
    @returns accuracy angle
*/
inline
float
ncGPWeaponCharsClass::GetAccuracyAngle()
{
    return this->accuracyAngle;
}

//------------------------------------------------------------------------------
/**
    @param value new recoil deviation in horizontal
*/
inline
void
ncGPWeaponCharsClass::SetRecoilDeviationX( float value )
{
    this->recoilDeviationX = value;
}

//------------------------------------------------------------------------------
/**
    @returns recoil deviation in horizontal
*/
inline
float
ncGPWeaponCharsClass::GetRecoilDeviationX()
{
    return this->recoilDeviationX;
}

//------------------------------------------------------------------------------
/**
    @param value new recoil deviation in vertical
*/
inline
void
ncGPWeaponCharsClass::SetRecoilDeviationY( float value )
{
    this->recoilDeviationY = value;
}

//------------------------------------------------------------------------------
/**
    @returns recoil deviation in vertical
*/
inline
float
ncGPWeaponCharsClass::GetRecoilDeviationY()
{
    return this->recoilDeviationY;
}

//------------------------------------------------------------------------------
/**
    @param value new recoil recover time
*/
inline
void
ncGPWeaponCharsClass::SetRecoilTime( float value )
{
    this->recoilTime = value;
}

//------------------------------------------------------------------------------
/**
    @returns recoil recover time
*/
inline
float
ncGPWeaponCharsClass::GetRecoilTime()
{
    return this->recoilTime;
}

//------------------------------------------------------------------------------
/**
    @param value new recoil deviation in horizontal when prone
*/
inline
void
ncGPWeaponCharsClass::SetRecoilDeviationProneX( float value )
{
    this->recoilDeviationProneX = value;
}

//------------------------------------------------------------------------------
/**
    @returns recoil deviation in horizontal when prone
*/
inline
float
ncGPWeaponCharsClass::GetRecoilDeviationProneX()
{
    return this->recoilDeviationProneX;
}

//------------------------------------------------------------------------------
/**
    @param value new recoil deviation in vertical when prone
*/
inline
void
ncGPWeaponCharsClass::SetRecoilDeviationProneY( float value )
{
    this->recoilDeviationProneY = value;
}

//------------------------------------------------------------------------------
/**
    @returns recoil deviation in vertical when prone
*/
inline
float
ncGPWeaponCharsClass::GetRecoilDeviationProneY()
{
    return this->recoilDeviationProneY;
}

//------------------------------------------------------------------------------
/**
    @param value new aim speed
*/
inline
void
ncGPWeaponCharsClass::SetAimSpeed( float value )
{
    this->aimSpeed = value;
}

//------------------------------------------------------------------------------
/**
    @returns aim speed
*/
inline
float
ncGPWeaponCharsClass::GetAimSpeed()
{
    return this->aimSpeed;
}

//------------------------------------------------------------------------------
/**
    @param value new aim speed when prone
*/
inline
void
ncGPWeaponCharsClass::SetAimSpeedProne( float value )
{
    this->aimSpeedProne = value;
}

//------------------------------------------------------------------------------
/**
    @returns aim speed when prone
*/
inline
float
ncGPWeaponCharsClass::GetAimSpeedProne()
{
    return this->aimSpeedProne;
}

//------------------------------------------------------------------------------
/**
    @param value new range
*/
inline
void
ncGPWeaponCharsClass::SetRange( float value )
{
    this->range = value;
}

//------------------------------------------------------------------------------
/**
    @returns new range
*/
inline
float
ncGPWeaponCharsClass::GetRange()
{
    return this->range;
}

//------------------------------------------------------------------------------
/**
    @param value new minimum range
*/
inline
void
ncGPWeaponCharsClass::SetMinRange( float value )
{
    this->minRange = value;
}

//------------------------------------------------------------------------------
/**
    @returns minimum range
*/
inline
float
ncGPWeaponCharsClass::GetMinRange()
{
    return this->minRange;
}

//------------------------------------------------------------------------------
/**
    @param value new damage
*/
inline
void
ncGPWeaponCharsClass::SetDamage( float value )
{
    this->damage = value;
}

//------------------------------------------------------------------------------
/**
    @returns damage
*/
inline
float
ncGPWeaponCharsClass::GetDamage()
{
    return this->damage;
}

//------------------------------------------------------------------------------
/**
    @param value new damage at half range
*/
inline
void
ncGPWeaponCharsClass::SetDamageHalfRange( float value )
{
    this->damageHalfRange = value;
}

//------------------------------------------------------------------------------
/**
    @returns damage at half range
*/
inline
float
ncGPWeaponCharsClass::GetDamageHalfRange()
{
    return this->damageHalfRange;
}

//------------------------------------------------------------------------------
/**
    @param value new shoot volume
*/
inline
void
ncGPWeaponCharsClass::SetShootVolume( float value )
{
    this->shootVolume = value;
}

//------------------------------------------------------------------------------
/**
    @returns shoot volume
*/
inline
float
ncGPWeaponCharsClass::GetShootVolume()
{
    return this->shootVolume;
}

//------------------------------------------------------------------------------
/**
    @param value new clip size
*/
inline
void
ncGPWeaponCharsClass::SetClipSize( int value )
{
    this->clipSize = value;
}

//------------------------------------------------------------------------------
/**
    @returns clip size
*/
inline
int
ncGPWeaponCharsClass::GetClipSize()
{
    return this->clipSize;
}

//------------------------------------------------------------------------------
/**
    @param value new slots
*/
inline
void
ncGPWeaponCharsClass::SetSlots( int value )
{
    n_assert( value >= 0 && value < int( SLOT_LAST ) );
    this->slots = value;
}

//------------------------------------------------------------------------------
/**
    @returns the slots of the weapon
*/
inline
int
ncGPWeaponCharsClass::GetSlots( ) const
{
    return this->slots;
}

//------------------------------------------------------------------------------
/**
    @param type new slots type
*/
inline
void
ncGPWeaponCharsClass::AddSlot( int type )
{
    n_assert( type > 0 && type < int( SLOT_LAST ) );
    this->slots |= type;
}

//------------------------------------------------------------------------------
/**
    @param type new slots
    @retval true if type is allowed in the addon
*/
inline
bool
ncGPWeaponCharsClass::AllowType( int type ) const
{
    n_assert( type > 0 && type < int( SLOT_LAST ) );
    return (( this->slots & type ) != 0);
}

//------------------------------------------------------------------------------
/**
    @param type the slot to remove
*/
inline
void
ncGPWeaponCharsClass::RemoveSlot( int type )
{
    n_assert( type > 0 && type < int( SLOT_LAST ) );
    this->slots &= ~ type;
}

//------------------------------------------------------------------------------
/**
    @param value new traits
*/
inline
void
ncGPWeaponCharsClass::SetTraits( int value )
{
    n_assert( value >= 0 && value < int( MOD_LAST ) );
    this->traits = value;
}

//------------------------------------------------------------------------------
/**
    @returns the traits of the weapon
*/
inline
int
ncGPWeaponCharsClass::GetTraits() const
{
    return this->traits;
}

//------------------------------------------------------------------------------
/**
    @param type the trait to add
*/
inline
void
ncGPWeaponCharsClass::AddTrait( int type )
{
    n_assert( type > 0 && type < int( MOD_LAST ) );
    this->traits |= type;
}

//------------------------------------------------------------------------------
/**
    @param type type of trait
    @retval true if trait is allowed
*/
inline
bool 
ncGPWeaponCharsClass::AllowTrait( int type ) const
{
    n_assert( type > 0 && type < int( MOD_LAST ) );
    return (( this->traits & type ) != 0);
}

//------------------------------------------------------------------------------
/**
    @param type the trait to remove
*/
inline
void
ncGPWeaponCharsClass::RemoveTrait( int type )
{
    n_assert( type > 0 && type < int( MOD_LAST ) );
    this->traits &= ~ type;
}

//------------------------------------------------------------------------------
#endif//NCGPWEAPONCHARSCLASS_H
