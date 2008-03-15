//-----------------------------------------------------------------------------
//  nstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nstate.h"
#include "nfsm/ntransition.h"
#include "nfsm/nactiondesc.h"
#include "nfsm/neventcondition.h"
#include "nfsmserver/nfsmserver.h"

#ifndef __NEBULA_NO_LOG__
#include "ncfsm/ncfsm.h"
#endif

//-----------------------------------------------------------------------------
nNebulaScriptClass(nState, "nroot");


//-----------------------------------------------------------------------------
/**
    Constructor
*/
nState::nState() :
    stateType(Leaf),  
    eventTransitions(0,1)
{
    this->SetSaveModeFlags( N_FLAG_SAVEUPSIDEDOWN );
}

//-----------------------------------------------------------------------------
/**
    Constructor
*/
nState::nState( TypeId stateType )
    : stateType(stateType), eventTransitions(0,1)
{
    this->SetSaveModeFlags( N_FLAG_SAVEUPSIDEDOWN );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    Releases all actions and transitions
*/
nState::~nState()
{
    // Delete actions
    for ( int i(0); i < this->emotionalActions.Size(); ++i )
    {
        this->emotionalActions[i]->Release();
    }
    this->emotionalActions.Clear();

    // Delete event transitions
    for ( int i(0); i < this->eventTransitions.Size(); ++i )
    {
        this->eventTransitions.GetElementAt(i)->Release();
    }
    this->eventTransitions.Clear();

    // Delete script transitions
    for ( int i(0); i < this->scriptTransitions.Size(); ++i )
    {
        this->scriptTransitions[i]->Release();
    }
    this->scriptTransitions.Clear();
}

//-----------------------------------------------------------------------------
/**
    Add a transition from this state to one or more target states

    The transition MUST HAVE a condition and if that condition changes the
    transition must be reinserted to the state.
*/
void nState::AddTransition( nTransition* transition )
{
    n_assert( transition );
    n_assert( transition->GetCondition() );

    // Add the transition to the correct container for its condition type
    switch ( transition->GetCondition()->GetConditionType() )
    {
        case nCondition::Event:
            {
                int eventType = static_cast<nEventCondition*>( transition->GetCondition() )->GetEvent();
                n_assert( this->GetEventTransition( eventType ) == NULL );
                this->eventTransitions.Add( eventType, transition );
            }
            break;
        case nCondition::Script:
            n_assert( this->scriptTransitions.FindIndex(transition) == -1 );
            this->scriptTransitions.PushBack( transition );
            break;
        default:
            n_error( "It's just impossible to reach this line?!?! Maybe has a new condition type been added?" );
    }
}

//-----------------------------------------------------------------------------
/**
    Remove a transition from this state
    
    It does NOT release the transition
*/
void nState::RemoveTransition( nTransition* transition )
{
    n_assert( transition );
    n_assert( transition->GetCondition() );

    // Delete the transition from the correct container for its condition type
    switch ( transition->GetCondition()->GetConditionType() )
    {
        case nCondition::Event:
            {
                int eventType = static_cast<nEventCondition*>( transition->GetCondition() )->GetEvent();
                n_assert( this->GetEventTransition( eventType ) == transition );
                this->eventTransitions.Rem( eventType );
            }
            break;
        case nCondition::Script:
            {
                int index( this->scriptTransitions.FindIndex(transition) );
                n_assert( index != -1 );
                this->scriptTransitions.Erase(index);
            }
            break;
        default:
            n_error( "It's just impossible to reach this line?!?! Maybe has a new condition type been added?" );
    }
}

//-----------------------------------------------------------------------------
/**
    Delete a transition from this state
    
    It releases the transition
*/
void nState::DeleteTransition( nTransition* transition )
{
    n_assert( transition );
    this->RemoveTransition( transition );
    transition->Release();
}

//-----------------------------------------------------------------------------
/**
    Add an emotional action to the state
*/
void nState::AddEmotionalAction( nActionDesc* actionDesc )
{
    n_assert( actionDesc );
    n_assert( this->emotionalActions.FindIndex(actionDesc) == -1 );
    this->emotionalActions.PushBack( actionDesc );
}

//-----------------------------------------------------------------------------
/**
    Remove an emotional action from the state
    
    It does NOT release the action
*/
void nState::RemoveEmotionalAction( nActionDesc* actionDesc )
{
    n_assert( actionDesc );
    int index( this->emotionalActions.FindIndex(actionDesc) );
    n_assert( index != -1 );
    this->emotionalActions.Erase(index);
}

//-----------------------------------------------------------------------------
/**
    Delete an emotional action from the state
    
    It releases the action
*/
void nState::DeleteEmotionalAction( nActionDesc* actionDesc )
{
    n_assert( actionDesc );
    this->RemoveEmotionalAction( actionDesc );
    actionDesc->Release();
}

//-----------------------------------------------------------------------------
/**
    Return that event transition whose event matches with the given one
    
    Return NULL if there isn't any transition that answers to the given event
*/
nTransition* nState::GetEventTransition( int eventType ) const
{
    nTransition* transition;
    // Ugly cast, but nKeyArray doesn't have a const Find method
    if ( const_cast<nState*>(this)->eventTransitions.Find( eventType, transition ) )
    {
        return transition;
    }
    else
    {
        return NULL;
    }
}

//-----------------------------------------------------------------------------
/**
    Return the first script transition whose condition evaluates to true
    
    Return NULL if all script transitions evaluate to false
*/
nTransition* nState::GetFirstTrueScriptTransition( nEntityObject* entity ) const
{
    for ( int i(0); i < this->scriptTransitions.Size(); ++i )
    {
        n_assert( this->scriptTransitions[i] );
        n_assert( this->scriptTransitions[i]->GetCondition() );

        NLOG( fsm, (nFSMServer::TransitionsLog | 3, "%s evaluating %s",
            entity->GetComponentSafe<ncFSM>()->GetLogPrefix(),
            this->scriptTransitions[i]->GetCondition()->GetName()) );

        nArg inArg;
        inArg.SetO( entity );
        nArg outArg;
        this->scriptTransitions[i]->GetCondition()->CallArgs( "evaluate", 1, &outArg, 1, &inArg );
        bool result = outArg.GetB();

        NLOG( fsm, (nFSMServer::TransitionsLog | 2, "%s evaluates %s to %d",
            entity->GetComponentSafe<ncFSM>()->GetLogPrefix(),
            this->scriptTransitions[i]->GetCondition()->GetName(), result) );

        if ( result )
        {
            return this->scriptTransitions[i];
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------
/**
    Do all processing needed for the given entity entering this state
*/
void nState::OnEnter (nEntityObject* entity) const
{
    n_assert( entity );

    // Apply emotional actions to the entity
    for ( int i(0); i < this->emotionalActions.Size(); ++i )
    {
        this->emotionalActions[i]->ApplyAction (entity, true);
    }
}

//-----------------------------------------------------------------------------
/**
    Add a transition by its name (used for persistence)
*/
void nState::AddTransitionByName( const char* transitionName )
{
    nTransition* transition = nFSMServer::Instance()->FindTransition( this, transitionName );
    this->AddTransition( transition );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
