#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolslope_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolslope.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiTerrainToolSlope )
    NSCRIPT_ADDCMD('SSLO', void, SetSlope, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GSLO', float, GetSlope, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
