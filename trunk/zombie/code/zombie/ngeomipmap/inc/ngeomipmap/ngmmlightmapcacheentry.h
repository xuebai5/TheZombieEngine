#ifndef N_GMMLIGHTMAPCACHEENTRY_H
#define N_GMMLIGHTMAPCACHEENTRY_H
/**
    (C) 2006 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ncache.h"

//------------------------------------------------------------------------------
class nGMMLightMapCacheEntry : public nCacheEntry
{
public:
    /// extra information provided to loader
    typedef ncTerrainGMMCell nCacheInfo;

    /// constructor
    nGMMLightMapCacheEntry();
    /// destructor
    ~nGMMLightMapCacheEntry();

    /// allocate the cache entry resources
    bool Alloc();
    /// deallocate the cache entry resources
    bool Dealloc();
    /// load data in the cache entry
    bool Load(nCacheKey key, nCacheInfo * info);
    /// unload data from the cache entry
    bool Unload();
    /// get lightmap texture
    nTexture2 * GetLightMapTexture();

    /// Setup shared for all cache entries
    static void Setup(nEntityClass * cl);
    /// Helper method to convert from block(bx,bz) to key 
    static nCacheKey ToKey(int bx, int bz);
    /// Helper method to convert from key to block(bx,bz)
    static void FromKey(nCacheKey key, int & bx, int & bz);

    /*
    /// enable / disable async resource loading
    static void SetAsync(bool async);
    /// get async state
    static bool GetAsync();
    */

protected:
    /// load a weightmap from disk
    bool LoadLightmap(int bx, int bz);

    nRef<nTexture2> refLightMapTexture;
    //ncTerrainGMMCell * cell;

    // static data
    static nEntityClass * outdoorClass;
    static int count;
    //static bool async;
};

//------------------------------------------------------------------------------
#endif //N_GMMTEXTURECACHEENTRY_H
