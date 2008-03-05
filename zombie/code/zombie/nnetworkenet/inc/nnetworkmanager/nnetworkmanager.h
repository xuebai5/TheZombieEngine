#ifndef NNETWORKMANAGER_H
#define NNETWORKMANAGER_H
//------------------------------------------------------------------------------
/**
    @class nNetworkManager
    @ingroup NNetworkEnetContribModule

    Network Manager base class. This class is a empty manager that cant use
    in singleplayer games.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "entity/nentityobject.h"

#include "nnetworkenet/nnethandler.h"

//------------------------------------------------------------------------------
/**
    @ingroup NNetworkEnetContribModule
    @brief Types of entity role
*/
enum nNetworkRole{
    /** The entity is not relevant in network play. */
    NET_ROLE_NONE = 1,   
    /** The entity is a proxy with an aproximate state of the real one. */
    NET_ROLE_DUMB,       
    /** The entity is a proxy with an aproximate state that should 
        simulate the physics an animations. */
    NET_ROLE_SIMULATED,  
    /** The entity is the local player. Uses prediction of movement                     
        rather than simple simulation. */
    NET_ROLE_AUTONOMOUS, 
    /** all actor in the server machine. */
    NET_ROLE_AUTHORITY,  
    NET_ROLE_LAST
};

//------------------------------------------------------------------------------
class nSessionServer;
class nSessionClient;
class nNetServerEnet;
class nNetClientEnet;
class nClientProxy;
class ncNetwork;

//------------------------------------------------------------------------------
class nNetworkManager : public nRoot, public nNetHandler
{
public:
    /// constructor
    nNetworkManager();
    /// destructor
    virtual ~nNetworkManager();

    /// Get the Singleton Instance of nNetworkManager
    static nNetworkManager * Instance();

    /// frame update of network manager
    virtual void Trigger( nTime time );

    /// Get the list of server located in network
    virtual void GetServers( char ** cads )const;

    /// call a RPC in the server
    virtual void CallRPCServer( unsigned char id, const int size, const char * const data );
    /// call a RPC in a client
    virtual void CallRPCClient( const int client, unsigned char id, const int size, const char * const data );
    /// call a RPC in all clients
    virtual void CallRPCAll( unsigned char id, const int size, const char * const data );
    /// call a RPC in all clients and a server
    virtual void CallRPCAllServer( unsigned char id, const int size, const char * const data );

    /// send a Basic Action to the server
    virtual void SendAction( const char* action, int numarg, nArg* arglist, bool foreground );
    /// send a Queued Basic Action to the server
    virtual void SendQueueAction( const char* action, int numarg, nArg* arglist, bool foreground );

    /// Handle a connect message from server
    virtual void HandleConnected();
    /// Handle message from server
    virtual void HandleMessage( const char *message, const int size );
    /// Handle server disconnect
    virtual void HandleDisconnect();

    /// Handle new client
    virtual void * HandleNewClient( const int i );
    /// Handle message from client
    virtual void  HandleMessage( const char *message, const int size, void *data );
    /// Handle client disconnect
    virtual void  HandleClientDisconnect( void *data );

    /// add a modified entity to trasmit in the next network update
    virtual void AddModifiedEntity( ncNetwork * const entity );
    /// add a new entity to the network manager
    virtual void AddNetworkEntity( ncNetwork * const entity );

    /// say if an entity is a registered network entity
    virtual bool IsRegisteredEntity( nEntityObject * entity )const;
    /// say if an entity id is a valid network id
    bool IsNetworkEntity( const nEntityObjectId entityId )const;

    /// start the network clock
    void StartClock();
    /// stop the network clock
    void StopClock();

    /// @name Script interface
    //@{
        /// say if network manager works like a server
        bool IsServer ()const;
        /// Start searching servers in network
        virtual void StartSearching(const nString &);
        /// Stop searching servers in network
        virtual void StopSearching ();
        /// Get the number of servers founded
        virtual int GetNumServers () const;
        /// Connect with the indicated server in the list of servers
        virtual bool Connect(const int);
        /// Connect with a host:port server
        virtual bool ConnectHost(const char * const, const char * const);
        /// sends a ping message
        virtual void Ping () const;
        /// Disconnect from server
        virtual void Disconnect ();
        /// Get the client Round Trip Time
        virtual int GetClientRTT () const;
        /// Set the maximum number of clients 
        virtual void SetMaxClients(const int);
        /// Set the port to accept clients
        virtual void SetServerPort(const nString &);
        /// Start the server
        virtual void StartServer ();
        /// Stop the server
        virtual void StopServer ();
        /// Kick out a client
        virtual void Kick(const int);
        /// push a basic action int parameter
        void PushIntArg(const int);
        /// push a basic action object parameter
        void PushObjectArg(nObject*);
        /// push a basic action float parameter
        void PushFloatArg(const float);
        /// push a basic action string parameter
        void PushStringArg(const char*);
        /// push a basic action char parameter
        void PushCharArg(const char);
        /// push a basic action bool parameter
        void PushBoolArg(const bool);
        /// Send an action from script
        void SendActionScript(const char *, bool);
        /// Send a queue action from script
        void SendQueueActionScript(const char *, bool);
        /// Get clock network clock
        nTime GetClock() const;
    //@}

    /// @name Signals interface
    //@{
    NSIGNAL_DECLARE( 'LNCL', void, OnNewClient, 1, (int), 0, ());
    NSIGNAL_DECLARE( 'LCON', void, OnConnected, 0, (), 0, () );
    NSIGNAL_DECLARE( 'LDCN', void, OnDisconnected, 0, (), 0, () );
    NSIGNAL_DECLARE( 'LCLD', void, OnClientDisconnected, 1, (int), 0, ());
    NSIGNAL_DECLARE( 'LRPC', void, OnRPC, 3, (char, int, const char*), 0, () );
    NSIGNAL_DECLARE( 'LRPS', void, OnRPCServer, 4, (int, char, int, const char*), 0, () );
    NSIGNAL_DECLARE( 'LNWE', void, OnNewEntity, 1, (int), 0, () );
    NSIGNAL_DECLARE( 'LDLE', void, OnDeleteEntity, 1, (int), 0, () );
    //@}

    static const int NLOGAPPLICATION;

protected:
    static const int NLOGBASIC;
    static const int NLOGCLIENT;
    static const int NLOGSERVER;
    static const int NLOGACTIONS;
    static const int NLOGENTITIES;

    enum MessageType{
        MESSAGE_ZSTR = 1,
        MESSAGE_ERROR,
        MESSAGE_PING,
        MESSAGE_PONG,
        MESSAGE_RPC,
        MESSAGE_ACTION,
        MESSAGE_ENTITY_UPDATE,
        MESSAGE_ENTITY_CREATED,
        MESSAGE_ENTITY_DELETED,
        MESSAGE_CLOCK,
        MESSAGE_LAST
    };

    nTime lastTime;
    nTime clock;

    float deltaTime;

    bool isServer;

private:
    static nNetworkManager * singleton;

    static const int MAX_ACTIONARGS = 20;

    int numActionArgs;
    nArg actionArgs[ MAX_ACTIONARGS ];

    bool updateClock;
};

//------------------------------------------------------------------------------
/**
    @returns true if is a server
*/
inline
bool
nNetworkManager::IsServer() const
{
    return this->isServer;
}

//------------------------------------------------------------------------------
/**
    @param entity entity to quest
    @returns true if entity is registered
*/
inline
bool 
nNetworkManager::IsRegisteredEntity( nEntityObject * /*entity*/ )const
{
    return true;
}

//------------------------------------------------------------------------------
/**
    @return network clock
*/
inline
nTime
nNetworkManager::GetClock() const
{
    return this->clock;
}

//------------------------------------------------------------------------------
#endif//NNETWORKMANAGER_H
