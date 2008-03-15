#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolphypick_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolphypick.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolPhyPick )
    NSCRIPT_ADDCMD('SETG', void, SetGridPath, 1, (nString), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
