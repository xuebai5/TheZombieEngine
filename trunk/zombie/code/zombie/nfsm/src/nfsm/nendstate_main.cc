//-----------------------------------------------------------------------------
//  nendstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nendstate.h"

nNebulaScriptClass(nEndState, "nstate");

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nEndState::nEndState()
    : nState(nState::End)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Do all processing needed for the given entity entering this state
*/
void nEndState::OnEnter( nEntityObject* entity ) const
{
    n_assert( entity );

    nState::OnEnter( entity );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
