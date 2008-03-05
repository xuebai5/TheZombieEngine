//-----------------------------------------------------------------------------
//  ngamemessagewindowproxy_main.cc
//  (C) 2006 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchgameplay.h"
#include "gameplay/ngamemessagewindowproxy.h"
#include "nnetworkmanager/nnetworkmanager.h"
#include "util/nstream.h"
//-----------------------------------------------------------------------------
nNebulaScriptClass(nGameMessageWindowProxy, "nroot");

//------------------------------------------------------------------------------
NSIGNAL_DEFINE(nGameMessageWindowProxy, OnGameMessage);

//------------------------------------------------------------------------------
nGameMessageWindowProxy* nGameMessageWindowProxy::instance = 0;

//-----------------------------------------------------------------------------
namespace
{
    // Maximum allowed length of a game message
    // Messages longer than that are truncated
    const int MaxMessageLength( 250 );
}

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nGameMessageWindowProxy::nGameMessageWindowProxy() :
    getClientCallback( NULL ),
    isDisplayingMessage( false )
{
    if ( !nGameMessageWindowProxy::instance )
    {
        // Initialize instance pointer
        nGameMessageWindowProxy::instance = this;
    }
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nGameMessageWindowProxy::~nGameMessageWindowProxy()
{
    if ( nGameMessageWindowProxy::instance == this )
    {
        nGameMessageWindowProxy::instance = 0;
    }

    this->Reset();
}

//-----------------------------------------------------------------------------
/**
    Return to initial state, just after construction
*/
void
nGameMessageWindowProxy::Reset()
{
    this->RegisterGetClientCallback( NULL );
    this->ClearMessageQueue();
    this->isDisplayingMessage = false;
}

//-----------------------------------------------------------------------------
/**
    Remove all messages waiting to be displayed
*/
void
nGameMessageWindowProxy::ClearMessageQueue()
{
    nNode* msgInfo;
    while ( NULL != ( msgInfo = this->messageQueue.RemHead() ) )
    {
        n_delete( msgInfo );
    }
}

//-----------------------------------------------------------------------------
/**
    Register the callback used to know which client own each player
*/
void
nGameMessageWindowProxy::RegisterGetClientCallback( GetClientOfPlayer callback )
{
    this->getClientCallback = callback;
}

//-----------------------------------------------------------------------------
/**
    Send a game text message to one or all players

    If a player is specified, the message is delivered only to the client of
    that player, otherwise it's broadcasted to all clients.

    @param player The message is send to this player. If NULL the message is broadcasted
    @param message Text to display
    @param time Time that the message should be displayed
    @param wait True to wait for previous message to expire before displaying this one, false to replace them all
*/
void
nGameMessageWindowProxy::SendGameMessage( nEntityObject* player, const char* message, int time, bool wait )
{
    n_assert( message );

    // Format the message
    char formattedMsg[ MaxMessageLength + 1 ];
    char* t( formattedMsg );
    const char* s( message );
    for ( int i(0); *s != '\0' && i < MaxMessageLength; ++s, ++t )
    {
        if ( *s == '\\' )
        {
            ++s;
            if ( *s == 'n' )
            {
                *t = '\n';
            }
            else
            {
                *t = *s;
            }
        }
        else
        {
            *t = *s;
        }
    }
    *t = '\0';

    // Pack the message
    nstream data;
    data.SetWrite( true );
    data.UpdateString( formattedMsg );
    data.UpdateInt( time );
    data.UpdateBool( wait );

    // Send the message
    if ( nNetworkManager::Instance() && nNetworkManager::Instance()->IsServer() )
    {
        if ( player )
        {
            // Send the message to the client owning the specified player
            if ( this->getClientCallback )
            {
                int client;
                bool isServer;
                if ( this->getClientCallback( player, client, isServer ) )
                {
                    if (!isServer)
                    {
                        nNetworkManager::Instance()->CallRPCClient(
                            client, nRnsEntityManager::SHOW_GAME_MESSAGE,
                            data.GetBufferSize(), data.GetBuffer() );
                    }
                    else
                    {
                        this->ReceiveGameMessage( &data );
                    }
                }
            }
        }
        else
        {
            // Send the message to all the players
            nNetworkManager::Instance()->CallRPCAllServer(
                nRnsEntityManager::SHOW_GAME_MESSAGE,
                data.GetBufferSize(), data.GetBuffer() );
        }
    }
    else
    {
        // This isn't server side, just display the message locally
        this->ReceiveGameMessage( &data );
    }
}

//-----------------------------------------------------------------------------
/**
    Receiver method for a game text message sent over the net

    It signals the reception of a message
*/
void
nGameMessageWindowProxy::ReceiveGameMessage( nstream* data )
{
    n_assert( data );

    // Unpack the message
    char message[ MaxMessageLength + 1 ];
    data->SetWrite( false );
    data->UpdateString( message );
    int time;
    data->UpdateInt( time );
    bool wait;
    data->UpdateBool( wait );

    // Show/queue the message
    if ( wait )
    {
        this->QueueGameMessage( message, time );
    }
    else
    {
        this->ReplaceAllGameMessages( message, time );
    }
}

//-----------------------------------------------------------------------------
/**
    Queue a message to wait for previous messages to expire
*/
void
nGameMessageWindowProxy::QueueGameMessage( const char* message, int displayTime )
{
    if ( this->isDisplayingMessage )
    {
        // Wait for previous messages to expire
        MessageInfo* msgInfo( n_new( MessageInfo ) );
        msgInfo->message = message;
        msgInfo->displayTime = displayTime;
        this->messageQueue.AddTail( msgInfo );
    }
    else
    {
        // There isn't any previous message, show this one right now
        this->ShowGameMessage( message, displayTime );
    }
}

//-----------------------------------------------------------------------------
/**
    Replace any previous displayed or waiting message for the given one
*/
void
nGameMessageWindowProxy::ReplaceAllGameMessages( const char* message, int displayTime )
{
    this->ClearMessageQueue();
    this->ShowGameMessage( message, displayTime );
}

//-----------------------------------------------------------------------------
/**
    Replace the currently displayed message
*/
void
nGameMessageWindowProxy::ShowGameMessage( const char* message, int displayTime )
{
    this->isDisplayingMessage = message != NULL;
    this->currentMessageExpireTime = nTimeServer::Instance()->GetTime() + displayTime;
    if ( message )
    {
        this->SignalOnGameMessage( this, message );
    }
    else
    {
        this->SignalOnGameMessage( this, "" );
    }
}

//-----------------------------------------------------------------------------
/**
    Update message displayed and queue (should be called from client side)
*/
void
nGameMessageWindowProxy::Trigger()
{
    // Replace the currently display message if its display time has expired
    double currentTime( nTimeServer::Instance()->GetTime() );
    if ( this->isDisplayingMessage && currentTime >= this->currentMessageExpireTime )
    {
        // Get next message to display, if there's any
        const char* message( NULL );
        int displayTime( 0 );
        MessageInfo* msgInfo( static_cast< MessageInfo* >( this->messageQueue.RemHead() ) );
        if ( msgInfo )
        {
            message = msgInfo->message.Get();
            displayTime = msgInfo->displayTime;
            this->currentMessageExpireTime = currentTime + msgInfo->displayTime;
        }

        // Replace currently displayed message
        this->ShowGameMessage( message, displayTime );

        // Destroy message copy
        if ( msgInfo )
        {
            n_delete( msgInfo );
        }
    }
}

/**
    Clear message window locally
*/
void
nGameMessageWindowProxy::ClearMessageWindow()
{
    this->ShowGameMessage("", 1);
}
