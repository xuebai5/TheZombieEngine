#include "precompiled/pchngrass.h"
#include "nvegetation/nvegetationmeshcacheentry.h"
#include "nvegetation/nVegetationMeshResourceLoader.h"
#include "nvegetation/ngrassmodule.h"

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
#include "ndebug/ndebugtext.h"
#include "ndebug/ndebuggraphicsserver.h"
#include "kernel/ntimeserver.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "napplication/napplication.h"
#endif
#endif



/*-----------------------------------------------------------------------------
    @file nvegetationmeshcacheentry.cc
    @ingroup NebulaGrass
    @author Cristobal Castillo Domingo

    @brief cache entry for vegetation mesh

    (C) 2004 Conjurer Services, S.A.
*/


//---------------------------------------------------------------------------
/**
*/
nVegetationMeshCacheEntry::nVegetationMeshCacheEntry() :
    hasMesh(false),
    terrainClass(0)
{
    /// empty
}

//---------------------------------------------------------------------------
/**
*/
nVegetationMeshCacheEntry::~nVegetationMeshCacheEntry()
{
    this->Unload();
    this->Dealloc();
}

//---------------------------------------------------------------------------
/**
*/
bool 
nVegetationMeshCacheEntry::Alloc()
{
    nCacheEntry::Alloc();
    nString resloader = nVegetationMeshResourceLoader::Instance()->GetFullName();
    this->terrainClass = 0;

    this->mesh = nGfxServer2::Instance()->NewMesh(0);
    n_assert(this->mesh);
    this->mesh->SetResourceLoader(resloader.Get());
    /*
    
    this->mesh->SetFilename(resName);
    this->mesh->Load();
    */
    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nVegetationMeshCacheEntry::Dealloc()
{
    this-> terrainClass = 0;
    if (this->mesh.isvalid())
    {
        if ( this->mesh->IsLoaded() )
        {
            this->mesh->Unload();
        }
        this->mesh->Release();
        this->mesh.invalidate();
    }

    return nCacheEntry::Dealloc();
}


//---------------------------------------------------------------------------
/**
*/
bool
nVegetationMeshCacheEntry::Load(nCacheKey key, nCacheInfo * terrainClass)
{
    nCacheEntry::Load(key, 0);
    this->terrainClass = terrainClass;

    #ifndef NGAME // For show the debug info
    #ifndef __ZOMBIE_EXPORTER__
    nTime time(0);
    static float ypos = 0.45f;
    if ( ncTerrainVegetationClass::debugInfo )
    {
        time = nTimeServer::Instance()->GetTime();
    }
    #endif  
    #endif

    nString fileName(key);
    this->mesh->SetFilename(fileName);

    if ( this->mesh->GetVertexBufferByteSize() == 0  ) //if not has vertex buffer create
    {
        this->mesh->Load();
        this->hasMesh = this->mesh->IsValid();
    } 
    else
    {
        // not use directly load method's mesh because it need a unload object
        this->hasMesh = nVegetationMeshResourceLoader::Instance()->Load( fileName.Get(), this->mesh);
        if (hasMesh) 
        {
            this->mesh->SetState(nResource::Valid);
        } else
        {
            this->mesh->SetState(nResource::Empty); // Mark as create but invalid data
            // it is necesary when device lost, unload the resource
        }
    }

 
#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    // For show debug info
    if ( ncTerrainVegetationClass::debugInfo )
    {
        time = nTimeServer::Instance()->GetTime() - time;
        // Create a different color for diffferent frame id
        float col= 0.1f*( nApplication::Instance()->GetFrameId() % 11);
        vector3 color( col , 1.0f, 1.0f - col);
        nDebugText * text = nDebugGraphicsServer::Instance()->NewDebugText( );
        text->SetScreenPos( -0.99f, ypos);
        text->SetColor(color);
        text->Format(" Growth mesh create %.8X time:%f ms", key , 1000.0f*time );
        text->SetLife( nGrassModule::lifeTimeDebugText );
        nDebugGraphicsServer::Instance()->Kill( text );
        ypos+=0.05f;
        if ( ypos>0.9f) ypos=0.45f;
        nGrassModule::AddMeshLoadTime(1000.f * float(time) );
    }
#endif  
#endif

    return true;
}

//---------------------------------------------------------------------------
/**
*/
bool
nVegetationMeshCacheEntry::Unload()
{
    if ( this->mesh.isvalid() )
    {
        if ( this->mesh->IsLoaded() )
        {
            // Not unload the mesh because the unload method destroy the vertex buffer
           
            this->hasMesh = false; // The mesh is not load
        }
    }

    return nCacheEntry::Unload();
}

//------------------------------------------------------------------------------
nMesh2 *
nVegetationMeshCacheEntry::GetMesh()
{
    return this->mesh;
}

//------------------------------------------------------------------------------
bool
nVegetationMeshCacheEntry::HasMesh()
{
    return this->hasMesh;
}
