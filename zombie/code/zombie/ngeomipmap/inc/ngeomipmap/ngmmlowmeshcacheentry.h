#ifndef N_GMMLOWMESHCACHEENTRY_H
#define N_GMMLOWMESHCACHEENTRY_H

/**
    (C) 2006 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ncache.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "kernel/nref.h"

//------------------------------------------------------------------------------
class ncTerrainGMMClass;
class nMesh2;
class ncTerrainGMMCell;

//------------------------------------------------------------------------------
/**
    This class has information for one entry of the geomipmap mesh cache for
    low level of detail terrain blocks. This is stored in system memory not
    in video memory.
*/
class nGMMLowMeshCacheEntry : public nCacheEntry
{
public:
    /// extra information provided to loader
    typedef ncTerrainGMMCell nCacheInfo;

    /// constructor
    nGMMLowMeshCacheEntry();
    /// destructor
    ~nGMMLowMeshCacheEntry();

    /// allocate the cache entry resources
    bool Alloc();
    /// deallocate the cache entry resources
    bool Dealloc();
    /// load data in the cache entry
    bool Load(nCacheKey key, nCacheInfo * info);
    /// unload data from the cache entry
    bool Unload();

    /// return the cached vertices
    float * GetVertices() const;

    /// Setup shared for all cache entries
    static void Setup(ncTerrainGMMClass * cl, int lod);
    /// return the maximum number of vertices stored
    static int GetMaxVertices();
    /// return the maximum number of vertices stored
    static int GetMaxLOD();
    /// Helper method to convert from block(bx,bz) to key 
    static nCacheKey ToKey(int bx, int bz);
    /// Helper method to convert from key to block(bx,bz)
    static void FromKey(nCacheKey key, int & bx, int & bz);

protected:
    float * vertices;
    ncTerrainGMMCell * cell;

    static ncTerrainGMMClass * tgmmc;
    static nRef<nGMMGeometryStorage> storage;
    static int lod;
    static int maxVertices;
};

//------------------------------------------------------------------------------
#endif //N_GMMLOWMESHCACHEENTRY_H
