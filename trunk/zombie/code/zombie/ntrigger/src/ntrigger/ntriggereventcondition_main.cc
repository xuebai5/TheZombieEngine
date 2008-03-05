//-----------------------------------------------------------------------------
//  ntriggereventcondition_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ntriggereventcondition.h"
#include "ntrigger/ntriggerstatecondition.h"
#include "ntrigger/ntriggerserver.h"

//-----------------------------------------------------------------------------
/**
    Get operation type
*/
nTriggerEventCondition::nTriggerEventCondition()
    : triggered( false ), filterCondition( NULL )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Tell if the filter condition evaluates to true

    Always return true if there's no filter condition or the condition has
    already being triggered (the filter is skipped in this last case).
    Set the 'condition triggered' flag.
*/
bool
nTriggerEventCondition::Evaluate( nEntityObject* trigger, nEntityObject* emitter )
{
    if ( !this->triggered )
    {
        if ( this->filterCondition )
        {
            this->triggered = this->filterCondition->Evaluate( trigger, emitter );
        }
        else
        {
            this->triggered = true;
        }
    }
    return this->triggered;
}

//-----------------------------------------------------------------------------
/**
    Get the filter condition
*/
nTriggerStateCondition*
nTriggerEventCondition::GetFilterCondition() const
{
    return this->filterCondition;
}

//-----------------------------------------------------------------------------
/**
    Get the filter condition name, or and empty string if there isn't any
*/
const char*
nTriggerEventCondition::GetFilterConditionName() const
{
    if ( !this->filterCondition )
    {
        static const char* EmptyString = "";
        return EmptyString;
    }
    return this->filterCondition->GetName();
}

//-----------------------------------------------------------------------------
/**
    Set the filter condition
*/
void
nTriggerEventCondition::SetFilterCondition( nTriggerStateCondition* filter )
{
    this->filterCondition = filter;
}

//-----------------------------------------------------------------------------
/**
    Set the filter condition, by its name
*/
void
nTriggerEventCondition::SetFilterConditionByName( const char* filterName )
{
    if ( !filterName )
    {
        this->filterCondition = NULL;
        return;
    }
    if ( nString(filterName) == "" )
    {
        this->filterCondition = NULL;
        return;
    }
    this->filterCondition = nTriggerServer::Instance()->GetTriggerCondition( filterName );
}
