#ifndef N_GMMLODSYSTEM_H
#define N_GMMLODSYSTEM_H
//------------------------------------------------------------------------------
/**
    @class nGMMLODSystem
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Abstracts and groups different functions for LOD management.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngmmindexbuilder.h"
#include "ngeomipmap/ngmmvertexbuilder.h"
#include "ngeomipmap/ngmmindexcache.h"
#include "ngeomipmap/ngmmvertexlodsorting.h"

//------------------------------------------------------------------------------
class nGMMMeshCacheEntry;
typedef nCache<nGMMMeshCacheEntry> nGMMMeshCache;
class nGMMHoleIndicesCacheEntry;
typedef nCache<nGMMHoleIndicesCacheEntry> nGMMHoleIndicesCache;
class nGMMLowMeshCacheEntry;
typedef nCache<nGMMLowMeshCacheEntry> nGMMLowResMeshCache;

//------------------------------------------------------------------------------
class ncTerrainGMMClass;
class ncTerrainMaterialClass;
class nGMMVertexLODSorting;
class nGMMVertexBuilder;
class nGMMIndexBuilder;
class nGMMIndexCache;

//------------------------------------------------------------------------------
class nGMMLODSystem 
{
public:

    /// constructor
    nGMMLODSystem();
    /// destructor
    virtual ~nGMMLODSystem();

    /// Setup the LOD system before usage
    virtual void Setup(ncTerrainGMMClass *);

    /// get vertex sorting object
    nGMMVertexLODSorting * GetVertexSorting() const;
    /// get vertex builder object
    nGMMVertexBuilder * GetVertexBuilder() const;
    /// get index builder object
    nGMMIndexBuilder * GetIndexBuilder() const;
    /// get index cache object
    nGMMIndexCache * GetIndexCache() const;
    /// get mesh cache object
    nGMMMeshCache * GetMeshCache() const;
    /// get hole indices cache object
    nGMMHoleIndicesCache * GetHoleIndicesCache() const;
    /// get low resolution mesh cache object
    nGMMLowResMeshCache * GetLowResMeshCache() const;

    /// get low resolution mesh LOD level
    int GetLowResMeshLOD() const;

    /// select the geometry and material lod level for the specified cell
    int SelectCellLOD(ncTerrainGMMCell * cell, const nGeoMipMapViewport * vpSettings);
    /// process for leveling of LODs between cells
    virtual void LevelLOD(ncTerrainGMM *);

    // mesh cache
    // hole cache
    // render function

protected:

    /// Level LOD with max LOD difference of 1 for all cells
    void LevelLODMax1(ncTerrainGMM *);
    /// Level LOD with max LOD difference of 1 for 1 cell
    bool LevelLODMax1Cell(ncTerrainGMMCell * cell, int frameId);

    /// select the geometry LOD of the cell based on pixel error
    int SelectCellLODPixelError(ncTerrainGMMCell * cell, const nGeoMipMapViewport * vpSettings);
    /// select the geometry LOD of the cell based on block distance
    int SelectCellLODDistance(ncTerrainGMMCell * cell, const nGeoMipMapViewport * vpSettings);
        /// select the geometry LOD of the cell based on block distance (old way)
    int SelectCellLODDistanceOld(ncTerrainGMMCell * cell, const nGeoMipMapViewport * vpSettings);
    /// select the material lod level for the terrain cell
    bool SelectMaterialLOD(ncTerrainGMMCell * cell, const nGeoMipMapViewport * vpSettings);

    /// deallocate objects
    virtual void DeallocateObjects();

    /// create objects of the proper classes
    virtual nGMMVertexLODSorting * CreateVertexSorting();
    virtual nGMMVertexBuilder * CreateVertexBuilder();
    virtual nGMMIndexBuilder * CreateIndexBuilder();
    virtual nGMMIndexCache * CreateIndexCache();

    // terrain class
    ncTerrainGMMClass * tgmmc;
    // terrain material class
    ncTerrainMaterialClass * matc;
    // 1.0f / blockSizeScaled
    float bsscaledInv;
    // max difference of blocks (cached in SelectMaterialLOD for SelectCellLODDistance* methods)
    int blockMaxDiff;

    // vertex sorting object
    nGMMVertexLODSorting * vertexSorting;
    // vertex builder object
    nGMMVertexBuilder * vertexBuilder;
    // index builder object
    nGMMIndexBuilder * indexBuilder;
    // index cache
    nGMMIndexCache * indexCache;
    // mesh cache
    nGMMMeshCache * meshCache;
    // hole mesh cache (only index buffer)
    nGMMHoleIndicesCache * holeIndicesCache;
    // vertices cache for low resolution (in user memory)
    nGMMLowResMeshCache * lowResMeshCache;
    int lowResMeshLOD;

};

//------------------------------------------------------------------------------
inline
nGMMVertexLODSorting * 
nGMMLODSystem::GetVertexSorting() const
{
    return this->vertexSorting;
}

//------------------------------------------------------------------------------
inline
nGMMVertexBuilder * 
nGMMLODSystem::GetVertexBuilder() const
{
    return this->vertexBuilder;
}

//------------------------------------------------------------------------------
inline
nGMMIndexBuilder * 
nGMMLODSystem::GetIndexBuilder() const
{
    return this->indexBuilder;
}

//------------------------------------------------------------------------------
inline
nGMMIndexCache * 
nGMMLODSystem::GetIndexCache() const
{
    return this->indexCache;
}

//------------------------------------------------------------------------------
inline
nGMMMeshCache * 
nGMMLODSystem::GetMeshCache() const
{
    return this->meshCache;
}

//------------------------------------------------------------------------------
inline
nGMMHoleIndicesCache * 
nGMMLODSystem::GetHoleIndicesCache() const
{
    return this->holeIndicesCache;
}

//------------------------------------------------------------------------------
inline
nGMMLowResMeshCache * 
nGMMLODSystem::GetLowResMeshCache() const
{
    return this->lowResMeshCache;
}

//------------------------------------------------------------------------------
inline
int 
nGMMLODSystem::GetLowResMeshLOD() const
{
    return this->lowResMeshLOD;
}

//------------------------------------------------------------------------------
#endif // N_GMMLODSYSTEM_H
