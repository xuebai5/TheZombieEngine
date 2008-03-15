#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpreloadweapon.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "rnsgameplay/ncgpweaponcharsclass.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncgameplayplayer/ncgameplayplayer.h"

#include "nscene/ncscene.h"

#include "ncsoundlayer/ncsoundlayer.h"

#include "ngpreloadweapon/ngpreloadweapon.h"

#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nGPReloadWeapon, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPReloadWeapon)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, bool), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPReloadWeapon::nGPReloadWeapon() :
    weapon( 0 ),
    animator( 0 ),
    gameplay( 0 ),
    animIndex( -1 ),
    removedBullets( 0 ),
    middleChange( false )
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPReloadWeapon::~nGPReloadWeapon()
{
    if( this->init && this->state != RS_END )
    {
        // restore initial state
        if( this->animator )
        {
            this->animator->SetIdle();    
        }
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPReloadWeapon::Init (nEntityObject* entity, bool fastReload )
{
    bool valid = true;

    this->entity = entity;

    if ( valid )
    {
        this->gameplay = entity->GetComponent<ncGameplayLiving>();
        this->animator = entity->GetComponent<ncLogicAnimator>();
        valid = (this->gameplay != 0) && (this->animator != 0);
        n_assert( valid );
    }

    if( valid )
    {
        if (this->gameplay->GetCurrentWeapon())
        {
            nEntityObject * weaponEntity = this->gameplay->GetCurrentWeapon();
            if( weaponEntity )
            {
                this->weapon = weaponEntity->GetComponentSafe<ncGPWeapon>();
                if( this->weapon )
                {
                    valid = this->CalculateAmmo( fastReload );
                }
            }
        }
        else
        {
            valid = false;
        }
    }

    if( valid )
    {
        ncGameplayPlayer* player = entity->GetComponentSafe<ncGameplayPlayer>();
        if ( player && this->gameplay->IsProne() )
        {
            player->MovementFlag ( ncGameplayPlayer::MF_UPDATEPOSITION, false );
        }
        this->gameplay->SetBusy( ncGameplayLiving::BL_NORMAL );

        player->MovementFlag( ncGameplayPlayer::MF_BLOCKSPRINT, true );
    }

    this->init = valid;

    if ( valid )
    {
        n_assert( this->weapon );
        this->weapon->GetComponentSafe<ncDictionary>()->SetLocalIntVariable( "reloading", 1 );
    }

    return valid;
}

//------------------------------------------------------------------------------
/**
    @param fastReload 
*/
bool
nGPReloadWeapon::CalculateAmmo( bool fastReload )
{
    // don't reload if we have full magazines
    if( this->weapon->HasFullAmmo() )
    {
        return false;
    }

    // don't fast reload if we have no ammo in weapon
    if( 0 == this->weapon->GetAmmo() && 0 == this->weapon->GetAmmoExtra() )
    {
        fastReload = false;
    }
    
    // if actual magazine has no ammo we need to cock
    if( 0 == this->weapon->GetAmmo() )
    {
        this->state = RS_NEEDCOCK;
    }
    else
    {
        this->state = RS_RELOADING;
    }

    ncSoundLayer * soundLayer = this->weapon->GetComponentSafe<ncSoundLayer>();

    this->removedBullets = 0;

    if( fastReload )
    {
        this->newAmmo = this->weapon->GetAmmoExtra();
        this->newAmmoExtra = this->weapon->GetAmmo();

        // put reload sound
        if( soundLayer )
        {
            soundLayer->PlaySound( GP_ACTION_FASTFIRSTRELOAD, false );
        }
    }
    else
    {
        // get maximum bullets in the inventory
        nInventoryContainer * inventory = 0;
        ncGameplayLiving * gameplay = 0;

        gameplay = this->entity->GetComponent<ncGameplayLiving>();

        if( gameplay )
        {
            inventory = gameplay->GetInventory();
        }

        int bullets = 0;
        if( inventory )
        {
            bullets = inventory->GetNumBullets( this->weapon->GetAmmoType() );
        }

        int inventoryBullets = bullets;

        // calculate the bullets already in the weapon
        bullets += this->weapon->GetAmmo();
        if( bullets <= 0 )
        {
            return false;
        }

        // create clips with inventory bullets
        int clipSize = this->weapon->GetClipSize();

        this->newAmmo = n_min( clipSize, bullets );
        bullets -= this->newAmmo;

        this->newAmmoExtra = 0;
        this->removedBullets = inventoryBullets - bullets;

        // animations with single magazine
        this->animIndex = this->animator->SetFullReload();

        this->middleChange = true;

        // put reload sound
        if( soundLayer )
        {
            soundLayer->PlaySound( GP_ACTION_FULLRELOAD, false );
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Run
*/
bool
nGPReloadWeapon::Run()
{
    bool done = this->IsDone();

    if( ! done )
    {
        if( ( this->animIndex == -1 ) || this->animator->HasFinished( this->animIndex ) )
        {
            ncSoundLayer * soundLayer = this->weapon->GetComponentSafe<ncSoundLayer>();
            switch( this->state )
            {
            case RS_RELOADING:
                this->animIndex = this->animator->SetEndReload();
                this->state = RS_END;
                break;

            case RS_NEEDCOCK:
                this->animIndex = this->animator->SetCockReload();
                if( soundLayer )
                {
                    if ( gameplay->IsCrouching() )
                    {
                        soundLayer->PlaySound( GP_ACTION_COCKRELOADCROUCH, false );
                    }
                    else if ( gameplay->IsProne() )
                    {
                        soundLayer->PlaySound( GP_ACTION_COCKRELOADPRONE, false );
                    }
                    else
                    {
                        soundLayer->PlaySound( GP_ACTION_COCKRELOAD, false );
                    }
                }
                this->state = RS_END;
                break;

            case RS_END:
                {
                    // update weapon
                    this->weapon->SetAmmo( this->newAmmo );
                    this->weapon->SetAmmoExtra( this->newAmmoExtra );

                    // remove bullets from inventory
                    ncGameplayLiving * gameplay = this->entity->GetComponent<ncGameplayLiving>();
                    if( gameplay )
                    {
                        nInventoryContainer * inventory = gameplay->GetInventory();
                        if( inventory )
                        {
                            inventory->RemoveBullets( this->weapon->GetAmmoType(), this->removedBullets );
                        }
                    }

                    if( soundLayer )
                    {
                        soundLayer->StopSound();
                    }
                    done = true;
                }
                break;

            default:
                n_assert_always();
            }
        }
        else if( this->middleChange && this->state != RS_END && ( this->animIndex != -1 ) )
        {
            int index = this->animator->GetFirstPersonStateIndex();
            if( index != -1 )
            {
                float remaining = this->animator->GetRemainingTime( index, true );
                float total = this->animator->GetStateDuration( index, true );

                if( remaining < ( total / 2.0f ) )
                {
                    this->middleChange = false;
                }
            }
        }
    }

    if( done )
    {
        ncGameplayPlayer* player = entity->GetComponentSafe<ncGameplayPlayer>();
        if ( player && this->gameplay->IsProne() )
        {
            player->MovementFlag ( ncGameplayPlayer::MF_UPDATEPOSITION, true );
        }

        this->gameplay->SetBusy( ncGameplayLiving::BL_NONE );
        player->MovementFlag( ncGameplayPlayer::MF_BLOCKSPRINT, false );

        n_assert( this->weapon );
        this->weapon->GetComponentSafe<ncDictionary>()->SetLocalIntVariable( "reloading", 0 );
    }

    return done;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPReloadWeapon::IsDone() const
{
    n_assert( this->init );

    return false;
}
