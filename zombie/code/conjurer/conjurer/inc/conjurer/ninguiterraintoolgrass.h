#ifndef N_INGUI_TERRAIN_TOOL_GRASS_H
#define N_INGUI_TERRAIN_TOOL_GRASS_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolgrass.h
    @class nInguiTerrainToolGrass
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for grass edition

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolgeom.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nbytemapbuilder.h"
#include "ngeomipmap/nfloatmapbuilder.h"
#include "ngeomipmap/nterrainweightmapbuilder.h"

//------------------------------------------------------------------------------
class ncTerrainVegetationClass;

//------------------------------------------------------------------------------
class nInguiTerrainToolGrass: public nInguiTerrainToolGeom
{
    public:

    // constructor
    nInguiTerrainToolGrass();
    // destructor
    virtual ~nInguiTerrainToolGrass();

    /// Set outdoor to edit
    virtual void SetOutdoor(nEntityObject * entityObject);

    /// Called when select and deselect the tool
    virtual void OnSelected();
    virtual void OnDeselected();

    /// Calculate the height and slope filtering
    void CalculateFilters(float x, float z);

    /// Apply the tool
    virtual bool Apply( nTime dt );

    virtual void SetGrassId(int);
    virtual int GetGrassId();

    /// Get draw resolution multiplier
    virtual float GetDrawResolutionMultiplier();

protected:
private:

    // Current grass id
    nuint8 currentGrassId;

    // Reference to the grass manager
    ncTerrainVegetationClass * grassManager;

    // Reference to the terrain layer manager
    ncTerrainMaterialClass * layerManager;

    // Terrain number of blocks
    int terrainNumBlocks;

    // Bytemap builder
    nByteMapBuilder bmBuilder;
};
//------------------------------------------------------------------------------
#endif
