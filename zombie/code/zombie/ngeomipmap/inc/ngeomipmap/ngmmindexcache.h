#ifndef N_GMMINDEXCACHE_H
#define N_GMMINDEXCACHE_H
//------------------------------------------------------------------------------
/**
    @file ngmmindexcache.h
    @class nGMMIndexCache
    @ingroup NebulaTerrain
    
    @author Mateu Batle Sastre

    @brief This class handles the management of index buffers for geomipmapped
    terrains.

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "gfx2/nmesh2.h"
#include "kernel/nref.h"

//------------------------------------------------------------------------------
class nGMMIndexBuilder;

//------------------------------------------------------------------------------
class nGMMIndexCache
{
public:
    /// constructor
    nGMMIndexCache();
    /// destructor
    ~nGMMIndexCache();

    /// set the number of LOD levels
    void Setup(const char * terrainClassName, int numLODLevels, nGMMIndexBuilder * indexBuilder);

    /// get the mesh to use for the lod settings specified
    nMesh2 * GetIndexMesh(int lod, int lodN, int lodE, int lodS, int lodW);
    /// get memory pointer to precalculated index buffers
    nuint16 * GetIndexBuffer(int lod, int lodN, int lodE, int lodS, int lodW, int & numIndices);

private:

    /// allocate resources
    void AllocateResources();
    /// deallocate resources
    void DeallocateResources();

    /// allocate system memory indices
    void AllocateBuffers();
    /// deallocate system memory indices
    void DeallocateBuffers();

    /// index builder
    nGMMIndexBuilder * indexBuilder;
    /// name of the terrain entity class
    nString terrainClassName;
    /// number of LOD levels
    int numLODLevels;

    /// meshes where to store indices (graphics memory)
    nRef<nMesh2> mesh[MaxLODLevels];

    struct nIndicesInfo
    {
        int firstIndex;
        int numIndices;
    };
    /// indices stored in system memory
    nuint16 * indices[MaxLODLevels];
    /// data with information about each group in indices
    nIndicesInfo * groups[MaxLODLevels];
};

//------------------------------------------------------------------------------
#endif // N_GMMINDEXCACHE_H
