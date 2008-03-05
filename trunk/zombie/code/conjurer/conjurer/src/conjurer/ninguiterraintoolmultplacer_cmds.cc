#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file ninguiterraintoolhole_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna

    @brief nInguiTerrainToolHole persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolmultplacer.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiTerrainToolMultPlacer )
    NSCRIPT_ADDCMD('RSTL', void, ResetClassList , 0, (), 0, ());
    NSCRIPT_ADDCMD('ADDC', void, AddClass, 1, (nString), 0, ());
    NSCRIPT_ADDCMD('SETI', void, SetIntensity, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GETI', float, GetIntensity , 0, (), 0, ());
    NSCRIPT_ADDCMD('SRRO', void, SetRandomRotation, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GRRO', bool, GetRandomRotation, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSSV', void, SetSizeVariation, 1, (float), 0, ());
    NSCRIPT_ADDCMD('JGSV', float, GetSizeVariation, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
