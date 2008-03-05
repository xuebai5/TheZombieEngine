#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nviewportnode_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nviewportnode.h"
#include "nscene/nscenegraph.h"
#include "nscene/nsceneserver.h"
#include "nscene/ncviewport.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nViewportNode, "nrenderpathnode");

//------------------------------------------------------------------------------
/**
*/
nViewportNode::nViewportNode() :
    bgColor(0.5f, 0.5f, 0.5f, 1.0f),
    viewPassIndex(-1),
    rtgtPassIndex(-1),
    override(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nViewportNode::~nViewportNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nViewportNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    if (this->viewPassIndex == -1)
    {
        this->viewPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('view'));
    }
    if (this->rtgtPassIndex == -1)
    {
        this->rtgtPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('rtgt'));
    }

    sceneGraph->AddGroup(this->viewPassIndex, this, entityObject);

    if (this->GetOverride())
    {
        sceneGraph->AddGroup(this->rtgtPassIndex, this, entityObject);
    }

    nRenderPathNode::Attach(sceneGraph, entityObject);
}

//------------------------------------------------------------------------------
/**
*/
void
nViewportNode::ApplyOverride(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(this->GetOverride());
    if (sceneGraph->BeginRender(this->viewPassIndex))
    {
        do {
            if (sceneGraph->GetCurrentEntity() != entityObject)
            {
                ncScene* viewContext = (ncScene*) sceneGraph->GetCurrentEntity()->GetComponentSafe<ncScene>();
                viewContext->SetPassEnabledFlags(viewContext->GetPassEnabledFlags() & ~(1<<this->viewPassIndex));
            }
        }
        while (sceneGraph->Next());
        sceneGraph->EndRender();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nViewportNode::RenderOverride(nSceneGraph* sceneGraph, nEntityObject* /*entityObject*/)
{
    n_assert(this->GetOverride());
    int currentIndex = sceneGraph->GetCurrentIndex();
    //HACK- we'll assume that the overriden viewport is always the first
    //in the pass
    sceneGraph->SetCurrentIndex(0);
    //do current render path with viewport entity to get its ncviewport
    nRenderPathNode::Render(sceneGraph, (nEntityObject*) sceneGraph->GetCurrentEntity());
    //restore view pass
    ncScene* viewContext = (ncScene*) sceneGraph->GetCurrentEntity()->GetComponentSafe<ncScene>();
    viewContext->SetPassEnabledFlags(viewContext->GetPassEnabledFlags() | (1<<this->viewPassIndex));
    //restore index of group within pass
    sceneGraph->SetCurrentIndex(currentIndex);
}

//------------------------------------------------------------------------------
/**
    Invokes rendering of the rest of scene into a selected viewport.
    The set of passes it renders can be customized through the render passes.
    the view pass can be disabled from some other entity to override this.
*/
bool
nViewportNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    //render target pass- override other viewport in the scene to change its behavior
    if (sceneGraph->GetCurrentPassIndex() == this->rtgtPassIndex)
    {
        this->ApplyOverride(sceneGraph, entityObject);
        return true;
    }

    //view pass- do render path with current entity if enabled
    ncScene* renderContext = entityObject->GetComponent<ncScene>();
    if (renderContext->GetPassEnabledFlags() & (1<<this->viewPassIndex))
    {
        //otherwise, do render path with overriden viewport entity
        if (this->GetOverride())
        {
            this->RenderOverride(sceneGraph, entityObject);
        }
        else
        {
            nRenderPathNode::Render(sceneGraph, entityObject);
        }
        return true;
    }

    return false;
}
