#ifndef N_GMMCACHEENTRY_H
#define N_GMMCACHEENTRY_H

/**
    (C) 2006 Conjurer Services, S.A.
*/

struct nGMMCacheEntry
{
    nRef<nMesh2> mesh;
    ncTerrainGMMCell * cell;
    int indexKey;
};

struct nGMMTextureCacheEntry
{
    nRef<nTexture2> tex;
    ncTerrainGMMCell * cell;
};

#endif //N_GMMCACHEENTRY_H
