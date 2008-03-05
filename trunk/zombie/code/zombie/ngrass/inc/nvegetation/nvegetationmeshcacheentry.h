#ifndef N_VEGETATIONMESHCAHEENTRY_H
#define N_VEGETATIONMESHCAHEENTRY_H
//------------------------------------------------------------------------------
#include "ngeomipmap/ncache.h"
#include "gfx2/nmesh2.h"
#include "entity/nentityclass.h"

//------------------------------------------------------------------------------
/**
    This class has information for one entry of the vegetation mesh for a one subCell
*/
class nVegetationMeshCacheEntry : public nCacheEntry
{
public:
    /// extra information provided to loader
    typedef nEntityClass nCacheInfo;

    /// constructor
    nVegetationMeshCacheEntry();
    /// destructor
    ~nVegetationMeshCacheEntry();

    /// allocate the cache entry resources
    bool Alloc();
    /// deallocate the cache entry resources
    bool Dealloc();
    /// load data in the cache entry
    bool Load(nCacheKey key, nCacheInfo * info);
    /// unload data from the cache entry
    bool Unload();
    /// return if mesh is valid
    bool HasMesh();
    /// return the cached mesh
    nMesh2 * GetMesh();

protected:
    nRef<nMesh2> mesh;
    bool hasMesh;
    nCacheInfo* terrainClass;
};

#endif //!N_VEGETATIONMESHCAHEENTRY_H