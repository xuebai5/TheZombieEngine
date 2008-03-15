#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolslope_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolslope.h"
#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiTerrainToolSlope, "ninguiterraintoolgeom");

//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolSlope::nInguiTerrainToolSlope()
{
    this->alwaysUsePicking = true;
    label.Set("Set terrain slope");
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolSlope::~nInguiTerrainToolSlope()
{
    // empty
}
//------------------------------------------------------------------------------   
/**
    @brief Apply the tool, given a world ray
*/
bool
nInguiTerrainToolSlope::Apply( nTime dt )
{
    n_assert( this->currentPaintbrush );

    nFloatMap* hmap = heightMap;

    int pbSize = currentPaintbrush->GetSize();
    int xApply = this->lastXMousePos - pbSize / 2;
    int zApply = this->lastZMousePos - pbSize / 2;

    // Get terrain normal
    float h0;
    vector3 normal;
    if ( ! hmap->GetHeightNormal( this->lastPos.x, this->lastPos.z, h0, normal ) )
    {
        h0 =0.0f;
        normal.set(0.0f, 1.0f, 0.0f);
    }

    // Rotate the normal according to user slope value
    if ( targetSlope > TINY )
    {
        float ang = tan ( ( 90.0f - targetSlope ) * PI / 180.0f );
        normal.y = vector2( normal.x, normal.z ).len() * ang ;
    }
    else
    {
        normal = vector3( 0.0f, 1.0f, 0.0f );
    }

    // Fill tempPaintbrush with the target plane
    FillWithPlane( tempPaintbrush1, normal, h0 );

    // Subtract the source map in 'temp'
    this->fmBuilder.Sub( hmap, -xApply, -zApply, tempPaintbrush1);

    // Clamp it so the terrain will only raise, not lower
    tempPaintbrush1->SetHeightOffset( hmap->GetHeightOffset() );
    tempPaintbrush1->SetHeightScale( hmap->GetHeightScale() );

    this->fmBuilder.Clamp( tempPaintbrush1 );

    // Multiply it by the paintbrush and intensity
    this->fmBuilder.Mult( currentPaintbrush, 0, 0, this->intensity * float(dt), tempPaintbrush1 );
    // Add the result to the source heightmap
    this->fmBuilder.Add( tempPaintbrush1, xApply, zApply, hmap );
    // Clamp it in the region modified
    this->fmBuilder.Clamp( hmap, xApply, zApply, tempPaintbrush1 );

    this->MakeRectangleDirtyGeometry(xApply, zApply, xApply + pbSize, zApply + pbSize);

    return true;
}
//------------------------------------------------------------------------------   
/**
	@brief Fill a heightmap with a plane
    @param hmap The heightmap to be filled
    @param normal Normal of the plane, in HC
    @param h0 height of the plane at the central point of the heightmap, in HC

*/
void
nInguiTerrainToolSlope::FillWithPlane(nFloatMap* hmap, vector3 normal, float h0)
{

    if ( abs( normal.x ) < TINY || abs( normal.y ) < TINY || abs( normal.z ) < TINY )
    {
        hmap->FillHeight( h0 );
        return;
    }
    
    int size = hmap->GetSize();

    float dx = - normal.x / normal.y;
	float dz = - normal.z / normal.y;

    float x, z;
	z = - size / 2.0f;
	for (int j = 0; j < size; j++)
	{
		x = - size / 2.0f;
		for (int i = 0; i < size; i++)
		{
			float h = dx * x + dz * z + h0;

            hmap->SetHeightLC( i, j, h );
			x += 1.0f;
		}
		z += 1.0f;
	}
}
//------------------------------------------------------------------------------