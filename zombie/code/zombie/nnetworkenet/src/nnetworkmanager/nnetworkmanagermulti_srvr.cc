//------------------------------------------------------------------------------
//  nnetworkmanagermulti_srvr.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkmanager/nnetworkmanagermulti.h"
#include "nnetworkmanager/nclientproxy.h"

#include "nnetworkenet/nnetserverenet.h"

#include "network/nsessionserver.h"

#include "nnetworkenet/nnetutils.h"

#include "ncnetwork/ncnetwork.h"

#include "entity/nentityobjectserver.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
    @param clients number of maximun clients
*/
void
nNetworkManagerMulti::SetMaxClients( const int clients )
{
    n_assert2( ! this->isServerStarted, "Can not modify maximum clients with the server running" );

    if( ! this->isServerStarted )
    {
        this->maxClients = clients;
        this->clients.SetFixedSize( this->maxClients );
        for( int i = 0 ; i < this->maxClients ; ++i )
        {
            this->clients[ i ] = 0;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param port a name for the server port
*/
void
nNetworkManagerMulti::SetServerPort( const nString & port )
{
    n_assert2( ! this->isServerStarted, "Can not modify server port with the server running" );

    if( ! this->isServerStarted )
    {
        this->serverPort = port;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::StartServer()
{
    n_assert2( this->serverPort.Length(), "No Server Port" );
    n_assert2( this->maxClients > 0, "No clients allowed to connect server" );
    n_assert2( !this->isServerStarted, "Network Server already started" );

    bool valid = this->serverPort.Length() && (this->maxClients > 0) && ! this->isServerStarted;

    nString newname;

    // create enet server
    if( valid )
    {
        newname = this->GetFullName();
        newname.Append( "/netserver" );
        this->refNetServer = (nNetServerEnet*) kernelServer->New("nnetserverenet", newname.Get() );
        valid = this->refNetServer.isvalid();
        n_assert2( valid , "Can not create NetServer Enet");
    }

    // initialize enet server
    if( valid )
    {
        this->refNetServer->SetMaxClients( this->maxClients );
        this->refNetServer->SetServerPort( this->serverPort.Get() );
        valid = this->refNetServer->Open();
        this->refNetServer->SetHandler( this );
    }

    // Create session server
    if( valid )
    {
        newname = this->GetFullName();
        newname.Append( "/sessionserver" );
        this->refSessionServer = (nSessionServer*) kernelServer->New( "nsessionserver", newname.Get() );
        valid = this->refSessionServer.isvalid();
        n_assert2( valid, "Can not create Session Server" );
    }

    // initialize session server
    if( valid )
    {
        this->refSessionServer->SetAppName( this->serverPort.Get() );
        this->refSessionServer->SetAppVersion( "0.1" );
        this->refSessionServer->SetMaxNumClients( this->maxClients );
        this->refSessionServer->Open();
    }

    if( ! valid )
    {
        this->refSessionServer.invalidate();
        this->refNetServer.invalidate();
    }

    if( valid )
    {
        this->StartClock();
    }

    this->isServerStarted = valid;
    this->isServer = valid;

    NLOGCOND( network, valid, ( NLOGSERVER | 0, "Server Started" ) );
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::StopServer()
{
    n_assert2( this->isServerStarted, "Network Server is not started" );

    if( this->isServerStarted )
    {
        this->refSessionServer->Release();
        this->refNetServer->Release();

        this->DeleteClients();

        this->StopClock();

        this->isServerStarted = false;
        NLOG( network, ( NLOGSERVER | 0, "Server Stoped" ) );
    }
}

//------------------------------------------------------------------------------
/**
    @param index client index
*/
void
nNetworkManagerMulti::Kick( const int index )
{
    n_assert2( this->isServerStarted, "Network Server is not started" );

    if( this->isServerStarted )
    {
        bool valid = (index >= 0) && (index < this->clients.Size());
        n_assert2( valid, "Invalid index of Network Client" );

        nClientProxy * client = 0;
        if( valid )
        {
            client = this->clients[ index ];
            valid = client != 0;
        }

        if( valid )
        {
            this->refNetServer->CloseConnection( client->number );
            NLOG( network, ( NLOGSERVER | 1, "Kick of client %d", client->number ) );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param index index of the client
    @param id id of the RPC call
    @param size size of the data
    @param data data of the RPC
*/
void
nNetworkManagerMulti::CallRPCClient( const int index, unsigned char id, const int size, const char * const data )
{
    n_assert2( this->isServerStarted, "Network Server is not started" );

    if( this->isServerStarted )
    {
        bool valid = (index >= 0) && (index < this->clients.Size());
        n_assert2( valid, "Invalid index of Network Client" );

        nClientProxy * client = 0;
        if( valid )
        {
            client = this->clients[ index ];
            valid = client != 0;
        }

        if( valid )
        {
            valid = size <= MAX_RPC_BUFFER - 2;
            n_assert2( valid, "RPC Call with to much data" );

            if( valid )
            {
                char * pointer = this->rpcBuffer;
                pointer += nNetUtils::PackTo( pointer, char( nNetworkManager::MESSAGE_RPC ) );
                pointer += nNetUtils::PackTo( pointer, id );
                pointer += nNetUtils::PackArrayTo( pointer, data, size );

                this->refNetServer->SendMessage( this->rpcBuffer, 2 + size, client->number, nNetUtils::Ordered );
                NLOG( network, ( NLOGSERVER | 1, "Call RPC[%d] to client %d", id, client->number ) );

                if( unsigned int(2 + size) > this->longestRPC )
                {
                    this->longestRPC = 2 + size;
                }
                this->rpcSended.lastBytes += 2 + size;
                ++this->rpcSended.lastNum;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param id id of the RPC call
    @param size size of the data
    @param data data of the RPC
*/
void
nNetworkManagerMulti::CallRPCAll( unsigned char id, const int size, const char * const data )
{
    n_assert2( this->isServerStarted, "Network Server is not started" );

    if( this->isServerStarted )
    {
        bool valid = size <= MAX_RPC_BUFFER - 2;
        n_assert2( valid, "RPC Call with to much data" );

        if( valid )
        {
            char * pointer = this->rpcBuffer;
            pointer += nNetUtils::PackTo( pointer, char( nNetworkManager::MESSAGE_RPC ) );
            pointer += nNetUtils::PackTo( pointer, id );
            pointer += nNetUtils::PackArrayTo( pointer, data, size );

            this->refNetServer->SendMessageAll( this->rpcBuffer, 2 + size, nNetUtils::Ordered );
            NLOG( network, ( NLOGSERVER | 1, "Call RPC[%d] to all clients", id ) );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param id id of the RPC call
    @param size size of the data
    @param data data of the RPC
*/
void
nNetworkManagerMulti::CallRPCAllServer( unsigned char id, const int size, const char * const data )
{
    NLOG( network, ( NLOGSERVER | 1, "The server call RPC[%d]", id ) );
                
    this->SignalOnRPC( this, id, size, data );

    this->CallRPCAll( id, size, data );
}

//------------------------------------------------------------------------------
/**
    @param client number of connected client
*/
void
nNetworkManagerMulti::SendNetworkEntities( int client )
{
    if( this->isServerStarted )
    {
        nstream data;
        char messageType = char( nNetworkManager::MESSAGE_ENTITY_CREATED );
        int entityId;
        ncNetwork * entity = 0;

        nEntityObjectServer * objectServer = nEntityObjectServer::Instance();

        for( int i = 0 ; i < this->networkEntities.Size() ; ++i )
        {
            entity = this->networkEntities[ i ];
            
            entityId = entity->GetEntityObject()->GetId();

            NLOGCOND( network, nEntityObjectServer::IDINVALID == entityId, 
                ( NLOGENTITIES | 1, "Server: invalid entityId" ) );
            if( entityId != nEntityObjectServer::IDINVALID )
            {
                // only send network entities
                // don't send normal entities, because client already load they from level
                if( objectServer->GetEntityObjectType( entityId ) == nEntityObjectServer::Server )
                {
                    data.Reset();
                    data.SetWrite( true );
                    data.UpdateByte( messageType );
                    data.UpdateInt( entityId );
                    data.UpdateString( entity->GetEntityObject()->GetClass()->GetName() );

                    if( entity->InitFromNetwork( data ) )
                    {
                        this->refNetServer->SendMessage( 
                            data.GetBuffer(), data.GetBufferSize(), client, nNetUtils::Ordered );

                        this->protocolSendOverhead.lastBytes += data.GetBufferSize();
                        ++this->protocolSendOverhead.lastNum;
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::SendActualFrame()
{
    char type = nNetworkManager::MESSAGE_ENTITY_UPDATE;
    nstream data;

    int blocksSended = 0;

    // reset entity buffer
    int bufferSize = 0;
    char * pointer = this->entityUpdateBuffer;
    char * sizePointer = 0;
    pointer += nNetUtils::PackTo( pointer, type );

    for( int i = 0 ; i < this->modifiedEntities.Size() ; ++i )
    {
        ncNetwork * entity = this->modifiedEntities[ i ];
        n_assert( entity );

        if( entity )
        {
            int id = entity->GetEntityObject()->GetId();

            bufferSize = MTU_BUFFER_SIZE - int( pointer - this->entityUpdateBuffer );
            data.SetExternBuffer( bufferSize, pointer );
            data.SetWrite( true );
            data.UpdateInt( id );

            // prepare slot for size of update
            sizePointer = pointer + data.GetBufferSize();
            data.UpdateInt( id );

            entity->UpdateNetwork( data );

            n_assert2( data.GetBufferSize() < MAX_ENTITY_UPDATE, "Entity Update too much big" );

            if( unsigned int( data.GetBufferSize() ) > this->longestUpdateEntity )
            {
                this->longestUpdateEntity = data.GetBufferSize();
            }

            bufferSize = data.GetBufferSize();
            pointer += bufferSize;

            // update size in the buffer
            nNetUtils::PackTo( sizePointer, bufferSize );

            // don't erase the buffer
            data.SetExternBuffer( 0, 0 );

            entity->SetUpdated();
        }

        // send actual block
        bufferSize = int( pointer - this->entityUpdateBuffer );
        if( bufferSize > MTU_SIZE )
        {
            this->refNetServer->SendMessageAll( 
                this->entityUpdateBuffer, bufferSize, nNetUtils::Unordered );

            this->entitiesUpdated.lastBytes += bufferSize;
            ++this->entitiesUpdated.lastNum;

            pointer = this->entityUpdateBuffer;
            pointer += nNetUtils::PackTo( pointer, type );

            if( ++blocksSended >= this->maxBlocksSended )
            {
                break;
            }
        }
    }

    // send last block
    bufferSize = int( pointer - this->entityUpdateBuffer );
    if( bufferSize > 1 )
    {
        this->refNetServer->SendMessageAll( 
            this->entityUpdateBuffer, bufferSize, nNetUtils::Unordered );

        this->entitiesUpdated.lastBytes += bufferSize;
        ++this->entitiesUpdated.lastNum;
    }

    this->modifiedEntities.Reset();
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::SendClockTime()
{
    bool valid = this->refNetServer.isvalid();
    n_assert2( valid, "The Net Server is not created" );

    if( valid )
    {
        int len = static_cast<int>( sizeof( this->clock ) ) + 1;
        char * buffer = n_new_array( char, len );
        n_assert( buffer );
        if( buffer )
        {
            char * pointer = buffer;
            pointer += nNetUtils::PackTo( pointer, char( nNetworkManager::MESSAGE_CLOCK ) );
            pointer += nNetUtils::PackTo( pointer, this->clock );

            this->refNetServer->SendMessageAll( buffer, len, nNetUtils::Ordered );

            n_delete_array( buffer );
            NLOG( network, ( NLOGSERVER | 3, "Send clock update to all \"%f\"", this->clock ) );

            this->protocolSendOverhead.lastBytes += len;
            ++this->protocolSendOverhead.lastNum;
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param message data of the message
    @param size size of the message payload
    @param data pointer to a client struct
*/
void
nNetworkManagerMulti::HandleMessage( const char * message, const int size, void * data )
{
    bool valid = message && ( size > 0 ) && data;
    n_assert2( valid, "A Invalid Message in the Network Server" );

    if( valid )
    {
        nClientProxy * client = static_cast< nClientProxy* >( data );

        switch( message[ 0 ] )
        {
        case nNetworkManager::MESSAGE_ZSTR:
            NLOG( network, ( NLOGSERVER | 2, "The client %d say \"%s\"", client->number, message + 1 ) );

            this->protocolRecvOverhead.lastBytes += size;
            ++this->protocolRecvOverhead.lastNum;
            break;

        case nNetworkManager::MESSAGE_PING:
            {
                NLOG( network, ( NLOGSERVER | 2, "Client %d ping", client->number ) );
                char buffer = char( nNetworkManager::MESSAGE_PONG );
                this->refNetServer->SendMessage( &buffer, 1, client->number, nNetUtils::Ordered );

                this->protocolRecvOverhead.lastBytes += size;
                ++this->protocolRecvOverhead.lastNum;

                this->protocolSendOverhead.lastBytes += 1;
                ++this->protocolSendOverhead.lastNum;
            }
            break;

        case nNetworkManager::MESSAGE_PONG:
            NLOG( network, ( NLOGSERVER | 2, "Client %d pong", client->number ) );

            this->protocolRecvOverhead.lastBytes += size;
            ++this->protocolRecvOverhead.lastNum;
            break;

        case nNetworkManager::MESSAGE_ERROR:
            n_message( "FATAL ERROR: %s\n", message + 1 );

            this->protocolRecvOverhead.lastBytes += size;
            ++this->protocolRecvOverhead.lastNum;
            break;

        case nNetworkManager::MESSAGE_RPC:
            {
                char rpcId( message[ 1 ] );
                NLOG( network, ( NLOGSERVER | 1, "Client %d call RPC[%d]", client->number, rpcId ) );
                
                this->rpcRecv.lastBytes += size;
                ++this->rpcRecv.lastNum;

                this->SignalOnRPCServer( this, client->number, rpcId, size - 2, message + 2 );
            }
            break;

        case nNetworkManager::MESSAGE_ACTION:
            NLOG( network, ( NLOGACTIONS | 2, "Server: action buffer of %d bytes from %d", size, client->number ) );
            this->ActionsFromClient( message + 1, size - 1 );

            this->actionsRecv.lastBytes += size;
            ++this->actionsRecv.lastNum;
            break;

        case nNetworkManager::MESSAGE_ENTITY_CREATED:
            n_assert2_always( "The Network Client can't create a Network entity" );
            break;

        case nNetworkManager::MESSAGE_ENTITY_DELETED:
            n_assert2_always( "The Network Client can't delete a Network entity" );
            break;

        default:
            n_assert2_always( "Unknow network message type" );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param num number of the new client
    @returns pointer to a client struct
    @retval 0 if we do not accept the new client
*/
void *
nNetworkManagerMulti::HandleNewClient( const int num )
{
    NLOG( network, ( NLOGSERVER | 1, "New Client at %d", num ) );

    nClientProxy * client = n_new( nClientProxy );
    n_assert( client );

    client->number = num;

    this->clients[ num ] = client;

    this->SignalOnNewClient( this, client->number );

    this->SendNetworkEntities( client->number );

    return client;
}

//------------------------------------------------------------------------------
/**
    @param data pointer to a client struct
*/
void
nNetworkManagerMulti::HandleClientDisconnect( void * data )
{
    NLOGCOND( network, !data, ( NLOGSERVER | 1, "Unknow Client disconnected " ) );

    if( data )
    {
        nClientProxy * client = static_cast< nClientProxy * >( data );

        NLOG( network, ( NLOGSERVER | 1, "Client disconnected at %d", client->number ) );

        this->clients[ client->number ] = 0;

        this->SignalOnClientDisconnected( this, client->number );

        n_delete( client );
    }
}

//------------------------------------------------------------------------------
