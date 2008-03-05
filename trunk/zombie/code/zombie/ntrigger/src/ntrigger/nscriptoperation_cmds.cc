//-----------------------------------------------------------------------------
//  nscriptoperation_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/nscriptoperation.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nScriptOperation )
    NSCRIPT_ADDCMD('EGTL', const char*, GetTypeLabel , 0, (), 0, ());
NSCRIPT_INITCMDS_END()
