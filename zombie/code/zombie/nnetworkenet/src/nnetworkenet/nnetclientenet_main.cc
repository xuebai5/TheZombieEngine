//------------------------------------------------------------------------------
//  nnetclientenet_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkenet/nnetclientenet.h"
#include "nnetworkenet/nnethandler.h"
#include "kernel/nkernelserver.h"
#include <climits>

//------------------------------------------------------------------------------
nNebulaScriptClass(nNetClientEnet, "nroot");

//------------------------------------------------------------------------------
int nNetClientEnet::inBandwidth = 0;
int nNetClientEnet::outBandwidth = 0;

//------------------------------------------------------------------------------
/**
*/
nNetClientEnet::nNetClientEnet() :
    isOpen( false ),
    serverPort( 0 ),
    serverHost( 0 ),
    state(nNetClientEnet::Invalid),
    unorderedChannel( nNetUtils::MIN_UNORDERED ),
    handler( 0 ),
    bytesSend( 0 ),
    bytesRecv( 0 ),
    msgSend( 0 ),
    msgRecv( 0 )
{
    this->SetServerHost( "localhost" );
}

//------------------------------------------------------------------------------
/**
*/
nNetClientEnet::~nNetClientEnet()
{
    if( this->isOpen )
    {
        this->Close();
    }
    if( this->serverPort )
    {
        n_delete_array( this->serverPort );
    }
    if( this->serverHost )
    {
        n_delete_array( this->serverHost );
    }
}

//------------------------------------------------------------------------------
/**
    @retval true if client is open
*/
bool
nNetClientEnet::Open()
{
    n_assert( !this->isOpen );

    int error_code;
    error_code = enet_initialize();
    n_assert2( error_code == 0, "Error initialize ENET");

    this->client = enet_host_create( NULL, 1, 
        nNetClientEnet::inBandwidth, nNetClientEnet::outBandwidth );
    n_assert( this->client != NULL );

    this->state = nNetClientEnet::Create;

    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    @remarks the state of client is change to Connecting. When the server 
    accepts the connection the state will change to Connected, or if the server
    rejects the client th state will change to Create.
*/
void
nNetClientEnet::Connect()
{
    if( this->state != nNetClientEnet::Create )
    {
        return;
    }
    ENetAddress address;

    enet_address_set_host( &address, this->serverHost );
    address.port = static_cast<enet_uint16>( nNetUtils::GetPort( this->serverPort ) );

    this->peer = enet_host_connect( this->client, &address, nNetUtils::NUM_CHANNELS );    
    n_assert( this->peer != NULL );
    this->state = nNetClientEnet::Connecting;
}

//------------------------------------------------------------------------------
/**
    @remarks the state of client is change to Closing. When the server 
    reply to close message the state will change to Create. Or when the timeout
    reaches zero.
*/
void
nNetClientEnet::Disconnect()
{
    if( this->state != nNetClientEnet::Connected )
    {
        return;
    }
    enet_peer_disconnect( &this->client->peers[0]);
    this->state = nNetClientEnet::Closing;
}

//------------------------------------------------------------------------------
/**
    @remarks the state of client is change to invalid.
*/
void
nNetClientEnet::Close()
{
    n_assert( this->isOpen );

    if( this->client )
    {
        if( this->state == Connected )
        {
            enet_peer_disconnect( &this->client->peers[0]);
            enet_host_flush( this->client );
        }
        enet_host_destroy( this->client );
        this->state = nNetClientEnet::Invalid;
    }
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    @remarks Get events from network and respond to it.
    no messages are sent to server if Trigger isn't call, even the
    connect and disconnect messages.
*/
void
nNetClientEnet::Trigger()
{
    n_assert( this->isOpen );
    n_assert( this->client );

    ENetEvent event;

    enet_uint32 val;
    while( enet_host_service( this->client, &event, 0) > 0 )
    {
        switch( event.type )
        {
            case ENET_EVENT_TYPE_CONNECT:
                val = event.peer->address.host;
                n_printf("nNetClientEnet:: EVENT CONNECT from %d.%d.%d.%d:%u\n", 
                    (val & 0x000000ff),
                    (val & 0x0000ff00) >> 8, 
                    (val & 0x00ff0000) >> 16, 
                    (val & 0xff000000) >> 24, 
                    event.peer->address.port);
                if( this->state == nNetClientEnet::Connecting )
                {
                    this->state = nNetClientEnet::Connected;
                    event.peer->data = "a server";
                }
                if( handler )
                {
                    handler->HandleConnected();
                }
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                n_assert( event.packet->dataLength <= INT_MAX );
                if( handler )
                {
                    handler->HandleMessage( (char *)event.packet->data, static_cast<int>( event.packet->dataLength ) );
                }
                this->bytesRecv += static_cast<unsigned int>( event.packet->dataLength );
                ++this->msgRecv;
                enet_packet_destroy (event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                val = event.peer->address.host;
                n_printf("nNetClientEnet::EVENT DISCONNECT from %s\n",
                    event.peer->data );
                if( handler )
                {
                    handler->HandleDisconnect();
                }
                event.peer->data = 0;
                this->state = nNetClientEnet::Create;
                break;
            default:
                n_assert2_always( "Invalid Client Enet Event");
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
nNetClientEnet::SendMessage(const char* msg, int size, nNetUtils::MessageType type)
{
    if( this->state != nNetClientEnet::Connected )
    {
        return;
    }

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
            enet_peer_send(this->peer, nNetUtils::ORDERED_CHANNEL, packet);
            break;

        case nNetUtils::Unordered:
            enet_peer_send(this->peer, this->unorderedChannel, packet);
            ++this->unorderedChannel;
            if( this->unorderedChannel > nNetUtils::MAX_UNORDERED )
            {
                this->unorderedChannel = nNetUtils::MIN_UNORDERED;
            }
            break;

        case nNetUtils::Unreliable:
            enet_peer_send(this->peer, nNetUtils::UNRELIABLE_CHANNEL, packet);
            break;
        default:
            n_assert2_always("Invalid Packet Type");
    }
    this->bytesSend += size;
    ++this->msgSend;
}

//------------------------------------------------------------------------------
