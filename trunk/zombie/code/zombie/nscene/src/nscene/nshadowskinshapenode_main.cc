#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nshadowskinshapenode_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nshadowskinshapenode.h"
#include "nscene/nskinshapenode.h"
#include "nscene/nskinanimator.h"
#include "nscene/nscenegraph.h"

nNebulaScriptClass(nShadowSkinShapeNode, "nscenenode");

// HACK!
const float nShadowSkinShapeNode::maxDistance = 20.0f;

//------------------------------------------------------------------------------
/**
*/
nShadowSkinShapeNode::nShadowSkinShapeNode() :
    shadPassIndex(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nShadowSkinShapeNode::~nShadowSkinShapeNode()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Unload mesh resource if valid.
*/
void
nShadowSkinShapeNode::UnloadShadowCaster()
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
nShadowSkinShapeNode::LoadShadowCaster()
{
    nSkinShadowCaster* shadowCaster;
    shadowCaster = (nSkinShadowCaster*) nShadowServer::Instance()->NewShadowCaster(nShadowServer::Skin, this->GetSkinShapeNode()->GetMesh());
    n_assert(shadowCaster);

    if (!shadowCaster->IsValid())
    {
        shadowCaster->SetFilename(this->GetSkinShapeNode()->GetMesh());
        n_assert(shadowCaster->Load());
    }
    
    this->refShadowCaster = shadowCaster;
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nShadowSkinShapeNode::LoadResources()
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
nShadowSkinShapeNode::UnloadResources()
{
    nSceneNode::UnloadResources();
    this->UnloadShadowCaster();
}

//------------------------------------------------------------------------------
/**
    Called back by the scene graph when the node is attached to the scene.
*/
void
nShadowSkinShapeNode::Attach(nSceneGraph *sceneGraph, nEntityObject* entityObject)
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
nShadowSkinShapeNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{   
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // compute distance to viewer
    const matrix44& modelMatrix = sceneGraph->GetModelTransform();
    const vector3& viewerPos = gfxServer->GetTransform(nGfxServer2::InvView).pos_component();
    const vector3& worldPos  = modelMatrix.pos_component();
    float dist = vector3::distance(viewerPos, worldPos);
    if (dist < maxDistance)
    {
        // render the shadow volume
        n_assert(sceneGraph);
        n_assert(entityObject);

        // call parent skin shape animator (updates the char skeleton pointer)
        this->GetSkinShapeNode()->InvokeAnimators(entityObject);
        
        this->refShadowCaster->SetCharSkeleton(this->GetSkinShapeNode()->GetCharSkeleton());
        this->refShadowCaster->RenderShadow(modelMatrix, this->GetSkinShapeNode()->GetGroupIndex());
    }
    return true;
}
