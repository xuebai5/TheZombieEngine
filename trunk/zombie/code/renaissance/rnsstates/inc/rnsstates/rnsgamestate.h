#ifndef RNSGAMESTATE_H
#define RNSGAMESTATE_H
//------------------------------------------------------------------------------
/**
    @class RnsGameState
    @ingroup RnsStates
    
    The Game State class of Renaissance Game
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ncommonapp/ncommonstate.h"
#include "mathlib/vector.h"
#include "mathlib/polar.h"
#include "kernel/ncmdprotonativecpp.h"
#include "rnsgameplay/nrnsentitymanager.h"
#include "kernel/nprofiler.h"

//------------------------------------------------------------------------------
class nAppViewport;
class nGPActionManager;
class nNetworkManager;
class ncGameCamera;
class nFxEventTrigger;
class nFXObject;
class ncGameplayPlayer;

//------------------------------------------------------------------------------
class RnsGameState : public nCommonState
{
public:
    enum CrossModeType
    {
        CMT_NEVER    = 0,
        CMT_NORMAL   = 1,
        CMT_ALWAYS   = 2,

        CMT_LAST
    };

    /// Constructor
    RnsGameState();
    /// Destructor
    virtual ~RnsGameState();

    /// Called when state is created
    virtual void OnCreate( nApplication* application );

    /// Called when state is becoming active
    virtual void OnStateEnter( const nString & prevState );
    /// Called when state is becoming inactive
    virtual void OnStateLeave( const nString & nextState );
    /// Called on state to perform state logic 
    virtual void OnFrame();
    /// Called on state to perform 3d rendering
    virtual void OnRender3D();
    /// Called on state to perform 2d rendering
    virtual void OnRender2D();

    /// sets the previous viewport
    void SetPreviousViewport( nAppViewport* prevViewport );

    /// Init and start up the required systems and entities to turn on AI
    void StartAI();
    /// Restore and shut down the required systems and entities to turn off AI
    void StopAI();
    /// Do an AI update cycle (may be more or less cycles per update if we're in accelerated time)
    void UpdateAI();

        /// get mouse x factor
        float GetMouseXFactor ()const;
        /// set mouse x factor
        void SetMouseXFactor(float);
        /// get mouse y factor
        float GetMouseYFactor ()const;
        /// set mouse y factor
        void SetMouseYFactor(float);
        /// get mouse y invert
        bool GetMouseYInvert ()const;
        /// set mouse y invert
        void SetMouseYInvert(bool);
        /// save mouse settings to file
        void SaveMouseSettings(const char *);
        /// load mouse settings from file
        void LoadMouseSettings(const char *);
        /// set the cross mode draw
        void SetCrossMode(int);
        /// get the cross mode draw
        int GetCrossMode ()const;
        /// set the full reload time
        void SetFullReloadTime(float);
        /// get the full reload time
        float GetFullReloadTime ()const;
        /// set the next state before this
        void SetNextState( const nString & stateName );
        /// Exit from Game State
        void Exit();
        /// set the connection data
        void InitConnectionWith( const nString & host, const nString & port );

    // receiver of network signal OnNewClientConnected
    void NewClientConnected( int i );
    // receiver of network signal OnClientDisconnected
    void ClientDisconnected( int i );
    // receiver of network signal OnConnected
    void Connected();
    // receiver of network signal OnDisconnected
    void Disconnected();
    // receiver of network signal OnRPCFromServer
    void RPCFromServer( char id, int size, const char * data );
    // receiver of network signal OnRPCFromClient
    void RPCFromClient( int client, char id, int size, const char * data );
    // receiver of Entity Manager signal EntityEliminated
    void GameEntityEliminated( nEntityObject * );

    /// Get the client that owns a player
    bool GetClientOfPlayer( nEntityObject* player, int& client, bool& isServer ) const;

private:
    static const int MAX_PLAYERS = 32 + 1;
    static const int SERVER_PLAYER = MAX_PLAYERS - 1;

    struct PlayerInfo{
        nEntityObject * clientEntity;
    };

    /// initialize the network
    void InitNetwork();
    /// close the network
    void EndNetwork();

    /// Get spawn path name for client
    void SpawnPathForClient( int client, nString& path );
    // Set the correct position for a player spawn
    void SetPlayerSpawnPosition( int client, nRnsEntityManager::PlayerData& data );

    /// spawn a player for a client
    void SpawnPlayer( int client, nRnsEntityManager::PlayerData & data );
    /// set the local player
    void SetLocalPlayer( int entityId );

    /// draw the  Heads-Up Display
    void DrawHUD();
    /// update the camera position
    void UpdateCamera( nTime time );

    /// Handle general input
    void HandleInput( const float frameTime );
    /// Handle player input
    void PlayerInput( const float frameTime );
    /// Handle free camera input
    void FreeCameraInput( const float frameTime );
    /// Players vehicle input
    void PlayerVehicleInput( const float frameTime );
    /// player input of movement
    void PlayerMoveInput( const ncGameplayPlayer * player );
    /// player input for weapon fire
    void PlayerWeaponFireInput( const float frameTime, const ncGameplayPlayer * player );
    /// player input for normal state
    void PlayerNormalInput( const ncGameplayPlayer * player );

    /// change the camera anchor
    void ChangeCameraAnchor( nEntityObject * entity );

    /// Get an item index from player inventory
    int GetLastInventoryItem()const;

    /// sets a new player state
    void NewPlayerState( int newstate, nEntityObject* player );

    PlayerInfo * players[ MAX_PLAYERS ];

    nArg inputArgs[ 12 ];

    nRef<nAppViewport> refViewport;
    nRef<nAppViewport> refPrevViewport;

    ncGameCamera* gameCamera;

    nRef<nRnsEntityManager> entityManager;

#ifdef __NEBULA_STATS__
    nProfiler profGameplay;
    nProfiler profNetwork;
    nProfiler profInput;
#endif

    nGPActionManager * actionManager;
    nNetworkManager * network;

    nFXObject * scopeFx;

    nString savedState;
    bool useSavedState;

    bool initClient;
    nString hostName;
    nString hostPort;

    polar2 viewerAngles;
    vector3 viewerPos;

    float mouseXfactor;
    float mouseYfactor;

    float cameraVelocity;

    float reloadInputTime;
    float fullReloadTime;

    bool isStarted;
    bool aiStarted;

    bool showPhysics;

    bool showMenu;

    int lastMoveForward;
    int lastMoveStep;
    int lastMoveFlags;
    float lastLookYaw;
    float lastLookPitch;
    bool lastWeaponTrigger;
    bool lastWeaponPressed;

    // vars to draw the crosshair
    static const float crossStick;
    static const float crossSightZ;
    vector3 crossList[8];
    int crossMode;

    nRef<nFxEventTrigger> refFxEventTrigger;

#ifndef NGAME
    // switches on/off artist mode
    void SwitchArtistMode();
#endif
};

//-----------------------------------------------------------------------------
/**
    @param prevViewport viewport in previous state
*/
inline
void 
RnsGameState::SetPreviousViewport( nAppViewport* prevViewport )
{
    this->refPrevViewport = prevViewport;
}

//-----------------------------------------------------------------------------
/**
    @param value new mouse x factor
*/
inline
void
RnsGameState::SetMouseXFactor( float value )
{
    this->mouseXfactor = n_sgn( this->mouseXfactor ) * value;
}

//-----------------------------------------------------------------------------
/**
    @returns the mouse x factor
*/
inline
float
RnsGameState::GetMouseXFactor() const
{
    return n_abs( this->mouseXfactor );
}

//-----------------------------------------------------------------------------
/**
    @param value new mouse y factor
*/
inline
void
RnsGameState::SetMouseYFactor( float value )
{
    this->mouseYfactor = n_sgn( this->mouseYfactor ) * value;
}

//-----------------------------------------------------------------------------
/**
    @returns the mouse y factor
*/
inline
float
RnsGameState::GetMouseYFactor() const
{
    return n_abs( this->mouseYfactor );
}

//-----------------------------------------------------------------------------
/**
    @param value invert value
*/
inline
void
RnsGameState::SetMouseYInvert( bool value )
{
    this->mouseYfactor = n_abs( this->mouseYfactor );
    if( value )
    {
        this->mouseYfactor *= -1.0f;
    }
}

//-----------------------------------------------------------------------------
/**
    @returns the mouse y invert flag
*/
inline
bool
RnsGameState::GetMouseYInvert() const
{
    if( this->mouseYfactor < 0.0f )
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    @param value new cross mode
*/
inline
void
RnsGameState::SetCrossMode( int value )
{
    this->crossMode = value;
}

//-----------------------------------------------------------------------------
/**
    @returns cross mode
*/
inline
int
RnsGameState::GetCrossMode() const
{
    return this->crossMode;
}

//-----------------------------------------------------------------------------
/**
    @param value new full reload time
*/
inline
void
RnsGameState::SetFullReloadTime( float value )
{
    this->fullReloadTime = value;
}

//-----------------------------------------------------------------------------
/**
    @returns full reload time
*/
inline
float
RnsGameState::GetFullReloadTime() const
{
    return this->fullReloadTime;
}

//------------------------------------------------------------------------------
#endif//RNSGAMESTATE_H
