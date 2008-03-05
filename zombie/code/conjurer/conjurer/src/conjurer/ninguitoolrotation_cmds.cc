#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file ninguiterraintool_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna

    @brief nInguiTerrainTool persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
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
