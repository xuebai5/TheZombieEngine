#ifndef N_GMMHOLEINDICESCACHEENTRY_H
#define N_GMMHOLEINDICESCACHEENTRY_H
//------------------------------------------------------------------------------
#include "ngeomipmap/ncache.h"

//------------------------------------------------------------------------------
class ncTerrainGMMClass;
class nMesh2;

//------------------------------------------------------------------------------
/**
    This class has information for one entry of the hole indices cache.
    It contains only information for the indices, the vertex buffer is in
    the GMM mesh cache.
*/
class nGMMHoleIndicesCacheEntry : public nCacheEntry
{
public:
    /// extra information provided to loader
    typedef ncTerrainGMMCell nCacheInfo;

    /// constructor
    nGMMHoleIndicesCacheEntry();
    /// destructor
    ~nGMMHoleIndicesCacheEntry();

    /// allocate the cache entry resources
    bool Alloc();
    /// deallocate the cache entry resources
    bool Dealloc();
    /// load data in the cache entry
    bool Load(nCacheKey key, nCacheInfo * info);
    /// unload data from the cache entry
    bool Unload();

    /// return the cached mesh
    nMesh2 * GetMesh();

    /// Setup shared for all cache entries
    static void Setup(ncTerrainGMMClass * cl);
    /// Helper method to convert from block(bx,bz) to key 
    static nCacheKey ToKey(int bx, int bz, int indexKey);
    /// Helper method to convert from key to block(bx,bz)
    static void FromKey(nCacheKey key, int & bx, int & bz, int & indexKey);

protected:
    nRef<nMesh2> mesh;
    ncTerrainGMMCell * cell;

    static ncTerrainGMMClass * tgmmc;
};

//------------------------------------------------------------------------------
#endif //N_GMMHOLEINDICESCACHEENTRY_H
