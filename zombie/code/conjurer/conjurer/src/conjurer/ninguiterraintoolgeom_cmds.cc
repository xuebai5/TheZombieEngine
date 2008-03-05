#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file ninguiterraintoolgeom_cmds.cc
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna

    @brief nInguiTerrainToolGeom persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolgeom.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiTerrainToolGeom )
    NSCRIPT_ADDCMD('SINT', void, SetIntensity, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GINT', float, GetIntensity, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
