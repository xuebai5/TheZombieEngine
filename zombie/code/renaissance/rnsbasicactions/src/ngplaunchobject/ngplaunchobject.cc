//------------------------------------------------------------------------------
//  ngplaunchobject.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsbasicactions.h"

#include "ngplaunchobject/ngplaunchobject.h"
#include "rnsgameplay/ncgpexplosion.h"
#include "rnsgameplay/ncgpthrowable.h"
#include "rnsgameplay/nrnsentitymanager.h"

#include "nscene/ncscene.h"

#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayplayer/ncgameplayplayer.h"

#include "nclogicanimator/nclogicanimator.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPLaunchObject, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPLaunchObject)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, const char*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPLaunchObject::nGPLaunchObject():gameplay(0),animator(0),animIndex(-1),isHiding(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGPLaunchObject::~nGPLaunchObject()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    @param entity entity that launch the object
    @param objectName name of class for the object
    @returns true if the action can be done
*/
bool
nGPLaunchObject::Init( nEntityObject * entity, const char * objectName )
{
    bool valid = true;
    
    this->entity = entity;

    valid = objectName != 0;

    if( valid )
    {
        this->gameplay = entity->GetComponent<ncGameplayLiving>();
        this->animator = entity->GetComponent<ncLogicAnimator>();

        valid = ( this->gameplay != 0 ) && ( this->animator != 0 );
    }

    if( valid )
    {
        // create the object to launch
        nEntityObject * object = nRnsEntityManager::Instance()->CreateLocalEntity( objectName );

        if( object )
        {
            vector3 direction;
            vector3 position;

            this->gameplay->GetShotDirection( direction );
            this->gameplay->GetShotPosition( position );

            nEntityObject * weapon = this->gameplay->GetCurrentWeapon();
            if( weapon )
            {
                ncScene * weaponScene = weapon->GetComponentSafe<ncScene>();
                if( weaponScene )
                {
                    quaternion rotation;
                    weaponScene->GetWorldPlugData( "underbarrel", position, rotation );
                }
            }

            // launch 1/2 meters from origin
            position = position + direction * 0.5f;

            ncGPThrowable* throwObject = object->GetComponent<ncGPThrowable>();
            valid = throwObject != 0;
            n_assert( valid );

            if ( valid )
            {
                throwObject->SetInitial( direction, position );
                throwObject->Create();
            }
        }
    }

    if( valid )
    {
        ncGameplayPlayer * player = this->entity->GetComponent<ncGameplayPlayer>();
        if( player && player->IsSprinting() )
        {
            player->MovementFlag( ncGameplayPlayer::MF_BLOCKSPRINT, true );
        }
        
        this->gameplay->SetBusy( ncGameplayLiving::BL_NORMAL );
    }

    if( valid )
    {
        this->animIndex = this->animator->SetHideWeapon();
        this->isHiding = true;
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPLaunchObject::Run()
{
    bool done = false;

    if( -1 == this->animIndex )
    {
        done = true;
    }

    if( !done && this->animator->HasFinished( this->animIndex ) )
    {
        if( this->isHiding )
        {
            this->animIndex = this->animator->SetShowWeapon();
            this->isHiding = false;
        }
        else
        {
            done = true;
        }
    }

    if( done )
    {
        this->gameplay->SetBusy( ncGameplayLiving::BL_NONE );
    }

    return done;
}

//------------------------------------------------------------------------------
/**
    @returns true
*/
bool
nGPLaunchObject::IsDone() const
{
    return false;
}

//------------------------------------------------------------------------------
