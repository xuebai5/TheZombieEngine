#ifndef N_ENDSTATE_H
#define N_ENDSTATE_H

//------------------------------------------------------------------------------
/**
    @class nEndState
    @ingroup NebulaFSMSystem

    State that represents the end of a FSM.

    Although the state can contain emotional actions and state transitions, they
    really seem useless for this state, so an end state tipically has no data,
    it just is used by the ncFSM to know that an end state has been reached and
    that the FSM must be popped and signaled its completion.

    (C) 2006 Conjurer Services, S.A.
*/

#include "nstate.h"

//------------------------------------------------------------------------------
class nEndState : public nState
{
public:
    // Default constructor
    nEndState();

    /// Do all processing needed for the given entity entering this state
    virtual void OnEnter( nEntityObject* entity ) const;

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );
};

#endif
