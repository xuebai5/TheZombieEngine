#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nnavmeshbuilderstate_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nnavmeshbuilderstate.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nnavmeshbuilderstate

    @cppclass
    nnavmeshbuilderstate

    @superclass
    nroot

    @classinfo
    Editor state for building and saving navigation meshes.
*/
NSCRIPT_INITCMDS_BEGIN(nNavMeshBuilderState)
    NSCRIPT_ADDCMD('BNAV', bool, BuildNavMesh, 0, (), 0, ());
    NSCRIPT_ADDCMD('BINV', bool, BuildIndoorNavMesh, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('BBNV', bool, BuildBrushNavMesh, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('LNAV', bool, LoadNavMesh, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('SNAV', bool, SaveNavMesh, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('SONM', bool, SaveOutdoorNavMesh, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('ECEL', void, ClearExternalLinks, 0, (), 0, ());
    NSCRIPT_ADDCMD('EGEL', void, GenerateExternalLinks, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESOS', void, SetMinObstacleSize, 1, (float), 0, ());
    NSCRIPT_ADDCMD('EGOS', float, GetMinObstacleSize, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESMO', void, SetMinObstacleHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD('EGMO', float, GetMinObstacleHeight, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESCH', void, SetCharacterHeight, 1, (float), 0, ());
    NSCRIPT_ADDCMD('EGCH', float, GetCharacterHeight, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESHS', void, SetMinHardSlope, 1, (float), 0, ());
    NSCRIPT_ADDCMD('EGHS', float, GetMinHardSlope, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESWS', void, SetMaxWalkableSlope, 1, (float), 0, ());
    NSCRIPT_ADDCMD('EGWS', float, GetMaxWalkableSlope, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESSL', void, SetSeaLevel, 1, (float), 0, ());
    NSCRIPT_ADDCMD('EGSL', float, GetSeaLevel, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESTH', void, SetTerrainHolesEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('EGTH', bool, GetTerrainHolesEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESTM', void, SetTriangleMeshesAsObstacles, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('EGTM', bool, GetTriangleMeshesAsObstacles, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESFO', void, SetFitToObstacles, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('EGFO', bool, GetFitToObstacles, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESMP', void, SetMergePolygons, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('EGMP', bool, GetMergePolygons, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESGP', void, SetGeneratePortals, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('EGGP', bool, GetGeneratePortals, 0, (), 0, ());
NSCRIPT_INITCMDS_END()
