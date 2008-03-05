#ifndef N_NCTERRAINGMMCLASS_H
#define N_NCTERRAINGMMCLASS_H
//------------------------------------------------------------------------------
/**
    @file ncterraingmmclass.h
    @class ncTerrainGMMClass
    @ingroup NebulaTerrain
    @author Mateu Batle Sastre
    @brief Class component class for terrain graphics using geomipmap algorithm.    

    ncGfxTerrainGMMClass contains information about one a "class" of the terrain.
    The algorithm used for rendering is geometrical mipmapping by Willem H.
    de Boer, article "Fast Terrain Rendering Using Geometrical MipMapping".
    http://www.flipcode.com/tutorials/geomipmaps.pdf 

    The idea is quite simple, apply the same technique used for texture LOD
    (mipmapping) to terrain geometry. The terrain is partitioned in tiles
    of fixed size, e.g. 17x17. Every level of LOD is applied to each tile 
    by removing every other row and column, so each new LOD is exactly 1/4th 
    the size of the previous LOD.

    The terrain heightmaps must be square (size power of two plus one), e.g. 
    1025x1025.

    See also @ref N2ScriptInterface_ngeomipmap
    @todo Things to do in the whole terrain package
    - Mechanism to update rendering buffers when heightMap changes
    - special test mesh to check LOD selection with different errors per block ...
    - lod by distance not error, polygon budget
    - geomorphing

    Optimizations:
    - pool of vertex buffers to avoid refill every frame
    - test just one vertex buffer per block with vertices grouped per lod

    Holes in terrains. Basically two ways:

    1) Don't generate the terrain geometry. An special value in the
    heightMap could be reserved to mark the vertices that should not
    be rendered. This would slow down the process of triangle 
    generation in the terrain mesh builder.

    2) Use a polygon to mark the areas where there is a hole. This 
    polygons will affect:

    - Rendering. Render the polygon in the stencil buffer. Issues with 
    ordering of passes.
    - Collision. If these polygons belong to some special class it could be 
    handled more or less easily.
    - AI (pathfinding). 

    This technique allows more precision in the cut of the terrain.

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "util/nstring.h"
#include "kernel/nref.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nfloatmap.h"
#include "ngeomipmap/ngeomipmap.h"
//#include "ngeomipmap/ngmmmeshcacheentry.h"
#include "ngeomipmap/ncache.h"

//------------------------------------------------------------------------------
class nGMMMeshCacheEntry;
typedef nCache<nGMMMeshCacheEntry> nGMMMeshCache;
class nGMMHoleIndicesCacheEntry;
typedef nCache<nGMMHoleIndicesCacheEntry> nGMMHoleIndicesCache;
class nGMMLowMeshCacheEntry;
typedef nCache<nGMMLowMeshCacheEntry> nGMMLowResMeshCache;

//------------------------------------------------------------------------------
class nFloatMap;
class nGMMVertexBuilder;
class nGMMVertexLODSorting;
class nGMMIndexBuilder;
class nGMMIndexCache;
class nGMMLODSystem;

//------------------------------------------------------------------------------
namespace nTerrainLog
{
    const int NLOG_RENDER       = NLOG1;
    const int NLOG_BUILDVERTEX  = NLOG2;
    const int NLOG_BUILDINDEX   = NLOG3;
    const int NLOG_STREAMING    = NLOG4;
    const int NLOG_WMAPCACHE    = NLOG5;
    const int NLOG_LMAPCACHE    = NLOG6;
    const int NLOG_VERTEXCACHE  = NLOG7;
    const int NLOG_INDEXCACHE   = NLOG8;
};

//------------------------------------------------------------------------------
class ncTerrainGMMClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncTerrainGMMClass,nComponentClass);

public:

    enum LODSelectionMethod {
        PixelErrorLOD,
        BlockDistance,
    };

    /// constructor
    ncTerrainGMMClass();
    /// destructor
    ~ncTerrainGMMClass();

    /// save object to persistent stream
    bool SaveCmds(nPersistServer* ps);
    /// initialize component pointers from entity object
    void InitInstance(nObject::InitInstanceMsg initType);

    /// load resources
    bool LoadResources();
    /// unload resources
    void UnloadResources();
    /// are resources loaded & valid
    bool IsValid();

    /// set the block size of the lod unit block
    void SetBlockSize(int);
    /// get the block size of the lod unit
    int GetBlockSize() const;
    /// get the block side size scaled (according to the scale factor)
    float GetBlockSideSizeScaled() const;
    /// get the total side size scaled (according to the scale factor)
    float GetTotalSideSizeScaled() const;
    /// get the block size of the lod unit
    int GetNumBlocks() const;
    /// get the heightmap size
    int GetHeightMapSize() const;
    /// get the distance between consecutive points in the heightmap
    float GetPointSeparation() const;
    /// get the minimum allowed height for the terrain
    float GetMinHeight() const;
    /// get the maximum allowed height for the terrain
    float GetMaxHeight() const;
    /// get the block size of the lod unit
    int GetNumLODLevels() const;
    /// get the depth of the terrain (for quadtrees)
    int GetDepth() const;
    /// get heightmap resource
    nFloatMap *  GetHeightMap() const;
    /// get heightmap resource path
    nString GetHeightMapPath() const;
    /// set error in screen pixels
    void SetError(float);
    /// get error
    float GetError() const;
    /// save the associated resources
    bool SaveTerrainGMM() const;
    /// set LOD selection technique
    void SetLODSelectorMethod(LODSelectionMethod);
    /// get LOD selection technique
    LODSelectionMethod GetLODSelectorMethod() const;

    /// set geometry lod distance parameters for block distance LOD method
    void SetGeometryLODDistance(int, int, int, int, int, int);
    /// get geometry lod distance parameters for block distance LOD method
    void GetGeometryLODDistance(int &, int &, int &, int &, int &, int &) const;
    /// get geometry LOD distances
    const int * GetGeometryLODDistance() const;
    /// get number of geometry LOD distances
    int GetNumGeometryLODDistances() const;

    /// Convert from world coordinates to block coordinates
    int GlobalCoordToBlockCoord(float x) const;
    /// Convert from hexel coordinates to block coordinates
    int HexelCoordToBlockCoord(int x) const;

    /// get the vertex builder
    nGMMVertexBuilder * GetVertexBuilder() const;
    /// get the vertex sorter
    nGMMVertexLODSorting * GetVertexSorting() const;
    /// get the index builder
    nGMMIndexBuilder * GetIndexBuilder() const;
    /// get the index cache
    nGMMIndexCache * GetIndexCache() const;
    /// get the mesh cache
    nGMMMeshCache * GetMeshCache() const;
    /// get the hole indices cache
    nGMMHoleIndicesCache * GetHoleIndicesCache() const;
    /// get the low resolution mesh cache
    nGMMLowResMeshCache * GetLowResMeshCache() const;
    /// is the lod level provided considered low resolution (low resolution mesh cache useful?)
    bool IsLowResMeshLOD(int lod) const;
    /// get LOD system
    nGMMLODSystem * GetLODSystem() const;

    /// reset statistics
    void ResetStats();

protected:

    /// Calculate terrain parameters (number of blocks, lod levels)
    void CalcTerrainParams();
    /// allocate all LOD Systems
    void AllocateLODSystems();
    /// deallocate all LOD Systems
    void DeallocateLODSystems();

    /// Pixel error in screen pixels
    float error;
    /// Block size
    int blockSize;
    /// number of blocks in one direction (x,z)
    int numBlocks;
    /// number of LOD levels
    int numLODLevels;
    /// number of depth levels 
    int depth;
    /// lod selection method
    LODSelectionMethod lodSelMethod;
    /// reference to the heightMap object
    nRef<nFloatMap> refHeightMap;

    /// Level of detail system used for terrains
    nGMMLODSystem * currentLODSystem;

    /// block distances to use for block distance LOD method
    int geometryLODDistance[MaxLODLevels];

};

//------------------------------------------------------------------------------
N_CMDARGTYPE_NEW_TYPE(ncTerrainGMMClass::LODSelectionMethod, "i", (value = (ncTerrainGMMClass::LODSelectionMethod) cmd->In()->GetI()), (cmd->Out()->SetI(value))  );

//---------------------------------------------------------------------------          
/**
    Set the size of the block in each direction (x,z).
    The size must be power of 2 + 1.
    The block is square of blockSize x blockSize dimensions.
*/
inline            
void 
ncTerrainGMMClass::SetBlockSize(int blockSize)
{
    n_assert2(!((blockSize-1) & (blockSize-2)), "block size must be power of 2 + 1");

    this->blockSize = blockSize;
}

//---------------------------------------------------------------------------
/**
    @return the size of the block
*/
inline
int 
ncTerrainGMMClass::GetBlockSize() const
{
    return this->blockSize;
}

//---------------------------------------------------------------------------
/**
    @return the number of blocks in one direction
*/
inline
int 
ncTerrainGMMClass::GetNumBlocks() const
{
    return this->numBlocks;
}

//---------------------------------------------------------------------------
/**
    @return the number of LOD levels
*/
inline
int 
ncTerrainGMMClass::GetNumLODLevels() const
{
    return this->numLODLevels;
}

//---------------------------------------------------------------------------
/**
    @return the depth of the quadtree
*/
inline
int 
ncTerrainGMMClass::GetDepth() const
{
    return this->depth;
}

//---------------------------------------------------------------------------
/**
    @return the heightMap object
*/
inline
nFloatMap * 
ncTerrainGMMClass::GetHeightMap() const
{
    return this->refHeightMap.get();
}

//---------------------------------------------------------------------------
/**
    Set the error in number of pixels
*/
inline
void 
ncTerrainGMMClass::SetError(float error)
{
    n_assert(error >= 0);
    this->error = error;
}

//---------------------------------------------------------------------------
/**
    @return the error in number of pixels
*/
inline
float
ncTerrainGMMClass::GetError() const
{
    return this->error;
}

//---------------------------------------------------------------------------
/**
    @return the LOD system currently used for this terrain class
*/
inline
nGMMLODSystem * 
ncTerrainGMMClass::GetLODSystem() const
{
    return this->currentLODSystem;
}

//---------------------------------------------------------------------------
#endif // N_NCGFXTERRAINGMMCLASS_H
