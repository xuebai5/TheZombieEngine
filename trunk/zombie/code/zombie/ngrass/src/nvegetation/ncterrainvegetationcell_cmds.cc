#include "precompiled/pchngrass.h"
//------------------------------------------------------------------------------
//  ncterrainvegetationcell_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nvegetation/ncterrainvegetationcell.h"
//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTerrainVegetationCell)
#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    NSCRIPT_ADDCMD_COMPOBJECT('CGGL', nByteMap*, GetGrowthMap, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('CGGP', nByteMap*, GetValidGrowthMap, 0, (), 0, ());
#endif
#endif
NSCRIPT_INITCMDS_END()
