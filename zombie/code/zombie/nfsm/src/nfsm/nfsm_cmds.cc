//-----------------------------------------------------------------------------
//  nfsm_cmds.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nfsm.h"
#include "nfsm/nstate.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nFSM )

    NSCRIPT_ADDCMD('ADDS', void, AddState, 1, (nState*), 0, ());
    NSCRIPT_ADDCMD('REMS', void, RemoveState, 1, (nState*), 0, ());
    NSCRIPT_ADDCMD('DELS', void, DeleteState, 1, (nState*), 0, ());
    NSCRIPT_ADDCMD('SETI', void, SetInitialState, 1, (nState*), 0, ());
    NSCRIPT_ADDCMD('GETI', nState*, GetInitialState , 0, (), 0, ());
    NSCRIPT_ADDCMD('EAST', void, AddStateByName, 2, (const char*, int), 0, ());
    NSCRIPT_ADDCMD('ESIS', void, SetInitialStateByName, 1, (const char*), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('EISD', bool, IsDirty , 0, (), 0, ());
    NSCRIPT_ADDCMD('ESDI', void, SetDirty, 1, (bool), 0, ());
#endif

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool nFSM::SaveCmds( nPersistServer* ps )
{
    // Add all states
    for ( int i(0); i < this->states.Size(); ++i )
    {
        ps->Put( this, 'EAST', this->states[i]->GetName(), this->states[i]->GetStateType() );
    }

    // Initial state
    if ( this->initialState )
    {
        ps->Put( this, 'ESIS', this->initialState->GetName() );
    }

    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
