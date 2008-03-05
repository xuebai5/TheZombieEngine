#ifndef N_INGUI_TERRAIN_TOOL_GEOM_H
#define N_INGUI_TERRAIN_TOOL_GEOM_H
//------------------------------------------------------------------------------
/**
    @file ninguiterraintoolgeom.h
    @class nInguiTerrainToolGeom
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nInguiTool for terrain surface geometry tools

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "conjurer/ninguiterraintool.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nfloatmapbuilder.h"

class nGeoMipMapNode;
class nAppViewport;

//------------------------------------------------------------------------------
class nInguiTerrainToolGeom : public nInguiTerrainTool
{
public:

    // constructor
    nInguiTerrainToolGeom();

    // destructor
    virtual ~nInguiTerrainToolGeom();

    /// Set Intensity
   virtual void SetIntensity(float);

    /// Get intensity
   virtual float GetIntensity();

    /// Sets the paintbrush (a FloatMap)
    void SetPaintbrush( nFloatMap *pbrush );

    /// Sets the temporary paintbrushes (HeightMaps)
    void SetTempPaintbrushes( nFloatMap *temppbrush1, nFloatMap *temppbrush2 );

    /// Sets the second temporary paintbrush (a HeightMap)
    void SetTempPaintbrush2( nFloatMap *temppbrush );

    /// Marks a rectangle geometry as dirty
    void MakeRectangleDirtyGeometry(int x0, int z0, int x1, int z1);
    /// Marks a rectangle material as dirty
    void MakeRectangleDirtyMaterial(int x0, int z0, int x1, int z1);

    /// Find GeoMipMap node
    bool FindGeoMipMapNode(nRoot *);

protected:

    /// Intensity 
    float intensity;

    /// Current paintbrush for the tool
    nRef<nFloatMap> currentPaintbrush;

    /// Temp paintbrushes for the tool
    nRef<nFloatMap> tempPaintbrush1, tempPaintbrush2;

    /// Floatmap builder
    nFloatMapBuilder fmBuilder;
};

//------------------------------------------------------------------------------
/**
    @brief Set intensity
    @param Intensity
*/
inline
void
nInguiTerrainToolGeom::SetIntensity( float i )
{
    intensity = i;
}

//------------------------------------------------------------------------------
/**
    @brief Get intensity
    @return Intensity
*/
inline
float
nInguiTerrainToolGeom::GetIntensity( void )
{
    return intensity;
}
//------------------------------------------------------------------------------
/**
    @brief Sets the temporary paintbrush (a HeightMap)
*/
inline
void 
nInguiTerrainToolGeom::SetTempPaintbrushes( nFloatMap *temppbrush1, nFloatMap *temppbrush2 )
{
    n_assert( temppbrush1 && temppbrush2 );
    this->tempPaintbrush1 = temppbrush1;
    this->tempPaintbrush2 = temppbrush2;
}
//------------------------------------------------------------------------------
/**
    @brief Sets the paintbrush (a nFloatMap)
*/
inline
void 
nInguiTerrainToolGeom::SetPaintbrush( nFloatMap *pbrush )
{
    n_assert( pbrush );
    this->currentPaintbrush = pbrush;
}
//------------------------------------------------------------------------------
#endif
