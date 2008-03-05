//-----------------------------------------------------------------------------
//  ntriggerstatecondition_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ntriggerstatecondition.h"

nNebulaScriptClass(nTriggerStateCondition, "nroot");

//-----------------------------------------------------------------------------
/**
    Tell if the condition rule evaluates to true

    If it isn't overwrited it returns always true to indicate that the condition
    must trigger always.

    @param trigger Trigger to which this condition belongs to
    @param emitter Entity that has emitted the event, or NULL if this is a state condition
*/
bool
nTriggerStateCondition::Evaluate( nEntityObject* /*trigger*/, nEntityObject* /*emitter*/ )
{
    return true;
}
