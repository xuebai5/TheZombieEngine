#ifndef N_INGUI_TERRAIN_TOOL_FLATTEN_H
#define N_INGUI_TERRAIN_TOOL_FLATTEN_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolflatten.h
    @class nInguiTerrainToolFlatten
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for terrain surface tools - Flatten terrain

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintoolgeom.h"
#include "signals/nsignalnative.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nfloatmapbuilder.h"

//------------------------------------------------------------------------------
class nInguiTerrainToolFlatten: public nInguiTerrainToolGeom
{
    public:

    // constructor
    nInguiTerrainToolFlatten();
    // destructor
    virtual ~nInguiTerrainToolFlatten();

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Set height
    //void SetHeight( float h );
    void SetHeight(float);

    /// Get height
    //float GetHeight( void );
    float GetHeight();

    /// Set adaptive intensity
    //void SetAdaptiveIntensity( float i );
    void SetAdaptiveIntensity(float);

    /// Get adaptive intensity
    //float GetAdaptiveIntensity( void );
    float GetAdaptiveIntensity();

    // Signals
    NSIGNAL_DECLARE('RFHE', void, RefreshFlattenHeight, 0, (), 0, ());

    /// Makes a lerp between the target height and the parameter height, using adaptive factor as weight
    void AdaptHeight( float h0 );

protected:

    // Target height for the flatten tool
    float targetHeight;

    // Adaptive intensity
    float adaptiveIntensity;

private:
};
//------------------------------------------------------------------------------
/**
    @brief Set target height 
    @param h Target height in LC
*/
inline void
nInguiTerrainToolFlatten::SetHeight( float h )
{
    targetHeight = h;
}

//------------------------------------------------------------------------------
/**
    @brief Get target height
    @return Target height in LC
*/
inline float
nInguiTerrainToolFlatten::GetHeight( void )
{
    return targetHeight;
}
//------------------------------------------------------------------------------
/**
    @brief Set intensity
    @param Intensity
*/
inline void
nInguiTerrainToolFlatten::SetAdaptiveIntensity( float i )
{
    adaptiveIntensity = i;
}

//------------------------------------------------------------------------------
/**
    @brief Get intensity
    @return Intensity
*/
inline float
nInguiTerrainToolFlatten::GetAdaptiveIntensity( void )
{
    return adaptiveIntensity;
}
//------------------------------------------------------------------------------
inline void
nInguiTerrainToolFlatten::AdaptHeight( float h0 )
{
    targetHeight = targetHeight * adaptiveIntensity + h0 * ( 1.0f - adaptiveIntensity );
}

#endif
