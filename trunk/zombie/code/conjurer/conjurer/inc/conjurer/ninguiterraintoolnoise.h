#ifndef N_INGUI_TERRAIN_TOOL_NOISE_H
#define N_INGUI_TERRAIN_TOOL_NOISE_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolnoise.h
    @class nInguiTerrainToolNoise
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for terrain surface tools - Noise

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolgeom.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nfloatmapbuilder.h"

//------------------------------------------------------------------------------
class nInguiTerrainToolNoise: public nInguiTerrainToolGeom
{
    public:

    // constructor
    nInguiTerrainToolNoise();
    // destructor
    virtual ~nInguiTerrainToolNoise();

    /// Apply the tool
    virtual bool Apply( nTime dt );

protected:
private:

    void NoiseHeightMap(nFloatMap* hmap);
};

//------------------------------------------------------------------------------

#endif
