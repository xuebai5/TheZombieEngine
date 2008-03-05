#ifndef N_GMMMESHCACHEENTRY_H
#define N_GMMMESHCACHEENTRY_H
//------------------------------------------------------------------------------
#include "ngeomipmap/ncache.h"

//------------------------------------------------------------------------------
class ncTerrainGMMClass;
class nMesh2;
class ncTerrainGMMCell;

//------------------------------------------------------------------------------
/**
    This class has information for one entry of the geomipmap mesh cache.
*/
class nGMMMeshCacheEntry : public nCacheEntry
{
public:
    /// extra information provided to loader
    typedef ncTerrainGMMCell nCacheInfo;

    /// constructor
    nGMMMeshCacheEntry();
    /// destructor
    ~nGMMMeshCacheEntry();

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
    static nCacheKey ToKey(int bx, int bz);
    /// Helper method to convert from key to block(bx,bz)
    static void FromKey(nCacheKey key, int & bx, int & bz);

protected:
    nRef<nMesh2> mesh;
    ncTerrainGMMCell * cell;

    static ncTerrainGMMClass * tgmmc;
};

//------------------------------------------------------------------------------
#endif //N_GMMMESHCACHEENTRY_H
