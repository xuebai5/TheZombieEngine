//------------------------------------------------------------------------------
//  ngpswitchfiremode.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngpswitchfiremode/ngpswitchfiremode.h"

#include "rnsgameplay/ncgpweaponclass.h"

#include "ncgameplayliving/ncgameplayliving.h"

#include "nclogicanimator/nclogicanimator.h"

#include "ncsoundlayer/ncsoundlayer.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPSwitchFireMode, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPSwitchFireMode)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPSwitchFireMode::nGPSwitchFireMode():
    animator( 0 ),
    gameplay( 0 ),
    weapon( 0 ),
    animIndex( -1 ),
    newMode( ncGPWeaponClass::FM_AUTO )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPSwitchFireMode::~nGPSwitchFireMode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param entity entity that switch mode
    @returns true if basic action is valid
*/
bool
nGPSwitchFireMode::Init( nEntityObject* entity )
{
    bool valid = true;

    this->entity = entity;

    // get components
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
            }
        }

        valid = this->weapon != 0;
    }

    if( valid )
    {
        switch( this->weapon->GetFireMode() )
        {
        case ncGPWeaponClass::FM_AUTO:
            this->newMode = ncGPWeaponClass::FM_SEMI;
            break;
        case ncGPWeaponClass::FM_SEMI:
            this->newMode = ncGPWeaponClass::FM_AUTO;
            break;
        }
        this->animIndex = this->animator->SetSwitchFireMode();

        this->gameplay->SetBusy( ncGameplayLiving::BL_NORMAL );

        ncSoundLayer * soundLayer = this->weapon->GetComponentSafe<ncSoundLayer>();
        if( soundLayer )
        {
            soundLayer->PlaySound( GP_ACTION_SWITCH, false );
        }
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPSwitchFireMode::Run()
{
    bool done = this->IsDone();

    if( done )
    {
        this->weapon->SetFireMode( this->newMode );
        this->gameplay->SetBusy( ncGameplayLiving::BL_NONE );
    }

    return done;
}

//------------------------------------------------------------------------------
/**
    @returns true when basic action is done
*/
bool
nGPSwitchFireMode::IsDone()const
{
    return this->animator->HasFinished( this->animIndex );
}

//------------------------------------------------------------------------------
