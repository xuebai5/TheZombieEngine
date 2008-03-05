#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ninstscenebuilder.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/ninstscenebuilder.h"
#include "nscene/ninstshapenode.h"
#include "nscene/nsurfacenode.h"
#include "nmaterial/nmaterialserver.h"
#include "nmaterial/nmaterialnode.h"
#include "nmaterial/nmaterial.h"

//------------------------------------------------------------------------------
/**
*/
nInstSceneBuilder::nInstSceneBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nInstSceneBuilder::nInstSceneBuilder(nSceneNode* sceneRoot) :
    refSceneRoot(sceneRoot)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nInstSceneBuilder::~nInstSceneBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Build scene resource using instancing. This replaces every geometry node
    by an instanced version, and asssigns an instanced material to it.

    history:
        - 19-Dec-2005   MA Garcias  created
        - 26-Apr-2006   MA Garcias  keep original resource, use stream
*/
void
nInstSceneBuilder::BuildInstancedScene()
{
    n_assert(this->refSceneRoot.isvalid());

    //if the scene is already loaded, AddRef and return
    nString instScenePath(this->refSceneRoot->GetFullName());
    instScenePath.Append(".instanced");

    nKernelServer* kernelServer = nKernelServer::Instance();
    nSceneNode* sceneRoot = static_cast<nSceneNode*>(nKernelServer::ks->Lookup(instScenePath.Get()));
    if (sceneRoot)
    {
        sceneRoot->AddRef();
        return;
    }
    
    // create a root node for the instanced geometry nodes
    sceneRoot = static_cast<nSceneNode*>(kernelServer->New("nscenenode", instScenePath.Get()));
    n_assert(sceneRoot);

    // traverse down the scene and push an instanced version of every node (w/ the same name)
    kernelServer->PushCwd(sceneRoot);
    this->BuildFromNode(this->refSceneRoot.get());
    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
    Destroy the instanced scene resources of the scene node at hand.

    history:
        - 26-Apr-2006   MA Garcias  created
*/
void
nInstSceneBuilder::DestroyInstancedScene()
{
    n_assert(this->refSceneRoot.isvalid());

    nString instScenePath(this->refSceneRoot->GetFullName());
    instScenePath.Append(".instanced");

    nAutoRef<nSceneNode> refInstScene(instScenePath.Get());
    if (refInstScene.isvalid())
    {
        refInstScene->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Build an instanced version of the scene node at hand.

    history:
        - 19-Dec-2005   MA Garcias  created
        - 26-Apr-2006   MA Garcias  set instanced geometry as stream
*/
void
nInstSceneBuilder::BuildFromNode(nSceneNode* parentNode)
{
    n_assert(parentNode);
    nSceneNode* instNode = 0;

    // create a node depending on the type of the current one
    const char* nodeClass = parentNode->GetClass()->GetName();

    if (!strcmp(nodeClass, "nshapenode") || !strcmp(nodeClass, "nstaticbatchnode"))
    {
        // create a clone shape node for the current geometry
        instNode = static_cast<nSceneNode*>(nKernelServer::ks->New("ninstshapenode", parentNode->GetName()));
        instNode->CopyStateFrom(parentNode);
        nInstShapeNode* instShapeNode = static_cast<nInstShapeNode*>(instNode);

        // set instance stream from the scene class
        instShapeNode->SetShape(parentNode->GetFullName().Get());
        instShapeNode->SetInstanceStream(this->GetInstanceStream());

        // set instanced surface from the current one
        nSurfaceNode* surface = this->BuildFromSurface(static_cast<nGeometryNode*>(parentNode)->GetSurfaceNode());
        instShapeNode->SetSurface(surface->GetFullName().Get());

        // set instanced node as geometry stream
        static_cast<nGeometryNode*>(parentNode)->SetStream(instShapeNode->GetFullName().Get());
    }
    else
    {
        //create dummy nodes to avoid name clashing for shapes
        instNode = static_cast<nSceneNode*>(nKernelServer::ks->New("nscenenode", parentNode->GetName()));
    }

    // push created node
    if (instNode)
    {
        nKernelServer::ks->PushCwd(instNode);
    }

    // traverse down subnodes
    nSceneNode* curChild;
    for (curChild = (nSceneNode*) parentNode->GetHead();
         curChild;
         curChild = (nSceneNode*) curChild->GetSucc())
    {
        this->BuildFromNode(curChild);
    }

    // pop created node if there was one
    if (instNode)
    {
        nKernelServer::ks->PopCwd();
    }
}

//------------------------------------------------------------------------------
/**
    history:
        - 13-Jan-2006   MA Garcias  created
        - 26-Apr-2006   MA Garcias  use material server to register instanced material
*/
nSurfaceNode*
nInstSceneBuilder::BuildFromSurface(nSurfaceNode* surfaceNode)
{
    if (surfaceNode->IsA("nmaterialnode"))
    {
        nMaterialServer *materialServer = nMaterialServer::Instance();

        //build instanced material (or reuse if there is one available)
        nMaterial* material = static_cast<nMaterialNode*>(surfaceNode)->GetMaterialObject();
        n_assert(material);

        nMaterial* instMaterial = materialServer->NewMaterialFromMaterial(material);
        n_assert(instMaterial);
        instMaterial->AddParam("deform", "instanced");
        material = materialServer->FindSameMaterial(instMaterial);
        if (material)
        {
            material->AddRef();
            instMaterial->Release();
            instMaterial = material;
        }

        //clone the material node, set the cloned material
        nMaterialNode* instMaterialNode = static_cast<nMaterialNode*>(materialServer->NewSurfaceFromSurface(surfaceNode));
        n_assert(instMaterialNode);
        instMaterialNode->SetMaterial(instMaterial->GetFullName().Get());
        nMaterialNode* materialNode = static_cast<nMaterialNode*>(materialServer->FindSameSurface(instMaterialNode));
        if (materialNode)
        {
            materialNode->AddRef();
            instMaterialNode->Release();
            instMaterialNode = materialNode;
        }

        return instMaterialNode;
    }

    nString instSurfacePath = surfaceNode->GetFullName();
    instSurfacePath.Append(".instanced");
    nSurfaceNode* instSurfaceNode = 0;

    //instSurfaceNode = ...ideally, get the instanced version from a database, etc.
    //for now, set a fixed surface (with no material cases):
    instSurfaceNode = static_cast<nSurfaceNode*>(nKernelServer::ks->New("nsurfacenode", instSurfacePath.Get()));
    if (surfaceNode->GetTexture(nShaderState::clipMap))
    {   
        instSurfaceNode->SetShader(FOURCC('inst'), "shaders:default_inst_clipmap.fx");
    }
    else
    {
        instSurfaceNode->SetShader(FOURCC('inst'), "shaders:default_inst.fx");
    }
    instSurfaceNode->GetShaderParams().Copy(surfaceNode->GetShaderParams());
    int tex;
    for (tex = 0; tex < surfaceNode->GetNumTextures(); ++tex)
    {
        instSurfaceNode->SetTexture(surfaceNode->GetTextureParamAt(tex), surfaceNode->GetTextureAt(tex));
    }
    //@todo ma.garcias- copy SetUv... as well

    return instSurfaceNode;
}
