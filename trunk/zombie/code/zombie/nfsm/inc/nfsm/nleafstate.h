#ifndef N_LEAFSTATE_H
#define N_LEAFSTATE_H

//------------------------------------------------------------------------------
/**
    @class nLeafState
    @ingroup NebulaFSMSystem

    State that contains a behavioural action.

    A behavioural action is that action executed each frame and intended to make
    the agent which is attached to show some behaviour.
*/

#include "nstate.h"

//------------------------------------------------------------------------------
class nLeafState : public nState
{
public:
    /// Default constructor
    nLeafState();
    /// Destructor (releases the behavioural action)
    virtual ~nLeafState();

    /// Do all processing needed for the given entity entering this state
    virtual void OnEnter( nEntityObject* entity ) const;

    /// Set the behavioural action contained in this state (releases the previous one, if there's any)
    void SetBehaviouralAction(nActionDesc*);
    /// Get the behavioural action contained in this state, or NULL if there isn't any
    nActionDesc* GetBehaviouralAction () const;

    /// Set the behavioural action contained in this state (used for persistence)
    void SetBehaviouralActionByName(const char*);

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Releases the behavioural action, if there's any
    void DeleteBehaviouralAction();

    /// Behavioural action contained in this state
    nActionDesc* behaviouralAction;
};

//-----------------------------------------------------------------------------
/**
    Get the behavioural action contained in this state

    Return NULL if there isn't any behavioural action
*/
inline
nActionDesc* nLeafState::GetBehaviouralAction() const
{
    return this->behaviouralAction;
}

#endif
