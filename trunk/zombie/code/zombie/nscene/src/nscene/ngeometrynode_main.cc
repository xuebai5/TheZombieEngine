#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ngeometrynode_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ngeometrynode.h"
#include "nscene/nsurfacenode.h"
#include "nscene/nscenegraph.h"
#include "nscene/nsceneshader.h"
#include "nscene/ncsceneclass.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ngfxserver2.h"

nNebulaScriptClass(nGeometryNode, "nabstractshadernode");

uint nGeometryNode::uniqueGeometryId = 0;

//------------------------------------------------------------------------------
/**
*/
nGeometryNode::nGeometryNode() :
    worldCoord(false),
    streamIndex(-1)
#ifndef NGAME
   ,wireframeShaderIndex(-1)
#endif
{
    this->geometryId = uniqueGeometryId++;
    this->streamId = this->geometryId;
}

//------------------------------------------------------------------------------
/**
*/
nGeometryNode::~nGeometryNode()
{
    this->UnloadResources();

    // unload stream resources
    if (this->refStreamGeometry.isvalid())
    {
        this->refStreamGeometry->Release();
    }

    // unload surface resources
    if (this->refSurfaceNode.isvalid())
    {
        this->refSurfaceNode->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nGeometryNode::LoadResources()
{
    #ifndef NGAME
    this->wireframeShaderIndex = nSceneServer::Instance()->FindShader("wireframe");
    #endif

    kernelServer->PushCwd(this);
    bool success = true;

    if (this->refStreamGeometry.isvalid())
    {
        this->streamId = this->refStreamGeometry->GetStreamId();

        success = this->refStreamGeometry->LoadResources();
        if (success)
        {
            this->resourcesValid = true;
            this->CacheSurfacePassesByLevel(this->refStreamGeometry->GetSurfaceNode());
        }
    }
    else 
    {
        this->streamId = this->geometryId;

        success = nAbstractShaderNode::LoadResources();
        if (success)
        {
            if (this->refSurfaceNode.isvalid())
            {
                success = this->refSurfaceNode->LoadResources();
                if (success)
                {
                    this->CacheSurfacePassesByLevel(this->refSurfaceNode);
                }
            }
        }
    }

    kernelServer->PopCwd();

    return success;
}

//------------------------------------------------------------------------------
/**
    Unload the resources if refcount has reached zero.
*/
void
nGeometryNode::UnloadResources()
{
    nAbstractShaderNode::UnloadResources();

    if (this->refStreamGeometry.isvalid())
    {
        //can't unload stream geometry resources, they could be shared
        //this->refStreamGeometry->Release();
        this->passesByLevel.Clear();
    }

    if (this->refSurfaceNode.isvalid())
    {
        //FIXME ma.garcias- can't unload surface resources, they could be shared
        //this->refSurfaceNode->UnloadResources();
        this->passesByLevel.Clear();
    }
}

//------------------------------------------------------------------------------
/**
    get set of levels and passes, and cache this for fast attach
*/
void
nGeometryNode::CacheSurfacePassesByLevel(nSurfaceNode* surfaceNode)
{
    n_assert(surfaceNode);

    // get set of levels and passes
    this->passesByLevel.Reset();
    int numLevels = surfaceNode->GetNumLevels();
    int level;
    for (level = 0; level < numLevels; ++level)
    {
        nArray<int>& levelPasses = this->passesByLevel.PushBack(nArray<int>(4, 4));
        int pass;
        for (pass = 0; pass < surfaceNode->GetNumLevelPasses(level); ++pass)
        {
            levelPasses.Append(surfaceNode->GetLevelPassIndex(level, pass));
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryNode::EntityCreated(nEntityObject* entityObject)
{
    //create a cache entry for the entity
    //if the node is used more than once in the entity (decals)
    //all instances will share the same index (which is fine)
    nSceneNode::EntityCreated(entityObject);
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryNode::EntityDestroyed(nEntityObject* entityObject)
{
    nSceneNode::EntityDestroyed(entityObject);
}

//------------------------------------------------------------------------------
/**
    Attach shape to the scene graph, once for every render pass,
    which comes defined by its surface.
*/
void
nGeometryNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    ncScene* renderContext = entityObject->GetComponent<ncScene>();
    n_assert(renderContext);

    #if __NEBULA_STATS__
    //sceneGraph->profAttachGeometry.StartAccum();
    #endif

    // get max level as max from scenegraph and rendercontext, and maxlevels
    int numLevels = this->passesByLevel.Size();
    int level = 0;
    if (numLevels > 1)
    {
        level = entityObject->GetComponent<ncScene>()->GetMaxMaterialLevel();
        level = n_max(sceneGraph->GetMaxMaterialLevel(), level);
        level = n_min(level, numLevels - 1);
    }

    //n_assert(level < this->passesByLevel.Size());
    nArray<int>& levelPasses = this->passesByLevel.At(level);
    int numPasses = levelPasses.Size();
    int pass;
    for (pass = 0; pass < numPasses; ++pass)
    {
        int passIndex = levelPasses.At(pass);
        if (renderContext->GetPassEnabledFlags() & this->passEnabledFlags & (1<<passIndex))
        {
            #if __NEBULA_STATS__
            //sceneGraph->profAttachGeometry.StopAccum();
            #endif
            sceneGraph->AddGroup(passIndex, this, entityObject);
            #if __NEBULA_STATS__
            //sceneGraph->profAttachGeometry.StartAccum();
            #endif
        }
    }

    #if __NEBULA_STATS__
    //sceneGraph->profAttachGeometry.StopAccum();
    #endif

    nSceneNode::Attach(sceneGraph, entityObject);
}

//------------------------------------------------------------------------------
/**
    Perform pre-instancing geometry rendering.
*/
bool
nGeometryNode::Apply(nSceneGraph* sceneGraph)
{
    #ifndef NGAME
    // do not use streaming to draw wireframe overlay (for now)
    if (sceneGraph->GetShaderIndex() != this->wireframeShaderIndex)
    #endif

    if (this->refStreamGeometry.isvalid())
    {
        return this->refStreamGeometry->Apply(sceneGraph);
    }

    return nAbstractShaderNode::Apply(sceneGraph);
}

//------------------------------------------------------------------------------
/**
    Perform post-instancing geometry rendering.

    This is ensured to be called for any geometry node when the last
    instance of the node has been rendered, either because the shader or
    the surface is about to change, or because it was the last in the list.
    Override this method to perform specific post-instancing rendering, eg.
    end a streamed rendering operation, etc.
*/
bool
nGeometryNode::Flush(nSceneGraph* sceneGraph)
{
    #ifndef NGAME
    // do not use streaming to draw wireframe overlay (for now)
    if (sceneGraph->GetShaderIndex() != this->wireframeShaderIndex)
    #endif

    if (this->refStreamGeometry.isvalid())
    {
        return this->refStreamGeometry->Flush(sceneGraph);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Common behavior for all geometry nodes.
    
    nXXXNode::Render(sceneGraph, renderContext)
    {
        if( nGeometryNode::Render(sceneGraph, renderContext) )
        {
            // ...
        }
        // ...
    }
*/
bool
nGeometryNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    #ifndef NGAME
    // do not use streaming to draw wireframe overlay (for now)
    if (sceneGraph->GetShaderIndex() != this->wireframeShaderIndex)
    #endif

    // delegate streamed rendering
    if (this->refStreamGeometry.isvalid())
    {
        return this->refStreamGeometry->Render(sceneGraph, entityObject);
    }

    // animate shader parameters
    this->InvokeAnimators(entityObject);
    
    // set per-shape shader parameters
    nAbstractShaderNode::Render(sceneGraph, entityObject);
    
    // set shader overrides
    // (this is called last to allow overriding per-shape parameters)
    nShader2 *shader = nSceneServer::Instance()->GetShaderAt(sceneGraph->GetShaderIndex()).GetShaderObject();
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    shader->SetParams(renderContext->GetShaderOverrides());
    
    return true;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
nGeometryNode::BindDirtyDependence(nObject* receiver)
{
    nSceneNode::BindDirtyDependence(receiver);
    nSurfaceNode* surface = this->GetSurfaceNode();
    if ( surface )
    {
        surface->BindDirtyDependence(this);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGeometryNode::RecruseSetObjectDirty(bool dirty)
{
    nSceneNode::RecruseSetObjectDirty(dirty);
    nSurfaceNode* surface = this->GetSurfaceNode();
    if ( surface )
    {
        surface->RecruseSetObjectDirty(dirty);
    }
}
#endif //!NGAME