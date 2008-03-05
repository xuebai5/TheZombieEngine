#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterraineditorstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nautoref.h"
#include "conjurer/nterraineditorstate.h"
#include "conjurer/nconjurerapp.h"
#include "napplication/napplication.h"
#include "napplication/nappviewport.h"
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "kernel/ndirectory.h"
#include "kernel/nscriptserver.h"
#include "kernel/nlogclass.h"
#include "ngeomipmap/ngeomipmapnode.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/nterrainline.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/ncterraingmm.h"
#include "nscene/nsurfacenode.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/nspatialquadtreespacebuilder.h"
#include "nspatial/nhorizonsegmentbuilder.h"
#include "nspatial/nchorizonclass.h"
#include "conjurer/ninguiterraintoolgeom.h"
#include "conjurer/ninguiterraintoolflatten.h"
#include "conjurer/ninguiterraintoolslope.h"
#include "conjurer/ninguiterraintoolpaint.h"
#include "tools/ntexturebuilder.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "zombieentity/ncloaderclass.h"
#include "nscene/ncsceneclass.h"
#include "nmaterial/nmaterial.h"
#include "nmaterial/nmaterialnode.h"
#include "conjurer/nterrainlightmapbuilder.h"
#include "nvegetation/ncterrainvegetationclass.h"

//------------------------------------------------------------------------------
const char* terrainEditorPath = "/editor/terrain/";

const char* paintbrushesPath = "/editor/terrain/paintbrushes/";

const char* paintbrushesDir = "wc:libs/system/textures/paintbrushes/";

const int minimumBrushSize = 1;

const int maximumBrushSize = 999;

//------------------------------------------------------------------------------
nNebulaScriptClass(nTerrainEditorState, "neditorstate");

//------------------------------------------------------------------------------
NSIGNAL_DEFINE(nTerrainEditorState, PaintbrushSizeChanged);

//------------------------------------------------------------------------------
/**
*/
nTerrainEditorState::nTerrainEditorState():
    selectedPaintbrush( 0 ),
    paintbrushSize( 1 ),
    drawDebug( true ),
    currentInguiTool( 0 ),
    lightMapBuilder(0),
    debugLine(0)
{

    this->debugLine = static_cast<nTerrainLine*>(nKernelServer::Instance()->New( "nterrainline" ));

    // Creates heightmap for generating the predefined paintbrushes
    nString name = paintbrushesPath;
    name.Append("predefinedPaintbrush");
    predefinedPaintbrush = static_cast<nFloatMap*>(nResourceServer::Instance()->NewResource("nfloatmap", name.Get(), nResource::Other));
    n_assert(predefinedPaintbrush);
    this->predefinedPaintbrush->SetSize( paintbrushSize );
    this->predefinedPaintbrush->SetUsage( nFloatMap::CreateEmpty );
    this->predefinedPaintbrush->Load();

    // Creates heightmap for temporary operations
    name.Set( paintbrushesPath );
    name.Append( "tempPaintbrush" );
    tempPaintbrush = static_cast<nFloatMap*>(nResourceServer::Instance()->NewResource("nfloatmap", name.Get(), nResource::Other));
    n_assert(tempPaintbrush);
    this->tempPaintbrush->SetSize( paintbrushSize );
    this->tempPaintbrush->SetUsage( nFloatMap::CreateEmpty );
    this->tempPaintbrush->Load();

    // Second temp heightmap
    name.Set( paintbrushesPath );
    name.Append( "tempPaintbrush2" );
    tempPaintbrush2 = static_cast<nFloatMap*>(nResourceServer::Instance()->NewResource("nfloatmap", name.Get(), nResource::Other));
    n_assert(tempPaintbrush2);
    this->tempPaintbrush2->SetSize( paintbrushSize );
    this->tempPaintbrush2->SetUsage( nFloatMap::CreateEmpty );
    this->tempPaintbrush2->Load();

    GeneratePredefinedPaintbrush();

    // Initialize tool class reference objects
    terrainToolClass = nKernelServer::Instance()->FindClass("ninguiterraintool");
    terrainToolGeomClass = nKernelServer::Instance()->FindClass("ninguiterraintoolgeom");

    // Create tool objects

    name.Set( terrainEditorPath );
    name.Append( "/toolRaiseLow");
    inguiTerrainTool[ RaiseLow ] = static_cast<nInguiTerrainTool*>
                          (nKernelServer::Instance()->New("ninguiterraintoolraiselow", name.Get() ));
    ((nInguiTerrainToolGeom*)inguiTerrainTool[ RaiseLow ])->SetTempPaintbrushes( this->tempPaintbrush, this->tempPaintbrush2 );

    name.Set( terrainEditorPath );
    name.Append( "/toolFlatten");
    inguiTerrainTool[ Flatten ] = static_cast<nInguiTerrainTool*>
                          (nKernelServer::Instance()->New("ninguiterraintoolflatten", name.Get() ));
    ((nInguiTerrainToolGeom*)inguiTerrainTool[ Flatten ])->SetTempPaintbrushes( this->tempPaintbrush, this->tempPaintbrush2 );

    name.Set( terrainEditorPath );
    name.Append( "/toolSlope");
    inguiTerrainTool[ Slope ] = static_cast<nInguiTerrainTool*>
                          (nKernelServer::Instance()->New("ninguiterraintoolslope", name.Get() ));
    ((nInguiTerrainToolGeom*)inguiTerrainTool[ Slope ])->SetTempPaintbrushes( this->tempPaintbrush, this->tempPaintbrush2 );

    name.Set( terrainEditorPath );
    name.Append( "/toolSmooth");
    inguiTerrainTool[ Smooth ] = static_cast<nInguiTerrainTool*>
                          (nKernelServer::Instance()->New("ninguiterraintoolsmooth", name.Get() ));
    ((nInguiTerrainToolGeom*)inguiTerrainTool[ Smooth ])->SetTempPaintbrushes( this->tempPaintbrush, this->tempPaintbrush2 );

    name.Set( terrainEditorPath );
    name.Append( "/toolNoise");
    inguiTerrainTool[ Noise ] = static_cast<nInguiTerrainTool*>
                          (nKernelServer::Instance()->New("ninguiterraintoolnoise", name.Get() ));
    ((nInguiTerrainToolGeom*)inguiTerrainTool[ Noise ])->SetTempPaintbrushes( this->tempPaintbrush, this->tempPaintbrush2 );

    name.Set( terrainEditorPath );
    name.Append( "/toolPaint");
    inguiTerrainTool[ Paint ] = static_cast<nInguiTerrainTool*>
                          (nKernelServer::Instance()->New("ninguiterraintoolpaint", name.Get() ));
    ((nInguiTerrainToolGeom*)inguiTerrainTool[ Paint ])->SetTempPaintbrushes( this->tempPaintbrush, this->tempPaintbrush2 );

    name.Set( terrainEditorPath );
    name.Append( "/toolHole");
    inguiTerrainTool[ Hole ] = static_cast<nInguiTerrainTool*>
                          (nKernelServer::Instance()->New("ninguiterraintoolhole", name.Get() ));

    name.Set( terrainEditorPath );
    name.Append( "/toolGrass");
    inguiTerrainTool[ Grass ] = static_cast<nInguiTerrainTool*>
                          (nKernelServer::Instance()->New("ninguiterraintoolgrass", name.Get() ));
    ((nInguiTerrainToolGeom*)inguiTerrainTool[ Grass ])->SetTempPaintbrushes( this->tempPaintbrush, this->tempPaintbrush2 );

    // Initial tool state
    for ( int i = Smooth; i >= RaiseLow; i-- )
    {
        this->SelectTool( i );
        ((nInguiTerrainToolGeom*)currentInguiTool)->SetIntensity( 0.1f );
        ((nInguiTerrainToolGeom*)currentInguiTool)->SetDiameter( float( InitialPaintbrushSize ) );
    }

    // Tool initializations
    ((nInguiTerrainToolFlatten*)inguiTerrainTool[ Flatten ])->SetHeight(0.5f);
    ((nInguiTerrainToolFlatten*)inguiTerrainTool[ Flatten ])->SetAdaptiveIntensity(0.0f);

    ((nInguiTerrainToolSlope*)inguiTerrainTool[Slope])->SetSlope(45.0f);


    // Creates icons for the predefined paintbrushes
    this->SelectPaintbrush( 0 );
    this->SetPaintbrushSize( PaintbrushIconSize );
    for (int i=0; i < numPredefinedPaintbrushes; i++) 
    {
        SelectPaintbrush( i );
        nTexture2* tex = floatMapBuilder.GenerateTexture( predefinedPaintbrush, PaintbrushIconSize );
        n_assert( tex );
        predefinedPaintbrushIconList.Append( tex );
    }

    // Load paintbrushes at the constant directory and creates icons for them
    this->SetPaintbrushSize( PaintbrushIconSize );
    LoadPaintbrushes();

    // Save paintbrush icons (thumbnails) for use in the gui
    int n = this->PaintbrushCount();
    nTextureBuilder texBuilder;
    for (int i=0; i < n; i++) 
    {
        texBuilder.SetTexture( this->GetPaintBrushImage(i) );
        texBuilder.Save( this->GetPaintbrushThumbnail( i ).Get() );
    }

    // Set brush cursor shader
    this->lineDrawer.SetShaderPath("shaders:terrpol.fx");

}

//------------------------------------------------------------------------------
/**
*/
nTerrainEditorState::~nTerrainEditorState()
{
    if ( predefinedPaintbrush.isvalid() )
    {
        predefinedPaintbrush->Release();
    }
    if ( tempPaintbrush.isvalid() )
    {
        tempPaintbrush->Release();
    }
    if ( tempPaintbrush2.isvalid() )
    {
        tempPaintbrush2->Release();
    }
    if ( heightMapBuffer.isvalid() )
    {
        heightMapBuffer->Release();
    }
    
    for (int i = 0; i < NumTerrainTools; i++ )
    {
        inguiTerrainTool[ i ]->Release();
    }

    int n = this->userPaintbrushList.Size();
    for (int i = 0; i < n; i++)
    {
        this->userPaintbrushList[i]->Release();
    }

    n = this->predefinedPaintbrushIconList.Size();
    for (int i = 0; i < n; i++)
    {
        this->predefinedPaintbrushIconList[i]->Release();
    }

    n = this->userPaintbrushIconList.Size();
    for (int i = 0; i < n; i++)
    {
        this->userPaintbrushIconList[i]->Release();
    }

    thumbnailNames.Reset();

    this->debugLine->Release();
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainEditorState::OnStateEnter(const nString &prevState)
{
    nEntityObject * outdoor = this->GetOutdoorEntityObject();
    this->SetOutdoor( outdoor );

    nEditorState::OnStateEnter(prevState);
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainEditorState::OnStateLeave(const nString& /*nextState*/)
{
    this->currentInguiTool->SetState( nInguiTool::NotInited );

    this->DeselectCurrentTool();

    // free the outdoor object
    this->SetOutdoor(0);
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainEditorState::OnCreate(nApplication* application)
{
    nEditorState::OnCreate(application);
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainEditorState::OnRender2D()
{

    // Call parent's OnRender3D
    nEditorState::OnRender2D();
}

//------------------------------------------------------------------------------
/**
    @brief Create a new terrain class
    @param className Terrain class name
    @param heightMapSize size of the heightmap
    @param blockSize size of each block
    @param vegetationCellsPerBlock num of vegetation cells per terrain block
    @param vegetationCelSize resolution of one vegetation cell
    @param heightMapSize size of the weightmap
    @param globalTextureSize size of the whole texture
    @param gridScale map point separation
    @return Success. If false, the classname is not correct, is not unique, 
    or other parameter is incorrect
*/
bool
nTerrainEditorState::CreateTerrainClass(const char * className, int heightMapSize, int blockSize, int vegetationCellsPerBlock, int vegetationCellSize, int weightMapSize, int globalTextureSize, float gridScale )
{
    nEntityClassServer* entityServer = nEntityClassServer::Instance();

    nString clazzName( className);

    // Check class name
    if ( !entityServer->ValidClassName( className ) )
    {
        // Try to set the class name okay
        int n = clazzName.Length();
        if ( n > 0 )
        {
            clazzName[0] = (char) toupper( clazzName[0] ); 
            for (int i=1; i<n; i++)
            {
                clazzName[i] = (char) tolower( clazzName[i] ); 
            }
        }
        else
        {
            return false;
        }
    }

    if ( !entityServer->ValidClassName( className ) || heightMapSize <= 0 || weightMapSize <= 0 || blockSize <= 0 || globalTextureSize <= 0 || gridScale <= 0)
    {
        return false;
    }

    // Create terrain subclass
    nEntityClass* outdoorClass = entityServer->GetEntityClass("neoutdoor");
    n_assert(outdoorClass);
    nEntityClass* newClass = entityServer->NewEntityClass( outdoorClass, clazzName.Get() );
    n_assert(newClass);
    ncTerrainGMMClass* newClassGmmComp = newClass->GetComponent<ncTerrainGMMClass>();
    n_assert(newClassGmmComp);
    ncTerrainMaterialClass* newClassMaterialComp = newClass->GetComponent<ncTerrainMaterialClass>();
    n_assert(newClassMaterialComp);

    // Check that blockSize is smaller than heightMapSize / 2
    blockSize = min(blockSize, ( heightMapSize - 1 ) / 2 + 1);

    // Set the block size
    newClassGmmComp->SetBlockSize( blockSize );

    // Set asset resource file
    ncLoaderClass* ld = newClass->GetComponent<ncLoaderClass>();
    n_assert( ld );
    nString resFile = "wc:export/assets/" + clazzName;
    ld->SetResourceFile( resFile.Get() );

    // Create class resources
    bool succeed = true;

    // create scene path
    nString sceneFilename = resFile + "/scene";
    nFileServer2::Instance()->MakePath( sceneFilename.Get() );
    sceneFilename.Append("/");
    sceneFilename.Append(className);
    sceneFilename.Append(".n2");

    // Create geomipmap node
    nString objName = "/lib/scene/";
    objName += className;
    nGeoMipMapNode* gmmn = static_cast<nGeoMipMapNode*>( nKernelServer::Instance()->New("ngeomipmapnode", objName.Get()) );
    n_assert(gmmn);

    gmmn->SetDependency("wc:libs/system/materials/surface0_geomipmap.n2", "/sys/servers/dependency/nsurfacedependencyserver", "setsurface" );
    gmmn->nGeometryNode::LoadResources();
    // save scene to n2 and remove it from memory
    gmmn->SaveAs(sceneFilename.Get());
    gmmn->Release();

    // create directory path for source material (not neeeded in final app)
    nString sourceDir = resFile + "/source";
    nFileServer2::Instance()->MakePath( sourceDir.Get() );

    // create directory path for autogenerated material (needed in final app, but it can be rebuilt from source)
    nString autogenDir = resFile + "/autogen";
    nFileServer2::Instance()->MakePath( autogenDir.Get() );

    // create directory path for terrain asset 
    nString terrainDir = resFile + "/terrain";
    nFileServer2::Instance()->MakePath( terrainDir.Get() );

    // create directory path for terrain asset 
    nString texturesDir = resFile + "/textures";
    nFileServer2::Instance()->MakePath( texturesDir.Get() );

    // heightmap
    nFloatMap* hmap = static_cast<nFloatMap*>( nResourceServer::Instance()->NewResource("nfloatmap", newClassGmmComp->GetHeightMapPath().Get(), nResource::Other ) );
    n_assert( hmap );
    hmap->SetUsage( nFloatMap::CreateEmpty );
    hmap->SetSize( heightMapSize );
    hmap->SetGridScale( gridScale );
    hmap->SetFilename( newClassGmmComp->GetHeightMapPath() );
    succeed &= hmap->Load();
    succeed &= hmap->Save();

    // load resources (heightmap)
    succeed &= newClassGmmComp->LoadResources();

    // set global texture size and all weightmaps size
    newClassMaterialComp->SetGlobalTextureSize( globalTextureSize );
    newClassMaterialComp->SetAllWeightMapsSize( weightMapSize );

    // Add a starting blank layer, assigned to default texture and filled with 1.0
    nTerrainMaterial::LayerHandle layerHnd = newClassMaterialComp->CreateLayer();
    nTerrainMaterial * layer = newClassMaterialComp->GetLayerByHandle( layerHnd );
    n_assert(layer);
    layer->LoadResources();
    layer->SaveResources();
    layer->SetUVScale( vector2(1.0f,1.0f) );

    // generate resources
    succeed &= newClassMaterialComp->LoadResources();

    // save materials
    succeed &= newClassMaterialComp->SaveResources();//SaveTerrainMaterial();

    // release resources created
    hmap->Release();

    // Create n2 file, nvegetation node and set parameters
    ncTerrainVegetationClass * vegClassComp = newClass->GetComponentSafe<ncTerrainVegetationClass>();
    vegClassComp->SetVegetationCellSubDivision( vegetationCellsPerBlock );
    vegClassComp->SetGrowthMapSizeByCell( vegetationCellSize );
    vegClassComp ->CreateSceneResource();

    return succeed;
}

//------------------------------------------------------------------------------
/**
    @brief Set terrain parameters for the new terrain class to be created on
    call to CreateTerrainClass. Not enough parameters from scripting.
    @param Terrain entity class to instantiate
    @param minheight min height
    @param maxheight max height
    @return true if class name is valid, otherwise false
*/
bool
nTerrainEditorState::SetTerrainClassParams(nString className, float minHeight, float maxHeight, bool initHeight, float baseLevel )
{
    nEntityClassServer* entityServer = nEntityClassServer::Instance();

    if ( !entityServer->ValidClassName( className ) || maxHeight < minHeight )
    {
        return false;
    }

    // get terrain class
    nEntityClass* outdoorClass = entityServer->GetEntityClass(className.Get());
    if ( ! outdoorClass)
    {
        return false;
    }

    // get terrain geomipmap component
    ncTerrainGMMClass* newClassGmmComp = outdoorClass->GetComponent<ncTerrainGMMClass>();
    if ( ! newClassGmmComp )
    {
        return false;
    }

    // get heightmap, set parameters and save
    nFloatMap* hmap = static_cast<nFloatMap*>( nResourceServer::Instance()->FindResource(newClassGmmComp->GetHeightMapPath().Get(), nResource::Other ) );
    if ( hmap )
    {
        hmap->SetHeightOffset( minHeight );
        hmap->SetHeightScale( maxHeight - minHeight );

        // Initialize map height
        if ( initHeight )
        {
            if ( hmap )
            {
                baseLevel = min( hmap->GetHeightOffset() + hmap->GetHeightScale(), max( baseLevel, hmap->GetHeightOffset() ) );
                hmap->FillHeight( baseLevel );
            }
        }
        else
        {
            hmap->FillHeight( hmap->GetHeightOffset() );
        }

        hmap->Save();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Create a new terrain class
    @param Terrain entity class to instantiate
    @return Created entity object
*/
bool
nTerrainEditorState::CreateTerrainInstance( nString className )
{

    // Create terrain entity instance
    nEntityObject* terrainInstance = static_cast<nEntityObject*>( nEntityObjectServer::Instance()->NewEntityObject( className.Get() ) );
    if ( !terrainInstance )
    {
        return false;
    }

    // set name as "outdoor"
    nLevel * level = nLevelManager::Instance()->GetCurrentLevel();
    n_assert(level);
    level->AddEntity( terrainInstance );
    level->SetEntityName( terrainInstance->GetId(), "outdoor");

    // Set terrain instance as current terrain
    this->SetOutdoor( terrainInstance );

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Set path of heightmap to be edited
    @param hmap The heightmap path
*/
void 
nTerrainEditorState::SetOutdoor( nEntityObject * entityObject )
{
    if ( heightMapBuffer.isvalid() )
    {
        heightMapBuffer->Release();
    }
    if ( layerPaintFilter.isvalid() )
    {
        layerPaintFilter->Release();
    }
    if ( this->heightMap.isvalid() )
    {
        this->heightMap.invalidate();
    }

    // Set null outdoor to the terrain tools
    for (int i=0; i < NumTerrainTools; i++)
    {
        this->inguiTerrainTool[i]->SetOutdoor( 0 );
    }

    if ( ! entityObject )
    {
        return;
    }

    // get the heightmap
    nFloatMap* hmap = this->GetHeightMap();
    this->heightMap = hmap;

    // Creates heightmap buffer for undo/redo system
    heightMapBuffer = static_cast<nFloatMap*>(nResourceServer::Instance()->NewResource("nfloatmap", "heightMapBuffer", nResource::Other));
    n_assert( heightMapBuffer.isvalid() );
    heightMapBuffer->SetSize( heightMap->GetSize() );
    heightMapBuffer->SetUsage( nFloatMap::CreateEmpty );
    heightMapBuffer->Load();
    //And copy the heightmap into it
    floatMapBuilder.Copy(heightMap, 0, 0, heightMapBuffer);

    // Set heightmap path to the terrain tools
    for (int i=0; i < NumTerrainTools; i++)
    {
        this->inguiTerrainTool[i]->SetOutdoor( entityObject );
        this->inguiTerrainTool[i]->SetState( nInguiTool::NotInited );
        if ( this->inguiTerrainTool[i]->IsA( this->terrainToolGeomClass ) )
        {
            static_cast<nInguiTerrainToolGeom*>(this->inguiTerrainTool[i])->SetDiameter( float( InitialPaintbrushSize ) );
        }
    }

    // get the terrain geomipmap class component
    this->layerManager = entityObject->GetClassComponent<ncTerrainMaterialClass>();
    n_assert(this->layerManager);

    // Set previous selected paintbrush and size
    static_cast<nInguiTerrainToolGeom*>(this->inguiTerrainTool[selectedTool])->SetDiameter( float( this->GetPaintbrushSize() )  );

    // Debug
    debugLine->SetHeightMap( heightMap );
}

//------------------------------------------------------------------------------
/**
    @brief Tell if a heightmap has been set
    @return True if a heightmap has been set
*/
bool
nTerrainEditorState::IsHeightMapSet( void )
{
    return heightMap.isvalid();
}

//------------------------------------------------------------------------------
/**
    @brief Get reference to heightmap of the level
*/
nFloatMap*
nTerrainEditorState::GetHeightMap( void )
{
    // get entity object
    nEntityObject * object = this->GetOutdoorEntityObject();
    if (object)
    {
        nEntityClass* terrainClass = object->GetEntityClass();
        
        ncTerrainGMMClass* gmm = terrainClass->GetComponent<ncTerrainGMMClass>();
        n_assert( gmm );
        nFloatMap* hmap = gmm->GetHeightMap();
        return hmap;
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    @brief Import the terrain data from a texture file.
    @param path Input image file, full pathname with extension
    @return The heightmap path

    The file can be any type supported by nTexture2
*/
bool 
nTerrainEditorState::ImportHeightMap(const char * fileName)
{
    nFloatMap * imphm = static_cast<nFloatMap *> (nResourceServer::Instance()->NewResource("nfloatmap", fileName, nResource::Other));
    if (imphm)
    {
        nFloatMap* hmap = this->GetHeightMap();

        imphm->SetFilename(fileName);
        imphm->SetHeightOffset( hmap->GetHeightOffset() );
        imphm->SetHeightScale( hmap->GetHeightScale() );
        imphm->Load();

        if (imphm->IsValid())
        {
            n_assert(hmap);
            floatMapBuilder.Copy(imphm, 0, 0, hmap);

            nEntityObject * outdoor = this->GetOutdoorEntityObject();
            n_assert_if(outdoor)
            {
                ncTerrainGMM * tgmm = outdoor->GetComponent<ncTerrainGMM>();
                n_assert_return(tgmm,false);
                tgmm->UpdateGeometry(0, 0, hmap->GetSize(), hmap->GetSize());
            }
        }

        imphm->Release();

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Export the terrain data to a texture file.
    @param path Output image file, full pathname with extension
    @return success
*/
bool 
nTerrainEditorState::ExportHeightMap(const char * fileName)
{
    nFloatMap* hmap = this->GetHeightMap();
    if ( ! hmap ) 
    {
        return false;
    }

    nTexture2* tex = floatMapBuilder.GenerateTexture( hmap, hmap->GetSize()-1, 16 );
    bool success = tex->SaveResourceAs( fileName, tex->GetFormat() );

    tex->Release();
    return success;
}

//------------------------------------------------------------------------------
/**
    @brief Get number of paintbrushes
    @return Number of paintbrushes loaded
*/
int
nTerrainEditorState::PaintbrushCount(void) const
{
    return numPredefinedPaintbrushes + userPaintbrushList.Size();
}
//------------------------------------------------------------------------------
/**
    @brief Get file name of a paintbrush thumbnail
    @param brush_id Paintbrush identifier
    @return File name of the thumbnail associated to the given paintbrush id
*/
nString
nTerrainEditorState::GetPaintbrushThumbnail( int brush_id )
{
    nString fileName("outgui:images/terrain/");

    if ( brush_id < numPredefinedPaintbrushes )
    {
        fileName.Append("System");
    }
    else
    {
        fileName.Append("brushthumb");
    }
    fileName.AppendInt( brush_id );
    fileName.Append(".bmp");

    return fileName;
}
//------------------------------------------------------------------------------
/**
    @brief Select paintbrush to use
    @param paintbrush The paintbrush identifier
*/
void 
nTerrainEditorState::SelectPaintbrush( int pbrush )
{
    n_assert(pbrush >= 0 && pbrush < this->PaintbrushCount());

    this->selectedPaintbrush = pbrush;

    // Get a pointer to the newly selected paintbrush
    nFloatMap* curBrush;
    if ( pbrush < numPredefinedPaintbrushes )
    {
        // Predefined paintbrush
        curBrush = this->predefinedPaintbrush;
        // Check if it has to be resized
        if ( this->paintbrushSize != predefinedPaintbrush->GetSize() )
        {
            predefinedPaintbrush->Unload();
            predefinedPaintbrush->SetSize( this->paintbrushSize );
            predefinedPaintbrush->SetUsage( nFloatMap::CreateEmpty );
            predefinedPaintbrush->Load();
        }
    }
    else
    {
        // User paintbrush
        curBrush = userPaintbrushList[ selectedPaintbrush - numPredefinedPaintbrushes ];
    }

    // Check if the temporary heightmap has to be resized
    if (curBrush->GetSize() != tempPaintbrush->GetSize())
    {
        tempPaintbrush->Unload();
        tempPaintbrush->SetSize(curBrush->GetSize());
        tempPaintbrush->SetUsage( nFloatMap::CreateEmpty );
        tempPaintbrush->Load();
        
        tempPaintbrush2->Unload();
        tempPaintbrush2->SetSize(curBrush->GetSize());
        tempPaintbrush2->SetUsage( nFloatMap::CreateEmpty );
        tempPaintbrush2->Load();
    }

    // Now that the heightmaps have been resized, generate the selected paintbrush
    // if it is a predefined one
    if ( selectedPaintbrush < numPredefinedPaintbrushes )
    {
        if ( this->paintbrushSize == 1 ) {
            predefinedPaintbrush->SetHeightHC(0, 0, 1.0f);
        }
        else
        {
            GeneratePredefinedPaintbrush();
        }
    }
    // Actualize shape flag in the terrain paint tool object
    ((nInguiTerrainToolGeom*)this->currentInguiTool)->SetShapeSquared( ! ( this->selectedPaintbrush < numPredefinedPaintbrushes &&
                                                 this->selectedPaintbrush != paintbrushSquare ) );
}
//------------------------------------------------------------------------------
/**
  @brief Get selected paintbrush
  @return The selected paintbrush
*/
int
nTerrainEditorState::GetSelectedPaintbrush( void)
{
    return selectedPaintbrush;
}
//------------------------------------------------------------------------------
/**
    @brief  Tells if a paintbrush is affected by SetPaintBrushSize()
    @param paintbrush The paintbrush identifier
    @return Tells if the paintbrush is sizeable
*/
bool 
nTerrainEditorState::IsSizeablePaintbrush(int pbrush)
{
    n_assert(pbrush >= 0 && pbrush < this->PaintbrushCount());
    return pbrush < numPredefinedPaintbrushes;
}
//------------------------------------------------------------------------------
/**
    @brief  Sets the current size for the selected paintbrush. Does nothing if the paintbrush is not sizeable
    @param size The current size of the selected paintbrush
*/
void 
nTerrainEditorState::SetPaintbrushSize(int size)
{
    n_assert(size > 0);

    this->paintbrushSize = size;

    // If the selected paintbrush is a predefined one, check if it has to be resized
    if (selectedPaintbrush < numPredefinedPaintbrushes)
    {
        // Check if it has to be resized
        if (paintbrushSize != predefinedPaintbrush->GetSize())
        {
            predefinedPaintbrush->Unload();
            predefinedPaintbrush->SetSize( paintbrushSize );
            predefinedPaintbrush->SetUsage( nFloatMap::CreateEmpty );
            predefinedPaintbrush->Load();
        }
        // Check also if the temporary heightmap has to be resized
        if (paintbrushSize != tempPaintbrush->GetSize())
        {
            tempPaintbrush->Unload();
            tempPaintbrush->SetSize( paintbrushSize );
            tempPaintbrush->SetUsage( nFloatMap::CreateEmpty );
            tempPaintbrush->Load();
            
            tempPaintbrush2->Unload();
            tempPaintbrush2->SetSize( paintbrushSize );
            tempPaintbrush2->SetUsage( nFloatMap::CreateEmpty );
            tempPaintbrush2->Load();
        }

        // Generate it with the new size, except the case size = 1
        if ( this->paintbrushSize == 1 ) {
            predefinedPaintbrush->SetHeightHC(0, 0, 1.0f);
        }
        else
        {
            GeneratePredefinedPaintbrush();
        }
    

        // Change the diameter of the terrainTool object, if it's a geometry one
        if ( this->currentInguiTool->IsA( this->terrainToolGeomClass ) )
        {
            ((nInguiTerrainToolGeom*)this->currentInguiTool)->SetDiameter( float( size ) );
        }
    }
}
//------------------------------------------------------------------------------
/**
    @brief  Gets the current size of the paintbrush
    @return The current size of the paintbrush
*/
int
 nTerrainEditorState::GetPaintbrushSize(void)
{
    if ( selectedPaintbrush < numPredefinedPaintbrushes )
    {
        return paintbrushSize;
    }
    else
    {
        return userPaintbrushList[ selectedPaintbrush - numPredefinedPaintbrushes ]->GetSize();
    }
}
//------------------------------------------------------------------------------
/**
    @brief  Increases the size of the selected paintbrush. 
            Does nothing if it would mean making the brush 
            bigger than the maximum permitted size
*/
void 
nTerrainEditorState::MakePaintbrushBigger()
{
    int currentSize = this->GetPaintbrushSize();

    if (currentSize < maximumBrushSize)
    {
        this->SetPaintbrushSize(currentSize + 1);
    
        this->SignalPaintbrushSizeChanged(this, this->GetPaintbrushSize() );
    }
}
//------------------------------------------------------------------------------
/**
    @brief  Decreases the size of the selected paintbrush. 
            Does nothing if it would mean making the brush 
            smaller than the minimum permitted size
*/
void 
nTerrainEditorState::MakePaintbrushSmaller()
{
    int currentSize = this->GetPaintbrushSize();

    if (currentSize > minimumBrushSize)
    {
        this->SetPaintbrushSize(currentSize - 1);

        this->SignalPaintbrushSizeChanged(this, this->GetPaintbrushSize() );
    }
}//------------------------------------------------------------------------------
/**
    @brief  Import paintbrushes from texture files in a default directory
    @return Number of paintbrushes loaded

    The 'paintbrushesDir' constant is the directory to load the images from.
    The 'paintbrushesPath' constant is the NOH path to put the heightmap paintbrushes to.
    
    All previous references to nTexture2 objects obtained with GetPaintbrushImage() are invalid,
    since all the icons are deleted and recreated with the new file/s in the directory.
*/
int
nTerrainEditorState::LoadPaintbrushes( void )
{
    int numPbLoaded = 0;
    static bool first = true;

    // Delete current user paintbrushes and icons
    int n = userPaintbrushList.Size();
    for (int i=0; i < n; i++)
    {
        userPaintbrushList[ i ]->Release();
        userPaintbrushIconList[ i ]->Release();
    }
    userPaintbrushList.Reset();
    userPaintbrushIconList.Reset();
    thumbnailNames.Reset();

    // Scan the directory for file entries and try to load every one
    nFileServer2* fileserv = nFileServer2::Instance();
    n_assert( fileserv );
    nDirectory *d = fileserv->NewDirectoryObject();
    
    bool moreFiles = d->Open( paintbrushesDir );

    if ( moreFiles )
    {
        moreFiles = d->SetToFirstEntry();
    }

    n_assert2( moreFiles, "Couldn't open paintbrush directory");
    if ( ! moreFiles )
    {
        NLOG(resource, (NLOGUSER | 0, "Couldn't open paintbrush directory '%s'", nFileServer2::Instance()->ManglePath(paintbrushesDir)) );
        return 0;
    }

    nFloatMap* curmap = NULL;

    while ( moreFiles )
    {
        if ( d->GetEntryType() == nDirectory::FILE )
        {
            nString fname( d->GetEntryName() );

            curmap = static_cast<nFloatMap*>(nResourceServer::Instance()->NewResource("nfloatmap", fname.Get(), nResource::Other));
            
            n_assert(curmap);
            curmap->SetFilename( fname.Get() );
            if ( curmap->Load() )
            {
                userPaintbrushList.Append( curmap );
                // Create new icon texture and append it to paintbrushIconList
                nTexture2* tex = floatMapBuilder.GenerateTexture( curmap, PaintbrushIconSize );
                n_assert( tex);
                userPaintbrushIconList.Append( tex );

                thumbnailNames.Append( fname );

                numPbLoaded ++;
            }
        }
        moreFiles = d->SetToNextEntry();
    }
    d->Close();
    n_delete(d);

    if ( selectedPaintbrush >= this->PaintbrushCount() )
    {
        if ( numPbLoaded > 0 )
        {
            SelectPaintbrush( numPredefinedPaintbrushes );
        }
        else
        {
            SelectPaintbrush( 0 );
        }
    }

    first = false;

    return numPbLoaded;
}


//------------------------------------------------------------------------------
/**
    @brief Get an image of a paintbrush
    @param paintbrush Identifier of the paintbrush. 0 <= paintbrush <= PaintbrushCount()
    @return A pointer to the nTexture2. This should not be released by the caller, will
    be released by the module when needed. Returns 0 if the heightmap was not inited.
    Furthermore, calling LoadPaintbrushes() makes the pointers returned by this function invalid,
    since the textured are regenerated.
*/
nTexture2*
nTerrainEditorState::GetPaintBrushImage( int paintbrush )
{
    n_assert( paintbrush >= 0 && paintbrush < this->PaintbrushCount() );

    if ( paintbrush < numPredefinedPaintbrushes )
    {
        return predefinedPaintbrushIconList[ paintbrush ];
    }
    else
    {
        return userPaintbrushIconList[ paintbrush - numPredefinedPaintbrushes ];
    }
}
//------------------------------------------------------------------------------
/**
    @brief Select tool to use
    @param tool The tool
*/
void 
nTerrainEditorState::SelectTool(int tool)
{

    n_assert(tool >= 0 && tool < NumTerrainTools);
    if ( this->selectedTool == tool )
    {
        return;
    }
        
    this->selectedTool = tool;

    if ( this->currentInguiTool )
    {
        this->currentInguiTool->OnDeselected();
        this->currentInguiTool->SetState( nInguiTool::NotInited );
    }
    
    this->currentInguiTool = this->inguiTerrainTool[ selectedTool ];
    nInguiTerrainToolGeom* toolGeom = static_cast<nInguiTerrainToolGeom*>(this->currentInguiTool);

    if ( this->currentInguiTool && this->currentInguiTool->IsA( this->terrainToolGeomClass ) &&
        toolGeom->GetDiameter() != 0 )
    {
        this->SetPaintbrushSize( int( toolGeom->GetDiameter() ) );
        toolGeom->SetDiameter( float( this->GetPaintbrushSize() ) );

        bool isShapeSquared = !(this->selectedPaintbrush < numPredefinedPaintbrushes && 
                                this->selectedPaintbrush != paintbrushSquare); 

        toolGeom->SetShapeSquared( isShapeSquared );
        this->currentInguiTool->SetState( nInguiTool::NotInited );
        this->currentInguiTool->OnSelected();
    }    

    this->firstPoint = vector3(0.0f, 0.0f, 0.0f);
    this->lastPoint = vector3(0.0f, 0.0f, 0.0f);

}
//------------------------------------------------------------------------------
/**
    @brief Get the currently selected tool
    @return Tthe selected tool
*/
int
nTerrainEditorState::GetSelectedTool( void )
{
    return selectedTool;
}

//------------------------------------------------------------------------------
/**
    @brief Get height and slope info about a point in the terrain
    @param pos 3d point for obtaining the info (Y coord not used)
    @param dist Returned terrain height at pos
    @param slope Returned slope of terrain at pos
    @return True if success, false if there is no terrain, or the point lies outside of it
*/
bool
nTerrainEditorState::GetHeightSlope(vector3 pos, float& height, float& slope)
{
    if ( ! this->heightMap )
    {
        return false;
    }

    float c;
    vector3 n;    
    if ( ! this->heightMap->GetHeightNormal(pos.x, pos.z, height, n) )
    {
        return false;
    }
    if ( abs(n.y) < TINY )
    {
        c = 0.0f;
    }
    else
    {
        c = sqrt( n.x * n.x + n.z * n.z ) / n.y;
    }
    slope = 90.0f - n_rad2deg( float( atan( c ) ) );

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Generate the selected predefined paintbrush

    Called from SelectPaintbrush() if a predefined paintbrush was selected
*/
void
nTerrainEditorState::GeneratePredefinedPaintbrush()
{  
    n_assert(selectedPaintbrush >= 0 && selectedPaintbrush < numPredefinedPaintbrushes);

    switch (selectedPaintbrush)
    {
        case paintbrushSquare:
            predefinedPaintbrush->FillHeight(1.0f);
            break;
        case paintbrushCircle:
            FillWithFunc2D(predefinedPaintbrush, circle);
            break;
        case paintbrushHat:
            FillWithFunc2D(predefinedPaintbrush, cosinus);
            break;
        case paintbrushCone:
            FillWithFunc2D(predefinedPaintbrush, distance);
            break;
    }
}

//------------------------------------------------------------------------------
/**
	@brief Fill a heightmap with a plane
    @param hmap The heightmap to be filled
    @param normal Normal of the plane, in HC
    @param h0 height of the plane at the central point of the heightmap, in HC

*/
void
nTerrainEditorState::FillWithPlane(nFloatMap* hmap, vector3 normal, float h0)
{

    if ( abs( normal.x ) < TINY || abs( normal.y ) < TINY || abs( normal.z ) < TINY )
    {
        hmap->FillHeight( h0 );
        return;
    }
    
    int size = hmap->GetSize();

    float dx = - normal.x / normal.y;
	float dz = - normal.z / normal.y;

    float x, z;
	z = - size / 2.0f;
	for (int j = 0; j < size; j++)
	{
		x = - size / 2.0f;
		for (int i = 0; i < size; i++)
		{
			float h = dx * x + dz * z + h0;

            hmap->SetHeightHC( i, j, h );
			x += 1.0f;
		}
		z += 1.0f;
	}
}

//------------------------------------------------------------------------------
/**
	@brief Fill a heightmap with a 2D function
    @param hmap The heightmap to be filled
    @param func The 2D function used to filled it. It will be sampled from [-1, -1] to [1, 1]
    and should return values in [0,1]

*/
void
nTerrainEditorState::FillWithFunc2D(nFloatMap* hmap, HeightmapFunc2D func)
{
    int size = hmap->GetSize();
	float dx = 2.0f / size ;
	float dy = 2.0f / size ;

    float x, y;

	y = -1.0;
	for (int j = 0; j < size; j++)
	{
		x = -1.0;
		for (int i = 0; i < size; i++)
		{
			float h = (*func)(x,y);

            hmap->SetHeightHC(i, j, h);
			x += dx;
		}
		y += dy;
	}
}

float cubic(float x, float y)
{
    float h;
    float absx = abs(x) * 2.0f;
	float absx2 = absx * absx;           
    if (absx < 1.0f)
	{
		h = 0.5f * absx2*absx - absx2 + 2.0f/3.0f;
	}
	else if (absx < 2.0)
	{
		h = - 1.0f/6.0f * absx2*absx + absx2 - 2 * absx + 4.0f/3.0f;
	}
	else
	{
		h = 0;
	}

    float absy = abs(y) * 2.0f;
	float absy2 = absy * absy;
	if (absy < 1.0f)
	{
		h *= 0.5f * absy2*absy - absy2 + 2.0f/3.0f;
	}
	else if (absy < 2.0f)
	{
		h *= - 1.0f/6.0f * absy2*absy + absy2 - 2 * absy + 4.0f/3.0f;
	}
	else
	{
		h *= 0.0f;
	}
    return h;
}

float distance(float x, float y)
{
    float h = 1.0f - sqrt ( x * x + y * y );
    return max(0, h);
}

float circle(float x, float y)
{
    float h = 1.0f - pow( x * x + y * y, float(5.5) );
    return max(0, h);
}

float cosinus(float x, float y)
{
    float h = sqrt ( x * x + y * y );
    if ( h >= 1.0f) {
        return 0.0f;
    }

    return 0.5f * cos ( h * PI ) + 0.5f;

}
float noise(float /*x*/, float /*y*/)
{
    return float( rand() ) / float( RAND_MAX ) - 0.5f;
}
//------------------------------------------------------------------------------
/** Draw debug visualization
*/
void
nTerrainEditorState::DrawDebug( nAppViewport* vp )
{
    n_assert( vp );

    if ( ! heightMap.isvalid() )
    {
        return;
    }

    //if (!drawDebug || !nConjurerApp::Instance()->IsActiveModule("TERRAIN"))
    if (!drawDebug || nApplication::Instance()->GetCurrentState() != nString("terrain"))
    {
        return;
    }

    //nFloatMap* hmap = heightMap;

    // Get viewport and camera info
    //nGfxServer2* refGfxServer = nGfxServer2::Instance();
    nCamera2 camera = vp->GetCamera();

    // Sets vp in gfxserver
    //FIXME
    //vp->SetInGfxServer();

    // Draw the current tool
    this->currentInguiTool->Draw( vp, &camera );

    //--- this is old debugging ---

    // draw the debugLine points
/*    matrix44 m;
    vector3 scale(0.5f,0.5f,0.5f);
    m.scale( scale );
    int np = debugLine.GetNumVertices();
    vector3* lineVertices = debugLine.GetVertexBuffer();

    refGfxServer->BeginShapes();
    for (int i=0; i < np; i++)
    {
        m.set_translation( lineVertices[ i ] );
        refGfxServer->DrawShape( nGfxServer2::Box, m, vector4(0.0f, 0.0f, 0.0f, 1.0f) );
    }
    refGfxServer->EndShapes();

    // Draw debugLine
    if ( np > 1)
    {        
        / *refGfxServer->BeginLines();
        refGfxServer->DrawLines3d( lineVertices, np, vector4(1.0f, 1.0f, 1.0f, 1.0f));
        refGfxServer->EndLines();* /

        m.ident();
        refGfxServer->SetTransform(nGfxServer2::Model, m);

        vector4 c(0.8f, 0.3f, 1.0f, 1.0f);
        this->lineDrawer.DrawExtruded3DLine( debugLine.GetVertexBuffer(), debugLine.GetNumVertices(), &c, 1, vector3(0.0f, 3.0f, 0.0f), true );

    }

    // draw the generated polygons
    refGfxServer->SetTransform(nGfxServer2::Model, matrix44());

    for (int i=0; i<debugLine.debugPolygons.Size(); i++)
    {
        refGfxServer->SetMesh( debugLine.debugPolygons[ i ] );
        refGfxServer->SetVertexRange(0, debugLine.debugPolygons[ i ]->GetNumVertices());
		refGfxServer->SetIndexRange(0, debugLine.debugPolygons[ i ]->GetNumIndices());
        // Set shader variable "diffMap" to our texture, need for texture version
        //refShader->SetTexture(nShaderState::DiffMap0, refTexture);
		refGfxServer->SetShader(debugLine.debugShader);

		// Draw our mesh with selected shader and texture
        refGfxServer->DrawIndexed(nGfxServer2::TriangleList);
    }
    //--- end of old debugging ---
    */
}

//------------------------------------------------------------------------------
/** 
*/
void
nTerrainEditorState::CreateTerrainLightMaps(const char* lightMapClassName, int lightMapSize, int shadowMapSize, int globalLightMapSize, nEntityObjectId lightId, bool overwriteExistingFiles , const float distance, const vector2 offset, const float diskSize)
{
    this->lightMapBuilder = n_new( nTerrainLightMapBuilder(lightMapClassName, lightMapSize, shadowMapSize, globalLightMapSize, lightId, distance, offset,diskSize) );

    if (this->app->GetCurrentState() == "object")
    {
        nObjectEditorState* objectEditorState = static_cast<nObjectEditorState*>(this->app->FindState("object"));
        //if (objectEditorState->GetSelectedTool()->IsA("ninguitoolselection"))
        this->lightMapBuilder->SetSelectedCells(objectEditorState->GetSelection());
    }

    this->lightMapBuilder->SetOverwriteFiles(overwriteExistingFiles);

    this->lightMapBuilder->CreateLightEnvironmentClass();

    this->lightMapBuilder->CreateLightEnvironmentObject();

    n_delete( this->lightMapBuilder);
}

//------------------------------------------------------------------------------
/** 
*/
bool
nTerrainEditorState::CreateTerrainGlobalLightMap(const char* lightMapClassName, int globalLightMapSize )
{
    bool result;
    this->lightMapBuilder = n_new( nTerrainLightMapBuilder() );
    n_assert( this->lightMapBuilder );

    // Update only for selected cells, if not has selected cell then the builder update all cells
    if (this->app->GetCurrentState() == "object")
    {
        nObjectEditorState* objectEditorState = static_cast<nObjectEditorState*>(this->app->FindState("object"));
        this->lightMapBuilder->SetSelectedCells(objectEditorState->GetSelection());
    }

    result = this->lightMapBuilder->GenerateGlobalLightMap( lightMapClassName, globalLightMapSize );
    n_delete(this->lightMapBuilder );
    return result;

}

//------------------------------------------------------------------------------
/**
    Get the size of a user paintbrush

*/
void
nTerrainEditorState::CreateTerrainGlobalTexture()
{
    nEntityObject * outdoor = this->GetOutdoorEntityObject();
    if (!outdoor)
    {
        return;
    }

    ncTerrainMaterialClass * tmc = outdoor->GetClassComponent<ncTerrainMaterialClass>();
    if (!tmc)
    {
        return;
    }
    
    // generate the global texture
    int maxblock = outdoor->GetClassComponentSafe<ncTerrainGMMClass>()->GetNumBlocks() - 1;
    nTerrainGlobalTextureBuilder globalTextureBuilder;
    globalTextureBuilder.SetTerrainMaterialComponent(tmc);
    globalTextureBuilder.GenerateGlobalTexture(0, 0, maxblock, maxblock);

    // save the global texture to disk
    nTextureBuilder texBuilder;
    texBuilder.SetTexture(tmc->GetGlobalTexture());
    texBuilder.Save(tmc->GetGlobalTexturePath().Get());
}

//------------------------------------------------------------------------------
/** 
*/
void
nTerrainEditorState::CreateTerrainWeightmaps()
{
    nTerrainWeightMapBuilder weightMapBuilder;

    nEntityObject * outdoor = this->GetOutdoorEntityObject();
    if (!outdoor)
    {
        return;
    }

    ncTerrainMaterialClass * tmc = outdoor->GetClassComponent<ncTerrainMaterialClass>();
    if (!tmc)
    {
        return;
    }

    ncTerrainGMMClass * tgmmc = outdoor->GetClassComponent<ncTerrainGMMClass>();
    if (!tmc)
    {
        return;
    }

    // create new texture
    nTexture2 * weightmapTexture = tmc->NewWeightMapTexture(0);

    // setup a texture builder, used to save the texture in DDS format
    nTextureBuilder texBuilder;
    texBuilder.SetTexture( weightmapTexture );

    // setup a weightmap builder
    weightMapBuilder.SetTerrainMaterialComponent( tmc );
    weightMapBuilder.SetWeightTexture( weightmapTexture );

    // for all the blocks calculate the weightmap
    for(int bz = 0;bz < tgmmc->GetNumBlocks();bz++)
    {
        for(int bx = 0;bx < tgmmc->GetNumBlocks ();bx++ )
        {
            nTerrainCellInfo * cellInfo = tmc->GetTerrainCellInfo(bx, bz);
            n_assert(cellInfo);

            cellInfo->CreateByteMaps(false);
            cellInfo->UpdateFlags();

            if ( cellInfo->NeedToSaveWeightMap() )
            {
                // generate the weightmap
                weightMapBuilder.Generate( bx, bz );

                // save the global texture to disk
                texBuilder.Save( tmc->GetWeightMapTexturePath(bx, bz).Get() );
                

            }
        }
    }
    
    // release resources
    weightmapTexture->Release();
}

//------------------------------------------------------------------------------
/**
    Return current paintbrush size if brush is sizeable
*/
int
nTerrainEditorState::GetPaintbrushSizeByIndex( int index ) const
{
    n_assert( index >= 0 && index < this->PaintbrushCount() );

    if ( index < this->numPredefinedPaintbrushes )
    {
        return this->paintbrushSize;
    }
    else
    {
        return this->userPaintbrushList[ index - this->numPredefinedPaintbrushes ]->GetSize();
    }
}


//------------------------------------------------------------------------------
nEntityObject *
nTerrainEditorState::GetOutdoorEntityObject() const
{
    // get current level from the level manager
    nLevel * level = nLevelManager::Instance()->GetCurrentLevel();
    if (!level)
    {
        return 0;
    }

    nEntityObjectId oid = level->FindEntity("outdoor");
    if (oid == nEntityObjectServer::IDINVALID)
    {
        return 0;
    }

    // get entity object
    nEntityObject * outdoor = nEntityObjectServer::Instance()->GetEntityObject(oid);

    return outdoor;
}

//------------------------------------------------------------------------------
/** 
*/
void
nTerrainEditorState::PreprocessHorizon()
{
    nHorizonSegmentBuilder horizonBuilder;

    nEntityObject * outdoor = this->GetOutdoorEntityObject();
    if (!outdoor)
    {
        return;
    }

    nEntityClass * outdoorClass = outdoor->GetEntityClass();
    if (!outdoorClass)
    {
        return;
    }

    ncTerrainGMMClass * tgmmc = outdoor->GetClassComponent<ncTerrainGMMClass>();
    if (!tgmmc)
    {
        return;
    }

    ncHorizonClass * hc = outdoor->GetClassComponent<ncHorizonClass>();
    if (!hc)
    {
        return;
    }

    // if not valid
    if(!hc->IsValid())
    {
        int numSegments = tgmmc->GetNumBlocks();
        hc->BeginHorizonSegments(numSegments + 1, numSegments + 1);
    }

    horizonBuilder.Setup(tgmmc->GetHeightMap(), tgmmc->GetBlockSize(), tgmmc->GetBlockSideSizeScaled());
    horizonBuilder.Build(outdoorClass);

    hc->EndHorizonSegments();

    horizonBuilder.SaveChunk(outdoorClass);
}

//------------------------------------------------------------------------------
/** 
    @brief Unselect the current tool used
*/
void
nTerrainEditorState::DeselectCurrentTool()
{
    this->currentInguiTool->OnDeselected();
}
