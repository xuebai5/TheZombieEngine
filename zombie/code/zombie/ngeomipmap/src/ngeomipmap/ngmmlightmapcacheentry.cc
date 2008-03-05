/*-----------------------------------------------------------------------------
    @file ngmmlightmapcachenetry.cc
    @ingroup NebulaTerrain

    @author Cristobal Castillo Domingo

    (C) 2005 Conjurer Services, S.A.
*/
//---------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmlightmapcacheentry.h"

#include "precompiled/pchngeomipmap.h"
#include "kernel/nlogclass.h"
#include "kernel/nfileserver2.h"

//---------------------------------------------------------------------------
const int MAX_BLOCKS_SIDE = 2048;

//---------------------------------------------------------------------------
extern const char * errorTexturePath;

//---------------------------------------------------------------------------
nEntityClass * nGMMLightMapCacheEntry::outdoorClass = 0;
int nGMMLightMapCacheEntry::count = 0;
//bool nGMMLightMapCacheEntry::async = false;

//---------------------------------------------------------------------------
/**
*/
nGMMLightMapCacheEntry::nGMMLightMapCacheEntry()
    //cell(0)
{
    /// empty
}

//---------------------------------------------------------------------------
/**
*/
nGMMLightMapCacheEntry::~nGMMLightMapCacheEntry()
{
    this->Unload();
    this->Dealloc();
}

//---------------------------------------------------------------------------
/**
*/
bool
nGMMLightMapCacheEntry::Alloc()
{
    NLOG(terrain, (nTerrainLog::NLOG_LMAPCACHE | 1, "nGMMLightMapCacheEntry::Alloc 0x%x", this));

    nCacheEntry::Alloc();

    // get terrain material class
    ncTerrainMaterialClass * tlm = this->outdoorClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    // generate resource name
    nString resLightName("lightmap_cache_");
    resLightName.AppendInt(count);
    ++count;

    this->refLightMapTexture = nGfxServer2::Instance()->NewTexture( resLightName.Get() );


    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGMMLightMapCacheEntry::Dealloc()
{
    NLOG(terrain, (nTerrainLog::NLOG_LMAPCACHE | 1, "nGMMLightMapCacheEntry::Dealloc 0x%x", this));

    nCacheEntry::Dealloc();

    if (this->refLightMapTexture.isvalid())
    {
        this->refLightMapTexture->Release();
        this->refLightMapTexture.invalidate();
    }

    return true;
}

//---------------------------------------------------------------------------
/**
    Load the cache entry with the key provided. When it is in editor mode
    it will recalculate the weightmap using the weightmap builder, but
    when not in terrain-editor or in NGAME then it will be loaded from disk.
*/
bool
nGMMLightMapCacheEntry::Load(nCacheKey key, nCacheInfo * /*cell*/)
{
    NLOG(terrain, (nTerrainLog::NLOG_LMAPCACHE | 0, "nGMMLightMapCacheEntry::Load key=0x%x", key));

    int bx, bz;
    nGMMLightMapCacheEntry::FromKey(key, bx, bz);

    if (! this->LoadLightmap(bx, bz) )
    {
        return false;
    }

    return nCacheEntry::Load(key, 0);
}



//---------------------------------------------------------------------------
/**
    Loads a lightmap from disk. It checks the cache async state to know if
    the load must be done sync / async.
*/
bool
nGMMLightMapCacheEntry::LoadLightmap(int bx, int bz)
{
    ncTerrainMaterialClass * tlm = this->outdoorClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    nString filename = tlm->GetLightMapTexturePath(bx, bz);
    this->refLightMapTexture->SetFilename(filename);

    if ( filename.IsEmpty() )
    {
        return true; // empty lightmap loaded
    }


    if (!nFileServer2::Instance()->FileExists(filename))
    {
        return false;
    }

    this->refLightMapTexture->SetFilename(filename);
    return this->refLightMapTexture->Load();
}

//---------------------------------------------------------------------------
/**
*/
bool
nGMMLightMapCacheEntry::Unload()
{
    NLOG(terrain, (nTerrainLog::NLOG_LMAPCACHE | 0, "nGMMLightMapCacheEntry::Unload key=0x%x", key));
    

    if (this->refLightMapTexture.isvalid())
    {
        this->refLightMapTexture->Unload();
    }

    return nCacheEntry::Unload();
}


//------------------------------------------------------------------------------
nTexture2 *
nGMMLightMapCacheEntry::GetLightMapTexture()
{
    return this->refLightMapTexture.get();
}

//------------------------------------------------------------------------------
nCacheKey 
nGMMLightMapCacheEntry::ToKey(int bx, int bz)
{
    return bz * MAX_BLOCKS_SIDE + bx;
}

//------------------------------------------------------------------------------
void
nGMMLightMapCacheEntry::FromKey(nCacheKey key, int & bx, int & bz)
{
    bx = key % MAX_BLOCKS_SIDE;
    bz = key / MAX_BLOCKS_SIDE;
}

//------------------------------------------------------------------------------
void 
nGMMLightMapCacheEntry::Setup(nEntityClass * cl)
{
    nGMMLightMapCacheEntry::outdoorClass = cl;

    // initialize the mesh builder
    ncTerrainGMMClass * terrainGMMClass = cl->GetComponent<ncTerrainGMMClass>();
    n_assert(terrainGMMClass);
}


//------------------------------------------------------------------------------
