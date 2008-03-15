#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolflatten_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolflatten.h"
#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiTerrainToolFlatten, "ninguiterraintoolgeom");

//------------------------------------------------------------------------------
NSIGNAL_DEFINE( nInguiTerrainToolFlatten, RefreshFlattenHeight );

//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolFlatten::nInguiTerrainToolFlatten()
{
    label.Set("Flatten terrain");
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolFlatten::~nInguiTerrainToolFlatten()
{
    // empty
}
//------------------------------------------------------------------------------   

/**
    @brief Apply the tool, given a world ray
    @param ray The ray
*/
bool
nInguiTerrainToolFlatten::Apply( nTime dt )
{
    n_assert( this->currentPaintbrush );

    int pbSize = currentPaintbrush->GetSize();
    int xApply = this->lastXMousePos - pbSize / 2;
    int zApply = this->lastZMousePos - pbSize / 2;

    // Fill 'temp' with the target height
    tempPaintbrush1->FillHeight( max( heightMap->GetHeightOffset(), min( heightMap->GetHeightOffset() + heightMap->GetHeightScale(), this->targetHeight ) ) );
    // Subtract the source map in 'temp'
    this->fmBuilder.Sub( heightMap, -xApply, -zApply, tempPaintbrush1);
    // Multiply it by the paintbrush and intensity
    this->fmBuilder.Mult( currentPaintbrush, 0, 0, this->intensity * float(dt), tempPaintbrush1 );
    // Add the result to the source heightmap
    this->fmBuilder.Add( tempPaintbrush1,  xApply, zApply, heightMap );
    // Clamp it in the region modified
    this->fmBuilder.Clamp(heightMap, xApply, zApply, tempPaintbrush1);

    this->MakeRectangleDirtyGeometry(xApply, zApply, xApply + pbSize, zApply + pbSize);

    return false;
}
//------------------------------------------------------------------------------   
