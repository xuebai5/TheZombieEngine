#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintool_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintool.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nInguiTerrainTool )
    NSCRIPT_ADDCMD('SDIA', void, SetDiameter, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GDIA', float, GetDiameter , 0, (), 0, ());
    NSCRIPT_ADDCMD('GLTC', bool, GetLastTerrainCoords, 0, (), 2, (int&, int&));
    NSCRIPT_ADDCMD('SAUP', void, SetAlwaysUsePickingFlag, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GAUP', bool, GetAlwaysUsePickingFlag, 0, (), 0, ());
    NSCRIPT_ADDCMD('SSSQ', void, SetShapeSquared, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GSSQ', bool, GetShapeSquared , 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
