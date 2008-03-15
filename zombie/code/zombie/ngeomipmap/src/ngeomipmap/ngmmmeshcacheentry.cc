//------------------------------------------------------------------------------
//  ngmmmeshcacheentry.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmmeshcacheentry.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ngmmvertexbuilder.h"
#include "ngeomipmap/ngmmmeshstorage.h"

//---------------------------------------------------------------------------
/**
    The cache key has 3 elements bx, bz and index key. Each one must not be
    larger than following value MAX_CACHE_KEY_ITEM_SIZE.
*/
const int MAX_CACHE_KEY_ITEM_SIZE = (1 << 10);

//---------------------------------------------------------------------------
ncTerrainGMMClass * nGMMMeshCacheEntry::tgmmc = 0;

//---------------------------------------------------------------------------
/**
*/
nGMMMeshCacheEntry::nGMMMeshCacheEntry() :
    cell(0)
{
    /// empty
}

//---------------------------------------------------------------------------
/**
*/
nGMMMeshCacheEntry::~nGMMMeshCacheEntry()
{
    this->Unload();
    this->Dealloc();
}

//---------------------------------------------------------------------------
/**
    Allocate buffers for the geometry of a terrain cell.
*/
bool
nGMMMeshCacheEntry::Alloc()
{
    NLOG(terrain, (nTerrainLog::NLOG_VERTEXCACHE | 1, "nGMMMeshCacheEntry::Alloc 0x%x", this));

    this->nCacheEntry::Alloc();

    nGMMVertexBuilder * vertexBuilder = this->tgmmc->GetVertexBuilder();
    n_assert(vertexBuilder);

    nString resName;
    nGeoMipMapResourceLoader::GenerateResourceLoaderString( 
        resName, nGMMMeshCacheEntry::tgmmc->GetEntityClass(), 0, 0, 0,
        vertexBuilder->CountVertices(0), 0 );

    this->cell = 0;
    this->mesh = nGfxServer2::Instance()->NewMesh(0);
    n_assert(this->mesh);
    nString resloader = nGeoMipMapResourceLoader::Instance()->GetFullName();
    this->mesh->SetResourceLoader(resloader.Get());
    this->mesh->SetFilename(resName);
    return this->mesh->Load();
}

//---------------------------------------------------------------------------
/**
*/
bool 
nGMMMeshCacheEntry::Dealloc()
{
    NLOG(terrain, (nTerrainLog::NLOG_VERTEXCACHE | 1, "nGMMMeshCacheEntry::Dealloc 0x%x", this));

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
nGMMMeshCacheEntry::Load(nCacheKey key, nCacheInfo * cell)
{
    NLOG(terrain, (nTerrainLog::NLOG_VERTEXCACHE | 0, "nGMMMeshCacheEntry::Load key=0x%x", key));

    this->nCacheEntry::Load(key, 0);

    this->cell = cell;

    nGMMMeshStorage * meshStorage = n_new(nGMMMeshStorage);
    n_assert(meshStorage);
    meshStorage->AddRef();
    meshStorage->SetupMesh(this->mesh);

    // fill mesh data
    nGMMVertexBuilder * vertexBuilder = this->tgmmc->GetVertexBuilder();
    vertexBuilder->SetupStorage(meshStorage);

    meshStorage->BeginTerrainMesh();
    meshStorage->BeginTerrainGroup( vertexBuilder->CountVertices( 0 ), 0 );
    vertexBuilder->FillVerticesBlock( cell->GetCellX(), cell->GetCellZ(), 0 );
    meshStorage->EndTerrainGroup();
    meshStorage->EndTerrainMesh();

    meshStorage->Release();

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool 
nGMMMeshCacheEntry::Unload()
{
    NLOG(terrain, (nTerrainLog::NLOG_VERTEXCACHE | 0, "nGMMMeshCacheEntry::Unload key=0x%x", key));

    return this->nCacheEntry::Unload();
}

//------------------------------------------------------------------------------
nMesh2 *
nGMMMeshCacheEntry::GetMesh()
{
    return this->mesh;
}

//------------------------------------------------------------------------------
void 
nGMMMeshCacheEntry::Setup(ncTerrainGMMClass * cl)
{
    tgmmc = cl;
}

//------------------------------------------------------------------------------
nCacheKey 
nGMMMeshCacheEntry::ToKey(int bx, int bz)
{
    return ( bz * MAX_CACHE_KEY_ITEM_SIZE + bx );
}

//------------------------------------------------------------------------------
void
nGMMMeshCacheEntry::FromKey(nCacheKey key, int & bx, int & bz)
{
    bz = key / MAX_CACHE_KEY_ITEM_SIZE;
    bx = key % MAX_CACHE_KEY_ITEM_SIZE;
}

//------------------------------------------------------------------------------
