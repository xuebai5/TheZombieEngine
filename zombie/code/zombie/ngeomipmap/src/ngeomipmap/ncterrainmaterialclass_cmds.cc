//------------------------------------------------------------------------------
//  ncterrainmaterialclass_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "kernel/npersistserver.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTerrainMaterialClass)
    NSCRIPT_ADDCMD_COMPCLASS('BGLC', int, GetLayerCount, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGLH', nTerrainMaterial::LayerHandle, GetLayerHandle, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGLP', int, GetLayerPos, 1, (nTerrainMaterial::LayerHandle), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BADL', nTerrainMaterial *, AddLayer, 1, (nTerrainMaterial::LayerHandle), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BCRL', nTerrainMaterial::LayerHandle, CreateLayer, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BRLA', bool, RemoveLayer, 1, (nTerrainMaterial::LayerHandle), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSEL', bool, SelectLayer, 1, (nTerrainMaterial::LayerHandle), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGSL', nTerrainMaterial::LayerHandle, GetSelectedLayerHandle , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BRST', void, Reset , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGWP', nString, GetWeightMapTexturePath, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGLP', nString, GetLightMapTexturePath, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGLM', nEntityObject*, GetLightMap, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSLM', void, SetLightMap, 1 , (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGLB', nString, GetLayerByteMapPath, 3, (int, int, nTerrainMaterial::LayerHandle), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGGP', nString, GetGlobalTexturePath, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGMP', nString, GetModulationTexturePath, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSAS', void, SetAllWeightMapsSize, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGAS', int, GetAllWeightMapsSize, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSWS', void, SetWeightMapSize, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGWS', int, GetWeightMapSize , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSTS', void, SetGlobalTextureSize, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGTS', int, GetGlobalTextureSize , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSMF', void, SetModulationFactor, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGMF', float, GetModulationFactor , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSMD', void, SetMaterialLODDistance, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGMD', int, GetMaterialLODDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSBP', void, SetNumBlockPreload, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGBP', int, GetNumBlockPreload, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSMN', void, SetMagicNumber, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JSNB', void, SetMaterialNumBlocks, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JGNB', int, GetMaterialNumBlocks, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JSSS', void, SetSingleWeightMapSize, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JGSS', int, GetSingleWeightMapSize, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JGNL', int, GetWeightMapNumberOfLayers, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JGLH', int, GetWeightMapLayerHandle, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JALH', void, AddWeightMapLayerHandle, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JSLI', void, SetWeightMapLayerIndex, 4, (int, int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JGLI', int, GetWeightMapLayerIndex, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BSCW', void, SetCurrentWeightMap, 4, (int, int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSIF', void, SetCellInfoFlags, 2, (int, int), 0, ());
//#ifndef NGAME
    NSCRIPT_ADDCMD_COMPCLASS('CGMV', void, SetCellInfoGrowthMapValue, 1, (int), 0, ());
//#endif
    NSCRIPT_ADDCMD_COMPCLASS('BAWH', void, AddCurrentWeightMapLayerHandle, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BCWI', void, SetCurrentWeightMapIndices, 5, (int, int, int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BCWH', void, SetCurrentWeightMapHandles, 5, (int, int, int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JGWU', int, GetSingleWeightMapFlags, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JSWU', void, SetSingleWeightMapFlags, 3, (int, int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GLBH', nTerrainMaterial *, GetLayerByHandle, 1, (nTerrainMaterial::LayerHandle), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BIMA', bool, ImportMaterials, 1, (nEntityClass *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGTM', nTerrainMaterial *, GetTerrainMaterialAt, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGGN', const char *, GetGameMaterialNameAt, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BGGM', nGameMaterial *, GetGameMaterialAt, 2, (float, float), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPCLASS('CSDL', void, SetDefaultLighmapIsBlack, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGDL', bool, GetDefaultLighmapIsBlack, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BRWT', void, RefreshWeightTexture , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JRGT', void, RefreshGlobalTexture , 0, (), 0, ());
#endif
    NSCRIPT_ADDCMD_COMPCLASS('JSMD', void, SetMaterialResolutionDivisor, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JGMD', int, GetMaterialResolutionDivisor, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('BUPM', void, UpdateMaterialAll, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncTerrainMaterialClass::SaveCmds(nPersistServer * ps)
{
    if (nComponentClass::SaveCmds(ps))
    {
        ps->Put(this->GetEntityClass(), 'BSAS', this->GetAllWeightMapsSize());
        ps->Put(this->GetEntityClass(), 'BSTS', this->GetGlobalTextureSize());
        ps->Put(this->GetEntityClass(), 'BSMD', this->GetMaterialLODDistance());
        ps->Put(this->GetEntityClass(), 'BSBP', this->GetNumBlockPreload());
        ps->Put(this->GetEntityClass(), 'JSMD', this->GetMaterialResolutionDivisor());
        ps->Put(this->GetEntityClass(), 'BSMF', this->GetModulationFactor());

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
