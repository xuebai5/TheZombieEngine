#include "precompiled/pchngrass.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "ngrassgrowth/ngrowthmaterial.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTerrainVegetationClass)

    NSCRIPT_ADDCMD_COMPCLASS('CSVS', void, SetVegetationCellSubDivision , 1 , (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGVS', int, GetVegetationCellSubDivision  , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSGS', void, SetGrowthMeshLibrarySize, 1 , (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGGS', int, GetGrowthMeshLibrarySize , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSGM', void, SetGrowthMeshName, 2 , (int, const char*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGGM', const char*, GetGrowthMeshName, 1 , (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSBC', void, SetNumSubBlockClip, 1 , (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGBC', int, GetNumSubBlockClip , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSFF', void, SetGrasBeginFadeFactor, 1 , (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGFF', float, GetGrasBeginFadeFactor , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSGD', void, SetGeometryFactorClipDistance, 1 , (const vector2&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGGD', void, GetGeometryFactorClipDistance , 0, (), 1 , (vector2&));

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__

    NSCRIPT_ADDCMD_COMPCLASS('CGGL', nRoot*, GetGrowthMaterialList, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGGI', nGrowthMaterial*, GetGrowthMaterialById, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGID', bool, HasGrowthMaterialWithName, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CCGM', nGrowthMaterial*, CreateGrowthMaterial, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CCGX', nGrowthMaterial*, AddCopyOfGrowthMaterial, 2, (nGrowthMaterial*, const char*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CDGM', void, DeleteGrowthMaterial, 1, (int), 0, ()) ;
    NSCRIPT_ADDCMD_COMPCLASS('CNGM', int, GetNumberOfGrowthMaterials, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSTV', bool, SaveTerrainVegetationResources, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSMS', void, SetGrowthMapSizeByCell, 1 , (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGMS', int, GetGrowthMapSizeByCell , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CBGE', bool, BeginGrowthEditon , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CIGE', bool, IsInGrowthEdition , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CBG_', bool, BuildGrowth , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CBGD', bool, BuildOnlyDirtyGrowth , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CBGS', bool, BuildGrassOnlySelected , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGUM', bool, GrassUpdateMeshes , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGUH', bool, GrassUpdateHeight , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CEGE', bool, EndGrowtEdition, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CSGA', void, SetGrowthEditionAlpha, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('CGGA', float, GetGrowthEditionAplha , 0, (), 0, ());
#endif
#endif
    
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncTerrainVegetationClass::SaveCmds(nPersistServer * ps)
{
    if (nComponentClass::SaveCmds(ps))
    {
        ps->Put(this->GetEntityClass(), 'CSVS', this->GetVegetationCellSubDivision() );
        ps->Put(this->GetEntityClass(), 'CSBC', this->GetNumSubBlockClip() );
        ps->Put(this->GetEntityClass(), 'CSFF', this->GetGrasBeginFadeFactor() );
        ps->Put(this->GetEntityClass(), 'CSGS', this->meshNameLibrary.Size() );
        ps->Put(this->GetEntityClass(), 'CSGD', this->geometryFactorClipDistance.x , geometryFactorClipDistance.y);

        for ( int idx = 0; idx < this->meshNameLibrary.Size()  ; ++idx)
        {
            ps->Put(this->GetEntityClass(), 'CSGM', idx , this->GetGrowthMeshName(idx) );
        }

        return true;
    }

    return false;
}

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__

bool 
ncTerrainVegetationClass::SaveConjurerCmds(nPersistServer * ps)
{
    ps->Put(this->GetEntityClass(), 'CSMS', this->GetGrowthMapSizeByCell() );
    return true;
}

#endif //#ifndef NGAME
#endif //#ifndef __ZOMBIE_EXPORTER__
