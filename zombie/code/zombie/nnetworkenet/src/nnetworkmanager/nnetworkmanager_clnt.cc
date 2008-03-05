//------------------------------------------------------------------------------
//  nnetworkmanager_clnt.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnetworkenet.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "ngpactionmanager/ngpactionmanager.h"

#include "kernel/nlogclass.h"

#ifdef WIN32
#include "windows.h"
#endif//WIN32

//------------------------------------------------------------------------------
/**
    @param port in that port number the server would be listening
*/
void
nNetworkManager::StartSearching( const nString & port )
{
    NLOG( network, ( NLOGCLIENT | 0, "Started searching for %s", port.Get() ) );
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManager::StopSearching()
{
    NLOG( network, ( NLOGCLIENT | 0, "Stop Searching" ) );
}

//------------------------------------------------------------------------------
/**
    @returns the number of servers founded
*/
int
nNetworkManager::GetNumServers() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    @param cads the list of servers
*/
void
nNetworkManager::GetServers( char ** /*cads*/ )const
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param hostnum index in the list of servers
    @return true if connect process is correct
*/
bool
nNetworkManager::Connect( const int hostnum )
{
    NLOG( network, ( NLOGCLIENT | 0, "Connect to host number %d", hostnum ) );
    return true;
}

//------------------------------------------------------------------------------
/**
    @param host host machine
    @param port port name
    @return true if connect process is correct
*/
bool
nNetworkManager::ConnectHost( const char * const host, const char * const port )
{
    NLOG( network, ( NLOGCLIENT | 0, "Connect to host %s:%s", host, port ) );
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManager::Disconnect()
{
    NLOG( network, ( NLOGCLIENT | 0, "Disconnect" ) );
}

//------------------------------------------------------------------------------
/**
    @returns the client RTT
    @retval 0 if there is not Connected Client
*/
int
nNetworkManager::GetClientRTT() const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManager::Ping() const
{
    NLOG( network, ( NLOGCLIENT | 2, "The server pong" ) );
}

//------------------------------------------------------------------------------
/**
    @param id id of the RPC call
    @param size size of the data
    @param data data of the RPC
*/
void
nNetworkManager::CallRPCServer( unsigned char id, const int size, const char * const data )
{
    NLOG( network, ( NLOGSERVER | 1, "Client call RPC[%d]", id ) );
                
    this->SignalOnRPC( this, id, size, data );
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManager::HandleDisconnect()
{
    n_assert2_always( "Handle Disconnect can't be called in Single Network Manager" );
}

//------------------------------------------------------------------------------
/**
    @param message buffer with the message data
    @param size size of the message buffer
*/
void
nNetworkManager::HandleMessage( const char * /*message*/, const int /*size*/ )
{
    n_assert2_always( "Handle Disconnect can't be called in Single Network Manager" );
}

//------------------------------------------------------------------------------
/**
*/
void
nNetworkManager::HandleConnected()
{
    n_assert2_always( "Handle Connected can't be called in Single Network Manager" );
}

//------------------------------------------------------------------------------
