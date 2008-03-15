#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolgrass_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolgrass.h"
#include "kernel/nkernelserver.h"
#include "input/ninputserver.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "nvegetation/ncterrainvegetationcell.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiTerrainToolGrass, "ninguiterraintoolgeom");

//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolGrass::nInguiTerrainToolGrass():
    currentGrassId( 0 ),
    terrainNumBlocks( 0 )
{
    label.Set("Terrain grass edit");
    alwaysUsePicking = true;
}

//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolGrass::~nInguiTerrainToolGrass()
{
    // empty
}

//------------------------------------------------------------------------------
/// Set reference to layer manager
void
nInguiTerrainToolGrass::SetOutdoor( nEntityObject * entityObject )
{
    nInguiTerrainToolGeom::SetOutdoor(entityObject);

    if ( entityObject )
    {
        // get the grass manager class component from the outdoor object
        this->grassManager = entityObject->GetClassComponent<ncTerrainVegetationClass>();
        n_assert(this->grassManager);

        // Get terrain layer manager 
        this->layerManager = entityObject->GetClassComponent<ncTerrainMaterialClass>();
        n_assert( layerManager );
    }
}

//------------------------------------------------------------------------------   
/**
    @brief Apply the tool, given a world ray
*/
bool
nInguiTerrainToolGrass::Apply( nTime /*dt*/ )
{
    n_assert( this->heightMap );
    n_assert( this->currentPaintbrush );
    n_assert( this->tempPaintbrush1 );
    n_assert( this->grassManager );

    if ( this->currentGrassId < 0 )
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

    // Calc. affected rectangle in index coords.
    int areaSize = this->grassManager->GetGrowthMapSizeByCell();
    int x0 = max( 0, xApply / areaSize );
    int z0 = max( 0, zApply / areaSize );
    int x1 = min( ( xApply + pbrushSize + 1 ) / areaSize, this->layerManager->GetMaterialNumBlocks() -1);
    int z1 = min( ( zApply + pbrushSize + 1 ) / areaSize, this->layerManager->GetMaterialNumBlocks() -1);

    // Begin grass edition if not begun yet
    this->grassManager->BeginGrowthEditon();

    // Loop affected weightmaps
    for ( int x = x0; x <= x1; x++ )
    {
        for ( int z = z0; z <= z1; z++ )
        {
            // Get cell info
            nTerrainCellInfo* cellInfo = this->layerManager->GetTerrainCellInfo( x, z ); 
            n_assert( cellInfo );

            // Get cell entity's vegetation component
            nEntityObject * cellEntity = cellInfo->GetTerrainCell();
            n_assert(cellEntity);
            ncTerrainVegetationCell * vegCell = cellEntity->GetComponentSafe<ncTerrainVegetationCell>();
            n_assert( vegCell );

            // Get cell grass bytemap
            nByteMap * grassByteMap = vegCell->GetValidGrowthMap();

            // Paint grass
            this->bmBuilder.Substitute( pbrush, xApply - x * areaSize, zApply - z * areaSize, this->currentGrassId, grassByteMap );
            vegCell->UpdateTextureBeforeRender();

            grassByteMap->SetUserDirty();
        }
    }

    return true;
}

//------------------------------------------------------------------------------  
/**
    @brief Get draw resolution multiplier
*/
float
nInguiTerrainToolGrass::GetDrawResolutionMultiplier()
{
    n_assert( this->grassManager );
    n_assert( this->layerManager );
    return float ( this->grassManager->GetGrowthMapSizeByCell() * this->layerManager->GetMaterialNumBlocks() ) / float( this->heightMap->GetSize() - 1 );
}

//------------------------------------------------------------------------------
/**
    @brief Set grass id to paint
    @param grass id
*/
void
nInguiTerrainToolGrass::SetGrassId( int id )
{
    // Grass id are 8 bit values
    n_assert( id >= 0 && id <= 0xFF );
    this->currentGrassId = static_cast<nuint8>( id );
}

//------------------------------------------------------------------------------
/**
    @brief Get current grass id to paint
    @return grass id
*/
int
nInguiTerrainToolGrass::GetGrassId( void )
{
    return this->currentGrassId;
}

//------------------------------------------------------------------------------
/**
    @brief Stops the grass edition
*/
void
nInguiTerrainToolGrass::OnDeselected()
{
    // without save
    this->grassManager->EndGrowtEdition( false );
}

//------------------------------------------------------------------------------
/**
    @brief Begin grass edition
*/
void
nInguiTerrainToolGrass::OnSelected()
{
    this->grassManager->BeginGrowthEditon();
}


//------------------------------------------------------------------------------