//-----------------------------------------------------------------------------
//  neventcondition_main.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/neventcondition.h"
#include "nfsm/nscriptcondition.h"
#include "nfsmserver/nfsmserver.h"
#include "ntrigger/ngameevent.h"

#ifndef __NEBULA_NO_LOG__
#include "ncfsm/ncfsm.h"
#endif

nNebulaScriptClass(nEventCondition, "nroot");

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nEventCondition::nEventCondition()
    : nCondition(nCondition::Event), eventType(-1), filterCondition(NULL)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    Does NOT release the associated filter condition
*/
nEventCondition::~nEventCondition()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Set the event that triggers this condition
*/
void nEventCondition::SetEvent( int eventType )
{
    this->eventType = eventType;
}

//-----------------------------------------------------------------------------
/**
    Set the event that triggers this condition by unmutable id
*/
void nEventCondition::SetEventByPersistentId( const char* idName )
{
    this->eventType = nGameEvent::GetEventTransientId( idName );
}

//-----------------------------------------------------------------------------
/**
    Set the optional filter condition

    Replaces the old one, but does NOT release it
*/
void nEventCondition::SetFilterCondition( nCondition* filterCondition )
{
    this->filterCondition = filterCondition;
}

//-----------------------------------------------------------------------------
/**
    Get the optional filter condition

    Return NULL if there isn't any filter condition
*/
nCondition* nEventCondition::GetFilterCondition() const
{
    return this->filterCondition;
}

//-----------------------------------------------------------------------------
/**
    Tell if the filter condition evaluates to true

    Always return true if there's no filter condition
*/
bool nEventCondition::Evaluate( nEntityObject* entity ) const
{
    if ( this->filterCondition )
    {
        NLOG( fsm, (nFSMServer::TransitionsLog | 3, "%s evaluating filter %s",
            entity->GetComponentSafe<ncFSM>()->GetLogPrefix(),
            this->filterCondition->GetName()) );

        nArg inArg;
        inArg.SetO( entity );
        nArg outArg;
        this->filterCondition->CallArgs( "evaluate", 1, &outArg, 1, &inArg );
        bool result = outArg.GetB();

        NLOG( fsm, (nFSMServer::TransitionsLog | 1, "%s evaluates filter %s to %d",
            entity->GetComponentSafe<ncFSM>()->GetLogPrefix(),
            this->filterCondition->GetName(), result) );

        return result;
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
/**
    Set an event condition as the filter condition, loading it if needed (used for peristence)
*/
void nEventCondition::SetEventFilterCondition( const char* conditionName )
{
    this->filterCondition = nFSMServer::Instance()->GetEventCondition( conditionName );
}

//-----------------------------------------------------------------------------
/**
    Set a script condition as the filter condition, loading it if needed (used for peristence)
*/
void nEventCondition::SetScriptFilterCondition( const char* conditionName )
{
    this->filterCondition = nFSMServer::Instance()->GetScriptCondition( conditionName );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
