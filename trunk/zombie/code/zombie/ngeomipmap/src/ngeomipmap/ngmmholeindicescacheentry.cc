/*-----------------------------------------------------------------------------
    @file ngmmholeindicescacheentry.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    (C) 2006 Conjurer Services, S.A.
*/
//---------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmholeindicescacheentry.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ngmmindexbuilder.h"
#include "ngeomipmap/ngmmmeshstorage.h"

//---------------------------------------------------------------------------
/**
    The cache key has 3 elements bx, bz and index group. Each one must not be
    larger than following value MAX_CACHE_KEY_ITEM_SIZE.
*/
const int MAX_CACHE_KEY_ITEM_SIZE = (1 << 10);

//---------------------------------------------------------------------------
ncTerrainGMMClass * nGMMHoleIndicesCacheEntry::tgmmc = 0;
//nHeightMapMeshBuilder nGMMHoleIndicesCacheEntry::meshBuilder;

//---------------------------------------------------------------------------
/**
*/
nGMMHoleIndicesCacheEntry::nGMMHoleIndicesCacheEntry() :
    cell(0)
{
    /// empty
}

//---------------------------------------------------------------------------
/**
*/
nGMMHoleIndicesCacheEntry::~nGMMHoleIndicesCacheEntry()
{
    this->Unload();
    this->Dealloc();
}

//---------------------------------------------------------------------------
/**
    Allocate buffers for the index buffer of a hole terrain cell.
*/
bool
nGMMHoleIndicesCacheEntry::Alloc()
{
    this->nCacheEntry::Alloc();
    nGMMIndexBuilder * indexBuilder = this->tgmmc->GetIndexBuilder();

    this->cell = 0;
    this->mesh = nGfxServer2::Instance()->NewMesh(0);
    n_assert(this->mesh);
    this->mesh->SetVertexComponents(0);
    this->mesh->SetUsage( nMesh2::NeedsVertexShader );
    this->mesh->SetNumVertices( 0 );
    this->mesh->SetNumIndices( indexBuilder->CountIndices(0) );
    this->mesh->SetNumGroups(1);
    return mesh->CreateEmpty();
}

//---------------------------------------------------------------------------
/**
*/
bool 
nGMMHoleIndicesCacheEntry::Dealloc()
{
    this->nCacheEntry::Dealloc();

    this->cell = 0;
    if (this->mesh.isvalid())
    {
        this->mesh->Release();
        this->mesh.invalidate();
    }

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool 
nGMMHoleIndicesCacheEntry::Load(nCacheKey key, nCacheInfo * cell)
{
    this->nCacheEntry::Load(key, 0);

    this->cell = cell;

    nGMMMeshStorage * meshStorage = n_new(nGMMMeshStorage);
    n_assert(meshStorage);
    meshStorage->AddRef();
    meshStorage->SetupMesh(this->mesh);

    // from key to values, we need indexKey
    int bx, bz, indexKey;
    nGMMHoleIndicesCacheEntry::FromKey(key, bx, bz, indexKey);

    // fill mesh data
    nGMMIndexBuilder * indexBuilder = this->tgmmc->GetIndexBuilder();
    indexBuilder->SetupStorage(meshStorage);

    // parse the indexKey to proper lod values
    int lod = cell->GetGeometryLODLevel();
    int lodN, lodE, lodS, lodW;
    indexBuilder->ParseIndexGroup(lod, indexKey, lodN, lodE, lodS, lodW);

    meshStorage->BeginTerrainMesh();
    meshStorage->BeginTerrainGroup(0, indexBuilder->CountIndices( 0 ) );
    indexBuilder->FillIndicesHoledBlock( cell->GetCellX(), cell->GetCellZ(), lod, lodN, lodE, lodS, lodW );
    meshStorage->EndTerrainGroup();
    meshStorage->EndTerrainMesh();

    meshStorage->Release();

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool 
nGMMHoleIndicesCacheEntry::Unload()
{
    return this->nCacheEntry::Unload();
}

//------------------------------------------------------------------------------
nMesh2 *
nGMMHoleIndicesCacheEntry::GetMesh()
{
    return this->mesh;
}

//------------------------------------------------------------------------------
void 
nGMMHoleIndicesCacheEntry::Setup(ncTerrainGMMClass * cl)
{
    tgmmc = cl;
}

//------------------------------------------------------------------------------
nCacheKey 
nGMMHoleIndicesCacheEntry::ToKey(int bx, int bz, int indexKey)
{
    return (    indexKey * MAX_CACHE_KEY_ITEM_SIZE * MAX_CACHE_KEY_ITEM_SIZE +
                bz * MAX_CACHE_KEY_ITEM_SIZE +
                bx );
}

//------------------------------------------------------------------------------
void
nGMMHoleIndicesCacheEntry::FromKey(nCacheKey key, int & bx, int & bz, int & indexKey)
{
    nCacheKey keydiv = key / MAX_CACHE_KEY_ITEM_SIZE;
    bx = key % MAX_CACHE_KEY_ITEM_SIZE;
    indexKey = keydiv / MAX_CACHE_KEY_ITEM_SIZE;
    bz = keydiv % MAX_CACHE_KEY_ITEM_SIZE;
}

//------------------------------------------------------------------------------
