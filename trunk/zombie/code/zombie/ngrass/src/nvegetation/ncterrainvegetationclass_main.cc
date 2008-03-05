//------------------------------------------------------------------------------
//  ncterrainvegetationclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "precompiled/pchngrass.h"
#include "nvegetation/ncterrainvegetationclass.h"
#include "kernel/nkernelserver.h"
#include "ngrassgrowth/ngrowthmaterial.h"
#include "ngeomipmap/ncterraingmm.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nasset/nentityassetbuilder.h"
#include "nvegetation/ngrassmodule.h"

#include "nscene/nscenegraph.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ncterraingmmcell.h"
#include "nscene/ncsceneclass.h"
#include "ngeomipmap/ngeomipmapnode.h"
#include "nvegetation/ncterrainvegetationcell.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialquadtree.h"
#include "ngeomipmap/ncterrainmaterialclass.h"

#ifndef NGAME
#include "kernel/npersistserver.h"
#include "nvegetation/nvegetationbuilder.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndependencyserver.h"
#include "ngeomipmap/nbytemapbuilder.h"
#endif

//const int CACHEGROWTHSEEDSIZE = 9;
//const int CACHEVEGETATIONMESHSIZE =  36;
//const int CACHEGROWTHTEXTURESIZE = 1;
const int MAX_NO_OF_GROWTH_MATERIALS = 15;

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
bool ncTerrainVegetationClass::debugDisableDrawGrass(false);
bool ncTerrainVegetationClass::debugDisableDrawMesh(false);
bool ncTerrainVegetationClass::debugInfo(false);
#endif
#endif

//------------------------------------------------------------------------------
nNebulaComponentClass(ncTerrainVegetationClass,nComponentClass);

//------------------------------------------------------------------------------
/**
*/
ncTerrainVegetationClass::ncTerrainVegetationClass() :
    vegetationCellSubDivision(4) ,
    meshLibrary(0),
    meshNameLibrary(0),
    visibleSubCell(2),
    beginFadeFactor(0.5f),
    geometryFactorClipDistance(0.5f,1.f)
#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    ,
    growthMapSizeByCell(128),
    isInGrowthEdition(false),
    builder(0),
    growthEditionAplha( 0.5f )
#endif
#endif
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncTerrainVegetationClass::~ncTerrainVegetationClass()
{
}

//------------------------------------------------------------------------------
/**
    @param loaded is false when the object is a new instance
*/
void
ncTerrainVegetationClass::InitInstance(nObject::InitInstanceMsg initType)
{
    //loaded = loaded; // prevent warning
    
    // The iniInstance of editor components is called before others components
    /*
    if( loaded )
    {
        // Empty
    } else
    {
        // Empty
    }
    */
    if( initType == nObject::LoadedInstance )
    {
#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
        // The persistence of conjurer cmds is in other file , load it
        nString fileName = nEntityAssetBuilder::GetTerrainPath( this->GetEntityClass() , true );
        fileName += "ncterrainvegetationclass.n2";
        nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
        n_assert(ps);
        if( ps )
        {
            if( nFileServer2::Instance()->FileExists( fileName ) )
            {
                nKernelServer::Instance()->PushCwd( this->GetEntityClass() );
                nKernelServer::Instance()->Load( fileName.Get(), false );
                nKernelServer::Instance()->PopCwd();
            }
        }
#endif//!__ZOMBIE_EXPORTER__
#endif//!NGAME
    }

}

//------------------------------------------------------------------------------
/**
    @param value the number of subdivision of cell  for vegetation
*/
void
ncTerrainVegetationClass::SetVegetationCellSubDivision( int value)
{
    this->vegetationCellSubDivision = value;
}

//------------------------------------------------------------------------------
/**
    @param value the number of subdivision of cell  for vegetation
*/
int
ncTerrainVegetationClass::GetVegetationCellSubDivision() const
{
    return this->vegetationCellSubDivision;
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::SetGrowthMeshLibrarySize(int idx)
{
    idx = idx;
    meshLibrary.SetSize( idx );
    meshNameLibrary.SetSize( idx );
}

//------------------------------------------------------------------------------
/**
*/
int
ncTerrainVegetationClass::GetGrowthMeshLibrarySize() const
{
   return meshLibrary.Size();
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::SetGrowthMeshName(int idx, const char* name)
{
    meshNameLibrary[idx] = name;
    //idx = idx;
}

//------------------------------------------------------------------------------
/**
*/
const char*
ncTerrainVegetationClass::GetGrowthMeshName(int idx) const
{
    return meshNameLibrary[idx].Get();
}

//------------------------------------------------------------------------------
/**
*/
const nString 
ncTerrainVegetationClass::GetCellName(int bx, int bz) const
{
    nString name;
    name.Format("cell_%.2d_%.2d" , bx ,bz);
    return name;
}

//------------------------------------------------------------------------------
/**
*/
const nString
ncTerrainVegetationClass::GetGrowthSeedFileName(int bx, int bz) const
{
    nString name(this->GetCellName(bx,bz) );
    name += "_seeds.ngs";
    return nEntityAssetBuilder::GetTerrainPath( this->GetEntityClass() ) + name;
}

//------------------------------------------------------------------------------
/**
*/
nGrowthSeeds *
ncTerrainVegetationClass::GetGrowthSeeds(int bx, int bz)
{
    int key = nGrowthSeedCacheEntry::ToKey(bx , bz);
    ncTerrainMaterialClass* matClass = this->GetComponentSafe<ncTerrainMaterialClass>();
    nTerrainCellInfo*       cellInfo = matClass->GetTerrainCellInfo( bx , bz );

    if ( ( nTerrainCellInfo::HasGrass & cellInfo->GetFlags()  ) != 0 )
    {
        return  cacheGrowthSeed.Lookup( key , this->GetEntityClass() , true )->GetGrowthSeeds();
    } else
    {
        return 0;
    }
}


//------------------------------------------------------------------------------
/**
*/
nGrowthSeeds *
ncTerrainVegetationClass::GetGrowthIndex(const char* cmdString, int&  group) 
{
    
    n_assert(cmdString);
    int bx , bz , relx, relz ;
    SubCellId subCellId( atoi(cmdString) , this->GetVegetationCellSubDivision() );
    subCellId.Get(bx,bz, relx , relz );
    
    const int maxIdx = this->GetMaxSubCellIndexAbsolute();


    if ( subCellId.cell.subCellx < 0 || subCellId.cell.subCellx >= maxIdx ||  
         subCellId.cell.subCellz < 0 || subCellId.cell.subCellz >= maxIdx  
       )
    {
        return 0; // not has a subCellIndes
    }

    group = this->GetSubGroupIdx( relx, relz );
    return  this->GetGrowthSeeds(bx , bz) ;
}



//------------------------------------------------------------------------------
/**
*/
int 
ncTerrainVegetationClass::GetSubGroupIdx(int relx, int relz) const
{
    int div = this->GetMaxSubCellIndexRelative();
    return relz*div + relx;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::LoadMeshLibraryResources()
{
    n_assert( meshNameLibrary.Size() == meshLibrary.Size() ) 
    for ( int idx = 0; idx < meshNameLibrary.Size() ; ++idx)
    {
            // get a new or shared mesh
        nRef<nMesh2> mesh = meshLibrary[idx];

        n_assert( mesh.isvalid() )
        if (!mesh->IsLoaded())
        {
            mesh->SetFilename(meshNameLibrary[idx] );
            //mesh->SetUsage( this->GetMeshUsage() );
            if (!mesh->Load())
            {
                n_printf("nMeshNode: Error loading mesh '%s'\n", meshNameLibrary[idx].Get()  );
                return false;
            }
        }
    }
    return true;
}


//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::UnloadMeshLibraryResources()
{
    for ( int idx = 0; idx < meshLibrary.Size() ; ++idx)
    {
            // get a new or shared mesh
        nRef<nMesh2> mesh = meshLibrary[idx];
        n_assert(mesh);
        if ( mesh.isvalid() )
        {
            mesh->Unload();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::LoadResources()
{
    this->cacheGrowthSeed.Dealloc();
    this->cacheVegetationMesh.Dealloc();
    this->cacheGrowthSeed.Alloc( this->GetGrowthSeedCacheSize() );
    this->cacheVegetationMesh.Alloc( this->GetVegetationMeshCacheSize() );

    if ( meshNameLibrary.Size() >  0 )
    {
        meshLibrary.SetSize( meshNameLibrary.Size() );
        for ( int idx = 0; idx < meshNameLibrary.Size() ; ++idx)
        {
             // get a new or shared mesh
            nMesh2* mesh = nGfxServer2::Instance()->NewMesh(  meshNameLibrary[idx].Get() );
            n_assert(mesh);
            mesh->SetFilename(meshNameLibrary[idx]);
            mesh->SetUsage(nMesh2::ReadOnly);
            meshLibrary[idx] = mesh;
        }
    }
    return true;
}


//------------------------------------------------------------------------------
/**
*/
void
#if defined(NGAME) || defined(__ZOMBIE_EXPORTER__)
ncTerrainVegetationClass::UnloadResources()
#else
ncTerrainVegetationClass::UnloadResources(bool exitGrowthEdition)
#endif
{
    this->cacheGrowthSeed.Dealloc();
    this->cacheVegetationMesh.Dealloc();
    for ( int idx = 0; idx < meshLibrary.Size() ; ++idx)
    {
            // get a new or shared mesh
        nRef<nMesh2> mesh = meshLibrary[idx];
        n_assert(mesh);
        if ( mesh.isvalid() )
        {
            mesh->Release();
        }
    }
    meshLibrary.SetSize(0);


    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
        // The unload resource when unload level
        if ( exitGrowthEdition && this->IsInGrowthEdition() )
        {
            this->EndGrowtEdition(true);
        }
    #endif
    #endif
}

//------------------------------------------------------------------------------
/**
*/
const nString 
ncTerrainVegetationClass::GetSceneResourcePath() const
{
    nString fileName( nEntityAssetBuilder::GetScenePath( this->GetEntityClass() ) );
    return fileName + "grass.n2";
}

//------------------------------------------------------------------------------
/**
*/
nRoot* 
ncTerrainVegetationClass::GetLibSceneRoot()
{
    nRoot* libSceneRoot =  nKernelServer::Instance()->Lookup("/lib/grass");
    if (!libSceneRoot)
    {
        libSceneRoot = nKernelServer::Instance()->New( "nscenenode", "/lib/grass" )  ;
        n_assert( libSceneRoot);
    }
    return libSceneRoot;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::LoadScene()
{
    // initialize root node for scene resources
  
    nKernelServer* kernelServer = nKernelServer::Instance();
    nRoot* libSceneRoot( ncTerrainVegetationClass::GetLibSceneRoot() );


    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
    // if not exist create it
    if ( ! nFileServer2::Instance()->FileExists( this->GetSceneResourcePath() ) )
    {
        this->CreateSceneResource();
    }
    #endif
    #endif

            /// create a Nebula object from a persistent object file with given name
    kernelServer->PushCwd(libSceneRoot);
    this->vegetationNode = static_cast<nVegetationNode*>(
                                kernelServer->LoadAs( this->GetSceneResourcePath().Get(),  this->GetEntityClass()->GetName() ) 
                           );
    kernelServer->PopCwd();
    this->vegetationNode->SetVegetaionClass( this );
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::UnloadScene()
{
    if ( this->vegetationNode.isvalid() )
    {
        this->vegetationNode->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
nVegetationNode* 
ncTerrainVegetationClass::GetVegetationNode() const
{
    return  this->vegetationNode.isvalid() ? this->vegetationNode.get() : 0 ;
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncTerrainVegetationClass::IsValid()
{
    return true;
}


//------------------------------------------------------------------------------
/**
*/
int  
ncTerrainVegetationClass::GetMaxSubCellIndexRelative() const
{
    // the data is the same in all scene
    return 1 << ( this->GetVegetationCellSubDivision() );
}

//------------------------------------------------------------------------------
/**
*/
int  
ncTerrainVegetationClass::GetMaxSubCellIndexAbsolute() const
{
    // the data is the same in all scene
    int numSubCell = this->GetMaxSubCellIndexRelative();
    return numSubCell * ( this->GetComponentSafe< ncTerrainGMMClass>()->GetNumBlocks() );
}


//------------------------------------------------------------------------------
/**
*/
ncTerrainVegetationClass::SubCellId 
ncTerrainVegetationClass::GetSubCellIndex( float bx , float bz) const
{
    const ncTerrainGMMClass* gmmClass = this->GetComponentSafe< ncTerrainGMMClass>();
    int  div = this->GetMaxSubCellIndexAbsolute();
    float size= gmmClass->GetTotalSideSizeScaled();
    // The terrain size is (0,0) to (size, size)  and size > 0
    int valX = static_cast<int> ( n_floor ( ( bx / size ) * div ) ) ;
    int valZ = static_cast<int> ( n_floor ( ( bz / size ) * div ) ) ;

    /* return the invalid index is posible
    valX = n_max( 0 , n_min (div - 1, valX) );  // prevent the valx = div
    valZ = n_max( 0 , n_min (div - 1, valZ) );
    */
    

    return SubCellId( valX, valZ , this->GetVegetationCellSubDivision() );

}

//------------------------------------------------------------------------------
/**
*/
bool 
ncTerrainVegetationClass::IsValidSubCellIndex(const SubCellId& id) const
{
    const int maxIdx = this->GetMaxSubCellIndexAbsolute();
    if ( id.cell.subCellx < 0 || id.cell.subCellx >= maxIdx ||  
         id.cell.subCellz < 0 || id.cell.subCellz >= maxIdx  
       )
    {
        return false; // not has a subCellIndex
    } else
    {
        return true;
    }

}

//------------------------------------------------------------------------------
/**
*/
float 
ncTerrainVegetationClass::GetSubBlockSideSizeScaled() const
{
    const ncTerrainGMMClass* gmmClass = this->GetComponentSafe< ncTerrainGMMClass>();
    float  div = static_cast<float>( this->GetMaxSubCellIndexRelative() );
    float size= gmmClass->GetBlockSideSizeScaled();
    return size / div;
}


//------------------------------------------------------------------------------
/**
*/
nMesh2* 
ncTerrainVegetationClass::GetMeshForSubCell(const SubCellId& id)
{
    /*
    if ( this->isValidSubCellIndex(id) )
    {
        return 0;
    }
    */
    
    nVegetationMeshCacheEntry*  cache = this->cacheVegetationMesh.Lookup( id.id, this->GetEntityClass() , true );
    
    /// The block is with empty grass
    if ( cache->HasMesh() )
    {
        return cache->GetMesh();
    } else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
ncTerrainVegetationClass::LoadSceneResources()
{
    if ( !this->cacheVegetationMesh.IsAllocated() )
    {
        this->cacheVegetationMesh.Alloc( this->GetVegetationMeshCacheSize() );
    }

    if ( this->LoadMeshLibraryResources() )
    {
        return  this->GetVegetationNode()->LoadResources();
    } 
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::UnloadSceneResources()
{
   this->cacheVegetationMesh.Dealloc();
   this->GetVegetationNode()->UnloadResources();
   this->UnloadMeshLibraryResources();
}

//------------------------------------------------------------------------------
/**
    @brief prepare meshes for draw
*/
void 
ncTerrainVegetationClass::Attach(nSceneGraph *sceneGraph, nEntityObject *entityObject)
{

    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
    if (ncTerrainVegetationClass::debugDisableDrawGrass)
    {
        return;
    }
    #endif
    #endif


    // Atach the vegetaion node
    this->GetVegetationNode()->Attach(sceneGraph, entityObject);



    //!TODO temporal sollution for terrain without grass
    if (meshLibrary.Size() == 0 )
    {
        this->meshDrawList.Clear();
        return ;
    }

    // Get the camera positon
    vector3 cameraPos = sceneGraph->GetViewTransform().pos_component();


    SubCellId id0 = this->GetSubCellIndex( cameraPos.x , cameraPos.z);
    vector3 cameraDir =  - (sceneGraph->GetViewTransform().z_component());
    nCamera2 & camera = nGfxServer2::Instance()->GetCamera();


    // Calculate trinagle of vision
    float distanceClip = this->GetSubBlockSideSizeScaled() * this->GetNumSubBlockClip();
    vector2 dirXZ ( cameraDir.x , cameraDir.z);
    dirXZ.norm();

    vector2 tanXZ = vector2( dirXZ.y , -dirXZ.x);
    tanXZ *= distanceClip * n_tan( 0.5f * n_deg2rad( camera.GetAngleOfView() ) ) ;

    vector2 v0( cameraPos.x , cameraPos.z );
    vector2 v1( v0 + dirXZ*distanceClip +  tanXZ);
    vector2 v2( v0 + dirXZ*distanceClip -  tanXZ );

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    int countTriangles = 0;
    int validCells = 0;
#endif
#endif


    /// the id0 id1 id2 has a triangle
    this->VisibilityTriangle( v0 , v1 , v2);
    ncTerrainMaterialClass* matClass = this->GetComponentSafe<ncTerrainMaterialClass>();
    for ( int idx = 0 ; idx < this->meshDrawList.Size() ;  ++ idx )
    {
        int bx, bz;      
        SubCellMesh& subCellMesh = this->meshDrawList.At( idx);

        if ( this->IsValidSubCellIndex( subCellMesh.id ) )
        {
            subCellMesh.id.GetCellIndex( bx, bz);
            nTerrainCellInfo* cellInfo = matClass->GetTerrainCellInfo( bx , bz);  
            subCellMesh.mesh = this->GetMeshForSubCell( subCellMesh.id ) ;
            subCellMesh.cell = cellInfo->GetTerrainCell();
            
            subCellMesh.CalculateLod( sceneGraph , this );

        #ifndef NGAME
        #ifndef __ZOMBIE_EXPORTER__
            countTriangles += subCellMesh.numIndices ;
            validCells++;
        #endif
        #endif
        } else
        {
            // Other solution is clear this from array

            subCellMesh.mesh = 0;
            subCellMesh.cell = 0;
        }
    }

    n_assert( this->meshDrawList.Size() < this->GetVegetationMeshCacheSize() );

    this->meshDrawList.QSort( SubCellMesh::CompareByBlock);

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    if ( ncTerrainVegetationClass::debugInfo )
    {
        nString text("grass camera pos " );
        text = text + nString(cameraPos);
        nGfxServer2::Instance()->Text( text.Get(), vector4( 1, 0.8f, 0.8f, 0.8f), -0.99f, 0.20f );

        text = "grass camera dir ";
        text = text + nString(cameraDir);
        nGfxServer2::Instance()->Text( text.Get(), vector4( 1, 0.8f, 0.8f, 0.8f), -0.99f, 0.25f );

        text = "Camera Fov ";
        text = text + nString(camera.GetAngleOfView());
        nGfxServer2::Instance()->Text( text.Get(), vector4( 1, 0.8f, 0.8f, 0.8f), -0.99f, 0.30f );

        

        int x, z, relx ,relz;
        id0.Get(x, z , relx, relz );
        text =  "";
        text = text + "grass subcell pos x:" + x + " z:" +z + " relx:" + relx  + " relz:" + relz ;
        nGfxServer2::Instance()->Text( text.Get(), vector4( 1, 0.8f, 0.8f, 0.8f), -0.99f, 0.35f );


        text = "grass number of subcells ";
        text = text + nString(validCells);
        nGfxServer2::Instance()->Text( text.Get(), vector4( 0.8f, 0.8f, 1.f, 0.8f), -0.99f, 0.15f );

        text = "number of triangles ";
        text = text + nString(countTriangles / 3 );
        nGfxServer2::Instance()->Text( text.Get(), vector4( 0.8f, 0.8f, 1.f, 0.8f), -0.99f, 0.10f );


        text.Format("Last mesh generation time %f , number of meshes %d", nGrassModule::GetMeshGenerationTime().time , nGrassModule::GetMeshGenerationTime().count);
        nGfxServer2::Instance()->Text( text.Get(), vector4( 1, 0.0f, 0.8f, 0.8f), -0.99f, 0.05f );

        text.Format("Last load seed time %f , number of seed %d", nGrassModule::GetSeedLoadTime().time , nGrassModule::GetSeedLoadTime().count);;
        nGfxServer2::Instance()->Text( text.Get(), vector4( 1, 0.0f, 0.8f, 0.8f), -0.99f, 0.00f );


    }
#endif  
#endif

}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationClass::VisibilityTriangle( vector2 v1 , vector2 v2 , vector2 v3 )
{
    const ncTerrainGMMClass* gmmClass = this->GetComponentSafe< ncTerrainGMMClass>();
    int  div = this->GetMaxSubCellIndexAbsolute();
    float size= gmmClass->GetTotalSideSizeScaled();
    float toGrid = div / size;

    v1 *= toGrid; //For pass to itenger use n_floor
    v2 *= toGrid;
    v3 *= toGrid;
    this->meshDrawList.Clear();


    ///  v1.x < v2.x < v3.x
    if ( v2.x <  v1.x) // swap v2 with v1
    {
        vector2 tmp(v1);
        v1 = v2;
        v2 = tmp;
    }

    if ( v3.x <  v1.x) // swap v3 with v1
    {
        vector2 tmp(v1);
        v1 = v3;
        v3 = tmp;
    } 

    if ( v3.x <  v2.x) // swap v3 with v2
    {
        vector2 tmp(v3);
        v3 = v2;
        v2 = tmp;
    }


    bool isGreater_13; // line 13 > 12 and 13 > 23
    if ( v2.y != v3.y)
    {
        isGreater_13 = v2.y < v3.y; 
    } else
    {
        isGreater_13 = v2.y < v1.y; 
    }


    const float dy_13 = v3.y - v1.y;
    const float dy_12 = v2.y - v1.y;
    const float dy_23 = v3.y - v2.y;

    const float dx_13 = v3.x - v1.x;
    const float dx_12 = v2.x - v1.x;
    const float dx_23 = v3.x - v2.x;

    const float m_13 = dy_13 / dx_13;
    const float m_12 = dy_12 / dx_12;
    const float m_23 = dy_23 / dx_23;


    float y_13 =  v1.y - fmod( v1.x, 1.0f)*m_13; // Y position in Interseciton with left quad
    float y_12 =  v1.y - fmod( v1.x, 1.0f)*m_12;
    float y_23 =  v2.y - fmod( v2.x, 1.0f)*m_23;

    if ( isGreater_13 ) // get the worse case
    {
        y_13 = n_max( y_13 , y_13 + m_13 ); // compare Y intersection with left quad and right quad
        y_12 = n_min( y_12 , y_12 + m_12 );
        y_23 = n_min( y_23 , y_23 + m_23 );
    } else
    {
        y_13 = n_min( y_13 , y_13 + m_13 );
        y_12 = n_max( y_12 , y_12 + m_12 );
        y_23 = n_max( y_23 , y_23 + m_23 );
    }

    int min_y;
    int max_y;
    int y_13_int;
    int y_12_int;
    int y_23_int;
    const int x1 = static_cast<int> ( n_floor(v1.x));
    int x2 = static_cast<int> ( n_floor(v2.x));
    const int x3 = static_cast<int> ( n_floor(v3.x));
    const int y1 = static_cast<int> ( n_floor(v1.y));
    const int y2 = static_cast<int> ( n_floor(v2.y));
    const int y3 = static_cast<int> ( n_floor(v3.y));

    // if dx_23 == 0.f only draw left triangle
    // if dx_12 == 0.f only draw Right triangle
    if (  x2 == x3  )
    {
        // Only draw left triangle
        x2++;
    }

    // Left triangle
    SubCellMesh cellMesh;
    int x;
    for ( x = x1 ;  x < x2 ; ++x)
    {
        y_13_int = static_cast<int> ( n_floor( y_13) );
        y_13_int = n_max( n_min(y1,y3) , y_13_int ); // Inside of triangle , only is posible outside of triangle if x = x1 or x = x2, because it is the worse case.
        y_13_int = n_min( n_max(y1,y3) , y_13_int );

        y_12_int = static_cast<int> ( n_floor( y_12) );
        y_12_int = n_max( n_min(y1,y2) , y_12_int ); // Inside of triangle
        y_12_int = n_min( n_max(y1,y2) , y_12_int );

        if ( isGreater_13 )
        {
            max_y = y_13_int;
            min_y = y_12_int;
        } else
        {
            max_y = y_12_int;
            min_y = y_13_int;
        }

        for ( int y = min_y; y <=max_y ; ++y )
        {    
            cellMesh.id = SubCellId( x, y , this->GetVegetationCellSubDivision() );
            this->meshDrawList.Append(cellMesh);
        }

        y_12 += m_12;
        y_13 += m_13;
    }

    //Right triangle
    for ( ;  x <= x3; ++x)
    {
        y_13_int = static_cast<int> ( n_floor( y_13 ) );
        y_13_int = n_max( n_min(y1,y3) , y_13_int ); // Inside of triangle
        y_13_int = n_min( n_max(y1,y3) , y_13_int );

        y_23_int = static_cast<int> ( n_floor( y_23 ) );
        y_23_int = n_max( n_min(y2,y3) , y_23_int ); // Inside of triangle
        y_23_int = n_min( n_max(y2,y3) , y_23_int );

        if ( isGreater_13 )
        {
            max_y = y_13_int;
            min_y = y_23_int;
        } else
        {
            max_y = y_23_int;
            min_y = y_13_int;
        }

        for ( int y = min_y; y <=max_y ; ++y )
        {    
            cellMesh.id = SubCellId( x, y , this->GetVegetationCellSubDivision() );
            this->meshDrawList.Append(cellMesh);
        }

        y_23 += m_23; 
        y_13 += m_13;
    }
 
   
}


//------------------------------------------------------------------------------
/**
*/
const ncTerrainVegetationClass::MeshList&
ncTerrainVegetationClass::GetMeshes() const
{
    return this->meshDrawList;
}

//------------------------------------------------------------------------------
/**
*/
const nFixedArray<nRef<nMesh2> >& 
ncTerrainVegetationClass::GetMeshLibrary()
{
    return this->meshLibrary;
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationClass::SaveResources()
{
    #ifndef NGAME
    #ifndef __ZOMBIE_EXPORTER__
    this->SaveTerrainVegetationResources();
    #endif
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationClass::SetNumSubBlockClip( int val )
{
#ifndef NGAME
    val = n_max( val ,  1 );
    val = n_min( val ,  5 );
#endif
    this->visibleSubCell = val;

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    
    if ( this->cacheGrowthSeed.IsAllocated() )
    {
        this->cacheGrowthSeed.Dealloc();
        this->cacheGrowthSeed.Alloc( this->GetGrowthSeedCacheSize() );
    }

    if ( this->cacheVegetationMesh.IsAllocated() )
    {
        this->cacheVegetationMesh.Dealloc();
        this->cacheVegetationMesh.Alloc( this->GetVegetationMeshCacheSize() );
    }

#endif
#endif
}

//------------------------------------------------------------------------------
/**
*/
int
ncTerrainVegetationClass::GetNumSubBlockClip() const
{
    return this->visibleSubCell;
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::SetGrasBeginFadeFactor(float  beginFadeFactor) 
{
    #ifndef NGAME
    beginFadeFactor = n_max( beginFadeFactor ,  0.0f );
    beginFadeFactor = n_min( beginFadeFactor ,  1.0f );
    #endif
    this->beginFadeFactor = beginFadeFactor;
}

//------------------------------------------------------------------------------
/**
*/
float
ncTerrainVegetationClass::GetGrasBeginFadeFactor() const
{
    return this->beginFadeFactor;
}
//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::SetGeometryFactorClipDistance( const vector2&  geometryFactorClipDistance) 
{
    this->geometryFactorClipDistance = geometryFactorClipDistance;

    #ifndef NGAME
    //End
    this->geometryFactorClipDistance.y = n_min( this->geometryFactorClipDistance.y ,  1.2f );
    this->geometryFactorClipDistance.y = n_max( this->geometryFactorClipDistance.y ,  0.0001f );

    //Begin
    this->geometryFactorClipDistance.x = n_min( this->geometryFactorClipDistance.x ,  this->geometryFactorClipDistance.y -  0.0001f  );
    this->geometryFactorClipDistance.x = n_max( this->geometryFactorClipDistance.x ,  0.f );
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationClass::GetGeometryFactorClipDistance(vector2& geometryFactorClipDistance) const
{
    geometryFactorClipDistance = this->geometryFactorClipDistance;
}

//------------------------------------------------------------------------------
/**
*/
int
ncTerrainVegetationClass::GetVegetationMeshCacheSize() const
{
    int num = 2*this->visibleSubCell + 1;
    return num*num;
}

//------------------------------------------------------------------------------
/**
*/
int
ncTerrainVegetationClass::GetGrowthSeedCacheSize() const
{
    int subCellByBlock =  GetMaxSubCellIndexRelative();
    int cacheSubCell = this->visibleSubCell;
    int num =  cacheSubCell / subCellByBlock ;

    if (  0 != cacheSubCell % subCellByBlock  ) // ceil operation
    {
        num++;
    }
    num = 2*num + 1;
    return num*num;
}

//------------------------------------------------------------------------------
/**
*/
vector2
ncTerrainVegetationClass::GetGeometryClipDistance() const
{
    float distanceClip = this->GetSubBlockSideSizeScaled() * this->GetNumSubBlockClip();
    return this->geometryFactorClipDistance * distanceClip;
}


#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationClass::CreateSceneResource()
{
    nKernelServer* ks = nKernelServer::Instance();
    n_assert(ks);
    nRoot* libSceneRoot( ncTerrainVegetationClass::GetLibSceneRoot() );

    ks->PushCwd( libSceneRoot);
    nVegetationNode* veg = static_cast<nVegetationNode*>(ks->New("nvegetationnode" , this->GetEntityClass()->GetName() ) );
    ks->PopCwd();

    // Set the default material. the dependency server create copy of it
    veg->SetDependency( "wc:libs/system/materials/surface_grass.n2", 
                        "/sys/servers/dependency/nsurfacedependencyserver", 
                        "setsurface" );

    nDependencyServer::InitGlobal();
    veg->SaveAs( this->GetSceneResourcePath().Get() );
    nDependencyServer::InitGlobal();

    veg->Release();


    //@todo set the material
}

//------------------------------------------------------------------------------
/**
*/
const nString
ncTerrainVegetationClass::GetGrowthMapFileName(int bx, int bz) const
{
    nString name(this->GetCellName(bx,bz) );
    name += "_growth.tga";
    return nEntityAssetBuilder::GetTerrainPath( this->GetEntityClass() , true ) + name;
}

//------------------------------------------------------------------------------
/**
*/
const nString 
ncTerrainVegetationClass::GetGrowthMapFileName(nEntityObject* cell)
{
    ncTerrainGMMCell* gmmCell =  cell->GetComponentSafe<ncTerrainGMMCell>();
    return gmmCell->GetOutdoor()->GetClassComponentSafe<ncTerrainVegetationClass>()->GetGrowthMapFileName( gmmCell->GetCellX() , gmmCell->GetCellZ() );
}

//------------------------------------------------------------------------------
/**
*/
nObject*
ncTerrainVegetationClass::CreateGrowthMaterialList()
{
    nRoot* parent( this->GetRootNode() );
    nRoot* materialList;

    nKernelServer::Instance()->PushCwd(parent);
    materialList = nKernelServer::Instance()->New("nroot" , this->GetEntityClass()->GetName() );
    nKernelServer::Instance()->PushCwd(materialList);

    nGrowthMaterial* mat = static_cast<nGrowthMaterial*>(nKernelServer::Instance()->New("ngrowthmaterial" , "Default" )); // Create the defaultMaterial
    mat->SetColor( vector3( 0.0f, 0.0f , 0.0f )) ;

    nKernelServer::Instance()->PopCwd(); //this->GetEntityClass()->GetName() 
    nKernelServer::Instance()->PopCwd(); //"/lib/terrainGrowthMaterials"

    return materialList;


}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::SetGrowthMeshLibrary( const nArray<nString>& meshNameList)
{
    this->meshNameLibrary.SetSize( meshNameList.Size() );

    for ( int idx = 0; idx < meshNameList.Size() ; ++idx )
    {
        this->meshNameLibrary[idx] = meshNameList[idx];
    }
}


//------------------------------------------------------------------------------
/**
*/
nRoot* 
ncTerrainVegetationClass::GetGrowthMaterialList()
{
    nObject*  list;
    if ( ! refGrowthMaterialList.isvalid() )
    {
        nString noh(nEntityAssetBuilder::GetGrowthTerrrainMaterialNOH( this->GetEntityClass() ) ) ;
        list = nKernelServer::Instance()->Lookup( noh.Get() ) ; // Create the defaultMaterial
        if (!list)
        {
            list = this->LoadGrowthMaterialList();
            if (!list)
            {
                list = this->CreateGrowthMaterialList();
            }
        }
        refGrowthMaterialList = static_cast<nRoot*>(list);
    }
    return refGrowthMaterialList.get();
}

//------------------------------------------------------------------------------
/**
    @return true if there is an existing material with the given name, otherwise false
*/
bool
ncTerrainVegetationClass::HasGrowthMaterialWithName(const char* name)
{
    n_assert_return2(name, false, "name string is null");

    return ( this->GetIdOfGrowthMaterialWithName(name) != 0 );
}

//------------------------------------------------------------------------------
/**
    @return the id of the material with the given name if there is one, otherwise 0
*/
int
ncTerrainVegetationClass::GetIdOfGrowthMaterialWithName(const char* name)
{
    n_assert_return2(name, false, "name string is null");

    nRoot* materialList = this->GetGrowthMaterialList();
    n_assert_return2(materialList, false, "failed to get growth material list");

    nRoot* node = materialList->GetHead();
    n_assert_return2(node, false, "head of growth material list not found");

    for ( int i = 0; i < this->GetNumberOfGrowthMaterials(); i++ )
    {
        if ( strcmp (node->GetName(), name) == 0 )
        {
            return i;
        }
        node = node->GetSucc();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @return the newly created material, or 0 if the material was not created
*/
nGrowthMaterial*
ncTerrainVegetationClass::CreateGrowthMaterial(const char* name)
{
    n_assert_return2(name, false, "name string is null");

    nRoot* materialList = this->GetGrowthMaterialList();
    n_assert_return2(materialList, false, "failed to get growth material list");

    if ( (! materialList->Find( name ) ) && GetNumberOfGrowthMaterials() < MAX_NO_OF_GROWTH_MATERIALS )
    {
        nKernelServer::Instance()->PushCwd(materialList);
        nRoot* obj = nKernelServer::Instance()->New("ngrowthmaterial", name);
        nKernelServer::Instance()->PopCwd();

        if (obj)
        {
            return static_cast<nGrowthMaterial*>(obj);
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    @create a copy of the given growth material, using the name supplied
    @by default we delete any existing material with the same name first
    @return the new material if successful, otherwise 0
*/
nGrowthMaterial*
ncTerrainVegetationClass::AddCopyOfGrowthMaterial(nGrowthMaterial * originalMaterial, const char* name)
{
    n_assert_return2(originalMaterial && name, false, "material and/or name null");

    int index = this->GetIdOfGrowthMaterialWithName(name);

    if (index)
    {
        this->DeleteGrowthMaterial(index);
    }

    if ( GetNumberOfGrowthMaterials() < MAX_NO_OF_GROWTH_MATERIALS )
    {
        nRoot* materialList = this->GetGrowthMaterialList();
        n_assert_return2(materialList, false, "failed to get growth material list");

        nKernelServer::Instance()->PushCwd( materialList );
        nObject* obj = originalMaterial->Clone(name);
        nKernelServer::Instance()->PopCwd();

        if (obj)
        {
            return static_cast<nGrowthMaterial*>(obj);
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**

*/
nGrowthMaterial*
ncTerrainVegetationClass::GetGrowthMaterialById(int idx)  
{
    nRoot* node = this->GetGrowthMaterialList()->GetHead();
    for ( int i = 0; i < idx && node;  i++ )
    {
        node = node->GetSucc();
    }
    n_assert(node);
    return static_cast<nGrowthMaterial*>(node);
}

//------------------------------------------------------------------------------
/**
    @param idx index of material 1 to 15
*/
void
ncTerrainVegetationClass::DeleteGrowthMaterial(int idx )
{
    if ( idx > 0 ) // The default material is never deleted
    {
        nObject* material = this->GetGrowthMaterialById(idx);
        if ( material )
        {
            material->Release();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncTerrainVegetationClass::GetNumberOfGrowthMaterials()
{
    int count = 0;
    nRoot* node = this->GetGrowthMaterialList()->GetHead();
    while ( node)
    {
        node = node->GetSucc();
        ++count;
    }
    return count;
}

//------------------------------------------------------------------------------
/**
*/
nRoot* 
ncTerrainVegetationClass::LoadGrowthMaterialList()
{
    // Set parent as current  nroot and then load the materials
    nRoot* parent( this->GetRootNode() );
    nRoot* materialList(0);
    nString fileName = nEntityAssetBuilder::GetTerrainPath( this->GetEntityClass() , true );
    fileName += "growthmaterials.n2";

    if ( nFileServer2::Instance()->FileExists( fileName ) )
    {
        nKernelServer::Instance()->PushCwd(parent); 
        materialList = static_cast<nRoot*>( nKernelServer::Instance()->LoadAs( fileName.Get() , this->GetEntityClass()->GetName() ) );
        nKernelServer::Instance()->PopCwd();
    } else
    {
        // empty
    }

    return  materialList;

}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::SaveTerrainVegetationResources()
{
    bool result(false);
    nString pathName = nEntityAssetBuilder::GetTerrainPath( this->GetEntityClass() , true );
    nEntityAssetBuilder::MakeTerrainPath( this->GetEntityClass() , true );

    // First save the material list
    nString fileName(pathName);
    fileName += "growthmaterials.n2";
    nRoot* node = this->GetGrowthMaterialList();
    result = node->SaveAs(fileName.Get() );

    // After save the conjurer values
    fileName = pathName;
    fileName += "ncterrainvegetationclass.n2";
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);
    if( ps )
    {
        // change command to 'THIS'
        nCmd * cmd = ps->GetCmd( this->GetEntityClass(), 'THIS' );
        n_assert( cmd );
        if( cmd )
        {
            if( ps->BeginObjectWithCmd( this->GetEntityClass(), cmd, fileName.Get() ) ) 
            {
                // save commands
                this->SaveConjurerCmds( ps );
                ps->EndObject( true );
                result &= true;
            }
        }
    }


    // Save bytemaps
    nEntityObject* outdoor = nSpatialServer::Instance()->GetOutdoorEntity();

    nArray<ncSpatialQuadtreeCell*> & list= * (outdoor->GetComponentSafe<ncSpatialQuadtree>()->GetLeafCells() ) ;
    for ( int idx = 0; idx < list.Size() ; ++idx )
    {
        ncSpatialQuadtreeCell* cell = list[idx];
        ncTerrainVegetationCell* vegCell = cell->GetComponentSafe<ncTerrainVegetationCell>();
        vegCell->SaveEditionResources();
    }

    return result;
}

//------------------------------------------------------------------------------
/**
*/
nRoot* 
ncTerrainVegetationClass::GetRootNode()
{
    nString parentName( nEntityAssetBuilder::GetGrowthTerrrainMaterialNOH() );
    nRoot* parent = nKernelServer::Instance()->Lookup( parentName.Get() );
    if (!parent)
    {
        parent = nKernelServer::Instance()->New("nroot" , parentName.Get());
        n_assert(parent);
    }

    return parent;
}


//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::SetGrowthMapSizeByCell(int size)
{
    this->growthMapSizeByCell = size;
}
//------------------------------------------------------------------------------
/**
*/
int
ncTerrainVegetationClass::GetGrowthMapSizeByCell() const
{
    return this->growthMapSizeByCell;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::BeginGrowthEditon()
{
    if (isInGrowthEdition)
    {
        return false;
    }
    n_assert( this->builder == 0)
    this->isInGrowthEdition = true;
    this->builder = n_new( nVegetationBuilder );

    if (! this->builder->Begin( this->GetEntityClass() ) ) 
    {
        n_delete( this->builder);
        this->builder = 0;
        this->isInGrowthEdition = false;
        return false;
    }

    ncSceneClass * scene = this->GetComponentSafe<ncSceneClass>();
    nSceneNode * rootNode = scene->GetRootNode();
    if (rootNode->IsA("ngeomipmapnode"))
    {
        nGeoMipMapNode * gmmnode = static_cast<nGeoMipMapNode*> (rootNode);
        gmmnode->SetRenderGrassEdition( true );
    }

    this->cacheGrowthTexture.Dealloc();
    this->cacheGrowthTexture.Alloc( this->GetComponentSafe<ncTerrainMaterialClass>()->GetDetailCacheSize() );

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::BuildGrowth()
{
    if (!isInGrowthEdition)
    {
        return false;
    }
    n_assert( this->builder );
    return this->builder->Build();
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::BuildOnlyDirtyGrowth()
{
    if (!isInGrowthEdition)
    {
        return false;
    }
    n_assert( this->builder );
    return this->builder->Build( nVegetationBuilder::Dirty);
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::BuildGrassOnlySelected()
{
    if (!isInGrowthEdition)
    {
        return false;
    }
    n_assert( this->builder );
    return this->builder->Build( nVegetationBuilder::Selected );
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::GrassUpdateMeshes()
{
    nVegetationBuilder otherBuilder;
    return otherBuilder.UpdateMeshes(this->GetEntityClass() );
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::GrassUpdateHeight()
{
    nVegetationBuilder otherBuilder;
    return otherBuilder.UpdateHeight( this->GetEntityClass() );
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::IsInGrowthEdition() const
{
    return this->isInGrowthEdition;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncTerrainVegetationClass::EndGrowtEdition(bool save)
{
    if (!isInGrowthEdition)
    {
        return false;
    }
    n_assert(this->builder);
    this->builder->End(save);

    n_delete(this->builder);
    this->isInGrowthEdition = false;
    this->builder = 0;

    ncSceneClass * scene = this->GetComponentSafe<ncSceneClass>();
    nSceneNode * rootNode = scene->GetRootNode();
    if (rootNode && rootNode->IsA("ngeomipmapnode"))
    {
        nGeoMipMapNode * gmmnode = static_cast<nGeoMipMapNode*> (rootNode);
        gmmnode->SetRenderGrassEdition( false );
    }
    this->cacheGrowthTexture.Dealloc();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
nTexture2*
ncTerrainVegetationClass::GetGrowthTexture( int bx , int bz )
{
    nTerrainCellInfo * cellInfo = this->GetComponentSafe<ncTerrainMaterialClass>()->GetTerrainCellInfo( bx, bz );
    nEntityObject * cell = cellInfo->GetTerrainCell();

    int key = nGrowthSeedCacheEntry::ToKey(bx , bz);
    nGrowthTextureCacheEntry* entry = cacheGrowthTexture.Lookup( key , this->GetEntityClass() , true ) ;
    
    if ( cell )
    {
        ncTerrainVegetationCell* vegCell = cell->GetComponentSafe<ncTerrainVegetationCell>();
        if (vegCell->GetUpdateTextureBeforeRender() )
        {
            vegCell->UpdateTextureBeforeRender( false );
            nByteMapBuilder::FastCopyToTexture( entry->GetResource()  , vegCell->GetValidGrowthMap() );
        }
        return entry->GetResource();
    } else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationClass::GetTerrainGrassPalette(nArray<vector4>& colors)
{
    const int size = this->GetNumberOfGrowthMaterials();
    for ( int idx = 0; idx < size ; ++idx )
    {
        nGrowthMaterial* mat = this->GetGrowthMaterialById(idx);
        const vector3&   col = mat->GetColor();
        vector4 v4( col.x , col.y , col.z , growthEditionAplha );
        colors.Append(v4);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ncTerrainVegetationClass::SetGrowthEditionAlpha( float  val)
{
    this->growthEditionAplha =val;
}

//------------------------------------------------------------------------------
/**
*/
float 
ncTerrainVegetationClass::GetGrowthEditionAplha() const
{
    return this->growthEditionAplha;
}

#endif //#ifndef NGAME
#endif //#ifndef __ZOMBIE_EXPORTER__

//------------------------------------------------------------------------------
/**
*/
ncTerrainVegetationClass::SubCellId::SubCellId( int id , int desp )  :
    id(id),
    desp(static_cast<nint16>(desp))
{
}


//------------------------------------------------------------------------------
/**
*/
/*
ncTerrainVegetationClass::SubCellId::SubCellId( nuint16 x, nuint16 z, nuint8 relx, nuint8 relz, int desp ) :
    desp( desp )
{
    this->cell.subCellx =  (x <<  ( (nuint16)desp) ) + relx;
    this->subCellz = (z << desp) + relz;
}
*/

//------------------------------------------------------------------------------
/**
*/

ncTerrainVegetationClass::SubCellId::SubCellId( int subCellx, int subCellz, int desp) :
    desp( static_cast<nint16>(desp) )
{
    this->cell.subCellx= static_cast<nint16>(subCellx); 
    this->cell.subCellz=  static_cast<nint16>(subCellz);
}

//------------------------------------------------------------------------------
/**
*/

void
ncTerrainVegetationClass::SubCellId::Get( int& x, int& z , int& relx, int& relz ) const
{
    const nuint16 mask =  (1 << desp ) - 1;
    this->GetCellIndex(x,z);
    relx = cell.subCellx & ( mask);
    relz = cell.subCellz & ( mask);
}

//------------------------------------------------------------------------------
/**
*/
void
ncTerrainVegetationClass::SubCellId::GetCellIndex( int& x, int& z) const
{
    x = cell.subCellx >> desp;
    z = cell.subCellz >> desp;
}


//------------------------------------------------------------------------------
/**
*/

ncTerrainVegetationClass::SubCellId
ncTerrainVegetationClass::SubCellId::GetNeighbour( int x , int z) const
{
    return SubCellId( this->cell.subCellx  + static_cast<nint16>(x) , this->cell.subCellz + static_cast<nint16>(z) , this->desp );
}

//------------------------------------------------------------------------------
/**
*/
ncTerrainVegetationClass::SubCellId::SubCellId() :
    id( ~ static_cast<nuint32>(0) )
{
}


//------------------------------------------------------------------------------
/**
*/
ncTerrainVegetationClass::SubCellMesh::SubCellMesh() :
    mesh(0),
    cell(0)
{
}

//------------------------------------------------------------------------------
/**
*/
ncTerrainVegetationClass::SubCellMesh::~SubCellMesh()
{
    ///Empty
}

//------------------------------------------------------------------------------
/**
    Dertemine how triangles to draw. The mesh is sorted random.
    @param sceneGraph , the sceneGraph contains the camera
    @param geometryClipDistance x is the near plane begin clip ,and y the far plen where object not draw
*/
void
ncTerrainVegetationClass::SubCellMesh::CalculateLod(nSceneGraph *sceneGraph, ncTerrainVegetationClass* vegClass) 
{
    
    if ( this->mesh)
    {
        // Calculate the midle point of terrain subCell
        //const ncTerrainVegetationClass* vegClass = this->cell->GetComponentSafe(ncTerrainVegetationClassType);
        vector2 center (  float(this->id.cell.subCellx) , float(this->id.cell.subCellz) );
        center.x += 0.5f; // midle to subcellBox X
        center.y += 0.5f; // midle to subcellBox Z
        center = center * vegClass->GetSubBlockSideSizeScaled();
        float innerRadius = 0.5f*vegClass->GetSubBlockSideSizeScaled();

        //Calculate the distance to camera
        const vector3& cam = sceneGraph->GetViewTransform().pos_component();
        vector2 dir = vector2( center.x, center.y) - vector2( cam.x , cam.z);
        float distance = dir.len();
        distance = n_max ( 0 , distance - innerRadius ); // If the camera is in circle then distance is 0

        //Calculate the factor of triangles to draw
        const vector2 geometryClipDistance = vegClass->GetGeometryClipDistance();
        float factor =  (geometryClipDistance.y - distance) / ( geometryClipDistance.y - geometryClipDistance.x);
        factor = n_min(1.f , n_max(0.f,factor) ); //Clamp value to 0 .. 1 

        //Calculate the number of triangles to draw
        const nMeshGroup& curGroup = this->mesh->Group( 0 );
        float totalTriangles = curGroup.GetNumIndices() / 3.f; // 
        this->numIndices = (int)( factor * totalTriangles ); // The round before multiply by 3,  a Triangle more or less not important.
        this->numIndices *=3;                                     // But the number of index is multiple of 3 is very important,    
    } else
    {
        this->numIndices = 0;
    }
}


//------------------------------------------------------------------------------
/**
*/
int
__cdecl 
ncTerrainVegetationClass::SubCellMesh::CompareByBlock(const void* elm0, const void* elm1)
{
    const SubCellMesh* i0 = static_cast<const SubCellMesh*>(elm0);
    const SubCellMesh* i1 = static_cast<const SubCellMesh*>(elm1);

    int x0,z0,x1,z1;

    i0->id.GetCellIndex(x0,z0);
    i1->id.GetCellIndex(x1,z1);

    if ( x0 < x1) 
    {
        return -1;
    } else if ( x0 > x1)
    {
        return 1;
    }

    if ( z0 < z1) 
    {
        return -1;
    } else if ( z0 > z1) 
    {
        return 1;
    }

    return 0;
}


//------------------------------------------------------------------------------
