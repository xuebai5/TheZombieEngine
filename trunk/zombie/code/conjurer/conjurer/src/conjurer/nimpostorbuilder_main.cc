#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nimpostorbuilder_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nimpostorbuilder.h"
#include "ndebug/nceditorclass.h"
#include "nscene/ncscenelodclass.h"
#include "nspatial/ncspatialclass.h"
#include "nspatial/ncspatial.h"
#include "zombieentity/ncassetclass.h"
#include "zombieentity/ncloader.h"
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/nscenegraph.h"
#include "nscene/ncviewport.h"
#include "tools/ntexturebuilder.h"
#include "nmaterial/nmaterialserver.h"
#include "mathlib/rectanglei.h"

int nImpostorBuilder::uniqueSurfaceId = 0;

namespace
{
    //ColorBufferName, NormalBufferName must be the same as in impostor_renderpath.xml
    const char *ColorBufferName("ImpostorColorBuffer");
    const char *NormalBufferName("ImpostorNormalBuffer");
    const char *ViewportClassName("Impostorcam");
    const char *ViewportClassResource("wc:libs/system/cameras/impostorcam.n2");
}

//------------------------------------------------------------------------------
/**
*/
nImpostorBuilder::nImpostorBuilder() :
    sourceLevel(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nImpostorBuilder::~nImpostorBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nImpostorBuilder::BuildImpostorAsset()
{
    //get access to source entity class
    n_assert(!this->className.IsEmpty());
    nEntityClass* sourceClass = nEntityClassServer::Instance()->GetEntityClass(this->className.Get());
    if (!sourceClass)
    {
        return;
    }
    //check for all required components within the source
    ncEditorClass* editorClass = sourceClass->GetComponent<ncEditorClass>();
    ncAssetClass* assetClass = sourceClass->GetComponent<ncAssetClass>();
    ncSpatialClass* spatialClass = sourceClass->GetComponent<ncSpatialClass>();
    ncSceneLodClass* sceneLodClass = sourceClass->GetComponent<ncSceneLodClass>();
    if (!editorClass || !assetClass || !spatialClass)
    {
        return;
    }
    //TODO- if there is a single level, move to scene level 0, build impostor as 1
    if (!sceneLodClass || assetClass->GetNumLevels() == 0)
    {
        return;
    }
    //generate impostor asset name, as a[className]_impostor
    this->impostorAssetName.Format("a%s_impostor", this->className.Get());
    //set the nceditor class key "impostorResource" if not set.
    editorClass->SetClassKeyString("impostorResource", this->impostorAssetName);
    //TODO- if the asset already exists, reuse it somehow?

    //somehow identify that it needs saving with the imposted class is saved.
    int impostorLevel;
    if (editorClass->IsSetClassKey("impostorLevel"))
    {
        impostorLevel = editorClass->GetClassKeyInt("impostorLevel");
    }
    else
    {
        impostorLevel = assetClass->GetNumLevels();
    }
    //build the impostor textures
    this->sourceLevel = impostorLevel - 1;
    this->BuildImpostorTextures();
    //build a scene resource using them, 
    this->impostorBBox = spatialClass->GetOriginalBBox();
    nSceneNode* sceneRoot = this->BuildImpostorScene();
    if (!sceneRoot)
    {
        return;
    }
    //TODO- somehow identify that it needs saving with the imposted class is saved.
    editorClass->SetClassKeyInt("impostorLevel", impostorLevel);
    editorClass->SetClassKeyInt("editableLevel", impostorLevel);
    //set the name of the impostor resource into the asset component
    nString assetPath("wc:export/assets/");
    assetPath.Append(this->impostorAssetName);
    assetClass->SetLevelResource(impostorLevel, assetPath.Get());
    assetClass->SetAssetEditable(true);
    //set the scene root as an additional detail level into the target class
    sceneLodClass->SetLevelRoot(impostorLevel, sceneRoot);
    //set lod distance for the impostor level in the spatial class comp-
    spatialClass->SetLevelDistance(impostorLevel, 1000);
    //set class as dirty, to force saving
    nEntityClassServer::Instance()->SetEntityClassDirty(sourceClass, true);
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode*
nImpostorBuilder::BuildImpostorScene()
{
    n_assert(!this->impostorAssetName.IsEmpty());
    //build a node with the impostor asset name
    //set a single impostor node, with a fixed impostor surface.
    //(do not use materials for impostors for now- useless)
    nAutoRef<nRoot> refLibScene("/lib/scene");
    n_assert(refLibScene.isvalid());//TEMP
    nSceneNode* rootNode = static_cast<nSceneNode*>(refLibScene->Find(this->impostorAssetName.Get()));
    if (rootNode)
    {
        //TODO- rebuild impostor scene if it is already there?
        nGeometryNode* impostorNode = static_cast<nGeometryNode*>(rootNode->Find("impostor.0"));
        if (impostorNode && impostorNode->IsA("nimpostornode"))
        {
            nSurfaceNode* surface = impostorNode->GetSurfaceNode();
            surface->SetTexture(nShaderState::diffMap, this->impostorTexturePath.Get());
            surface->SetTexture(nShaderState::bumpMap, this->impostorNormalPath.Get());
            surface->LoadResources();
        }
        return rootNode;
    }

    //create the root transform node
    nKernelServer::ks->PushCwd(refLibScene.get());
    rootNode = static_cast<nSceneNode*>(nKernelServer::ks->New("ntransformnode", this->impostorAssetName.Get()));
    n_assert(rootNode);
    nKernelServer::ks->PopCwd();
    //create an impostor geometry node
    nKernelServer::ks->PushCwd(rootNode);
    nGeometryNode* impostorNode;
    impostorNode = static_cast<nGeometryNode*>(nKernelServer::ks->New("nimpostornode", "impostor.0"));
    nKernelServer::ks->PopCwd();
    n_assert(impostorNode);

    //build impostor surface, or reuse one
    //TEMP- use a standard surface for all impostors generated this way
    nSurfaceNode* surface = nMaterialServer::Instance()->LoadSurfaceFromFile("wc:libs/system/materials/surface0_impostor.n2");
    nString impostorSurfacePath;
    impostorSurfacePath.Format("/tmp/impostorbuilder/surface%04u", uniqueSurfaceId++);
    surface = static_cast<nSurfaceNode*>(surface->Clone(impostorSurfacePath.Get()));
    surface->SetTexture(nShaderState::diffMap, this->impostorTexturePath.Get());
    surface->SetTexture(nShaderState::bumpMap, this->impostorNormalPath.Get());

    //nSurfaceNode* surface = refLibSurfaces->Find();
    //refLibSurfaces->
    impostorNode->SetSurface(surface->GetFullName().Get());
    impostorNode->SetLocalBox(this->impostorBBox);

    return rootNode;
}

//------------------------------------------------------------------------------
/**
*/
void
nImpostorBuilder::BuildImpostorTextures()
{
    //build the render target texture (before loading the impostor render path)
    //SetupImpostorTexture()
    this->impostorTexturePath.Clear();
    const int texSize = 128;
    const nTexture2::Format texFormat = nTexture2::A8R8G8B8;
    nTexture2* colorRenderTarget = nGfxServer2::Instance()->NewRenderTarget(ColorBufferName, texSize, texSize, texFormat, nTexture2::RenderTargetColor|nTexture2::RenderTargetDepth);
    if (!colorRenderTarget)
    {
        n_message("nImpostorBuilder::BuildImpostorTextures() failed: couldn't create render target '%s'", ColorBufferName);
        return;
    }

    nTexture2* normalRenderTarget = nGfxServer2::Instance()->NewRenderTarget(NormalBufferName, texSize, texSize, texFormat, nTexture2::RenderTargetColor|nTexture2::RenderTargetDepth);
    if (!normalRenderTarget)
    {
        n_message("nImpostorBuilder::BuildImpostorTextures() failed: couldn't create render target '%s'", NormalBufferName);
        return;
    }

    //SetupImpostorRender()
    //build a builder render path, and a local viewport
    const int LEVEL_EDITOR = 4;//TODO- customize from the tool
    //const char *viewportClassName("Impostorcam");
    //const char *viewportClassResource("wc:libs/system/cameras/impostorcam.n2");
    nEntityClass* viewportClass = nEntityClassServer::Instance()->GetEntityClass("neviewport");
    viewportClass = nEntityClassServer::Instance()->NewEntityClass(viewportClass, ViewportClassName);
    viewportClass->GetComponentSafe<ncLoaderClass>()->SetResourceFile(ViewportClassResource);
    if (viewportClass->GetComponentSafe<ncLoaderClass>()->LoadResources())
    {
        //build an entity of the source class and a scene graph, set the camera
        nEntityObject* viewportEntity = nEntityObjectServer::Instance()->NewLocalEntityObject(ViewportClassName);
        n_assert(viewportEntity);
        if (viewportEntity->GetComponentSafe<ncLoader>()->LoadComponents())
        {
            nEntityObject* sourceEntity = nEntityObjectServer::Instance()->NewLocalEntityObject(this->className.Get());
            n_assert(sourceEntity);
            if (sourceEntity->GetClassComponentSafe<ncLoaderClass>()->LoadResources() &&
                sourceEntity->GetComponentSafe<ncLoader>()->LoadComponents())
            {
                nSceneServer* sceneServer = nSceneServer::Instance();
                int trfmPassIndex = sceneServer->GetPassIndexByFourCC(FOURCC('trfm'));
                int rtgtPassIndex = sceneServer->GetPassIndexByFourCC(FOURCC('rtgt'));
                nSceneGraph* sceneGraph = sceneServer->NewSceneGraph("impostor");
                //and render offline to texture(s) as many times as needed.
                sourceEntity->GetComponentSafe<ncScene>()->SetAttachIndex(this->sourceLevel);
                int prevMaterialLevel = sourceEntity->GetClassComponentSafe<ncSceneClass>()->GetMaxMaterialLevel();
                sourceEntity->GetClassComponentSafe<ncSceneLodClass>()->SetLevelMaterialProfile(this->sourceLevel, LEVEL_EDITOR);

                //BeginImpostorRender();
                //position: looking at the center of the entity bbox, from at least the bbox distance * 10
                const bbox3& lookAtBox = sourceEntity->GetComponent<ncSpatial>()->GetBBox();
                vector3 camPos(lookAtBox.center());
                camPos.z = camPos.z + (lookAtBox.extents().z * 10.0f);
                float projSize = n_max(lookAtBox.extents().x, lookAtBox.extents().y) * 2.0f;
                viewportEntity->GetComponent<ncTransform>()->SetPosition(camPos);
                viewportEntity->GetComponent<ncViewport>()->SetOrthogonal(projSize, projSize, 1, 1000);
                nCamera2 cam = viewportEntity->GetComponentSafe<ncViewport>()->GetCamera();
                nGfxServer2::Instance()->SetCamera(cam);
                //rotation: none, look in the Z- axis for now

                //RenderImpostor()
                //TODO- repeat for all angles from which we want to create an impostor
                //attach'em all
                nGfxServer2::Instance()->BeginFrame();
                sceneGraph->BeginAttach();
                viewportEntity->GetComponent<ncScene>()->Render(sceneGraph);
                sourceEntity->GetComponent<ncScene>()->Render(sceneGraph);
                sceneGraph->EndAttach();
                //render'em all
                sceneGraph->RenderPass(trfmPassIndex);
                sceneGraph->RenderPass(rtgtPassIndex);
                nGfxServer2::Instance()->EndFrame();

                //SaveImpostorTexture()
                //define a convention to save all these source textures.
                //save the texture(s) to wc:export/assets/[assetName]/textures
                nString texturesPath;
                texturesPath.Format("wc:export/assets/%s/textures/", this->impostorAssetName.Get());
                nKernelServer::ks->GetFileServer()->MakePath(texturesPath);
                //save impostor color texture
                nString texturePath(texturesPath);
                texturePath.Append("impostor.dds");
                if (this->SaveRenderTarget(colorRenderTarget, texturePath.Get()))
                {
                    this->impostorTexturePath = texturePath.Get();
                }
                //save impostor normal
                texturePath.Set(texturesPath.Get());
                texturePath.Append("impostor_NORMAL.dds");
                if (this->SaveRenderTarget(normalRenderTarget, texturePath.Get()))
                {
                    this->impostorNormalPath = texturePath.Get();
                }

                //CleanupImpostorRender()
                //restore values, get rid of all helper objects
                sourceEntity->GetClassComponentSafe<ncSceneLodClass>()->SetLevelMaterialProfile(this->sourceLevel, prevMaterialLevel);
                sceneGraph->Release();
            }
            nEntityObjectServer::Instance()->RemoveEntityObject(sourceEntity);
        }
        nEntityObjectServer::Instance()->RemoveEntityObject(viewportEntity);
        viewportClass->GetComponentSafe<ncLoaderClass>()->UnloadResources();
    }
    nEntityClassServer::Instance()->RemoveEntityClass(ViewportClassName);
    //FIXME- this triggers an assert when removing the texture
    //renderTarget->Release();
}

//------------------------------------------------------------------------------
/**
*/
bool
nImpostorBuilder::SaveRenderTarget(nTexture2* renderTarget, const char *texturePath)
{
    nTexture2* copyTexture = this->CopyTextureFrom(renderTarget);
    n_assert(copyTexture);
    nTextureBuilder texBuilder;
    texBuilder.SetTexture(copyTexture);
    bool success = texBuilder.Save(texturePath);
    if (!success)
    {
        n_message("nImpostorBuilder::BuildImpostorTextures() failed: couldn't save texture '%s'", texturePath);
    }
    copyTexture->Release();
    return success;
}

//------------------------------------------------------------------------------
/**
*/
nTexture2*
nImpostorBuilder::CopyTextureFrom(nTexture2* src)
{
    ushort width = src->GetWidth();
    ushort height = src->GetHeight();
    nTexture2* dst = nGfxServer2::Instance()->NewTexture(0);
    n_assert(dst);
    dst->SetType(nTexture2::TEXTURE_2D);
    dst->SetFormat(nTexture2::A8R8G8B8);
    dst->SetUsage(nTexture2::CreateEmpty | nTexture2::Dynamic);
    dst->SetWidth(width);
    dst->SetHeight(height);
    dst->Load();
    n_assert(dst->IsLoaded());

    rectanglei rect( 0, 0, width - 1, height - 1);
    src->Copy(dst, rect, rect, nTexture2::FILTER_POINT);
    return dst;
}
