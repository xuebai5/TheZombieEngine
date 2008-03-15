#ifndef N_NODESTATE_H
#define N_NODESTATE_H

//------------------------------------------------------------------------------
/**
    @class nNodeState
    @ingroup NebulaFSMSystem

    State that contains a child FSM.

    The state actually doesn't have the FSM, just a FSM selection function.
    So when the FSM enters in a node state this selection function is called to
    choose which child FSM must be expanded.

    This selection function is a 'SelectFSM' named function contained within a
    script. This function receives the agent as a parameter and returns a
    reference to the FSM to be selected.

    (C) 2006 Conjurer Services, S.A.
*/

#include "nstate.h"

class nFSM;
class nFSMSelector;

//------------------------------------------------------------------------------
class nNodeState : public nState
{
public:
    /// Default constructor
    nNodeState();

    /// Select and return a reference to a FSM for the given entity
    nFSM* SelectFSM( nEntityObject* entity ) const;

    /// Set the FSM selector used to select children FSMs (replaces the previous one, but does NOT release it)
    void SetFSMSelector(nFSMSelector*);
    /// Get the FSM selector used to select children FSMs, or NULL if there isn't any
    nFSMSelector* GetFSMSelector () const;
    /// Set the FSM selector giving its name (used for persistence)
    void SetFSMSelectorByName(const char*);

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Selector of the children FSMs
    nFSMSelector* fsmSelector;
};

//-----------------------------------------------------------------------------
/**
    Get the FSM selector used to select children FSMs
    
    Return NULL if there isn't any FSM selector
*/
inline
nFSMSelector* nNodeState::GetFSMSelector() const
{
    return this->fsmSelector;
}

#endif
