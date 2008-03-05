#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nabstractcameranode_main.cc
//  (C) 2004 RadonLabs GmbH
//  author: matthias
//------------------------------------------------------------------------------
#include "nscene/nabstractcameranode.h"
#include "kernel/nfileserver2.h"

nNebulaScriptClass(nAbstractCameraNode, "nrenderpathnode");

//------------------------------------------------------------------------------
/**
*/
nAbstractCameraNode::nAbstractCameraNode() :
    rtgtPassIndex(-1)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
nAbstractCameraNode::~nAbstractCameraNode()
{
    // unload the resources
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
bool 
nAbstractCameraNode::HasCamera() const
{
    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nAbstractCameraNode::RenderCamera(const matrix44& /*modelWorldMatrix*/, const matrix44& /*viewMatrix*/, const matrix44& /*projectionMatrix*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nAbstractCameraNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    // get pass indices for the renderTarget and View scene passes
    if (this->rtgtPassIndex == -1)
    {
        this->rtgtPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('rtgt'));
    }

    // attach to "view" pass if there are passes that render to the frame buffer
    sceneGraph->AddGroup(this->rtgtPassIndex, this, entityObject);

    nRenderPathNode::Attach(sceneGraph, entityObject);
}

//------------------------------------------------------------------------------
/**
*/
bool
nAbstractCameraNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    nGfxServer2 *gfxServer = nGfxServer2::Instance();

    this->RenderCamera(sceneGraph->GetModelTransform(), 
                       gfxServer->GetTransform(nGfxServer2::View),
                       gfxServer->GetTransform(nGfxServer2::Projection));

    gfxServer->PushTransform(nGfxServer2::View, this->GetViewMatrix());
    gfxServer->PushTransform(nGfxServer2::Projection, this->GetProjectionMatrix());

    this->DoRenderPath(sceneGraph, entityObject);

    gfxServer->PopTransform(nGfxServer2::View);
    gfxServer->PopTransform(nGfxServer2::Projection);

    return true;
}
