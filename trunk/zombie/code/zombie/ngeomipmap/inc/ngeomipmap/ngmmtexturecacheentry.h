#ifndef N_GMMTEXTURECACHEENTRY_H
#define N_GMMTEXTURECACHEENTRY_H
//------------------------------------------------------------------------------
#include "kernel/nprofiler.h"
#include "ngeomipmap/ncache.h"
#ifndef NGAME
#include "ngeomipmap/nterrainweightmapbuilder.h"
#endif

//------------------------------------------------------------------------------
class ncTerrainGMMCell;

//------------------------------------------------------------------------------
class nGMMTextureCacheEntry : public nCacheEntry
{
public:
    /// extra information provided to loader
    typedef ncTerrainGMMCell nCacheInfo;

    /// constructor
    nGMMTextureCacheEntry();
    /// destructor
    ~nGMMTextureCacheEntry();

    /// allocate the cache entry resources
    bool Alloc();
    /// deallocate the cache entry resources
    bool Dealloc();
    /// load data in the cache entry
    bool Load(nCacheKey key, nCacheInfo * info);
    /// unload data from the cache entry
    bool Unload();

    /// get weightmap texture
    nTexture2 * GetWeightMapTexture();
    /// Setup shared for all cache entries
    static void Setup(nEntityClass * cl);
    /// Helper method to convert from block(bx,bz) to key 
    static nCacheKey ToKey(int bx, int bz);
    /// Helper method to convert from key to block(bx,bz)
    static void FromKey(nCacheKey key, int & bx, int & bz);
    /// enable / disable async resource loading
    static void SetAsync(bool async);
    /// get async state
    static bool GetAsync();

    /// Get material layer index with normalized coords
    nuint8 GetLayerIndexAt( float dx, float dz );
    /// Generate material lookup table from weightmap info
    void GenerateMaterialLookupTable(int bx, int bz);

protected:

#ifndef NGAME
    /// generates the weightmap from the bytemap information (editor-only)
    bool GenerateWeightmap(int bx, int bz);
#endif
    /// load a weightmap from disk
    bool LoadWeightmap(int bx, int bz);

    nRef<nTexture2> refWeightMapTexture;
    
    /// Block material lookup table size
    int materialLookupTableSize;

    /// Material lookup table generated flag
    bool materialLookupTableGenerated;

    /// Block material lookup table
    nuint8 *materialLookupTable;

    // static data
    static nEntityClass * outdoorClass;
    static int count;
    static bool async;
    static nProfiler profGMMWeightMapIO;
};

//------------------------------------------------------------------------------
#endif //N_GMMTEXTURECACHEENTRY_H
