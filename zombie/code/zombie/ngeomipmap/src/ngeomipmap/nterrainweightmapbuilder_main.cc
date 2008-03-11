//------------------------------------------------------------------------------
//  nterrainweightmapbuilder_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#ifndef NGAME
#include "ngeomipmap/nterrainweightmapbuilder.h"
#include "tools/ntexturebuilder.h"
#include "gfx2/ngfxserver2.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ncterrainmaterialclass.h"

//------------------------------------------------------------------------------
/**
    constructor
*/
nTerrainWeightMapBuilder::nTerrainWeightMapBuilder()
{
    /// empty
}
//------------------------------------------------------------------------------
/**
    destructor
*/
nTerrainWeightMapBuilder::~nTerrainWeightMapBuilder()
{
    this->SetWeightTexture(0);
}

//------------------------------------------------------------------------------
/**
    @brief Setup the terrain material to start with the builder
*/
void
nTerrainWeightMapBuilder::SetTerrainMaterialComponent( ncTerrainMaterialClass* tmc )
{
    n_assert_return( tmc, );
    this->terrainMaterialComponent = tmc;
}


//------------------------------------------------------------------------------
/**
    @brief Get weight map texture
*/
nTexture2*
nTerrainWeightMapBuilder::GetWeightTexture()
{
    return this->refWeightTexture.get();
}

//------------------------------------------------------------------------------
/**
    @brief Set weight map texture (must be a A8R8G8B8 texture)
*/
void 
nTerrainWeightMapBuilder::SetWeightTexture(nTexture2 * tex)
{
    if (this->refWeightTexture.isvalid())
    {
        this->refWeightTexture->Release();
        this->refWeightTexture.invalidate();
    }

    if (tex)
    {
        n_assert(tex->GetFormat() == nTexture2::A8R8G8B8);
        this->refWeightTexture = tex;
        this->refWeightTexture->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Refresh only one weight map
*/
void
nTerrainWeightMapBuilder::RefreshWeightMap( int bx, int bz )
{
    n_assert_return( this->terrainMaterialComponent, );

    this->CheckBlockBoundaries( bx, bz, bx, bz );

    // Generate weightmaps
    this->Generate( bx, bz );
}

//------------------------------------------------------------------------------
/**
    @brief Refresh one weight map, only in rectangle specified by texel coordinates
    @param  bx  Terrain cell x index
    @param  bz  Terrain cell z index
    @param  x0  left coord in the block
    @param  z0  top coord in the block
    @param  x1  right coord in the block
    @param  z1  bottom coord in the block
*/
void
nTerrainWeightMapBuilder::RefreshWeightMapsPortion( int bx, int bz, int x0, int z0, int x1, int z1 )
{
    n_assert_return( this->terrainMaterialComponent, );
    n_assert_return( bx >= 0 && bx < this->terrainMaterialComponent->GetMaterialNumBlocks(), );
    n_assert_return( bz >= 0 && bz < this->terrainMaterialComponent->GetMaterialNumBlocks(), );

    // Update boundaries of blocks
    this->CheckBlockBoundaries( bx, bz, bx, bz );

    // Generate weightmap
    this->Generate( bx, bz, x0, z0, x1, z1 );
}


//------------------------------------------------------------------------------
/**
    @brief Generate weightmap texture and layer indexes for a block
    @param  bx  x coordinate of block
    @param  bz  z coordinate of block
*/
void
nTerrainWeightMapBuilder::Generate( int bx, int bz )
{
    n_assert_return( this->terrainMaterialComponent, );
    n_assert_return( bx >= 0 && bx < this->terrainMaterialComponent->GetMaterialNumBlocks(), );
    n_assert_return( bz >= 0 && bz < this->terrainMaterialComponent->GetMaterialNumBlocks(), );

    // Size of the weightmap
    int wmapSize = this->terrainMaterialComponent->GetAllWeightMapsSize();

    // Coordinates of blocks to generate
    int tx0 = wmapSize * bx;
    int tz0 = wmapSize * bz;
    int tx1 = wmapSize * ( bx + 1 ) - 1;
    int tz1 = wmapSize * ( bz + 1 ) - 1;

    this->Generate( bx, bz, tx0, tz0, tx1, tz1 );
}

//------------------------------------------------------------------------------
/**
    @brief Generate weightmap texture and layer indexes for a block

    Parameters are rectangle to refresh in weightmap texel coordinates

    This function selects the five more used layers for the block, and calculates weights with the
    layers of that block. If less than five layers are used, the remaining ones are arbitrarily selected.
*/
void
nTerrainWeightMapBuilder::Generate( int bx, int bz, int x0, int z0, int x1, int z1 )
{
    n_assert_return( this->terrainMaterialComponent, );

    int numBlocks = this->terrainMaterialComponent->GetMaterialNumBlocks();
    n_assert_return( numBlocks > 0, );
    n_assert_return( bx >= 0 && bx < numBlocks, );
    n_assert_return( bz >= 0 && bz < numBlocks, );

    int weightMapSize = this->terrainMaterialComponent->GetAllWeightMapsSize();
    n_assert_return( weightMapSize > 0, );

    nTerrainCellInfo* cellInfo = this->terrainMaterialComponent->GetTerrainCellInfo( bx, bz );
    n_assert_return( cellInfo, );

    int numLayers = cellInfo->GetNumberOfLayers();

#ifndef NGAME
    // Weightmap must have some layer created. If not, fill texture with 'pink'
    // Pink is only for edit mode
    if ( numLayers == 0 || !cellInfo->AreLayersCreated() )
    {
        this->CleanWeightMapTexture();
        this->CleanWeightMapLayers(bx, bz);
        return;
    }
#endif

    // check target weightmap texture
    n_assert_return( this->refWeightTexture.isvalid(), );
    n_assert_return( this->refWeightTexture->GetFormat() == nTexture2::A8R8G8B8, );

    nTexture2::LockInfo cellInfoLockInfo;
    int cellInfoRemainder = 0;
    nuint8* cellInfoLine = 0;
    if ( this->refWeightTexture->Lock(nTexture2::WriteOnly, 0, cellInfoLockInfo) )
    {
        cellInfoLine = (nuint8*) cellInfoLockInfo.surfPointer;
        // Line remainder: be careful, this is in bytes
        cellInfoRemainder = cellInfoLockInfo.surfPitch - ( weightMapSize ) * 4;
    }
    else
    {
        n_assert2_always("Couldn't lock weight texture for writing");
        return;
    }

    // Selected layers in the block
    int selLayers[MaxWeightMapBlendLayers];

    // Previous weightmap indexes
    int prevLayers[MaxWeightMapBlendLayers];

    for (int k = 0; k < MaxWeightMapBlendLayers; k++ )
    {
        selLayers[k] = static_cast<nuint8>( min( numLayers-1, k) );

        if ( numLayers > k )
        {
            selLayers[k] = k;
        }
        else
        {
            selLayers[k] = 0;
        }

        prevLayers[k] = cellInfo->GetLayerIndex( k );
    }

    // Select the five most used layers
    int numAffectingLayers = 0;

    // Mark all weightmap layers as unselected
    for (int k = 0; k < numLayers; k++ )
    {
        cellInfo->GetLayerInfo( k ).selected = false;
    }

    for (int numSelLayers = 0; numSelLayers < MaxWeightMapBlendLayers; numSelLayers++ )
    {
        unsigned int max = 0;
        int selLayer = 0;
        bool didSel = false;
        for (int l=0; l < numLayers; l++)
        {
            nTerrainCellInfo::WeightMapLayerInfo& layerInfo = cellInfo->GetLayerInfo( l );

            if ( layerInfo.selected )
            {
                continue;
            }
            unsigned int use = layerInfo.refLayer->GetAccumulator();
            if ( use > max )
            {
                selLayer = l;
                max = use;
                if ( !didSel )
                {
                    numAffectingLayers++;
                }
                didSel = true;
            }
        }
        if ( !didSel )
        {
            selLayer = 0;
        }
        else
        {
            cellInfo->GetLayerInfo( selLayer ).selected = true;
            selLayers[ numSelLayers ] =  (nuint8)selLayer;
        }
    }

    // Refresh layer indexes
    bool layersChanged = false;
    for (int k = 0; k < MaxWeightMapBlendLayers; k++)
    {
        if ( numAffectingLayers > k )
        {
            cellInfo->SetLayerIndex( k, selLayers[k]);
        }
        else
        {
            cellInfo->SetLayerIndex( k, 255);
        }

        if ( cellInfo->GetLayerIndex( k ) != prevLayers[k] )
        {
            layersChanged = true;
        }
    }

    // Loop in the weightmap area covered by the indexmap point, this time for filling the weightmap
    // with the selected layers

    // Initialize bytemap (layer) coordinates and pointers

    int wi0 = 0;
    int wi1 = 0;
    int wj0 = 0;
    int wj1 = 0;

    if ( !layersChanged )
    {
        // If layers did not change, update just the rectangle
        wi0 = max( x0,       bx * weightMapSize    ) % weightMapSize;
        wi1 = min( x1, (bx + 1) * weightMapSize - 1) % weightMapSize;
        wj0 = max( z0,       bz * weightMapSize    ) % weightMapSize;
        wj1 = min( z1, (bz + 1) * weightMapSize - 1) % weightMapSize;
    }
    else
    {
        // else update entire weightmap
        wi0 = 0;
        wi1 = weightMapSize - 1;
        wj0 = 0;
        wj1 = weightMapSize - 1;
    }

    nByteMap* layer = cellInfo->GetLayerInfo( selLayers[0] ).refLayer;
    n_assert( layer );
    nuint8* bpoint0 = layer->GetByteMap();
    bpoint0 += wj0 * weightMapSize + wi0;
    layer = cellInfo->GetLayerInfo( selLayers[1] ).refLayer;
    n_assert( layer );
    nuint8* bpoint1 = layer->GetByteMap();
    bpoint1 += wj0 * weightMapSize + wi0;
    layer = cellInfo->GetLayerInfo( selLayers[2] ).refLayer;
    n_assert( layer );
    nuint8* bpoint2 = layer->GetByteMap();
    bpoint2 += wj0 * weightMapSize + wi0;
    layer = cellInfo->GetLayerInfo( selLayers[3] ).refLayer;
    n_assert( layer );
    nuint8* bpoint3 = layer->GetByteMap();
    bpoint3 += wj0 * weightMapSize + wi0;
    layer = cellInfo->GetLayerInfo( selLayers[4] ).refLayer;
    n_assert( layer );
    nuint8* bpoint4 = layer->GetByteMap();
    bpoint4 += wj0 * weightMapSize + wi0;

    // Bytemap row increment
    int cellInfoInc = weightMapSize - ( wi1 - wi0 ) - 1;

    // Texture pointer for writing
    nuint8* curLine = cellInfoLine + ( ( weightMapSize + ( cellInfoRemainder / 4 ) )* wj0 + wi0 ) * 4;
    cellInfoRemainder += cellInfoInc * 4;

    // Main loop over weight texture points
    for (int wj = wj0; wj <= wj1; wj++ )
    {
        for (int wi = wi0; wi <= wi1; wi++ )
        {
#if 0
            /**
                @todo this is old code to handle 5 layers, probably will get removed
                this has to be cleanup & refactored
            */
            nuint8* curTexel = curLine;
            nuint8 p0, p1, p2, p3, p4;
            p0 = numAffectingLayers > 0? *bpoint0 : 0;
            p1 = numAffectingLayers > 1? *bpoint1 : 0;
            p2 = numAffectingLayers > 2? *bpoint2 : 0;
            p3 = numAffectingLayers > 3? *bpoint3 : 0;
            p4 = numAffectingLayers > 4? *bpoint4 : 0;

            // Normalizing factor
            float u = float(p0) + float(p1) + float(p2) + float(p3) + float(p4);

            if ( u == 0 || numAffectingLayers == 0 ) {
                // not painted areas are marked with sum > 1
                *curTexel++ = 255;
                *curTexel++ = 255;
                *curTexel++ = 255;
                *curTexel = 255;
            }
            else
            {
                // normalizing factor
                u = 255.0f / u;

                // put the remaining of 255 - sum(layers) to the first layer
                int remaining = 255 - ( int(u * p0) + int(u * p1) + int(u * p2) + int(u * p3) + int(u * p4) );

                // Put layer values in the weightmap
                nuint8 w = static_cast<nuint8> ( u * p2 );
                *curTexel++ = w;

                w = static_cast<nuint8> ( u * p1 );
                *curTexel++ = w;

                w = static_cast<nuint8> ( u * p0 );
                if ( remaining > 0 )
                {
                    w = w + nuint8(remaining);
                }
                *curTexel++ = w;

                w = static_cast<nuint8> ( u * p3 );
                *curTexel = w;

            }
#else
            nuint8* curTexel = curLine;
            nuint8 p0, p1, p2, p3;
            p0 = numAffectingLayers > 0? *bpoint0 : 0;
            p1 = numAffectingLayers > 1? *bpoint1 : 0;
            p2 = numAffectingLayers > 2? *bpoint2 : 0;
            p3 = numAffectingLayers > 3? *bpoint3 : 0;

            // Normalizing factor
            float u = float(p0) + float(p1) + float(p2) + float(p3);

            if ( u == 0 || numAffectingLayers == 0 ) {
                // not painted areas are marked with sum > 1
                *curTexel++ = 255;
                *curTexel++ = 255;
                *curTexel++ = 255;
                *curTexel = 255;
            }
            else
            {
                // normalizing factor
                u = 255.0f / u;

                // put the remaining of 255 - sum(layers) to the first layer
                nuint8 remaining = static_cast<nuint8> (255 - ( int(u * p0) + int(u * p1) + int(u * p2) + int(u * p3) ));

                // Put layer values in the weightmap
                *curTexel++ = static_cast<nuint8> ( u * p2 );
                *curTexel++ = static_cast<nuint8> ( u * p1 );
                *curTexel++ = static_cast<nuint8> ( u * p0 ) + remaining;
                *curTexel = static_cast<nuint8> ( u * p3 );
            }
#endif
            bpoint0++;
            bpoint1++;
            bpoint2++;
            bpoint3++;
            bpoint4++;
            curLine += 4;
        }
        bpoint0 += cellInfoInc;
        bpoint1 += cellInfoInc;
        bpoint2 += cellInfoInc;
        bpoint3 += cellInfoInc;
        bpoint4 += cellInfoInc;
        curLine += cellInfoRemainder;
    }

    this->refWeightTexture->Unlock(0);

}

//------------------------------------------------------------------------------
/**
    @brief Update block borders to fix boundary interpolation of textures
    @param x0 NW corner of rectangle to update
    @param z0 NW corner of rectangle to update
    @param x1 SE corner of rectangle to update
    @param z1 SE corner of rectangle to update

*/
void
nTerrainWeightMapBuilder::CheckBlockBoundaries( int x0, int z0, int x1, int z1)
{

    n_assert( this->terrainMaterialComponent );

    // Number of blocks in the terrain
    int numBlocks = this->terrainMaterialComponent->GetMaterialNumBlocks();
    n_assert( numBlocks > 0 );

    n_assert( x0 >= 0 && x0 < numBlocks );
    n_assert( z0 >= 0 && z0 < numBlocks );
    n_assert( x1 >= 0 && x1 < numBlocks );
    n_assert( z1 >= 0 && z1 < numBlocks );

    // Number of layer in the terrain
    n_assert( this->terrainMaterialComponent->GetLayerCount() > 0 );

    // Crop rectangle area to update
    if ( x1 < x0 )
    {
        int t = x0; 
        x0 = x1; 
        x1 = t;
    }
    if ( z1 < z0 )
    {
        int t = z0; 
        z0 = z1; 
        z1 = t;
    }
    if ( x0 < 0 )
    {
        x0 = 0;
    }
    if ( z0 < 0 )
    {
        z0 = 0;
    }
    if ( x1 >= numBlocks )
    {
        x1 = numBlocks - 1;
    }
    if ( z1 >= numBlocks )
    {
        z1 = numBlocks - 1;
    }

    // Main loop over blocks
    for ( int z=z0; z <= z1; z++ )
    {
        for ( int x=x0; x <= x1; x++ )
        {
            // Get weightmap to refresh
            nTerrainCellInfo* wmap = this->terrainMaterialComponent->GetTerrainCellInfo( x, z );
            n_assert( wmap );

            // If weightmap has the layers still uncreated, try to load them
            if ( ! wmap->AreLayersCreated() )
            {
                wmap->CreateByteMaps( false );
            }

            // Size of the weightmap
            int wmapSize = wmap->GetWeightMapSize();

            // Loop in the current block's first row and column

            nTerrainCellInfo * northNeighbourWmap = 0;
            if ( z > 0 )
            {
                northNeighbourWmap = this->terrainMaterialComponent->GetTerrainCellInfo( x, z - 1 );
                n_assert( northNeighbourWmap );
            }

            nTerrainCellInfo * westNeighbourWmap = 0;
            if ( x > 0 )
            {
                westNeighbourWmap = this->terrainMaterialComponent->GetTerrainCellInfo( x - 1, z );
                n_assert( westNeighbourWmap );
            }

            // Loop layers
            for (int k = 0; k < wmap->GetNumberOfLayers(); k++)
            {
                // Layer handle
                nTerrainMaterial::LayerHandle layerHandle = wmap->GetLayerHandle( k );

                // Get north weightmap layer k bytemap
                nByteMap* layer = wmap->GetLayerInfo( k ).refLayer;
                if ( ! layer )
                {
                    // If the layer is not created don't do anything in this texel
                    continue;
                }
                nuint8 * currentBlockLayer = layer->GetByteMap();
                n_assert( currentBlockLayer );

                // Loop first row
                if ( z > 0 )
                {
                    // Get layer index in the neighbour weightmap
                    int neighbourLayerIndex = northNeighbourWmap->GetLayerIndexByHandle( layerHandle );
                    if ( neighbourLayerIndex != InvalidLayerIndex )
                    {
                        nByteMap* neighbourLayer = northNeighbourWmap->GetLayerInfo( neighbourLayerIndex ).refLayer.get_unsafe();
                        if ( neighbourLayer )
                        {
                            nuint8 * neighbourBlockLayer = neighbourLayer->GetByteMap();
                            n_assert( neighbourBlockLayer );

                            // First row of current wmap
                            nuint8 *curTexel = currentBlockLayer;
                            // Last row of north wmap
                            nuint8 *neighbTexel = neighbourBlockLayer + wmapSize * ( wmapSize - 1 ) ;
                            for (int wi = 0; wi < wmapSize; wi++ )
                            {
                                *curTexel = *neighbTexel;
                                curTexel++;
                                neighbTexel++;
                            }
                        }
                    }
                }

                // Loop first column
                if ( x > 0 )
                {
                    // Get layer index in the neighbour weightmap
                    int neighbourLayerIndex = westNeighbourWmap->GetLayerIndexByHandle( layerHandle );
                    if ( neighbourLayerIndex != InvalidLayerIndex )
                    {
                        nByteMap* neighbourLayer = westNeighbourWmap->GetLayerInfo( neighbourLayerIndex ).refLayer.get_unsafe();
                        if ( neighbourLayer )
                        {
                            nuint8 * neighbourBlockLayer = neighbourLayer->GetByteMap();
                            n_assert( neighbourBlockLayer );

                            // First column of current wmap
                            nuint8 *curTexel = currentBlockLayer;
                            // Last column of current wmap
                            nuint8 *neighbTexel = neighbourBlockLayer + wmapSize - 1;
                            for (int wi = 0; wi < wmapSize; wi++ )
                            {
                                *curTexel = *neighbTexel;
                                curTexel += wmapSize;
                                neighbTexel += wmapSize;
                            }
                        }
                    }
                }
            }
        }
    }
}

#if 0
//------------------------------------------------------------------------------
/**
    @brief Generate weight texture from a single layer
    @param layer The layer
*/
void
nTerrainWeightMapBuilder::GenerateFromSingleLayer( nTerrainMaterial::LayerHandle layer )
{
    n_assert( this->IsLoaded() );

    // Target weightmap texture - there is only one for the moment
    n_assert( this->refWeightTexture.isvalid() );

    nTexture2::LockInfo wmapLockInfo;
    int wmapRemainder=0;
    nuint8* wmapLine=0;
    if ( this->refWeightTexture->Lock(nTexture2::WriteOnly, 0, wmapLockInfo) )
    {
        wmapLine = (nuint8*) wmapLockInfo.surfPointer;
        // Line remainder: be careful, this is in bytes
        wmapRemainder = wmapLockInfo.surfPitch - ( currentWmapTex->GetWidth() ) * 4;
    }
    else
    {
        n_assert2_always("Couldn't lock weight texture for writing");
    }

    float* layerBuffer = layer->GetHeightMap();

    // Initialize floatmap (layer) coordinates
    int weightPoint = 0;

    int areaSize = this->refWeightTexture->GetWidth();
    int wi0 = 0;
    int wj0 = 0;
    int wTexturePoint = wi0 + wj0 * areaSize;

    // Texture pointer for writing
    nuint8* curLine = wmapLine;
    
    for (int wj = 0; wj < areaSize; wj++ )
    {
        for (int wi = 0; wi < areaSize; wi++ )
        {
            *curLine++ = ( static_cast<nuint8> ( *( layerBuffer + weightPoint ) * 255.0f ) );
            *curLine++ = 0;//( static_cast<nuint8> ( *( layerBuffer + weightPoint ) / 255.0f ) );
            *curLine++ = 0;//( static_cast<nuint8> ( *( layerBuffer + weightPoint ) / 255.0f ) );
            *curLine++ = 0;
            weightPoint++;
            wTexturePoint++;
        }
    }
    this->refWeightTexture->Unlock(0);
}
#endif

//------------------------------------------------------------------------------
/**
    Reset the weightmap texture to pink.
    The shader knows that when all values A + R + G + B > 3
*/
void
nTerrainWeightMapBuilder::CleanWeightMapTexture()
{
    // check target weightmap texture
    n_assert_return( this->refWeightTexture.isvalid(), );
    n_assert_return( this->refWeightTexture->GetFormat() == nTexture2::A8R8G8B8, );
    int weightMapSize = this->terrainMaterialComponent->GetAllWeightMapsSize();
    n_assert_return( weightMapSize > 0, );

    nTexture2::LockInfo cellInfoLockInfo;
    if ( !this->refWeightTexture->Lock(nTexture2::WriteOnly, 0, cellInfoLockInfo) )
    {
        n_assert2_always("Couldn't lock weight texture for writing");
        return;
    }

    // Line remainder: be careful, this is in bytes
    int cellInfoRemainder = cellInfoLockInfo.surfPitch - ( weightMapSize ) * 4;
    nuint8 * cellInfoLine = (nuint8 *) cellInfoLockInfo.surfPointer;

    // Fill texture with empty (pink) weightmap
    nuint8* curLine = cellInfoLine;
    for ( int line = 0; line < weightMapSize; line++ )
    {
        memset(curLine, 0xFF, weightMapSize * 4);
        curLine += ( weightMapSize + cellInfoRemainder ) * 4;
    }

    this->refWeightTexture->Unlock(0);
}

//------------------------------------------------------------------------------
/**
    Set invalid terrain layer indices for a terrain cell.
    There are no terrain material in this terrrain cell
*/
void
nTerrainWeightMapBuilder::CleanWeightMapLayers( int bx, int bz )
{
    n_assert_return( this->terrainMaterialComponent, );

    int numBlocks = this->terrainMaterialComponent->GetMaterialNumBlocks();
    n_assert_return( numBlocks > 0, );
    n_assert_return( bx >= 0 && bx < numBlocks, );
    n_assert_return( bz >= 0 && bz < numBlocks, );

    nTerrainCellInfo* cellInfo = this->terrainMaterialComponent->GetTerrainCellInfo( bx, bz );
    n_assert_return( cellInfo, );

    for (int k = 0; k < MaxWeightMapBlendLayers; k++)
    {
        cellInfo->SetLayerIndex( k, 255 );
    }
}

//------------------------------------------------------------------------------
#endif