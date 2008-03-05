#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ninstshapenode_main.cc
//  (C) Conjurer Services, S.A. 2005
//------------------------------------------------------------------------------
#include "nscene/ninstshapenode.h"
#include "nscene/nbatchmeshloader.h"
#include "gfx2/ninstancestream.h"

#include "kernel/nlogclass.h"

nNebulaScriptClass(nInstShapeNode, "ngeometrynode");

//------------------------------------------------------------------------------
/**
*/
nInstShapeNode::nInstShapeNode():
    numBatchedInstances(MaxBatchedInstances),
    numVerticesPerInstance(0),
    numIndicesPerInstance(0),
    numInstancesDrawn(0)
{
    // hint to the scene manager to avoid setting the model matrix
    // lighting is done in world coord, so render light for first instance only
    this->SetWorldCoord(true);
}

//------------------------------------------------------------------------------
/**
*/
nInstShapeNode::~nInstShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    load batch mesh from the primitive node, whatever it is:
    - single shape node
    - batch node
*/
bool
nInstShapeNode::LoadResources()
{
    // check that there is a primitive shape node and that it is valid
    kernelServer->PushCwd(this);
    bool success = this->refShapeNode.isvalid();
    kernelServer->PopCwd();

    if (!success)
    {
        return false;
    }

    // create instanced mesh using batch resource loader
    if (!this->refBatchMesh.isvalid())
    {
        nMesh2* mesh = nGfxServer2::Instance()->NewMesh(this->GetFullName().Get());
        n_assert(mesh);
        if (!mesh->IsLoaded())
        {
            mesh->SetFilename(this->GetFullName().Get());
            mesh->SetUsage(nMesh2::NeedsVertexShader|nMesh2::WriteOnce);
            mesh->SetResourceLoader(nBatchMeshLoader::Instance()->GetFullName().Get());

            if (!mesh->Load())
            {
                NLOG(resource, (0, "nInstShapeNode: Error loading instanced mesh: '%s'\n", this->GetFullName().Get()));
                mesh->Release();
                return false;
            }
        }

        this->refBatchMesh = mesh;
    }

    // trick to force shapes with same vertex declaration to be sorted together
    this->streamId |= (this->refBatchMesh->GetVertexComponents() & 0x0000ffff)<<16;

    // load instance stream
    /*if (!this->refInstanceStream.isvalid() && !this->instanceStreamName.IsEmpty())
    {
        this->refInstanceStream = nGfxServer2::Instance()->NewInstanceStream(this->instanceStreamName.Get());
        n_assert(this->refInstanceStream.isvalid());
        n_assert(this->refInstanceStream->IsValid());
    }*/

    // initialize palettes of instanced parameters
    this->positionPalette.SetFixedSize(this->numBatchedInstances);
    this->rotationPalette.SetFixedSize(this->numBatchedInstances);

    return nGeometryNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
nInstShapeNode::UnloadResources()
{
    if (this->refBatchMesh.isvalid())
    {
        this->refBatchMesh->Release();
        this->refBatchMesh.invalidate();
    }

    /*if (this->refInstanceStream.isvalid())
    {
        this->refInstanceStream->Release();
        this->refInstanceStream.invalidate();
    }*/
}

//------------------------------------------------------------------------------
/**
*/
bool
nInstShapeNode::Apply(nSceneGraph * sceneGraph)
{
    #ifndef NGAME
    if (sceneGraph->GetShaderIndex() == this->wireframeShaderIndex)
    {
        n_assert(this->refShapeNode.isvalid());
        return this->refShapeNode->Apply(sceneGraph);
    }
    #endif //NGAME

    if (nGeometryNode::Apply(sceneGraph))
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        n_assert(this->refBatchMesh.isvalid());

        gfxServer->SetMesh(this->refBatchMesh, this->refBatchMesh);

        // set the scene class instance stream
        //gfxServer->SetInstanceStream(this->refInstanceStream.get());
        //this->refInstanceStream->Lock(nInstanceStream::Write);

        this->numInstancesDrawn = 0;
    }

    //return false to prevent the shape node from applying itself
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nInstShapeNode::Render(nSceneGraph *sceneGraph, nEntityObject *entityObject)
{
    #ifndef NGAME
    if (sceneGraph->GetShaderIndex() == this->wireframeShaderIndex)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        gfxServer->SetTransform(nGfxServer2::Model, sceneGraph->GetModelTransform());
        n_assert(this->refShapeNode.isvalid());
        return this->refShapeNode->Render(sceneGraph, entityObject);
    }
    #endif //NGAME

    //don't call this, per-instance parameters are not rendered:
    //nGeometryNode::Render(sceneGraph, entityObject);

    ncTransform* transform = entityObject->GetComponent<ncTransform>();
    vector4 instPosScale(transform->GetPosition());
    instPosScale.w = transform->GetScale().x;
    this->positionPalette.Set(this->numInstancesDrawn, instPosScale);
    this->rotationPalette.Set(this->numInstancesDrawn, transform->GetEuler());

    ++this->numInstancesDrawn;
    if (this->numInstancesDrawn >= this->numBatchedInstances)
    {
        this->Flush(sceneGraph);
    }

    //to prevent the non-instanced shape node render itself
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nInstShapeNode::Flush(nSceneGraph * /*sceneGraph*/)
{
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    if (this->numInstancesDrawn > 0)
    {
        nShader2* curShader = gfxServer->GetShader();

        if (curShader->IsParameterUsed(nShaderState::InstPositionPalette))
        {
            curShader->SetVector4Array(nShaderState::InstPositionPalette, this->positionPalette.Begin(), this->numInstancesDrawn);
        }
        if (curShader->IsParameterUsed(nShaderState::InstRotationPalette))
        {
            curShader->SetVector4Array(nShaderState::InstRotationPalette, this->rotationPalette.Begin(), this->numInstancesDrawn);
        }

        gfxServer->PushTransform(nGfxServer2::Model, matrix44());//fixme

        // set vertex and index ranges for current set of instances
        gfxServer->SetVertexRange(0, this->numVerticesPerInstance * this->numInstancesDrawn);
        gfxServer->SetIndexRange(0, this->numIndicesPerInstance * this->numInstancesDrawn);
        gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);

        gfxServer->PopTransform(nGfxServer2::Model);//fixme

        this->numInstancesDrawn = 0;
    }

    return true;
}
