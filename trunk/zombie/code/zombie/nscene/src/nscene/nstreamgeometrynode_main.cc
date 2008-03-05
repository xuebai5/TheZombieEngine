#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nstreamgeometrynode_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nstreamgeometrynode.h"
#include "nscene/nbatchmeshloader.h"

#include "kernel/nlogclass.h"

nNebulaScriptClass(nStreamGeometryNode, "ngeometrynode");

//------------------------------------------------------------------------------
/**
*/
nStreamGeometryNode::nStreamGeometryNode()
{
    this->SetWorldCoord(true);
}

//------------------------------------------------------------------------------
/**
*/
nStreamGeometryNode::~nStreamGeometryNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nStreamGeometryNode::BeginShapes(int num)
{
    this->streamSlots.SetFixedSize(num);
}

//------------------------------------------------------------------------------
/**
*/
void
nStreamGeometryNode::SetShapeAt(int index, const char *relPath)
{
    this->streamSlots[index].refShapeNode = relPath;
}

//------------------------------------------------------------------------------
/**
*/
const char *
nStreamGeometryNode::GetShapeAt(int index)
{
    return this->streamSlots[index].refShapeNode.getname();
}

//------------------------------------------------------------------------------
/**
*/
void
nStreamGeometryNode::EndShapes()
{
    // TODO- sum the number of slots resulting from all shapes * frequency
}

//------------------------------------------------------------------------------
/**
*/
int
nStreamGeometryNode::GetNumShapes()
{
    return this->streamSlots.Size();
}

//------------------------------------------------------------------------------
/**
*/
void
nStreamGeometryNode::SetUvPosAt(int index, int layer, const vector2& pos)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    this->streamSlots[index].textureTransform[layer].settranslation(pos);
    this->streamSlots[index].useTextureTransform[layer] = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nStreamGeometryNode::SetUvScaleAt(int index, int layer, const vector2& scale)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    this->streamSlots[index].textureTransform[layer].setscale(scale);
    this->streamSlots[index].useTextureTransform[layer] = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nStreamGeometryNode::SetFrequencyAt(int index, int frequency)
{
    this->streamSlots[index].frequency = frequency;
}

//------------------------------------------------------------------------------
/**
*/
int
nStreamGeometryNode::GetFrequencyAt(int index)
{
    return this->streamSlots[index].frequency;
}

//------------------------------------------------------------------------------
/**
*/
bool
nStreamGeometryNode::LoadResources()
{
    //try to reuse the nStaticBatchNode resource loading implementation:
    //input: the meshes to batch, vertex components, number of copies of each
    //texture transform(s) for each slot, insert instance index as Indices[0]
    if (!this->refMesh.isvalid() && this->streamSlots.Size() > 0)
    {
        // get a new or shared mesh
        nMesh2* mesh = nGfxServer2::Instance()->NewMesh(this->GetFullName().Get());
        n_assert(mesh);
        if (!mesh->IsLoaded())
        {
            mesh->SetFilename(this->GetFullName().Get());
            mesh->SetUsage(nMesh2::NeedsVertexShader|nMesh2::WriteOnce);
            mesh->SetResourceLoader(nBatchMeshLoader::Instance()->GetFullName().Get());

            if (!mesh->Load())
            {
                NLOG(resource, (0, "nStreamGeometryNode: Error loading batch: '%s'\n", this->GetFullName().Get()));
                mesh->Release();
                return false;
            }

        }
        this->refMesh = mesh;
    }

    // initialize the slot map
    int numSlots = 0;
    for (int index = 0; index < this->streamSlots.Size(); ++index)
    {
        this->streamSlots[index].firstSlot = numSlots;
        numSlots += this->streamSlots[index].frequency;
    }

    this->instanceSlots.SetFixedSize(numSlots);
    this->positionPalette.SetFixedSize(numSlots);
    this->rotationPalette.SetFixedSize(numSlots);

    return nGeometryNode::LoadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
nStreamGeometryNode::UnloadResources()
{
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
        this->refMesh.invalidate();
    }

    return nGeometryNode::UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
nStreamGeometryNode::EntityCreated(nEntityObject* entityObject)
{
    return nGeometryNode::EntityCreated(entityObject);
}

//------------------------------------------------------------------------------
/**
*/
void
nStreamGeometryNode::EntityDestroyed(nEntityObject* entityObject)
{
    return nGeometryNode::EntityDestroyed(entityObject);
}

//------------------------------------------------------------------------------
/**
    this should never be called- streamed geometry is always rendered through
    it primitive geometry node.
*/
void
nStreamGeometryNode::Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    //n_assert_always();
    nGeometryNode::Attach(sceneGraph, entityObject);
}

//------------------------------------------------------------------------------
/**
*/
bool
nStreamGeometryNode::Apply(nSceneGraph* /*sceneGraph*/)
{
    //apply per-stream surface parameters
    //if nGeometryNode::Apply(sceneGraph);
    //TODO- set the geometry stream or whatever
    if (!this->refMesh.isvalid())
    {
        return false;
    }

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    n_assert(this->refMesh->IsValid());

    // set mesh, vertex and index range
    gfxServer->SetMesh(this->refMesh, this->refMesh);
    gfxServer->SetVertexRange(0, this->refMesh->GetNumVertices());
    gfxServer->SetIndexRange(0, this->refMesh->GetNumIndices());
    
    return false;//HACK to prevent subclass doing anything else
}

//------------------------------------------------------------------------------
/**
*/
bool
nStreamGeometryNode::Render(nSceneGraph* sceneGraph, nEntityObject* /*entityObject*/)
{
    if (!this->refMesh.isvalid())
    {
        return false;
    }

    //knowing the node in behalf of which I'm being called now is easy, because
    //it is the one in the scene graph
    nGeometryNode* curGeometry = (nGeometryNode*) sceneGraph->GetCurrentNode();
    n_assert(curGeometry->GetStreamId() == this->geometryId);
    if (curGeometry->GetStreamIndex() != -1)
    {
        //TODO- check that there are that many slots
        n_assert(curGeometry->GetStreamIndex() < this->streamSlots.Size());
        this->instanceIndices.Append(sceneGraph->GetCurrentIndex());
        //actual rendering is deferred until ::Flush()
        //where all instance information will have been collected

        //don't call this, per-instance parameters are not rendered:
        //nGeometryNode::Render(sceneGraph, entityObject);

        return false;//HACK to prevent subclass doing anything else
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nStreamGeometryNode::Flush(nSceneGraph* sceneGraph)
{
    //return nGeometryNode::Flush(sceneGraph);
    if (this->instanceIndices.Size() > 0)
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();
        nShader2* curShader = gfxServer->GetShader();

        //TEMP- ensure that the shader is an instanced shader indeed
        if (!curShader->IsParameterUsed(nShaderState::InstPositionPalette) ||
            !curShader->IsParameterUsed(nShaderState::InstRotationPalette))
        {
            this->instanceIndices.Reset();
            return false;
        }

        int curIndex = sceneGraph->GetCurrentIndex();
        gfxServer->SetTransform(nGfxServer2::Model, matrix44());

        // render as many times as needed while there are slots left to draw
        while (this->instanceIndices.Size() > 0)
        {
            if (this->RenderStreamSlots(sceneGraph, curShader))
            {
                gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);//this->GetPrimitiveType()
            }
        }

        sceneGraph->SetCurrentIndex(curIndex);
        return false;//HACK! to prevent subclass doing anything else
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    This method fills the instance stream with shader parameters
    and removes instances from the array, until all possible slots are used.
    It is called until there are no instances left to draw.
*/
bool
nStreamGeometryNode::RenderStreamSlots(nSceneGraph* sceneGraph, nShader2* curShader)
{
    vector4 nullVec;
    this->instanceSlots.Fill(0, this->instanceSlots.Size(), -1);
    this->positionPalette.Fill(0, this->positionPalette.Size(), nullVec);
    this->rotationPalette.Fill(0, this->rotationPalette.Size(), nullVec);

    //1- fill the map of slots, -1 if slot not used
    for (int index = 0; index < this->instanceIndices.Size(); /*empty*/)
    {
        sceneGraph->SetCurrentIndex(this->instanceIndices[index]);
        nGeometryNode* curGeometry = (nGeometryNode*) sceneGraph->GetCurrentNode();
        int streamIndex = curGeometry->GetStreamIndex();
        n_assert(streamIndex != -1);

        //check if there are empty slots for this stream index
        const StreamSlot& streamSlot = this->streamSlots[streamIndex];
        int slotIndex = streamSlot.firstSlot;
        int maxSlotIndex = streamSlot.firstSlot + streamSlot.frequency;
        while (this->instanceSlots.At(slotIndex) != -1 && slotIndex < maxSlotIndex)
        {
            ++slotIndex;
        }

        if (slotIndex < maxSlotIndex)
        {
            ncTransform* transform = (ncTransform*) sceneGraph->GetCurrentEntity()->GetComponent<ncTransform>();
            
            // instance position + scale
            vector4 instPosScale(transform->GetPosition());
            instPosScale.w = transform->GetScale().x;
            this->positionPalette.Set(slotIndex, instPosScale);

            // instance rotation
            vector4 instEuler(transform->GetEuler());
            this->rotationPalette.Set(slotIndex, instEuler);

            this->instanceSlots.Set(slotIndex, this->instanceIndices[index]);
            this->instanceIndices.Erase(index);
        }
        else
        {
            ++index;
        }
    }

    //2- flush collected instance parameters into shader slots
    curShader->SetVector4Array(nShaderState::InstPositionPalette, this->positionPalette.Begin(), this->positionPalette.Size());
    curShader->SetVector4Array(nShaderState::InstRotationPalette, this->rotationPalette.Begin(), this->rotationPalette.Size());

    //TODO- return false to prevent drawing if no slot was filled
    return true;
}
