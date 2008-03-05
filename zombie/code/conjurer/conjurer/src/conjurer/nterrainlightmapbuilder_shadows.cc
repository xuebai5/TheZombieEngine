#include "precompiled/pchconjurerapp.h"
/*-----------------------------------------------------------------------------
    @file nterrainlightmapbuilder_shadows.cc
    @ingroup NebulaConjurerEditor

    @brief nTerrainLightmapBuilder

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "conjurer/nterrainlightmapbuilder.h"
#include "conjurer/nconjurerapp.h"
//------------------------------------------------------------------------------
#include "ngeomipmap/ncterraingmmcell.h"
#include "ngeomipmap/ncterraingmm.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ngeomipmap/nterraincellinfo.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "ngeomipmap/ngmmvertexbuilder.h"
//------------------------------------------------------------------------------
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/ncloader.h"
//------------------------------------------------------------------------------
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialcamera.h"
//------------------------------------------------------------------------------
#include "nscene/nlightnode.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/ncscenelightenvclass.h"
#include "nscene/nscenegraph.h"
#include "nscene/ncviewport.h"
#include "zombieentity/ncdictionary.h"
//------------------------------------------------------------------------------
#include "mathlib/line.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
    The Z camera is the same direction of light , but this has a free axis toration, 
    This algorithm get the best orientation for best qulity of shadowmap
*/
vector3 
nTerrainLightMapBuilder::GetBestOrientation()
{
    vector3 lightEuler = this->refLightObject->GetComponentSafe<ncTransform>()->GetEuler();
    //vector3 lightDirection(0,0,1);
    matrix33 m;
    m.from_euler(lightEuler);
    vector3 lightDirection = m.z_component();//m*lightDirection;

    //Calculate the best orientation, the terrain's cells always are aligned axis2
    matrix33 orientation;
    vector3 upVector;
    if ( n_abs( lightDirection.x ) > n_abs( lightDirection.x ) ) // Use the greater perpendicular
    {
        upVector = vector3( 0.f,0.f,1.f); 
    }
    else
    {   
        upVector = vector3( 1.f,0.f,0.f);
    }
    orientation.lookat( vector3(0,0,0) ,  - lightDirection , upVector ) ;

    //return lightEuler;
    return orientation.to_euler();
}

//------------------------------------------------------------------------------
/**
    Matrix A = r.x , r.y 
               r.z , r.w
    transform to orthogonal u v aproach
   
   v1 = [1.0,0.0,0.0] * invModelCamera;
   v2 = [0.0,0.0,1.0] * invModelCamera;
   L = MAX( |v1| , |v2|
   Find r values that
   v1*A = [ 0 , L ]  
   v2*A = [ L , 0 ]
*/
vector4
nTerrainLightMapBuilder::GetBestDeformation(const vector3& lightEuler)
{
    vector4 r;
    transform44 tr;
    nCamera2 cam;
    tr.seteulerrotation(lightEuler); // This transform put the light int the world.
    matrix44 invModelCamera(tr.getmatrix());
    invModelCamera.invert();

    vector3 v1 = invModelCamera * vector3( 1.f, 0.f, 0.f); // Matrix44 use incorrect sort.
    vector3 v2 = invModelCamera * vector3( 0.f, 0.f, 1.f);
    v1.z = 0.f;
    v2.z = 0.f;

    float L = n_max( v1.len() , v2.len() );
    /*
        v1.x * r.x + v1.y * r.z = 0
        v2.x * r.x + v2.y * r.z = L

        v1.x * r.y + v1.y * r.w = L
        v2.x * r.y + v2.y * r.w = 0

        resolve by crammer
    */

    float det = v1.x * v2.y  - v2.x * v1.y;

    r.x = (  0.f*v2.y - L*v1.y ) / det;
    r.z = ( v1.x*L    - v2.x*0.f ) / det;

    r.y = ( L*v2.y    - 0.f*v1.y)/det;
    r.w = ( v1.x*0.f  - v2.x*L) /det;

    return r;
}

//------------------------------------------------------------------------------
/**
     return the boundinh box of terrain transformed. 

*/
bbox3
nTerrainLightMapBuilder::GetBBoxOfTerrainTransformed(ncTerrainGMMCell* currentCell, const matrix44& toCameraSpace) const
{    
#if 0
     //A aproximation is transform all cell's bbox's vertex
    // But the best fit is transform all cell's vertex
    bbox3 bbInCamSpace; // Boundig box in camera space
    bbox3 bbCell;// Terrain's bbox

    bbInCamSpace.begin_extend();
    bbCell = currentCell->GetBBox();
    for (int cornerIdx = 0; cornerIdx < 8 ; ++cornerIdx)
    {
        vector3 vertexInCamSpace = bbCell.corner_point( cornerIdx);
        vector3 vertex = toCameraSpace.transform_coord( vertexInCamSpace);
        bbInCamSpace.extend( vertex );
    }
    bbInCamSpace.end_extend();
    return bbInCamSpace;
#else
    // transform all cell's vertex
    bbox3 bbInCamSpace; // Boundig box in camera space

    // First step. Calculate the terrain vertex
    ncTerrainGMMClass * terrainClass = this->refOutDoorObject->GetClassComponentSafe<ncTerrainGMMClass>();
    nGMMGeometryStorage* geom = n_new( nGMMGeometryStorage);
    geom->AddRef();
    geom->SetFillNormals(false);
    nGMMVertexBuilder* vertexBuilder = terrainClass->GetVertexBuilder();
    int countVertex  = vertexBuilder->CountVertices( 0 ); // Get vertex for lod 0
    float* vb =n_new_array( float,  3 * countVertex);

    geom->SetupVertexBuffer( vb , countVertex);
    vertexBuilder->SetupStorage( geom);
    vertexBuilder->FillVerticesBlock( currentCell->GetCellX(), currentCell->GetCellZ(), 0);

    // Second step. for each vertex transform and extend the bbox
    bbInCamSpace.begin_extend();
    for ( int i = 0; i < countVertex ; ++i )
    {
        //vector3* vertex  = geom->GetVertex(i);
        vector3* vertex= reinterpret_cast<vector3 *> (vb + i*3);
        vector3 vertexInCamSpace = toCameraSpace.transform_coord( *vertex );
        bbInCamSpace.extend( vertexInCamSpace );
    }
    bbInCamSpace.end_extend();
    vertexBuilder->SetupStorage(0);

    n_delete_array( vb );
    geom->Release();
    geom = 0;

    return bbInCamSpace;
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::GetPositionAndOrthogonal(ncTerrainGMMCell* currentCell,vector3& position, vector4& orthogonal, const vector3& lightEuler, const vector4& deformation)
{
    //const float maxDistance = 1000.0f;
    const float nearPlane = 0.0f;

    // Get unitari projection
    // Use transform for not future dependence euler order
    // Use camera for not future dependence RH or LH
    // to optimize this is easy
    transform44 tr;
    nCamera2 cam;
    tr.seteulerrotation(lightEuler); // This transform put the light int the world.
    cam.SetOrthogonal( 2.0f , 2.0f, 0.0f , 1.0f); // This transform not has scale

    matrix44 toCameraSpace;
    matrix44 toWorldSpace;
    matrix44 invModelCamera(tr.getmatrix());  
    invModelCamera.invert();
    matrix44 deformationMatrix( deformation.x, deformation.y, 0.f, 0.f,
                                deformation.z, deformation.w, 0.f, 0.f,
                                          0.f,           0.f, 1.f, 0.f,
                                          0.f,           0.f, 0.f, 1.f );

    toCameraSpace = invModelCamera * deformationMatrix * cam.GetProjection();
    toWorldSpace = toCameraSpace;
    toWorldSpace.invert();

    // For now is the same for all cells

    //Second step: Transform all points to camera space and caluclate bbox in camera space.
    bbox3 bbInCamSpace = GetBBoxOfTerrainTransformed( currentCell , toCameraSpace);// Boundig box in camera space

    // Third step:
    // Obtain position , is in the middle of -z side
    vector3 posInCamSpace = bbInCamSpace.center(); // posInCamSpace.xy = pos.bbox.center.xy
    posInCamSpace.z = bbInCamSpace.vmin.z; // Set the camera in the minimun z
    posInCamSpace.z -= (this->maxDistance + nearPlane); // Add to volume the distance, remember the matrix not has scale
    position = toWorldSpace * posInCamSpace; // transform to world space

    // fourth step 
    // Obtain the size;
    vector3 size  = bbInCamSpace.vmax - bbInCamSpace.vmin;
    orthogonal.x = size.x;
    orthogonal.y = size.y;
    orthogonal.z = nearPlane ; // near plane , this value is valid for orthogonal cameras
    orthogonal.w = nearPlane+size.z + this->maxDistance; // Far plane 

    // Note: it is posible that you add small offset to size.x and size.y due to precision's problem.
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::GenerateMiniShadowMapTextures()
{
    ncTerrainGMMClass * terrainClass = this->refOutDoorObject->GetClassComponentSafe<ncTerrainGMMClass>();
    int numBlocks = terrainClass->GetNumBlocks();
    float blockSize = this->ComputeLightMapResolution();

    //create scenegraph
    nSceneGraph* sceneGraph = nSceneServer::Instance()->NewSceneGraph("lightMapGenerationSceneGraph");

    vector4 orthogonal(blockSize*3, blockSize*3, 1, 1000); // 3 is a magic tune number

    //generate mini textures
    this->refViewportObject->GetComponentSafe<ncViewport>()->SetOrthogonal(orthogonal.x, orthogonal.y, orthogonal.z, orthogonal.w);

    vector3 lightPos = this->refLightObject->GetComponentSafe<ncTransform>()->GetPosition();
    vector3 lightEuler = this->GetBestOrientation();
    this->refViewportObject->GetComponentSafe<ncTransform>()->SetEuler( lightEuler );
    vector4 deformation = this->GetBestDeformation(lightEuler);
    this->refViewportObject->GetComponentSafe<ncDictionary>()->SetVectorVariable( "ShadowProjectionDeform",deformation);

    // apply that deviation to all cells
    ncSceneLightEnvClass* sceneClass = this->lightMapClass->GetComponentSafe<ncSceneLightEnvClass>();
    sceneClass->Reset();//clear previous associations of cell to lightnode
    sceneClass->SetBlockSize(numBlocks, numBlocks);//to access nodes by block coordinates

    // copy all terrain blocks into the array if no selection
    nArray<nEntityObject*> terrainBlocks;
    this->GetCells(terrainBlocks);

    // invalidate terrain lightmap cache
    ncTerrainMaterialClass* terrainMaterial = this->refOutDoorObject->GetClassComponentSafe<ncTerrainMaterialClass>();
    nEntityObject* lightMapObject = terrainMaterial->GetLightMap();
    terrainMaterial->SetLightMap(0);

    //UGLY HACK- overwrite the surface in the terrain root geomipmapnode
    nString prevSurface = this->TerrainSetNewSurface( FOURCC('gmsd'), "shaders:gmmt_sdep.fx" );

    // generate cell lightmaps
    for (int i = 0; i < terrainBlocks.Size(); ++i)
    {
        ncTerrainGMMCell* currentCell = terrainBlocks[i]->GetComponent<ncTerrainGMMCell>();
        int bx = currentCell->GetCellX();
        int bz = currentCell->GetCellZ();
        // filter empty cells
        nTerrainCellInfo* cellInfo = terrainMaterial->GetTerrainCellInfo(bx, bz);
        if (!cellInfo->IsEmpty())
        {
            this->GenerateMiniShadowMapTexture(terrainBlocks[i], sceneGraph, lightEuler, deformation);
        }
    }

    // restore geometry node original surface
    this->TerrainRestoreSurface(prevSurface);

    // restore original lightmap
    terrainMaterial->SetLightMap(lightMapObject);
}

//------------------------------------------------------------------------------
/**
*/
bool
nTerrainLightMapBuilder::GenerateMiniShadowMapTexture(nEntityObject* cell,
                                                      nSceneGraph* sceneGraph,
                                                      const vector3& lightEuler,
                                                      const vector4& deformation)
{
    ncTerrainGMMCell* currentCell = cell->GetComponent<ncTerrainGMMCell>();
    nSceneServer* sceneServer = nSceneServer::Instance();
    int trfmPassIndex = sceneServer->GetPassIndexByFourCC(FOURCC('trfm'));
    int rtgtPassIndex = sceneServer->GetPassIndexByFourCC(FOURCC('rtgt'));

    //ncTerrainMaterialClass* terrainMaterial = this->refOutDoorObject->GetComponentSafe<ncTerrainMaterialClass>();
    int bx = currentCell->GetCellX();
    int bz = currentCell->GetCellZ();

    //set viewport to cell center
    bbox3 bbox = currentCell->GetBBox();
    vector3 center = bbox.center();
    vector4 orthogonal;

    this->GetPositionAndOrthogonal(currentCell, center, orthogonal, lightEuler , deformation);
    this->refViewportObject->GetComponentSafe<ncViewport>()->SetOrthogonal(orthogonal.x, orthogonal.y, orthogonal.z, orthogonal.w);

    //vector3 cellToLightVector = lightPos - center;

    //find position to place viewport
    //center += deviation;

    //add the block to the scene resource for the shadow-
    nString shadowTextureName(this->GetShadowTextureName(bx, bz));//this fills shadowTextureName
    nString fileName = shadowTextureName.ExtractFileName();
    fileName.StripExtension();

    this->SceneShadowMapData(currentCell->GetEntityObject(), shadowTextureName, orthogonal, lightEuler, center, deformation);

    //get the filename to store the shadow map
    int cellId = currentCell->GetComponentSafe<ncSpatialQuadtreeCell>()->GetId();
    ncSceneLightEnvClass* sceneClass = this->lightMapClass->GetComponentSafe<ncSceneLightEnvClass>();
    sceneClass->SetPathByCellId(cellId, fileName.Get());
    sceneClass->SetBlockByCellId(cellId, bx, bz);//to access nodes by block coordinates

    if (nKernelServer::ks->GetFileServer()->FileExists(shadowTextureName) && !this->overwriteExistingFiles)
    {
        return false;
    }

    //set viewport position
    this->refViewportObject->GetComponentSafe<ncTransform>()->SetPosition(center);

    //begin attach
    sceneGraph->BeginAttach();
    nGfxServer2::Instance()->BeginFrame();

    //clear shader overrides so that the generation wont crash
    currentCell->GetShaderOverrides().ClearArg(nShaderState::matDiffuse);
    currentCell->GetShaderOverrides().ClearArg(nShaderState::lightMap);
    currentCell->GetShaderOverrides().ClearArg(nShaderState::GlobalLightMap);

    // Discoment this line for draw the current cell in the same sahdowmap, only use this for debug
    //sceneGraph->Attach( cell);

    //attach
    //this->AttachCurrentCellEntities(sceneGraph, currentCell->GetEntityObject(), true);
    
    //perform a spatial query to get list of entities visible from light
    n_assert(this->refCameraObject.isvalid());
    ncSpatialCamera* spatialCam = this->refCameraObject->GetComponentSafe<ncSpatialCamera>();

    bool horizonEnabled = nSpatialServer::Instance()->IsDoingHorizonCulling();
    nSpatialServer::Instance()->SetDoHorizonCulling(false);
	nSpatialServer::Instance()->SetFrameId( nSpatialServer::Instance()->GetFrameId() +1 );
    spatialCam->SetUseCameraCollector(true);
    spatialCam->TurnOn();
    spatialCam->Update(this->refViewportObject->GetComponent<ncTransform>()->GetTransform(), nConjurerApp::Instance()->GetFrameId());
    spatialCam->Update(this->refViewportObject->GetComponent<ncViewport>()->GetCamera());
    nSpatialServer::Instance()->DetermineVisibility(spatialCam, 0);
    spatialCam->TurnOff();
    nSpatialServer::Instance()->SetDoHorizonCulling(horizonEnabled);

    //attach list of entities visible from the light
    const nArray<nRef<nEntityObject> >& visibleEntities = spatialCam->GetVisibleEntities();
    this->AttachEntityArray(sceneGraph, visibleEntities);
    
    //attach light entity
    this->AttachLightEntity(sceneGraph);

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
    sceneGraph->RenderPass(rtgtPassIndex);// render targest

    nGfxServer2::Instance()->EndFrame();

    //update texturename and save lightmap
    if (this->SaveShadowMapTexture(bx, bz)) //currentCell->GetEntityObject(), bx, bz, orthogonal, lightEuler, center))
    {
        //int cellId = currentCell->GetComponentSafe<ncSpatialQuadtreeCell>()->GetId();
        //fileName.StripExtension();
        //ncSceneLightClass* sceneClass = this->lightMapClass->GetComponentSafe<ncSceneLightClass>();
        //sceneClass->SetPathByCellId(cellId, fileName.Get());
        //sceneClass->SetBlockByCellId(cellId, bx, bz);//to access nodes by block coordinates
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainLightMapBuilder::GenerateGlobalShadowMapTexture()
{
    ncTerrainGMMClass * terrainClass = this->refOutDoorObject->GetClassComponentSafe<ncTerrainGMMClass>();
    int numBlocks = terrainClass->GetNumBlocks();
    float blockSize = this->ComputeLightMapResolution();

    ncTerrainGMM* terrainGMM = this->refOutDoorObject->GetComponentSafe<ncTerrainGMM>();
    ncTerrainGMMCell* firstCell = terrainGMM->GetNorthWestCell();

    //create scenegraph
    nSceneGraph* sceneGraph = nSceneServer::Instance()->NewSceneGraph("lightMapGenerationSceneGraph");

    nSceneServer* sceneServer = nSceneServer::Instance();
    int trfmPassIndex = sceneServer->GetPassIndexByFourCC(FOURCC('trfm'));
    int rtgtPassIndex = sceneServer->GetPassIndexByFourCC(FOURCC('rtgt'));

    //UGLY HACK- overwrite the surface in the terrain root geomipmapnode
    ncSceneClass* sceneClass = this->refOutDoorObject->GetClassComponentSafe<ncSceneClass>();
    nGeometryNode* geometryNode = static_cast<nGeometryNode*>(sceneClass->GetRootNode());
    n_assert(geometryNode->IsA("ngeometrynode"));
    nSurfaceNode* surfaceNode = geometryNode->GetSurfaceNode();

    nString prevSurface(geometryNode->GetSurface());
    nKernelServer::ks->PushCwd(geometryNode);
    surfaceNode = static_cast<nSurfaceNode*>(nKernelServer::ks->New("nsurfacenode", "terrainLightmapSurface"));
    nKernelServer::ks->PopCwd();

    surfaceNode->SetShader(FOURCC('gmlm'), "shaders:gmmt_lmap.fx");
    geometryNode->SetSurface(surfaceNode->GetFullName().Get());
    geometryNode->GetSurfaceNode()->LoadResources();

    // set viewport data
    this->refViewportObject->GetComponentSafe<ncViewport>()->SetOrthogonal(blockSize*numBlocks, blockSize*numBlocks, 1, 1000);
    this->refViewportObject->GetComponentSafe<ncTransform>()->SetEuler(vector3(n_deg2rad(-90), 0, 0));

    //begin attach
    sceneGraph->BeginAttach();
    nGfxServer2::Instance()->BeginFrame();

    firstCell = terrainGMM->GetNorthWestCell();
    for(int bz = 0; bz < numBlocks; bz++)
    {
        ncTerrainGMMCell * currentCell = firstCell;
        for(int bx = 0; bx < numBlocks; bx++)
        {
            n_assert(currentCell);
            //TODO- this should be done in the destructor of ncSpatialLightEnv ma.garcias
            currentCell->GetShaderOverrides().ClearArg(nShaderState::matDiffuse);
            currentCell->GetShaderOverrides().ClearArg(nShaderState::ShadowMap);
            currentCell->GetShaderOverrides().ClearArg(nShaderState::lightMap);
            currentCell->GetShaderOverrides().ClearArg(nShaderState::GlobalLightMap);
            //TODO- render the shadow map one cell at a time, like global terrain texture
            //this->AttachCurrentCellEntities(sceneGraph, currentCell->GetEntityObject(), false);
            currentCell->GetComponent<ncScene>()->Render(sceneGraph);
            this->AttachLightEntity(sceneGraph);
            currentCell = currentCell->GetNeighbor(ncTerrainGMMCell::East);
        }
        firstCell = firstCell->GetNeighbor(ncTerrainGMMCell::South);
    }

    //set viewport to terrain center
    vector3 center(blockSize*numBlocks/2,terrainClass->GetMaxHeight(),blockSize*numBlocks/2);
    this->refViewportObject->GetComponentSafe<ncTransform>()->SetPosition(center);

    //ugly hack, camera nodes don't get the projection from anywhere
    nCamera2 cam = this->refViewportObject->GetComponentSafe<ncViewport>()->GetCamera();
    nGfxServer2::Instance()->SetCamera(cam);

    //attach viewport
    this->AttachViewport(sceneGraph);

    //end attach
    sceneGraph->EndAttach();

    // set all lod levels to minimum
    firstCell = terrainGMM->GetNorthWestCell();
    for(int bz = 0; bz < numBlocks; bz++)
    {
        ncTerrainGMMCell * currentCell = firstCell;
        for(int bx = 0; bx < numBlocks; bx++)
        {
            n_assert(currentCell);
            currentCell->SetGeometryLODLevel(0);
            currentCell->SetMaterialLODLevel(0);
            currentCell = currentCell->GetNeighbor(ncTerrainGMMCell::East);
        }
        firstCell = firstCell->GetNeighbor(ncTerrainGMMCell::South);
    }

    //render scene
    sceneGraph->RenderPass(trfmPassIndex);// compute transforms
    sceneGraph->RenderPass(rtgtPassIndex);// render targets

    nGfxServer2::Instance()->EndFrame();

    this->SaveGlobalShadowMapTexture();

    // restore geometry node original surface
    surfaceNode->Release();
    geometryNode->SetSurface(prevSurface.Get());
    geometryNode->GetSurfaceNode();
    sceneGraph->Release();
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
