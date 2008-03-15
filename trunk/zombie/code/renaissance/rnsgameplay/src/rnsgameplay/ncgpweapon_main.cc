//------------------------------------------------------------------------------
//  ncgpweapon_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpweapon.h"
#include "entity/nentityobjectserver.h"

#include "zombieentity/ncloaderclass.h"
#include "rnsgameplay/ncgpweaponcharsclass.h"

#include "nscene/ncscene.h"

#include "nspatial/ncspatial.h"

#include "zombieentity/ncdictionary.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGPWeapon,ncGameplay);

//------------------------------------------------------------------------------
/**
*/
ncGPWeapon::ncGPWeapon():
    defaultMag( 0 ),

    actualAccuracy( 0.0f ),

    weaponAccuracyMax( 0.0f ),
    weaponAccuracyMin( 0.0f ),
    weaponAccuracyLoss( 0.0f ),
    weaponDamage( 0.0f ),
    weaponRange( 0.0f ),
    weaponRecoilDeviationX( 0.0f ),
    weaponRecoilDeviationY( 0.0f ),
    weaponRecoilTime( 0.0f ),
    weaponClipSize( 0 ),

    ammo(10),
    ammoExtra(10),
    fireMode(ncGPWeaponClass::FM_SEMI),

    isIronsight( false ),
    isProne( false ),
    isCrouch( false ),
    isMoving( false ),

    max_slots( 0 ),
    slots_types( 0 ),
    slots_addons( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncGPWeapon::~ncGPWeapon()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncGPWeapon::InitInstance( nObject::InitInstanceMsg /*initType*/ )
{
    ncGPWeaponClass * weaponClass = this->GetClassComponent<ncGPWeaponClass>();
    n_assert( weaponClass );
    if( weaponClass )
    {
        // get the slot mask from class object
        int slots = weaponClass->GetSlots();

        // count the number of slots
        int count = 0;
        for( int mask = 1 ; mask < ncGPWeaponCharsClass::SLOT_LAST ; mask <<= 1 )
        {
            if( (mask & slots) != 0 )
            {
                ++count;
            }
        }

        if( weaponClass->HasAutoFireMode() )
        {
            this->fireMode = ncGPWeaponClass::FM_AUTO;
        }
        else if( weaponClass->HasSemiFireMode() )
        {
            this->fireMode = ncGPWeaponClass::FM_SEMI;
        }
        else
        {
            n_assert2_always( "Weapon without right fire mode" );
        }
    }

    this->CalculateWeaponChars();

    this->ammo = this->weaponClipSize;
    this->ammoExtra = this->weaponClipSize;

    this->actualAccuracy = this->weaponAccuracyMax;
}

//------------------------------------------------------------------------------
/**
*/
void
ncGPWeapon::CalculateWeaponChars()
{
    // init weapon characteristics 
    this->weaponAccuracyMax = 0.0f;
    this->weaponAccuracyMin = 0.0f;
    this->weaponRange = 0.0f;
    this->weaponDamage = 0.0f;
    this->weaponAccuracyLoss = 0.0f;
    this->weaponAimSpeed = 0.0f;
    this->weaponRecoilDeviationX = 0.0f;
    this->weaponRecoilDeviationY = 0.0f;
    this->weaponRecoilTime = 0.0f;
    this->weaponClipSize = 0;

    // init local characteristics
    float weaponAccuracyIronsight = 0.0f;
    float weaponAccuracyProne = 0.0f;
    float weaponAccuracyCrouch = 0.0f;
    float weaponAccuracyMove = 0.0f;

    // get weapon characteristics from weapon class
    ncGPWeaponClass * weaponClass = this->GetClassComponent<ncGPWeaponClass>();
    n_assert( weaponClass );
    if( weaponClass )
    {
        this->weaponAccuracyMax += weaponClass->GetAccuracyMax();
        this->weaponAccuracyMin += weaponClass->GetAccuracyMin();
        this->weaponDamage += weaponClass->GetDamage();
        this->weaponRange += weaponClass->GetRange();
        this->weaponAccuracyLoss += weaponClass->GetAccuracyLoss();
        this->weaponRecoilTime += weaponClass->GetRecoilTime();
        weaponAccuracyIronsight += weaponClass->GetAccuracyIronsight();
        if( this->isProne )
        {
            this->weaponAimSpeed += weaponClass->GetAimSpeedProne();
            this->weaponRecoilDeviationX += weaponClass->GetRecoilDeviationProneX();
            this->weaponRecoilDeviationY += weaponClass->GetRecoilDeviationProneY();
            weaponAccuracyProne += weaponClass->GetAccuracyProne();
        }
        else
        {
            if( this->isCrouch )
            {
                weaponAccuracyCrouch += weaponClass->GetAccuracyCrouch();
            }

            this->weaponAimSpeed += weaponClass->GetAimSpeed();
            this->weaponRecoilDeviationX += weaponClass->GetRecoilDeviationX();
            this->weaponRecoilDeviationY += weaponClass->GetRecoilDeviationY();
        }
        
        if( this->isMoving )
        {
            weaponAccuracyMove += weaponClass->GetAccuracyMove();
        }

        this->weaponClipSize += weaponClass->GetClipSize();
    }

    // adjust maximum accuracy
    if( this->isIronsight )
    {
        this->weaponAccuracyMax *= weaponAccuracyIronsight;
    }

    if( this->isProne )
    {
        this->weaponAccuracyMax *= weaponAccuracyProne;
    }
    else if( this->isCrouch )
    {
        this->weaponAccuracyMax *= weaponAccuracyCrouch;
    }

    if( this->isMoving )
    {
        this->weaponAccuracyMax *= weaponAccuracyMove;
    }

    n_assert( this->weaponAccuracyMax <= 1.0 );

    // adjust actual accuracy
    if( this->actualAccuracy > this->weaponAccuracyMax )
    {
        this->actualAccuracy = this->weaponAccuracyMax;
    }
    if( this->actualAccuracy < this->weaponAccuracyMin )
    {
        this->actualAccuracy = this->weaponAccuracyMin;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncGPWeapon::ApplyAccuracyLoss()
{
    this->actualAccuracy -= this->weaponAccuracyLoss;
    if( this->actualAccuracy < this->weaponAccuracyMin )
    {
        this->actualAccuracy = this->weaponAccuracyMin;
    }
}

//------------------------------------------------------------------------------
/**
    @param deltatime time trascurred from last tick
*/
void
ncGPWeapon::ApplyAccuracyRecover( float deltaTime )
{
    this->actualAccuracy += deltaTime * this->weaponAimSpeed;
    if( this->actualAccuracy > this->weaponAccuracyMax )
    {
        this->actualAccuracy = this->weaponAccuracyMax;
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncGPWeapon::GetAmmoType()const
{
    const ncGPWeaponClass * weaponClass = this->GetClassComponentSafe<ncGPWeaponClass>();
    if( weaponClass )
    {
        return weaponClass->GetAmmoType();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @returns actual rate of fire
*/
float
ncGPWeapon::GetRateOfFire()const
{
    const ncGPWeaponClass * weaponClass = this->GetClassComponentSafe<ncGPWeaponClass>();
    if( weaponClass )
    {
        if( this->fireMode == ncGPWeaponClass::FM_AUTO )
        {
            return weaponClass->GetRateOfFire();
        }
        return weaponClass->GetRateOfTapFire();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    @param mode new fire mode
*/
void
ncGPWeapon::SetFireMode( int mode )
{
    const ncGPWeaponClass * weaponClass = this->GetClassComponentSafe<ncGPWeaponClass>();
    if( weaponClass )
    {
        if( mode & weaponClass->GetFireModes() )
        {
            this->fireMode = ncGPWeaponClass::FireMode( mode );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @returns the down offset
*/
float
ncGPWeapon::GetDownOffset()const
{
    float downOffset = 0;

    return downOffset;
}

//------------------------------------------------------------------------------
/**
    @returns true if has full the magazines
*/
bool
ncGPWeapon::HasFullAmmo()const
{
    if( this->ammo == this->weaponClipSize )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @param hideValue if true, hide weapon
*/
void
ncGPWeapon::SetScopeState( bool hideValue )
{
    ncScene * scene = 0;

    scene = this->GetEntityObject()->GetComponent<ncScene>();
    if( scene )
    {
        scene->SetHiddenEntity( hideValue );
    }

    for( int i = 0 ; i < this->max_slots ; ++i )
    {
        if( this->slots_addons[ i ] )
        {
            scene = this->slots_addons[ i ]->GetComponent<ncScene>();
            if( scene )
            {
                scene->SetHiddenEntity( hideValue );
            }
        }
    }

}

//------------------------------------------------------------------------------
/**
    @returns true if need full reload
*/
bool
ncGPWeapon::NeedFullReload() const
{
    if( 0 == this->ammoExtra )
    {
        return true;
    }

    if( 0 == this->ammo && 0 == this->ammoExtra )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
