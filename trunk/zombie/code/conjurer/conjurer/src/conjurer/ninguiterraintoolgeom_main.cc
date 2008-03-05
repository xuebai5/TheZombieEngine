#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguiterraintoolgeom_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguiterraintoolgeom.h"
#include "kernel/nkernelserver.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/ncterraingmm.h"
#include "napplication/napplication.h"
#include "conjurer/nassetloadstate.h"
#include "nscene/ncsceneclass.h"
#include "nphysics/ncphyterrain.h"

//------------------------------------------------------------------------------

nNebulaScriptClass(nInguiTerrainToolGeom, "ninguiterraintool");
//------------------------------------------------------------------------------

/**
*/
nInguiTerrainToolGeom::nInguiTerrainToolGeom():
    intensity( 1.0f )
{
    this->cursorColor = vector4 (1.0f, 0.3f, 0.0f, 0.5f );
    this->cursor3DLine->SetLineMargin( 0.05f );
}
//------------------------------------------------------------------------------
/**
*/
nInguiTerrainToolGeom::~nInguiTerrainToolGeom()
{
    // empty
}

//------------------------------------------------------------------------------   
/**
    Mark a rectangle as dirty, needed to invalidate geometry cache.
*/
void
nInguiTerrainToolGeom::MakeRectangleDirtyGeometry(int x0, int z0, int x1, int z1)
{
    if (this->outdoor.isvalid())
    {
        ncTerrainGMM * tgmm = this->outdoor->GetComponent<ncTerrainGMM>();
        n_assert_return(tgmm,);
        tgmm->UpdateGeometry(x0, z0, x1, z1);

        if ( this->GetState() == nInguiTool::Finishing )
        {
            // Update physics bounding boxes
            ncPhyTerrain* phyTerrain = outdoor->GetComponent<ncPhyTerrain>();
            phyTerrain->UpdateTerrainBoundingBoxes();
        }
    }
}

//------------------------------------------------------------------------------   
/**
    Mark a rectangle as dirty, needed to invalidate material cache.
*/
void
nInguiTerrainToolGeom::MakeRectangleDirtyMaterial(int x0, int z0, int x1, int z1)
{
    NLOG(conjurer, (0, "MakeRectangleDirtyMaterial(%d,%d,%d,%d)", x0, z0, x1, z1));

    if (this->outdoor.isvalid())
    {
        nEntityClass * neclass = this->outdoor->GetEntityClass();
        if (neclass)
        {
            ncTerrainMaterialClass * matClass = neclass->GetComponent<ncTerrainMaterialClass>();
            if (matClass)
            {
                matClass->UpdateMaterial(x0, z0, x1, z1);
            }
        }
    }
}

//------------------------------------------------------------------------------
