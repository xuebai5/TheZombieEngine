#ifndef N_NETSERVERENET_H
#define N_NETSERVERENET_H
//------------------------------------------------------------------------------
/**
    @class nNetServerEnet
    @ingroup NNetworkEnetContribModule

    A server in a multiplayer session.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nnetworkenet/nnetutils.h"
#include "kernel/nroot.h"
#include "enet/enet.h"

//------------------------------------------------------------------------------
class nNetHandler;

//------------------------------------------------------------------------------
class nNetServerEnet : public nRoot {
public:
    /// Constructor
    nNetServerEnet();
    /// Destructor
    virtual ~nNetServerEnet();
    /// Set the name of the server port
    void SetServerPort( const char *port );
    /// Set the extern handler object
    void SetHandler( nNetHandler *object );
    /// Set the net bandwidth
    static void SetBandwidth( int in, int out );
    /// Get number of clients connected
    int GetNumClients() const;
    /// Open the network session
    bool Open();
    /// Close the network session
    void Close();
    /// Return true if server is open
    bool IsOpen() const;
    /// Per-frame-trigger
    void Trigger();
    /// Set the maximum number of clients allocated for the server
    void SetMaxClients(const unsigned int number);
    /// Send a message to all the clients
    void SendMessageAll(const char* msg, int size, 
        nNetUtils::MessageType type=nNetUtils::Ordered);
    /// Send a message to one client
    void SendMessage(const char* msg, int size, int client, 
        nNetUtils::MessageType type=nNetUtils::Ordered);
    /// Close the connection with one client
    void CloseConnection( int client );
    /// Force close the connection with one client
    void ResetConnection( int client );

private:
    char *serverPort;
    int numClients;
    unsigned int maxClients;
    bool isOpen;
    ENetHost* server;
    nNetHandler *handler;

    enet_uint8 unorderedChannel;

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
nNetServerEnet::SetServerPort( const char *port )
{
    if( serverPort )
    {
        n_delete_array( this->serverPort );
    }
    this->serverPort = strdup( port );
}

//------------------------------------------------------------------------------
/**
    @param object new handler object
*/
inline
void
nNetServerEnet::SetHandler( nNetHandler *object )
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
nNetServerEnet::SetBandwidth( int in, int out )
{
    nNetServerEnet::inBandwidth = in;
    nNetServerEnet::outBandwidth = out;
}

//------------------------------------------------------------------------------
/**
    @retval true if server is open
*/
inline
bool
nNetServerEnet::IsOpen() const
{
    return this->isOpen;
}
//------------------------------------------------------------------------------
/**
    @param number maximun number of clients
*/
inline
void
nNetServerEnet::SetMaxClients( const unsigned int number )
{
    this->maxClients = number;
}
//------------------------------------------------------------------------------
/**
    @returns the number of clients connected
*/
inline
int
nNetServerEnet::GetNumClients() const
{
    return this->numClients;
}

//------------------------------------------------------------------------------
#endif//N_NETSERVERENET_H
