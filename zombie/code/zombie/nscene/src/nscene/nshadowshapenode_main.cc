#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nshadowshapenode_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nshadowshapenode.h"
#include "nscene/nscenegraph.h"

nNebulaClass(nShadowShapeNode, "nscenenode");

//------------------------------------------------------------------------------
/**
*/
nShadowShapeNode::nShadowShapeNode() :
    shadPassIndex(-1)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nShadowShapeNode::~nShadowShapeNode()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Unload mesh resource if valid.
*/
void
nShadowShapeNode::UnloadShadowCaster()
{
    if (this->refShadowCaster.isvalid())
    {
        this->refShadowCaster->Release();
        this->refShadowCaster.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Setup the ShadowCaster
*/
bool
nShadowShapeNode::LoadShadowCaster()
{
    kernelServer->PushCwd(this);
    nStaticShadowCaster* shadowCaster;
    shadowCaster = (nStaticShadowCaster *) nShadowServer::Instance()->NewShadowCaster(nShadowServer::Static, this->GetShapeNode()->GetMesh());
    n_assert(shadowCaster);
    
    if (!shadowCaster->IsValid())
    {
        shadowCaster->SetFilename(this->GetShapeNode()->GetMesh());
        n_assert(shadowCaster->Load());
    }
    
    this->refShadowCaster = shadowCaster;
    kernelServer->PopCwd();
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nShadowShapeNode::LoadResources()
{
    if (nSceneNode::LoadResources())
    {
        if (this->LoadShadowCaster())
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nShadowShapeNode::UnloadResources()
{
    nSceneNode::UnloadResources();
    this->UnloadShadowCaster();
}

//------------------------------------------------------------------------------
/**
    Called back by the scene graph when the node is attached to the scene.
*/
void
nShadowShapeNode::Attach(nSceneGraph *sceneGraph, nEntityObject* entityObject)
{
    if (this->shadPassIndex == -1)
    {
        this->shadPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('shad'));
    }
    sceneGraph->AddGroup(this->shadPassIndex, this, entityObject);
}

//------------------------------------------------------------------------------
/**
*/
bool
nShadowShapeNode::Render(nSceneGraph *sceneGraph, nEntityObject* /*entityObject*/)
{
    // HACK:
    // check distance for small objects < smaller 4 meters diagonal)
    // should be replaced by some proper LODing!
    matrix44 modelTransform = sceneGraph->GetModelTransform();
    bool cull = false;
    //if (this->GetLocalBox().diagonal_size() < 4.0f)
    //{
    //    nGfxServer2* gfxServer = nGfxServer2::Instance();
    //    const vector3& viewerPos = gfxServer->GetTransform(nGfxServer2::InvView).pos_component();
    //    const vector3& worldPos  = modelMatrix.pos_component();
    //    float dist = vector3::distance(viewerPos, worldPos);
    //    if (dist > maxSmallObjectDistance)
    //    {
    //        cull = true;
    //    }
    //}
    
    // render the shadow volume
    if (!cull)
    {
        this->refShadowCaster->RenderShadow(modelTransform, this->GetShapeNode()->GetGroupIndex());
    }
    return true;
}
