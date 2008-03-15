#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolraiselow_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolraiselow.h"
#include "conjurer/ninguiterraintoolsmooth.h"
#include "kernel/nkernelserver.h"
#include "ngeomipmap/nfloatmap.h"
#include "input/ninputserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiTerrainToolRaiseLow, "ninguiterraintoolgeom");

//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolRaiseLow::nInguiTerrainToolRaiseLow():
    doSmooth(false)
{
    label.Set("Raise/low terrain");
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolRaiseLow::~nInguiTerrainToolRaiseLow()
{
    // empty
}
//------------------------------------------------------------------------------
bool
nInguiTerrainToolRaiseLow::HandleInput( nAppViewport* /*vp*/ )
{
    // 'Alt' reverses the effect of the Raise/Dig tool
    if ( nInputServer::Instance()->GetButton("buton0_alt") || nInputServer::Instance()->GetButton("buton0_down_alt") )
    {
        this->raiseNotLower = false;
        this->doSmooth = false;
    }
    else if ( nInputServer::Instance()->GetButton("buton0_shift") || nInputServer::Instance()->GetButton("buton0_down_shift") )
    {
        this->doSmooth = true;
    }
    else
    {
        this->raiseNotLower = true;
        this->doSmooth = false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Apply the tool, given a world ray
*/
bool
nInguiTerrainToolRaiseLow::Apply( nTime dt )
{
    n_assert( this->currentPaintbrush );

    int pbSize = currentPaintbrush->GetSize();
    int xApply = this->lastXMousePos - pbSize / 2;
    int zApply = this->lastZMousePos - pbSize / 2;

    if ( this->doSmooth )
    {
        this->fmBuilder.Smooth( currentPaintbrush, xApply, zApply, this->intensity * float(dt), tempPaintbrush1, heightMap );
        this->fmBuilder.Clamp( heightMap, xApply, zApply, tempPaintbrush1 );
    }
    else
    {
        if ( this->raiseNotLower )
        {
            this->fmBuilder.MultAdd( this->currentPaintbrush, xApply, zApply, 0.3f * this->intensity * float(dt) * heightMap->GetHeightScale(), heightMap.get() );
        }
        else
        {
            this->fmBuilder.MultSub( this->currentPaintbrush, xApply, zApply, 0.3f * this->intensity * float(dt) * heightMap->GetHeightScale(), heightMap.get() );
        }

        // Clamp it in the region modified
        this->fmBuilder.Clamp( this->heightMap, xApply, zApply, currentPaintbrush.get() );
    }

    this->MakeRectangleDirtyGeometry(xApply, zApply, xApply + pbSize, zApply + pbSize);
    
    return true;
}

//------------------------------------------------------------------------------   
