#ifndef N_NNETCLIENTENET_H
#define N_NNETCLIENTENET_H
//------------------------------------------------------------------------------
/**
    @class nNetClientEnet
    @ingroup NNetworkEnetContribModule

    A client in a multiplayer session.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "enet/enet.h"
#include "nnetworkenet/nnetutils.h"

//------------------------------------------------------------------------------
class nNetHandler;

//------------------------------------------------------------------------------
class nNetClientEnet : public nRoot{
public:
    /// State of the client-server connection
    enum State
    {
        /// the client is in invalid state
        Invalid,
        /// the client is valid but not connected with server
        Create,
        /// the client has sent the connect message but no response
        Connecting,
        /// the client has connected with the server
        Connected,
        /// the client has sent the close message but no response
        Closing,
    };

    /// Constructor
    nNetClientEnet();
    /// Destructor
    virtual ~nNetClientEnet();
    /// Set the name of the server port
    void SetServerPort( const char *port );
    /// Set the host of the server
    void SetServerHost( const char *host );
    /// Set the extern handler object
    void SetHandler( nNetHandler *object );
    /// Set the net bandwidth
    static void SetBandwidth( int in, int out );
    /// Open the network client
    bool Open();
    /// Connect with the server
    void Connect();
    /// Disconnect from the server
    void Disconnect();
    /// Close the client server
    void Close();
    /// Return true if server is open
    bool IsOpen() const;
    /// Return the Round Trip Time of the client
    int GetRTT() const;
    /// Return the state of the client
    State ClientState() const;
    /// Per-frame-trigger
    void Trigger();
    /// Send a message to the server
    void SendMessage(const char* msg, int size, 
        nNetUtils::MessageType type=nNetUtils::Ordered);

protected:

private:
    char *serverPort;
    char *serverHost;
    State state;
    bool isOpen;
    enet_uint8 unorderedChannel;
    ENetHost* client;
    ENetPeer* peer;
    nNetHandler *handler;

    static int inBandwidth;
    static int outBandwidth;

    unsigned int bytesSend;
    unsigned int bytesRecv;
    unsigned int msgSend;
    unsigned int msgRecv;
};
//------------------------------------------------------------------------------
/**
    @param port string with the server port name
*/
inline
void
nNetClientEnet::SetServerPort( const char *port )
{
    if( serverPort )
    {
        n_delete_array( this->serverPort );
    }
    this->serverPort = strdup( port );
}

//------------------------------------------------------------------------------
/**
    @param host string with the server host location
*/
inline
void
nNetClientEnet::SetServerHost( const char *host )
{
    if( serverHost )
    {
        n_delete_array( this->serverHost );
    }
    this->serverHost = strdup( host );
}

//------------------------------------------------------------------------------
/**
    @param object new handler object
*/
inline
void
nNetClientEnet::SetHandler( nNetHandler *object )
{
    this->handler = object;
}

//------------------------------------------------------------------------------
/**
    @param in incoming bandwidth
    @param out outgoing bandwidth
*/
inline
void 
nNetClientEnet::SetBandwidth( int in, int out )
{
    nNetClientEnet::inBandwidth = in;
    nNetClientEnet::outBandwidth = out;
}

//------------------------------------------------------------------------------
/**
    @returns the state of the client
*/
inline
nNetClientEnet::State
nNetClientEnet::ClientState() const
{
    return this->state;
}

//------------------------------------------------------------------------------
/**
    @retval true if client is open
*/
inline
bool
nNetClientEnet::IsOpen() const
{
    return this->isOpen;
}
//------------------------------------------------------------------------------
/**
    @returns the Round Trip Time in milliseconds
*/
inline
int 
nNetClientEnet::GetRTT() const
{
    return this->peer->roundTripTime;
}

//------------------------------------------------------------------------------
#endif//N_NNETCLIENTENET_H
