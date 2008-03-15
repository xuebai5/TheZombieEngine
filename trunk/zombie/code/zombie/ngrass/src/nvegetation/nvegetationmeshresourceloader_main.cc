#include "precompiled/pchngrass.h"
//------------------------------------------------------------------------------
//  nvegetationmeshresourceloader_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nvegetation/nvegetationmeshresourceloader.h"
#include "gfx2/nmesh2.h"
#include "nspatial/nspatialserver.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "kernel/nlogclass.h"
#include "nvegetation/ngrassmodule.h"
#include "mathlib/simdlib.h"




//------------------------------------------------------------------------------
nNebulaScriptClass(nVegetationMeshResourceLoader, "nresourceloader");

//------------------------------------------------------------------------------
//static const char * const SEPARATOR = "!";
nVegetationMeshResourceLoader * nVegetationMeshResourceLoader::singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nVegetationMeshResourceLoader::nVegetationMeshResourceLoader()
{
    if (!this->singleton)
    {
        this->singleton = this;
    }
}

//------------------------------------------------------------------------------
/**
*/
nVegetationMeshResourceLoader::~nVegetationMeshResourceLoader()
{
    if (this->singleton == this)
    {
        this->singleton = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
nVegetationMeshResourceLoader * 
nVegetationMeshResourceLoader::Instance()
{
    return singleton;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nVegetationMeshResourceLoader::Load(const char * cmdString, nResource * callingResource )
{
    n_assert(callingResource);
    n_assert( callingResource->IsA("nmesh2") );
    nMesh2* batchMesh = static_cast<nMesh2*>(callingResource);
 

    // need the spatial server for get a terrain instance
    nSpatialServer* spatialServer = nSpatialServer::Instance();
	n_assert(spatialServer);
    nEntityObject* terrain = spatialServer->GetOutdoorEntity();
    n_assert(terrain);
    ncTerrainVegetationClass* vegClass = terrain->GetClassComponentSafe<ncTerrainVegetationClass>();

    int growthGroup; // the index of group in a GrowthSeed
    nGrowthSeeds* growthSeed = vegClass->GetGrowthIndex(cmdString, growthGroup);

    if (!growthSeed) // not has a grass for this subcelll
    {
        return false; 
    }

   
    if (!growthSeed->IsLoaded()  ) // not has a grass for this subcelll
    {
        growthSeed->Load(); 
    }

    if (!growthSeed->IsValid() )
    {
       return false;
    }

    const nGrowthSeeds::ListSeed& listSeed = growthSeed->GetSeedsByGroup( growthGroup );

    if ( 0 >= listSeed.Size() ) 
    {
        return false; // if subcell has not a seeds
    }

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
    nTime time = 0;
    if ( ncTerrainVegetationClass::debugInfo )
    {
        time = nTimeServer::Instance()->GetTime();
    }
    #endif  
    #endif

    const nFixedArray<nRef<nMesh2> >& meshLibrary = vegClass->GetMeshLibrary();

    //TODO move all of this to a meshresourceloader
    //---------------------------------------------
    int numVertices = 0;
    int numIndices = 0;

    // Common data info this is info is same in all render, todo: Optimize it
    int vertexComp = 0;


    // First get the data info, this is info is same in all render
    for (int idx = 0; idx < meshLibrary.Size() ; ++idx )
    {
        nRef<nMesh2>& mesh = meshLibrary[ idx ];

        if (mesh.isvalid() && ( !mesh->IsLoaded() ) ) // When device lost, then load all resource but the sort is random
        {
            mesh->Load(); 
        }

        if (mesh.isvalid() && mesh->IsLoaded())
        {
            vertexComp = mesh->GetVertexComponents();
        }
    }

    if ( vertexComp  == 0) 
    {
        return false; // not has any components
    }

    // Second count total size of mesh
    bool outIndex = false;
    int numValidSeeds;
    for (numValidSeeds = 0; numValidSeeds < listSeed.Size() && ! outIndex; ++numValidSeeds )
    {
        int meshId = growthSeed->GetMeshId( listSeed[numValidSeeds] );
        nRef<nMesh2>& mesh = meshLibrary[ meshId ];
        if (mesh.isvalid() && mesh->IsLoaded())
        {
            n_assert(vertexComp == mesh->GetVertexComponents()); // als meshes with sames components
            outIndex = (numVertices + mesh->GetNumVertices() > 65535) || 
                       (numIndices  + mesh->GetNumIndices() > 65535);
            if ( ! outIndex)
            {
                numVertices += mesh->GetNumVertices();
                numIndices += mesh->GetNumIndices();
            }
        }
    }
    if ( outIndex)
    {
        numValidSeeds--;
        NLOG( grass, ( 4 | nGrassModule::CREATION, 
                      "Grass has many seeds, use %d of %d. Block key %s. \n"
                      "\t numVertices %d, numIndicese %d",
                      numValidSeeds, listSeed.Size() , cmdString,numVertices,numIndices 
                      ) 
            );
    }

    // 3rd Create the bacth mesh
    batchMesh->SetUsage( nMesh2::WriteOnly | nMesh2::NeedsVertexShader );
    batchMesh->SetVertexComponents(vertexComp);
    
    if ( batchMesh->GetVertexBufferByteSize() == 0 )
    {
        //Is the first time to load or the mesh is unload
        batchMesh->SetNumVertices(65535);
        batchMesh->SetNumIndices(65535); 
        batchMesh->SetNumGroups(1);
        batchMesh->CreateEmpty();
        n_assert(batchMesh->IsEmpty());
    } else
    {
        n_assert_return2( numIndices  <= batchMesh->GetNumIndices(), false, "the grass has to many indices");
        n_assert_return2( numVertices <= batchMesh->GetNumVertices(), false,  "the grass has to many vertex");
        n_assert_return2( batchMesh->GetNumGroups() == 1 , false, "then grass not has a group");
    }
    
    batchMesh->Group(0).SetFirstVertex(0);
    batchMesh->Group(0).SetNumVertices(numVertices);
    batchMesh->Group(0).SetFirstIndex(0);
    batchMesh->Group(0).SetNumIndices(numIndices);
    
    //int batchVertexWidth = batchMesh->GetVertexWidth();

    // lock vertices and indices
    float* batchVertices     = batchMesh->LockVertices();
    ushort* batchIndices = batchMesh->LockIndices();
    int batchedVertices = 0;
    int batchedIndices = 0;

    // Fill the batch
    for (int idx = 0; idx < numValidSeeds ; ++idx )
    {
        int meshId =  growthSeed->GetMeshId( listSeed[idx] );
        nRef<nMesh2>& mesh = meshLibrary[ meshId ];
        if (mesh.isvalid() && mesh->IsLoaded())
        {
            // batch mesh into local oned
            int numMeshVertices = mesh->GetNumVertices();
            int numMeshIndices  =  mesh->GetNumIndices();
            /*int meshVertexWidth = mesh->GetVertexWidth();*/

            float* vertices = mesh->LockVertices();
            ushort* indices = mesh->LockIndices();

            matrix44 matrix = growthSeed->GetMatrix( listSeed[idx] );

            this->CopyAndTransformVertices( batchMesh , batchVertices, batchedVertices, vertices, numMeshVertices, matrix);

            SIMDLib::Add16u_tuple3( batchIndices + batchedIndices , indices , (ushort)batchedVertices , numMeshIndices/3);

            mesh->UnlockVertices();
            mesh->UnlockIndices();
            batchedVertices += numMeshVertices;
            batchedIndices += numMeshIndices;
        }
    }


    batchMesh->UnlockVertices();
    batchMesh->UnlockIndices();

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
    if ( ncTerrainVegetationClass::debugInfo )
    {
        nGrassModule::AddMeshGenerationTime( 1000.f* float( nTimeServer::Instance()->GetTime() -time));
    }
    #endif  
    #endif

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Copy and transform the vertex , optimize for special case Coord Uv0 and Color
    @param mesh the destionation mesh
    @param bvDst the destination vertex buffer, this is the vertexbuffer of mesh
    @param idxBegim  the index of vertex where to begin to copy
    @param vbSrc the source of vertex buffer
    @param numVertices the number of vertex to copy
    @param matrix the transformation matrix

*/
bool 
nVegetationMeshResourceLoader::CopyAndTransformVertices(nMesh2* mesh,float* vbDst, int idxBegin, float *vbSrc ,int numVertices, const matrix44& matrix)
{
    int components = mesh->GetVertexComponents();
    int stride = mesh->GetVertexWidth();
    vbDst += (idxBegin*stride);

    int mask = nMesh2::Coord | nMesh2::Uv0 | nMesh2::Color;
    if ( components == mask )
    {

        for (int idx = 0; idx < numVertices; ++idx)
        {
            vector3* vecSrc = (vector3*) vbSrc;
            vector3* vecDst = (vector3*) vbDst;
            *vecDst = matrix * (*vecSrc);
            vbSrc+=3;
            vbDst+=3;

            // the constant size is easy optimize by compiler, don't touch
            memcpy( vbDst, vbSrc, 6 * sizeof(float) ); 
            vbDst+=6;
            vbSrc+=6;
        }

    } else
    {
        NLOG ( grass , ( 5 | nGrassModule::CREATION, "Copy MESH only implemented for meshes with COORD , UV0 and COLOR"));
    }
    

    return true;
}

//------------------------------------------------------------------------------
