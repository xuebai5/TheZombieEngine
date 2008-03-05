#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntransformnode_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/ntransformnode.h"
#include "nscene/nscenegraph.h"
#include "gfx2/ngfxserver2.h"
#include "nscene/nanimator.h"

nNebulaScriptClass(nTransformNode, "nscenenode");

//------------------------------------------------------------------------------
/**
*/
nTransformNode::nTransformNode() :
    transformFlags(Active),
    trfmPassIndex(-1),
    hasTransform(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTransformNode::~nTransformNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Attach to the scene graph.
*/
void
nTransformNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);

    //optimization: do not attach if uninitialized
    if (this->GetNumAnimators() == 0 && !this->HasTransform())
    {
        nSceneNode::Attach(sceneGraph, entityObject);
        return;
    }

    #if __NEBULA_STATS__
    //sceneGraph->profAttachTransform.Start();
    #endif

    if (this->trfmPassIndex == -1)
    {
        this->trfmPassIndex = nSceneServer::Instance()->GetPassIndexByFourCC(FOURCC('trfm'));
    }
    
    if (this->CheckFlags(Active))
    {
        sceneGraph->BeginGroup(this->trfmPassIndex, this, entityObject);
        #if __NEBULA_STATS__
        //sceneGraph->profAttachTransform.StopAccum();
        #endif

        nSceneNode::Attach(sceneGraph, entityObject);

        #if __NEBULA_STATS__
        //sceneGraph->profAttachTransform.Start();
        #endif
        sceneGraph->EndGroup();
    }

    #if __NEBULA_STATS__
    //sceneGraph->profAttachTransform.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
    Compute the resulting modelview matrix and set it in the scene
    server as current modelview matrix.
*/
bool
nTransformNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    n_assert(sceneGraph);
    n_assert(entityObject);

    this->InvokeAnimators(entityObject);
    if (this->GetLockViewer())
    {
        // handle lock to viewer
        const matrix44& viewMatrix = nGfxServer2::Instance()->GetTransform(nGfxServer2::InvView);
        matrix44 m = this->tform.getmatrix();
        m = m * sceneGraph->GetModelTransform();
        m.M41 = viewMatrix.M41;
        m.M42 = viewMatrix.M42;
        m.M43 = viewMatrix.M43;
        sceneGraph->SetModelTransform(m);
    }
    else
    {
        // default case
        sceneGraph->SetModelTransform(this->tform.getmatrix() * sceneGraph->GetModelTransform());
    }
    return true;
}
