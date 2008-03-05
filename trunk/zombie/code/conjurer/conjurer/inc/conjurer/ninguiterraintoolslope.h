#ifndef N_INGUI_TERRAIN_TOOL_SLOPE_H
#define N_INGUI_TERRAIN_TOOL_SLOPE_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolslope.h
    @class nInguiTerrainToolSlope
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for terrain surface tools - Slope

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolgeom.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nfloatmapbuilder.h"

//------------------------------------------------------------------------------
class nInguiTerrainToolSlope: public nInguiTerrainToolGeom
{
    public:

    // constructor
    nInguiTerrainToolSlope();
    // destructor
    virtual ~nInguiTerrainToolSlope();

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Set slope
    //void SetSlope( float s );
    void SetSlope(float);

    /// Get intensity
    //float GetSlope( void );
    float GetSlope();

protected:

    /// Target slope in degrees
    float targetSlope;

private:

    // Fill a heightmap with a plane
    void FillWithPlane(nFloatMap* hmap, vector3 normal, float h0);
};

//------------------------------------------------------------------------------
/**
    @brief Set slope
    @param Slope angle in degrees
*/
inline void
nInguiTerrainToolSlope::SetSlope( float s )
{
    targetSlope = s;
}

//------------------------------------------------------------------------------
/**
    @brief Get slope
    @return Intensity
*/
inline float
nInguiTerrainToolSlope::GetSlope( void )
{
    return targetSlope;
}
//------------------------------------------------------------------------------

#endif
