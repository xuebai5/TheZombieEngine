//------------------------------------------------------------------------------
//  ngmmlodsystem.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmlodsystem.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ncterraingmm.h"
#include "napplication/napplication.h"

//------------------------------------------------------------------------------
/// the maximum number of entries in the mesh cache
static int nGMMMeshCacheMaxEntries = 128;
/// the maximum number of entries in the hole indices cache
static int nGMMHoleIndicesCacheMaxEntries = 32;

/**
    lods and aproximate numbers of vertices and triangles (M is num lod levels)

    M-1 -> 2x2 4 vert 2 tri (with skirts 4x4 16 vert)
    M-2 -> 3x3 9 vert 8 tri (with skirts 5x5 25 vert)
    M-3 -> 5x5 25 vert 32 tri (with skirts 7x7 49 vertices)
    M-4 -> 9x9 64 vert 128 tri (with skirts 11x11 121 vertices, 200 tri)
    M-5 -> 17x17 256 vert 512 tri (with skirts 19x19 361 vertices, 648 tri)

    the four lower levels of detail will be used through the low level cache

    about 3K max per entry, so in 6 MB of system memory -> more than 2048 entries
    This is for max sized terrain 2048x2048 with 32x32 block sizes
*/
static int nGMMLowResCacheLODLevels = 4;
/// maximum number of entries in the low resolution cache
static int nGMMLowResCacheMaxEntries = 2000;

//------------------------------------------------------------------------------
nGMMLODSystem::nGMMLODSystem() :
    tgmmc(0),
    matc(0),
    bsscaledInv(0.0f),
    blockMaxDiff(0),
    vertexSorting(0),
    vertexBuilder(0),
    indexBuilder(0),
    indexCache(0),
    meshCache(0),
    holeIndicesCache(0),
    lowResMeshCache(0),
    lowResMeshLOD(0)
{
}

//------------------------------------------------------------------------------
nGMMLODSystem::~nGMMLODSystem()
{
    this->DeallocateObjects();
}

//------------------------------------------------------------------------------
void 
nGMMLODSystem::Setup(ncTerrainGMMClass * tgmmc)
{
    n_assert(tgmmc);

    this->tgmmc = tgmmc;

    // get terrain material class
    this->matc = tgmmc->GetComponent<ncTerrainMaterialClass>();;
    n_assert(this->matc);

    // get terrain gmm class component i
    //this->bsscaledInv = 1.0f / matc->GetBlockSideSizeScaled();

    // create vertex sorting
    if (!this->vertexSorting)
    {
        //this->vertexSorting = n_new(nGMMVertexLODSorting);
        this->vertexSorting = this->CreateVertexSorting();
        n_assert(this->vertexSorting);
        this->vertexSorting->AddRef();
        this->vertexSorting->Setup(tgmmc);
    }

    // create vertex builder
    if (!this->vertexBuilder)
    {
        this->vertexBuilder = this->CreateVertexBuilder();
        this->vertexBuilder->SetupTerrain(tgmmc);
        this->vertexBuilder->SetupVertexSorting(this->vertexSorting);
        this->vertexBuilder->SetupStorage(0);
    }

    // create index builder
    if (!this->indexBuilder)
    {
        this->indexBuilder = this->CreateIndexBuilder();
        this->indexBuilder->SetupTerrain(tgmmc);
        this->indexBuilder->SetupVertexSorting(this->vertexSorting);
        this->indexBuilder->SetupStorage(0);
    }

    // create the index cache
    if (!this->indexCache)
    {
        this->indexCache = this->CreateIndexCache();
        this->indexCache->Setup(
            tgmmc->GetEntityClass()->nClass::GetName(), 
            tgmmc->GetNumLODLevels(), 
            this->indexBuilder);
    }

    // create the mesh cache
    if (!this->meshCache)
    {
        this->meshCache = n_new(nGMMMeshCache);
        nGMMMeshCacheEntry::Setup(tgmmc);
        /// setup the mesh cache size depending on the terrain size (1/4th blocks)
        int numBlocks = tgmmc->GetNumBlocks();
        numBlocks = min(nGMMMeshCacheMaxEntries, tgmmc->GetNumBlocks() * tgmmc->GetNumBlocks());
        this->meshCache->Alloc(numBlocks);
    }

    // create the hole indices cache
    if (!this->holeIndicesCache)
    {
        this->holeIndicesCache = n_new(nGMMHoleIndicesCache);
        nGMMHoleIndicesCacheEntry::Setup(this->tgmmc);
        this->holeIndicesCache->Alloc(nGMMHoleIndicesCacheMaxEntries);
    }

    // create the low resolution mesh cache
    if (!this->lowResMeshCache)
    {
        this->lowResMeshCache = n_new(nGMMLowResMeshCache);
        this->lowResMeshLOD = max(0, this->tgmmc->GetNumLODLevels() - nGMMLowResCacheLODLevels);
        nGMMLowMeshCacheEntry::Setup(this->tgmmc, this->lowResMeshLOD);
        int numBlocks = min(nGMMLowResCacheMaxEntries, this->tgmmc->GetNumBlocks() * this->tgmmc->GetNumBlocks());
        this->lowResMeshCache->Alloc(numBlocks);
    }
}

//------------------------------------------------------------------------------
void 
nGMMLODSystem::DeallocateObjects()
{
    // release low resolution mesh cache    
    if (this->lowResMeshCache->IsAllocated())
    {
        this->lowResMeshCache->Dealloc();
        n_delete(this->lowResMeshCache);
        this->lowResMeshCache = 0;
        nGMMLowMeshCacheEntry::Setup(0, 0);
    }

    // release hole cache
    if (this->holeIndicesCache->IsAllocated())
    {
        this->holeIndicesCache->Dealloc();
        n_delete(this->holeIndicesCache);
        this->holeIndicesCache = 0;
        nGMMHoleIndicesCacheEntry::Setup(0);
    }

    // release mesh cache
    if (this->meshCache && this->meshCache->IsAllocated())
    {
        this->meshCache->Dealloc();
        n_delete(this->meshCache);
        this->meshCache = 0;
        nGMMMeshCacheEntry::Setup(0);        
    }

    // release index cache
    if (this->indexCache)
    {
        n_delete(this->indexCache);
        this->indexCache = 0;
    }

    // release index builder
    if (this->indexBuilder)
    {
        n_delete(this->indexBuilder);
        this->indexBuilder = 0;
    }

    // release vertex builder
    if (this->vertexBuilder)
    {
        n_delete(this->vertexBuilder);
        this->vertexBuilder = 0;
    }

    // release vertex sorting
    if (this->vertexSorting)
    {
        this->vertexSorting->Release();
        this->vertexSorting = 0;
    }
}

//------------------------------------------------------------------------------
nGMMVertexLODSorting * 
nGMMLODSystem::CreateVertexSorting()
{
    return n_new(nGMMVertexLODSorting);
}

//------------------------------------------------------------------------------
nGMMVertexBuilder * 
nGMMLODSystem::CreateVertexBuilder()
{
    return n_new(nGMMVertexBuilder);
}

//------------------------------------------------------------------------------
nGMMIndexBuilder * 
nGMMLODSystem::CreateIndexBuilder()
{
    return 0;
}

//------------------------------------------------------------------------------
nGMMIndexCache * 
nGMMLODSystem::CreateIndexCache()
{
    return n_new(nGMMIndexCache);
}

//------------------------------------------------------------------------------
/**
    Does a LOD leveling process if needed. Some LOD system require that the
    difference of LOD between contiguous cells is <= 1.
*/
void
nGMMLODSystem::LevelLOD(ncTerrainGMM * tgmm)
{
    this->LevelLODMax1(tgmm);
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMLODSystem::LevelLODMax1(ncTerrainGMM * tgmm)
{
    int ntimes = 0;

    // initialize counters
    //this->blocksLeveled = 0;
    //this->lodsLeveled = 0;
    //this->trianglesLeveled = 0;

    int numBlocks = this->tgmmc->GetNumBlocks();

    // frameid
    int frameId = nApplication::Instance()->GetFrameId();
    
    // scan all the cells and level the LOD
    bool repeat = false;
    do 
    {
        repeat = false;
        ncTerrainGMMCell * firstCell = tgmm->GetNorthWestCell();
        for(int bz = 0;bz < numBlocks;bz++)
        {
            ncTerrainGMMCell * currentCell = firstCell;
            for(int bx = 0;bx < numBlocks;bx++)
            {
                n_assert(currentCell);
                if (currentCell->IsVisible(frameId) && this->LevelLODMax1Cell(currentCell, frameId))
                {
                    repeat = true;
                }
                currentCell = currentCell->GetNeighbor(ncTerrainGMMCell::East);
            }
            firstCell = firstCell->GetNeighbor(ncTerrainGMMCell::South);
        }
        ntimes++;
    } while (repeat);
}


//------------------------------------------------------------------------------
/**
    Selects the proper LOD level for the terrain block
*/
bool
nGMMLODSystem::LevelLODMax1Cell(ncTerrainGMMCell * cell, int frameId)
{
    // original lod
    int lod = cell->GetGeometryLODLevel();
    // leveled lod
    int llod = lod;

    if (cell->GetNeighbor(ncTerrainGMMCell::North) && cell->GetNeighbor(ncTerrainGMMCell::North)->IsVisible( frameId )) 
    {
        llod = n_min(llod, cell->GetNeighbor(ncTerrainGMMCell::North)->GetGeometryLODLevel());
    }
    if (cell->GetNeighbor(ncTerrainGMMCell::South) && cell->GetNeighbor(ncTerrainGMMCell::South)->IsVisible( frameId ))
    {
        llod = n_min(llod, cell->GetNeighbor(ncTerrainGMMCell::South)->GetGeometryLODLevel());
    }
    if (cell->GetNeighbor(ncTerrainGMMCell::West) && cell->GetNeighbor(ncTerrainGMMCell::West)->IsVisible( frameId ))
    {
        llod = n_min(llod, cell->GetNeighbor(ncTerrainGMMCell::West)->GetGeometryLODLevel());
    }
    if (cell->GetNeighbor(ncTerrainGMMCell::East) && cell->GetNeighbor(ncTerrainGMMCell::East)->IsVisible( frameId ))
    {
        llod = n_min(llod, cell->GetNeighbor(ncTerrainGMMCell::East)->GetGeometryLODLevel());
    }

    // if the lod and minimum neighbor lod is > 1 then level
    if ((lod - llod) > 1)
    {
        //this->blocksLeveled++;
        //this->lodsLeveled += (lod - llod - 1);
        //this->trianglesLeveled += power(2, 2 * (lod - llod)) * blockSize * blockSize - ;
        cell->SetGeometryLODLevel(llod + 1);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Selects the proper LOD level for the terrain block
*/
int 
nGMMLODSystem::SelectCellLOD(ncTerrainGMMCell * cell, const nGeoMipMapViewport * vpSettings)
{
    if( ! this->SelectMaterialLOD(cell, vpSettings) )
    {
        switch(this->tgmmc->GetLODSelectorMethod())
        {
        case ncTerrainGMMClass::PixelErrorLOD:
            return this->SelectCellLODPixelError(cell, vpSettings);
            break;
        case ncTerrainGMMClass::BlockDistance:
            return this->SelectCellLODDistance(cell, vpSettings);
            break;
        default:
            n_assert_always();
            break;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    Selects the proper LOD level for the terrain block
    Cells with holes are always rendered at full geometry LOD.
*/
int 
nGMMLODSystem::SelectCellLODPixelError(ncTerrainGMMCell * cell, const nGeoMipMapViewport * vpSettings)
{
    int geometryLOD = 0;
    if( !cell->HasHoles() )
    {
        // Get distance from camera to terrain block centre
        vector3 len = cell->GetCenter() - vpSettings->GetCameraPos();
        // L2 is the distance squared 
        float L2divC2 = len.lensquared() * vpSettings->GetC2Inv();

        // Iterate and select appropriate level, going backwards, probably more calculations
        geometryLOD = vpSettings->GetMaxLODLevel();
        const float * minDelta2 = cell->GetMinDelta2();
        while ( (minDelta2[geometryLOD] > L2divC2) &&
                (geometryLOD > vpSettings->GetMinLODLevel()) )
        {
            geometryLOD--;
        }
    }

    cell->SetGeometryLODLevel(geometryLOD);

    return geometryLOD;
}

//------------------------------------------------------------------------------
/**
    Selects the proper geometry LOD level for the terrain block based on a
    set of parameters entered by the user (LOD distances).
*/
int 
nGMMLODSystem::SelectCellLODDistance(ncTerrainGMMCell * cell, const nGeoMipMapViewport * /*vpSettings*/)
{
    int geometryLOD = 0;
    if( !cell->HasHoles() )
    {
        int numDistances = this->tgmmc->GetNumGeometryLODDistances();
        const int * lodDistances = this->tgmmc->GetGeometryLODDistance();
        int lod = 0;
        while(lod < numDistances && this->blockMaxDiff >= lodDistances[lod])
        {
            this->blockMaxDiff -= lodDistances[lod++];
        }
        geometryLOD = min(lod, this->tgmmc->GetNumLODLevels() - 1);
    }

    cell->SetGeometryLODLevel(geometryLOD);

    return geometryLOD;
}

//------------------------------------------------------------------------------
/**
    Select geometry LOD level based on count of blocks, in relation to material    
    LOD distance.
*/
int 
nGMMLODSystem::SelectCellLODDistanceOld(ncTerrainGMMCell * /*cell*/, const nGeoMipMapViewport * /*vpSettings*/)
{
    int geometryLOD = 0;

    // number of distance
    int lod = (this->blockMaxDiff / (this->matc->GetMaterialLODDistance() + 1));
    geometryLOD = min(lod, this->tgmmc->GetNumLODLevels() - 1);

    return geometryLOD;
}

//------------------------------------------------------------------------------
/**
    Select the material LOD level based on the material lod distance parameter
    provided in the ncTerrainMaterialClass. There are just two materials:
    0 which is High Quality (HQ), and 1 which is Low Quality (LQ).
    If HQ material is selected, then geometry LOD is set to zero.

    Note: blockMaxDiff is the max difference of blocks (cached here
    to be used later in SelectCellLODDistance* methods)
*/
bool
nGMMLODSystem::SelectMaterialLOD(ncTerrainGMMCell * cell, const nGeoMipMapViewport * vpSettings)
{
    n_assert(cell);
    n_assert(vpSettings);

    // get camera block position
    const int * cameraBlockPos  = vpSettings->GetCameraBlockPos();
    this->blockMaxDiff = max( 
        abs( cell->GetCellX() - cameraBlockPos[0] ), 
        abs( cell->GetCellZ() - cameraBlockPos[2] ) );

    // calculate block y for the current cell (do LOD selection on y too)
    //int by = n_fchop( cell->GetBBox().center.y * bsscaledInv );
    //this->blockMaxDiff = max(abs(by - cameraBlockPos[1]), this->blockMaxDiff);

    // calculate material lod
    if (this->blockMaxDiff <= this->matc->GetMaterialLODDistance())
    {
        cell->SetMaterialLODLevel(0);
        // force geometry lod level to HQ too when material is HQ
        cell->SetGeometryLODLevel(0);
        return true;
    }

    cell->SetMaterialLODLevel(1);
    return false;
}

//------------------------------------------------------------------------------
