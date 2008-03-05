//------------------------------------------------------------------------------
//  ncterraingmmclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTerrainGMMClass)
    NSCRIPT_ADDCMD_COMPCLASS('BSBS', void, SetBlockSize, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGBS', int, GetBlockSize, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGSS', float, GetBlockSideSizeScaled, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGT2', float, GetTotalSideSizeScaled, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGNB', int, GetNumBlocks, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGMS', int, GetHeightMapSize, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGPS', float, GetPointSeparation, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGMI', float, GetMinHeight, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGMA', float, GetMaxHeight, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGNL', int, GetNumLODLevels, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGDE', int, GetDepth, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGHM', nFloatMap * , GetHeightMap, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGHP', nString, GetHeightMapPath, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSER', void, SetError, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGER', float, GetError, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSTG', bool, SaveTerrainGMM, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSLS', void, SetLODSelectorMethod, 1, (ncTerrainGMMClass::LODSelectionMethod), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGLS', ncTerrainGMMClass::LODSelectionMethod, GetLODSelectorMethod, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSGL', void, SetGeometryLODDistance, 6, (int, int, int, int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGGL', void, GetGeometryLODDistance, 0, (), 6, (int &, int &, int &, int &, int &, int &));
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainGMMClass::SaveCmds(nPersistServer* ps)
{
    if (nComponentClass::SaveCmds(ps))
    {
        ps->Put(this->GetEntityClass(), 'BSBS', this->GetBlockSize());
        ps->Put(this->GetEntityClass(), 'BSER', this->GetError());
        ps->Put(this->GetEntityClass(), 'BSLS', this->GetLODSelectorMethod());
        ps->Put(this->GetEntityClass(), 'BSGL', 
            this->geometryLODDistance[0],
            this->geometryLODDistance[1],
            this->geometryLODDistance[2],
            this->geometryLODDistance[3],
            this->geometryLODDistance[4],
            this->geometryLODDistance[5]);
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------