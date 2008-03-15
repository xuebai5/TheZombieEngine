#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolgeom_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
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
