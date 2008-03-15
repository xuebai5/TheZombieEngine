#ifndef N_GROWTHTURECACHEENTRY_H
#define N_GROWTHTURECACHEENTRY_H
//------------------------------------------------------------------------------
/**
    (C) 2006 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ncache.h"
#include "resource/nresource.h"
#include "gfx2/ntexture2.h"

//------------------------------------------------------------------------------
class nGrowthTextureCacheEntry : public nCacheEntry
{
public:
    /// extra information provided to loader
    //typedef ncTerrainGMMCell nCacheInfo;
    typedef nEntityClass nCacheInfo;

    /// constructor
    nGrowthTextureCacheEntry();
    /// destructor
    ~nGrowthTextureCacheEntry();

    /// allocate the cache entry resources
    bool Alloc();
    /// deallocate the cache entry resources
    bool Dealloc();
    /// load data in the cache entry
    bool Load(nCacheKey key, nCacheInfo * info);
    /// unload data from the cache entry
    bool Unload();
    /// return the resource;
    nTexture2* GetResource();

    /// Helper method to convert from block(bx,bz) to key 
    static nCacheKey ToKey(int bx, int bz);
    /// Helper method to convert from key to block(bx,bz)
    static void FromKey(nCacheKey key, int & bx, int & bz);

protected:
    nRef<nTexture2> refTexture;
};

//------------------------------------------------------------------------------
#endif //N_GMMTEXTURECACHEENTRY_H