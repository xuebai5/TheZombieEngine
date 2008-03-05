#ifndef N_TERRAINWEIGHTMAPBUILDER_H
#define N_TERRAINWEIGHTMAPBUILDER_H
//------------------------------------------------------------------------------
/**
    @file nterrainweightmapbuilder.h
    @class nTerrainWeightMapBuilder
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief nTerrainCellInfo builder

    (C) 2004 Conjurer Services, S.A. 

*/
#ifndef NGAME
//------------------------------------------------------------------------------
#include "resource/nresource.h"
#include "kernel/ncmdprotonativecpp.h"
#include "gfx2/ntexture2.h"

class nTerrainCellInfo;
class ncTerrainMaterial;
class ncTerrainMaterialClass;

//------------------------------------------------------------------------------
class nTerrainWeightMapBuilder
{

public:

    /// constructor
    nTerrainWeightMapBuilder();
    /// destructor
    ~nTerrainWeightMapBuilder();

    /// Set ncTerrainMaterial to work with
    void SetTerrainMaterialComponent( ncTerrainMaterialClass* tmc );
    /// Set weight texture to update
    void SetWeightTexture(nTexture2 *);
    /// Get weight texture
    nTexture2* GetWeightTexture();

    /// Refresh only one weight map
    void RefreshWeightMap( int bx, int bz );
    /// Refresh one block weight map, only in rectangle specified by texel coordinates
    void RefreshWeightMapsPortion( int bx, int bz, int x0, int z0, int x1, int z1 );
    /// Generate the entire weightmap for a block
    void Generate( int bx, int bz );
    /// Generate weightmap given weightmap texel coordinates, for one block
    void Generate( int bx, int bz, int x0, int z0, int x1, int z1 );

    /// Update block borders to fix boundary interpolation of textures
    void CheckBlockBoundaries( int x0, int z0, int x1, int z1);

private:

    /// clean a weight map texture to invalid
    void CleanWeightMapTexture();
    /// clean the layers of the weightmap to invalid
    void CleanWeightMapLayers( int bx, int bz );

    /// terrain material information
    ncTerrainMaterialClass* terrainMaterialComponent;

    /// Texture to be updated
    nRef<nTexture2> refWeightTexture;
};

//------------------------------------------------------------------------------
#endif
#endif