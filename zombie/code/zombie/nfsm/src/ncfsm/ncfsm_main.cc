//-----------------------------------------------------------------------------
//  ncfsm_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "ncfsm/ncfsm.h"
#include "ncfsm/ncfsmclass.h"
#include "nfsm/nfsm.h"
#include "nfsm/nnodestate.h"
#include "nfsm/ntransition.h"
#include "nfsm/neventcondition.h"
#include "ntrigger/ngameevent.h"
#include "nfsmserver/nfsmserver.h"
#include "napplication/napplication.h"

#ifndef NGAME
#include "gfx2/ngfxserver2.h"
#endif

#ifndef __NEBULA_NO_LOG__
#include "rnsgameplay/ncgameplay.h"
#endif

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncFSM,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncFSM)
    NSCRIPT_ADDCMD_COMPOBJECT('EEVT', void, OnTransitionEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EACT', void, OnActionEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EFSM', void, OnFSMDoneEvent , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EIBE', void, StartBehavior , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESBE', void, SetBehavior, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('FGTE', float, GetStateTimeElapsed , 0, (), 0, ());
#ifndef NGAME
    cl->BeginSignals(1);
    NCOMPONENT_ADDSIGNAL( OnTransitionEvent )
    cl->EndSignals();
#endif
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
#ifndef NGAME
NSIGNAL_DEFINE( ncFSM, OnTransitionEvent );
#endif

//-----------------------------------------------------------------------------
#ifdef __NEBULA_STATS__
namespace
{
    nProfiler profTotal;
    nProfiler profEvents;
    nProfiler profPooling;
}
#endif

//-----------------------------------------------------------------------------
/**
   Default constructor
*/
ncFSM::ncFSM() :
    ticksToCheckConditions(0),
        lastActionEvent( nGameEvent::INVALID_TYPE )
{
#ifdef __NEBULA_STATS__
    if ( !profTotal.IsValid() )
    {
        profTotal.Initialize( "profAIFSM", true );
        profEvents.Initialize( "profAIFSMEvents", true );
        profPooling.Initialize( "profAIFSMPooling", true );
    }
#endif
}

//-----------------------------------------------------------------------------
void 
ncFSM::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType == nObject::ReloadedInstance)
    {
        this->Reset();
    }
}

//-----------------------------------------------------------------------------
/**
    Set the FSM out of any state
    
    You need to call to Init again to restart the FSM
*/
void
ncFSM::Reset()
{
    while ( !this->statesStack.Empty() )
    {
        this->ExitTopState();
        this->incomingEvents.Clear();
        this->lastActionEvent = nGameEvent::INVALID_TYPE;
    }
}

//-----------------------------------------------------------------------------
/**
    Set initial state.
*/
void
ncFSM::Init()
{
    // Set the ticks left to check for script conditions
    ncFSMClass* componentClass = this->GetClassComponent<ncFSMClass>();
    n_assert( componentClass );
    this->ticksToCheckConditions = componentClass->GetConditionPoolingFrequency();

    // Set the initial state
    this->EnterFSM( componentClass->GetParentFSM() );
}

//-----------------------------------------------------------------------------
/**
    Same as Init, but with a more intuitive name to be used by designers
*/
void
ncFSM::StartBehavior()
{
    this->Init();
}

//-----------------------------------------------------------------------------
/**
    Get the time elapsed since the fsm enters in current state
*/
float
ncFSM::GetStateTimeElapsed()
{
    return static_cast<float> ( nApplication::Instance()->GetTime() ) - this->initTime; 
}


//-----------------------------------------------------------------------------
/**
    Replace the current FSM by another one and start executing it.
*/
void
ncFSM::SetBehavior( const char* fsmName )
{
    // Exit from current FSM
    this->Reset();

    // Get new FSM
    n_assert( fsmName );
    nFSM* fsm( nFSMServer::Instance()->GetFSM(fsmName) );

#ifndef NGAME
    // Validate new FSM
    if ( !fsm )
    {
        nString msg( "FSM '" + nString(fsmName) + "' not found" );
        n_error( msg.Get() );
        return;
    }
#endif

    // Enter to new FSM
    this->EnterFSM( fsm );
}

//-----------------------------------------------------------------------------
/**
    Make the FSM transitionate if some event or script transition triggers

    At most, one state transition is done per update
*/
void
ncFSM::Update()
{
#ifdef __NEBULA_STATS__
    profTotal.StartAccum();
#endif

    if ( !this->ProcessTransitionEvents() )
    {
        if ( !this->ProcessLastActionEvent() )
        {
            this->ProcessTransitionScripts();
        }
    }

#ifdef __NEBULA_STATS__
    profTotal.StopAccum();
#endif
}

//-----------------------------------------------------------------------------
/**
    Process the last action event received since the last update process

    @return True if a state transition has been followed, false otherwise
*/
bool
ncFSM::ProcessLastActionEvent()
{
    bool eventHandled = false;

#ifdef __NEBULA_STATS__
    profEvents.StartAccum();
#endif

    if ( this->lastActionEvent != nGameEvent::INVALID_TYPE )
    {
        nGameEvent::Type actionEvent = this->lastActionEvent;
        this->lastActionEvent = nGameEvent::INVALID_TYPE;
        eventHandled = this->ApplyActionEvent( actionEvent );
    }

#ifdef __NEBULA_STATS__
    profEvents.StopAccum();
#endif

    return eventHandled;
}

//-----------------------------------------------------------------------------
/**
    Process the non action events received since the last update process

    It postposes processing of next events when some event causes a state
    transition.

    @return True if a state transition has been followed, false otherwise
*/
bool
ncFSM::ProcessTransitionEvents()
{
    bool eventHandled = false;

#ifdef __NEBULA_STATS__
    profEvents.StartAccum();
#endif

    while ( !this->incomingEvents.Empty() && !eventHandled )
    {
        nGameEvent::Type event = this->incomingEvents.Front();
        this->incomingEvents.Erase(0);
        eventHandled = this->ApplyTransitionEvent( event );
    }

#ifdef __NEBULA_STATS__
    profEvents.StopAccum();
#endif

    return eventHandled;
}

//-----------------------------------------------------------------------------
/**
    Evaluate transition scripts and transitionate if anyone triggers

    Scripts are evaluated only once every X ticks, where X is defined by
    ncFSMClass::GetConditionPoolingFrequency()

    @return True if a state transition has been followed, false otherwise
*/
bool
ncFSM::ProcessTransitionScripts()
{
    bool eventHandled = false;

#ifdef __NEBULA_STATS__
    profPooling.StartAccum();
#endif

    if ( --this->ticksToCheckConditions <= 0 )
    {
        // Check script conditions and transitionate if anyone triggers.
        // Start looking from the parent state, since parent states have more priority.
        for ( int i = 0; i < this->statesStack.Size() && !eventHandled; ++i )
        {
            n_assert( this->statesStack[i].state );
            nTransition* triggeredTransition = this->statesStack[i].state->GetFirstTrueScriptTransition( this->GetEntityObject() );
            eventHandled = this->FollowTransition( triggeredTransition, i );
        }

        // Reset the ticks left to check for script conditions
        ncFSMClass* componentClass = this->GetClassComponent<ncFSMClass>();
        n_assert( componentClass );
        this->ticksToCheckConditions = componentClass->GetConditionPoolingFrequency();
    }

#ifdef __NEBULA_STATS__
    profPooling.StopAccum();
#endif

    return eventHandled;
}

//-----------------------------------------------------------------------------
/**
    Make the entity this component belongs to enter in the FSM's initial state
*/
void
ncFSM::EnterFSM( nFSM* fsm )
{
    n_assert( fsm );
    if ( fsm )
    {
        NLOG( fsm, (nFSMServer::StatesLog | 2, "%s entering <%s>",
            this->GetLogPrefix(1), fsm->GetName()) );

        nState* initialState = fsm->GetInitialState();
        n_assert( initialState );
        if ( initialState )
        {
            this->EnterState( initialState );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Make the entity this component belongs to enter in the given state
*/
void
ncFSM::EnterState( nState* state )
{
#ifndef __NEBULA_NO_LOG__
    nFSM* fsm = nFSMServer::Instance()->FindFSMOfState( state );
    n_assert( fsm );
    NLOG( fsm, (nFSMServer::StatesLog | 0, "%s entering <%s,%s>",
        this->GetLogPrefix(1), fsm->GetName(), state->GetName()) );
#endif

    // Any action event received from last state is invalid for the new one
    this->lastActionEvent = nGameEvent::INVALID_TYPE;

    // Pushes the state into the states stack
    this->statesStack.PushBack( ActiveState(state) );

    // Do state specific stuff and effectively set the entity into the state
    state->OnEnter( this->GetEntityObject() );
    this->initTime = static_cast<float> ( nApplication::Instance()->GetTime() );

    // If the state is a node state expand it to its child FSM
    if ( state->GetStateType() == nState::Node )
    {
        this->EnterFSM( static_cast<nNodeState*>(state)->SelectFSM( this->GetEntityObject() ) );
    }
    // @todo Remove when the end state throws the FSMDone event.
    //       Meanwhile simulate it by calling directly to OnFSMDoneEvent when entering an end state.
    else if ( state ->GetStateType() == nState::End )
    {
        this->OnFSMDoneEvent();
    }
}

//-----------------------------------------------------------------------------
/**
    Make the entity this component belongs to exit from its most child active state
*/
void
ncFSM::ExitTopState()
{
    n_assert( !this->statesStack.Empty() );

    if ( !this->statesStack.Empty() )
    {
#ifndef __NEBULA_NO_LOG__
        nState* state = this->statesStack.Back().state;
        n_assert( state );
        nFSM* fsm = nFSMServer::Instance()->FindFSMOfState( state );
        n_assert( fsm );
        NLOG( fsm, (nFSMServer::StatesLog | 2, "%s leaving <%s,%s>",
            this->GetLogPrefix(), fsm->GetName(), state->GetName()) );
#endif

        // Any action event received from last state is invalid when exiting the state that contained that action
        this->lastActionEvent = nGameEvent::INVALID_TYPE;

        // Poppes the top state from the states stack
        this->statesStack.EraseQuick( this->statesStack.Size() - 1 );
    }
}

//-----------------------------------------------------------------------------
/**
    Make the FSM follow a transition, if it selects a target.

    The transition may be NULL (of course, in that case the transition is not followed).
    A stack level where the transition belongs to must be given. If the transition
    is followed, the state of that level and all its children will be replaced for the
    new target state and its children.

    Return true when the transition has been followed, false otherwise.
*/
bool
ncFSM::FollowTransition( nTransition* transition, int stackLevel )
{
    if ( transition )
    {
        nCondition* condition = transition->GetCondition();
        if ( condition->GetConditionType() == nCondition::Event )
        {
            if ( !static_cast<nEventCondition*>(condition)->Evaluate( this->GetEntityObject() ) )
            {
                return false;
            }
        }

        nState* targetState = transition->SelectTarget();
        if ( targetState )
        {
#ifndef __NEBULA_NO_LOG__
            nState* state = this->statesStack[stackLevel].state;
            n_assert( state );
            nFSM* fsm = nFSMServer::Instance()->FindFSMOfState( state );
            n_assert( fsm );
            nCondition* condition = transition->GetCondition();
            NLOG( fsm, (nFSMServer::TransitionsLog | 0, "%s following %s from <%s,%s> to <%s,%s>",
                this->GetLogPrefix( 1 + stackLevel - this->statesStack.Size() ), condition->GetName(),
                fsm->GetName(), state->GetName(), fsm->GetName(), targetState->GetName() ) );
#endif

            // Exit from the state whose transition has triggered and from all its children
            while ( this->statesStack.Size() > stackLevel )
            {
                this->ExitTopState();
            }
            // Enter into the target state chosen by the triggered transition
            this->EnterState( targetState );
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Handle all events (but Done) belonging to some of the current states' transitions.
*/
void
ncFSM::OnTransitionEvent( int eventType )
{
#ifdef __NEBULA_STATS__
    bool profTotalStarted( profTotal.IsStarted() );
    bool profEventsStarted( profEvents.IsStarted() );
    if ( !profTotalStarted )
    {
        profTotal.StartAccum();
    }
    if ( !profEventsStarted )
    {
        profEvents.StartAccum();
    }
#endif

#ifndef NGAME
    bool acceptEvent( true );
    this->SignalOnTransitionEvent( this->GetEntityObject(), this->GetEntityObject(), eventType, &acceptEvent );
    if ( !acceptEvent )
    {
        return;
    }
#endif

    NLOG( fsm, (nFSMServer::EventsLog | 0, "%s receiving event %s",
        this->GetLogPrefix(), nGameEvent::GetEventLabel( nGameEvent::Type(eventType) ) ) );

#ifndef NGAME
    if ( !nFSMServer::Instance()->GetEnqueueEvents() )
    {
        this->ApplyTransitionEvent( nGameEvent::Type(eventType) );
    }
    else
#endif
    {
        this->incomingEvents.Append( nGameEvent::Type(eventType) );
    }

#ifdef __NEBULA_STATS__
    if ( !profEventsStarted )
    {
        profEvents.StopAccum();
    }
    if ( !profTotalStarted )
    {
        profTotal.StopAccum();
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Make the FSM transitionate if some transition matches the given non action event
*/
bool
ncFSM::ApplyTransitionEvent( nGameEvent::Type eventType )
{
    bool eventHandled = false;

    NLOG( fsm, (nFSMServer::EventsLog | 1, "%s processing event %s",
        this->GetLogPrefix(), nGameEvent::GetEventLabel( nGameEvent::Type(eventType) ) ) );

    // Look for that event transition whose event matches with the catched one,
    // beginning from the parent state, since parent transitions have more priority.
    for ( int i = 0; i < this->statesStack.Size() && !eventHandled; ++i )
    {
        n_assert( this->statesStack[i].state );
        nTransition* triggeredTransition = this->statesStack[i].state->GetEventTransition( eventType );
        eventHandled = this->FollowTransition( triggeredTransition, i );
    }

    return eventHandled;
}

//-----------------------------------------------------------------------------
/**
    Handle the event relating to an action finishing (ActionDone/ActionFail).
*/
void
ncFSM::OnActionEvent( int eventType )
{
#ifdef __NEBULA_STATS__
    bool profTotalStarted( profTotal.IsStarted() );
    bool profEventsStarted( profEvents.IsStarted() );
    if ( !profTotalStarted )
    {
        profTotal.StartAccum();
    }
    if ( !profEventsStarted )
    {
        profEvents.StartAccum();
    }
#endif

#ifndef NGAME
    bool acceptEvent( true );
    this->SignalOnTransitionEvent( this->GetEntityObject(), this->GetEntityObject(), eventType, &acceptEvent );
    if ( !acceptEvent )
    {
        return;
    }
#endif
    //n_assert( !this->statesStack.Empty() );

    NLOG( fsm, (nFSMServer::EventsLog | 0, "%s receiving event %s",
        this->GetLogPrefix(), nGameEvent::GetEventLabel( nGameEvent::Type(eventType) ) ) );

#ifndef NGAME
    if ( this->lastActionEvent != nGameEvent::INVALID_TYPE )
    {
        NLOG( fsm, (nFSMServer::ErrorsLog | 0, "%s receiving multiple action events on the same frame. Old event = %s. New event = %s.",
            this->GetLogPrefix(), nGameEvent::GetEventLabel( nGameEvent::Type(this->lastActionEvent) ),
            nGameEvent::GetEventLabel( nGameEvent::Type(eventType) ) ) );
    }

    if ( !nFSMServer::Instance()->GetEnqueueEvents() )
    {
        this->ApplyActionEvent( nGameEvent::Type(eventType) );
    }
    else
#endif
    {
        this->lastActionEvent = nGameEvent::Type(eventType);
    }

#ifdef __NEBULA_STATS__
    if ( !profEventsStarted )
    {
        profEvents.StopAccum();
    }
    if ( !profTotalStarted )
    {
        profTotal.StopAccum();
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Make the FSM transitionate if some transition matches the given action event
*/
bool
ncFSM::ApplyActionEvent( nGameEvent::Type eventType )
{
    bool eventHandled = false;

    NLOG( fsm, (nFSMServer::EventsLog | 1, "%s processing event %s",
        this->GetLogPrefix(), nGameEvent::GetEventLabel( nGameEvent::Type(eventType) ) ) );

    // Look for that event transition which answers to the given action event,
    // looking only in the most child state, since an action event always
    // refers to the action belonging to that state.
    if ( !this->statesStack.Empty() )
    {
        nState* topState = this->statesStack.Back().state;
        n_assert( topState );
        nTransition* transition = topState->GetEventTransition( eventType );
        eventHandled = this->FollowTransition( transition, this->statesStack.Size() - 1 );
    }

    return eventHandled;
}

//-----------------------------------------------------------------------------
/**
    Handle the event thrown because of entering an end state (FSMDone).
*/
void
ncFSM::OnFSMDoneEvent()
{
    n_assert( !this->statesStack.Empty() );

    // Pop top of states stack, since it can be said to be the end state which
    // has thrown the FSMDone event. Then throw an ActionDone event to signal
    // that the action (represented as the child FSM) has finished.
    if ( !this->statesStack.Empty() )
    {
#ifndef __NEBULA_NO_LOG__
        nState* state = this->statesStack.Back().state;
        n_assert( state );
        nFSM* fsm = nFSMServer::Instance()->FindFSMOfState( state );
        n_assert( fsm );
        NLOG( fsm, (nFSMServer::StatesLog | 1, "%s ending <%s>",
            this->GetLogPrefix(), fsm->GetName()) );
#endif

        this->ExitTopState();
        // @todo Make the central triggerer throw an ActionDone event coming from the entity this component belongs to.
        //       Meanwhile simulate it by calling directly to OnActionEvent.
        this->OnActionEvent( nGameEvent::ACTION_DONE );
    }
}

#ifndef NGAME

//-----------------------------------------------------------------------------
/**
    Display the states stack
*/
void
ncFSM::Draw( const vector4& color, const vector2& pos ) const
{
    // Build states stack's labels
    nArray<nString> text;
    if ( this->statesStack.Empty() )
    {
        text.Append( "State level 0: <none>" );
    }
    else
    {
        for ( int i(0); i < this->statesStack.Size(); ++i )
        {
            nState* state = this->statesStack[i].state;
            n_assert( state );
            nFSM* fsm = nFSMServer::Instance()->FindFSMOfState( state );
            n_assert( fsm );
            nString level(i);
            text.Append( "State level " + level + ": <" + fsm->GetName() + "," + state->GetName() + ">" );
        }
    }

    // Draw labels
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    vector2 pos2 = pos;
    for ( int i(0); i < text.Size(); ++i )
    {
        pos2.y -= gfxServer->GetTextExtent( text[i].Get() ).y * 2;
        gfxServer->Text( text[i].Get(), color, pos2.x, pos2.y );
    }
}
#endif

#ifndef __NEBULA_NO_LOG__
//-----------------------------------------------------------------------------
/**
    Get the prefix added to all log messages
*/
const char*
ncFSM::GetLogPrefix( int indentOffset ) const
{
    static nString prefix;
    prefix.Clear();

    // Indentation to show current stack level
    int fsmLevel = this->statesStack.Size() - 1 + indentOffset;
    for ( int i(0); i < fsmLevel; ++i )
    {
        prefix.Append("  ");
    }

    // Agent's gameplay name
    prefix.Append( this->GetComponentSafe<ncGameplay>()->GetName().Get() );

    return prefix.Get();
}
#endif // !__NEBULA_NO_LOG__
