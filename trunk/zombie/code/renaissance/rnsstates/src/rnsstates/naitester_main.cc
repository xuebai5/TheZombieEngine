//-----------------------------------------------------------------------------
//  naitester_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "rnsstates/naitester.h"

#ifndef NGAME
#include "ntrigger/ntriggerserver.h"
#include "rnsgameplay/ncgameplay.h"
#include "ncfsm/ncfsm.h"
#include "rnsstates/rnsgamestate.h"
#include "conjurer/nconjurerapp.h"
#include "ndebug/nceditor.h"

// @todo Remove when store/restore is done in a generic way
#include "ncgameplayliving/ncgameplayliving.h"
#include "ncaimovengine/ncaimovengine.h"
#include "ntrigger/ncareatrigger.h"
#include "ncaistate/ncaistate.h"
#include "ncagentmemory/ncagentmemory.h"
#include "ncsound/ncsound.h"
#include "ntrigger/ncareaevent.h"
#include "zombieentity/ncdictionary.h"
#endif

//------------------------------------------------------------------------------
nNebulaScriptClass(nAITester, "nroot");

//------------------------------------------------------------------------------
#ifndef NGAME
NSIGNAL_DEFINE( nAITester, GameplayUpdatingEnabled );
NSIGNAL_DEFINE( nAITester, GameplayUpdatingDisabled );
NSIGNAL_DEFINE( nAITester, SoundSourcesEnabled );
NSIGNAL_DEFINE( nAITester, SoundSourcesDisabled );
NSIGNAL_DEFINE( nAITester, PendingEventChanged );

//------------------------------------------------------------------------------
nAITester* nAITester::instance = 0;

//------------------------------------------------------------------------------
// @todo Remove when store/restore is done in a generic way
namespace
{
#if 0
    /// Data needed to restore agents
    struct AgentState
    {
        nRef<nEntityObject> agent;
        vector3 position;
        vector3 orientation;
        int visibilityEvent;
        int health;
    };
    nArray<AgentState> agentStates;

    /// Data needed to restore triggers
    struct TriggerState
    {
        nRef<nEntityObject> trigger;
        bool triggerOnce;
        bool enabled;
    };
    nArray<TriggerState> triggerStates;
#endif
}

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nAITester::nAITester()
    : ticks(0),
      ticksPerAIUpdate(-1)
{
    // Initialize instance pointer
    if ( !nAITester::instance )
    {
        nAITester::instance = this;
    }

    // Initialize updating flags (all are deactivated by default)
    this->updatingFlags.Init( UPDATING_FLAGS_NUMBER );
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nAITester::~nAITester()
{
    this->DisableGameplayUpdating();
    this->DisableSoundSources();
    nAITester::instance = 0;
}

//-----------------------------------------------------------------------------
/**
    Enable gameplay entities for updating, doing any needed initialization
*/
void
nAITester::EnableGameplayUpdating()
{
    if ( !this->updatingFlags.IsFlagEnabled( GAMEPLAY ) )
    {
        this->updatingFlags.EnableFlag( GAMEPLAY );

        this->StoreGameState();

        RnsGameState* state = static_cast<RnsGameState*>( nConjurerApp::Instance()->FindState( "game" ) );
        n_assert( state );
        state->StartAI();

        this->SignalGameplayUpdatingEnabled( this );
    }
}

//-----------------------------------------------------------------------------
/**
    Disable gameplay entities from updating, doing any needed clean up/state restoring
*/
void
nAITester::DisableGameplayUpdating()
{
    if ( this->updatingFlags.IsFlagEnabled( GAMEPLAY ) )
    {
        this->updatingFlags.DisableFlag( GAMEPLAY );

        RnsGameState* state = static_cast<RnsGameState*>( nConjurerApp::Instance()->FindState( "game" ) );
        n_assert( state );
        state->StopAI();

        this->RestoreGameState();

        this->SignalGameplayUpdatingDisabled( this );
    }
}

//-----------------------------------------------------------------------------
/**
    Enable sound sources, doing any needed initialization
*/
void
nAITester::EnableSoundSources()
{
    if ( !this->updatingFlags.IsFlagEnabled( SOUND_SOURCES ) )
    {
        this->updatingFlags.EnableFlag( SOUND_SOURCES );
        this->InitSoundSources();
        this->SignalSoundSourcesEnabled( this );
    }
}

//-----------------------------------------------------------------------------
/**
    Disable sound sources, doing any needed clean up/state restoring
*/
void
nAITester::DisableSoundSources()
{
    if ( this->updatingFlags.IsFlagEnabled( SOUND_SOURCES ) )
    {
        this->updatingFlags.DisableFlag( SOUND_SOURCES );
        this->RestoreSoundSources();
        this->SignalSoundSourcesDisabled( this );
    }
}

//-----------------------------------------------------------------------------
/**
    Store a copy of that data that may change due to AI
*/
void
nAITester::StoreGameState()
{
#if 0
    // Store entity states
    for ( nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject(); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
    {
#ifndef NGAME
        // Skip deleted entities
        ncEditor* editor = entity->GetComponent<ncEditor>();
        if ( !editor )
        {
            continue;
        }
        else if ( editor->IsDeleted() )
        {
            continue;
        }
#endif

        // Agent entities
        if ( entity->GetComponent<ncFSM>() )
        {
            // Store agent state
            AgentState state;
            state.agent = entity;
            state.position = entity->GetComponentSafe<ncTransform>()->GetPosition();
            state.orientation = entity->GetComponentSafe<ncTransform>()->GetEuler();
            state.health = entity->GetComponentSafe<ncGameplayLiving>()->GetHealth();
            agentStates.Append( state );
        }

        // Triggers
        if ( entity->GetComponent<ncTrigger>() && !entity->IsA("nesoundsource") )
        {
            // Store trigger state
            TriggerState state;
            state.trigger = entity;
            state.triggerOnce = entity->GetComponentSafe<ncTrigger>()->GetTriggerOnce();
            state.enabled = entity->GetComponentSafe<ncTrigger>()->GetTriggerEnabled();
            triggerStates.Append( state );
        }
    }
#endif
}


//-----------------------------------------------------------------------------
/**
    Restore the last stored game state
*/
void
nAITester::RestoreGameState()
{
#if 0
    // Restore agents
    for ( int i(0); i < agentStates.Size(); ++i )
    {
        AgentState& state = agentStates[i];
        if ( state.agent.isvalid() )
        {
            // Abort current action and exit from the FSM
            state.agent->GetComponentSafe<ncGameplay>()->AbortCurrentAction();
            state.agent->GetComponentSafe<ncAIMovEngine>()->Stop();
            state.agent->GetComponentSafe<ncFSM>()->Reset();

            // Restore the AI state
            state.agent->GetComponentSafe<ncAIState>()->Reset();
            state.agent->GetComponentSafe<ncAgentMemory>()->ResetMemory();

            // Restore the transformation
            ncTransform * transformComp = state.agent->GetComponentSafe<ncTransform>();
            transformComp->SetPosition( state.position );
            transformComp->SetEuler( state.orientation );

            // Restore the health
            state.agent->GetComponentSafe<ncGameplayLiving>()->SetHealth( state.health );
            // Destroy the rings manager
            state.agent->GetComponentSafe<ncGameplayLiving>()->DestroyRingsManager();

            // Delete user temporary variables
            state.agent->GetComponentSafe<ncDictionary>()->ClearLocalVars();
        }
    }
    agentStates.Clear();

    // Restore triggers
    for ( int i(0); i < triggerStates.Size(); ++i )
    {
        TriggerState& state = triggerStates[i];
        if ( state.trigger.isvalid() )
        {
            // Clear area triggers temporary data
            if ( state.trigger->GetComponent<ncAreaTrigger>() )
            {
                state.trigger->GetComponentSafe<ncAreaTrigger>()->ResetTrigger();
            }

            // Restore trigger state
            state.trigger->GetComponentSafe<ncTrigger>()->SetTriggerOnce( state.triggerOnce );
            state.trigger->GetComponentSafe<ncTrigger>()->SetTriggerEnabled( state.enabled );

            // Delete user temporary variables
            state.trigger->GetComponentSafe<ncDictionary>()->ClearLocalVars();
        }
    }

    // Restore spawners
    for ( nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject(); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
    {
        if ( entity->GetComponent( ncSpawnerType ) || entity->GetComponent<ncSpawnPoint>() )
        {
            entity->GetComponentSafe<ncDictionary>()->ClearLocalVars();
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
    Prepare sound sources to be triggered for 'on enter'/'on exit' area events
*/
void
nAITester::InitSoundSources()
{
    if ( nApplication::Instance()->GetCurrentState() != "game" )
    {
        for ( nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject(); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
        {
            // Create a player event attached to each viewport camera
            if ( entity->IsA("necamera") )
            {
                nGameEvent event;
                event.SetType( nGameEvent::PLAYER );
                event.SetSourceEntity( entity->GetId() );
                event.SetEmitterEntity( entity->GetId() );
                event.SetDuration( 0 );
                nTriggerServer::Instance()->PostAreaEvent( event );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Restore sound sources to the state previous to the init call
*/
void
nAITester::RestoreSoundSources()
{
    for ( nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject(); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
    {
        // Restore sound sources
        ncSound* sound = entity->GetComponent<ncSound>();
        ncAreaTrigger* trigger = entity->GetComponent<ncAreaTrigger>();
        if ( sound && trigger )
        {
            sound->StopSound();
            trigger->ResetTrigger();
            ncDictionary * dicComp = trigger->GetComponentSafe<ncDictionary>();
            dicComp->SetLocalIntVariable("play_sound_counter", 0);
        }

        if ( nApplication::Instance()->GetCurrentState() != "game" )
        {
            // Destroy player events attached to viewport cameras
            if ( entity->IsA("necamera") )
            {
                ncAreaEvent* areaEvent = entity->GetComponentSafe<ncAreaEvent>();
                for ( ncAreaEvent::Iterator it( areaEvent->GetEventsIterator() ); !it.IsEnd(); it.Next() )
                {
                    nGameEvent* event = nTriggerServer::Instance()->GetEvent( it.Get() );
                    if ( event->GetType() == nGameEvent::PLAYER )
                    {
                        nTriggerServer::Instance()->DeleteEvent( event->GetId() );
                        break;
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Keep updating all the AI related entities at the specified update frequency
*/
void
nAITester::Trigger()
{
    if ( nConjurerApp::Instance()->GetCurrentState() != "game" )
    {
        if ( this->updatingFlags.IsFlagEnabled( GAMEPLAY ) )
        {
            RnsGameState* state = static_cast<RnsGameState*>( nConjurerApp::Instance()->FindState( "game" ) );
            n_assert( state );
            state->UpdateAI();
        }
#ifndef NGAME
        else if ( this->updatingFlags.IsFlagEnabled( SOUND_SOURCES ) )
        {
            nTriggerServer::Instance()->UpdateSoundSources();
        }
#endif
    }
}

//-----------------------------------------------------------------------------
/**
    Tell how many time per frame an AI update cycle must be done
    
    Call this method only once per frame (an internal counter is updated on
    each call).
*/
int
nAITester::GetAIUpdatesPerFrame()
{
    if ( !this->updatingFlags.IsFlagEnabled( GAMEPLAY ) )
    {
        return 0;
    }

    if ( this->ticksPerAIUpdate < 0 )
    {
        // A Negative number means accelerate AI -> update AI multiple times per frame
        return -this->ticksPerAIUpdate;
    }
    else if ( this->ticksPerAIUpdate > 0 )
    {
        // A positive number means slow down AI -> update AI once each several frames
        if ( ++this->ticks >= this->ticksPerAIUpdate )
        {
            this->ticks = 0;
            return 1;
        }
        return 0;
    }
    // A 0 means freeze AI -> do not update AI at all
    return 0;
}

//------------------------------------------------------------------------------
/**
    Get the AI update frequency
*/
int
nAITester::GetUpdateFrequency() const
{
    return this->ticksPerAIUpdate;
}

//------------------------------------------------------------------------------
/**
    Set the AI update frequency
*/
void
nAITester::SetUpdateFrequency( int ticksPerUpdate )
{
    this->ticksPerAIUpdate = ticksPerUpdate;
}

//-----------------------------------------------------------------------------
/**
    Tell if the gameplay updating is currently enabled
*/
bool
nAITester::IsGameplayUpdatingEnabled() const
{
    return this->updatingFlags.IsFlagEnabled( GAMEPLAY );
}

//-----------------------------------------------------------------------------
/**
    Tell if the sound sources are currently enabled
*/
bool
nAITester::AreSoundSourcesEnabled() const
{
    return this->updatingFlags.IsFlagEnabled( SOUND_SOURCES );
}

//-----------------------------------------------------------------------------
/**
    Enable gameplay debugging mode
*/
void
nAITester::EnableGameplayDebugging()
{
    if ( !this->updatingFlags.IsFlagEnabled( GAMEPLAY_DEBUG ) )
    {
        for ( nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject(); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
        {
#ifndef NGAME
            // Skip deleted entities
            ncEditor* editor = entity->GetComponent<ncEditor>();
            if ( !editor )
            {
                continue;
            }
            else if ( editor->IsDeleted() )
            {
                continue;
            }
#endif

#ifndef NGAME
            // Listen to transition event signals from gameplay entities
            if ( entity->GetComponent<ncFSM>() )
            {
                entity->BindSignal( ncFSM::SignalOnTransitionEvent, this, &nAITester::OnTransitionEvent, 0 );
            }
#endif
        }

        this->updatingFlags.EnableFlag( GAMEPLAY_DEBUG );
    }
}

//-----------------------------------------------------------------------------
/**
    Disable gameplay debugging mode
*/
void
nAITester::DisableGameplayDebugging()
{
    if ( this->updatingFlags.IsFlagEnabled( GAMEPLAY_DEBUG ) )
    {
        for ( nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject(); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
        {
#ifndef NGAME
            // Skip deleted entities
            ncEditor* editor = entity->GetComponent<ncEditor>();
            if ( !editor )
            {
                continue;
            }
            else if ( editor->IsDeleted() )
            {
                continue;
            }
#endif

#ifndef NGAME
            // Stop listening to transition event signals from gameplay entities
            if ( entity->GetComponent<ncFSM>() )
            {
                entity->UnbindTargetObject( ncFSM::SignalOnTransitionEvent.GetId(), this );
            }
#endif
        }

        // If there's any pending transition, do it now
        this->updatingFlags.DisableFlag( GAMEPLAY_DEBUG );
        while ( !this->pendingEvents.Empty() )
        {
            this->ApplyPendingEvent();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Tell if the gameplay debugging is enabled
*/
bool
nAITester::IsGameplayDebuggingEnabled() const
{
    return this->updatingFlags.IsFlagEnabled( GAMEPLAY_DEBUG );
}

//-----------------------------------------------------------------------------
/**
    Pause the FSM transitions if in gameplay debug mode
*/
void
nAITester::OnTransitionEvent( nEntityObject* entity, int event, bool* acceptEvent )
{
    // Tell the entity to accept or not the incoming event
    bool gameplayDebug = this->updatingFlags.IsFlagEnabled( GAMEPLAY_DEBUG );
    bool doNextGameplayStep = this->updatingFlags.IsFlagEnabled( DO_NEXT_GAMEPLAY_STEP );
    *acceptEvent = !gameplayDebug || doNextGameplayStep;

    // Store the incoming event
    if ( !*acceptEvent )
    {
        PendingEvent pendingEvent;
        pendingEvent.event = nGameEvent::Type( event );
        pendingEvent.entity = entity;
        this->pendingEvents.PushBack( pendingEvent );
        this->SignalPendingEventChanged( this );
    }

    // Do not allow to apply more than one event without user supervision
    this->updatingFlags.DisableFlag( DO_NEXT_GAMEPLAY_STEP );
}

//-----------------------------------------------------------------------------
/**
    If there's any pending transition event, make the owner entity proccess the next one
*/
void
nAITester::ApplyPendingEvent()
{
    if ( !this->pendingEvents.Empty() )
    {
        this->updatingFlags.EnableFlag( DO_NEXT_GAMEPLAY_STEP );
        PendingEvent& pendingEvent( this->pendingEvents.Front() );
        switch ( pendingEvent.event )
        {
            case nGameEvent::ACTION_DONE:
            case nGameEvent::ACTION_FAIL:
                pendingEvent.entity->GetComponentSafe<ncFSM>()->OnActionEvent( pendingEvent.event );
                break;
            default:
                pendingEvent.entity->GetComponentSafe<ncFSM>()->OnTransitionEvent( pendingEvent.event );
                break;
        }
        this->pendingEvents.Erase( 0 );
        this->updatingFlags.DisableFlag( DO_NEXT_GAMEPLAY_STEP );
        this->SignalPendingEventChanged( this );
    }
}

//-----------------------------------------------------------------------------
/**
    Discard the pending transition event
*/
void
nAITester::DiscardPendingEvent()
{
    if ( !this->pendingEvents.Empty() )
    {
        this->pendingEvents.Erase( 0 );
        this->SignalPendingEventChanged( this );
    }
}

//-----------------------------------------------------------------------------
/**
    Discard the pending transition event
*/
void
nAITester::GetPendingEvent( nEntityObjectId& entityId, nString& eventName )
{
    if ( this->pendingEvents.Empty() )
    {
        entityId = 0;
        eventName = "";
    }
    else
    {
        entityId = this->pendingEvents.Front().entity->GetId();
        eventName = nGameEvent::GetEventLabel( this->pendingEvents.Front().event );
    }
}

//-----------------------------------------------------------------------------
/**
    Tell if the FSM is in pause mode or may be updated
*/
bool
nAITester::IsFSMPaused() const
{
    return this->updatingFlags.IsFlagEnabled( GAMEPLAY_DEBUG ) && !this->pendingEvents.Empty();
}


//------------------------------------------------------------------------------
nAITester* nAITester::Instance()
{
    n_assert( nAITester::instance );
    return nAITester::instance;
}
//-----------------------------------------------------------------------------
#endif
