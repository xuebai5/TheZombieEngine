#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file ninguiterraintoolgrass_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna

    @brief nInguiIerrainToolGrass persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolgrass.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiTerrainToolGrass )
    NSCRIPT_ADDCMD('JSGI', void, SetGrassId, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGGI', int, GetGrassId, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
