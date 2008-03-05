#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nimpostornode_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nimpostornode.h"

nNebulaScriptClass(nImpostorNode, "ngeometrynode");

//------------------------------------------------------------------------------
/**
*/
nImpostorNode::nImpostorNode() :
    curVertexPtr(0),
    maxVertices(0),
    curVertex(0),
    curIndex(0)
{
    this->SetWorldCoord(true);
}

//------------------------------------------------------------------------------
/**
*/
nImpostorNode::~nImpostorNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nImpostorNode::LoadResources()
{
    return nGeometryNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
nImpostorNode::UnloadResources()
{
    nGeometryNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
nImpostorNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    return nGeometryNode::Attach(sceneGraph, entityObject);
}

//------------------------------------------------------------------------------
/**
*/
bool
nImpostorNode::Apply(nSceneGraph* sceneGraph)
{
    if (nGeometryNode::Apply(sceneGraph))
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        nShader2* curShader = gfxServer->GetShader();

        // set scale factor for the impostor
        if (curShader->IsParameterUsed(nShaderState::Scale))
        {
            curShader->SetFloat(nShaderState::Scale, this->localBox.extents().y);
        }

        // reset number of instances
        N_IFDEF_NLOG(this->numInstancesDrawn = 0);

        // setup and reset the dynamic mesh
        if (!this->dynMesh.IsValid())
        {
            this->dynMesh.Initialize(nGfxServer2::TriangleList,
                                     nMesh2::Coord | nMesh2::Uv0 | nMesh2::Uv1,
                                     nMesh2::WriteOnly | nMesh2::NeedsVertexShader, 
                                     false);//!indexed
            n_assert(this->dynMesh.IsValid());
        }

        // check that last rendering worked ok!
        n_assert(this->curVertexPtr == 0);
        n_assert(this->maxVertices == 0);
        n_assert(this->curVertex == 0);
        n_assert(this->curIndex == 0);
        this->dynMesh.Begin(this->curVertexPtr, this->maxVertices);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nImpostorNode::Render(nSceneGraph* sceneGraph, nEntityObject* /*entityObject*/)
{
    //do not call nGeometryNode::Render, it performs rendering of instances
    //nGeometryNode::Render(sceneGraph, entityObject);

    //TODO- if the current impostor doesn't fit the dynamic mesh, swap it
    //...

    N_IFDEF_NLOG(++this->numInstancesDrawn);

    // fill the dynamic array with info on this instance
    const vector2 spriteCorners[4] = { vector2(-1.0, -1.0),
                                       vector2(-1.0,  1.0),
                                       vector2(1.0,   1.0),
                                       vector2(1.0,  -1.0) };

    this->curVertex += 6;
    //const vector3& curPosition = sceneGraph->GetModelTransform().pos_component();
    const bbox3& localBox = this->GetLocalBox();
    vector3 curPosition = sceneGraph->GetModelTransform() * localBox.center();
    float*& dstVertices = this->curVertexPtr;

    // TODO transform center of local bbox, use extents as extrude vector

    // vertex 0
    dstVertices[curIndex++] = curPosition.x;
    dstVertices[curIndex++] = curPosition.y;
    dstVertices[curIndex++] = curPosition.z;

    dstVertices[curIndex++] = 0.0f;
    dstVertices[curIndex++] = 1.0f;

    dstVertices[curIndex++] = spriteCorners[0].x;
    dstVertices[curIndex++] = spriteCorners[0].y;

    // vertex 1
    dstVertices[curIndex++] = curPosition.x;
    dstVertices[curIndex++] = curPosition.y;
    dstVertices[curIndex++] = curPosition.z;

    dstVertices[curIndex++] = 0.0f;
    dstVertices[curIndex++] = 0.0f;

    dstVertices[curIndex++] = spriteCorners[1].x;
    dstVertices[curIndex++] = spriteCorners[1].y;

    // vertex 2
    dstVertices[curIndex++] = curPosition.x;
    dstVertices[curIndex++] = curPosition.y;
    dstVertices[curIndex++] = curPosition.z;

    dstVertices[curIndex++] = 1.0f;
    dstVertices[curIndex++] = 0.0f;

    dstVertices[curIndex++] = spriteCorners[2].x;
    dstVertices[curIndex++] = spriteCorners[2].y;

    // vertex 3
    dstVertices[curIndex++] = curPosition.x;
    dstVertices[curIndex++] = curPosition.y;
    dstVertices[curIndex++] = curPosition.z;

    dstVertices[curIndex++] = 0.0f;
    dstVertices[curIndex++] = 1.0f;

    dstVertices[curIndex++] = spriteCorners[0].x;
    dstVertices[curIndex++] = spriteCorners[0].y;

    // vertex 4
    dstVertices[curIndex++] = curPosition.x;
    dstVertices[curIndex++] = curPosition.y;
    dstVertices[curIndex++] = curPosition.z;

    dstVertices[curIndex++] = 1.0f;
    dstVertices[curIndex++] = 0.0f;

    dstVertices[curIndex++] = spriteCorners[2].x;
    dstVertices[curIndex++] = spriteCorners[2].y;

    // vertex 5
    dstVertices[curIndex++] = curPosition.x;
    dstVertices[curIndex++] = curPosition.y;
    dstVertices[curIndex++] = curPosition.z;

    dstVertices[curIndex++] = 1.0f;
    dstVertices[curIndex++] = 1.0f;

    dstVertices[curIndex++] = spriteCorners[3].x;
    dstVertices[curIndex++] = spriteCorners[3].y;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nImpostorNode::Flush(nSceneGraph* N_IFNDEF_NGAME(sceneGraph))
{
    NLOG(renderpass, (3, "  - nImpostorNode flushing %d instances", this->numInstancesDrawn))

    #ifndef NGAME
    if (sceneGraph->GetShaderIndex() == this->wireframeShaderIndex)
    {
        nShader2* curShader = nGfxServer2::Instance()->GetShader();
        if (curShader->IsParameterUsed(nShaderState::isSkinned))
        {
            curShader->SetInt(nShaderState::isSkinned, 2);//IMPOSTOR
        }
        if (curShader->IsParameterUsed(nShaderState::CullMode))
        {
            curShader->SetInt(nShaderState::CullMode, nShaderState::NoCull);
        }
        
    }
    #endif

    nGfxServer2::Instance()->PushTransform(nGfxServer2::Model, matrix44());
    this->dynMesh.End(curVertex);
    nGfxServer2::Instance()->PopTransform(nGfxServer2::Model);

    this->curVertexPtr = 0;
    this->maxVertices = 0;
    this->curVertex = 0;
    this->curIndex = 0;

    return true;
}
