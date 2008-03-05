//-----------------------------------------------------------------------------
//  ntriggereventcondition_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ntriggerstatecondition.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nTriggerStateCondition )
    NSCRIPT_ADDCMD('EVAL', bool, Evaluate, 2, (nEntityObject*, nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()
