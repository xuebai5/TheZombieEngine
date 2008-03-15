#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolrotation_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolrotation.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolRotation )
    NSCRIPT_ADDCMD('GCRA', float, GetCurrentRotAngle, 0, (), 0, ());
    NSCRIPT_ADDCMD('SROF', void, SetRotationFactor, 1, (float), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
