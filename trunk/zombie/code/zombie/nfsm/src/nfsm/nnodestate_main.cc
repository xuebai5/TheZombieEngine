//-----------------------------------------------------------------------------
//  nnodestate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nnodestate.h"
#include "nfsm/nfsm.h"
#include "nfsm/nfsmselector.h"
#include "nfsmserver/nfsmserver.h"

nNebulaScriptClass(nNodeState, "nstate");

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nNodeState::nNodeState()
    : nState(nState::Node), fsmSelector(NULL)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Select and return a reference to a FSM for the given entity
*/
nFSM* nNodeState::SelectFSM( nEntityObject* entity ) const
{
    if ( this->fsmSelector )
    {
        return this->fsmSelector->Select( entity );
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
/**
    Set the FSM selector used to select children FSMs
    
    Replaces the previous one, but does NOT release it
*/
void nNodeState::SetFSMSelector( nFSMSelector* selector )
{
    this->fsmSelector = selector;
}

//-----------------------------------------------------------------------------
/**
    Set the FSM selector giving its name (used for persistence)
*/
void nNodeState::SetFSMSelectorByName( const char* selectorName )
{
    // Needed to capitalize names to keep backwards compatibility with versions
    // prior to the use of the nScriptClassServer
    nString capitalizedName( selectorName );
    capitalizedName.ToCapital();

    this->fsmSelector = nFSMServer::Instance()->GetFSMSelector( capitalizedName.Get() );
}

//-----------------------------------------------------------------------------
