//-----------------------------------------------------------------------------
//  nfsm_main.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nfsm.h"
#include "nfsm/nstate.h"
#include "nfsmserver/nfsmserver.h"

nNebulaScriptClass(nFSM, "nroot");

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nFSM::nFSM()
    : initialState(NULL)
#ifndef NGAME
    , isDirty( false )
#endif
{
    this->SetSaveModeFlags( N_FLAG_SAVEUPSIDEDOWN );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    Delete all the states
*/
nFSM::~nFSM()
{
    for ( int i(0); i < this->states.Size(); ++i )
    {
        this->states[i]->Release();
    }
    this->states.Clear();
}

//-----------------------------------------------------------------------------
/**
    Add a state to the FSM
*/
void nFSM::AddState( nState* state )
{
    n_assert( state );
    n_assert( this->states.FindIndex(state) == -1 );
    this->states.PushBack(state);
}

//-----------------------------------------------------------------------------
/**
    Remove a state from the FSM
    
    It doesn't release the state
    It sets to NULL the initial state if it's the one deleted
*/
void nFSM::RemoveState( nState* state )
{
    n_assert( state );

    // Remove state from states array
    int index( this->states.FindIndex(state) );
    n_assert( index != -1 );
    this->states[index] = this->states[ this->states.Size()-1 ];
    this->states.Erase( this->states.Size()-1 );

    // Unset initial state if it's the state deleted
    if ( this->initialState == state )
    {
        this->initialState = NULL;
    }
}

//-----------------------------------------------------------------------------
/**
    Delete a state from the FSM
    
    It releases the state
    It sets to NULL the initial state if it's the one deleted
*/
void nFSM::DeleteState( nState* state )
{
    n_assert( state );
    this->RemoveState(state);
    state->Release();
}

//-----------------------------------------------------------------------------
/**
    Set the initial state

    The state isn't stored, it must already have been added by a call to AddState
*/
void nFSM::SetInitialState( nState* state )
{
    n_assert( this->states.FindIndex(state) != -1 );
    this->initialState = state;
}

//-----------------------------------------------------------------------------
/**
    Add a state by its name, creating the state if needed (used for persistence)
*/
void nFSM::AddStateByName( const char* stateName, int stateType )
{
    nState* state = nFSMServer::Instance()->GetState( this->GetName(), stateName, stateType );
    this->AddState( state );
}

//-----------------------------------------------------------------------------
/**
    Set the initial state by its name (used for persistence)

    The state must already have been added by a call to AddState
*/
void nFSM::SetInitialStateByName( const char* stateName )
{
    n_assert( stateName );
    for ( int i(0); i < this->states.Size(); ++i )
    {
        if ( strcmp( this->states[i]->GetName(), stateName ) == 0 )
        {
            this->initialState = this->states[i];
            return;
        }
    }
    n_assert_always();
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Tell if this FSM has changed since the last time it was saved
*/
bool nFSM::IsDirty() const
{
    return this->isDirty;
}
#endif

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Set the dirty flag (FSM has changed)
*/
void nFSM::SetDirty( bool dirty )
{
    this->isDirty = dirty;
}
#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
