#ifndef N_GLOBALTEXTUREBUILDER_H
#define N_GLOBALTEXTUREBUILDER_H
//------------------------------------------------------------------------------
/**
    @file nglobaltexturebuilder.h
    @class nGlobalTextureBuilder
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Terrain global texture builder

    (C) 2004 Conjurer Services, S.A. 

*/
//------------------------------------------------------------------------------
#include "resource/nresource.h"
#include "kernel/ncmdprotonativecpp.h"
#include "gfx2/ntexture2.h"

class nTerrainCellInfo;
class ncTerrainMaterial;
class ncTerrainMaterialClass;

//------------------------------------------------------------------------------
class nTerrainGlobalTextureBuilder
{

public:

    /// constructor
    nTerrainGlobalTextureBuilder();

    /// destructor
    ~nTerrainGlobalTextureBuilder();

    /// Set ncTerrainMaterial to work with
    void SetTerrainMaterialComponent( ncTerrainMaterialClass* tmc );
    /// Set global texture

    /// Generate global texture from current weightmap and indexmap, given a set of materials and tile rectangle to refresh
    void GenerateGlobalTexture( int x0, int z0, int x1, int z1 );

private:

    ncTerrainMaterialClass* terrainMaterialComponent;
    nRef<nTexture2> weightmapTexture;

    // Render target texture for generation of global texture
    nRef<nTexture2> refRenderTexture;

};

//------------------------------------------------------------------------------
#endif