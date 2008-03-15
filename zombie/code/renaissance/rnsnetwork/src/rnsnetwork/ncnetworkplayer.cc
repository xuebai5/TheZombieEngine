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
