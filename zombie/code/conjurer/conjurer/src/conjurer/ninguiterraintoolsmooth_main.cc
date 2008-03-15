#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolsmooth_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolsmooth.h"
#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiTerrainToolSmooth, "ninguiterraintoolgeom");

//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolSmooth::nInguiTerrainToolSmooth()
{
    label.Set("Smooth terrain");
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolSmooth::~nInguiTerrainToolSmooth()
{
    label.Set("Smooth terrain");
}
//------------------------------------------------------------------------------
/**
    @brief Apply the tool, given a world ray
*/
bool
nInguiTerrainToolSmooth::Apply( nTime dt )
{
    n_assert( this->currentPaintbrush );

    int pbSize = currentPaintbrush->GetSize();
    int xApply = this->lastXMousePos - pbSize / 2;
    int zApply = this->lastZMousePos - pbSize / 2;

    this->fmBuilder.Smooth( currentPaintbrush, xApply, zApply, this->intensity * float(dt), tempPaintbrush1, heightMap );
    this->fmBuilder.Clamp( heightMap, xApply, zApply, tempPaintbrush1 );

    this->MakeRectangleDirtyGeometry(xApply, zApply, xApply + pbSize, zApply + pbSize);

    return true;
}
//------------------------------------------------------------------------------   
