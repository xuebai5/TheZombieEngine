#include "precompiled/pchngrass_conjurer.h"
#include "nvegetation/ngrowthtexturecacheentry.h"
#include "gfx2/ngfxserver2.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "nvegetation/ncterrainvegetationcell.h"
#include "ngeomipmap/nbytemapbuilder.h"

//---------------------------------------------------------------------------
const int MAX_BLOCKS_SIDE = 2048;



//---------------------------------------------------------------------------
/**
*/
nGrowthTextureCacheEntry::nGrowthTextureCacheEntry()
    //cell(0)
{
    /// empty
}

//---------------------------------------------------------------------------
/**
*/
nGrowthTextureCacheEntry::~nGrowthTextureCacheEntry()
{
    this->Unload();
    this->Dealloc();
}

//---------------------------------------------------------------------------
/**
*/
bool
nGrowthTextureCacheEntry::Alloc()
{
    nCacheEntry::Alloc();

    this->refTexture = nGfxServer2::Instance()->NewTexture(0);
    this->refTexture->SetType( nTexture2::TEXTURE_2D );
    //this->refTexture->SetFormat( nTexture2::A8 );
	this->refTexture->SetFormat( nTexture2::L8 );
    this->refTexture->SetUsage( nTexture2::CreateEmpty | nTexture2::Dynamic);
    this->refTexture->SetWidth(0);
    this->refTexture->SetHeight(0);
    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGrowthTextureCacheEntry::Dealloc()
{
    nCacheEntry::Dealloc();

    if (this->refTexture.isvalid())
    {
        this->refTexture->Release();
        this->refTexture.invalidate();
    }

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGrowthTextureCacheEntry::Load(nCacheKey key, nCacheInfo * outdoorClass)
{
    nCacheEntry::Load(key, 0);
    int bx, bz;
    nGrowthTextureCacheEntry::FromKey(key, bx, bz);

    ncTerrainMaterialClass*  matClass = outdoorClass->GetComponentSafe<ncTerrainMaterialClass>();
    nTerrainCellInfo*        cellInfo = matClass->GetTerrainCellInfo( bx , bz );
    nEntityObject*           cell     = cellInfo->GetTerrainCell();
    ncTerrainVegetationCell* vegCell  = cell->GetComponentSafe<ncTerrainVegetationCell>();
    nByteMap*                byteMap  = vegCell->GetValidGrowthMap();
    ushort                   size     = static_cast<ushort>( byteMap->GetSize() ); 

    // The first time
    if (  this->refTexture->GetHeight() != size ||
          this->refTexture->GetWidth() != size 
       )
    {
        if ( this->refTexture->IsLoaded() )
        {
            this->refTexture->Unload();
        }
        this->refTexture->SetWidth( size );
        this->refTexture->SetHeight( size );
        this->refTexture->Load();
    }

    nByteMapBuilder::FastCopyToTexture( this->refTexture.get() , byteMap );


    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGrowthTextureCacheEntry::Unload()
{
    return nCacheEntry::Unload();
}

//------------------------------------------------------------------------------
nCacheKey 
nGrowthTextureCacheEntry::ToKey(int bx, int bz)
{
    return bz * MAX_BLOCKS_SIDE + bx;
}

//------------------------------------------------------------------------------
void
nGrowthTextureCacheEntry::FromKey(nCacheKey key, int & bx, int & bz)
{
    bx = key % MAX_BLOCKS_SIDE;
    bz = key / MAX_BLOCKS_SIDE;
}

//------------------------------------------------------------------------------
nTexture2* 
nGrowthTextureCacheEntry::GetResource()
{
    return this->refTexture.get();
}

//------------------------------------------------------------------
