//------------------------------------------------------------------------------
//  nnetserverenet_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkenet/nnetserverenet.h"
#include "nnetworkenet/nnethandler.h"
#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nNetServerEnet, "nroot");

//------------------------------------------------------------------------------
int nNetServerEnet::inBandwidth = 0;
int nNetServerEnet::outBandwidth = 0;

//------------------------------------------------------------------------------
/**
*/
nNetServerEnet::nNetServerEnet() :
    isOpen( false ),
    maxClients( 10 ),
    serverPort( 0 ),
    numClients( 0 ),
    unorderedChannel( nNetUtils::MIN_UNORDERED ),
    handler( 0 ),
    server( 0 ),
    bytesSend( 0 ),
    bytesRecv( 0 ),
    msgSend( 0 ),
    msgRecv( 0 )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nNetServerEnet::~nNetServerEnet()
{
    if( this->isOpen )
    {
        this->Close();
    }
    if( this->serverPort )
    {
        n_delete_array( this->serverPort );
    }
}

//------------------------------------------------------------------------------
/**
    @remarks Create a socket in the localhost address at port obtained from port
    name (with a hash function)
    @retval true if server is open.
*/
bool
nNetServerEnet::Open()
{
    n_assert( !this->isOpen );

    int error_code;
    error_code = enet_initialize();
    n_assert2( error_code == 0, "Error in enet_initialize");

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = static_cast<enet_uint16>( nNetUtils::GetPort( this->serverPort ) );

    this->server = enet_host_create( &address, this->maxClients, 
        nNetServerEnet::inBandwidth, nNetServerEnet::outBandwidth );
    n_assert( this->server != NULL );

    this->isOpen = this->server != NULL;
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
    @remarks Send to the clients a disconnect message and close the network 
    session.
*/
void
nNetServerEnet::Close()
{
    n_assert( this->isOpen );

    if( this->server )
    {
        for( unsigned int pos = 0 ; pos < this->maxClients ; ++pos )
        {
            if( this->server->peers[ pos ].state == ENET_PEER_STATE_CONNECTED )
            {
                enet_peer_disconnect( &this->server->peers[ pos ] );
                --this->numClients;
            }
        }
        enet_host_flush( this->server );
        enet_host_destroy( this->server );
    }
    this->isOpen = false;
}
//------------------------------------------------------------------------------
/**
    @remarks Get events from network and respond to it. The trigger function 
    need to be called to receive the new clients also with zero clients.
*/
void
nNetServerEnet::Trigger()
{
    n_assert( this->isOpen );
    n_assert( this->server );

    ENetEvent event;

    unsigned int val;
    while( enet_host_service( this->server, &event, 0) > 0 )
    {
        switch( event.type )
        {
            case ENET_EVENT_TYPE_CONNECT:
                val = event.peer->address.host;
                n_printf("nNetServerEnet:: EVENT CONNECT from %d.%d.%d.%d:%u[%d]\n", 
                    (val & 0x000000ff),
                    (val & 0x0000ff00) >> 8, 
                    (val & 0x00ff0000) >> 16, 
                    (val & 0xff000000) >> 24, 
                    event.peer->address.port,
                    int(event.peer - this->server->peers));
                ++this->numClients;
                if( handler )
                {
                    event.peer->data = handler->HandleNewClient( 
                        int(event.peer - this->server->peers) );

                    if( ! event.peer->data )
                    {
                        enet_peer_reset( event.peer );
                        --this->numClients;
                    }
                }
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                if( handler )
                {
                    handler->HandleMessage( 
                        (char *)event.packet->data, 
                        static_cast<int>( event.packet->dataLength ), 
                        event.peer->data );
                }
                this->bytesRecv += static_cast<unsigned int>( event.packet->dataLength );
                ++this->msgRecv;
                enet_packet_destroy ( event.packet );
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                val = event.peer->address.host;
                n_printf("nNetServerEnet::EVENT DISCONNECT from %p[%d]\n",
                    event.peer->data,int(event.peer - this->server->peers));
                if( handler )
                {
                    handler->HandleClientDisconnect( event.peer->data );
                }
                event.peer->data = 0;
                --this->numClients;
                break;
            default:
                n_assert2_always("Invalid Server Enet Event");
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param msg buffer of bytes to send
    @param size number of bytes to send
    @param type how to send the message, Unreliable, Unordered, or Ordered
*/
void
nNetServerEnet::SendMessageAll(const char* msg, int size, nNetUtils::MessageType type)
{
    ENetPacket* packet = 0;
    switch( type )
    {
        case nNetUtils::Ordered:
        case nNetUtils::Unordered:
            packet = enet_packet_create( msg, size, ENET_PACKET_FLAG_RELIABLE);
            break;

        case nNetUtils::Unreliable:
            packet = enet_packet_create( msg, size, 0 );
            break;
        default:
            n_assert2_always("Invalid Packet Type");
    }

    switch( type )
    {
        case nNetUtils::Ordered:
            enet_host_broadcast(this->server, nNetUtils::ORDERED_CHANNEL, packet);
            break;

        case nNetUtils::Unordered:
            enet_host_broadcast(this->server, static_cast<enet_uint8>( this->unorderedChannel ), packet);
            ++this->unorderedChannel;
            if( this->unorderedChannel > nNetUtils::MAX_UNORDERED )
            {
                this->unorderedChannel = nNetUtils::MIN_UNORDERED;
            }
            break;

        case nNetUtils::Unreliable:
            enet_host_broadcast(this->server, nNetUtils::UNRELIABLE_CHANNEL, packet);
            break;
        default:
            n_assert2_always("Invalid Packet Type");
    }
    this->bytesSend += size;
    ++this->msgSend;
}

//------------------------------------------------------------------------------
/**
    @param msg buffer of bytes to send
    @param size number of bytes to send
    @param type how to send the message, Unreliable, Unordered, or Ordered
    @param client number of client to send message
*/
void
nNetServerEnet::SendMessage(const char* msg, int size, int client, nNetUtils::MessageType type)
{
    ENetPacket* packet = 0;
    switch( type )
    {
        case nNetUtils::Ordered:
        case nNetUtils::Unordered:
            packet = enet_packet_create( msg, size, ENET_PACKET_FLAG_RELIABLE);
            break;

        case nNetUtils::Unreliable:
            packet = enet_packet_create( msg, size, 0 );
            break;
        default:
            n_assert2_always("Invalid Packet Type");
    }

    switch( type )
    {
        case nNetUtils::Ordered:
            enet_peer_send(this->server->peers + client, nNetUtils::ORDERED_CHANNEL, packet);
            break;

        case nNetUtils::Unordered:
            enet_peer_send(this->server->peers + client, this->unorderedChannel, packet);
            ++this->unorderedChannel;
            if( this->unorderedChannel > nNetUtils::MAX_UNORDERED )
            {
                this->unorderedChannel = nNetUtils::MIN_UNORDERED;
            }
            break;

        case nNetUtils::Unreliable:
            enet_peer_send(this->server->peers + client, nNetUtils::UNRELIABLE_CHANNEL, packet);
            break;
        default:
            n_assert2_always("Invalid Packet Type");
    }
    this->bytesSend += size;
    ++this->msgSend;
}

//------------------------------------------------------------------------------
/**
    @param client number of client to send message
*/
void
nNetServerEnet::CloseConnection( int client )
{
    enet_peer_disconnect( this->server->peers + client );
}

//------------------------------------------------------------------------------
/**
    @param client number of client to send message
*/
void
nNetServerEnet::ResetConnection( int client )
{
    enet_peer_reset( this->server->peers + client );
}

//------------------------------------------------------------------------------
