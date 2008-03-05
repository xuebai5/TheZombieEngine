//------------------------------------------------------------------------------
//  rnsgamestate_net.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsstates.h"

#include "rnsstates/rnsgamestate.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "rnsgameplay/nrnsentitymanager.h"

#include "ncgameplayplayer/ncgameplayplayer.h"

#include "ncgamecamera/ncgamecamera.h"

#include "gameplay/nmissionhandler.h"
#include "gameplay/ngamemessagewindowproxy.h"
#include "nmusictable/nmusictable.h"
#include "ncsound/ncsound.h"

#include "util/nstream.h"

#include "rnsgameplay/ninventoryitem.h"
#include "rnsgameplay/ninventorymanager.h"

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::InitConnectionWith( const nString &host, const nString & port )
{
    this->initClient = true;
    this->hostName = host;
    this->hostPort = port;
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::InitNetwork()
{
    // search for already created Network Manager
    this->network = nNetworkManager::Instance();

    // if there is no Network Manager
    if( ! this->network )
    {
        // Create an adecuated Network Manager
        this->network = (nNetworkManager*)kernelServer->New("nnetworkmanager", "/sys/servers/network");
    }

    n_assert_if2( this->network, "The Network manager can't be created" )
    {
        if( this->initClient )
        {
            this->network->ConnectHost( this->hostName.Get(), this->hostPort.Get() );
        }

        // bind with the Network Manager
        this->network->BindSignal( 
            nNetworkManager::SignalOnNewClient, this, &RnsGameState::NewClientConnected, 0 );
        this->network->BindSignal( 
            nNetworkManager::SignalOnClientDisconnected, this, &RnsGameState::ClientDisconnected, 0 );
        this->network->BindSignal(
            nNetworkManager::SignalOnConnected, this, &RnsGameState::Connected, 0 );
        this->network->BindSignal(
            nNetworkManager::SignalOnDisconnected, this, &RnsGameState::Disconnected, 0 );
        this->network->BindSignal( 
            nNetworkManager::SignalOnRPC, this, &RnsGameState::RPCFromServer, 0 );
        this->network->BindSignal( 
            nNetworkManager::SignalOnRPCServer, this, &RnsGameState::RPCFromClient, 0 );
    }
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::EndNetwork()
{
    if( this->network )
    {
        // unbind from the Network Manager
        this->network->UnbindTargetObject( nNetworkManager::SignalOnNewClient.GetId(), this );
        this->network->UnbindTargetObject( nNetworkManager::SignalOnClientDisconnected.GetId(), this );
        this->network->UnbindTargetObject( nNetworkManager::SignalOnConnected.GetId(), this );
        this->network->UnbindTargetObject( nNetworkManager::SignalOnDisconnected.GetId(), this );
        this->network->UnbindTargetObject( nNetworkManager::SignalOnRPC.GetId(), this );
        this->network->UnbindTargetObject( nNetworkManager::SignalOnRPCServer.GetId(), this );
        this->network->Release();
        this->network = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @param client number of connected client
*/
void
RnsGameState::NewClientConnected( int client )
{
    NLOG( network, ( nNetworkManager::NLOGAPPLICATION | 0, 
        "client %d connected\n", client ) );

    // create the player info
    n_assert( ! this->players[ client ] );
    this->players[ client ] = n_new( PlayerInfo );
    this->players[ client ]->clientEntity = 0;
    n_assert( this->players[ client ] );
}

//------------------------------------------------------------------------------
/**
    @param client number of disconnected client
*/
void
RnsGameState::ClientDisconnected( int client )
{
    NLOG( network, ( nNetworkManager::NLOGAPPLICATION | 0, 
        "client %d disconnected [%s]\n", client, 
        ( this->players[ client ] ? "full connection" : "failure connection" ) ) );

    if( this->players[ client ] )
    {
        // fix camera if point to deleted client
        if( this->gameCamera->GetAnchorPoint() == this->players[ client ]->clientEntity )
        {
            this->ChangeCameraAnchor( 0 );
        }

        if( this->players[ client ]->clientEntity )
        {
            nEntityObject * playerEntity = this->players[ client ]->clientEntity;
            ncGameplayLiving * living = playerEntity->GetComponentSafe<ncGameplayLiving>();
            if( living )
            {
                // kill living entity
                living->SetDead();
            }
            else
            {
                // delete client in the Entity Manager
                this->entityManager->DeleteEntity( playerEntity->GetId() );
            }
        }

        n_delete( this->players[ client ] );
        this->players[ client ] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::Connected()
{
    nstream data;
    nRnsEntityManager::PlayerData playerData;

    // initialice player data
    playerData.weapon = nRnsEntityManager::WEAPON_M4;
    playerData.flags = 0;

    data.SetWrite( true );
    playerData.UpdateStream( data );

    // tell server that create a player for you
    this->network->CallRPCServer( nRnsEntityManager::SPAWN_PLAYER, data.GetBufferSize(), data.GetBuffer() );
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::Disconnected()
{
    if( this->savedState.IsEmpty() )
    {
        this->app->SetQuitRequested(true);
    }
    else
    {
        this->app->SetState( this->savedState );
    }
}

//------------------------------------------------------------------------------
/**
    @param rpcId Remote Procedure Call Identifier
    @param size size of the buffer with the parameters
    @param buffer buffer with the parameters
*/
void
RnsGameState::RPCFromServer( char rpcId, int size, const char * buffer )
{
    int entityId = 0;
    nRnsEntityManager::PlayerData playerData;

    // initialize the strem with the parameters
    nstream data;
    data.SetExternBuffer( size, buffer );
    data.SetWrite( false );

    switch( rpcId )
    {
    case nRnsEntityManager::SPAWN_PLAYER:
        playerData.UpdateStream( data );
        this->SpawnPlayer( SERVER_PLAYER, playerData );
        break;

    case nRnsEntityManager::SET_LOCAL_PLAYER:
        {
            data.UpdateInt( entityId );
            this->SetLocalPlayer( entityId );
        }
        break;

    case nRnsEntityManager::KILL_ENTITY:
        data.UpdateInt( entityId );
        this->entityManager->KillEntity( entityId );
        break;

    case nRnsEntityManager::SUICIDE_PLAYER:
        if( this->network->IsServer() )
        {
            if( this->players[ SERVER_PLAYER ] && this->players[ SERVER_PLAYER ]->clientEntity )
            {
                data.UpdateInt( entityId );
                if( this->players[ SERVER_PLAYER ]->clientEntity->GetId() == unsigned(entityId) )
                {
                    this->entityManager->SuicidePlayer( this->players[ SERVER_PLAYER ]->clientEntity );
                }
            }
        }
        break;

    case nRnsEntityManager::DELETE_ENTITY:
        data.UpdateInt( entityId );
        this->entityManager->DeleteEntity( entityId );
        break;

    case nRnsEntityManager::UPDATE_INVENTORY:
        {
            nInventoryManager * inventory = nInventoryManager::Instance();
            if( inventory )
            {
                inventory->NetworkUpdate( data );
            }
        }
        break;

    case nRnsEntityManager::INVENTORY_ITEM:
        {
            nInventoryManager * inventory = nInventoryManager::Instance();
            if( inventory )
            {
                inventory->NewNetworkItem( data );
            }
        }
        break;
        
    case nRnsEntityManager::SET_OBJECTIVE_STATE:
        nMissionHandler::Instance()->ReceiveObjectiveStateChange( &data );
        break;

	case nRnsEntityManager::STOP_MUSIC:
		// @todo Make music table a singleton or get its NOH path from a constant
        static_cast<nMusicTable*>( nKernelServer::Instance()->Lookup("/usr/musictable") )->ReceiveStopMusic( &data );
        break;
	case nRnsEntityManager::PLAY_MUSIC_PART:
		static_cast<nMusicTable*>( nKernelServer::Instance()->Lookup("/usr/musictable") )->ReceivePlayMusicPart( &data );
        break;
	case nRnsEntityManager::PLAY_MUSIC_STINGER:
		static_cast<nMusicTable*>( nKernelServer::Instance()->Lookup("/usr/musictable") )->ReceivePlayMusicStinger( &data );
        break;
    case nRnsEntityManager::SET_MUSIC_MOOD:
        static_cast<nMusicTable*>( nKernelServer::Instance()->Lookup("/usr/musictable") )->ReceiveMoodToPlay( &data );
        break;
	case nRnsEntityManager::SET_MUSIC_STYLE:
		static_cast<nMusicTable*>( nKernelServer::Instance()->Lookup("/usr/musictable") )->ReceiveSetCurrentStyle( &data );
        break;

    // @todo Remove this and use a network component to issue play/stop commands on the entity
    case nRnsEntityManager::PLAY_SOUND_EVENT:
        {
            data.UpdateInt( entityId );
            nEntityObject* targetEntity( nEntityObjectServer::Instance()->GetEntityObject( entityId ) );
            if ( targetEntity )
            {
                ncSound* sound( targetEntity->GetComponent<ncSound>() );
                if ( sound )
                {
                    sound->ReceiveEventToPlay( &data );
                }
            }
        }
        break;
    case nRnsEntityManager::STOP_SOUND:
        {
            data.UpdateInt( entityId );
            nEntityObject* targetEntity( nEntityObjectServer::Instance()->GetEntityObject( entityId ) );
            if ( targetEntity )
            {
                ncSound* sound( targetEntity->GetComponent<ncSound>() );
                if ( sound )
                {
                    sound->StopSound();
                }
            }
        }
        break;

    case nRnsEntityManager::WEAPON_TRIGGER:
        if( this->entityManager->GetLocalPlayer() )
        {
            nEntityObject * player = this->entityManager->GetLocalPlayer();

            bool weaponTrigger, weaponPressed;

            data.UpdateBool( weaponTrigger );
            data.UpdateBool( weaponPressed );

            ncGameplayPlayer * gameplay = player->GetComponentSafe<ncGameplayPlayer>();
            if( gameplay )
            {
                gameplay->SetWeaponTrigger( weaponTrigger, weaponPressed );
            }
        }
        break;

    case nRnsEntityManager::WEAPON_BURST:
        {
            if( ! this->network->IsServer() )
            {
                bool weaponTrigger;
                bool weaponPressed;

                data.UpdateInt( entityId );
                data.UpdateBool( weaponTrigger );
                data.UpdateBool( weaponPressed );

                nEntityObject* entity = 0;
                entity = nEntityObjectServer::Instance()->GetEntityObject( entityId );
                if( entity )
                {
                    ncGameplayPlayer * gameplay = entity->GetComponentSafe<ncGameplayPlayer>();
                    if( gameplay )
                    {
                        gameplay->SetWeaponTrigger( weaponTrigger, weaponPressed );
                    }
                }
            }
        }
        break;

    case nRnsEntityManager::SHOW_GAME_MESSAGE:
        nGameMessageWindowProxy::Instance()->ReceiveGameMessage( &data );
        break;
    }

    data.SetExternBuffer( 0, 0 );
}

//------------------------------------------------------------------------------
/**
    @param client identifier of the client that request the call
    @param rpcId Remote Procedure Call Identifier
    @param size size of the buffer with the parameters
    @param buffer buffer with the parameters
*/
void
RnsGameState::RPCFromClient( int client, char rpcId, int size, const char * buffer )
{
    nRnsEntityManager::PlayerData playerData;
    int entityId = 0;

    // initialize the strem with the parameters
    nstream data;
    data.SetExternBuffer( size, buffer );
    data.SetWrite( false );

    switch( rpcId )
    {
    case nRnsEntityManager::SPAWN_PLAYER:
        playerData.UpdateStream( data );
        this->SpawnPlayer( client, playerData );
        break;

    case nRnsEntityManager::WEAPON_TRIGGER:
        if( this->players[ client ] && this->players[ client ]->clientEntity )
        {
            nEntityObject * player = this->players[ client ]->clientEntity;

            bool weaponTrigger, weaponPressed;

            data.UpdateBool( weaponTrigger );
            data.UpdateBool( weaponPressed );

            ncGameplayPlayer * gameplay = player->GetComponent<ncGameplayPlayer>();
            if( gameplay )
            {
                gameplay->SetWeaponTrigger( weaponTrigger, weaponPressed );
            }
        }
        break;

    case nRnsEntityManager::SUICIDE_PLAYER:
        if( this->players[ client ] && this->players[ client ]->clientEntity )
        {
            data.UpdateInt( entityId );
            if( this->players[ client ]->clientEntity->GetId() == unsigned(entityId) )
            {
                this->entityManager->SuicidePlayer( this->players[ client ]->clientEntity );
            }
        }
        break;

    }
    data.SetExternBuffer( 0, 0 );
}

//------------------------------------------------------------------------------
