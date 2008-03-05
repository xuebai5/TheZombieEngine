#ifndef N_GROWTHSEEDCACHEENTRY_H
#define N_GROWTHSEEDCACHEENTRY_H
//------------------------------------------------------------------------------
#include "ngeomipmap/ncache.h"
#include "ngrassgrowth/ngrowthseeds.h"

//------------------------------------------------------------------------------
class nGrowthSeedCacheEntry : public nCacheEntry
{
public:
    /// extra information provided to loader
    //typedef ncTerrainGMMCell nCacheInfo;
    typedef nEntityClass nCacheInfo;

    /// constructor
    nGrowthSeedCacheEntry();
    /// destructor
    ~nGrowthSeedCacheEntry();

    /// allocate the cache entry resources
    bool Alloc();
    /// deallocate the cache entry resources
    bool Dealloc();
    /// load data in the cache entry
    bool Load(nCacheKey key, nCacheInfo * info);
    /// unload data from the cache entry
    bool Unload();

    /// get groth seed texture
    nGrowthSeeds* GetGrowthSeeds();

    /// Helper method to convert from block(bx,bz) to key 
    static nCacheKey ToKey(int bx, int bz);
    /// Helper method to convert from key to block(bx,bz)
    static void FromKey(nCacheKey key, int & bx, int & bz);

protected:
    nRef<nGrowthSeeds> refGrowthSeed;
    //ncTerrainGMMCell * cell;

    static nEntityClass * outdoorClass;
    static int count;
};

#endif //!N_GROWTHSEEDCACHEENTRY_H
//------------------------------------------------------------------------------