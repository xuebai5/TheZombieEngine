//------------------------------------------------------------------------------
//  nrnsentitymanager.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"

#include "ncgameplayplayer/ncgameplayplayer.h"
#include "rnsgameplay/nrnsentitymanager.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "util/nstream.h"

#include "entity/nentityobjectserver.h"

#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "nworldinterface/nworldinterface.h"

#include "util/nrandomlogic.h"

#include "nspatial/ncspatial.h"
#include "nspatial/nspatialserver.h"

#include "nsoundscheduler/nsoundscheduler.h"

#include "nphysics/ncphysicsobj.h"

#include "rnsgameplay/ndamagemanager.h"
#include "rnsgameplay/ninventorymanager.h"
#include "rnsgameplay/ninventoryitem.h"

#include "ncgameplayvehicle/ncgameplayvehicle.h"

#include "npathfinder/npathfinder.h"
#include "nnavmesh/nnavmesh.h"
#include "ncnavmesh/ncnavmesh.h"

#include "ncfsm/ncfsm.h"

#include "ntrigger/ntriggerserver.h"
#include "ntrigger/nctrigger.h"
#include "ntrigger/ncareaevent.h"

#include "napplication/napplication.h"

#include "zombieentity/ncdictionaryclass.h"

#include "animcomp/nccharacter.h"

#include "nclogicanimator/nclogicanimator.h"
#include "ncaimovengine/ncaimovengine.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#endif

#define TIME_TO_DEATH 4.0f
#define TIME_TO_BURY 2.0f
#define BURY_DEEP_INCREMMENT 0.02f

//------------------------------------------------------------------------------
nNebulaScriptClass(nRnsEntityManager, "nroot");

//------------------------------------------------------------------------------
const char * nRnsEntityManager::weaponTable[nRnsEntityManager::WEAPON_TABLE_SIZE] = {
    "M4"
};

const char * nRnsEntityManager::playerName = "Localplayer";

nRnsEntityManager * nRnsEntityManager::singleton = 0;

//------------------------------------------------------------------------------
NSIGNAL_DEFINE( nRnsEntityManager, EntityEliminated );
NSIGNAL_DEFINE( nRnsEntityManager, MountingAccessory);
NSIGNAL_DEFINE( nRnsEntityManager, CancelAccessory);
NSIGNAL_DEFINE( nRnsEntityManager, WeaponChanged );

//------------------------------------------------------------------------------
/**
*/
nRnsEntityManager::nRnsEntityManager():
    damageManager( 0 ),
    inventoryManager( 0 ),
    pathFinder( 0 ),
    localPlayer( 0 ),
    lastTime( -1 ),
    hasStarted( false )
#ifdef __NEBULA_STATS__
    , profGPupdate("profGPupdate", true)
    , profGPtrigger("profGPtrigger", true)
#endif
{
    n_assert2( ! nRnsEntityManager::singleton, "Rns Entity Manager already created" );
    nRnsEntityManager::singleton = this;

    nNetworkManager * network = nNetworkManager::Instance();

    if( network )
    {
        network->BindSignal( 
            nNetworkManager::SignalOnNewEntity, this, &nRnsEntityManager::NewNetworkEntity, 0 );
        network->BindSignal(
            nNetworkManager::SignalOnDeleteEntity, this, &nRnsEntityManager::DeleteEntity, 0 );
        network->BindSignal(
            nNetworkManager::SignalOnNewClient, this, &nRnsEntityManager::NewNetworkClient, 0 );
    }

    // bind to signal of deleted entity
    nEntityObjectServer::Instance()->BindSignal( 
        nEntityObjectServer::SignalEntityDeleted, this, &nRnsEntityManager::EntityDeleted, 0 );

    this->damageManager = static_cast<nDamageManager*>( nKernelServer::Instance()->New("ndamagemanager") );
    this->inventoryManager = static_cast<nInventoryManager*>( nKernelServer::Instance()->New("ninventorymanager") );
}

//------------------------------------------------------------------------------
/**
*/
nRnsEntityManager::~nRnsEntityManager()
{
    n_assert( nRnsEntityManager::singleton == this );
    nRnsEntityManager::singleton = 0;

    this->Stop();

    this->SetLocalPlayer( nEntityObjectServer::IDINVALID );

    nNetworkManager * network = nNetworkManager::Instance();

    if( network )
    {
        network->UnbindTargetObject( nNetworkManager::SignalOnNewEntity.GetId(), this );
        network->UnbindTargetObject( nNetworkManager::SignalOnDeleteEntity.GetId(), this );
        network->UnbindTargetObject( nNetworkManager::SignalOnNewClient.GetId(), this );
    }

    if( this->damageManager )
    {
        this->damageManager->Release();
        this->damageManager = 0;
    }

    if( this->inventoryManager )
    {
        this->inventoryManager->Release();
        this->inventoryManager = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @returns the singleton instance
*/
nRnsEntityManager*
nRnsEntityManager::Instance()
{
    return nRnsEntityManager::singleton;
}

//------------------------------------------------------------------------------
/**
    @param data the player data
    @returns the created player
*/
nEntityObject *
nRnsEntityManager::CreatePlayer( const nRnsEntityManager::PlayerData & data )
{
    nEntityObject * player = nEntityObjectServer::Instance()->NewServerEntityObject( nRnsEntityManager::playerName );

    if( player )
    {
        this->InitEntityObject( player );
        this->InitPlayer( player, data );
    }

    return player;
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class for the object
    @returns the created object
*/
nEntityObject *
nRnsEntityManager::CreateLocalEntity( const char * classname )
{
    nEntityObject * object = nEntityObjectServer::Instance()->NewLocalEntityObject( classname );
    n_assert( object );
    if( object )
    {
        this->InitEntityObject( object );
    }

    return object;
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class for the object
    @returns the created object
*/
nEntityObject *
nRnsEntityManager::CreateServerEntity( const char * classname )
{
    nEntityObject * object = nEntityObjectServer::Instance()->NewServerEntityObject( classname );
    n_assert( object );
    if( object )
    {
        this->InitEntityObject( object );
    }

    return object;
}

//------------------------------------------------------------------------------
/**
    @param object entity object to init
*/
void
nRnsEntityManager::InitEntityObject( nEntityObject * object )
{
    n_assert( object );
    if( object )
    {
        nLevelManager::Instance()->GetCurrentLevel()->AddEntity( object );

        // Add entity to physic space
        ncPhysicsObj* phyComp = object->GetComponent<ncPhysicsObj>();
        if ( phyComp )
        {
            phyComp->AutoInsertInSpace();
        }

        ncGameplay * gameplay = object->GetComponent<ncGameplay>();
        if( gameplay )
        {
            this->gameplayEntities.Append( object );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::OnNewEntity( nEntityObject* entity )
{
    n_assert( entity );
    if( entity )
    {
        if ( entity->GetComponent<ncGameplay>() )
        {
            this->gameplayEntities.Append( entity );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param player a created player to init
    @param data player data
*/
void
nRnsEntityManager::InitPlayer( nEntityObject * player, const nRnsEntityManager::PlayerData & data )
{
    n_assert( player );

    ncGameplayPlayer * gameplayer = player->GetComponentSafe<ncGameplayPlayer>();
    if( gameplayer )
    {
        vector3 position;
        polar2 angles;
        if( data.flags & nRnsEntityManager::PDF_POSITION )
        {
            position = data.position;
        }
        else
        {
            position.y = 15.0f;
            position.x = 1.0f + n_rand_real( 10.0f );
            position.z = 1.0f + n_rand_real( 10.0f );
        }
        if( data.flags & nRnsEntityManager::PDF_ANGLES )
        {
            angles = data.angles;
        }
        else
        {
            angles = polar2( 0.0f, N_PI );
        }

        gameplayer->SetShotPosition( position );
        gameplayer->SetShotAngles( angles );
        gameplayer->UpdatePlayerPosition();
    }

    // Make the player perceptible by sight and touch
    ncAreaEvent* emittedEvents = player->GetComponentSafe<ncAreaEvent>();
    if ( emittedEvents )
    {
        nGameEvent event;
        event.SetType( nGameEvent::PLAYER );
        event.SetSourceEntity( player->GetId() );
        event.SetEmitterEntity( player->GetId() );
        event.SetDuration( 0 );
        nTriggerServer::Instance()->PostAreaEvent( event );
    }
}

//------------------------------------------------------------------------------
/**
    @param id new local player identifier
*/
void
nRnsEntityManager::SetLocalPlayer( int id )
{
    if( id == nEntityObjectServer::IDINVALID )
    {
        this->localPlayer = 0;
    }
    else
    {
        nEntityObject * player = 0;

        // search for the game entity
        for( int i = 0 ; i < this->gameplayEntities.Size() ; ++i )
        {
            if( this->gameplayEntities[ i ]->GetId() == unsigned int( id ) )
            {
                player = this->gameplayEntities[ i ];
            }
        }

        // if founded 
        if( player )
        {
            // check that the entity is a well-formed player
            ncGameplayPlayer * gameplay = player->GetComponent<ncGameplayPlayer>();
            n_assert2( gameplay, "Local Player without Gameplay Player" );
            if( gameplay )
            {
                this->localPlayer = player;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param entity dead entity
*/
void
nRnsEntityManager::EntityDead( ncGameplayLiving * entity )
{
    nEntityObject * deadEntity = entity->GetEntityObject();

    nNetworkManager * network = nNetworkManager::Instance();
    n_assert( network );

    if( network && network->IsServer() )
    {
        nstream data;
        int id;

        id = deadEntity->GetId();

        data.SetWrite( true );
        data.UpdateInt( id );
     
        // kill the player in all the clients
        network->CallRPCAll( nRnsEntityManager::KILL_ENTITY, data.GetBufferSize(), data.GetBuffer() );
    }
}

//------------------------------------------------------------------------------
/**
    @param id entity identifier
*/
void
nRnsEntityManager::KillEntity( int id )
{
    nEntityObject * entity = 0;

    // search for the created entity
    for( int i = 0 ; i < this->gameplayEntities.Size() ; ++i )
    {
        if( this->gameplayEntities[ i ]->GetId() == unsigned int( id ) )
        {
            entity = this->gameplayEntities[ i ];
            break;
        }
    }

    if( entity )
    {
        ncGameplayLiving * living = entity->GetComponent<ncGameplayLiving>();

        if( living && ! living->IsDead() )
        {
            living->SetDead();
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param entity player entity
*/
void
nRnsEntityManager::SuicidePlayer( nEntityObject * entity )
{
    n_assert_if( entity )
    {
        ncGameplayLiving * living = entity->GetComponent<ncGameplayLiving>();

        if( living && ! living->IsDead() )
        {
            living->SetDead();
        }
    }
}

//------------------------------------------------------------------------------
/**
    respond to signal that an entity has been remotely removed.
    @param id entity identifier
*/
void
nRnsEntityManager::DeleteEntity( int id )
{
    this->EntityDeleted( id, true );

    /*
    nEntityObject * entity = 0;

    // search for the delete entity in living gameplay
    for( int i = 0 ; i < this->gameplayEntities.Size() ; ++i )
    {
        if( this->gameplayEntities[ i ]->GetId() == unsigned int( id ) )
        {
            entity = this->gameplayEntities[ i ];
            this->gameplayEntities.EraseQuick( i );
            this->RemoveEntity( entity );
            return;
        }
    }

    // search for the delete entity in dead entities
    for( int i = 0 ; i < this->deadGameplayEntities.Size() ; ++i )
    {
        if( this->deadGameplayEntities[ i ]->entity->GetId() == unsigned int( id ) )
        {
            entity = this->deadGameplayEntities[ i ]->entity;
            n_delete( this->deadGameplayEntities[ i ] );
            this->deadGameplayEntities.EraseQuick( i );
            this->RemoveEntity( entity );
            return;
        }
    }

    // search for the delete entity in buried entities
    for( int i = 0 ; i < this->buriedGameplayEntities.Size() ; ++i )
    {
        if( this->buriedGameplayEntities[ i ]->entity->GetId() == unsigned int( id ) )
        {
            entity = this->buriedGameplayEntities[ i ]->entity;
            n_delete( this->buriedGameplayEntities[ i ] );
            this->buriedGameplayEntities.EraseQuick( i );
            this->RemoveEntity( entity );
            return;
        }
    }
    */
}

//------------------------------------------------------------------------------
/**
    respond to signal that an entity has been locally unloaded
    @param id entity identifier
*/
void
nRnsEntityManager::EntityDeleted( int id )
{
    this->EntityDeleted( id, false );
/*
    nEntityObject * entity = 0;
    bool removeEntity = false;

    // search for the delete entity in living gameplay
    for( int i = 0 ; i < this->gameplayEntities.Size() ; ++i )
    {
        if( this->gameplayEntities[ i ]->GetId() == unsigned int( id ) )
        {
            entity = this->gameplayEntities[ i ];
            this->gameplayEntities.EraseQuick( i );
            removeEntity = true;
            break;
            //this->RemoveEntity( entity );
            //return;
        }
    }

    // search for the delete entity in dead entities
    for( int i = 0 ; i < this->deadGameplayEntities.Size() ; ++i )
    {
        if( this->deadGameplayEntities[ i ]->entity->GetId() == unsigned int( id ) )
        {
            entity = this->deadGameplayEntities[ i ]->entity;
            n_delete( this->deadGameplayEntities[ i ] );
            this->deadGameplayEntities.EraseQuick( i );
            //this->RemoveEntity( entity );
            //return;
            removeEntity = true;
            break;
        }
    }

    // search for the delete entity in buried entities
    for( int i = 0 ; i < this->buriedGameplayEntities.Size() ; ++i )
    {
        if( this->buriedGameplayEntities[ i ]->entity->GetId() == unsigned int( id ) )
        {
            entity = this->buriedGameplayEntities[ i ]->entity;
            n_delete( this->buriedGameplayEntities[ i ] );
            this->buriedGameplayEntities.EraseQuick( i );
            //this->RemoveEntity( entity );
            //return;
            removeEntity = true;
            break;
        }
    }

    // effectivaly remove the entity
    if (removeEntity)
    {
        this->SignalEntityEliminated( this, entity );
        
        ncGameplay * gameplay = entity->GetComponent<ncGameplay>();
        if( gameplay )
        {
            gameplay->AbortCurrentAction();
        }
    }
*/
}

//------------------------------------------------------------------------------
/**
    @param  entityId        id of the entity that has been removed
    @param  removeFromWorld the entity must be 
*/
void
nRnsEntityManager::EntityDeleted( nEntityObjectId id, bool removeFromWorld )
{
    nEntityObject * entity = 0;

    // search for the delete entity in living gameplay
    for( int i = 0 ; i < this->gameplayEntities.Size() ; ++i )
    {
        if( this->gameplayEntities[ i ]->GetId() == unsigned int( id ) )
        {
            entity = this->gameplayEntities[ i ];
            this->gameplayEntities.EraseQuick( i );
            this->RemoveEntity( entity, removeFromWorld );
            return;
        }
    }

    // search for the delete entity in dead entities
    for( int i = 0 ; i < this->deadGameplayEntities.Size() ; ++i )
    {
        if( this->deadGameplayEntities[ i ]->entity->GetId() == unsigned int( id ) )
        {
            entity = this->deadGameplayEntities[ i ]->entity;
            n_delete( this->deadGameplayEntities[ i ] );
            this->deadGameplayEntities.EraseQuick( i );
            this->RemoveEntity( entity, removeFromWorld );
            return;
        }
    }

    // search for the delete entity in buried entities
    for( int i = 0 ; i < this->buriedGameplayEntities.Size() ; ++i )
    {
        if( this->buriedGameplayEntities[ i ]->entity->GetId() == unsigned int( id ) )
        {
            entity = this->buriedGameplayEntities[ i ]->entity;
            n_delete( this->buriedGameplayEntities[ i ] );
            this->buriedGameplayEntities.EraseQuick( i );
            this->RemoveEntity( entity, removeFromWorld );
            return;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param entity entity to remove
*/
void
nRnsEntityManager::RemoveEntity( nEntityObject * entity, bool removeFromWorld )
{
    this->SignalEntityEliminated( this, entity );
    
    ncGameplay * gameplay = entity->GetComponent<ncGameplay>();
    if( gameplay )
    {
        gameplay->AbortCurrentAction();
    }

    if( removeFromWorld )
    {
        nWorldInterface::Instance()->DeleteEntity( entity );
    }

    if( entity == this->localPlayer )
    {
        this->SetLocalPlayer( nEntityObjectServer::IDINVALID );
    }
}

//------------------------------------------------------------------------------
/**
    @param weapon type of the weapon
    @returns the classname of the weapon
*/
const char *
nRnsEntityManager::GetWeaponName( const unsigned char weapon )
{
    bool valid = weapon < N_ARRAY_SIZEOF( nRnsEntityManager::weaponTable );
    n_assert2( valid, "Weapon Id too high" );

    if( ! valid )
    {
        return nRnsEntityManager::weaponTable[ 0 ];
    }

    return nRnsEntityManager::weaponTable[ weapon ];
}

//------------------------------------------------------------------------------
/**
    UpdateDeadEntities

    @brief Leave the ragdoll a time in the ground and finally add the entity
           to the buried list
*/
void
nRnsEntityManager::UpdateDeadEntities( float deltaTime )
{
    DeadEntity *dE = 0;
    for( int i = 0 ; i < this->deadGameplayEntities.Size() ; ++i)
    {
        // delete entity from list
        dE = this->deadGameplayEntities[i];
        if ( dE->time > 0 )
        {
            dE->time -= deltaTime;
        }
        else
        {
            if (dE->timeRagdoll > 0 )
            {
                // activate ragdoll
                dE->timeRagdoll -= deltaTime;
                /*if ( !dE->entity->GetComponentSafe<ncCharacter>()->GetRagdollActive() )
                {
                dE->entity->GetComponentSafe<ncCharacter>()->SetRagdollActive( true );
                }*/       
            }
            else
            {
                // turn ragdoll off
                /*if ( dE->entity->GetComponentSafe<ncCharacter>()->GetRagdollActive() )
                {
                dE->entity->GetComponentSafe<ncCharacter>()->SetRagdollActive( false );
                }   */
                // add entity to buried list
                dE->time = TIME_TO_BURY;
                this->buriedGameplayEntities.Append( this->deadGameplayEntities[i ] );
                this->deadGameplayEntities.Erase( i );            
                --i; // to get the correct next element
            }
        }
        
    }
}
//------------------------------------------------------------------------------
/**
    BuryDeadEntities

    @brief Bury the entity into the ground
*/
void
nRnsEntityManager::BuryDeadEntities( float deltaTime )
{
    nNetworkManager * network = nNetworkManager::Instance();
    nEntityObjectServer * entityServer = nEntityObjectServer::Instance();

    DeadEntity *dE = 0;
    for( int i = 0 ; i < this->buriedGameplayEntities.Size() ; ++i )
    {
        dE = this->buriedGameplayEntities[i];
        if (dE->time > 0)
        {
            // bury the entity in the ground
            dE->time -= deltaTime;
            ncTransform* entTrComp = dE->entity->GetComponentSafe<ncTransform>();
            vector3 position = entTrComp->GetPosition();
            vector3 newPos ( position.x, position.y - BURY_DEEP_INCREMMENT, position.z );
            entTrComp->SetPosition( newPos );            
        }
        else
        {
            // @TODO: Add the efect for bury entities

            bool canDelete = true;

            if( network )
            {
                canDelete = network->IsServer();
                canDelete = canDelete || ( entityServer->GetEntityObjectType( dE->entity->GetId() ) == nEntityObjectServer::Local ); 
            }

            if( canDelete )
            {
                this->invalidEntities.Append( dE->entity );

                n_delete(dE);
                this->buriedGameplayEntities.EraseQuick( i );
                --i; // to get the correct next element
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param deltatime time since last update
*/
void
nRnsEntityManager::UpdateGameplayEntities( const float deltaTime )
{
    ncGameplay * gameplay = 0;
    ncGameplayLiving* living = 0;
    nEntityObject * entity = 0;
    ncAIMovEngine *mover = 0;

    bool removeFromList = false;
    for( nArray< nEntityObject* >::iterator iter = this->gameplayEntities.Begin() ; 
        iter != this->gameplayEntities.End() ; /*empty*/ )
    {
        entity = static_cast< nEntityObject * >( *iter );
        removeFromList = false;

        gameplay = entity->GetComponentSafe<ncGameplay>();
        living = entity->GetComponent<ncGameplayLiving>();
        mover = entity->GetComponent<ncAIMovEngine>();
        n_assert2( gameplay, "Entity Without gameplay" );

        if( gameplay )
        {
            if( gameplay->IsValid() )
            {
#ifdef __NEBULA_STATS__
                this->profGPupdate.StopAccum();
#endif
                gameplay->Run( deltaTime );
#ifdef __NEBULA_STATS__
                this->profGPupdate.StartAccum();
#endif

                // Check for FSM state transitions (only on server)
                if ( nNetworkManager::Instance()->IsServer() )
                {
                    ncFSM* fsm = entity->GetComponent<ncFSM>();
                    if ( fsm )
                    {
#ifdef __NEBULA_STATS__
                        this->profGPupdate.StopAccum();
#endif
                        if ( living && living->IsImpacted() )
                        {
                            living->SetImpacted(false);
                            fsm->OnTransitionEvent( nGameEvent::HIT );
                        }
                        else
                        {
                            fsm->Update();
                        }
#ifdef __NEBULA_STATS__
                        this->profGPupdate.StartAccum();
#endif
                    }
                }
            }

            if( ! gameplay->IsValid() )
            {
                this->invalidEntities.Append( entity );

                removeFromList = true;
            }
            else
            {
                if( living && living->IsDead() )
                {
                    if( !(mover && mover->IsJumping()) )
                    {
                        // Stop the move engine
                        if( mover )
                        {
                            mover->Stop();
                        }

                        // Play dead animation
                        ncLogicAnimator* animator = entity->GetComponentSafe<ncLogicAnimator>();
                        int index = -1;
                        n_assert(animator);
                        if ( animator )
                        {
                            index = animator->SetDie();
                        }

                        // If is a living before die must do some show
                        DeadEntity *dE = n_new(DeadEntity);
                        dE->entity = entity;
                        // Get the die time animation
                        if ( index > -1 )
                        {
                            dE->time = animator->GetStateDuration( index, false );
                        }
                        else
                        {
                            dE->time = 0;
                        }
                        // Time that entity must be with ragdoll before die
                        dE->timeRagdoll = TIME_TO_DEATH;

                        this->deadGameplayEntities.Append(dE);

                        this->EntityDead( living );

                        removeFromList = true;
                    }
                }
            }
        }

        if( removeFromList )
        {
            iter = this->gameplayEntities.EraseQuick( iter );
        }
        else
        {
            ++iter;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::RemoveInvalidEntities()
{
    ncGameplay * gameplay = 0;
    nEntityObject * entity = 0;

    nNetworkManager * network = nNetworkManager::Instance();
    nEntityObjectServer * entityServer = nEntityObjectServer::Instance();

    // update invalid entities
    for( nArray< nEntityObject* >::iterator iter = this->invalidEntities.Begin() ;
        iter != this->invalidEntities.End() ; /*empty*/ )
    {
        entity = static_cast< nEntityObject * >( *iter );
        gameplay = entity->GetComponent<ncGameplay>();

        bool deleteEntity = true;

        if( network )
        {
            nEntityObjectId entityId = gameplay->GetEntityObject()->GetId();

            deleteEntity = entityServer->GetEntityObjectType( entityId ) == nEntityObjectServer::Local;
            deleteEntity = deleteEntity || network->IsServer();
            NLOGCOND( network, !deleteEntity, ( 0, "Network Entity Deleted by the client" ) );
        }

        if( deleteEntity )
        {
            if( entityServer->GetEntityObjectType( entity->GetId() ) == nEntityObjectServer::Normal  && 
                network->IsServer() )
            {
                this->removedEntities.Append( entity->GetId() );
            }

            iter = this->invalidEntities.EraseQuick( iter );

            // delete entity from manager
            this->RemoveEntity( entity, true );
        }
        else
        {
            ++iter;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::Trigger()
{
#ifdef __NEBULA_STATS__
    this->profGPtrigger.Start();
#endif
    // update times
    if( this->lastTime <= 0 )
    {
        this->lastTime = nApplication::Instance()->GetTime();
    }

    nTime time = nApplication::Instance()->GetTime();
    float deltaTime = float( time - this->lastTime );
    this->lastTime = time;
    
    // update the dead entities and remove those whose ticks to death are zero
    this->UpdateDeadEntities( deltaTime );

    // bury delete entities into the ground
    this->BuryDeadEntities( deltaTime );
    
#ifdef __NEBULA_STATS__
    this->profGPtrigger.StopAccum();
    this->profGPupdate.StartAccum();
#endif
    this->UpdateGameplayEntities( deltaTime );
#ifdef __NEBULA_STATS__
    this->profGPupdate.StopAccum();
    this->profGPtrigger.StartAccum();
#endif

    this->RemoveInvalidEntities();
#ifdef __NEBULA_STATS__
    this->profGPtrigger.Stop();
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::Start()
{
    if ( !this->hasStarted )
    {
        this->hasStarted = true;
        this->StartUpLevelDependendSystems();
        this->InitNormalEntities();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::Stop()
{
    if ( this->hasStarted )
    {
        this->hasStarted = false;
        this->CleanEntities();
        this->ShutDownLevelDependendSystems();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::StartUpLevelDependendSystems()
{
    // Listen to new spawned entity signals
    nWorldInterface::Instance()->BindSignal( nWorldInterface::SignalEntityCreated, this, &nRnsEntityManager::OnNewEntity, 0 );

    // Create a new pathfinder instance if not created yet
    if ( !this->pathFinder )
    {
        this->pathFinder = static_cast<nPathFinder*>( nKernelServer::Instance()->New("npathfinder") );
    }

    // Assign the navigation mesh to the pathfinder
    n_assert( this->pathFinder );
    if ( this->pathFinder )
    {
        // Get the navigation mesh for the outdoor
        nEntityObject* outdoor = nSpatialServer::Instance()->GetOutdoorEntity();
        n_assert2( outdoor, "An outdoor is required.");
        nNavMesh* navMesh = NULL;
        if ( outdoor )
        {
            navMesh = outdoor->GetComponentSafe<ncNavMesh>()->GetNavMesh();
            NLOGCOND_REL(resource, !navMesh->IsValid(), (NLOGUSER | 0, "A navigation mesh for the outdoor is required."));

            // Initialize the pathfinder
            this->pathFinder->SetMesh( navMesh );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::InitNormalEntities()
{
    nNetworkManager * network = nNetworkManager::Instance();

    // Register persisted gameplay entities into the entity manager
    nEntityObjectServer* objServer( nEntityObjectServer::Instance() );
    for ( nEntityObject* entity = objServer->GetFirstEntityObject(); entity; entity = objServer->GetNextEntityObject() )
    {
#ifndef NGAME
        // Skip deleted entities
        ncEditor* editor = entity->GetComponent<ncEditor>();
        if ( editor && editor->IsDeleted() )
        {
            continue;
        }
#endif

        if ( entity->GetComponent<ncGameplay>() )
        {
            this->gameplayEntities.Append( entity );

            if( network && network->IsServer() )
            {
                // Make agent entities begin their FSM
                ncFSM* fsm = entity->GetComponent<ncFSM>();
                if ( fsm )
                {
                    fsm->Init();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::CleanEntities()
{
    this->gameplayEntities.Clear();

    // clean entities dead
    for( int i = 0 ; i < this->deadGameplayEntities.Size() ; ++i )
    {
        DeadEntity * cadaver = this->deadGameplayEntities[ i ];
        /*nEntityObject* entity = cadaver->entity;*/
        n_delete( cadaver );
    }

    this->deadGameplayEntities.Clear();

    // clean entities buried
    for( int i = 0 ; i < this->buriedGameplayEntities.Size() ; ++i )
    {
        DeadEntity * cadaver = this->buriedGameplayEntities[ i ];
        /*nEntityObject* entity = cadaver->entity;*/
        n_delete( cadaver );
    }

    this->buriedGameplayEntities.Clear();

    this->invalidEntities.Clear();
    this->removedEntities.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::ShutDownLevelDependendSystems()
{
    // Stop listen to new spawned entity signals
    nWorldInterface::Instance()->UnbindTargetObject( nWorldInterface::SignalEntityCreated.GetId(), this );

    if( this->pathFinder )
    {
        this->pathFinder->Release();
        this->pathFinder = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsEntityManager::ActivateObject( nEntityObject * player )
{
    nNetworkManager * network = nNetworkManager::Instance();
    if( network )
    {
        nObject * object = this->GetNearestObject( player );
        UsableObjectType type = this->GetUsableType( object );

        if( object && ( type != UOT_NOTHING ) )
        {
            nArg args[3];

            switch( type )
            {
            case UOT_DROPPING:
                {
                    nInventoryItem * item = static_cast<nInventoryItem*>( object );
                    args[0].Delete(); args[0].SetO( player );
                    args[1].Delete(); args[1].SetI( item->GetItemID() );
                    network->SendAction( "ngppickupitem", 2, args, true );
                }
                break;
                
            case UOT_USABLE:
                {
                    nEntityObject * entity = static_cast<nEntityObject*>( object );
                    args[0].Delete(); args[0].SetO( player );
                    args[1].Delete(); args[1].SetI( -1 );
                    args[2].Delete(); args[2].SetI( entity->GetId() );
                    network->SendAction( "ngpuseobject", 3, args, true );
                }
                break;

            case UOT_VEHICLE:
                {
                    nEntityObject* vehicle = static_cast<nEntityObject*>( object );

                    // sending basic action to the player to enter car
                    args[0].Delete(); args[0].SetO( player );
                    args[1].Delete(); args[1].SetO( vehicle );
                    network->SendAction( "ngpentervehicle", 2, args, true ); 
                }
                break;

            default:
                n_assert_always();
            }
        }
}
}

//------------------------------------------------------------------------------
/**
    @param user the entity to check nearest objects
    @return nearest object or null
*/
nObject*
nRnsEntityManager::GetNearestObject( nEntityObject * user )
{
    const float MIN_DISTANCE = 10.0f*10.0f;

    bool valid = true;

    nObject * returnedObject = 0;
    float minDistance = MIN_DISTANCE;

    vector3 position;

    ncTransform * transform = user->GetComponent<ncTransform>();
    valid = transform != 0;
    if( valid )
    {
        position = transform->GetPosition();
    }

    // search the droppings
    if( valid )
    {
        nEntityObject * entity = 0;
        nInventoryManager * manager = nInventoryManager::Instance();

        for( int i = 0 ; i < manager->GetNumItems() ; ++i )
        {
            nInventoryItem * item = manager->GetItemAt( i );
            if( item && item->IsDropped() )
            {
                entity = item->GetDropObject();

                if( entity )
                {
                    transform = entity->GetComponent<ncTransform>();
                    if( transform )
                    {
                        float distance = (position - transform->GetPosition()).lensquared();

                        if( distance < minDistance )
                        {
                            returnedObject = item;
                            minDistance = distance;
                        }
                    }
                }
            }
        }
    }

    // search usable objects
    if( valid )
    {
        nEntityObjectServer * objectServer = 0;
        objectServer = nEntityObjectServer::Instance();

        ncDictionaryClass * dictionary = 0;

        for( nEntityObject * entity = objectServer->GetFirstEntityObject() ; 
            entity ;
            entity = objectServer->GetNextEntityObject() )
        {
            dictionary = entity->GetClassComponent<ncDictionaryClass>();

            if( dictionary )
            {
                if( strcmp( dictionary->GetStringVariable( "ItemType" ), "UsableItem" ) == 0 )
                {
                    transform = entity->GetComponent<ncTransform>();
                    if( transform )
                    {
                        float distance = (position - transform->GetPosition()).lensquared();

                        if( distance < minDistance )
                        {
                            returnedObject = entity;
                            minDistance = distance;
                        }
                    }
                }
            }
        }
    }

    // search vehicles
    if( valid )
    {
        // checking for vehicles
        nEntityObject* vehicle( ncGameplayVehicle::GetClossestVehicle( position ) );
        
        if( vehicle )
        {
            transform = vehicle->GetComponent<ncTransform>();
            if( transform )
            {
                float distance = (position - transform->GetPosition()).lensquared();

                if( distance < minDistance )
                {
                    returnedObject = vehicle;
                    minDistance = distance;
                }
            }
        }
    }

    return returnedObject;
}

//------------------------------------------------------------------------------
/**
    @param object object to check the type
    @returns usable type of object
*/
nRnsEntityManager::UsableObjectType
nRnsEntityManager::GetUsableType( nObject * object )
{
    if( object )
    {
        if( object->IsA( "ninventoryitem" ) )
        {
            nInventoryItem * item = static_cast<nInventoryItem*>( object );
            if( item->IsDropped() )
            {
                return UOT_DROPPING;
            }
        }
        else if( object->IsA( "nentityobject" ) )
        {
            nEntityObject * entity = static_cast<nEntityObject*>( object );

            if( entity->HasComponent( "ncGameplayVehicle" ) )
            {
                return UOT_VEHICLE;
            }
            else
            {
                ncDictionaryClass * dictionary = 0;

                dictionary = entity->GetClassComponent<ncDictionaryClass>();

                if( dictionary )
                {
                    if( strcmp( dictionary->GetStringVariable( "ItemType" ), "UsableItem" ) == 0 )
                    {
                        return UOT_USABLE;
                    }
                }
            }
        }
    }

    return UOT_NOTHING;
}

//------------------------------------------------------------------------------
/**
    @param data strem of data to get/put the Player Data
*/
void
nRnsEntityManager::PlayerData::UpdateStream( nstream & data )
{
    data.UpdateInt( this->flags );
    data.UpdateByte( this->weapon );
    data.UpdateVector3( this->position );
    data.UpdateFloat( this->angles.rho );
    data.UpdateFloat( this->angles.theta );
}

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nRnsEntityManager)

    NSCRIPT_ADDCMD('JGLP', nEntityObject *, GetLocalPlayer , 0, (), 0, ());


    cl->BeginSignals( 1 );
    N_INITCMDS_ADDSIGNAL(EntityEliminated);
    N_INITCMDS_ADDSIGNAL( MountingAccessory )
    N_INITCMDS_ADDSIGNAL( CancelAccessory )
    N_INITCMDS_ADDSIGNAL( WeaponChanged )
    cl->EndSignals();

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
