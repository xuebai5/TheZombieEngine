//------------------------------------------------------------------------------
//  rnsgamestate_setup.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsstates.h"

#include "rnsstates/rnsgamestate.h"

#include "rnsgameplay/ngameplayutils.h"

#include "ngpactionmanager/ngpactionmanager.h"

#include "nnetworkmanager/nnetworkmanager.h"

#include "ntrigger/ntriggerserver.h"

#include "util/nstream.h"

#include "nsoundscheduler/nsoundscheduler.h"
#include "nmusictable/nmusictable.h"

#include "ncgamecamera/ncgamecamera.h"

#include "ncgameplayplayer/ncgameplayplayer.h"

#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialquadtree.h"

#include "gameplay/nfxeventtrigger.h"
#include "nspecialfx/nfxserver.h"
#include "nspecialfx/nfxobject.h"

#include "gameplay/gpimpactsound.h"
#include "gameplay/ngamemessagewindowproxy.h"

#ifndef NGAME
#include "rnsstates/naitester.h"
#include "conjurer/nconjurerapp.h"
#include "nundo/nundoserver.h"
#endif

//------------------------------------------------------------------------------
/**
    GameClientOfPlayer

    Callback to know which client a player belongs to
    returns true if the player provided is a client and the index was resolved
            false if didn't work or the message is for the 
*/
namespace
{
    bool
    GetClientOfPlayer( nEntityObject* player, int& client, bool& isServer )
    {
        nAppState* gameState( nApplication::Instance()->FindState("game") );
        if ( gameState )
        {
            if ( gameState->IsA("rnsgamestate") )
            {

                return static_cast< RnsGameState* >( gameState )->GetClientOfPlayer( player, client, isServer );
            }
        }
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    @param application application that create the state
*/
void
RnsGameState::OnCreate( nApplication * application )
{
    nCommonState::OnCreate( application );
}

//------------------------------------------------------------------------------
/**
    @param prevState previous state
*/
void
RnsGameState::OnStateEnter( const nString & prevState )
{
#ifndef NGAME
    // clean undo buffer before saving game state
    // problems with the game and save manager
    nUndoServer::Instance()->Clean();

    // save editor state if coming from the editor
    nConjurerApp::Instance()->SaveEditorState();
#endif

    if( ! this->useSavedState )
    {
        this->savedState = prevState;
    }

    // getting viewport
    this->refViewport = static_cast<nAppViewport*>( nKernelServer::ks->Lookup( "/usr/rnsview" ) );
    n_assert( this->refViewport.isvalid() );

    // creating game camera
    this->gameCamera = nEntityObjectServer::Instance()->NewLocalEntityObject( "negamecamera" )->GetComponent<ncGameCamera>();

    n_assert2( this->gameCamera, "Failed to create a game camera." );

    this->gameCamera->SetViewPort( this->refViewport );
    this->gameCamera->SetMinDistance( 1.0f );

    this->ChangeCameraAnchor( 0 ); 

#ifndef NGAME
    this->gameCamera->SetCameraOffset( vector3(-10, 5, 0) );
#endif

    this->gameCamera->GetViewPort()->SetVisible(true);

    // set the before camera position
    if( this->refPrevViewport.isvalid() )
    {
        this->gameCamera->GetViewPort()->SetViewerPos( this->refPrevViewport->GetViewerPos() );
        this->gameCamera->GetViewPort()->SetViewerAngles( this->refPrevViewport->GetViewerAngles() );
    }

    // hide the mouse cursor
    nGfxServer2::Instance()->SetCursorVisibility( nGfxServer2::Custom );

    this->StartAI();

    for( int i = 0 ; i < RnsGameState::MAX_PLAYERS ; ++i )
    {
        this->players[ i ] = 0;
    }

    // create PlayerInfo of server machine
    this->players[ SERVER_PLAYER ] = n_new( PlayerInfo );
    n_assert( this->players[ SERVER_PLAYER ] );
    this->players[ SERVER_PLAYER ]->clientEntity = 0;

    // setting up the impact function dealer
    nPhysicsServer::Instance()->RegisterImpactCallBack( gpImpactSoundProcess );

    nCommonState::OnStateEnter(prevState);

    this->isStarted = true;

    // if is singleplayer or multiplayer server -> create a player
    if( this->network )
    {
        if( this->network->IsServer() )
        {
            nstream data;
            nRnsEntityManager::PlayerData playerData;

            playerData.weapon = nRnsEntityManager::WEAPON_M4;
            playerData.flags = 0;

            data.SetWrite( true );
            playerData.UpdateStream( data );

            this->network->CallRPCServer( nRnsEntityManager::SPAWN_PLAYER, data.GetBufferSize(), data.GetBuffer() );
        }
    }

    // create effects of camera
    this->scopeFx = nFxEventTrigger::Instance()->Scope();
    if( this->scopeFx )
    {
        nFXServer::Instance()->KillEffect( this->scopeFx->GetKey() );
    }    

    // allow game message window proxy to know which client belongs to each player
    nGameMessageWindowProxy::Instance()->RegisterGetClientCallback( ::GetClientOfPlayer );

    // clear message window locally
    nGameMessageWindowProxy::Instance()->ClearMessageWindow();

    this->showMenu = false;
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::StartAI()
{
    if ( !this->aiStarted )
    {
        aiStarted = true;

        this->refFxEventTrigger = static_cast<nFxEventTrigger*>( nKernelServer::Instance()->New( "nfxeventtrigger") );
        nFXServer::Instance()->LoadFxMaterials();
        this->refFxEventTrigger->LoadSFx();

#ifndef NGAME
        nAITester::Instance()->EnableGameplayUpdating();
        nAITester::Instance()->DisableSoundSources();
        nAITester::Instance()->EnableSoundSources();
#endif

        nGameplayUtils::Init();

        this->actionManager = (nGPActionManager*)( nKernelServer::Instance()->New("ngpactionmanager") );
        this->InitNetwork();
        if ( !this->entityManager.isvalid() )
        {
            this->entityManager = static_cast<nRnsEntityManager*>( nKernelServer::Instance()->New( "nrnsentitymanager", "/sys/servers/rnsentitymanager" ) );
            this->entityManager->BindSignal( 
                    nRnsEntityManager::SignalEntityEliminated, this, &RnsGameState::GameEntityEliminated, 0 );
        }
        this->entityManager->Start();
        nTriggerServer::Instance()->Start();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
RnsGameState::StopAI()
{
    if ( this->aiStarted )
    {
        aiStarted = false;

        this->refFxEventTrigger->Release();

        nTriggerServer::Instance()->Stop();
        this->entityManager->Stop();

        this->EndNetwork();

        if( this->actionManager )
        {
            this->actionManager->Release();
            this->actionManager = 0;
        }

        if( this->entityManager.isvalid() )
        {
            this->entityManager->UnbindTargetObject( nRnsEntityManager::SignalEntityEliminated.GetId(), this );
            this->entityManager->Release();
        }

        nGameplayUtils::Destroy();

#ifndef NGAME
        nAITester::Instance()->DisableGameplayUpdating();
        nAITester::Instance()->DisableSoundSources();
#endif
    }
}

//------------------------------------------------------------------------------
/**
    @param nextState next state
*/
void
RnsGameState::OnStateLeave( const nString & /*nextState*/ )
{
    this->isStarted = false;

    this->StopAI();

    // delete PlayerInfo array
    n_assert( this->players[ SERVER_PLAYER ] );

    for( int i = 0 ; i < MAX_PLAYERS ; ++i )
    {
        if( this->players[ i ] )
        {
            n_delete( this->players[ i ] );
            this->players[ i ] = 0;
        }
    }

    if( this->refViewport.isvalid() )
    {
        if( this->refPrevViewport.isvalid() )
        {
            this->refPrevViewport->SetViewerAngles( this->gameCamera->GetViewPort()->GetViewerAngles() );
            this->refPrevViewport->SetViewerPos( this->gameCamera->GetViewPort()->GetViewerPos() );
            this->refPrevViewport.invalidate();
        }
        this->gameCamera->GetViewPort()->SetVisible(false);
    }

    // restore the mouse cursor
    nGfxServer2::Instance()->SetCursorVisibility( nGfxServer2::System );

    nEntityObjectServer::Instance()->RemoveEntityObject( this->gameCamera->GetEntityObject());

    /// restoring collision for edition
    nEntityObject * outdoor = nSpatialServer::Instance()->GetOutdoorEntity();

    if( outdoor )
    {
        nPhysicsWorld* world(outdoor->GetComponent<ncPhysicsObj>()->GetWorld());

        if( world )
        {
            world->GetObjectManager()->Restore();
        }
    }
    
    // set null hearing viewport to sound scheduler
    nSoundScheduler::Instance()->SetHearingViewport( 0 );

    // stop all sounds
    nSoundScheduler::Instance()->StopAllSounds();

	// stop music
	// @todo Make music table a singleton or get its NOH path from a constant
    static_cast<nMusicTable*>( nKernelServer::Instance()->Lookup("/usr/musictable") )->StopMusic();

#ifndef NGAME
    // save editor state if coming from the editor
    nConjurerApp::Instance()->RestoreEditorState();
#endif
    // unsetting up the impact function dealer
    nPhysicsServer::Instance()->RegisterImpactCallBack( 0 );

    // remove effects
    if( this->scopeFx )
    {
        nFXServer::Instance()->RemoveEffect( this->scopeFx->GetKey() );
        this->scopeFx = 0;
    }

    // restore game message window proxy
    nGameMessageWindowProxy::Instance()->Reset();
}

//------------------------------------------------------------------------------
/**
    @param entity new camera anchor. Or Null entity
*/
void
RnsGameState::ChangeCameraAnchor( nEntityObject * entity )
{
    if( entity )
    {
        this->gameCamera->SetCameraType( ncGameCamera::firstperson );
        this->gameCamera->SetAnchorPoint( entity );
        this->gameCamera->SetAttributes( ncGameCamera::directed );
    }
    else
    {
        // switch it off
        this->gameCamera->SetAnchorPoint( 0 );
    }
}

//------------------------------------------------------------------------------
/**
    @param stateName the next state before load
*/
void
RnsGameState::SetNextState( const nString & stateName )
{
    this->savedState = stateName;
    this->useSavedState = true;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Switches on/off artist mode.
*/
void
RnsGameState::SwitchArtistMode()
{
    nEntityObject * player = this->gameCamera->GetAnchorPoint();
    ncGameplayPlayer * gameplay = 0;
    if( player )
    {
        gameplay = player->GetComponent<ncGameplayPlayer>();
    }
    switch( this->gameCamera->GetCameraType() )
    {
    case ncGameCamera::firstperson:        
        if( gameplay )
        {
            gameplay->SetFirstCameraMode( false );
        }
        this->gameCamera->SetCameraType( ncGameCamera::thirdperson );
        this->gameCamera->SetAttributes( ncGameCamera::lookat );
        this->gameCamera->SetAnchorPoint( this->entityManager->GetLocalPlayer() );
        this->gameCamera->SetLookAtOffset( float(-2) );
        this->gameCamera->SetMaxDistance( float(12) );
        this->gameCamera->SetDampeningPosition( vector3(0.001f,0.05f,0.001f) );
        break;

    case ncGameCamera::thirdperson:
        if( gameplay )
        {
            gameplay->SetFirstCameraMode( true );
        }
        this->gameCamera->SetCameraType( ncGameCamera::firstperson );
        this->gameCamera->SetAttributes( ncGameCamera::directed );
        this->gameCamera->SetAnchorPoint( this->entityManager->GetLocalPlayer() );
        break;
    }
}

#endif

//------------------------------------------------------------------------------
