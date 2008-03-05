//------------------------------------------------------------------------------
//  nnetworkmanager_srvr.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "ngpactionmanager/ngpactionmanager.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
    @param clients number of maximun clients
*/
void
nNetworkManager::SetMaxClients( const int /*clients*/ )
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param port a name for the server port
*/
void
nNetworkManager::SetServerPort( const nString & /*port*/ )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManager::StartServer()
{
    NLOG( network, ( NLOGSERVER | 0, "Started" ) );

    this->StartClock();
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManager::StopServer()
{
    NLOG( network, ( NLOGSERVER | 0, "Stoped" ) );

    this->StopClock();
}

//------------------------------------------------------------------------------
/**
    @param index client index
*/
void
nNetworkManager::Kick( const int /*index*/ )
{
    n_assert2_always( "The Single Network Manager can Kick nobody" );
}

//------------------------------------------------------------------------------
/**
    @param index index of the client
    @param id id of the RPC call
    @param size size of the data
    @param data data of the RPC
*/
void
nNetworkManager::CallRPCClient( const int /*index*/, unsigned char id, const int size, const char * const data )
{
    NLOG( network, ( NLOGCLIENT | 1, "The server call RPC[%d]", id ) );

    this->SignalOnRPC( this, id, size, data );
}

//------------------------------------------------------------------------------
/**
    @param id id of the RPC call
    @param size size of the data
    @param data data of the RPC
*/
void
nNetworkManager::CallRPCAll( unsigned char id, const int size, const char * const data )
{
    NLOG( network, ( NLOGCLIENT | 1, "The server call RPC[%d]", id ) );
                
    this->SignalOnRPC( this, id, size, data );
}

//------------------------------------------------------------------------------
/**
    @param id id of the RPC call
    @param size size of the data
    @param data data of the RPC
*/
void
nNetworkManager::CallRPCAllServer( unsigned char id, const int size, const char * const data )
{
    NLOG( network, ( NLOGCLIENT | 1, "The server call RPC[%d]", id ) );
                
    this->SignalOnRPC( this, id, size, data );
}

//------------------------------------------------------------------------------
/**
    @param message data of the message
    @param size size of the message payload
    @param data pointer to a client struct
*/
void
nNetworkManager::HandleMessage( const char * /*message*/, const int /*size*/, void * /*data*/ )
{
    n_assert2_always( "Handle Message can't be called in Single Network Manager" );
}

//------------------------------------------------------------------------------
/**
    @param num number of the new client
    @returns pointer to a client struct
    @retval 0 if we do not accept the new client
*/
void *
nNetworkManager::HandleNewClient( const int /*num*/ )
{
    n_assert2_always( "Handle New Client can't be called in Single Network Manager" );
    return 0;
}

//------------------------------------------------------------------------------
/**
    @param data pointer to a client struct
*/
void
nNetworkManager::HandleClientDisconnect( void * /*data*/ )
{
    n_assert2_always( "Handle Client Disconnect can't be called in Single Network Manager" );
}

//------------------------------------------------------------------------------
