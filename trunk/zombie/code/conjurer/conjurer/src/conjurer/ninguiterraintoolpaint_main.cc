#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolpaint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolpaint.h"
#include "kernel/nkernelserver.h"
#include "input/ninputserver.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/nfloatmapbuilder.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiTerrainToolPaint, "ninguiterraintoolgeom");

//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolPaint::nInguiTerrainToolPaint():
    paintNotErase( true ),
    heightFilter( false ),
    slopeFilter( false ),
    blendIntensity( 1.0f )
{
    filterMinHeight = 1.0f;
    filterMaxHeight = 50.0f;

    filterMinSlope = 0.0f;
    filterMaxSlope = 90.0f;

    label.Set("Paint terrain");
    alwaysUsePicking = true;
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolPaint::~nInguiTerrainToolPaint()
{
    // empty
}
//------------------------------------------------------------------------------
bool
nInguiTerrainToolPaint::HandleInput( nAppViewport* /*vp*/ )
{
    // 'Alt' reverses the effect of the paint tool (i.e., erase)
    if ( nInputServer::Instance()->GetButton("buton0_alt") || nInputServer::Instance()->GetButton("buton0_down_alt"))
    {
        this->paintNotErase = false;
        return true;
    }
    else
    {
        this->paintNotErase = true;
        return false;
    }
}

//------------------------------------------------------------------------------
/// Set reference to layer manager
void
nInguiTerrainToolPaint::SetOutdoor( nEntityObject * entityObject )
{
    nInguiTerrainToolGeom::SetOutdoor(entityObject);

    if ( entityObject )
    {
        // get the material manager class component from the outdoor object
        this->layerManager = entityObject->GetClassComponent<ncTerrainMaterialClass>();
        n_assert(this->layerManager);
    }
}

//------------------------------------------------------------------------------   
/**
    @brief Apply the tool, given a world ray
*/
bool
nInguiTerrainToolPaint::Apply( nTime dt )
{
    n_assert( this->heightMap );
    n_assert( this->currentPaintbrush );
    n_assert( this->tempPaintbrush1 );
    n_assert( this->layerManager );

    if ( this->layerManager->GetSelectedLayerHandle() < 0 )
    {
        // No layer is selected, do nothing
        return false;
    }

    // Calc. scale
    float mul = this->GetDrawResolutionMultiplier();
    float s = this->heightMap->GetGridScale() / mul;
    n_assert( s > 0 );

    // Calculate apply position

    int pbrushSize = this->currentPaintbrush->GetSize();
    int offs = pbrushSize / 2;
    if ( ! (pbrushSize & 1 ) )
    {
        offs --;
    }
    int xApply = int( this->lastPos.x / s ) - offs;
    int zApply = int( this->lastPos.z / s ) - offs;

    nFloatMap* pbrush = this->currentPaintbrush;

    // Calc. filters (apply filters to brush)
    if ( this->heightFilter || this->slopeFilter )
    {

        this->CalculateFilters( this->lastPos.x - pbrushSize * s / 2, this->lastPos.z - pbrushSize * s / 2 );

        //this->fmBuilder.Copy( this->tempPaintbrush1, 0, 0, this->tempPaintbrush2);        
        this->fmBuilder.Smooth( this->tempPaintbrush1, this->tempPaintbrush2 );
        this->fmBuilder.Smooth( this->tempPaintbrush2, this->tempPaintbrush1 );
        this->fmBuilder.Smooth( this->tempPaintbrush1, this->tempPaintbrush2 );

        this->fmBuilder.Mult( this->currentPaintbrush, 0, 0, 1.0f, this->tempPaintbrush2);

        pbrush = this->tempPaintbrush2;

/*        this->bmBuilder.Copy( this->filterBuffer, -xApply, -zApply, this->tempPaintbrush1);
        this->bmBuilder.Smooth( this->tempPaintbrush1, this->tempPaintbrush2 );
        this->bmBuilder.Smooth( this->tempPaintbrush2, this->tempPaintbrush1 );
        
        pbrush = this->tempPaintbrush1;
        this->bmBuilder.Mult( this->currentPaintbrush, 0, 0, 1.0f, pbrush);
*/
    }

    // Calc. affected rectangle in index coords.
    // @todo When weightmap resolution is not constant, the affected weightmap rectangle must be calculated
    // assuming that xApply, zApply are in coordinates of the max resolution weightmap
    int numBlocks = this->layerManager->GetMaterialNumBlocks();
    int areaSize = this->layerManager->GetSingleWeightMapSize(0, 0);
    int x0 = max( 0, xApply / areaSize );
    int z0 = max( 0, zApply / areaSize );
    int x1 = min( ( xApply + pbrushSize + 1 ) / areaSize, numBlocks -1);
    int z1 = min( ( zApply + pbrushSize + 1 ) / areaSize, numBlocks -1);

    // Get the selected layer handle
    nTerrainMaterial::LayerHandle destLayerH = this->layerManager->GetSelectedLayerHandle();

    // Loop affected weightmaps
    for ( int x = x0; x <= x1; x++ )
    {
        for ( int z = z0; z <= z1; z++ )
        {
            nTerrainCellInfo* wmap = this->layerManager->GetTerrainCellInfo( x, z ); 
            n_assert( wmap );

            // If weightmap has the layers still uncreated, try to load them
            if ( ! wmap->AreLayersCreated() )
            {
                wmap->CreateByteMaps( false );
            }

            // If layer doesn't exist in the weightmap, add it
            int wmapLayerI = wmap->GetLayerIndexByHandle( destLayerH );
            if ( wmapLayerI == InvalidLayerIndex ) 
            {
                wmap->AddLayerHandle( destLayerH );
                wmap->CreateByteMaps( true );
            }

            float fi =  this->intensity * float(dt) * 255.0f;
            float bi = fi * this->blendIntensity * float(dt) * 255.0f;

            // If painting, paint selected layers and erase the others
            if ( this->paintNotErase )
            {
                // Loop over all layers
                for (int layer=0; layer < wmap->GetNumberOfLayers(); layer++)
                {
                    // Get weightmap layer info
                    nTerrainCellInfo::WeightMapLayerInfo& layerInfo = wmap->GetLayerInfo( layer );
                    // Get layer handle
                    nTerrainMaterial::LayerHandle layerH = layerInfo.layerHandle;
                    //Get floatmap
                    nByteMap* dest = layerInfo.refLayer;

                    if ( layerH == destLayerH )
                    {
                        this->bmBuilder.MultAdd( pbrush, xApply - x * areaSize, zApply - z * areaSize, fi, dest, 1 );
                    }
                    else
                    {
                        this->bmBuilder.MultSub( pbrush, xApply - x * areaSize, zApply - z * areaSize, bi, dest );
                    }

                    // Update layer use
                    //wmap->CalculateLayerUse( layerH );
                }
            }
            else
            {
                // Get selected layer index
                int layerI = wmap->GetLayerIndexByHandle( destLayerH );
                // Get weightmap layer info
                nTerrainCellInfo::WeightMapLayerInfo& layerInfo = wmap->GetLayerInfo( layerI );
                //Get floatmap
                nByteMap* dest = layerInfo.refLayer;

                // Erase the layer
                this->bmBuilder.MultSub( pbrush, xApply - x * areaSize, zApply - z * areaSize, fi, dest );

                // Update layer use
                //wmap->CalculateLayerUse( destLayerH );
            }

            wmap->SetDirty( true );
        }
    }

    // Notify the material info for this layer has been updated
    this->MakeRectangleDirtyMaterial( xApply, zApply, xApply + pbrushSize + 1, zApply + pbrushSize + 1);

    return true;
}
//------------------------------------------------------------------------------  
/**
    @brief Calculate the height and slope filtering
    @param x0 x position of brush filter in world coordinates
    @param z0 z position of brush filter in world coordinates
*/
void
nInguiTerrainToolPaint::CalculateFilters(float x0, float z0)
{
    if ( this->tempPaintbrush1.isvalid() && ( this->heightFilter || this->slopeFilter ) )
    {
        float minTan = tan( n_deg2rad( min( 89.0f, this->filterMinSlope ) ) );
        float maxTan = tan( n_deg2rad( min( 89.0f, this->filterMaxSlope ) ) );
        float* dest = this->tempPaintbrush1->GetHeightMap();
        int s = this->tempPaintbrush1->GetSize();
        int n = s * s;
        int i = 0;
        float scale = this->heightMap->GetGridScale() / this->GetDrawResolutionMultiplier();
        float z = z0;
        float x = x0;
        for (int p = 0; p < n; p++)
        {
            float h;
            vector3 normal;

            this->heightMap->GetHeightNormal(x, z, h, normal);

            if ( this->heightFilter )
            {
                if ( h >= this->filterMinHeight && h <= this->filterMaxHeight )
                {
                    *dest = 1.0f;
                }
                else
                {
                    *dest = 0.0f;
                }
            }
            else
            {
                *dest = 1.0f;
            }

            if ( this->slopeFilter )
            {
                float l1 = normal.x * normal.x + normal.z * normal.z;
                float c = sqrt( l1 ) / normal.y;
                if ( c >= minTan && c <= maxTan )
                {
                    *dest *= 1.0f;
                }
                else
                {
                    *dest *= 0.0f;
                }
            }

            dest++;
            i++;
            x += scale;
            if ( i == s )
            {
                x = x0;
                z += scale;
                i = 0;
            }
        }
    }
}
//------------------------------------------------------------------------------  
/**
    @brief Get draw resolution multiplier
*/
float
nInguiTerrainToolPaint::GetDrawResolutionMultiplier()
{
    // @todo JJ refactor terrain: When weightmap resolution is not constant, this changes to the (mini)weightmap size(x,z), the resolution of the block
    n_assert( this->layerManager );
    return float ( this->layerManager->GetAllWeightMapsSize() * this->layerManager->GetMaterialNumBlocks() ) / float( this->heightMap->GetSize() - 1 );
}
//------------------------------------------------------------------------------
/**
    @brief Get filter low height
*/
float
nInguiTerrainToolPaint::GetMinFilterHeight()
{
    return this->filterMinHeight;
}
//------------------------------------------------------------------------------
/**
    @brief Get filter high height
*/
float
nInguiTerrainToolPaint::GetMaxFilterHeight()
{
    return this->filterMaxHeight;
}
//------------------------------------------------------------------------------
/**
    @brief Get filter low slope
*/
float
nInguiTerrainToolPaint::GetMinFilterSlope()
{
    return this->filterMinSlope;
}
//------------------------------------------------------------------------------
/**
    @brief Get filter high slope
*/
float
nInguiTerrainToolPaint::GetMaxFilterSlope()
{
    return this->filterMaxSlope;
}
//------------------------------------------------------------------------------   
/**
    @brief Get height filter enabled
*/
bool
nInguiTerrainToolPaint::GetFilterHeightEnabled()
{
    return this->heightFilter;
}
//------------------------------------------------------------------------------   
/**
    @brief Get slope filter enabled
*/
bool
nInguiTerrainToolPaint::GetFilterSlopeEnabled()
{
    return this->slopeFilter;
}//------------------------------------------------------------------------------
/**
    @brief Set filter low height
*/
void
nInguiTerrainToolPaint::SetMinFilterHeight(float h)
{
    this->filterMinHeight = h;
}
//------------------------------------------------------------------------------
/**
    @brief Set filter high height
*/
void
nInguiTerrainToolPaint::SetMaxFilterHeight(float h)
{
    this->filterMaxHeight = h;
}
//------------------------------------------------------------------------------
/**
    @brief Set filter low slope
*/
void
nInguiTerrainToolPaint::SetMinFilterSlope(float h)
{
    this->filterMinSlope = h;
}
//------------------------------------------------------------------------------
/**
    @brief Set filter high slope
*/
void
nInguiTerrainToolPaint::SetMaxFilterSlope(float h)
{
    this->filterMaxSlope = h;
}
//------------------------------------------------------------------------------   
/**
    @brief Set height filter enabled
*/
void
nInguiTerrainToolPaint::SetFilterHeightEnabled(bool e)
{
    this->heightFilter = e;
}
//------------------------------------------------------------------------------   
/**
    @brief Set slope filter enabled
*/
void
nInguiTerrainToolPaint::SetFilterSlopeEnabled(bool e)
{
    this->slopeFilter = e;
}

//------------------------------------------------------------------------------
