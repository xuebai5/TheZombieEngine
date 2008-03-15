#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpshoot.cc
//------------------------------------------------------------------------------

#include "ngpshoot/ngpshoot.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "ncgameplayplayer/ncgameplayplayer.h"
#include "nclogicanimator/nclogicanimator.h"
#include "rnsgameplay/ngameplayutils.h"
#include "rnsgameplay/ncgpweaponcharsclass.h"
#include "rnsgameplay/ndamagemanager.h"

#include "nphysics/nphysicsserver.h"
#include "nphysics/nphygeomray.h"
#include "nphysics/nphyrigidbody.h"
#include "nphysics/ncphysicsobj.h"

#include "util/nrandomlogic.h"

#include "gameplay/ngamematerial.h"
#include "gameplay/nfxeventtrigger.h"
#include "ntrigger/ncareaevent.h"

#include "ncsoundlayer/ncsoundlayer.h"

#include "napplication/napplication.h"
#include "nworldinterface/nworldinterface.h"
#include "ntrigger/ntriggerserver.h"
#include "ncaimovengine/ncaimovengine.h"

#include "kernel/nlogclass.h"

#include "ncagentmemory/ncagentmemory.h"

#ifndef NGAME
#include "ndebug/ndebugserver.h"
#include "gameplay/ngamemessagewindowproxy.h"
#include "ndebug/ndebuggraphicsserver.h"
#endif//NGAME

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPShoot, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPShoot)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPShoot::nGPShoot() : 
    weapon( 0 ),
    gameplay( 0 ),
    animator( 0 ),
    targetAnimator ( 0 ),
    initTime( 0 ),
    totalTime( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPShoot::~nGPShoot()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPShoot::Init (nEntityObject* entity)
{
    bool valid = true;

    this->entity = entity;

    if ( valid )
    {
        this->gameplay = entity->GetComponent<ncGameplayLiving>();
        this->animator = entity->GetComponent<ncLogicAnimator>();
        valid = (this->animator != 0) && (this->gameplay != 0);
        n_assert( valid );
    }

    if ( valid )
    {
        // get weapon
        if (this->gameplay->GetCurrentWeapon())
        {
            this->weapon = this->gameplay->GetCurrentWeapon()->GetComponent<ncGPWeapon>();
            n_assert( this->weapon );
            if( this->weapon )
            {
                if( this->weapon->GetAmmo() > 0 )
                {
                    vector3 shotDirection;
                    vector3 shotPosition;

                    this->CalculateDirection( shotPosition, shotDirection);

                    this->ShootFrom( shotPosition, shotDirection );

                    // update accuracy of weapon
                    this->weapon->ApplyAccuracyLoss();

                    // apply recoil deviation
                    ncGameplayPlayer * player = this->entity->GetComponent<ncGameplayPlayer>();
                    if( player )
                    {
                        player->ApplyRecoilDeviation();

                        if ( player->IsSprinting() )
                        {
                            // Stop sprinting
                            player->MovementFlag( ncGameplayPlayer::MF_BLOCKSPRINT, true );
                        }
                    }

                    // spend a bullet
                    this->weapon->SetAmmo( this->weapon->GetAmmo() - 1 );

                    if( player && player->IsFirstShot() )
                    {
                        this->animator->SetShoot();
                    }
                    else
                    {
                        // put shot animation
                        switch( this->weapon->GetFireMode() )
                        {
                        case ncGPWeaponClass::FM_AUTO:
                            this->animator->SetShootAuto();
                            break;
                        case ncGPWeaponClass::FM_SEMI:
                            this->animator->SetShoot();
                            break;
                        }
                    }

                    // put shot sound
                    ncSoundLayer * soundLayer = this->weapon->GetComponentSafe<ncSoundLayer>();
                    if( soundLayer && ! soundLayer->IsPlaying() )
                    {
                        if ( this->weapon->GetFireMode() == ncGPWeaponClass::FM_SEMI )
                        {
                            soundLayer->PlayAlone( "shoot", 1 );
                        }
                    }

                    bool isPlayer = false;
                    if ( player )
                    {
                        isPlayer = true;
                    }

                    //launch special fx for shoot
                    if( ! ( player->IsFirstCameraMode() && player->IsInScope() ) )
                    {
                        nFxEventTrigger::Instance()->Shoot( 
                            player->GetEntityObject(), this->weapon->GetEntityObject());
                    }

                    this->totalTime = this->weapon->GetRateOfFire();
                }
                else
                {
                    // @todo put no bullet animation
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
        this->initTime = nApplication::Instance()->GetTime();
        ncGameplayPlayer* player = entity->GetComponentSafe<ncGameplayPlayer>();
        if ( player && this->gameplay->IsProne() )
        {
            player->MovementFlag ( ncGameplayPlayer::MF_UPDATEPOSITION, false );
        }
        this->gameplay->SetBusy( ncGameplayLiving::BL_NORMAL );
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
    Run
*/
bool
nGPShoot::Run()
{
    bool done = this->IsDone();

    if( done )
    {
        ncGameplayPlayer* player = entity->GetComponentSafe<ncGameplayPlayer>();
        if ( player )
        {
            player->AddShotTimer( this->totalTime * 1.5f );

            if( this->gameplay->IsProne() )
            {
                player->MovementFlag ( ncGameplayPlayer::MF_UPDATEPOSITION, true );
            }
        }
        this->gameplay->SetBusy( ncGameplayLiving::BL_NONE );
    }

    return done;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPShoot::IsDone() const
{
    nTime runTime = nApplication::Instance()->GetTime() - this->initTime;
    return runTime >= this->totalTime;
}

//------------------------------------------------------------------------------
/**
*/
void
nGPShoot::CalculateDirection( vector3 & position, vector3 & direction )
{
    // get the direction and position
    this->gameplay->GetShotPosition( position );
    this->gameplay->GetShotDirection( direction );

    vector3 normalDirection = direction.findortho();
    normalDirection.norm();

    normalDirection = normalDirection * ncGPWeaponCharsClass::MAX_DEVIATION;

    vector3 weaponDeviation = normalDirection*( 1.0f - this->weapon->GetAccuracy() );

#ifndef NGAME
    nDebugTrail * shotLine;
    shotLine = nDebugGraphicsServer::Instance()->NewDebugTrail( );
    shotLine->SetLife( 20.0f );
    shotLine->InsertPoint( position + direction * ncGPWeaponCharsClass::DEVIATION_DISTANCE);
    shotLine->InsertPoint( position + direction * ncGPWeaponCharsClass::DEVIATION_DISTANCE + normalDirection );
    shotLine->InsertPoint( position );
    shotLine->InsertPoint( position + direction * this->weapon->GetRange() );
    nDebugGraphicsServer::Instance()->Kill( shotLine );
    shotLine = nDebugGraphicsServer::Instance()->NewDebugTrail( );
    shotLine->SetLife( 20.0f );
    shotLine->SetColor( vector3( 0,1,0 ) );
    shotLine->InsertPoint( position + direction * ncGPWeaponCharsClass::DEVIATION_DISTANCE);
    shotLine->InsertPoint( position + direction * ncGPWeaponCharsClass::DEVIATION_DISTANCE + weaponDeviation );
    shotLine->InsertPoint( position );
    nDebugGraphicsServer::Instance()->Kill( shotLine );
#endif//NGAME

    // apply accuracy
    vector3 deviation = weaponDeviation * n_rand_real( 1.0f );

    quaternion q;

    float angle = n_rand_real( N_TWOPI );

    q.set_rotate_axis_angle( direction, angle );
    deviation = q.rotate( deviation );
    direction = direction * ncGPWeaponCharsClass::DEVIATION_DISTANCE + deviation;
    direction.norm();
}

//------------------------------------------------------------------------------
/**
*/
void
nGPShoot::ShootFrom( const vector3 & position, const vector3 & direction )
{
    vector3 impactPosition, impactNormal;
    geomid shotObj;
    ncPhysicsObj * physicObj;
    nGameMaterial * gmaterial;

#ifndef NGAME
    nDebugTrail * shotLine;
    shotLine = nDebugGraphicsServer::Instance()->NewDebugTrail( );
    shotLine->SetLife( 20.0f );
    shotLine->SetColor( vector3( 0.7f, 0.7f, 0 ) );
    shotLine->InsertPoint( position + direction*this->weapon->GetRange() );
    shotLine->InsertPoint( position );
    nDebugGraphicsServer::Instance()->Kill( shotLine );
    nDebugCross * shotPoint = nDebugGraphicsServer::Instance()->NewDebugCross( );
    shotPoint->SetLife( 70.0f );
    shotPoint->SetColor( vector3( 1,1,0 ) );
    shotPoint->SetSize( 0.001f );
    shotPoint->SetCenter( position + direction*0.5f );
    nDebugGraphicsServer::Instance()->Kill( shotPoint );
#endif//NGAME

    // disable the shooter
    ncPhysicsObj * shooterPhysics = this->gameplay->GetComponent<ncPhysicsObj>();
    bool enablePhysic = false;
    if( shooterPhysics )
    {
        enablePhysic = shooterPhysics->IsEnabled();
        shooterPhysics->Disable();
    }

    bool found = nGameplayUtils::ShotRay( 
        position, direction, this->weapon->GetRange(), impactPosition, &shotObj, &physicObj, impactNormal, &gmaterial );

    // enable the shooter
    if( shooterPhysics && enablePhysic )
    {
        shooterPhysics->Enable();
    }

    if( found )
    {
#ifndef NGAME
        nDebugCross * cross = nDebugGraphicsServer::Instance()->NewDebugCross( );
        cross->SetCenter( impactPosition );
        cross->SetLife( 20.0f );
        nDebugGraphicsServer::Instance()->Kill( cross );

        nString debugString;
#endif//NGAME

        bool damageApplied = false;
        ncGameplayLiving * living = 0;
        // apply shot
        if( physicObj )
        {
#ifndef NGAME
            debugString = physicObj->GetEntityObject()->GetClass()->GetName();
            ncGameplayClass * gameplayclass = physicObj->GetClassComponent<ncGameplayClass>();
            if( gameplayclass )
            {
                debugString.Append( " : " );
                debugString.Append( gameplayclass->GetBeautyName() );
            }
#endif//NGAME
            living = physicObj->GetEntityObject()->GetComponent<ncGameplayLiving>();
            if( living )
            {
                float damage = 0.0f;
                n_assert( nDamageManager::Instance() );
                if( nDamageManager::Instance() )
                {
                    damage = nDamageManager::Instance()->CalculateDamage(
                        this->weapon, impactPosition, shotObj, physicObj->GetEntityObject());
                }

                living->ApplyDamage( static_cast<int>( damage ) );
                damageApplied = true;
#ifndef NGAME
                debugString.Append( " : " );
                debugString.AppendFloat( damage );
#endif//NGAME
            }
        }

        nPhysicsGeom * geom = static_cast<nPhysicsGeom*>( phyRetrieveDataGeom( shotObj ) );

        if (living && !living->IsDead())
        {
            living->SetImpacted(true);
            // HACK - force the target to see the player
            ncAgentMemory *targetMemory = living->GetComponent<ncAgentMemory>();
            if (targetMemory)
            {
                ncAgentMemory::EventsIterator memIterator = targetMemory->GetEventsIterator();
                bool eventFound(false);
                const nGameEvent *currEvent;
                while ( !memIterator.IsEnd() )
                {
                    currEvent = nTriggerServer::Instance()->GetEvent(memIterator.Get());
                    if ( currEvent->GetType() == nGameEvent::PLAYER )
                    {
                        eventFound = true;
                        break;
                    }

                    memIterator.Next();
                }

                if ( !eventFound )
                {
                    nGameEvent event;
                    event.SetType( nGameEvent::PLAYER );
                    static int duration = 5000;
                    event.SetDuration( duration );      
                    nEntityObject* fakeEntity = nTriggerServer::Instance()->PlaceAreaEvent( position, event );
                    ncAreaEvent *areaEvent = fakeEntity->GetComponentSafe<ncAreaEvent>();
                    nGameEvent::Id eventId = areaEvent->GetEventByType(nGameEvent::PLAYER);
                    nGameEvent *finalEvent = nTriggerServer::Instance()->GetEvent(eventId);
                    finalEvent->SetSourceEntity( this->entity->GetId() );
                    targetMemory->AddEvent(finalEvent);
                }
            }
        }
        else
        {
            // if the target is now dead, apply a force to it to move the ragdoll
            if( shotObj != NoValidID && ! damageApplied )
            {
                if( geom )
                {
                    nPhyRigidBody * body = geom->GetBody();
                    if( body )
                    {
                        body->AddForceAtPosition( direction * float(.01) * this->weapon->GetDamage(), impactPosition );
                    }
                }
            }
        }

        
        if ( geom )
        {
            n_assert( this->weapon );
            NLOGCOND(defaultLog, !gmaterial, 
                (NLOGUSER | 0, "ShootFrom: Material not found for bullet impact sound for geometry of entity object 0x%x class %s",
                    geom->GetOwner() ? geom->GetOwner()->GetEntityObject()->GetId() : 0,
                    geom->GetOwner() ? geom->GetOwner()->GetEntityObject()->GetClass()->GetName() : "not exists" ));
            if ( gmaterial )
            {
                // Launch special fx
                nLaunchSpecialFxData effect("Impact_M4", impactPosition, gmaterial, physicObj->GetEntityObject(), 1.f , -impactNormal );
                nFxEventTrigger::Instance()->Launch( effect );
                
                // Play bullet impact sound
                const nString & materialName = gmaterial->GetGameMaterialName();
                ncSoundLayer * soundLayer = this->weapon->GetComponentSafe<ncSoundLayer>();
                n_assert( soundLayer );
                if ( soundLayer )
                {
                    soundLayer->PlayAloneMaterialAtPosition( "bulletimpact", materialName.Get(), 1, impactPosition );
                }
            }
        }

        // launch gameplay sound event at impact position
        nGameEvent event;
        event.SetType( nGameEvent::IMPACT_SOUND );
        event.SetDuration( 10 );      
        nEntityObject* fakeEntity = nTriggerServer::Instance()->PlaceAreaEvent( impactPosition, event );
        event.SetSourceEntity( fakeEntity->GetId() );


#ifndef NGAME
        bool debugTexts = nDebugServer::Instance()->GetFlagEnabled( "rnsview", "debugtexts" );
        if( debugTexts )
        {
            if( nGameMessageWindowProxy::Instance() )
            {
                if( debugString.Length() > 0 )
                {
                    nGameMessageWindowProxy::Instance()->SendGameMessage(this->entity, debugString.Get(), 2, false );
                }
                else
                {
                    nGameMessageWindowProxy::Instance()->SendGameMessage(this->entity, "<<nothing>>", 2, false );
                }
            }
        }
#endif//NGAME
    }
}
