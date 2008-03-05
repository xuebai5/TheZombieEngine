/*-----------------------------------------------------------------------------
    @file ngmmlowmeshcacheentry.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    @brief Cache for low level of detail terrain vertices data

    (C) 2006 Conjurer Services, S.A.
*/
//---------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmlowmeshcacheentry.h"
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
ncTerrainGMMClass * nGMMLowMeshCacheEntry::tgmmc = 0;
nRef<nGMMGeometryStorage> nGMMLowMeshCacheEntry::storage;
int nGMMLowMeshCacheEntry::lod = 0;
int nGMMLowMeshCacheEntry::maxVertices = 0;

//---------------------------------------------------------------------------
/**
*/
nGMMLowMeshCacheEntry::nGMMLowMeshCacheEntry() :
    vertices(0),
    cell(0)
{
    /// empty
}

//---------------------------------------------------------------------------
/**
*/
nGMMLowMeshCacheEntry::~nGMMLowMeshCacheEntry()
{
    this->Unload();
    this->Dealloc();
}

//---------------------------------------------------------------------------
/**
    Allocate buffers for the geometry of a terrain cell.
*/
bool
nGMMLowMeshCacheEntry::Alloc()
{
    n_assert(!this->vertices);

    this->nCacheEntry::Alloc();

    nGMMVertexBuilder * vertexBuilder = this->tgmmc->GetVertexBuilder();
    n_assert(vertexBuilder);

    this->cell = 0;
    // vertex position + normal, 3 floats each
    int size = this->GetMaxVertices() * 3 * 2;
    this->vertices = n_new_array(float,size);
    n_assert_return(this->vertices, false);

    if( !nGMMLowMeshCacheEntry::storage.isvalid() )
    {
        // Create a mesh storage
        nGMMLowMeshCacheEntry::storage = n_new(nGMMGeometryStorage);
        n_assert_return(storage, false);
        nGMMLowMeshCacheEntry::storage->SetFillNormals( true );
    }
    nGMMLowMeshCacheEntry::storage->AddRef();

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool 
nGMMLowMeshCacheEntry::Dealloc()
{
    this->nCacheEntry::Dealloc();

    // release GMM geometry storage object
    if (nGMMLowMeshCacheEntry::storage.isvalid())
    {
        nGMMLowMeshCacheEntry::storage->Release();
    }

    // release the vertex buffer in system memory
    if (this->vertices)
    {
        n_delete_array( this->vertices );
        this->vertices = 0;
    }

    this->cell = 0;

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool 
nGMMLowMeshCacheEntry::Load(nCacheKey key, nCacheInfo * cell)
{
    this->nCacheEntry::Load(key, 0);

    this->cell = cell;

    // fill mesh data
    nGMMVertexBuilder * vertexBuilder = nGMMLowMeshCacheEntry::tgmmc->GetVertexBuilder();
    vertexBuilder->SetupStorage(this->storage);

    nGMMLowMeshCacheEntry::storage->SetupVertexBuffer( this->vertices, nGMMLowMeshCacheEntry::GetMaxVertices() );
    nGMMLowMeshCacheEntry::storage->SetupIndexBuffer( 0, 0 );
    nGMMLowMeshCacheEntry::storage->BeginTerrainMesh();
    nGMMLowMeshCacheEntry::storage->BeginTerrainGroup( nGMMLowMeshCacheEntry::GetMaxVertices(), 0 );
    vertexBuilder->FillVerticesBlock( cell->GetCellX(), cell->GetCellZ(), nGMMLowMeshCacheEntry::GetMaxLOD() );
    nGMMLowMeshCacheEntry::storage->EndTerrainGroup();
    nGMMLowMeshCacheEntry::storage->EndTerrainMesh();

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool 
nGMMLowMeshCacheEntry::Unload()
{
    return this->nCacheEntry::Unload();
}

//------------------------------------------------------------------------------
float * 
nGMMLowMeshCacheEntry::GetVertices() const
{
    return this->vertices;
}

//------------------------------------------------------------------------------
void 
nGMMLowMeshCacheEntry::Setup(ncTerrainGMMClass * cl, int lod)
{
    n_assert_return(lod >= 0,);

    if (cl)
    {
        nGMMVertexBuilder * vertexBuilder =  cl->GetVertexBuilder();
        n_assert_return(vertexBuilder,);
        nGMMLowMeshCacheEntry::maxVertices = vertexBuilder->CountVertices( lod );
        n_assert(nGMMLowMeshCacheEntry::maxVertices > 0);
    }
    else
    {
        nGMMLowMeshCacheEntry::maxVertices = 0;
    }

    nGMMLowMeshCacheEntry::tgmmc = cl;
    nGMMLowMeshCacheEntry::lod = lod;

}

//------------------------------------------------------------------------------
int 
nGMMLowMeshCacheEntry::GetMaxVertices()
{
    return nGMMLowMeshCacheEntry::maxVertices;
}

//------------------------------------------------------------------------------
int 
nGMMLowMeshCacheEntry::GetMaxLOD()
{
    return nGMMLowMeshCacheEntry::lod;
}

//------------------------------------------------------------------------------
nCacheKey 
nGMMLowMeshCacheEntry::ToKey(int bx, int bz)
{
    return ( bz * MAX_CACHE_KEY_ITEM_SIZE + bx );
}

//------------------------------------------------------------------------------
void
nGMMLowMeshCacheEntry::FromKey(nCacheKey key, int & bx, int & bz)
{
    bz = key / MAX_CACHE_KEY_ITEM_SIZE;
    bx = key % MAX_CACHE_KEY_ITEM_SIZE;
}

//------------------------------------------------------------------------------
