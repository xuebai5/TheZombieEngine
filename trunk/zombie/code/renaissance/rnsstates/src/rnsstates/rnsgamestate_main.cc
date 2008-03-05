//------------------------------------------------------------------------------
//  rnsgamestate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsstates.h"

#include "rnsstates/rnsgamestate.h"

#include "napplication/nappviewport.h"
#include "napplication/napplication.h"

#include "ncommonapp/ncommonapp.h"

#include "nphysics/nphysicsserver.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "ntrigger/ntriggerserver.h"

#include "ncgameplayplayer/ncgameplayplayer.h"

#include "util/nstream.h"

/** ZOMBIE REMOVE
#include "rnsgameplay/ncgpweaponaddonclass.h"
#include "rnsgameplay/ncgpweaponcharsclass.h"
*/
#include "ncgamecamera/ncgamecamera.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncsoundlayer/ncsoundlayer.h"

#include "rnsgameplay/ninventoryitem.h"
#include "rnsgameplay/ninventorymanager.h"

#include "nspecialfx/nfxserver.h"
#include "nspecialfx/nfxobject.h"

#ifndef NGAME
#include "ndebug/ndebugserver.h"
#include "rnsstates/naitester.h"
#endif//!NGAME

#include "ncgameplayvehicleseat/ncgameplayvehicleseat.h"

#include "audio3/nlistener3.h"

#include "gameplay/ngamemessagewindowproxy.h"

#include "nwaypointserver/nwaypointserver.h"
#include "ncwaypointpath/ncwaypointpath.h"

//------------------------------------------------------------------------------
nNebulaScriptClass( RnsGameState, "ncommonstate" );

//------------------------------------------------------------------------------
const float RnsGameState::crossSightZ = -0.2f;
const float RnsGameState::crossStick = ( 0.001f * RnsGameState::crossSightZ ) / -0.1f;

//------------------------------------------------------------------------------
/**
*/
RnsGameState::RnsGameState():
    viewerPos( 0,1.7f,0 ),
    viewerAngles( 0.0f, N_PI ),
    isStarted( false ),
    aiStarted( false ),
    actionManager( 0 ),
    network( 0 ),
    showPhysics( true ),
    showMenu( false ),
    mouseXfactor( 1 ), mouseYfactor( 1 ),
    cameraVelocity( 10.0f ),
    gameCamera(0),
    lastMoveForward(0),
    lastMoveStep(0),
    lastMoveFlags(0),
    lastLookYaw(0),
    lastLookPitch(0),
    lastWeaponTrigger(false),
    lastWeaponPressed(false),
    reloadInputTime( 0 ),
    fullReloadTime( 1 ),
    scopeFx( 0 ),
    useSavedState( false ),
    crossMode( RnsGameState::CMT_NORMAL ),
    initClient( false )
#ifdef __NEBULA_STATS__
    , profInput( "profGameInput" )
    , profGameplay( "profGameplay", true )
    , profNetwork( "profNetwork" )
#endif
{
    // init the list of lines of crosshair
    this->crossList[0].x = 0.0f;
    this->crossList[0].y = 0.0f;
    this->crossList[0].z = this->crossSightZ;
    this->crossList[1].x = 0.0f;  
    this->crossList[1].y = 0.0f;  
    this->crossList[1].z = this->crossSightZ;

    this->crossList[2].x = 0.0f;  
    this->crossList[2].y = 0.0f;  
    this->crossList[2].z = this->crossSightZ;
    this->crossList[3].x = 0.0f;  
    this->crossList[3].y = 0.0f;  
    this->crossList[3].z = this->crossSightZ;

    this->crossList[4].x = 0.0f;  
    this->crossList[4].y = 0.0f;  
    this->crossList[4].z = this->crossSightZ;
    this->crossList[5].x = 0.0f;  
    this->crossList[5].y = 0.0f;  
    this->crossList[5].z = this->crossSightZ;

    this->crossList[6].x = 0.0f;  
    this->crossList[6].y = 0.0f;  
    this->crossList[6].z = this->crossSightZ;
    this->crossList[7].x = 0.0f;  
    this->crossList[7].y = 0.0f;  
    this->crossList[7].z = this->crossSightZ;
}

//------------------------------------------------------------------------------
/**
*/
RnsGameState::~RnsGameState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::Exit()
{
    if( this->savedState.IsEmpty() )
    {
        this->app->SetQuitRequested(true);
    }
    else
    {
        this->app->SetState( this->savedState );
    }
}

//------------------------------------------------------------------------------
/**
    Do an AI update cycle

    Less or more than one cycle may be done per update call when in accelerated
    time (only on debug)
*/
void
RnsGameState::UpdateAI()
{
#ifndef NGAME
    int aiUpdatesPerFrame( nAITester::Instance()->GetAIUpdatesPerFrame() ); 
    for ( int i(0); i < aiUpdatesPerFrame; ++i )
    {
#endif

        // @todo Uncomment when isServer is true in edit mode (or some similar way is available)
        //if ( this->isServer )
        {
#ifndef NGAME
            if ( !nAITester::Instance()->IsFSMPaused() )
#endif
            {
                static nGameEvent::Time currentTime(0);
                nTriggerServer::Instance()->Update( currentTime++ );
            }
        }

#ifdef __NEBULA_STATS__
        this->profGameplay.Start();
#endif
        this->entityManager->Trigger();
#ifdef __NEBULA_STATS__
        this->profGameplay.Stop();
#endif
#ifndef NGAME
    }
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::OnFrame()
{
#if __NEBULA_STATS__
    static_cast<nCommonApp*>(this->app.get())->profAppDoFramePhysics.StartAccum();
#endif
    nPhysicsServer::Instance()->Run(phyreal(this->app->GetFrameTime()));
#if __NEBULA_STATS__
    static_cast<nCommonApp*>(this->app.get())->profAppDoFramePhysics.StopAccum();
#endif

#ifdef __NEBULA_STATS__
    this->profInput.Start();
#endif
    this->HandleInput(static_cast<float>( this->app->GetFrameTime() ));
#ifdef __NEBULA_STATS__
    this->profInput.Stop();
#endif

#ifdef __NEBULA_STATS__
    this->profNetwork.Start();
#endif
    if( this->network )
    {
        this->network->Trigger( this->app->GetTime() );
    }
#ifdef __NEBULA_STATS__
    this->profNetwork.Stop();
#endif

    this->UpdateAI();

    this->UpdateCamera( this->app->GetTime() );

    this->gameCamera->Update( this->app->GetTime() );

    // viewport logic
    this->gameCamera->GetViewPort()->SetFrameId(this->app->GetFrameId());
    this->gameCamera->GetViewPort()->SetFrameTime(this->app->GetFrameTime());
    this->gameCamera->GetViewPort()->SetTime(this->app->GetTime());
    this->gameCamera->GetViewPort()->Trigger();


    // Set listener position
    nEntityObject* localPlayer(this->entityManager->GetLocalPlayer());
	if ( localPlayer && this->gameCamera->GetCameraType() == ncGameCamera::firstperson )
    {
        // Set transform from player entity
        ncTransform * playerTransform = localPlayer->GetComponentSafe<ncTransform>();
        
        // Matrix is rotated 180 degrees in Y axis
        matrix44 m(playerTransform->GetTransform());
        vector3 pos = m.pos_component();
        m.set_translation(vector3(0.0f,0.0f,0.0f));
        m.rotate_y(n_deg2rad(180.0f));
        m.set_translation(pos);
        nApplication::Instance()->GetAudioListener()->SetTransform( m );

        nSoundScheduler::Instance()->SetHearingViewport( 0 );
    }
    else
    {
        // Set viewport to take transform
        nSoundScheduler::Instance()->SetHearingViewport( this->gameCamera->GetViewPort() );
    }

    // Update game message window
    nGameMessageWindowProxy::Instance()->Trigger();

    nCommonState::OnFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::OnRender3D()
{
    this->gameCamera->GetViewPort()->OnRender3D();
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::OnRender2D()
{
    if( ! this->showMenu )
    {
        // move the mouse cursor to disallow it leave the window
        const nDisplayMode2 & mode = nGfxServer2::Instance()->GetDisplayMode(); 
        nGfxServer2::Instance()->SetCursorPosition( 
            mode.GetXPos() + ( mode.GetWidth() >> 1 ) , mode.GetYPos() + ( mode.GetHeight() >> 1 ) );
    }

    this->DrawHUD();

#ifndef NGAME
    bool debugTexts = nDebugServer::Instance()->GetFlagEnabled( "rnsview", "debugtexts" );

    if( debugTexts )
    {
        nString text;

        nEntityObject* localPlayer = this->entityManager->GetLocalPlayer();

        // Print current sound
        if ( localPlayer )
        {
            ncSoundLayer* soundLayer = localPlayer->GetComponent <ncSoundLayer>();

            if ( soundLayer )
            {
                // Action
                text.Clear();
                text = "Player Sound -> ";
                if( soundLayer->GetCurrentAction() )
                {
                    text.Append (soundLayer->GetCurrentAction());
                }
                nGfxServer2::Instance()->Text (text.Get(), vector4 (1.0f, 0.6f, 0.6f, 1.f), -0.6f, 0.45f);
                // Material
                text.Clear();
                text = "Player material -> ";
                text.Append (soundLayer->GetCurrentMaterial());
                nGfxServer2::Instance()->Text (text.Get(), vector4 (1.0f, 0.2f, 0.2f, 1.f), -0.2f, 0.45f);
            }
        }

        // Print logic animator state
        if( localPlayer )
        {
            ncLogicAnimator* animator = localPlayer->GetComponent <ncLogicAnimator>();

            if ( animator )
            {
                text.Clear();
                text = animator->GetStringState();
                nGfxServer2::Instance()->Text( text.Get(), vector4( 0.4f, 1.0f, 0.4f, 0.8f), 0.f, 0.7f ); 
            }
        }

        // Print player health    
        text.Clear();
        text = " Health -> ";
        if ( localPlayer )
        {   
            ncGameplayLiving* gameplayLiving = localPlayer->GetComponent<ncGameplayLiving>();
            if ( gameplayLiving ) 
            {
                text.AppendInt( gameplayLiving->GetHealth() );
            }            
        }        
        nGfxServer2::Instance()->Text( text.Get(), vector4( 1.0f, 0.0f, 0.0f, 1.0f), -0.95f, -0.95f );

        // Print player stamina
        text.Clear();    
        text = " Stamina -> ";
        if ( localPlayer )
        {   
            ncGameplayLiving* gameplayLiving = localPlayer->GetComponent<ncGameplayLiving>();
            if ( gameplayLiving ) 
            {
                text.AppendInt( localPlayer->GetComponent<ncGameplayPlayer>()->GetStamina() );
            }            
        }        
        nGfxServer2::Instance()->Text( text.Get(), vector4( 1.0f, 0.0f, 0.0f, 1.0f), -0.95f, -0.85f );

        // print free inventory items
        nInventoryManager * inventory = nInventoryManager::Instance();
        if( inventory )
        {
            inventory->DebugDraw( nGfxServer2::Instance() );
        }
    }

    nEntityObject* localPlayer = this->gameCamera->GetAnchorPoint();
    if( localPlayer && ( this->gameCamera->GetCameraType() != ncGameCamera::free ) )
    {
        ncGameplay * gameplay = localPlayer->GetComponent<ncGameplay>();
        if( gameplay )
        {
            gameplay->DebugDraw( nGfxServer2::Instance() );
        }
    }

#endif

}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::DrawHUD()
{
    bool hideScope = true;

    nEntityObject * cameraLookat = this->gameCamera->GetAnchorPoint();
    if( cameraLookat && (this->gameCamera->GetCameraType() == ncGameCamera::firstperson ))
    {
        ncGameplayPlayer * gameplay = 0;
        gameplay = cameraLookat->GetComponent<ncGameplayPlayer>();
        if( gameplay )
        {
            bool drawSight = false;

            if( gameplay->IsIronsight() )
            {
                drawSight = this->crossMode == CMT_ALWAYS;
            }
            else
            {
                drawSight = this->crossMode != CMT_NEVER;
            }

            if( this->scopeFx && gameplay->IsInScope() )
            {
                hideScope = false;

                if( ! this->scopeFx->IsAlive() )
                {
                    nFXServer::Instance()->PullEffect( this->scopeFx->GetKey() );
                }
            }

            if( drawSight )
            {
                matrix44 modelTransform;
                nGfxServer2::Instance()->SetTransform(nGfxServer2::Model, modelTransform);
                nGfxServer2::Instance()->PushTransform(nGfxServer2::View, modelTransform);

                float accuracy = 0.0f;
                if( gameplay->GetCurrentWeapon() )
                {
                    ncGPWeapon * gpWeapon = 0;
                    gpWeapon = gameplay->GetCurrentWeapon()->GetComponentSafe<ncGPWeapon>();
                    if( gpWeapon )
                    {
                        accuracy = gpWeapon->GetAccuracy();
                    }
                }

                float aperture = ncGPWeaponCharsClass::MAX_DEVIATION;
                aperture *= ( 1.0f - accuracy ) * (0.1f / ncGPWeaponCharsClass::DEVIATION_DISTANCE);
                aperture = ( aperture * RnsGameState::crossSightZ ) / -0.1f;

                // update the lines
                this->crossList[0].x = aperture;
                this->crossList[1].x = aperture + this->crossStick;

                this->crossList[2].x = -aperture;
                this->crossList[3].x = -(aperture + this->crossStick);

                this->crossList[4].y =  aperture;
                this->crossList[5].y =  aperture + this->crossStick;

                this->crossList[6].y = -aperture;
                this->crossList[7].y = -(aperture + this->crossStick);

                nGfxServer2::Instance()->BeginLines();
                nGfxServer2::Instance()->DrawLines3d( this->crossList,   2, vector4(1, 1, 1, 0.8f));
                nGfxServer2::Instance()->DrawLines3d( this->crossList+2, 2, vector4(1, 1, 1, 0.8f));
                nGfxServer2::Instance()->DrawLines3d( this->crossList+4, 2, vector4(1, 1, 1, 0.8f));
                nGfxServer2::Instance()->DrawLines3d( this->crossList+6, 2, vector4(1, 1, 1, 0.8f));
                nGfxServer2::Instance()->EndLines();

                nGfxServer2::Instance()->PopTransform(nGfxServer2::View);
            }
        }
    }

    if( this->scopeFx && hideScope && this->scopeFx->IsAlive() )
    {
        nFXServer::Instance()->KillEffect( this->scopeFx->GetKey() );
    }
}

//------------------------------------------------------------------------------
/**
    @param time actual application time
*/
void
RnsGameState::UpdateCamera( nTime time )
{
    nEntityObject * localPlayer = this->entityManager->GetLocalPlayer();
    if( localPlayer && ( this->gameCamera->GetAnchorPoint() == localPlayer ) )
    {
        ncGameplayPlayer * player = localPlayer->GetComponent<ncGameplayPlayer>();
        if( player )
        {
            if( this->gameCamera->GetCameraType() == ncGameCamera::firstperson )
            {
                vector3 position;
                polar2 angles;

                player->UpdateCamera( time );

                player->GetCameraPosition( position );
                player->GetShotAngles( angles );

                this->gameCamera->GetViewPort()->SetViewerAngles( angles );
                this->gameCamera->GetViewPort()->SetViewerPos( position );
            }

            this->gameCamera->GetViewPort()->SetAngleOfView( player->GetAngleOfView() );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param object dead player entity
*/
void
RnsGameState::GameEntityEliminated( nEntityObject * object )
{
#ifndef NGAME
    if ( !this->gameCamera )
    {
        return;
    }
#endif

    // remove from camera
    if( this->gameCamera->GetAnchorPoint() == object )
    {
        this->ChangeCameraAnchor( 0 );
    }

    // search the entity in the list of players
    for( int i = 0 ; i < MAX_PLAYERS ; ++i )
    {
        if( this->players[ i ] )
        {
            if( this->players[ i ]->clientEntity == object )
            {
                this->players[ i ]->clientEntity = 0;
            }
        }
    }

    // if entity is the local entity
    if( this->entityManager->GetLocalPlayer() == object )
    {
        // re-spawn 
        nstream data;
        nRnsEntityManager::PlayerData playerData;

        playerData.weapon = nRnsEntityManager::WEAPON_M4;
        playerData.flags = nRnsEntityManager::PDF_DEAD;
        playerData.position = object->GetComponentSafe<ncTransform>()->GetPosition();

        data.SetWrite( true );
        playerData.UpdateStream( data );

        this->network->CallRPCServer( nRnsEntityManager::SPAWN_PLAYER, data.GetBufferSize(), data.GetBuffer() );
    }
}

//------------------------------------------------------------------------------
/**
    @param client client identifier
    @param playerData data to create the player
*/
void
RnsGameState::SpawnPlayer( int client, nRnsEntityManager::PlayerData & playerData )
{
    this->SetPlayerSpawnPosition(client, playerData);

    nEntityObject * player = this->entityManager->CreatePlayer( playerData );
    n_assert( player );

    n_assert( this->players[ client ] );
    if( this->players[ client ] )
    {
        this->players[ client ]->clientEntity = player;
    }

    if( player )
    {
        int entityId = player->GetId();

        // set entity as local in the right client
        if( client == SERVER_PLAYER )
        {
            this->SetLocalPlayer( entityId );
        }
        else
        {
            nstream data;
            data.SetWrite( true );
            data.UpdateInt( entityId );
            // set local player to the connected player
            this->network->CallRPCClient( client, 
                nRnsEntityManager::SET_LOCAL_PLAYER, data.GetBufferSize(), data.GetBuffer() );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param entityId new local player Id
*/
void
RnsGameState::SetLocalPlayer( int entityId )
{
    this->entityManager->SetLocalPlayer( entityId );

    nEntityObject * player = this->entityManager->GetLocalPlayer();
    if( player )
    {
        // bind to level creation and loading
        player->BindSignal( ncGameplayPlayer::SignalChangePlayerState, 
                                this,
                                &RnsGameState::NewPlayerState,
                                0);

        this->ChangeCameraAnchor( player );

        ncGameplayPlayer * gameplay = player->GetComponent<ncGameplayPlayer>();
        if( gameplay )
        {
            gameplay->SetFirstCameraMode( true );
        }

        if( player->GetClass()->FindCmdByName( "InitEquipment" ) )
        {
            player->Call( "InitEquipment", 0, 0 );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @returns the index of the las inventory item in the weapon
*/
int
RnsGameState::GetLastInventoryItem()const
{
    nEntityObject * player = this->entityManager->GetLocalPlayer();
    ncGameplayPlayer * gameplay = 0;
    if( player )
    {
        gameplay = player->GetComponent<ncGameplayPlayer>();
    }

    bool valid = gameplay != 0;
    if( valid )
    {
        // get inventory 
        nInventoryContainer * inventory = gameplay->GetInventory();
        if( inventory )
        {
            return ( inventory->GetNumItems() - 1 );
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
/**
    Sets a new player state.

    IMPORTANT!!: Don't call this function directly, change player's state directly

    @param newstate player's new state
    
    history:
        - 27-Jan-2006   David Reyes    created
*/
void RnsGameState::NewPlayerState( int newstate, nEntityObject* player )
{
    nEntityObject * localPlayer = this->entityManager->GetLocalPlayer();

    switch( ncGameplayPlayer::state(newstate) )
    {
    case ncGameplayPlayer::IS_DRIVER:

        // 1st time set camera
        this->gameCamera->SetCameraType( ncGameCamera::thirdperson );

        this->gameCamera->SetAttributes( ncGameCamera::lookat );

        this->gameCamera->SetAnchorPoint( localPlayer );

        this->gameCamera->LookAt( localPlayer->GetComponent<ncGameplayPlayer>()->GetSeat()->GetComponent<ncGameplayVehicleSeat>()->GetVehicle() );

        this->gameCamera->SetCameraOffset( vector3(0, 3, -8) );

        this->gameCamera->SetTranspositionXType( ncGameCamera::dampen );
        this->gameCamera->SetTranspositionYType( ncGameCamera::dampen );
        this->gameCamera->SetTranspositionZType( ncGameCamera::dampen );

        this->gameCamera->SetDampeningPosition( vector3(float(.001),float(.001),float(.001)) );

        this->gameCamera->SetTranspositionOrientationType( ncGameCamera::dampen );

        this->gameCamera->Build();

        break;

    default:

        this->gameCamera->SetCameraType( ncGameCamera::firstperson );

        this->gameCamera->SetAttributes( ncGameCamera::directed );

        this->gameCamera->SetAnchorPoint( localPlayer );

        this->gameCamera->SetTranspositionXType( ncGameCamera::dampen );
        this->gameCamera->SetTranspositionYType( ncGameCamera::dampen );
        this->gameCamera->SetTranspositionZType( ncGameCamera::dampen );

        this->gameCamera->SetDampeningPosition( vector3(float(.3),float(.3),float(.3)) );

        this->gameCamera->Build();

        if( localPlayer == player )
        {
            player->GetComponent<ncGameplayPlayer>()->SetFirstCameraMode( true );
        }
        else
        {
            player->GetComponent<ncGameplayPlayer>()->SetFirstCameraMode( false );
        }

        break;
    }
}

//-----------------------------------------------------------------------------
/**
    Get the client that owns a player

    @param player Player entity whose client is wanted to be known
    @retval client Client id owning the given player
    @return True if a client owning the player is found, false otherwise

    history:
        - 23-May-2006   Carles Ros    created
*/
bool
RnsGameState::GetClientOfPlayer( nEntityObject* player, int& client, bool & isServer ) const
{
    client = -1;
    isServer = false;
    for ( int i(0); i < MAX_PLAYERS; ++i )
    {
        if ( this->players[i] )
        {
            if ( this->players[i]->clientEntity == player )
            {
                client = i;
                isServer = (client == SERVER_PLAYER);
                return true;
            }
        }
    }
    return false;
}
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Get the path name for a player

    @param client the client id
    @retval path the path name

*/
void
RnsGameState::SpawnPathForClient( int client, nString& path )
{
    if( client == SERVER_PLAYER )
    {
        path = "Player0";
    }
    else
    {
        path = "Player";
        path.AppendInt( client + 1 );
    }
}
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
    Set the correct position for a player spawn

    @param client the client id
    @param startup true if first spawn, false if re-spawn
    @retval data the modified player data

*/
void
RnsGameState::SetPlayerSpawnPosition( int client,
                                      nRnsEntityManager::PlayerData& data )
{
    nString pathname;
    this->SpawnPathForClient( client, pathname );

    // Get the path
    ncWayPointPath* path = nWayPointServer::Instance()->CheckPathExists( pathname );
    if( !path )
    {
        // No path present
#ifndef NGAME
        // If first player, set from current viewport if valid
        if( client == SERVER_PLAYER )
        {
            if( this->refPrevViewport.isvalid() )
                {
                    data.flags |= nRnsEntityManager::PDF_POSITION |
                        nRnsEntityManager::PDF_ANGLES;
                    data.position = this->refPrevViewport->GetViewerPos();
                    data.angles = this->refPrevViewport->GetViewerAngles();
                }
        }
#endif // NGAME
        // Else don't move from <0,0,0>

        return;
    }

    // If path has no waypoints, just appear at <0,0,0>
    if( path->GetNumWayPoints() == 0 )
    {
        return;
    }

    data.flags |= nRnsEntityManager::PDF_POSITION;
    if( !(data.flags & nRnsEntityManager::PDF_DEAD) )
    {
        // Get the first point at startup
        data.position = path->GetWayPointIndex(0)->GetPosition();
    }
    else
    {
        // Get nearest point to death site
        data.position = path->GetWayPointIndex(path->ClosestWayPoint(data.position))->GetPosition();
    }

    // Raise player position
    data.position.y += 1.0f;

    return;
}

//------------------------------------------------------------------------------
/**
    Saves the mouse settings to file
*/
void
RnsGameState::SaveMouseSettings( const char *filename )
{
    // save the mouse settings for the level
    
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert( ps );

    nCmd* cmd = ps->GetCmd( this, 'THIS' );
    if ( ps->BeginObjectWithCmd( this, cmd, filename ) )
    {
        ps->Put( this, 'LSMX', this->GetMouseXFactor() );
        ps->Put( this, 'LSMY', this->GetMouseYFactor() );
        ps->Put( this, 'LSYI', this->GetMouseYInvert() );

        ps->EndObject( true );
    }
}

//------------------------------------------------------------------------------
/**
    Loads the mouse settings from file
*/
void
RnsGameState::LoadMouseSettings( const char *filename )
{
    // load mouse settings for the level
    if ( kernelServer->GetFileServer()->FileExists( filename ) )
    {
        kernelServer->PushCwd( this );
        kernelServer->Load( filename, false );
        kernelServer->PopCwd();
    }
}
