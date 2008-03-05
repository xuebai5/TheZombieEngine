#ifndef N_NETHANDLER_H
#define N_NETHANDLER_H
//------------------------------------------------------------------------------
/**
    @class nNetHandler
    @ingroup NNetworkEnetContribModule

    A virtual class with for create handlers to net events.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
class nNetHandler 
{
public:
    /// Handle message from server
    virtual void   HandleMessage(const char *message, const int size);
    /// Handle connect message
    virtual void   HandleConnected();
    /// handle server disconnect
    virtual void   HandleDisconnect();

    /// handle message from client
    virtual void   HandleMessage(const char *message, const int size, void *data);
    /// handle new client
    virtual void * HandleNewClient( const int i );
    /// handle client disconnect
    virtual void   HandleClientDisconnect( void *data);
};

//------------------------------------------------------------------------------
/**
    @param message buffer with the message data
    @param size size of the message buffer
*/
inline void
nNetHandler::HandleMessage( const char * /*message*/, const int /*size*/ )
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param message buffer with the message data
    @param size size of the message buffer
    @param data data of the client that send the message
*/
inline void
nNetHandler::HandleMessage( const char *message, const int size, void * /*data*/)
{
    this->HandleMessage( message, size );
}

//------------------------------------------------------------------------------
/**
*/
inline void
nNetHandler::HandleConnected()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline void
nNetHandler::HandleDisconnect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param i number of client
    @returns the data associated with the new client
*/
inline void *
nNetHandler::HandleNewClient( const int i)
{
    return reinterpret_cast<void*>( size_t(i) );
}

//------------------------------------------------------------------------------
/**
    @param data data associated with the disconnected client
*/
inline void 
nNetHandler::HandleClientDisconnect( void * /*data*/ )
{
    // empty
}

//------------------------------------------------------------------------------
#endif//N_NETHANDLER_H
