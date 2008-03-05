//-----------------------------------------------------------------------------
//  nscriptcondition_cmds.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nscriptcondition.h"
#include "entity/nentityobject.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nScriptCondition )
    NSCRIPT_ADDCMD('EVAL', bool, Evaluate, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool nScriptCondition::SaveCmds( nPersistServer* /*ps*/ )
{
    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
