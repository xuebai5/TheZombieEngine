//------------------------------------------------------------------------------
//  nGMMIndexCache.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmindexcache.h"
#include "ngeomipmap/ngmmindexbuilder.h"
#include "ngeomipmap/ngmmindexresloader.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
/**
*/
nGMMIndexCache::nGMMIndexCache() :
    numLODLevels(0)
{
    for(int i = 0;i < MaxLODLevels;i++)
    {
        this->indices[i] = 0;
        this->groups[i] = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
nGMMIndexCache::~nGMMIndexCache()
{
    this->DeallocateResources();
    this->DeallocateBuffers();
}

//------------------------------------------------------------------------------
void 
nGMMIndexCache::Setup(const char * terrainClassName, int numLODLevels, nGMMIndexBuilder * indexBuilder)
{
    n_assert(terrainClassName);
    n_assert(numLODLevels > 0 && numLODLevels < MaxLODLevels);

    this->terrainClassName = terrainClassName;
    this->numLODLevels = numLODLevels;
    this->indexBuilder = indexBuilder;

    this->AllocateResources();
    this->AllocateBuffers();
}

//------------------------------------------------------------------------------
/**
*/
nMesh2 * 
nGMMIndexCache::GetIndexMesh(int lod, 
	int N_IFDEF_ASSERTS(lodN), 
	int N_IFDEF_ASSERTS(lodE), 
	int N_IFDEF_ASSERTS(lodS), 
	int N_IFDEF_ASSERTS(lodW) )
{
    n_assert(lod >= 0 && lod <= this->numLODLevels);
    n_assert(lodN >= 0 && lod <= this->numLODLevels);
    n_assert(lodE >= 0 && lod <= this->numLODLevels);
    n_assert(lodS >= 0 && lod <= this->numLODLevels);
    n_assert(lodW >= 0 && lod <= this->numLODLevels);
    n_assert(this->mesh[lod].isvalid());

    return this->mesh[lod];
}

//------------------------------------------------------------------------------
void 
nGMMIndexCache::AllocateResources()
{
    for(int i = 0;i < this->numLODLevels;i++)
    {
        // create the mesh resource
        nString resname;
        resname = nGMMIndexResLoader::GenerateResourceString(this->terrainClassName.Get(), i);
        nMesh2 * mesh = nGfxServer2::Instance()->NewMesh(resname.Get());
        mesh->SetResourceLoader(nGMMIndexResLoader::Instance()->GetFullName().Get());
        mesh->SetFilename(resname.Get());
        mesh->Load();

        // assign the mesh
        this->mesh[i] = mesh;
    }
}

//------------------------------------------------------------------------------
void 
nGMMIndexCache::DeallocateResources()
{
    for(int i = 0;i < MaxLODLevels;i++)
    {
        if (this->mesh[i].isvalid())
        {
            this->mesh[i]->Release();
            this->mesh[i].invalidate();
        }
    }
}

//------------------------------------------------------------------------------
void 
nGMMIndexCache::AllocateBuffers()
{
    nGMMGeometryStorage * geometryStorage = n_new(nGMMGeometryStorage);
    this->indexBuilder->SetupStorage(geometryStorage);
    geometryStorage->BeginTerrainMesh();

    for(int i = 0;i < this->numLODLevels;i++)
    {
        int firstIndex = 0;

        n_assert(!this->indices[i]);
        int numIndices = this->indexBuilder->CountIndices(i);
        int numGroups = this->indexBuilder->CountGroups(i);

        this->indices[i] = static_cast<nuint16 *> ( n_malloc(numIndices * numGroups * sizeof(nuint16)) );
        n_assert(this->indices[i]);

        this->groups[i] = static_cast<nIndicesInfo *> ( n_malloc(numGroups * sizeof(nIndicesInfo)) );
        n_assert(this->groups[i]);

        geometryStorage->SetupVertexBuffer(0, 0);
        geometryStorage->SetupIndexBuffer(this->indices[i], numIndices * numGroups);

        for(int g = 0;g < numGroups;g++)
        {
            int lodN, lodE, lodS, lodW;
            this->indexBuilder->ParseIndexGroup(i, g, lodN, lodE, lodS, lodW);
            geometryStorage->BeginTerrainGroup(0, numIndices);
            this->indexBuilder->FillIndicesBlock(i, lodN, lodE, lodS, lodW);
            this->groups[i][g].firstIndex = firstIndex;
            this->groups[i][g].numIndices = geometryStorage->GetNumIndices() - firstIndex;
            n_assert(this->groups[i][g].numIndices > 0 && this->groups[i][g].numIndices <= numIndices);
            firstIndex += this->groups[i][g].numIndices;
            geometryStorage->EndTerrainGroup();
        }
    }
    
    geometryStorage->EndTerrainMesh();
    this->indexBuilder->SetupStorage(0);
    //n_delete(geometryStorage);
}

//------------------------------------------------------------------------------
void 
nGMMIndexCache::DeallocateBuffers()
{
    for(int i = 0;i < MaxLODLevels;i++)
    {
        if (this->indices[i])
        {
            n_free(this->indices[i]);
            this->indices[i] = 0;
        }
        if (this->groups[i])
        {
            n_free(this->groups[i]);
            this->groups[i] = 0;
        }
    }
}

//------------------------------------------------------------------------------
nuint16 * 
nGMMIndexCache::GetIndexBuffer(int lod, int lodN, int lodE, int lodS, int lodW, int & numIndices)
{   
    n_assert(lod >= 0 && lod < this->numLODLevels);
    int group = this->indexBuilder->CalcIndexGroup(lod, lodN, lodE, lodS, lodW);
    numIndices = this->groups[lod][group].numIndices;
    return this->indices[lod] + this->groups[lod][group].firstIndex;
}
