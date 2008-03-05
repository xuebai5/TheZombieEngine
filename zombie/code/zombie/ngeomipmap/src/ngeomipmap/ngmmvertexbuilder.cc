/*-----------------------------------------------------------------------------
    @file ngmmvertexbuilder.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmvertexbuilder.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "ngeomipmap/ngmmvertexlodsorting.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
*/
nGMMVertexBuilder::nGMMVertexBuilder() :
    numVerticesPerRow(0),
    numLODLevels(0),
    geometryStorage(0),
    vertexSorting(0),
    scale(0),
    tgmmc(0)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
nGMMVertexBuilder::~nGMMVertexBuilder()
{
    this->SetupTerrain(0);
    this->SetupStorage(0);
    this->SetupVertexSorting(0);
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMVertexBuilder::SetupTerrain(ncTerrainGMMClass * tgmmc)
{
    if (tgmmc) 
    {
        this->refHeightMap = tgmmc->GetHeightMap();
        this->scale = this->refHeightMap->GetGridScale();
        n_assert(this->refHeightMap.isvalid());
        this->numVerticesPerRow = tgmmc->GetBlockSize();
        this->numLODLevels = tgmmc->GetNumLODLevels();
        n_assert(this->numLODLevels > 0 && this->numLODLevels < MaxLODLevels);
        this->tgmmc = tgmmc;
    }
    else
    {
        this->refHeightMap.invalidate();
        this->numVerticesPerRow = 0;
        this->numLODLevels = 0;
        this->tgmmc = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMVertexBuilder::SetupStorage(nGMMGeometryStorage * geom)
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
nGMMVertexBuilder::SetupVertexSorting(nGMMVertexLODSorting * vertexSorting)
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
/**
    Fills the vertex buffer with proper (x,y,z) for a given block (bx, bz).
    @param bx block x coordinate 
    @param bz block z coordinate
    @param lod lod level used in the block
*/
void
nGMMVertexBuilder::FillVerticesBlock(int bx, int bz, int lod)
{
    NLOG(terrain, (nTerrainLog::NLOG_BUILDVERTEX | 0, "FillVerticesBlock(%d,%d,%d)", bx, bz, lod));

    int maxVerticesPerLOD = this->CountVertices(lod);

    // calculate heightmap coordinates (x, z) of top-left corner
    int x = bx * (this->numVerticesPerRow - 1);
    int z = bz * (this->numVerticesPerRow - 1);

    this->geometryStorage->LockVerticesGroup( maxVerticesPerLOD );
#if 1
    int numVertices = 0;
    for(int j = lod;j < this->numLODLevels;j++)
    {
        numVertices += this->vertexSorting->GetVertexCount(j);
    }
    for(int i = 0;i < numVertices;i++)
    {
        const nGMMVertexLODSorting::nGMMVertexLOD & vertex = this->vertexSorting->GetVertex(i);
        n_assert(vertex.lod >= lod);
        this->AddVertex(i, x + vertex.x, z + vertex.z);
    }
#else

    int lodstep  = (1 << lod);
    for(int iz = z;iz < (z + this->numVerticesPerRow); iz += lodstep) 
    {
        for(int ix = x;ix < (x + this->numVerticesPerRow); ix += lodstep) 
        {
            this->AddVertex(x, z, ix, iz);
        }
    }
#endif
    this->geometryStorage->UnlockVerticesGroup();
}

//------------------------------------------------------------------------------
/**
    Calculate the number of vertices of a block needed for a given a lod
*/
inline
int 
nGMMVertexBuilder::CountVertices(int lod) const
{
    int numVertices = 0;
    for(int j = lod;j < this->numLODLevels;j++)
    {
        numVertices += this->vertexSorting->GetVertexCount(j);
    }

    return numVertices;
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMVertexBuilder::AddVertex(int index, int ix, int iz)
{
    vector3 * vertex = this->geometryStorage->GetVertex(index);

    vertex->x = ix * this->scale;
    vertex->y = this->refHeightMap->GetHeightLC(ix, iz);
    vertex->z = iz * this->scale;

    NLOG(terrain, (nTerrainLog::NLOG_BUILDVERTEX | 1, "AddVertex(%d,%d,%d) (%3.3f,%3.3f,%3.3f)", index, ix, iz, vertex->x, vertex->y, vertex->z));

    if (this->geometryStorage->GetFillNormals())
    {
        this->refHeightMap->GetNormal(ix, iz, vertex + 1);
    }
}
