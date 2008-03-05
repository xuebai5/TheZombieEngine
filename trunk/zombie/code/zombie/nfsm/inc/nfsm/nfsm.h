#ifndef N_FSM_H
#define N_FSM_H

//------------------------------------------------------------------------------
/**
    @class nFSM
    @ingroup NebulaFSMSystem

    Finite state machine.

    This class just hold the data of a FSM, which can be shared among all
    entities that use it, but it doesn't contain the logic to run it. To use it
    you need the ncFSM component.

    This FSM is probabilitic and hierarchical.

    Probabilistic means that a transition can have a probability to fire when
    several transition conditions become true simultaneously. Use case for
    probabilitic FSM:

     - For some situation is wanted that a creature most times does some action,
       but occasionaly does another action.
     - This is solved by having two target states for that situation (transition
       condition) and attach a probability to each one of them.

    Hierarchical means that a state can hold an action or another child FSM.
    Actually, the children FSMs are not fixed, but selected among several FSMs
    when entering a state. This selection is done by a script. Anyway, at parent
    level the child FSM is just seen like an action, so when a FSM ends (it
    enters a special end state) at parent level it's seen as the "action" has
    ended.

    The FSM doesn't know about how actions are implemented (just the parameters
    their need to be initialized). And actions can be (and are) either native or
    scripted.

    Transition can be triggered by events or be checked periodically. The first
    ones are native, while the last ones are scripted. For the last, the order
    in which are checked defines its priority (although who handles the event
    condition response and script condition checking is the ncFSM component).

    @see ncFSM to know more about the agent dependent FSM part.
    @see ncFSMClass to know more about the agent type dependent FSM part.
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "util/narray.h"

class nPersistServer;
class nState;

//------------------------------------------------------------------------------
class nFSM : public nRoot
{
public:
    /// Default constructor
    nFSM();
    /// Destructor (deletes states)
    ~nFSM();

    /// Add a state to the FSM
    void AddState(nState*);
    /// Remove a state from the FSM (it does NOT release the state)
    void RemoveState(nState*);
    /// Delete a state from the FSM (it releases the state)
    void DeleteState(nState*);
    /// Set the initial state
    void SetInitialState(nState*);
    /// Return the initial state, or NULL if there isn't any initial state
    nState* GetInitialState () const;

    /// Add a state by its name, creating the state if needed (used for persistence)
    void AddStateByName(const char*, int);
    /// Set the initial state by its name (used for persistence)
    void SetInitialStateByName(const char*);

#ifndef NGAME
    // Commands used only by the editor to know when the FSM has changed and needs to be saved
    /// Tell if this FSM has changed since the last time it was saved
    bool IsDirty () const;
    /// Set the dirty flag (FSM has changed)
    void SetDirty(bool);
#endif

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// The states this FSM has
    nArray<nState*> states;
    /// Which state the FSM has to start from
    nState* initialState;

#ifndef NGAME
    bool isDirty;
#endif
};

//-----------------------------------------------------------------------------
/**
    Return the initial state
    
    Return NULL if there isn't any initial state
*/
inline
nState* nFSM::GetInitialState() const
{
    return this->initialState;
}

#endif
