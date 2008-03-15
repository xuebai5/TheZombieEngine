//------------------------------------------------------------------------------
//  ncterraingmmclass_main.cc
//  (c) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "zombieentity/ncloaderclass.h"
#include "kernel/nlogclass.h"
#include "resource/nresourceserver.h"
#include "ngeomipmap/ngmmvertexlodsorting.h"
#include "ngeomipmap/ngmmvertexlodsorting.h"
#include "ngeomipmap/ngmmvertexlodnsorting.h"
#include "ngeomipmap/ngmmindexlod1builder.h"
#include "ngeomipmap/ngmmindexlodnbuilder.h"
#include "ngeomipmap/ngmmvertexbuilder.h"
#include "ngeomipmap/ngmmindexcache.h"
#include "ngeomipmap/ngmmlodsystem.h"
//#include "ngeomipmap/ngmmlod1system.h"
//#include "ngeomipmap/ngmmlodnsystem.h"
#include "ngeomipmap/ngmmskirtlodsystem.h"
#include "ngeomipmap/ngmmmeshcacheentry.h"
#include "ngeomipmap/ncache.h"
#include "ngeomipmap/ngmmholeindicescacheentry.h"
#include "ngeomipmap/ngmmholeindicescacheentry.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncTerrainGMMClass,nComponentClass);

static const char * groupNames[] = {
    "Render",
    "Vertex build",
    "Index build",
    "Streaming",
    "Weightmap cache",
    "Lightmap cache",
    0
};

//------------------------------------------------------------------------------
NCREATELOGLEVELGROUP(terrain, "Terrain", false, 3, groupNames, NLOG_GROUP_MASK);
// log levels defined
// 0 - info on initialization, setup, finalization
// 1 - statistics summary & info updated per frame
// 2 - info updated per frame, per terrain block
// 3 - verbose log

//------------------------------------------------------------------------------
static const int defaultBlockSize = 17;

//------------------------------------------------------------------------------
ncTerrainGMMClass::ncTerrainGMMClass() :
    blockSize(defaultBlockSize),
    numBlocks(0),
    numLODLevels(0),
    error(5.0f),
    lodSelMethod(PixelErrorLOD),
    currentLODSystem(0)
{
    for(int i = 0;i < N_ARRAY_SIZEOF(this->geometryLODDistance);i++)
    {
        this->geometryLODDistance[i] = 1;
    }    
}

//------------------------------------------------------------------------------
ncTerrainGMMClass::~ncTerrainGMMClass()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    instance initialization
*/
void
ncTerrainGMMClass::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    /// empty
}

//------------------------------------------------------------------------------
void
ncTerrainGMMClass::CalcTerrainParams()
{
    this->numBlocks = (this->refHeightMap->GetSize() - 1) / (this->blockSize - 1);
    this->numLODLevels = static_cast<int> (logf(static_cast<float> (this->blockSize - 1)) / logf(2.0f) + 0.5f) + 1;
    this->depth = static_cast<int> (logf(static_cast<float>(this->numBlocks)) / logf(2.0f) + 0.5f);
}

//------------------------------------------------------------------------------
/**
    load the resources of the terrain
*/
bool
ncTerrainGMMClass::LoadResources()
{
    // create resource if not exists
    if (!this->refHeightMap.isvalid())
    {
        nString filename = this->GetHeightMapPath();
        this->refHeightMap = static_cast<nFloatMap *> (
            nResourceServer::Instance()->NewResource(
                "nfloatmap",  filename.Get(), nResource::Other) );
        n_assert(this->refHeightMap.isvalid());
        this->refHeightMap->SetFilename(filename.Get());
    }

    // load the weightmap
    if (!this->refHeightMap->Load())
    {
        n_printf("ncTerrainGMMClass: Error loading heightmap '%s'\n", this->GetHeightMapPath().Get());
        return false;
    }

    // calculate params
    this->CalcTerrainParams();

    this->AllocateLODSystems();

    return true;
}

//------------------------------------------------------------------------------
/**
    load the resources of the terrain
*/
void
ncTerrainGMMClass::UnloadResources()
{
    if (this->refHeightMap.isvalid())
    {
        this->refHeightMap->Unload();
        this->refHeightMap.invalidate();
    }

    this->DeallocateLODSystems();
}

//---------------------------------------------------------------------------
bool 
ncTerrainGMMClass::IsValid()
{
    return this->refHeightMap.isvalid() && this->refHeightMap->IsLoaded();
}

//---------------------------------------------------------------------------
/**
    @return the Nebula hierarchy path to the heightMap node.
*/
nString
ncTerrainGMMClass::GetHeightMapPath() const
{
    const ncLoaderClass * loader = this->GetComponent<ncLoaderClass>();
    n_assert(loader);

    nString filename = loader->GetResourceFile();
    filename.StripExtension();
    filename.Append("/terrain/heightmap.t16");
    return filename;
}

//---------------------------------------------------------------------------
/**
*/
bool
ncTerrainGMMClass::SaveTerrainGMM() const
{
    // save heightmap
    if (this->refHeightMap->IsDirty()) 
    {
        return this->refHeightMap->Save();
    }

    return true;
}

//---------------------------------------------------------------------------
/**
    @return the minimum allowed height for the terrain
*/
float 
ncTerrainGMMClass::GetMinHeight() const
{
    if (this->refHeightMap.isvalid())
    {
        return this->refHeightMap->GetHeightOffset();
    }
    else
    {
        return 0;
    }
}

//---------------------------------------------------------------------------
/**
    @return the maximum allowed height for the terrain
*/
float 
ncTerrainGMMClass::GetMaxHeight() const
{
    if (this->refHeightMap.isvalid())
    {
        return this->refHeightMap->GetHeightOffset() + this->refHeightMap->GetHeightScale();
    }
    else
    {
        return 0;
    }
}

//---------------------------------------------------------------------------
/**
    @return the heightmap size
*/
int 
ncTerrainGMMClass::GetHeightMapSize() const
{
    if (this->refHeightMap.isvalid())
    {
        return this->refHeightMap->GetSize();
    }
    else
    {
        return 0;
    }
}

//---------------------------------------------------------------------------
/**
    @return the distance between consecutive points in the heightmap
*/
float 
ncTerrainGMMClass::GetPointSeparation() const
{
    if (this->refHeightMap.isvalid())
    {
        return this->refHeightMap->GetGridScale();
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
/**
    Selects the LOD selection technique
*/
void
ncTerrainGMMClass::SetLODSelectorMethod(ncTerrainGMMClass::LODSelectionMethod lsm)
{
    this->lodSelMethod = lsm;
}

//---------------------------------------------------------------------------
/**
    Get LOD selection technique
*/
ncTerrainGMMClass::LODSelectionMethod
ncTerrainGMMClass::GetLODSelectorMethod() const
{
    return this->lodSelMethod;
}

//---------------------------------------------------------------------------
/**
    Convert from world coordinates to cell coordinates
    This can be used for both x and z coordinates.
*/
int
ncTerrainGMMClass::GlobalCoordToBlockCoord(float x) const
{
    // The terrain size is (0,0) to (size, size)  and size > 0
    int valX = static_cast<int> ( n_floor ( ( x / this->GetTotalSideSizeScaled() ) * this->GetNumBlocks() ) ) ;
    return n_max( 0 , n_min (this->GetNumBlocks() - 1, valX) );
}

//---------------------------------------------------------------------------
/**
    Convert from hexel coordinates to cell coordinates
    This can be used for both x and z coordinates.
*/
int
ncTerrainGMMClass::HexelCoordToBlockCoord(int bx) const
{
    // The terrain size is (0,0) to (blocksize-1, blocksize-1) and size > 0
    int valX = bx / (this->GetBlockSize() - 1);
    return n_max( 0 , n_min(this->GetNumBlocks() - 1, valX) );
}

//---------------------------------------------------------------------------
/**
*/
float
ncTerrainGMMClass::GetTotalSideSizeScaled() const
{
    return (this->GetHeightMap()->GetSize() - 1) * this->GetHeightMap()->GetGridScale();
}

//---------------------------------------------------------------------------
/**
*/
float
ncTerrainGMMClass::GetBlockSideSizeScaled() const
{
    return (this->GetBlockSize() - 1) * this->GetHeightMap()->GetGridScale();
}

//---------------------------------------------------------------------------
/**
*/
nGMMVertexBuilder * 
ncTerrainGMMClass::GetVertexBuilder() const
{
    n_assert(this->currentLODSystem->GetVertexBuilder());
    return this->currentLODSystem->GetVertexBuilder();
}

//---------------------------------------------------------------------------
/**
*/
nGMMVertexLODSorting * 
ncTerrainGMMClass::GetVertexSorting() const
{
    n_assert(this->currentLODSystem->GetVertexSorting());
    return this->currentLODSystem->GetVertexSorting();
}

//---------------------------------------------------------------------------
/**
*/
nGMMIndexBuilder * 
ncTerrainGMMClass::GetIndexBuilder() const
{
    n_assert(this->currentLODSystem->GetIndexBuilder());
    return this->currentLODSystem->GetIndexBuilder();
}

//---------------------------------------------------------------------------
/**
*/
nGMMIndexCache * 
ncTerrainGMMClass::GetIndexCache() const
{
    n_assert(this->currentLODSystem->GetIndexCache());
    return this->currentLODSystem->GetIndexCache();
}


//---------------------------------------------------------------------------
nGMMMeshCache * 
ncTerrainGMMClass::GetMeshCache() const
{
    return this->currentLODSystem->GetMeshCache();
}

//---------------------------------------------------------------------------
nGMMHoleIndicesCache * 
ncTerrainGMMClass::GetHoleIndicesCache() const
{
    return this->currentLODSystem->GetHoleIndicesCache();
}

//---------------------------------------------------------------------------
nGMMLowResMeshCache * 
ncTerrainGMMClass::GetLowResMeshCache() const
{
    return this->currentLODSystem->GetLowResMeshCache();
}

//---------------------------------------------------------------------------
bool 
ncTerrainGMMClass::IsLowResMeshLOD(int lod) const
{
    return ( lod >= this->currentLODSystem->GetLowResMeshLOD() );
}

//---------------------------------------------------------------------------
void 
ncTerrainGMMClass::ResetStats()
{
    this->GetMeshCache()->ResetStats();
    this->GetHoleIndicesCache()->ResetStats();
}

//---------------------------------------------------------------------------
/**
*/
void
ncTerrainGMMClass::AllocateLODSystems()
{
    if (!this->currentLODSystem)
    {
        this->currentLODSystem = n_new(nGMMSkirtLODSystem);
        //this->currentLODSystem = n_new(nGMMLOD1System);
        //this->currentLODSystem = n_new(nGMMLODNSystem);
        n_assert(this->currentLODSystem);
        this->currentLODSystem->Setup(this);
    }
}

//---------------------------------------------------------------------------
/**
*/
void
ncTerrainGMMClass::DeallocateLODSystems()
{
    if (this->currentLODSystem)
    {
        n_delete(this->currentLODSystem);
        this->currentLODSystem = 0;
    }
}

//---------------------------------------------------------------------------
/**
    set the geometry lod distances, each number specifies the number of blocks
    which will use that lod level. For example, 
    - l0 is the number of blocks which will use the lod level 0 (maximum detail)
    - l1 is the number of blocks which will use the lod level 1
    ...
    Limitation: at least there must one block per each level.
*/
void
ncTerrainGMMClass::SetGeometryLODDistance(int l0, int l1, int l2, int l3, int l4, int l5)
{
    this->geometryLODDistance[0] = n_max(l0, 1);
    this->geometryLODDistance[1] = n_max(l1, 1);
    this->geometryLODDistance[2] = n_max(l2, 1);
    this->geometryLODDistance[3] = n_max(l3, 1);
    this->geometryLODDistance[4] = n_max(l4, 1);
    this->geometryLODDistance[5] = n_max(l5, 1);
}

//---------------------------------------------------------------------------
/**
    Return the geometry lod distances by reference.
    @see SetGeometryLODDistance for details
*/
void
ncTerrainGMMClass::GetGeometryLODDistance(int & l0, int & l1, int & l2, int & l3, int & l4, int & l5) const
{
    l0 = this->geometryLODDistance[0];
    l1 = this->geometryLODDistance[1];
    l2 = this->geometryLODDistance[2];
    l3 = this->geometryLODDistance[3];
    l4 = this->geometryLODDistance[4];
    l5 = this->geometryLODDistance[5];
}

//---------------------------------------------------------------------------
/**
    Return the geometry lod distances as an array of values.
    @see SetGeometryLODDistance for details
*/
const int *
ncTerrainGMMClass::GetGeometryLODDistance() const
{
    return this->geometryLODDistance;
}

//---------------------------------------------------------------------------
/**
    Get the number of items returned by the array GetGeometryLODDistance.
*/
int 
ncTerrainGMMClass::GetNumGeometryLODDistances() const
{
    return n_min(6, this->numLODLevels);
}
