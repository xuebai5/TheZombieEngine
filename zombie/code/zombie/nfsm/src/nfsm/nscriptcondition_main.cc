//-----------------------------------------------------------------------------
//  nscriptcondition_main.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nscriptcondition.h"

nNebulaScriptClass(nScriptCondition, "nroot");

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nScriptCondition::nScriptCondition()
    : nCondition(nCondition::Script)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Tell if the condition rule evaluates to true for the given entity
*/
bool nScriptCondition::Evaluate( nEntityObject* /*entity*/ ) const
{
#ifndef NGAME
    nString error( "carles.ros: Command 'Evaluate' not found in the condition script '" );
    error += this->GetClass()->GetName();
    error += "'";
    n_error( error.Get() );
#endif
    return false;
}

//-----------------------------------------------------------------------------
