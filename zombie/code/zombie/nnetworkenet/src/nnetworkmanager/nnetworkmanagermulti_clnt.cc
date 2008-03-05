//------------------------------------------------------------------------------
//  nnetworkmanagermulti_clnt.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkmanager/nnetworkmanagermulti.h"

#include "nnetworkenet/nnetutils.h"
#include "nnetworkenet/nnetclientenet.h"

#include "network/nsessionclient.h"
#include "network/nsessionservercontext.h"

#include "rnsgameplay/ncgameplay.h"

#include "entity/nentityobjectserver.h"

#include "ncnetwork/ncnetwork.h"

#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
    @param port in that port number the server would be listening
*/
void
nNetworkManagerMulti::StartSearching( const nString & port )
{
    bool valid = ! this->isSearching;
    n_assert2( valid , "Network Client already searching" );

    if( ! valid )
    {
        return;
    }

#ifdef WIN32
    WORD versionRequested = MAKEWORD (1, 1);
    WSADATA wsaData;
    
    int result = WSAStartup (versionRequested, & wsaData);
    valid = result == 0;
    n_assert2( valid, "Windows Socket Startup failed" );
    if( ! valid )
    {
        return;
    }
#endif//WIN32

    nString newname;

    // Create session client
    newname = this->GetFullName();
    newname.Append( "/sessionclient" );
    this->refSessionClient = (nSessionClient*) kernelServer->New("nsessionclient", newname.Get() );
    valid = this->refSessionClient.isvalid();
    n_assert( valid );
    if( valid )
    {
        this->refSessionClient->SetAppName( port.Get() );
        this->refSessionClient->SetAppVersion( "0.1" );
        this->refSessionClient->Open();
        this->searchPort = port;
    }

    this->isSearching = valid;

    NLOGCOND( network, valid, ( NLOGCLIENT | 0, "Started searching for %s", port.Get() ) );
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::StopSearching()
{
    if( this->isSearching )
    {
        if( this->refSessionClient.isvalid() )
        {
            this->refSessionClient->Release();
        }
        this->isSearching = false;
        NLOG( network, ( NLOGCLIENT | 0, "Stop Searching" ) );
    }
}

//------------------------------------------------------------------------------
/**
    @returns the number of servers founded
*/
int
nNetworkManagerMulti::GetNumServers() const
{
    if( this->refSessionClient.isvalid() )
    {
        return this->refSessionClient->GetNumServers();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @param cads the list of servers
*/
void
nNetworkManagerMulti::GetServers( char ** cads )const
{
    if( ! this->isSearching )
    {
        return;
    }

    n_assert2( cads, "No space to put the servers names" );
    if( cads )
    {
        nSessionServerContext* serverContext;
        for( int i=0 ; i<this->refSessionClient->GetNumServers() ; ++i )
        {
            serverContext = this->refSessionClient->GetServerAt( i );
            cads[i] = strdup( serverContext->GetHostName() );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param hostnum index in the list of servers
    @return true if connect process is correct
*/
bool
nNetworkManagerMulti::Connect( const int hostnum )
{
    if( ! this->isSearching )
    {
        return false;
    }

    // Get Server to join
    nSessionServerContext* serverContext;
    serverContext = this->refSessionClient->GetServerAt( hostnum );
    if( ! serverContext )
    {
        return false;
    }

    NLOG( network, ( NLOGCLIENT | 0, "Connect query to host number %d", hostnum ) );
    return this->ConnectHost( serverContext->GetHostName(), this->searchPort.Get() );
}

//------------------------------------------------------------------------------
/**
    @param host host machine
    @param port port name
    @return true if connect process is correct
*/
bool
nNetworkManagerMulti::ConnectHost( const char * const host, const char * const port )
{
    bool valid = this->clientState == CLIENT_DISCONNECTED;
    n_assert2( valid, "The client is not disconnected" );

    nString newname;

    if( valid )
    {
        newname = this->GetFullName();
        newname.Append( "/netclient" );
        this->refNetClient = (nNetClientEnet*) kernelServer->New( "nnetclientenet", newname.Get() );
        valid = this->refNetClient.isvalid();
        n_assert( valid );
    }

    if( valid )
    {
        this->refNetClient->Open();
        this->refNetClient->SetServerHost( host );
        this->refNetClient->SetServerPort( port );
        this->refNetClient->Connect();
        this->refNetClient->SetHandler( this );
        this->clientState = CLIENT_CONNECTING;
    }

    NLOGCOND( network, valid, ( NLOGCLIENT | 0, "Connecting to host %s:%s", host, port ) );

    if( valid )
    {
        this->StopSearching();
    }

    return valid;
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::Disconnect()
{
    bool valid = this->refNetClient.isvalid();
    n_assert2( valid, "There is not Net Client initialized" );

    if( valid )
    {
        this->refNetClient->Disconnect();
        this->clientState = CLIENT_DISCONNECTING;
        NLOG( network, ( NLOGCLIENT | 0, "Disconnecting" ) );
    }
}

//------------------------------------------------------------------------------
/**
    @returns the client RTT
    @retval 0 if there is not Connected Client
*/
int
nNetworkManagerMulti::GetClientRTT() const
{
    if( this->refNetClient.isvalid() )
    {
        return this->refNetClient->GetRTT();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::Ping() const
{
    bool valid = this->refNetClient.isvalid();
    n_assert2( valid, "The Net Client is not created" );

    if( valid )
    {
        char buffer = char( nNetworkManager::MESSAGE_PING );
        this->refNetClient->SendMessage( &buffer, 1, nNetUtils::Ordered );
        NLOG( network, ( NLOGCLIENT | 2, "Send ping" ) );

        this->protocolSendOverhead.lastBytes += 1;
        ++this->protocolSendOverhead.lastNum;
    }
}

//------------------------------------------------------------------------------
/**
    @param id id of the RPC call
    @param size size of the data
    @param data data of the RPC
*/
void
nNetworkManagerMulti::CallRPCServer( unsigned char id, const int size, const char * const data )
{
    if( this->refNetServer.isvalid() )
    {
        NLOG( network, ( NLOGSERVER | 1, "Client call RPC[%d]", id ) );
        this->SignalOnRPC( this, id, size, data );
    }
    else
    {
        bool valid = this->refNetClient.isvalid();
        n_assert2( valid, "The Net Client is not created" );

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

                this->refNetClient->SendMessage( this->rpcBuffer, 2 + size, nNetUtils::Ordered );
                NLOG( network, ( NLOGCLIENT | 1, "Send to server RPC[%d]", id ) );

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
    @param buffer buffer with the entity update data
    @param size size of the message buffer
*/
size_t
nNetworkManagerMulti::CreateNewEntity( const char * buffer, const int size )
{
    nstream data;
    int id;

    nEntityObjectServer * objectServer = nEntityObjectServer::Instance();

    // create data stream
    data.SetBuffer( size, buffer );
    data.SetWrite( false );

    // check that it is a network entity
    data.UpdateInt( id );

    bool valid = objectServer->GetEntityObjectType( id ) == nEntityObjectServer::Server;

    NLOGCOND( network, ! valid, 
        ( NLOGENTITIES | 1, "Client ERROR: Created Network entity %x without Network ID", id ) );

    nEntityObject * object = 0;

    if( valid )
    {
        // search for already created entity
        object = objectServer->GetEntityObject( id );
        
        valid = ! object;

        NLOGCOND( network, ! valid, ( NLOGENTITIES | 1, "Client ERROR: Entity %x already created", id ) );
    }

    if( valid )
    {
        char entityName[N_MAXNAMELEN];

        data.UpdateString( entityName );

        // create the new entity
        object = objectServer->NewServerEntityObjectWithId( entityName, id );

        valid = object != 0;

        NLOGCOND( network, ! valid, ( NLOGENTITIES | 1, "Client ERROR: Can create entity %x", id ) );
    }

    if( valid )
    {
        nLevelManager::Instance()->GetCurrentLevel()->AddEntity( object );

        ncNetwork * network = object->GetComponentSafe<ncNetwork>();

        if( network )
        {
            network->InitFromNetwork( data );
            NLOG( network, ( NLOGENTITIES | 1, "Client: Created new entity %x ", id ) );
        }
        else
        {
            NLOG( network, ( NLOGENTITIES | 1, "Client ERROR: Created an entity %x without network component", id ) );
        }

        this->SignalOnNewEntity( this, id );
    }

    return data.GetBufferSize();
}

//------------------------------------------------------------------------------
/**
    @param id entity identifier
*/
void
nNetworkManagerMulti::DeleteEntity( int id )
{
    nEntityObjectServer * objectServer = nEntityObjectServer::Instance();

    bool valid = objectServer->GetEntityObjectType( id ) != nEntityObjectServer::Local;

    NLOGCOND( network, ! valid, 
        ( NLOGENTITIES | 1, "Client ERROR: Delete Network entity %x without Network ID", id ) );

    nEntityObject * object = 0;

    if( valid )
    {
        // search for entity
        object = objectServer->GetEntityObject( id );
        
        valid = object != 0;

        NLOGCOND( network, ! valid, 
            ( NLOGENTITIES | 1, "Client ERROR: Entity %x deleted without creation", id ) );
    }

    if( valid )
    {
        this->SignalOnDeleteEntity( this, id );

        NLOG( network, ( NLOGENTITIES | 1, "Client: Entity %x deleted", id ) );
    }
}

//------------------------------------------------------------------------------
/**
    @param buffer buffer with the entity update data
    @param size size of the message buffer
*/
void
nNetworkManagerMulti::UpdateEntities( const char * buffer, const int size )
{
    nstream data;
    int id;
    int entitySize;

    const char * pointer = buffer;
    while( size > pointer - buffer )
    {
        // create data stream
        data.SetExternBuffer( size - int( pointer - buffer ), pointer );
        data.SetWrite( false );

        // get entity object
        data.UpdateInt( id );
        data.UpdateInt( entitySize );
        if( nEntityObjectServer::IDINVALID == id )
        {
            NLOG( network, ( NLOGENTITIES | 1, "Client: invalid entityId. Buffer rejected." ) );
            pointer += entitySize;
            continue;
        }

        nEntityObject * object = nEntityObjectServer::Instance()->GetEntityObject( id );
        if( 0 == object )
        {
            NLOG( network, ( NLOGENTITIES | 1, "Client: network entity %x don't found. Buffer rejected.", id ) );
            pointer += entitySize;
            continue;
        }

        ncNetwork * net = object->GetComponent<ncNetwork>();

        NLOGCOND( network, 0 == net, 
            ( NLOGENTITIES | 2, "Client ERROR: Entity %x update without network component", id ) );

        if( net )
        {
            NLOG( network, ( NLOGENTITIES | 3, "Client: Entity %x update from server", id ) );

            ncGameplay * gameplay = object->GetComponent< ncGameplay >();
            if( gameplay && gameplay->IsRunningAction( ) )
            {
                int headerSize = 2 * sizeof( int );
                net->SetSavedState( pointer + headerSize, entitySize - headerSize );
            }
            else
            {
                net->UpdateNetwork( data );
            }
        }

        pointer += entitySize;
    }

    // dont erase buffer
    data.SetExternBuffer( 0, 0 );
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::HandleDisconnect()
{
    n_assert( this->clientState != CLIENT_DISCONNECTED );

    this->SignalOnDisconnected( this );
    
    NLOG( network, ( NLOGCLIENT | 0, "Disconnected" ) );

    this->clientState = CLIENT_DISCONNECTED;
}

//------------------------------------------------------------------------------
/**
    @param message buffer with the message data
    @param size size of the message buffer
*/
void
nNetworkManagerMulti::HandleMessage( const char * message, const int size )
{
    const char * pointer = 0;
    bool valid = message && ( size > 0 );
    n_assert2( valid, "A Invalid Message in the Network Client" );
    int id;

    if( valid )
    {
        switch( message[ 0 ] )
        {
        case nNetworkManager::MESSAGE_ZSTR:
            NLOG( network, ( NLOGCLIENT | 2, "The server say \"%s\"", message + 1 ) );

            this->protocolRecvOverhead.lastBytes += size;
            ++this->protocolRecvOverhead.lastNum;
            break;

        case nNetworkManager::MESSAGE_PING:
            {
                NLOG( network, ( NLOGCLIENT | 2, "Server ping" ) );
                char buffer = char( nNetworkManager::MESSAGE_PONG );
                this->refNetClient->SendMessage( &buffer, 1, nNetUtils::Ordered );

                this->protocolRecvOverhead.lastBytes += size;
                ++this->protocolRecvOverhead.lastNum;

                this->protocolSendOverhead.lastBytes += 1;
                ++this->protocolSendOverhead.lastNum;
            }
            break;

        case nNetworkManager::MESSAGE_PONG:
            NLOG( network, ( NLOGCLIENT | 2, "Server pong" ) );

            this->protocolRecvOverhead.lastBytes += size;
            ++this->protocolRecvOverhead.lastNum;
            break;

        case nNetworkManager::MESSAGE_ERROR:
            n_message( "FATAL ERROR: %s\n", message + 1 );
            this->protocolRecvOverhead.lastBytes += size;
            ++this->protocolRecvOverhead.lastNum;
            break;

        case nNetworkManager::MESSAGE_CLOCK:
            {
                NLOG( network, ( NLOGCLIENT | 2, "Server clock update" ) );
                nTime serverTime;
                nNetUtils::UnpackFrom( message + 1, serverTime );
                this->clock = serverTime + this->GetClientRTT() / 2000.0f;

                this->protocolRecvOverhead.lastBytes += size;
                ++this->protocolRecvOverhead.lastNum;
            }
            break;

        case nNetworkManager::MESSAGE_RPC:
            {
                char rpcId( message[ 1 ] );
                NLOG( network, ( NLOGCLIENT | 1, "Server call RPC[%d]", rpcId ) );
                
                this->rpcRecv.lastBytes += size;
                ++this->rpcRecv.lastNum;

                this->SignalOnRPC( this, rpcId, size - 2, message + 2 );
            }
            break;

        case nNetworkManager::MESSAGE_ACTION:
            NLOG( network, ( NLOGACTIONS | 3, "Client: action buffer of %d bytes", size ) );
            this->ActionsFromServer( message + 1, size - 1 );

            this->actionsRecv.lastBytes += size;
            ++this->actionsRecv.lastNum;
            break;

        case nNetworkManager::MESSAGE_ENTITY_UPDATE:
            this->UpdateEntities( message + 1, size - 1 );

            this->protocolRecvOverhead.lastBytes += size;
            ++this->protocolRecvOverhead.lastNum;
            break;

        case nNetworkManager::MESSAGE_ENTITY_CREATED:
            this->CreateNewEntity( message + 1, size - 1 );

            this->protocolRecvOverhead.lastBytes += size;
            ++this->protocolRecvOverhead.lastNum;
            break;

        case nNetworkManager::MESSAGE_ENTITY_DELETED:
            pointer = message + 1;
            pointer += nNetUtils::UnpackFrom( pointer, id );
            this->DeleteEntity( id );

            this->protocolRecvOverhead.lastBytes += size;
            ++this->protocolRecvOverhead.lastNum;
            break;

        default:
            n_assert2_always( "Unknow network message type" );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManagerMulti::HandleConnected()
{
    n_assert( this->clientState != CLIENT_CONNECTED );

    this->SignalOnConnected( this );

    NLOG( network, ( NLOGCLIENT | 0, "Connected" ) );

    this->clientState = CLIENT_CONNECTED;
}

//------------------------------------------------------------------------------
