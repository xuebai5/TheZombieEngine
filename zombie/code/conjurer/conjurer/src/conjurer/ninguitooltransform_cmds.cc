#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitooltransform_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguitooltransform.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiToolTransform )
    NSCRIPT_ADDCMD('GSNP', float, GetSnap, 0, (), 0, ());
    NSCRIPT_ADDCMD('SSNP', void, SetSnap, 1, (float), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
