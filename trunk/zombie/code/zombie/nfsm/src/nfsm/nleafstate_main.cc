//-----------------------------------------------------------------------------
//  nleafstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nleafstate.h"
#include "nfsm/nactiondesc.h"
#include "nfsmserver/nfsmserver.h"

//-----------------------------------------------------------------------------
nNebulaScriptClass(nLeafState, "nstate");

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nLeafState::nLeafState()
    : nState(nState::Leaf), behaviouralAction(NULL)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    Deletes the behavioural action
*/
nLeafState::~nLeafState()
{
    this->DeleteBehaviouralAction();
}

//-----------------------------------------------------------------------------
/**
    Releases the behavioural action, if there's any
*/
void nLeafState::DeleteBehaviouralAction()
{
    if ( this->behaviouralAction )
    {
        this->behaviouralAction->Release();
    }
    this->behaviouralAction = NULL;
}

//-----------------------------------------------------------------------------
/**
    Set the behavioural action contained in this state (releases the previous one, if there's any)
*/
void nLeafState::SetBehaviouralAction( nActionDesc* actionDesc )
{
    this->DeleteBehaviouralAction();
    this->behaviouralAction = actionDesc;
}

//-----------------------------------------------------------------------------
/**
    Do all processing needed for the given entity entering this state
*/
void nLeafState::OnEnter( nEntityObject* entity ) const
{
    n_assert( entity );

    nState::OnEnter( entity );

    // Apply the behavioural action to the entity
    if ( this->behaviouralAction )
    {
        this->behaviouralAction->ApplyAction (entity, true);
    }
}

//-----------------------------------------------------------------------------
/**
    Set the behavioural action contained in this state (used for persistence)

    At the contrary of the SetBehaviouralAction method, with this varition
    there must not be a previous behavioural action.
*/
void nLeafState::SetBehaviouralActionByName( const char* actionName )
{
    n_assert( !this->behaviouralAction );
    this->behaviouralAction = nFSMServer::Instance()->FindBehaviouralAction( this, actionName );
    n_assert( this->behaviouralAction );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
