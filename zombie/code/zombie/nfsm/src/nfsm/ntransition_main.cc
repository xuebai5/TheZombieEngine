//-----------------------------------------------------------------------------
//  ntransition_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/ntransition.h"
#include "nfsm/nstate.h"
#include "nfsm/neventcondition.h"
#include "nfsm/nscriptcondition.h"
#include "nfsmserver/nfsmserver.h"
#include "util/nrandomlogic.h"

nNebulaScriptClass(nTransition, "nroot");

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nTransition::nTransition()
    : condition(NULL)
{
    this->SetSaveModeFlags( N_FLAG_SAVEUPSIDEDOWN );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    Does NOT release the condition
*/
nTransition::~nTransition()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Set the condition that triggers this transition

    Replaces the previous one, but does NOT release it
*/
void nTransition::SetCondition( nCondition* condition )
{
    this->condition = condition;
}

//-----------------------------------------------------------------------------
/**
    Return the index for the given target state within the targets array, or -1 if the target isn't found
*/
int nTransition::FindTargetIndex( const nState* state ) const
{
    for ( int i(0); i < this->targets.Size(); ++i )
    {
        if ( this->targets[i].state == state )
        {
            return i;
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
/**
    Add a target state to this transition

    @param state Target state to go when the transition's condition triggers
    @param int Probability to choose the target state when the transition's condition triggers (probability is supposed to be within [0,100] range)
*/
void nTransition::AddTarget( nState* state, int probability )
{
    n_assert( state );
    n_assert( this->FindTargetIndex(state) == -1 );
    this->targets.PushBack( Target(state,probability) );
}

//-----------------------------------------------------------------------------
/**
    Remove a target state from this transition

    @param state Target state to be removed
*/
void nTransition::RemoveTarget( nState* state )
{
    n_assert( state );
    int index( this->FindTargetIndex(state) );
    if ( index != -1 )
    {
        this->targets.Erase(index);
    }
}

//-----------------------------------------------------------------------------
/**
    Change the probability to choose the given target state
*/
void nTransition::SetTargetProbability( const nState* state, int probability )
{
    n_assert( state );
    int index( this->FindTargetIndex(state) );
    n_assert( index != -1 );
    this->targets[index].probability = probability;
}

//-----------------------------------------------------------------------------
/**
    Get one of the targets at random, but using their probability to be choosen
    
    Return NULL if no target is chosen
*/
nState* nTransition::SelectTarget() const
{
    // Roll a 100 faces dice
    int roll( n_rand_int(99) );

    // Find which target the roll has fallen in
    for ( int i(0); i < this->targets.Size(); ++i )
    {
        if ( roll < this->targets[i].probability )
        {
            return this->targets[i].state;
        }
        else
        {
            roll -= this->targets[i].probability;
        }
    }

    // No target selected (due to the fact that their probabilities don't sum 100%
    // and this time the dice has skipped them all).
    return NULL;
}

//-----------------------------------------------------------------------------
/**
    Change a target state by its index
*/
void nTransition::SetTargetStateByIndex( int index, nState* state )
{
    n_assert( index >= 0 && index < this->targets.Size() );
    n_assert( state );
    n_assert( this->FindTargetIndex(state) == -1 );
    this->targets[index].state = state;
}

//-----------------------------------------------------------------------------
/**
    Change a target state probability by its index
*/
void nTransition::SetTargetProbabilityByIndex( int index, int probability )
{
    n_assert( index >= 0 && index < this->targets.Size() );
    this->targets[index].probability = probability;
}

//-----------------------------------------------------------------------------
/**
    Remove a target state by its index
*/
void nTransition::RemoveTargetByIndex( int index )
{
    n_assert( index >= 0 && index < this->targets.Size() );
    this->targets.Erase(index);
}

//-----------------------------------------------------------------------------
/**
    Set an event condition to trigger this transition, loading it if needed (used for peristence)
*/
void nTransition::SetEventCondition( const char* conditionName )
{
    this->condition = nFSMServer::Instance()->GetEventCondition( conditionName );
}

//-----------------------------------------------------------------------------
/**
    Set a script condition to trigger this transition, loading it if needed (used for peristence)
*/
void nTransition::SetScriptCondition( const char* conditionName )
{
    // Needed to capitalize names to keep backwards compatibility with versions
    // prior to the use of the nScriptClassServer
    nString capitalizedName( conditionName );
    capitalizedName.ToCapital();

    this->condition = nFSMServer::Instance()->GetScriptCondition( capitalizedName.Get() );
}

//-----------------------------------------------------------------------------
/**
    Set an event condition with a filter to trigger this transition (used for peristence)
*/
void nTransition::SetFilterCondition( const char* conditionName )
{
    this->condition = nFSMServer::Instance()->FindLocalCondition( this, conditionName );
}

//-----------------------------------------------------------------------------
/**
    Add a target state to this transition, creating the state if needed (used for persistence)
*/
void nTransition::AddTargetByName( const char* fsmName, const char* stateName, int stateType, int probability )
{
    nState* state = nFSMServer::Instance()->GetState( fsmName, stateName, stateType );
    this->AddTarget( state, probability );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
