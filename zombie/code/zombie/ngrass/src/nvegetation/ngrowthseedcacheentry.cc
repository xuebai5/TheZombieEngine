#include "precompiled/pchngrass.h"

/*-----------------------------------------------------------------------------
    @file ngmmtexturecachenetry.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    (C) 2005 Conjurer Services, S.A.
*/
//---------------------------------------------------------------------------
#include "nvegetation/ngrowthseedcacheentry.h"
#include "entity/nentity.h"
#include "resource/nresourceserver.h"
#include "nasset/nentityassetbuilder.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "nvegetation/ngrassmodule.h"

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
#include "ndebug/ndebugtext.h"
#include "ndebug/ndebuggraphicsserver.h"
#include "kernel/ntimeserver.h"
#endif
#endif


//---------------------------------------------------------------------------
const int MAX_BLOCKS_SIDE = 2048;

//---------------------------------------------------------------------------
nEntityClass * nGrowthSeedCacheEntry::outdoorClass = 0;
int nGrowthSeedCacheEntry::count = 0;

//---------------------------------------------------------------------------
/**
*/
nGrowthSeedCacheEntry::nGrowthSeedCacheEntry()
    //cell(0)
{
    /// empty
}

//---------------------------------------------------------------------------
/**
*/
nGrowthSeedCacheEntry::~nGrowthSeedCacheEntry()
{
    this->Unload();
    this->Dealloc();
}

//---------------------------------------------------------------------------
/**
*/
bool
nGrowthSeedCacheEntry::Alloc()
{
    nCacheEntry::Alloc();

    // get terrain material class
    /*
    ncTerrainMaterialClass * tlm = this->outdoorClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(tlm);
    */

    // generate resource name
    nString resName;
    resName.Append("growtseed_cache_");
    resName.AppendInt(count++);

    this->refGrowthSeed =  static_cast<nGrowthSeeds*>(nResourceServer::Instance()->NewResource("ngrowthseeds", resName.Get() , nResource::Other));


    //this->cell  = 0;
//    this->refWeightMapTexture = tlm->NewWeightMapTexture(resName.Get());
    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGrowthSeedCacheEntry::Dealloc()
{
    nCacheEntry::Dealloc();

    if (this->refGrowthSeed.isvalid())
    {
        this->refGrowthSeed->Release();
        this->refGrowthSeed.invalidate();
    }

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGrowthSeedCacheEntry::Load(nCacheKey key, nCacheInfo * outdoorClass)
{

    nCacheEntry::Load(key, 0);

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
    nTime time = 0;
    if ( ncTerrainVegetationClass::debugInfo )
    {
        time = nTimeServer::Instance()->GetTime();
    }
    #endif  
    #endif


    if (this->refGrowthSeed.isvalid() && this->refGrowthSeed->IsLoaded())
    {
        this->refGrowthSeed->Unload();
    }

    // decode key
    int bx, bz;
    nGrowthSeedCacheEntry::FromKey(key, bx, bz);
    this->refGrowthSeed->SetFilename( outdoorClass->GetComponentSafe<ncTerrainVegetationClass>()->GetGrowthSeedFileName(bx,bz) );
    this->refGrowthSeed->Load();


#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    if ( ncTerrainVegetationClass::debugInfo )
    {
        time = nTimeServer::Instance()->GetTime() - time;
        nDebugText * text = nDebugGraphicsServer::Instance()->NewDebugText( );
        text->SetScreenPos( -0.99f, 0.40f);
        text->Format(" Growth loaded, cell  x:%d  z: %d   time:%f ms", bx,bz , 1000.f*time );
        text->SetLife( nGrassModule::lifeTimeDebugText );
        nDebugGraphicsServer::Instance()->Kill( text );
        nGrassModule::AddSeedLoadTime( 1000.f*float(time) );
    }

#endif  
#endif
    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nGrowthSeedCacheEntry::Unload()
{
    if ( this->refGrowthSeed.isvalid() && this->refGrowthSeed->IsLoaded() )
    {
        this->refGrowthSeed->Unload();
    }
    return nCacheEntry::Unload();
}

//------------------------------------------------------------------------------
nGrowthSeeds *
nGrowthSeedCacheEntry::GetGrowthSeeds()
{
    return this->refGrowthSeed.get();
}

//------------------------------------------------------------------------------
nCacheKey 
nGrowthSeedCacheEntry::ToKey(int bx, int bz)
{
    return bz * MAX_BLOCKS_SIDE + bx;
}

//------------------------------------------------------------------------------
void
nGrowthSeedCacheEntry::FromKey(nCacheKey key, int & bx, int & bz)
{
    bx = key % MAX_BLOCKS_SIDE;
    bz = key / MAX_BLOCKS_SIDE;
}

//------------------------------------------------------------------
