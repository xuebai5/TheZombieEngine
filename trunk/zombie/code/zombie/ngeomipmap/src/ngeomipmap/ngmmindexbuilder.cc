/*-----------------------------------------------------------------------------
    @file ngmmindexbuilder.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmindexbuilder.h"
#include "kernel/nlogclass.h"

#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "nphysics/ncphyterraincell.h"

//------------------------------------------------------------------------------
/**
*/
nGMMIndexBuilder::nGMMIndexBuilder() :
    blockSize(0),
    numLODLevels(0),
    geometryStorage(0),
    vertexSorting(0)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
nGMMIndexBuilder::~nGMMIndexBuilder()
{
    this->SetupTerrain(0);
    this->SetupStorage(0);
    this->SetupVertexSorting(0);
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMIndexBuilder::SetupTerrain(ncTerrainGMMClass * tgmmc)
{
    if (tgmmc) 
    {
        this->blockSize = tgmmc->GetBlockSize();
        this->numLODLevels = tgmmc->GetNumLODLevels();
        n_assert(this->numLODLevels > 0 && this->numLODLevels < MaxLODLevels);
        this->tgmmc = tgmmc;
    }
    else
    {
        this->blockSize = 0;
        this->numLODLevels = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMIndexBuilder::SetupStorage(nGMMGeometryStorage * geom)
{
    if (geom)
    {
        geom->AddRef();
    }

    if (this->geometryStorage)
    {
        this->geometryStorage->Release();
    }

    this->geometryStorage = geom;
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMIndexBuilder::SetupVertexSorting(nGMMVertexLODSorting * vertexSorting)
{
    if (vertexSorting)
    {
        vertexSorting->AddRef();
    }

    if (this->vertexSorting)
    {
        this->vertexSorting->Release();
    }

    this->vertexSorting = vertexSorting;
}

//------------------------------------------------------------------------------
void
nGMMIndexBuilder::AddTriangleHoleSafe(int x1, int z1, int x2, int z2, int x3, int z3)
{
    if (!this->phyCell->GetHole(x1,z1) ||
        !this->phyCell->GetHole(x2,z2) ||
        !this->phyCell->GetHole(x3,z3))
    {
        this->geometryStorage->AddTriangle(
            this->vertexSorting->GetVertexIndex(x1, z1),
            this->vertexSorting->GetVertexIndex(x2, z2),
            this->vertexSorting->GetVertexIndex(x3, z3)
        );
    }
}

//------------------------------------------------------------------------------
ncPhyTerrainCell *
nGMMIndexBuilder::GetPhysicalCell(int bx, int bz)
{
    ncTerrainMaterialClass * matc = this->tgmmc->GetComponentSafe<ncTerrainMaterialClass>();
    n_assert(matc);
    nTerrainCellInfo * cellInfo = matc->GetTerrainCellInfo(bx, bz);
    n_assert(cellInfo);
    ncPhyTerrainCell * phyCell = cellInfo->GetTerrainCell()->GetComponentSafe<ncPhyTerrainCell>();
    n_assert(phyCell);
    return phyCell;
}

//------------------------------------------------------------------------------
void 
nGMMIndexBuilder::FillIndicesHoledBlock(int /*bx*/, int /*bz*/, int lod, int lodN, int lodE, int lodS, int lodW)
{
    this->FillIndicesBlock(lod, lodN, lodE, lodS, lodW);
}
