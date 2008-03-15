#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolscale_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolscale.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolScale )
    NSCRIPT_ADDCMD('JGCS', float, GetCurrentScaling, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSSF', void, SetScaleFactor, 1, (float), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
