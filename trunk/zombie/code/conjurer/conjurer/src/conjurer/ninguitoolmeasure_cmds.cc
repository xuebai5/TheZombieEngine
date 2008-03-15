#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolmeasure_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitoolmeasure.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolMeasure )
    NSCRIPT_ADDCMD('JGMI', float, GetMeasureInfo, 0, (), 1, (float&));
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
