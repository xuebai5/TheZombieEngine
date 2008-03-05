#ifndef NNETWORKMANAGERMULTI_H
#define NNETWORKMANAGERMULTI_H
//------------------------------------------------------------------------------
/**
    @class nNetworkManagerMulti
    @ingroup NNetworkEnetContribModule

    Description of the porpouse of the class

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nnetworkmanager/nnetworkmanager.h"
#include "nnetworkenet/nnetutils.h"

//------------------------------------------------------------------------------
class nClientProxy;

//------------------------------------------------------------------------------
class nNetworkManagerMulti : public nNetworkManager
{
public:
    /// constructor
    nNetworkManagerMulti();
    /// destructor
    virtual ~nNetworkManagerMulti();

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
    /// call a RPC in all clients and the server
    virtual void CallRPCAllServer( unsigned char id, const int size, const char * const data );

    /// send a Basic Action to the server
    virtual void SendAction( const char* action, int numarg, nArg* arglist, bool foreground );
    /// send a Queued Basic Action to the server
    virtual void SendQueueAction( const char* action, int numarg, nArg* arglist, bool foreground );

    /// Start searching servers in network
    virtual void StartSearching( const nString & port );
    /// Stop searching servers in network
    virtual void StopSearching();
    /// Get the number of servers founded
    virtual int  GetNumServers() const;
    /// Connect with the indicated server in the list of servers
    virtual bool Connect( const int hostnum );
    /// Connect with a host:port server
    virtual bool ConnectHost( const char * const host, const char * const port );
    /// sends a ping message
    virtual void Ping() const;
    /// Disconnect from server
    virtual void Disconnect();
    /// Get the client Round Trip Time
    virtual int  GetClientRTT() const;
    /// Set the maximum number of clients 
    virtual void SetMaxClients( const int num );
    /// Set the port to accept clients
    virtual void SetServerPort( const nString & port);
    /// Start the server
    virtual void StartServer();
    /// Stop the server
    virtual void StopServer();
    /// Kick out a client
    virtual void Kick( const int index );

    /// Handle a connect message from server
    virtual void HandleConnected();
    /// Handle message from server
    virtual void HandleMessage(const char *message, const int size);
    /// Handle server disconnect
    virtual void HandleDisconnect();

    /// Handle new client
    virtual void * HandleNewClient( const int i );
    /// Handle message from client
    virtual void  HandleMessage(const char *message, const int size, void *data);
    /// Handle client disconnect
    virtual void  HandleClientDisconnect( void *data);

    /// add a modified entity to trasmit in the next network update
    virtual void AddModifiedEntity( ncNetwork * const entity);
    /// add a new entity to the network manager
    virtual void AddNetworkEntity( ncNetwork * const entity );

    /// say if an entity is a registered network entity
    virtual bool IsRegisteredEntity( nEntityObject * entity )const;

    /// receiver of entity object server signal for a deleted entity
    void EntityDeleted( int id );

private:
    enum ClientStatus{
        CLIENT_DISCONNECTED,
        CLIENT_CONNECTING,
        CLIENT_CONNECTED,
        CLIENT_DISCONNECTING
    };

    enum BasicActionFlags{
        // the first 5 bits are for number of args
        BAF_NUMARGS    = (0x1f),

        BAF_FOREGROUND = (1<<5),
        BAF_QUEUE      = (1<<6),
        BAF_LAST
    };

    /// delete the clients proxies
    void DeleteClients();

    /// send a Basic Action to the server
    void SendBasicAction( const char* action, int numarg, nArg* arglist, bool foreground, bool queue );
    /// unpack a basic action from a message bufer
    size_t UnpackAction( const char * message, nEntityObject * & entity, const char * &action, int & numargs, bool & foreground, bool & queue );
    /// the server gets a basic action from a client
    void ActionsFromClient( const char * message, const int size );
    /// the client recieve a basic action from server
    void ActionsFromServer( const char * message, const int size );
    /// get the basic action init command
    nCmd * GetBasicActionInit( const char * action )const;
    /// send an action buffer
    void SendActionBuffer( const char * buffer, int & size, const nNetUtils::MessageType type );

    /// add level entities that are network ones
    void AddNetworkEntities();
    /// create a new entity in the client
    size_t CreateNewEntity( const char * message, const int size );
    /// delete a network entity
    void DeleteEntity( int id );
    /// Send already created entities
    void SendNetworkEntities( int client );
    /// Send the actual state of entities
    void SendActualFrame();
    /// update a entity with a network package
    void UpdateEntities( const char * buffer, const int size );

    /// update the clock from server to clients
    void SendClockTime();

    static const int MAX_ACTION_BUFFER = 200;
    static const int MAX_RPC_BUFFER = 200;
    static const int MAX_NUM_PARAMS = 20;

    static const int MAX_ENTITY_UPDATE = 250;
    static const int MAX_ACTION_SIZE = 200;
    static const int MTU_SIZE = 1100;
    static const int MTU_BUFFER_SIZE = MTU_SIZE + MAX_ENTITY_UPDATE;
    static const int MTU_ACTIONS_SIZE = MTU_SIZE + MAX_ACTION_SIZE;

    static const float TIME_ENTITY_UPDATE;
    static const float TIME_CLOCK_UPDATE;

    nRef<nSessionClient> refSessionClient;
    nRef<nSessionServer> refSessionServer;
    nRef<nNetClientEnet> refNetClient;
    nRef<nNetServerEnet> refNetServer;

    nArray<nClientProxy*> clients;

    nString serverPort;
    nString searchPort;
    ClientStatus clientState;
    int maxClients;
    float updateEntityTime;
    float updateClockTime;
    int maxBlocksSended;

    char entityUpdateBuffer[ MTU_BUFFER_SIZE ];
    char actionOrderedBuffer[ MTU_ACTIONS_SIZE ];
    char actionUnreliableBuffer[ MTU_ACTIONS_SIZE ];
    int actionOrderedIndex;
    int actionUnreliableIndex;

    char rpcBuffer[ MAX_RPC_BUFFER ];
    nArg argsArray[ MAX_NUM_PARAMS ];

    nArray<ncNetwork*> modifiedEntities;
    nArray<ncNetwork*> networkEntities;

    bool isServerStarted;
    bool isSearching;

    // statistics
    struct NetStatistic
    {
        NetStatistic() : totalBytes( 0 ), totalNum( 0 ), lastBytes( 0 ), lastNum( 0 ),
            maxBytes( 0 ), maxNum( 0 ), minBytes( INT_MAX ), minNum( INT_MAX ),
            activeTicks( 0 ), activeSeconds( 0 ) { /*empty*/ }

        /// calculate bytes per tick
        float BytesPerTick() const;
        /// calculate bytes per second
        float BytesPerSecond() const;
        /// update statistic
        void Update( const float deltatime );
        /// show resume
        void Resume( const char * msg )const;

        int totalBytes;
        int totalNum;
        int lastBytes;
        int lastNum;
        int maxBytes;
        int maxNum;
        int minBytes;
        int minNum;
        int activeTicks;
        nTime activeSeconds;
    };

    nWatched bytesActionsSended;
    nWatched bytesActionsReceived;
    nWatched bytesEntitiesUpdated;

    mutable NetStatistic actionsSended;
    mutable NetStatistic actionsRecv;
    mutable NetStatistic rpcSended;
    mutable NetStatistic rpcRecv;
    mutable NetStatistic entitiesUpdated;
    mutable NetStatistic protocolSendOverhead;
    mutable NetStatistic protocolRecvOverhead;

    unsigned int longestAction;
    unsigned int longestRPC;
    unsigned int longestUpdateEntity;
};

//------------------------------------------------------------------------------
/**
    @param action basic action name
    @param numarg number of input arguments of the basic action
    @param arglist input arguments
    @param foreground if the action is foreground action or background action
*/
inline
void
nNetworkManagerMulti::SendAction( const char * action, int numarg, nArg * arglist,  bool foreground )
{
    this->SendBasicAction( action, numarg, arglist, foreground, false );
}

//------------------------------------------------------------------------------
/**
    @param action basic action name
    @param numarg number of input arguments of the basic action
    @param arglist input arguments
    @param foreground if the action is foreground action or background action
*/
inline
void
nNetworkManagerMulti::SendQueueAction( const char * action, int numarg, nArg * arglist,  bool foreground )
{
    this->SendBasicAction( action, numarg, arglist, foreground, true );
}

//------------------------------------------------------------------------------
/**
    returns bytes per tick
*/
inline
float
nNetworkManagerMulti::NetStatistic::BytesPerTick() const
{
    if( this->activeTicks > 0 )
    {
        return ( float( this->totalBytes ) / this->activeTicks );
    }
    return 0.0f;
}

//------------------------------------------------------------------------------
/**
    returns bytes per second
*/
inline
float
nNetworkManagerMulti::NetStatistic::BytesPerSecond() const
{
    if( this->activeSeconds > 0 )
    {
        return float( this->totalBytes / this->activeSeconds );
    }
    return 0.0f;
}

//------------------------------------------------------------------------------
#endif//NNETWORKMANAGERMULTI_H
