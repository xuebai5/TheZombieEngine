//------------------------------------------------------------------------------
//  ngphideweapon.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngphideweapon/ngphideweapon.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncsoundlayer/ncsoundlayer.h"
#include "rnsgameplay/ngameplayenums.h"

#include "ncgameplayliving/ncgameplayliving.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPHideWeapon, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPHideWeapon)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*,bool), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPHideWeapon::nGPHideWeapon():
    animator( 0 ),
    animIndex( -1 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPHideWeapon::~nGPHideWeapon()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    @param entity entity that hide the weapon
    @param hide true if weapon is hide, false if weapon is show
    @returns true if the action can be done
*/
bool
nGPHideWeapon::Init( nEntityObject* entity, bool hide )
{
    bool valid = true;

    ncGameplayLiving * gameplay = entity->GetComponent<ncGameplayLiving>();
    n_assert( gameplay );

    this->entity = entity;
    this->animator = entity->GetComponent <ncLogicAnimator>();

    this->soundLayer = entity->GetComponent <ncSoundLayer>();

    valid = this->animator != 0 && this->soundLayer != 0;

    if ( valid )
    {
        if ( hide )
        {
            this->animIndex = this->animator->SetHideWeapon();

            this->soundLayer->PlaySound( GP_ACTION_HIDEWEAPON );
        }
        else
        {
            this->animIndex = this->animator->SetShowWeapon();

            if ( gameplay->IsCrouching() )
            {
                soundLayer->PlaySound( GP_ACTION_SHOWWEAPONCROUCH, false );
            }
            else if ( gameplay->IsProne() )
            {
                soundLayer->PlaySound( GP_ACTION_SHOWWEAPONPRONE, false );
            }
            else
            {
                soundLayer->PlaySound( GP_ACTION_SHOWWEAPON, false );
            }
        }
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    @returns true if action is completed
*/
bool
nGPHideWeapon::Run()
{
    return this->IsDone();
}

//------------------------------------------------------------------------------
/**
    @returns true if action is completed
*/
bool
nGPHideWeapon::IsDone() const
{
    return this->animator->HasFinished( this->animIndex );
}

//------------------------------------------------------------------------------
