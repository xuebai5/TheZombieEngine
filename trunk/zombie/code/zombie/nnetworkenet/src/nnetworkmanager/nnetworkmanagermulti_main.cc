//------------------------------------------------------------------------------
//  nnetworkmanagermulti_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkmanager/nclientproxy.h"

#include "nnetworkenet/nnetclientenet.h"
#include "nnetworkenet/nnetserverenet.h"

#include "network/nsessionserver.h"
#include "network/nsessionclient.h"
#include "network/nsessionservercontext.h"

#include "nnetworkmanager/nnetworkmanagermulti.h"

#include "entity/nentityobjectserver.h"

#include "ncnetwork/ncnetwork.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nNetworkManagerMulti, "nnetworkmanager");

//------------------------------------------------------------------------------
const float nNetworkManagerMulti::TIME_ENTITY_UPDATE = 0.2f;
const float nNetworkManagerMulti::TIME_CLOCK_UPDATE = 0.5f;

//------------------------------------------------------------------------------
/**
*/
nNetworkManagerMulti::nNetworkManagerMulti():
    clientState( nNetworkManagerMulti::CLIENT_DISCONNECTED ),
    isServerStarted( false ),
    isSearching( false ),
    updateEntityTime( 0.0f ),
    updateClockTime( 0.0f ),
    maxBlocksSended( 2 ),

    bytesActionsSended( "netSendedActions", nArg::Int ),
    bytesActionsReceived( "netReceivedActions", nArg::Int ),
    bytesEntitiesUpdated( "netEntitiesUpdate", nArg::Int ),
    longestAction( 0 ),
    longestRPC( 0 ),
    longestUpdateEntity( 0 )
{
    this->isServer = false;

    this->SetMaxClients( 10 );

    // prepare buffers
    this->actionOrderedIndex = int( nNetUtils::PackTo( 
        this->actionOrderedBuffer, char( nNetworkManager::MESSAGE_ACTION ) ) );
    this->actionUnreliableIndex = int( nNetUtils::PackTo( 
        this->actionUnreliableBuffer, char( nNetworkManager::MESSAGE_ACTION ) ) );

    nEntityObjectServer::Instance()->BindSignal( nEntityObjectServer::SignalEntityDeleted, 
        this, &nNetworkManagerMulti::EntityDeleted, 0 );

    this->AddNetworkEntities();

    NLOG( network, ( NLOGBASIC | 0, "Create Network Manager Multiplayer" ) );
}

//------------------------------------------------------------------------------
/**
*/
nNetworkManagerMulti::~nNetworkManagerMulti()
{
    nEntityObjectServer::Instance()->UnbindTargetObject( 
        nEntityObjectServer::SignalEntityDeleted.GetId(), this );

    if( this->isServerStarted )
    {
        this->StopServer();
    }

    if( this->isSearching )
    {
        this->StopSearching();
    }

    if( this->clientState == CLIENT_CONNECTED )
    {
        this->Disconnect();
    }

    this->actionsSended.Resume( "Basic Actions Sended" );
    this->actionsRecv.Resume( "Basic Actions Recv" );
    this->rpcSended.Resume( "Remote Calls Sended" );
    this->rpcRecv.Resume( "Remote Calls Recv" );
    this->entitiesUpdated.Resume( "Entities Updated" );
    this->protocolSendOverhead.Resume( "Network Manager Send Overhead" );
    this->protocolRecvOverhead.Resume( "Network Manager Recv Overhead" );

    NLOG( network, ( NLOGBASIC | 1, "Longest Action Size : %d bytes", this->longestAction ) );
    NLOG( network, ( NLOGBASIC | 1, "Longest RPC message : %d bytes", this->longestRPC ) );
    NLOG( network, ( NLOGBASIC | 1, "Longest Entity Update Size : %d bytes", this->longestUpdateEntity ) );

    NLOG( network, ( NLOGBASIC | 0, "Destroy Network Manager Multiplayer" ) );
}

//------------------------------------------------------------------------------
/**
    @param time application time
*/
void
nNetworkManagerMulti::Trigger( nTime time )
{
    nNetworkManager::Trigger( time );

    // trigger session layer of the network
    if( this->refSessionServer.isvalid() )
    {
        this->refSessionServer->SetTime( time );
        this->refSessionServer->Trigger();
    }

    if( this->refSessionClient.isvalid() && this->isSearching )
    {
        this->refSessionClient->SetTime( time );
        this->refSessionClient->Trigger();
    }

    // trigger the transport layer of the network
    if( this->refNetServer.isvalid() )
    {
        this->refNetServer->Trigger();

        this->updateEntityTime += this->deltaTime;
        if( this->updateEntityTime > TIME_ENTITY_UPDATE )
        {
            this->SendActualFrame();
            this->updateEntityTime = 0;
        }

        this->updateClockTime += this->deltaTime;
        if( this->updateClockTime > TIME_CLOCK_UPDATE )
        {
            this->SendClockTime();
            this->updateClockTime = 0;
        }
    }

    if( this->refNetClient.isvalid() )
    {
        this->refNetClient->Trigger();
    }

    if( this->clientState == CLIENT_DISCONNECTED && this->refNetClient.isvalid() )
    {
        this->refNetClient->Release();
    }

    // update network entities
    for( int i = 0 ; i < this->networkEntities.Size() ; ++i )
    {
        this->networkEntities[ i ]->Update( this->deltaTime );
    }

    // send buffers
    if( this->actionOrderedIndex > 1 )
    {
        this->SendActionBuffer( 
            this->actionOrderedBuffer, this->actionOrderedIndex, nNetUtils::Ordered );
    }

    if( this->actionUnreliableIndex > 1 )
    {
        this->SendActionBuffer( 
            this->actionUnreliableBuffer, this->actionUnreliableIndex, nNetUtils::Unreliable );
    }

    // update statistics
    this->bytesActionsSended->SetI( this->actionsSended.lastBytes );
    this->bytesActionsReceived->SetI( this->actionsRecv.lastBytes );
    this->bytesEntitiesUpdated->SetI( this->entitiesUpdated.lastBytes );

    this->actionsSended.Update( this->deltaTime );
    this->actionsRecv.Update( this->deltaTime );
    this->rpcSended.Update( this->deltaTime );
    this->rpcRecv.Update( this->deltaTime );
    this->entitiesUpdated.Update( this->deltaTime );
    this->protocolSendOverhead.Update( this->deltaTime );
    this->protocolRecvOverhead.Update( this->deltaTime );
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::DeleteClients()
{
    for( int i = 0 ; i < this->clients.Size() ; ++i )
    {
        if( this->clients[ i ] ){
            n_delete( this->clients[ i ] );
            this->clients[ i ] = 0;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param entity modified entity
    @returns true if entity can be added
*/
void
nNetworkManagerMulti::AddModifiedEntity( ncNetwork * const entity )
{
    n_assert( entity );
    if( ! entity )
    {
        return;
    }

    bool valid = this->networkEntities.Find( entity ) != 0;
    if( valid )
    {
        NLOG( network, ( NLOGENTITIES | 3, "Add Modified Entity %x", entity->GetEntityObject()->GetId() ) );
        this->modifiedEntities.Append( entity );
    }
    else
    {
        entity->SetUpdated();
    }
}

//------------------------------------------------------------------------------
/**
    @param entity new entity
*/
void
nNetworkManagerMulti::AddNetworkEntity( ncNetwork * const entity )
{
    n_assert( entity );
    if( ! entity )
    {
        return;
    }

    bool valid = false;

    int id = entity->GetEntityObject()->GetId();

    valid = nEntityObjectServer::Instance()->GetEntityObjectType( id ) == nEntityObjectServer::Server ||
            nEntityObjectServer::Instance()->GetEntityObjectType( id ) == nEntityObjectServer::Normal;

    NLOGCOND( network, ! valid, ( NLOGENTITIES | 1, "Server ERROR: Created entity is not a Network Entity" ) );

    if( valid && ! this->IsRegisteredEntity( entity->GetEntityObject() ) )
    {
        this->networkEntities.Append( entity );

        if( this->isServer )
        {
            // send new entity to the clients
            nstream data;
            char messageType = char( nNetworkManager::MESSAGE_ENTITY_CREATED );

            data.Reset();
            data.SetWrite( true );
            data.UpdateByte( messageType );
            data.UpdateInt( id );
            data.UpdateString( entity->GetEntityObject()->GetClass()->GetName() );

            // get entity data
            if( entity->InitFromNetwork( data ) )
            {
                this->refNetServer->SendMessageAll( 
                    data.GetBuffer(), data.GetBufferSize(), nNetUtils::Ordered );
                NLOG( network, ( NLOGENTITIES | 1, "Server: New Entity %x send", id ) );

                this->protocolSendOverhead.lastBytes += data.GetBufferSize();
                ++this->protocolSendOverhead.lastNum;
            }
            else
            {
                NLOG( network, ( NLOGENTITIES | 1, "Server ERROR: New Entity %x can not be sended", id ) );
            }
        }
        else
        {
            NLOG( network, ( NLOGENTITIES | 1, "Server: New Network Entity %x", id ) );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param entity modified entity
*/
void
nNetworkManagerMulti::EntityDeleted( int id )
{
    for( int i = 0 ; i < this->modifiedEntities.Size() ; ++i )
    {
        if( this->modifiedEntities[ i ]->GetEntityObject()->GetId() == (unsigned)id )
        {
            this->modifiedEntities.EraseQuick( i );
            break;
        }
    }

    nEntityObject * entity = 0;

    for( int i = 0 ; i < this->networkEntities.Size() ; ++i )
    {
        if( this->networkEntities[ i ]->GetEntityObject()->GetId() == (unsigned)id )
        {
            entity = this->networkEntities[ i ]->GetEntityObject();
            this->networkEntities.EraseQuick( i );

            if( this->isServer )
            {
                // send delete entity to the clients
                nstream data;
                char messageType = char( nNetworkManager::MESSAGE_ENTITY_DELETED );

                data.Reset();
                data.SetWrite( true );
                data.UpdateByte( messageType );
                data.UpdateInt( id );

                this->refNetServer->SendMessageAll( 
                    data.GetBuffer(), data.GetBufferSize(), nNetUtils::Ordered );
                NLOG( network, ( NLOGENTITIES | 1, "Server: Network Entity %x deleted", id ) );

                this->protocolSendOverhead.lastBytes += data.GetBufferSize();
                ++this->protocolSendOverhead.lastNum;
            }
            else
            {
                NLOG( network, ( NLOGENTITIES | 1, "Client: Network Entity %x deleted", id ) );
            }

            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::AddNetworkEntities()
{
    nEntityObjectServer * objectServer = 0;
    objectServer = nEntityObjectServer::Instance();

    nEntityObjectId entityId = nEntityObjectServer::IDINVALID;
    ncNetwork * networkComp = 0;
    bool valid = false;

    for( nEntityObject * entity = objectServer->GetFirstEntityObject() ; 
        entity ;
        entity = objectServer->GetNextEntityObject() )
    {
        entityId = entity->GetId();

        valid = entityId != nEntityObjectServer::IDINVALID;
        NLOGCOND( network, nEntityObjectServer::IDINVALID == entityId, 
            ( NLOGSERVER | 1, "Invalid entityId" ) );
        if( valid )
        {
            valid = objectServer->GetEntityObjectType( entityId ) == nEntityObjectServer::Normal;

            networkComp = entity->GetComponent<ncNetwork>();

            if( networkComp && valid && ! this->IsRegisteredEntity( entity ) )
            {
                this->networkEntities.Append( networkComp );

                if( networkComp->IsModified() )
                {
                    this->AddModifiedEntity( networkComp );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param entity entity to quest
    @returns true if entity is registered
*/
bool 
nNetworkManagerMulti::IsRegisteredEntity( nEntityObject * entity )const
{
    if( entity )
    {
        ncNetwork * network = entity->GetComponent<ncNetwork>();
        if( network )
        {
            return (this->networkEntities.Find( network ) != 0);
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    @param deltatime time since last update
*/
void
nNetworkManagerMulti::NetStatistic::Update( const float deltatime )
{
    if( this->lastNum > 0 )
    {
        ++this->activeTicks;
        this->activeSeconds += deltatime;

        if( this->lastBytes < this->minBytes )
        {
            this->minBytes = this->lastBytes;
        }

        if( this->lastBytes > this->maxBytes )
        {
            this->maxBytes = this->lastBytes;
        }

        if( this->lastNum < this->minNum )
        {
            this->minNum = this->lastNum;
        }

        if( this->lastNum > this->maxNum )
        {
            this->maxNum = this->lastNum;
        }

        this->totalBytes += this->lastBytes;
        this->lastBytes = 0;

        this->totalNum += this->lastNum;
        this->lastNum = 0;
    }
}

//------------------------------------------------------------------------------
/**
    @param msg description of statistic
*/
void
nNetworkManagerMulti::NetStatistic::Resume( const char * msg )const
{
    if( this->totalBytes < 1024 )
    {
        NLOG( network, ( NLOGBASIC | 1, "- %s : %d bytes, %d num", msg, this->totalBytes, this->totalNum ) );
    }
    else if( this->totalBytes < 1024*1024 )
    {
        NLOG( network, ( NLOGBASIC | 1, "- %s : %d KBs, %d num", msg, (this->totalBytes >> 10), this->totalNum ) );
    }
    else
    {
        NLOG( network, ( NLOGBASIC | 1, "- %s : %d MBs, %d num", msg, (this->totalBytes >> 20), this->totalNum ) );
    }
    if( this->totalNum > 0 )
    {
        NLOG( network, ( NLOGBASIC | 1, "  ticks : %d, [ %d .. %d ] bytes, mean : %.1f bytes, %.2f Bps", 
            this->activeTicks, this->minBytes, this->maxBytes, 
            this->BytesPerTick(), this->BytesPerSecond() ) );
    }
}

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(nNetworkManagerMulti);
NSCRIPT_INITCMDS_END();

//------------------------------------------------------------------------------
