#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterrainlightmapbuilder_lights.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nterrainlightmapbuilder.h"
//------------------------------------------------------------------------------
#include "ngeomipmap/ncterraingmmcell.h"
#include "ngeomipmap/ncterraingmm.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/nterraincellinfo.h"
//------------------------------------------------------------------------------
#include "nasset/nentityassetbuilder.h"
#include "nlevel/nlevelmanager.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/ncloader.h"
//------------------------------------------------------------------------------
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatiallightenv.h"
#include "nspatial/ncspatialcamera.h"
//------------------------------------------------------------------------------
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/ncscenelightenvclass.h"
#include "nscene/nscenegraph.h"
#include "nscene/ncviewport.h"
#include "nscene/nlightenvnode.h"
#include "nscene/nshadowlightnode.h"
//------------------------------------------------------------------------------
#include "kernel/nlogclass.h"
#include "mathlib/rectanglei.h"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::SetSelectedCells(const nArray<nRefEntityObject> entities)
{
    this->selectedCells.Reset();
    int i;
    for (i = 0; i < entities.Size(); ++i)
    {
        if (entities[i] && entities[i]->GetComponent<ncTerrainGMMCell>())
        {
            this->selectedCells.Append(entities[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::SetOverwriteFiles(bool overwrite)
{
    this->overwriteExistingFiles = overwrite;
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainLightMapBuilder::CreateLightEnvironmentClass()
{
    nEntityObjectId entityId = nLevelManager::Instance()->GetCurrentLevel()->FindEntity("outdoor");
    this->refOutDoorObject = nEntityObjectServer::Instance()->GetEntityObject(entityId);

    n_assert(this->lightId);
    this->refLightObject = nEntityObjectServer::Instance()->GetEntityObject(this->lightId);

    this->DestroyLightObject();

    // Create class
    n_assert(!this->lightMapClassName.IsEmpty())
    this->ConvertLightNameToValidClassName();

    //bool correct = nEntityClassServer::Instance()->RemoveEntityClass( this->lightMapClassName.Get());
    this->lightMapClass = nEntityClassServer::Instance()->GetEntityClass(this->lightMapClassName.Get());

    if( !this->lightMapClass)
    {
        nEntityClass * baseClass = nEntityClassServer::Instance()->GetEntityClass( "neoutlight" );
        n_assert( baseClass );
        if( ! baseClass )
        {
            n_message( "The base class nescene not exist" );
            return 0;
        }
        this->lightMapClass = nEntityClassServer::Instance()->NewEntityClass( baseClass, this->lightMapClassName.Get() );
    }

    // Configure the loader asset by default
    ncLoaderClass* loader = this->lightMapClass->GetComponent<ncLoaderClass>();
    if (loader)
    {
        loader->SetDefaultResourceFile();
        nFileServer2::Instance()->MakePath( loader->GetResourceFile() );

        this->lightTexturePath = loader->GetResourceFile();
        this->lightTexturePath += "/textures";
        nFileServer2::Instance()->MakePath( this->lightTexturePath );
        this->shadowTexturePath = this->lightTexturePath;
        this->lightTexturePath += "/lightmap";
        this->shadowTexturePath += "/shadowmap";

        nString sceneNOH = nEntityAssetBuilder::GetSceneNOHForAsset( loader->GetResourceFile() );
        this->libSceneRoot = static_cast<nLightEnvNode*>(nKernelServer::ks->New("nlightenvnode", sceneNOH.Get()));
        n_assert(this->libSceneRoot);
        this->libSceneRoot->SetSelectorType(FOURCC('lmap'));
        this->lightMapClass->GetComponent<ncSceneClass>()->SetRootNode(this->libSceneRoot);
    }
    else
    {
        return false;
    }

    //create lightmaps
    if (this->CreateTerrainLightMaps())
    {
        //save class
        nEntityClassServer::Instance()->SetEntityClassDirty( this->lightMapClass , true );
        return nEntityClassServer::Instance()->SaveEntityClass( this->lightMapClass );
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainLightMapBuilder::CreateTerrainLightMaps()
{
    //create scenegraph
    nSceneGraph* sceneGraph = nSceneServer::Instance()->NewSceneGraph("lightMapGenerationSceneGraph");

    ncTerrainGMMClass * terrainClass = this->refOutDoorObject->GetClassComponentSafe<ncTerrainGMMClass>();
    int numBlocks = terrainClass->GetNumBlocks();
    float blockSize = this->ComputeLightMapResolution();
    // force material to select maximum lod in all cases
    ncTerrainMaterialClass * materialClass = this->refOutDoorObject->GetClassComponentSafe<ncTerrainMaterialClass>();
    int prevLodDistance = materialClass->GetMaterialLODDistance();
    materialClass->SetMaterialLODDistance( numBlocks );

    // create light and shadow render targets with the specified size
    nString renderTargetName;
    nTexture2::Format texFormat;

    texFormat = nTexture2::A8R8G8B8;
    renderTargetName = "TerrainLightmap";//same as in terrain_renderpath.xml
    this->refRenderTarget = nGfxServer2::Instance()->NewRenderTarget(renderTargetName.Get(), this->lightMapSize, this->lightMapSize, texFormat, nTexture2::RenderTargetColor);
    if (!this->refRenderTarget.isvalid())
    {
        n_message("nTerrainLightMapBuilder::CreateTerrainLightMaps() failed: couldn't create render target '%s'", renderTargetName.Get());
        return false;
    }

    texFormat = nTexture2::A8R8G8B8;
    renderTargetName = "TerrainShadowmap";//same as in impostor_renderpath.xml
    this->refShadowRenderTarget = nGfxServer2::Instance()->NewRenderTarget(renderTargetName.Get(), this->shadowMapSize, this->shadowMapSize, texFormat, nTexture2::RenderTargetColor);
    if (!this->refShadowRenderTarget.isvalid())
    {
        n_message("nTerrainLightMapBuilder::CreateTerrainLightMaps() failed: couldn't create render target '%s'", renderTargetName.Get());
        return false;
    }

    // create viewport
    this->refViewportObject = nEntityObjectServer::Instance()->NewLocalEntityObject("Terraincam");
    this->refViewportObject->GetComponentSafe<ncViewport>()->SetOrthogonal(blockSize, blockSize, 1, 1000);
    this->refViewportObject->GetComponentSafe<ncTransform>()->SetEuler(vector3(n_deg2rad(-90), 0, 0));
    this->refViewportObject->GetClassComponentSafe<ncLoaderClass>()->LoadResources();
    this->refViewportObject->GetComponentSafe<ncLoader>()->LoadComponents();

    // create camera
    this->refCameraObject = nSpatialServer::Instance()->CreateCamera(nCamera2(), matrix44(), false);


    //shadow textures
    this->GenerateMiniShadowMapTextures();
    //lightmap textures
    this->GenerateMiniTextures();
    //global texture
    this->GenerateGlobalLightMapTexture();

    // render 
    sceneGraph->Release();

    // release camera
    nSpatialServer::Instance()->DestroyCamera(this->refCameraObject->GetComponentSafe<ncSpatialCamera>());

    // release viewport
    nEntityClass * entityClass = this->refViewportObject->GetEntityClass();
    nEntityObjectServer::Instance()->RemoveEntityObject(this->refViewportObject);
    entityClass->GetComponent<ncLoaderClass>()->UnloadResources();

    materialClass->SetMaterialLODDistance(prevLodDistance);

    ncLoaderClass* loader = this->lightMapClass->GetComponent<ncLoaderClass>();
    if (loader)
    {
        nEntityAssetBuilder::SaveSceneRoot( this->lightMapClass );
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::CreateLightEnvironmentObject()
{
    nEntityObjectServer* entityObjectServer = nEntityObjectServer::Instance();

    this->lightMapObject = entityObjectServer->NewEntityObject( this->lightMapClassName.Get() );
    n_assert(this->lightMapObject);

    this->lightMapObject->GetComponentSafe<ncSpatialLightEnv>()->SetSpaceEntity( this->refOutDoorObject.get() );
    this->lightMapObject->GetComponentSafe<ncSpatialLightEnv>()->SetLightEntity( this->refLightObject.get() );

    nLevelManager::Instance()->GetCurrentLevel()->AddEntity(this->lightMapObject);
    nLevelManager::Instance()->GetCurrentLevel()->SetEntityName( this->lightMapObject->GetId(), "outlight");

    //<HACK> force the outlight into the outdoor space
    this->lightMapObject->GetComponentSafe<ncSpatialLightEnv>()->SetBBox(0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f);
    this->lightMapObject->GetComponentSafe<ncSpatialLightEnv>()->SetAffectsOtherSpaceTypes(false);
    this->lightMapObject->GetComponentSafe<ncTransform>()->SetPosition(vector3(0.1f, 0.1f, 0.1f));
    //</HACK>

    //ncSpatialQuadtree* quadtree = this->refOutDoorObject->GetComponentSafe<ncSpatialQuadtree>();
    //quadtree->GetRootCell()->AddEntity(this->lightMapObject);
    //nSpatialServer::Instance()->InsertEntity(this->lightMapObject);
}


//------------------------------------------------------------------------------
/**
*/
bool
nTerrainLightMapBuilder::GenerateGlobalLightMap(const char* lightClassName, int globalLightMapsize )
{
    nEntityObjectId entityId = nLevelManager::Instance()->GetCurrentLevel()->FindEntity("outdoor");
    this->refOutDoorObject = nEntityObjectServer::Instance()->GetEntityObject(entityId);

    if ( !this->refOutDoorObject.isvalid() )
    { 
        NLOG( lightmap, ( 0 ,  " The level not has outdoor") );
        return false;
    }

    ncTerrainMaterialClass* terrainMaterial = this->refOutDoorObject->GetClassComponentSafe<ncTerrainMaterialClass>();
    nEntityObject* lightMapObject = terrainMaterial->GetLightMap();
    nString currentLightMapName;
    this->lightMapClassName = lightClassName;
    this->globalLightMapSize = globalLightMapsize;

    bool validName;
     // If the class name is empty use the current lightmap
    if ( this->lightMapClassName.IsEmpty() )
    {
        if ( lightMapObject )
        {
            this->lightMapClassName =  lightMapObject->GetEntityClass()->GetName();
            lightMapObject->GetEntityClass()->GetName();
            validName = true;
        } else
        {
            validName = false;
        }
    } else
    {
        validName = this->ConvertLightNameToValidClassName();
    }

    if ( ! validName )
    {
        NLOG( lightmap, ( 0 ,  " The terrain has not a lightmap and the lightmap name is invalid ") );
        return false;
    }

    this->lightMapClass = nEntityClassServer::Instance()->GetEntityClass(this->lightMapClassName.Get());
    if ( ! this->lightMapClass )
    {
        NLOG( lightmap, ( 0 , "Invalid lightmap entity class" ) );
        return false;
    }
    ncLoaderClass* loader = this->lightMapClass->GetComponent<ncLoaderClass>();
    if ( !loader )
    {
        NLOG( lightmap, ( 0 , "Invalid lightmap entity class, it doesn't have a loader" ) );
        return false;
    }
    
    this->lightTexturePath = loader->GetResourceFile();
    this->lightTexturePath += "/textures/lightmap";

    bool result;
    terrainMaterial->SetLightMap(0); // Unload resource
    result = this->GenerateGlobalLightMapTexture();
    terrainMaterial->SetLightMap(lightMapObject); //Restore  previous lightmap

    return result;
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainLightMapBuilder::GenerateGlobalLightMapTexture()
{
    // Calculate the block size in global texture
    ncTerrainGMMClass * terrainClass = this->refOutDoorObject->GetClassComponentSafe<ncTerrainGMMClass>();
    if (! terrainClass )
    {
        NLOG( lightmap, ( 0 ,  " The outdoor hasn't the TerrainGMMClass component") );
        return false;
    }
    int numBlocks = terrainClass->GetNumBlocks();
    int subBlockSize = this->globalLightMapSize / numBlocks ;

    if ( (this->globalLightMapSize % numBlocks) != 0 )
    {
        NLOG( lightmap, ( 0 ,  "The ligtmap's size is not multiple of number of blocks") );
        return false;
    }

    // First step load the global texture as dynamic, load for only update the selected cell
    nString globalLightTextureName = this->lightTexturePath + "global.dds";
    nTexture2* globalLightmap = nGfxServer2::Instance()->NewTexture(0);
    if (!globalLightmap)
    {
        NLOG( lightmap, ( 0 ,  " Don't create a new texture, globalLightmap ") );
        return false;
    }
    nTexture2* whiteMap = nGfxServer2::Instance()->NewTexture(0);
    if (!whiteMap)
    {
        NLOG( lightmap, ( 0 ,  " Don't create a new texture, whiteMap ") );
        globalLightmap->Release();
        return false;
    }

    nTexture2* srcLightmap(0);
    whiteMap->SetFilename( whiteTexturePath );
    if (! whiteMap->Load() )
    {
        NLOG( lightmap, ( 0 ,  " Can't load the white texture , %s  ",  whiteTexturePath ) );
        whiteMap->Release();
        globalLightmap->Release();
        return false;
    }

    // Create a empty texture without mipMap
    globalLightmap->SetUsage( nTexture2::CreateEmpty | nTexture2::Dynamic);
    globalLightmap->SetType( nTexture2::TEXTURE_2D );
    globalLightmap->SetFormat( nTexture2::DXT5 );
    globalLightmap->SetWidth( static_cast<ushort>(this->globalLightMapSize) );
    globalLightmap->SetHeight( static_cast<ushort>(this->globalLightMapSize) );
    globalLightmap->Load();
    if (! globalLightmap->IsLoaded() )
    {
        NLOG( lightmap, ( 0 ,  " Can't create empty texture , globalLightmap" ) );
        whiteMap->Release();
        globalLightmap->Release();
        return false;
    }

    // Load the previous global lightmap
    srcLightmap = nGfxServer2::Instance()->NewTexture(0);
    if (! srcLightmap )
    {
        NLOG( lightmap, ( 0 ,  " Can't create texture  for load the previous texture" ) );
        whiteMap->Release();
        globalLightmap->Release();
        return false;
    }

    srcLightmap->SetFilename( globalLightTextureName );
    if ( !srcLightmap->Load() )
    {
        srcLightmap->Release();
        srcLightmap = whiteMap; // Use the whiteMap for create texture
    }

    rectanglei srcRect( 0 ,0 , srcLightmap->GetWidth() - 1, srcLightmap->GetHeight() -1 );
    rectanglei dstRect( 0, 0, this->globalLightMapSize -1, this->globalLightMapSize -1 );

    // Copy the mipmap 0 , and scale it
    srcLightmap->Copy( globalLightmap, srcRect , dstRect , nTexture2::FILTER_LINEAR );

    // free the original global lightmap
    if (srcLightmap != whiteMap) 
    {
        srcLightmap->Unload();
        srcLightmap->Release();
        srcLightmap = 0;
    }

    /// The local texture , is a texture for each cell
    nTexture2* localLightmap = nGfxServer2::Instance()->NewTexture(0);
    if (! localLightmap )
    {
        NLOG( lightmap, ( 0 ,  " Can't create texture , it's a texture for each cell" ) );
        whiteMap->Release();
        globalLightmap->Release();
        return false;
    }

    //Generate for selected cells.
    nArray<nEntityObject*> terrainBlocks;
    this->GetCells(terrainBlocks);
    srcRect.v0.set( 0 , 0 );
    for (int i = 0; i < terrainBlocks.Size(); ++i) // In selected cells
    {
        ncTerrainGMMCell* currentCell = terrainBlocks[i]->GetComponent<ncTerrainGMMCell>();
        //Calculate the position and size the texture in global texture
        int bx = currentCell->GetCellX();
        int bz = currentCell->GetCellZ();
        dstRect.v0.x = bx;
        dstRect.v0.y = bz;
        dstRect.v0 *= subBlockSize;
        dstRect.v1.x = dstRect.v0.x + subBlockSize - 1;
        dstRect.v1.y = dstRect.v0.y + subBlockSize - 1;

        // Load the local texture, if it is not exist then it use a white texture
        nString textureName = this->GetLightmapTextureName(bx, bz);
        localLightmap->SetFilename( textureName );
        srcLightmap = localLightmap->Load() ? localLightmap : whiteMap; // the currrent cell not has a lightmap
        srcRect.v1.x = srcLightmap->GetWidth()  -1;
        srcRect.v1.y = srcLightmap->GetHeight() -1;

        srcLightmap->Copy( globalLightmap, srcRect, dstRect, nTexture2::FILTER_LINEAR);

        if ( srcLightmap == localLightmap ) // If localLightMap is loaded
        {
            localLightmap->Unload();    
        }
    }


    bool result = globalLightmap->SaveResourceAs(globalLightTextureName.Get(), nTexture2::DXT5);
    NLOGCOND( lightmap , !result , ( 0 ,"I can't save the %s" , globalLightTextureName.Get() ) );
    globalLightmap->Release();
    whiteMap->Release();
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::GenerateMiniTextures()
{
    float blockSize = this->ComputeLightMapResolution();
    //create scenegraph
    nSceneGraph* sceneGraph = nSceneServer::Instance()->NewSceneGraph("lightMapGenerationSceneGraph");

    nSceneServer* sceneServer = nSceneServer::Instance();
    int trfmPassIndex = sceneServer->GetPassIndexByFourCC(FOURCC('trfm'));
    int rtgtPassIndex = sceneServer->GetPassIndexByFourCC(FOURCC('rtgt'));

    //UGLY HACK- overwrite the surface in the terrain root geomipmapnode
    nString prevSurface = this->TerrainSetNewSurface( FOURCC('gmlm'), "shaders:gmmt_lmap.fx" );

    //generate mini textures
    this->refViewportObject->GetComponentSafe<ncViewport>()->SetOrthogonal(blockSize, blockSize, 1, 1000);
    this->refViewportObject->GetComponentSafe<ncTransform>()->SetEuler(vector3(n_deg2rad(-90), 0, 0));

    nTexture2* miniShadowMap = 0;

    // copy all terrain blocks into the array if no selection
    nArray<nEntityObject*> terrainBlocks;
    this->GetCells(terrainBlocks);

    // invalidate terrain lightmap cache
    ncTerrainMaterialClass* terrainMaterial = this->refOutDoorObject->GetClassComponentSafe<ncTerrainMaterialClass>();
    nEntityObject* lightMapObject = terrainMaterial->GetLightMap();
    terrainMaterial->SetLightMap(0);

    for (int i = 0; i < terrainBlocks.Size(); ++i)
    {
        ncTerrainGMMCell* currentCell = terrainBlocks[i]->GetComponent<ncTerrainGMMCell>();
        int bx = currentCell->GetCellX();
        int bz = currentCell->GetCellZ();

        // filter empty cells
        nTerrainCellInfo* cellInfo = terrainMaterial->GetTerrainCellInfo(bx, bz);
        if (cellInfo->IsEmpty())
        {
            continue;
        }

        // save scene data
        nString shadowTextureName(this->GetShadowTextureName(bx, bz));
        nString fileName = shadowTextureName.ExtractFileName();
        fileName.StripExtension();
        nString lightmapTextureName(this->GetLightmapTextureName(bx, bz));

        int cellId = currentCell->GetComponentSafe<ncSpatialQuadtreeCell>()->GetId();
        ncSceneLightEnvClass* sceneClass = this->lightMapClass->GetComponentSafe<ncSceneLightEnvClass>();
        sceneClass->SetPathByCellId(cellId, fileName.Get());
        sceneClass->SetBlockByCellId(cellId, bx, bz);//to access nodes by block coordinates

        //TODO-if the file exists and OVERWRITE=false, do not generate
        if (nKernelServer::ks->GetFileServer()->FileExists(lightmapTextureName) && !this->overwriteExistingFiles)
        {
            continue;
        }

        //update shadow map and shadowprojection
        nShadowLightNode* lightNode = static_cast<nShadowLightNode*>(sceneClass->GetClassLightByBlock(bx, bz));
        n_assert(lightNode);
        nShaderParams& shaderParams = currentCell->GetShaderOverrides();

        //const char* miniShadowMapPath = lightNode->GetTexture(nShaderState::ShadowMap);
        nString miniShadowMapPath = this->GetShadowTextureName(bx, bz);
        miniShadowMap = nGfxServer2::Instance()->NewTexture(miniShadowMapPath.Get());
        // unload previous shadow map if rebuilding lightmaps
        if (miniShadowMap->IsValid())
        {
            miniShadowMap->Unload();
            n_assert(!miniShadowMap->IsValid());
        }
        miniShadowMap->SetFilename(miniShadowMapPath);
        miniShadowMap->Load();

        //FIXME
        currentCell->GetShaderOverrides().ClearArg(nShaderState::GlobalLightMap);

        shaderParams.SetArg(nShaderState::ShadowMap, nShaderArg( miniShadowMap ));
        
        shaderParams.SetArg(nShaderState::ShadowProjection, nShaderArg(&lightNode->GetShadowProjection()));

        vector4 terrainShadowMapInfo( this->offset.x , this->offset.y , miniShadowMap->GetWidth(),  this->antiAliasingDiskSize );
        shaderParams.SetArg(nShaderState::TerrainShadowMapInfo , nShaderArg( terrainShadowMapInfo) );


        //force light to be handled as directional in shader
        const vector3& modelLightDir = this->refLightObject->GetComponent<ncTransform>()->GetTransform().z_component();
        shaderParams.SetArg(nShaderState::ModelLightDir, nShaderArg(modelLightDir));

        //begin attach
        sceneGraph->BeginAttach();
        nGfxServer2::Instance()->BeginFrame();

        //attach
        this->AttachCurrentCellEntities(sceneGraph, currentCell->GetEntityObject(), false);
        this->AttachLightEntity(sceneGraph);

        //set viewport to cell center
        bbox3 bbox = currentCell->GetEntityObject()->GetComponentSafe<ncSpatialQuadtreeCell>()->GetBBox();
        vector3 center = bbox.center();
        center.y = bbox.corner_point(4).y; //max_y
        NLOG ( lightmap , ( 0 , "cell_center: %f %f %f", center.x, center.y, center.z) );
        this->refViewportObject->GetComponentSafe<ncTransform>()->SetPosition(center);

        //ugly hack, camera nodes don't get the projection from anywhere
        nCamera2 cam = this->refViewportObject->GetComponentSafe<ncViewport>()->GetCamera();
        nGfxServer2::Instance()->SetCamera(cam);

        //attach viewport
        this->AttachViewport(sceneGraph);

        //end attach
        sceneGraph->EndAttach();

        //set geometry level to maximum
        currentCell->SetGeometryLODLevel(0);
        currentCell->SetMaterialLODLevel(0);

        //render scene
        sceneGraph->RenderPass(trfmPassIndex);// compute transforms
        sceneGraph->RenderPass(rtgtPassIndex);// render targets

        nGfxServer2::Instance()->EndFrame();

        //update texturename and save lightmap
        if (this->SaveLightMapTexture(bx, bz))
        {
            //int cellId = currentCell->GetComponentSafe<ncSpatialQuadtreeCell>()->GetId();
            //nString fileName = this->GetShadowTextureName(bx,bz).ExtractFileName();
            //fileName.StripExtension();
            //ncSceneLightClass* sceneClass = this->lightMapClass->GetComponentSafe<ncSceneLightClass>();
            //sceneClass->SetPathByCellId(cellId, fileName.Get());
            //sceneClass->SetBlockByCellId(cellId, bx, bz);//to access nodes by block coordinates
        }
        
        //release texture
        miniShadowMap->Release();
    }

    // restore original lightmap
    terrainMaterial->SetLightMap(lightMapObject);

    // restore geometry node original surface
    this->TerrainRestoreSurface(prevSurface);
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::DestroyLightObject()
{
    nEntityObjectServer* entityObjectServer = nEntityObjectServer::Instance();
    nEntityObjectId eoId = nLevelManager::Instance()->GetCurrentLevel()->FindEntity("outlight");
    if (eoId)
    {
        nEntityObject* eobj = entityObjectServer->GetEntityObject(eoId);
		if (!eobj)
		{
			n_message( "The \"outlight\" instance is defined in level manager but it isn't found in the entityObject server"); 
			return;
		}
        //@todo ma.garcias - need to explicitly remove the entity when destroyed
        //otherwise it has references left
        ncSpatialLightEnv* lightEnv = eobj->GetComponentSafe<ncSpatialLightEnv>();
        lightEnv->RemoveFromSpaces();
        ncLoader* loader = eobj->GetComponentSafe<ncLoader>();
        loader->UnloadComponents();
        ncLoaderClass* loaderClass = eobj->GetClassComponentSafe<ncLoaderClass>();
        loaderClass->UnloadResources();

        entityObjectServer->RemoveEntityObjectById( eoId );

        nLevelManager::Instance()->GetCurrentLevel()->RemoveEntityName("outlight");
    }
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
