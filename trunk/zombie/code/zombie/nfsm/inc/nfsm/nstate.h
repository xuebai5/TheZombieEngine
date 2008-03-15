#ifndef N_STATE_H
#define N_STATE_H

//------------------------------------------------------------------------------
/**
    @class nState
    @ingroup NebulaFSMSystem

    Base class for any state of a finite state machine.

    It's mostly a data container, any entity dependent logic is done by the
    ncFSM component.

    A FSM can have 3 state types:

     - Node state: State which contains a FSM, selected among several FSM on
       entering the state (the state actually doesn't have the FSM, just the
       selection function, but conceptually it has a child FSM).
     - Leaf state: State which contains an action.
     - End state: State that signals the end of the FSM which it belongs to.

    A state can also be the FSM's initial state, but this isn't a new state
    type, but which state the FSM starts at.

    All state types share the following:

     - Emotional actions: Actions intented to set some agent's emotion related
       parameters (i.e: change perception awareness). These actions are
       intended to be executed only once and when the agent enters into the
       state.
     - Transitions to other states, triggered by some event or scripted
       condition.

    (C) 2006 Conjurer Services, S.A.
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "util/nkeyarray.h"
#include "nfsm/keyarrayiterator.h"

class nTransition;
class nActionDesc;
class nEntityObject;

//------------------------------------------------------------------------------
class nState : public nRoot
{
public:
    /// Enum to represent the state type id
    enum TypeId
    {
        Node,
        Leaf,
        End
    };

    /// Type to iterate through the event transitions contained in a state
    typedef KeyArrayIterator< nTransition* > EventTransitionsIterator;

    /// Constructor
    nState();
    /// Destructor (releases actions and transitions)
    virtual ~nState();
    /// Get the type id of this state
    TypeId GetStateType() const;
    /// Return that event transition whose event matches with the given one, or NULL if no transition is found
    nTransition* GetEventTransition( int eventType ) const;
    /// Return the first script transition whose condition evaluates to true, or NULL if all evaluate to false
    nTransition* GetFirstTrueScriptTransition( nEntityObject* entity ) const;
    /// Do all processing needed for the given entity entering this state
    virtual void OnEnter( nEntityObject* entity ) const;
    /// Get an object to iterate through the event transitions contained in this state
    EventTransitionsIterator GetEventTransitionsIterator() const;

    /// Add a transition from this state to one or more target states (the transition MUST HAVE a condition)
    void AddTransition(nTransition*);
    /// Remove a transition from this state (it does NOT release the transition)
    void RemoveTransition(nTransition*);
    /// Delete a transition from this state (it releases the transition)
    void DeleteTransition(nTransition*);
    /// Add an emotional action to the state
    void AddEmotionalAction(nActionDesc*);
    /// Delete an emotional action from the state (it does NOT release the action)
    void RemoveEmotionalAction(nActionDesc*);
    /// Delete an emotional action from the state (it releases the action)
    void DeleteEmotionalAction(nActionDesc*);

    /// Add a transition by its name (used for persistence)
    void AddTransitionByName(const char*);

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

protected:
    /// Constructor
    nState( TypeId stateType );

private:
    /// Emotional actions descriptions
    nArray< nActionDesc* > emotionalActions;
    /// Transitions triggered by an event
    nKeyArray< nTransition* > eventTransitions;
    /// Transitions triggered by a script
    nArray< nTransition* > scriptTransitions;
    /// Type id of this state
    /*const*/ TypeId stateType; // Non const to avoid "assignment operator could not be generated" warning
};

//-----------------------------------------------------------------------------
/**
    Get the type id of this state
*/
inline
nState::TypeId nState::GetStateType() const
{
    return this->stateType;
}

//-----------------------------------------------------------------------------
/**
    Get an object to iterate through the event transitions contained in this state
*/
inline
nState::EventTransitionsIterator nState::GetEventTransitionsIterator() const
{
    return EventTransitionsIterator( this->eventTransitions );
}

#endif
