#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpreloadweapon.cc
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
        this->RestoreMagazine();
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
/** ZOMBIE REMOVE
    bool hasQuickReload = this->weapon->HasTrait( ncGPWeaponCharsClass::MOD_ENABLE_QUICK_RELOAD );
    ncScene * weaponScene = this->weapon->GetComponentSafe<ncScene>();
    nEntityObject * addon = 0;
    addon = this->weapon->GetAddon( ncGPWeaponCharsClass::SLOT_MAG_CLAMP );
    if( ! addon )
    {
        addon = this->weapon->GetAddon( ncGPWeaponCharsClass::SLOT_MAG_HICAP );
    }

    // get slot of the magazine addon
    if( addon )
    {
        this->oldPlugName = weaponScene->GetPlugNameEntity( addon->GetId() );
    }

    // check if can fast reload
    fastReload = fastReload && hasQuickReload;
*/
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
/** ZOMBIE REMOVE
        // put reload animation
        if( addon && weaponScene )
        {
            this->plugName = weaponScene->GetPlugNameEntity( addon->GetId() );
            int nameLength = this->plugName.Length();
            n_assert( nameLength > 0 );
            if( nameLength > 0 )
            {
                if( this->plugName[ nameLength - 1 ] == '2' )
                {
                    this->animIndex = this->animator->SetFastSecondReload();
                    this->plugName.TerminateAtIndex( nameLength - 1 );
                }
                else
                {
                    this->animIndex = this->animator->SetFastFirstReload();
                    this->plugName.Append( "2" );
                }
            }
        }
*/
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
/** ZOMBIE REMOVE
        if( hasQuickReload )
        {
            bullets += this->weapon->GetAmmoExtra();
        }
*/
        if( bullets <= 0 )
        {
            return false;
        }

        // create clips with inventory bullets
        int clipSize = this->weapon->GetClipSize();

        this->newAmmo = n_min( clipSize, bullets );
        bullets -= this->newAmmo;

        this->newAmmoExtra = 0;
/** ZOMBIE REMOVE
        if( hasQuickReload )
        {
            this->newAmmoExtra = n_min( clipSize, bullets );
            bullets -= this->newAmmoExtra;
        }
*/
        this->removedBullets = inventoryBullets - bullets;
/** ZOMBIE REMOVE
        // put reload animation
        if( hasQuickReload )
        {
            // animations with double magazine
            int oldLength = this->oldPlugName.Length();
            if( ( oldLength > 0 ) && ( this->oldPlugName[ oldLength - 1 ] == '2' ) )
            {
                this->animIndex = this->animator->SetFullFirstReload();
            }
            else
            {
                this->animIndex = this->animator->SetFullSecondReload();
            }
        }
        else
        {
*/
            // animations with single magazine
            this->animIndex = this->animator->SetFullReload();
/** ZOMBIE REMOVE
        }
*/
        this->middleChange = true;

        // put reload sound
        if( soundLayer )
        {
            soundLayer->PlaySound( GP_ACTION_FULLRELOAD, false );
        }
    }
/** ZOMBIE REMOVE
    ncCharacter * character = this->entity->GetComponentSafe<ncCharacter>();
    if( character && addon && weaponScene )
    {
        if( ! fastReload )
        {
            this->plugName = this->weapon->GetSlotName( addon );
        }

        weaponScene->UnPlug( addon->GetId() );
        character->Attach( "tgr_handacc", addon->GetId() );

        ncDictionary * dict = addon->GetComponent<ncDictionary>();
        if( dict )
        {
            int oldLength = this->oldPlugName.Length();
            if( ( oldLength > 0 ) && ( this->oldPlugName[ oldLength - 1 ] == '2' ) )
            {
                dict->SetIntVariable( "bulletsInMagazine1", n_min( 2, this->weapon->GetAmmoExtra() ) );
                dict->SetIntVariable( "bulletsInMagazine2", n_min( 2, this->weapon->GetAmmo() ) );
            }
            else
            {
                dict->SetIntVariable( "bulletsInMagazine1", n_min( 2, this->weapon->GetAmmo() ) );
                dict->SetIntVariable( "bulletsInMagazine2", n_min( 2, this->weapon->GetAmmoExtra() ) );
            }
        }
    }
*/
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
                this->PutMagazine();
                this->animIndex = this->animator->SetEndReload();
                this->state = RS_END;
                break;

            case RS_NEEDCOCK:
                this->PutMagazine();
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
                    this->PutMagazineBullets();
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

//------------------------------------------------------------------------------
/**
*/
void
nGPReloadWeapon::PutMagazine()
{
/** ZOMBIE REMOVE
    nEntityObject * addon = 0;
    ncCharacter * character = this->entity->GetComponentSafe<ncCharacter>();
    ncScene * weaponScene = this->weapon->GetComponentSafe<ncScene>();

    addon = this->weapon->GetAddon( ncGPWeaponCharsClass::SLOT_MAG_CLAMP );
    if( ! addon )
    {
        addon = this->weapon->GetAddon( ncGPWeaponCharsClass::SLOT_MAG_HICAP );
    }

    if( character && addon && weaponScene )
    {
        character->Dettach( addon->GetId() );
        weaponScene->Plug( this->plugName.Get(), addon->GetId() );

        ncDictionary * dict = addon->GetComponent<ncDictionary>();
        if( dict )
        {
            if( this->plugName[ this->plugName.Length() - 1 ] == '2' )
            {
                dict->SetIntVariable( "bulletsInMagazine1", n_min( 2, this->newAmmoExtra ) );
                dict->SetIntVariable( "bulletsInMagazine2", n_min( 2, this->newAmmo ) );
            }
            else
            {
                dict->SetIntVariable( "bulletsInMagazine1", n_min( 2, this->newAmmo ) );
                dict->SetIntVariable( "bulletsInMagazine2", n_min( 2, this->newAmmoExtra ) );
            }
        }
    }
*/
}

//------------------------------------------------------------------------------
/**
*/
void
nGPReloadWeapon::RestoreMagazine()
{
/** ZOMBIE REMOVE
    // the restore is made in the destructor and its possible that
    // several data will be not available. so get and check it again
    nEntityObject * addon = 0;
    ncCharacter * character = this->entity->GetComponent<ncCharacter>();
    ncGPWeapon * weapon = 0;
    ncScene * weaponScene = 0;
    if( this->gameplay->GetCurrentWeapon() )
    {
        nEntityObject * weaponEntity = this->gameplay->GetCurrentWeapon();
        if( weaponEntity )
        {
            weaponScene = weaponEntity->GetComponent<ncScene>();
            weapon = weaponEntity->GetComponent<ncGPWeapon>();
        }

        if( weapon )
        {
            addon = weapon->GetAddon( ncGPWeaponCharsClass::SLOT_MAG_CLAMP );
            if( ! addon )
            {
                addon = weapon->GetAddon( ncGPWeaponCharsClass::SLOT_MAG_HICAP );
            }
        }
    }

    if( character && addon && weaponScene )
    {
        character->Dettach( addon->GetId() );
        weaponScene->Plug( this->oldPlugName.Get(), addon->GetId() );

        ncDictionary * dict = addon->GetComponent<ncDictionary>();
        if( dict )
        {
            if( this->oldPlugName[ this->oldPlugName.Length() - 1 ] == '2' )
            {
                dict->SetIntVariable( "bulletsInMagazine1", n_min( 2, this->weapon->GetAmmoExtra() ) );
                dict->SetIntVariable( "bulletsInMagazine2", n_min( 2, this->weapon->GetAmmo() ) );
            }
            else
            {
                dict->SetIntVariable( "bulletsInMagazine1", n_min( 2, this->weapon->GetAmmo() ) );
                dict->SetIntVariable( "bulletsInMagazine2", n_min( 2, this->weapon->GetAmmoExtra() ) );
            }
        }
    }
*/
}

//------------------------------------------------------------------------------
/**
*/
void
nGPReloadWeapon::PutMagazineBullets()
{
/** ZOMBIE REMOVE
    nEntityObject * addon = 0;

    addon = this->weapon->GetAddon( ncGPWeaponCharsClass::SLOT_MAG_CLAMP );
    if( ! addon )
    {
        addon = this->weapon->GetAddon( ncGPWeaponCharsClass::SLOT_MAG_HICAP );
    }

    if( addon )
    {
        ncDictionary * dict = addon->GetComponent<ncDictionary>();
        if( dict )
        {
            if( this->plugName[ this->plugName.Length() - 1 ] == '2' )
            {
                dict->SetIntVariable( "bulletsInMagazine1", n_min( 2, this->newAmmoExtra ) );
                dict->SetIntVariable( "bulletsInMagazine2", n_min( 2, this->newAmmo ) );
            }
            else
            {
                dict->SetIntVariable( "bulletsInMagazine1", n_min( 2, this->newAmmo ) );
                dict->SetIntVariable( "bulletsInMagazine2", n_min( 2, this->newAmmoExtra ) );
            }
        }
    }
*/
}