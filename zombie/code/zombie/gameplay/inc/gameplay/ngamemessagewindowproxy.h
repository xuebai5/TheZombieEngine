#ifndef N_GAMEMESSAGEWINDOWPROXY_H
#define N_GAMEMESSAGEWINDOWPROXY_H

//------------------------------------------------------------------------------
/**
    @class nGameMessageWindowProxy
    @ingroup Gameplay

    Interface to handle from the server the client's game text message window.

    This proxy is decoupled from any client's window implementation: it sends
    messages to itself over the net to its instance in the client side and
    there it throws a 'message received' signal. Any client desiring to receive
    messages should listen to the signal OnGameMessage thrown by this class.

    This class implements a message queue. A message can be sent to wait for
    previous messages to expire or to replace them all. Each time a message
    should be displayed the OnGameMessage signal is thrown. This signal is
    also thrown with NULL as message when the currently displayed message has
    expired and there aren't anymore messages waiting.

    Game messages have the following parameters:

     - Text message to be displayed (printf style codes '\n' and '\\' are allowed)
     - Time that the message should be displayed
     - Replace/wait mode (if the new message should replace any other previous
       message or wait for them to expire)

    On the server side it's required to register a callback to know which client
    owns each player. Also, on the client side the Trigger method should be
    called periodically.
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"

class nstream;

//------------------------------------------------------------------------------
class nGameMessageWindowProxy : public nRoot
{
public:
    /**
        Callback type to get the client owning a player

        Given a player entity, the callback should give the client id as
        understood by the network manager. The callback should return true
        if a client for the player was found, false otherwise.
    */
    typedef bool (*GetClientOfPlayer)( nEntityObject*, int&, bool& );

    /// Default constructor
    nGameMessageWindowProxy();
    /// Destructor
    ~nGameMessageWindowProxy();
    /// Return to initial state, just after construction
    void Reset();

    /// Return singleton message window proxy
    static nGameMessageWindowProxy* Instance();

    /// Register the callback used to know which client own each player
    void RegisterGetClientCallback( GetClientOfPlayer callback );

    /// Receiver method for a game text message sent over the net
    void ReceiveGameMessage( nstream* data );

    /// Update message displayed and queue (should be called from client side)
    void Trigger();

    /// Send a game text message to one or all players
    void SendGameMessage(nEntityObject*, const char*, int, bool);

    NSIGNAL_DECLARE( 'EOSC', void, OnGameMessage, 1, (const char*), 0, () );

    /// clear message window locally
    void ClearMessageWindow();

private:
    /// Remove all messages waiting to be displayed
    void ClearMessageQueue();
    /// Queue a message to wait for previous messages to expire
    void QueueGameMessage( const char* message, int displayTime );
    /// Replace any previous displayed or waiting message for the given one
    void ReplaceAllGameMessages( const char* message, int displayTime );
    /// Replace the currently displayed message
    void ShowGameMessage( const char* message, int displayTime );

    /// Singleton instance
    static nGameMessageWindowProxy* instance;

    /// Callback used to know which client owns each player
    GetClientOfPlayer getClientCallback;

    /// Struct to store the data related to a message
    struct MessageInfo : public nNode
    {
        nString message;
        int displayTime;
    };

    /// Queue of messages waiting to be displayed
    nList messageQueue;

    /// Is a message currently being displayed?
    bool isDisplayingMessage;

    /// Time when the currently displayed message expires
    double currentMessageExpireTime;
};

//------------------------------------------------------------------------------
/**
    Return singleton message window proxy
*/
inline
nGameMessageWindowProxy*
nGameMessageWindowProxy::Instance()
{
    n_assert( nGameMessageWindowProxy::instance );
    return nGameMessageWindowProxy::instance;
}

//------------------------------------------------------------------------------
#endif // N_GAMEMESSAGEWINDOWPROXY_H
