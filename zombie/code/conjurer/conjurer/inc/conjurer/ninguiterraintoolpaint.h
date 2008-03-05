#ifndef N_INGUI_TERRAIN_TOOL_PAINT_H
#define N_INGUI_TERRAIN_TOOL_PAINT_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolpaint.h
    @class nInguiTerrainToolPaint
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for terrain surface tools - Paint terrain

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolgeom.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nbytemapbuilder.h"
#include "ngeomipmap/nfloatmapbuilder.h"
#include "ngeomipmap/nterrainweightmapbuilder.h"

//------------------------------------------------------------------------------
class nInguiTerrainToolPaint: public nInguiTerrainToolGeom
{
    public:

    // constructor
    nInguiTerrainToolPaint();
    // destructor
    virtual ~nInguiTerrainToolPaint();

    bool HandleInput( nAppViewport* vp );

    /// Set outdoor to edit
    virtual void SetOutdoor(nEntityObject * entityObject);

    /// Calculate the height and slope filtering
    void CalculateFilters(float x, float z);

    /// Apply the tool
    virtual bool Apply( nTime dt );

    bool GetFilterHeightEnabled ();
    bool GetFilterSlopeEnabled ();
    float GetMinFilterHeight ();
    float GetMaxFilterHeight ();
    float GetMinFilterSlope ();
    float GetMaxFilterSlope ();
    void SetFilterHeightEnabled(bool);
    void SetFilterSlopeEnabled(bool);
    void SetMinFilterHeight(float);
    void SetMaxFilterHeight(float);
    void SetMinFilterSlope(float);
    void SetMaxFilterSlope(float);
   virtual void SetBlendIntensity(float);
   virtual float GetBlendIntensity();

    /// Get draw resolution multiplier
    virtual float GetDrawResolutionMultiplier();

protected:
private:

    // Blend factor, 0 = no blend, 1 = total blend
    float blendIntensity;

    // Flag for paint/erase action
    bool paintNotErase;

    // Filter flags
    bool heightFilter, slopeFilter;

    // Height filter values
    float filterMinHeight, filterMaxHeight;

    // Slope filter values
    float filterMinSlope, filterMaxSlope;

    // Reference to the layer manager
    ncTerrainMaterialClass* layerManager;

    // Bytemap builder
    nByteMapBuilder bmBuilder;

    // Weightmap builder
    nTerrainWeightMapBuilder wmBuilder;

    // Floatmap builder
    nFloatMapBuilder fmBuilder;

};

//------------------------------------------------------------------------------
/**
    @brief Set blend intensity
    @param Intensity
*/
inline
void
nInguiTerrainToolPaint::SetBlendIntensity( float bi )
{
    blendIntensity = bi;
}

//------------------------------------------------------------------------------
/**
    @brief Get blend intensity
    @return Intensity
*/
inline
float
nInguiTerrainToolPaint::GetBlendIntensity( void )
{
    return blendIntensity;
}
//------------------------------------------------------------------------------
#endif
