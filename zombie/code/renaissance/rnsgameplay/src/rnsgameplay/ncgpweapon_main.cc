//------------------------------------------------------------------------------
//  ncgpweapon_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpweapon.h"
/** ZOMBIE REMOVE
#include "rnsgameplay/ncgpweaponaddonclass.h"
*/
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
/** ZOMBIE REMOVE
    if( this->slots_addons )
    {
        this->DeleteAllAddons();
        n_delete( this->slots_addons );
        this->slots_addons = 0;
    }

    if( this->defaultMag )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->defaultMag );
        this->defaultMag = 0;
    }

    if( this->slots_types )
    {
        n_delete( this->slots_types );
        this->slots_types = 0;
    }
*/
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
/** ZOMBIE REMOVE
        // create the array of slots
        if (initType != nObject::ReloadedInstance)
        {
            this->slots_addons = n_new_array( nEntityObject * , count );
            this->slots_types = n_new_array( int, count );
            this->max_slots = count;
        }

        // init the slots
        int i = 0;
        for( int mask = 1; 
            mask < ncGPWeaponCharsClass::SLOT_LAST && i < count; 
            mask <<= 1 )
        {
            if( (mask & slots) != 0 )
            {
                this->slots_types[ i ] = (mask & slots);
                this->slots_addons[ i ] = 0;
                ++i;
            }
        }
*/
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

        // create the default magazine
        nString name = weaponClass->GetDefaultMagazine();
        if( name.Length() )
        {
            this->defaultMag = nEntityObjectServer::Instance()->NewLocalEntityObject( name.Get() );
            n_assert( this->defaultMag );
            if( this->defaultMag )
            {
/** ZOMBIE REMOVE
                int valid = this->AddAddon( this->defaultMag );
                n_assert( valid );
*/
            }
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
/** ZOMBIE REMOVE
    // get addon characteristics from addon class
    ncGPWeaponAddonClass * addonClass;
    for( int i = 0 ; i < this->max_slots ; ++i )
    {
        if( this->slots_addons[ i ] )
        {
            addonClass = this->slots_addons[ i ]->GetClassComponent<ncGPWeaponAddonClass>();
            n_assert( addonClass );
            if( addonClass )
            {
                this->weaponAccuracyMax += addonClass->GetAccuracyMax();
                this->weaponAccuracyMin += addonClass->GetAccuracyMin();
                this->weaponDamage += addonClass->GetDamage();
                this->weaponRange += addonClass->GetRange();
                this->weaponAccuracyLoss += addonClass->GetAccuracyLoss();
                this->weaponRecoilTime += addonClass->GetRecoilTime();
                weaponAccuracyIronsight += addonClass->GetAccuracyIronsight();
                if( this->isProne )
                {
                    this->weaponAimSpeed += addonClass->GetAimSpeedProne();
                    this->weaponRecoilDeviationX += addonClass->GetRecoilDeviationProneX();
                    this->weaponRecoilDeviationY += addonClass->GetRecoilDeviationProneY();
                    weaponAccuracyProne += addonClass->GetAccuracyProne();
                }
                else
                {
                    if( this->isCrouch )
                    {
                        weaponAccuracyCrouch += addonClass->GetAccuracyCrouch();
                    }
                    this->weaponAimSpeed += addonClass->GetAimSpeed();
                    this->weaponRecoilDeviationX += addonClass->GetRecoilDeviationX();
                    this->weaponRecoilDeviationY += addonClass->GetRecoilDeviationY();
                }

                if( this->isMoving )
                {
                    weaponAccuracyMove += weaponClass->GetAccuracyMove();
                }

                this->weaponClipSize += addonClass->GetClipSize();
            }
        }
    }
*/
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
/** ZOMBIE REMOVE
    // down the iron if there is an addon in TOP slot
    ncDictionary * dictionary = this->GetComponent<ncDictionary>();
    if( dictionary )
    {
        bool downIron = this->HasAddon( ncGPWeaponCharsClass::SLOT_TOP );
        dictionary->SetIntVariable( "ironDownState", downIron ? 1 : 0 );
    }
*/
}

//------------------------------------------------------------------------------
/**
    @param addon the addon to check if its allowed
    @retval true if addon is allowed in the weapon
*/
/** ZOMBIE REMOVE
bool
ncGPWeapon::IsAllowed( nEntityObject * addon )
{
    n_assert( addon );
    if( addon )
    {
        ncGPWeaponAddonClass * addonClass = addon->GetClassComponent<ncGPWeaponAddonClass>();
        n_assert( addonClass );
        if( addonClass )
        {
            ncGPWeaponClass * weaponClass = this->GetClassComponent<ncGPWeaponClass>();
            n_assert( weaponClass );
            if( weaponClass )
            {
                if( 0 == (weaponClass->GetWeaponType() & addonClass->GetWeapons()))
                {
                    return false;
                }
            }

            int slots = addonClass->GetSlots();
            for( int i = 0 ; i < this->max_slots ; ++i )
            {
                if( ( ! this->slots_addons[ i ] || this->slots_addons[ i ] == this->defaultMag ) &&
                    ( slots & this->slots_types[ i ] ) )
                {
                    return true;
                }
            }
        }
    }
    return false;
}
*/
//------------------------------------------------------------------------------
/**
    @param addon the addon to check if its mounted
    @retval true if addon is allowed in the weapon
*/
/** ZOMBIE REMOVE
bool
ncGPWeapon::IsMounted( nEntityObject * addon )
{
    n_assert( addon );
    if( addon )
    {
        for( int i = 0 ; i < this->max_slots ; ++i )
        {
            if( addon == this->slots_addons[ i ] )
            {
                return true;
            }
        }
    }
    return false;
}
*/
//------------------------------------------------------------------------------
/**
    @param addon the addon to get the name of the slot
    @returns the name of the slot
*/
/** ZOMBIE REMOVE
const char *
ncGPWeapon::GetSlotName( nEntityObject * addon )const
{
    n_assert( addon );
    if( addon )
    {
        bool valid = true;
        int slotsAddon = 0;
        int slotsWeapon = 0;

        ncGPWeaponAddonClass * addonClass = addon->GetClassComponent<ncGPWeaponAddonClass>();
        valid = addonClass != 0;
        n_assert( valid );
        if( valid )
        {
            slotsAddon = addonClass->GetSlots();
        }

        const ncGPWeaponClass * weaponClass = this->GetClassComponent<ncGPWeaponClass>();
        n_assert( weaponClass );
        if( weaponClass )
        {
            slotsWeapon = weaponClass->GetSlots();
        }

        if( valid )
        {
            return ncGPWeaponCharsClass::GetSlotName( slotsAddon & slotsWeapon );
        }
    }

    return 0;
}
*/
//------------------------------------------------------------------------------
/**
    @param addon the addon to get the name of the slot
    @returns the slot
*/
/** ZOMBIE REMOVE
int
ncGPWeapon::GetSlotFor( nEntityObject * addon )const
{
    n_assert( addon );
    if( addon )
    {
        bool valid = true;
        int slotsAddon = 0;
        int slotsWeapon = 0;

        ncGPWeaponAddonClass * addonClass = addon->GetClassComponent<ncGPWeaponAddonClass>();
        valid = addonClass != 0;
        n_assert( valid );
        if( valid )
        {
            slotsAddon = addonClass->GetSlots();
        }

        const ncGPWeaponClass * weaponClass = this->GetClassComponent<ncGPWeaponClass>();
        n_assert( weaponClass );
        if( weaponClass )
        {
            slotsWeapon = weaponClass->GetSlots();
        }

        if( valid )
        {
            return ( slotsAddon & slotsWeapon );
        }
    }

    return 0;
}
*/
//------------------------------------------------------------------------------
/**
    @param addon the addon to add to the weapon
    @returns the slot when the addon is put on
    @retval 0 if the addon can not be added
*/
/** ZOMBIE REMOVE
int
ncGPWeapon::AddAddon( nEntityObject * addon  )
{

    n_assert( addon );
    if( addon )
    {
        if( ! this->IsAllowed( addon ) )
        {
            return 0;
        }

        ncGPWeaponAddonClass * addonClass = addon->GetClassComponent<ncGPWeaponAddonClass>();
        n_assert( addonClass );
        if( addonClass )
        {
            for( int i = 0 ; i < this->max_slots ; ++i )
            {
                if( ( ! this->slots_addons[ i ] || this->slots_addons[ i ] == this->defaultMag ) && 
                    addonClass->AllowType( this->slots_types[ i ] ) )
                {
                    // hide the default magazine if needed
                    if( this->defaultMag && this->defaultMag == this->slots_addons[ i ] )
                    {
                        ncSpatial * spatial = this->defaultMag->GetComponent<ncSpatial>();
                        if( spatial )
                        {
                            spatial->RemoveFromSpaces();
                        }
                    }

                    this->slots_addons[ i ] = addon;

                    // plug addon in weapon
                    const char * slotName = this->GetSlotName( addon );
                    n_assert( slotName );
                    if( slotName )
                    {
                        ncScene * scene = this->GetComponentSafe<ncScene>();
                        if( scene )
                        {
                            scene->Plug( slotName, addon->GetId() );
                        }
                    }

                    // calculate new weapon stats
                    this->CalculateWeaponChars();
                    return this->slots_types[ i ];
                }
            }
        }
    }

    return 0;
}
*/
//------------------------------------------------------------------------------
/**
    @param type the slot when the addon is on
    @returns the addon removed
    @retval 0 if no addon can be removed
*/
/** ZOMBIE REMOVE
nEntityObject *
ncGPWeapon::RemoveAddon( int type )
{
    bool valid = ( type > 0 && type < int( ncGPWeaponCharsClass::SLOT_LAST ) );
    n_assert( valid );
    if( valid )
    {
        for( int i = 0 ; i < this->max_slots ; ++i )
        {
            if( this->slots_addons[ i ] && ( this->slots_types[ i ] == type ) )
            {
                return this->RemoveAddonAt( i );
            }
        }
    }

    return 0;
}
*/
//------------------------------------------------------------------------------
/**
    @param index index in the array of addons
    @returns the addon removed
    @retval 0 if no addon can be removed
*/
/** ZOMBIE REMOVE
nEntityObject *
ncGPWeapon::RemoveAddonAt( int index )
{
    bool valid = ( index >= 0 && index < this->max_slots );
    n_assert( valid );
    if( valid )
    {
        nEntityObject * addon = this->slots_addons[ index ];
        n_assert( addon );
        if( addon )
        {
            int type = this->slots_types[ index ];

            this->slots_addons[ index ] = 0;

            // unplug addon from weapon
            ncScene * scene = this->GetComponentSafe<ncScene>();
            if( scene )
            {
                scene->UnPlug( addon->GetId() );
            }

            // put default magazine if a magazine is changed
            if( ( type == ncGPWeaponCharsClass::SLOT_MAG_HICAP ) || 
                ( type == ncGPWeaponCharsClass::SLOT_MAG_CLAMP ) )
            {
                if( this->defaultMag )
                {
                    int valid = this->AddAddon( this->defaultMag );
                    n_assert( valid );
                }
            }

            // calculate new weapon stats
            this->CalculateWeaponChars();
        }

        return addon;
    }

    return 0;
}
*/
//------------------------------------------------------------------------------
/**
    @param type the slot when the addon is on
    @returns the addon or 0
*/
/** ZOMBIE REMOVE
nEntityObject*
ncGPWeapon::GetAddon( int type )
{
    bool valid = ( type > 0 && type < int( ncGPWeaponCharsClass::SLOT_LAST ) );
    n_assert( valid );
    if( valid )
    {
        for( int i = 0 ; i < this->max_slots ; ++i )
        {
            if( this->slots_addons[ i ] && ( this->slots_types[ i ] == type ) )
            {
                return this->slots_addons[ i ];
            }
        }
    }

    return 0;
}
*/
//------------------------------------------------------------------------------
/**
    @param index index of the slot when the addon is on
    @returns the addon or 0
*/
/** ZOMBIE REMOVE
nEntityObject*
ncGPWeapon::GetAddonAt( int index )
{
    bool valid = ( index >= 0 && index < this->max_slots );
    n_assert( valid );
    if( valid )
    {
        return this->slots_addons[ index ];
    }

    return 0;
}
*/
//------------------------------------------------------------------------------
/**
    @param type the slot when the addon is on
    @returns the addon or 0
*/
/** ZOMBIE REMOVE
nArray<nEntityObject*>
ncGPWeapon::GetAddonsByTrait( int type )
{
    bool valid = ( type > 0 && type < int( ncGPWeaponCharsClass::MOD_LAST ) );
    n_assert( valid );
    nArray<nEntityObject*> addonsList(0, 1);
    if( valid )
    {
        for( int i = 0 ; i < this->max_slots ; ++i )
        {
            if( this->slots_addons[ i ] )
            {
                ncGPWeaponCharsClass* addonChars = 0;
                addonChars = this->slots_addons[ i ]->GetClassComponentSafe<ncGPWeaponCharsClass>();
                if ( addonChars && addonChars->AllowTrait( type ) )
                {
                    addonsList.Append( this->slots_addons[ i ] );
                }
            }
        }
    }

    return addonsList;
}
*/
//------------------------------------------------------------------------------
/**
    @param type the slot when the addon is on
    @returns true if there is an addon
*/
/** ZOMBIE REMOVE
bool
ncGPWeapon::HasAddon( int type )
{
    return ( this->GetAddon( type ) != 0 );
}
*/
//------------------------------------------------------------------------------
/**
*/
/** ZOMBIE REMOVE
void
ncGPWeapon::DeleteAllAddons( )
{
    for( int i = 0 ; i < this->max_slots ; ++i )
    {
        if( this->slots_addons[ i ] )
        {
            this->slots_addons[ i ] = 0;
        }
    }
    this->CalculateWeaponChars();
}
*/
//------------------------------------------------------------------------------
/**
    @param trait the trait to check
    @retval true if the weapon has the trait
*/
/** ZOMBIE REMOVE
bool
ncGPWeapon::HasTrait( int trait )const
{
    const ncGPWeaponClass * weaponClass = this->GetClassComponent<ncGPWeaponClass>();
    n_assert( weaponClass );
    if( weaponClass )
    {
        if( weaponClass->AllowTrait( trait ) )
        {
            return true;
        }
    }

    ncGPWeaponAddonClass * addonClass;
    for( int i = 0 ; i < this->max_slots ; ++i )
    {
        if( this->slots_addons[ i ] )
        {
            addonClass = this->slots_addons[ i ]->GetClassComponent<ncGPWeaponAddonClass>();
            n_assert( addonClass );
            if( addonClass )
            {
                if( addonClass->AllowTrait( trait ) )
                {
                    return true;
                }
            }
        }
    }

    return false;
}
*/
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
/** ZOMBIE REMOVE
    ncGPWeaponAddonClass * addonClass;
    for( int i = 0 ; i < this->max_slots ; ++i )
    {
        if( this->slots_addons[ i ] )
        {
            addonClass = this->slots_addons[ i ]->GetClassComponentSafe<ncGPWeaponAddonClass>();
            if( addonClass )
            {
                downOffset += addonClass->GetDownOffset();
            }
        }
    }
*/
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
/** ZOMBIE REMOVE
        if( this->HasTrait( ncGPWeaponCharsClass::MOD_ENABLE_QUICK_RELOAD ) )
        {
            return ( this->ammoExtra == this->weaponClipSize );
        }
*/
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
/** ZOMBIE REMOVE
    if( ! this->HasTrait( ncGPWeaponCharsClass::MOD_ENABLE_QUICK_RELOAD ) )
    {
        return true;
    }
*/
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
