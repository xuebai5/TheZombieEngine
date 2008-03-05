#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolnoise.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "stdlib.h"
#include "conjurer/ninguiterraintoolnoise.h"
#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiTerrainToolNoise, "ninguiterraintoolgeom");

//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolNoise::nInguiTerrainToolNoise()
{
    label.Set("Noise terrain");
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolNoise::~nInguiTerrainToolNoise()
{
    // empty
}
//------------------------------------------------------------------------------
/**
    @brief Apply the tool, given a world ray
*/
bool
nInguiTerrainToolNoise::Apply( nTime dt )
{
    n_assert( this->currentPaintbrush );

    int pbSize = currentPaintbrush->GetSize();
    int xApply = this->lastXMousePos - pbSize / 2;
    int zApply = this->lastZMousePos - pbSize / 2;

    nFloatMap* hmap = heightMap;

    tempPaintbrush1->SetHeightOffset( -0.5 );
    NoiseHeightMap( tempPaintbrush1 );
    this->fmBuilder.Mult( currentPaintbrush, 0, 0, this->intensity * float(dt) * hmap->GetHeightScale(), tempPaintbrush1);
    this->fmBuilder.Add( tempPaintbrush1, xApply, zApply, hmap);
    this->fmBuilder.Clamp( hmap, xApply, zApply, tempPaintbrush1 );
    tempPaintbrush1->SetHeightOffset( 0 );

    this->MakeRectangleDirtyGeometry(xApply, zApply, xApply + pbSize, zApply + pbSize);

    return true;
}
//------------------------------------------------------------------------------   
/**
    @brief Fill a heightmap with noise
    @param hmap The heightmap to be filled
*/
void
nInguiTerrainToolNoise::NoiseHeightMap(nFloatMap* hmap)
{
    n_assert( hmap );
    int size = hmap->GetSize();
	float dx = 2.0f / size ;
	float dy = 2.0f / size ;

    float x, y;

	y = -1.0;
	for (int j = 0; j < size; j++)
	{
		x = -1.0;
		for (int i = 0; i < size; i++)
		{
            hmap->SetHeightHC(i, j, n_rand() );
			x += dx;
		}
		y += dy;
	}
}
