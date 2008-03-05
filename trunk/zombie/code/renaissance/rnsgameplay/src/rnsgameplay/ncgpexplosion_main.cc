//------------------------------------------------------------------------------
//  ncgpexplosion_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "rnsgameplay/ncgpexplosion.h"
#include "rnsgameplay/ncgpexplosionclass.h"
#include "nphysics/nphysicsworld.h"
#include "nphysics/ncphysicsobj.h"

#include "nspecialfx/ncspecialfxclass.h"
#include "gameplay/nfxeventtrigger.h"

#include "nspatial/nspatialserver.h"
#include "zombieentity/nctransform.h"

#include "rnsgameplay/ndamagemanager.h"

#include "ncgameplayliving/ncgameplayliving.h"
#include "rnsgameplay/ngameplayutils.h"
#include "ntrigger/ntriggerserver.h"

#include "ncsoundlayer/ncsoundlayer.h"

#ifndef NGAME
#include "ndebug/ndebuggraphicsserver.h"
#endif//!NGAME

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGPExplosion,nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPExplosion)
NSCRIPT_INITCMDS_END()


const char* GRENADE_EXPLOSION_FX_EVENT_NAME = "Explosion_Grenade_group0";

//------------------------------------------------------------------------------
/**
*/
void
ncGPExplosion::Explode( const vector3 & direction )
{
    nEntityObject * entity = this->GetEntityObject();
    n_assert( entity );
    if( ! ( entity ) )
    {
        return;
    }

    ncGPExplosionClass * explosionClass = entity->GetClassComponent<ncGPExplosionClass>();
    if( explosionClass )
    {
        // get explosion data
        vector3 explosionPosition;
        ncPhysicsObj * object = entity->GetComponentSafe<ncPhysicsObj>();
        if( object )
        {
            object->GetPosition( explosionPosition );
        }

        float explosionRadius = explosionClass->GetExplosionRadius();
        float explosionPower = explosionClass->GetExplosionPower();
        float explosionDamage = explosionClass->GetExplosionDamage();

#ifndef NGAME
        // debug sphere
        nDebugSphere * area = nDebugGraphicsServer::Instance()->NewDebugSphere();
        n_assert( area );
        if( area )
        {
            area->SetCenter( explosionPosition );
            area->SetRadius( explosionRadius );
            area->SetLife( 1.0f );
            area->SetDead( true );
        }
#endif//!NGAME

        // launch physic effect
        nRef<nPhyBoom> boom = static_cast<nPhyBoom*>( nObject::kernelServer->New( "nphyboom" ) );
        n_assert( boom );
        if( boom )
        {
            boom->SetType( nPhyBoom::explosion );
            boom->SetLength( phyreal( explosionRadius ) );
            boom->SetForce( phyreal( explosionPower ) );

            if( object )
            {
                boom->SetWorld( object->GetWorld() );
            }

            boom->GoBoomAt( explosionPosition );
            boom->Release();
        }


        // launch gfx effect
        nLaunchSpecialFxData effect;
        this->CreateEffect( direction, explosionPosition, effect );
        nFxEventTrigger::Instance()->Launch( effect );
        
        // play sound
        ncSoundLayer * soundLayer = entity->GetComponent<ncSoundLayer>();
        if ( soundLayer )
        {
            soundLayer->PlayAloneAtPosition( "explosion", 1, explosionPosition );
        }

        // launch gameplay event at the explosionn position
        nGameEvent event;
        event.SetType( nGameEvent::EXPLOSION_SOUND );
        event.SetDuration( 4 );      
        nEntityObject* fakeEntity = nTriggerServer::Instance()->PlaceAreaEvent( explosionPosition, event );
        if( fakeEntity )
        {
            event.SetSourceEntity( fakeEntity->GetId() );
        }

        // get entities affected
        sphere sph( explosionPosition, explosionRadius );

        nArray<nEntityObject*> entities;
        nSpatialServer::Instance()->GetEntitiesCategory( sph, nSpatialTypes::CAT_AGENTS,
            nSpatialTypes::SPF_ALL_INDOORS | nSpatialTypes::SPF_OUTDOORS | 
            nSpatialTypes::SPF_USE_POSITION | nSpatialTypes::SPF_INTERSECTING, entities );

        // apply damage
        bool restoreObject = false;
        if( object )
        {
            restoreObject = object->IsEnabled();
            object->Disable();
        }

        nDamageManager * damageManager = nDamageManager::Instance();
        n_assert( nDamageManager::Instance() );

        ncGameplayLiving * living = 0;
        
        for( int i = 0 ; i < entities.Size() ; ++i )
        {
            if( entities[ i ] )
            {
                living = entities[ i ]->GetComponent<ncGameplayLiving>();
                if( living )
                {
                    float damage = 0.0f;
                    if( damageManager )
                    {
                        damage = damageManager->CalculateDamage( explosionDamage, 
                            explosionPosition, explosionRadius, entities[ i ] );
                    }

                    if( damage > 0.0f )
                    {
                        living->ApplyDamage( static_cast<int>( damage ) );
                    }
                }
            }
        }

        if( object && restoreObject )
        {
            object->Enable();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Calculate the effect params, collision point , material id
*/
void
ncGPExplosion::CreateEffect( const vector3 & direction, const vector3 & position, nLaunchSpecialFxData& effect )const
{
    vector3 impactPosition;
    geomid shotObj;
    ncPhysicsObj * physicObj;

    effect.impactPoint = position;
    bool found = nGameplayUtils::ShotRay( 
        effect.impactPoint, direction, 0.2f, impactPosition, &shotObj, &physicObj, effect.impactNormal, &effect.gameMat );

    effect.eventType = GRENADE_EXPLOSION_FX_EVENT_NAME;
    effect.scale = 1.f;
    if ( found )
    { 
        effect.entityObject = physicObj ? physicObj->GetEntityObject() : 0;
    } else
    {
        effect.entityObject = 0;
        effect.gameMat = 0;
        effect.impactNormal = vector3( 0.f , 1.f , 0.f); //Vertical
    }
}

//------------------------------------------------------------------------------
