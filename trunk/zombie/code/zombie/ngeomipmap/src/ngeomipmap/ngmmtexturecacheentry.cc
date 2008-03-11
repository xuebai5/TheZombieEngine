/*-----------------------------------------------------------------------------
    @file ngmmtexturecachenetry.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    (C) 2005 Conjurer Services, S.A.
*/
//---------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmtexturecacheentry.h"
#include "kernel/nlogclass.h"
#include "kernel/nfileserver2.h"

//---------------------------------------------------------------------------
const int MAX_BLOCKS_SIDE = 2048;

//---------------------------------------------------------------------------
extern const char * errorTexturePath;

//---------------------------------------------------------------------------
nEntityClass * nGMMTextureCacheEntry::outdoorClass = 0;
int nGMMTextureCacheEntry::count = 0;
bool nGMMTextureCacheEntry::async = false;
nProfiler nGMMTextureCacheEntry::profGMMWeightMapIO;

//---------------------------------------------------------------------------
/**
*/
nGMMTextureCacheEntry::nGMMTextureCacheEntry():
    materialLookupTableSize(0),
    materialLookupTable(0),
    materialLookupTableGenerated(false)
{
    /// empty
}

//---------------------------------------------------------------------------
/**
*/
nGMMTextureCacheEntry::~nGMMTextureCacheEntry()
{
    this->Unload();
    this->Dealloc();
}

//---------------------------------------------------------------------------
/**
*/
bool
nGMMTextureCacheEntry::Alloc()
{
    NLOG(terrain, (nTerrainLog::NLOG_WMAPCACHE | 1, "nGMMTextureCacheEntry::Alloc 0x%x", this));

    nCacheEntry::Alloc();

    // get terrain material class
    ncTerrainMaterialClass * tlm = this->outdoorClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    // generate resource name
    nString resWeightName("weightmap_cache_");
    resWeightName.AppendInt(count);
    ++count;

    this->refWeightMapTexture = tlm->NewWeightMapTexture(resWeightName.Get());
    this->refWeightMapTexture->Unload();

    // generate material lookup table
    this->materialLookupTableSize = max( 1, tlm->GetWeightMapSize() / tlm->GetMaterialResolutionDivisor() );
    int n = this->materialLookupTableSize * this->materialLookupTableSize;
    this->materialLookupTable = n_new_array( nuint8, n );
    memset( this->materialLookupTable, 0, n * sizeof( nuint8 ) );

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGMMTextureCacheEntry::Dealloc()
{
    NLOG(terrain, (nTerrainLog::NLOG_WMAPCACHE | 1, "nGMMTextureCacheEntry::Dealloc 0x%x", this));

    nCacheEntry::Dealloc();

    if (this->refWeightMapTexture.isvalid())
    {
        this->refWeightMapTexture->Release();
        this->refWeightMapTexture.invalidate();
    }

    if ( this->materialLookupTable )
    {
        n_delete_array( this->materialLookupTable );
        this->materialLookupTable = 0;
        this->materialLookupTableGenerated = false;
        this->materialLookupTableSize = 0;
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
nGMMTextureCacheEntry::Load(nCacheKey key, nCacheInfo * /*cell*/)
{
    profGMMWeightMapIO.StartAccum();

    int bx, bz;
    nGMMTextureCacheEntry::FromKey(key, bx, bz);

    NLOG(terrain, (nTerrainLog::NLOG_WMAPCACHE | 0, "nGMMTextureCacheEntry::Load key=0x%x (%d,%d) async=%d", key, bx, bz, this->GetAsync()));

    this->materialLookupTableGenerated = false;

#ifndef NGAME
    ncTerrainMaterialClass * tlm = this->outdoorClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    if (tlm->InTerrainEditorState())
    {
        if ( !this->GenerateWeightmap(bx, bz) )
        {
            profGMMWeightMapIO.StopAccum();
            return false;
        }

        profGMMWeightMapIO.StopAccum();
        return nCacheEntry::Load(key, 0);
    }
#endif

    if (! this->LoadWeightmap(bx, bz) )
    {
        profGMMWeightMapIO.StopAccum();
        return false;
    }

    profGMMWeightMapIO.StopAccum();

    return nCacheEntry::Load(key, 0);
}

#ifndef NGAME
//---------------------------------------------------------------------------
/**
    Generates a weightmap procedurally from the bytemap, this process is
    meant to be used only in the editor, too costly to do for real-time game.
*/
bool
nGMMTextureCacheEntry::GenerateWeightmap(int bx, int bz)
{
    ncTerrainMaterialClass * tlm = this->outdoorClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    this->refWeightMapTexture->SetFormat(nTexture2::A8R8G8B8);
    this->refWeightMapTexture->SetAsyncEnabled(false);
    //this->refWeightMapTexture->SetUsage( nTexture2::CreateEmpty | nTexture2::Dynamic );
    this->refWeightMapTexture->SetUsage( nTexture2::CreateEmpty );
    this->refWeightMapTexture->Load(); 

    nTerrainWeightMapBuilder wmapBuilder;
    wmapBuilder.SetTerrainMaterialComponent( tlm ); 
    wmapBuilder.SetWeightTexture( this->refWeightMapTexture.get() );
    wmapBuilder.RefreshWeightMap( bx, bz );

    return true;
}
#endif

//---------------------------------------------------------------------------
/**
    Loads a weightmap from disk. It checks the cache async state to know if
    the load must be done sync / async.
*/
bool
nGMMTextureCacheEntry::LoadWeightmap(int bx, int bz)
{
    ncTerrainMaterialClass * tlm = this->outdoorClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    nString filename = tlm->GetWeightMapTexturePath(bx, bz);

#ifdef NGAME
    this->refWeightMapTexture->SetUsage(0);
#else
    this->refWeightMapTexture->SetUsage( nTexture2::Dynamic );
#endif
    this->refWeightMapTexture->SetFilename(filename);
    this->refWeightMapTexture->SetAsyncEnabled(async);

    bool success = this->refWeightMapTexture->Load();

    return success;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGMMTextureCacheEntry::Unload()
{
#ifndef __NEBULA_NO_LOG__
    int bx, bz;
    nGMMTextureCacheEntry::FromKey(key, bx, bz);
#endif
    NLOG(terrain, (nTerrainLog::NLOG_WMAPCACHE | 0, "nGMMTextureCacheEntry::Unload key=0x%x (%d,%d)", key, bx, bz));

    if (this->refWeightMapTexture.isvalid())
    {
        this->refWeightMapTexture->Unload();
    }

    return nCacheEntry::Unload();
}

//------------------------------------------------------------------------------
/**
    Get material layer handle
*/
nuint8
nGMMTextureCacheEntry::GetLayerIndexAt( float dx, float dz )
{
    n_assert( dx >= 0.0f && dz >= 0.0f && dx <= 1.0f && dz <= 1.0f );
    n_assert( this->materialLookupTableSize > 0 );

    if ( ! this->materialLookupTableGenerated )
    {
        int bx(0), bz(0);
        this->FromKey( this->key, bx, bz );
        this->GenerateMaterialLookupTable(bx, bz);
        this->materialLookupTableGenerated = true;
    }

    int ix = int(min( dx * this->materialLookupTableSize, this->materialLookupTableSize - 1));
    int iz = int(min( dz * this->materialLookupTableSize, this->materialLookupTableSize - 1));

    return this->materialLookupTable[ iz * this->materialLookupTableSize + ix];
}

//------------------------------------------------------------------------------
void
nGMMTextureCacheEntry::GenerateMaterialLookupTable(int bx, int bz)
{
    // get terrain material class
    ncTerrainMaterialClass * tlm = this->outdoorClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);

    tlm->FillMaterialLookupTable( bx, bz, this->GetWeightMapTexture(), this->materialLookupTable, this->materialLookupTableSize );

#if 0 // This code is to dump the texture's data
    nFile * file = nFileServer2::Instance()->NewFileObject();
    nString fileName( "c:\\test\\pruebalookup" );
    fileName.AppendInt( bx );
    fileName.Append( "_" );
    fileName.AppendInt( bz );
    file->Open(fileName.Get(), "w");
    file->Write( this->materialLookupTable, this->materialLookupTableSize * this->materialLookupTableSize );
    file->Close();
#endif
}

//------------------------------------------------------------------------------
nTexture2 *
nGMMTextureCacheEntry::GetWeightMapTexture()
{
    return this->refWeightMapTexture.get();
}

//------------------------------------------------------------------------------
nCacheKey 
nGMMTextureCacheEntry::ToKey(int bx, int bz)
{
    return bz * MAX_BLOCKS_SIDE + bx;
}

//------------------------------------------------------------------------------
void
nGMMTextureCacheEntry::FromKey(nCacheKey key, int & bx, int & bz)
{
    bx = key % MAX_BLOCKS_SIDE;
    bz = key / MAX_BLOCKS_SIDE;
}

//------------------------------------------------------------------------------
void 
nGMMTextureCacheEntry::Setup(nEntityClass * cl)
{
    nGMMTextureCacheEntry::outdoorClass = cl;

    // initialize the mesh builder
    n_assert( cl->GetComponent<ncTerrainGMMClass>() );

    // initialize the profilers
    if (!profGMMWeightMapIO.IsValid())
    {
        profGMMWeightMapIO.Initialize("profGMMWeightMapIO", true);
    }
    profGMMWeightMapIO.ResetAccum();
}

//------------------------------------------------------------------------------
void 
nGMMTextureCacheEntry::SetAsync(bool val)
{
    async = val;
}

//------------------------------------------------------------------------------
bool 
nGMMTextureCacheEntry::GetAsync()
{
    return async;
}

//------------------------------------------------------------------------------
