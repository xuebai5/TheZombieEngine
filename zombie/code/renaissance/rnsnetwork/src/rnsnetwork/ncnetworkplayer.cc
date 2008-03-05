//------------------------------------------------------------------------------
//  ncnetworkplayer.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsnetwork.h"

#include "rnsnetwork/ncnetworkplayer.h"

#include "ncgameplayplayer/ncgameplayplayer.h"
#include "rnsnetwork/ncnetworkliving.h"

#include "rnsgameplay/ninventoryitem.h"

#include "nscene/ncscenelod.h"

#include "nspatial/ncspatial.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncNetworkPlayer,ncNetworkLiving);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncNetworkPlayer)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncNetworkPlayer::ncNetworkPlayer():
    playerComp( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncNetworkPlayer::~ncNetworkPlayer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncNetworkPlayer::InitInstance(nObject::InitInstanceMsg initType)
{
    this->playerComp = this->GetComponentSafe<ncGameplayPlayer>();

    ncNetworkLiving::InitInstance( initType );
}

//------------------------------------------------------------------------------
/**
    @param buffer where put the update data
    @returns true if can be updated
*/
void
ncNetworkPlayer::UpdateNetwork( nstream & buffer )
{
    ncNetworkLiving::UpdateNetwork( buffer );

    if( this->playerComp )
    {
        bool thereIsWeapon = false;

        // update weapon state
        nEntityObject * weaponEntity = this->playerComp->GetCurrentWeapon();

        if( buffer.GetWrite() )
        {
            thereIsWeapon = ( 0 != weaponEntity );
        }

        buffer.UpdateBool( thereIsWeapon );

        if( thereIsWeapon && weaponEntity )
        {
            ncGPWeapon * weapon = weaponEntity->GetComponentSafe< ncGPWeapon >();
            if( weapon )
            {
                int ammo, ammoExtra;

                if( buffer.GetWrite() )
                {
                    ammo = weapon->GetAmmo();
                    ammoExtra = weapon->GetAmmoExtra();
                }

                buffer.UpdateInt( ammo );
                buffer.UpdateInt( ammoExtra );

                if( ! buffer.GetWrite() )
                {
                    weapon->SetAmmo( ammo );
                    weapon->SetAmmoExtra( ammoExtra );
                }
/** ZOMBIE REMOVE
                nInventoryContainer * inventory = this->playerComp->GetInventory();

                if( inventory )
                {
                    const int DEFAULT_MAG = -2;
                    const int INVALID_SLOT = -1;

                    int numSlots = weapon->GetNumSlots();
                    int addonSlot;
                    nEntityObject * addon = 0, * oldAddon = 0;
                    nInventoryItem * item = 0;
                    bool changedAddon = false;

                    for( int i = 0 ; i < numSlots ; ++i )
                    {
                        if( buffer.GetWrite() )
                        {
                            addon = weapon->GetAddonAt( i );
                            if( addon )
                            {
                                addonSlot = inventory->GetEntitySlot( addon );
                                if( INVALID_SLOT == addonSlot )
                                {
                                    if( weapon->GetDefaultMagazine() == addon )
                                    {
                                        addonSlot = DEFAULT_MAG;
                                    }
                                }
                            }
                            else
                            {
                                addonSlot = INVALID_SLOT;
                            }
                        }

                        buffer.UpdateInt( addonSlot );

                        if( ! buffer.GetWrite() )
                        {
                            if( DEFAULT_MAG == addonSlot )
                            {
                                addon = weapon->GetDefaultMagazine();
                            }
                            else if( INVALID_SLOT == addonSlot )
                            {
                                addon = 0;
                            }
                            else
                            {
                                item = inventory->GetItemAt( addonSlot );
                                n_assert( item );
                                if( item )
                                {
                                    addon = item->GetEntity();
                                }
                            }

                            oldAddon = weapon->GetAddonAt( i );

                            // change addon if is different that server
                            if( oldAddon != addon )
                            {
                                ncGPWeaponCharsClass * addonChars = 0;

                                changedAddon = true;

                                // remove old addon if neccesary
                                if( oldAddon )
                                {
                                    oldAddon = weapon->RemoveAddonAt( i );

                                    addonChars = oldAddon->GetClassComponentSafe< ncGPWeaponCharsClass >();
                                    if( addonChars && addonChars->AllowTrait( ncGPWeaponCharsClass::MOD_ENABLE_LIGHT ) )
                                    {
                                        oldAddon->Call( "RemoveFlashlight", 0, 0 );
                                    }

                                    if( addonChars && addonChars->AllowTrait( ncGPWeaponCharsClass::MOD_ENABLE_DOTSIGHT ) )
                                    {
                                        oldAddon->Call( "RemoveDotlaser", 0, 0 );
                                    }

                                    // hide the addon
                                    ncSpatial * spatial = oldAddon->GetComponent< ncSpatial >();
                                    if( spatial )
                                    {
                                        spatial->RemoveFromSpaces();
                                    }

                                    item = inventory->GetItemWith( oldAddon );
                                    if( item )
                                    {
                                        item->SetUsed( false );
                                    }
                                }

                                // put new addon if neccesary
                                if( addon )
                                {
                                    weapon->AddAddon( addon );
                                    
                                    addonChars = addon->GetClassComponentSafe< ncGPWeaponCharsClass >();
                                    if( addonChars && addonChars->AllowTrait( ncGPWeaponCharsClass::MOD_ENABLE_LIGHT ) )
                                    {
                                        addon->Call( "CreateFlashlight", 0, 0 );
                                    }

                                    if( addonChars && addonChars->AllowTrait( ncGPWeaponCharsClass::MOD_ENABLE_DOTSIGHT ) )
                                    {
                                        addon->Call( "CreateDotlaser", 0, 0 );
                                    }

                                    // put first person LOD in the addon
                                    ncSceneLod * scene = addon->GetComponent< ncSceneLod >();
                                    if( scene)
                                    {
                                        scene->SetLockedLevel( this->playerComp->IsFirstCameraMode(), 0 );
                                    }

                                    item = inventory->GetItemWith( addon );
                                    if( item )
                                    {
                                        item->SetUsed( true );
                                    }
                                }
                            }
                        }
                    }

                    if( changedAddon )
                    {
                        this->GetEntityObject()->Call( "SetArmOffset", 0, 0 );
                    }
                }
*/
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param buffer where put the update data
    @returns true if can be initialized
*/
bool
ncNetworkPlayer::InitFromNetwork( nstream & buffer )
{
    ncNetworkLiving::InitFromNetwork( buffer );

    if( buffer.GetWrite() )
    {
        this->CopyTo( buffer );
    }
    else
    {
        this->InitializeWith( buffer );
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @param data where put the initialize data
*/
void
ncNetworkPlayer::CopyTo( nstream & data )
{
    if( this->playerComp )
    {
        vector3 position;
        polar2 angles;

        // copy the actual position of player to buffer
        this->playerComp->GetShotPosition( position );
        this->playerComp->GetShotAngles( angles );

        data.UpdateVector3( position );
        data.UpdateFloat( angles.rho );
        data.UpdateFloat( angles.theta );
    }
}

//------------------------------------------------------------------------------
/**
    @param data from where get the initialize data
*/
void
ncNetworkPlayer::InitializeWith( nstream & data )
{
    if( this->playerComp )
    {
        vector3 position;
        polar2 angles;

        // copy the actual position from buffer
        data.UpdateVector3( position );
        data.UpdateFloat( angles.rho );
        data.UpdateFloat( angles.theta );

        this->playerComp->SetShotPosition( position );
        this->playerComp->SetShotAngles( angles );
    }
}

//------------------------------------------------------------------------------
