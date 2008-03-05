#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nblendshapenode_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "nscene/nblendshapenode.h"
#include "gfx2/nmesh2.h"

nNebulaScriptClass(nBlendShapeNode, "ngeometrynode");

//------------------------------------------------------------------------------
/**
*/
nBlendShapeNode::nBlendShapeNode() :
    totalNumTargets(0),
    activeTargets(0),
    numShapes(0),
    groupIndex(0),
    shapeArray(MaxShapes)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nBlendShapeNode::~nBlendShapeNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method must return the mesh usage flag combination required by
    this shape node class. Subclasses should override this method
    based on their requirements.

    @return     a combination on nMesh2::Usage flags
*/
int
nBlendShapeNode::GetMeshUsage() const
{
    return nMesh2::WriteOnce | nMesh2::NeedsVertexShader;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
nBlendShapeNode::LoadResources()
{
    nGeometryNode::LoadResources();
    if(!this->refFullMeshArray.isvalid())
    {
        this->refFullMeshArray = nGfxServer2::Instance()->NewMeshArray(0);
    }

    // update resouce filenames in mesharray
    int i;
    for (i = 0; i < this->totalNumTargets; i++)
    {
        this->refFullMeshArray->SetFilenameAt(i, this->shapeArray[i].meshName);
        this->refFullMeshArray->SetUsageAt(i, this->GetMeshUsage());
    }
    this->resourcesValid &= this->refFullMeshArray->Load();

    // update shape bounding boxes
    if (true == this->resourcesValid)
    {
        for(i = 0; i < this->totalNumTargets; i++)
        {
            nMesh2* mesh = this->refFullMeshArray->GetMeshAt(i);
            if (0 != mesh)
            {
                this->shapeArray[i].localBox = mesh->Group(this->groupIndex).GetBoundingBox();
            }
        }
    }
    return this->resourcesValid;
}

//------------------------------------------------------------------------------
/**
    Unload the resources.
*/
void
nBlendShapeNode::UnloadResources()
{
    nGeometryNode::UnloadResources();
    if (this->refFullMeshArray.isvalid())
    {
        this->refFullMeshArray->Unload();
        this->refFullMeshArray->Release();
        this->refFullMeshArray.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Set the mesh resource name at index.
    Updates the number of current valid shapes.
    
    @param  index   
    @param  name    name of the resource to set, 0 to unset a resource
*/
void
nBlendShapeNode::SetMeshAt(int index, const char* name)
{
    n_assert((index >= 0) && (index < MaxShapes));
    if (this->shapeArray[index].meshName != name)
    {
        this->resourcesValid = false;
        this->shapeArray[index].meshName = name;
        //<OBSOLETE>
        if (0 != name)
        {
            // increase shapes count
            this->numShapes = n_max(index+1, this->numShapes);
        }
        else
        {
            // decrease shapes count if this was the last element
            if (index + 1 == this->numShapes)
            {
                this->numShapes--;
            }
        }
        //</OBSOLETE>
        this->totalNumTargets++;
        
        if( this->totalNumTargets < MaxShapes )
        {
            this->activeTargets = this->totalNumTargets;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Gives the weights to the shader
*/
bool
nBlendShapeNode::ApplyShader(nSceneGraph* /*sceneGraph*/)
{
    return true;
}

//------------------------------------------------------------------------------
/**
    Perform pre-instancing actions needed for rendering geometry. This
    is called once before multiple instances of this shape node are
    actually rendered.
*/
bool
nBlendShapeNode::Apply(nSceneGraph* sceneGraph)
{
    if (nGeometryNode::Apply(sceneGraph))
    {
        return true;
    }
    return false;
}


//------------------------------------------------------------------------------
/**
    Update geometry, set as current mesh in the gfx server and
    call nGfxServer2::DrawIndexed().

    - 15-Jan-04     floh    AreResourcesValid()/LoadResource() moved to scene server
*/
bool
nBlendShapeNode::Render(nSceneGraph* sceneGraph, nEntityObject* entityObject)
{
    nGeometryNode::Render(sceneGraph, entityObject);
 
    /// get mesharray data
    ncScene *renderContext = entityObject->GetComponent<ncScene>();

    nVariable var;
    var = renderContext->GetLocalVar(this->meshArrayIndex);

    // set mesh, vertex and index range
    nMeshArray* meshArray = static_cast<nMeshArray*>(var.GetObj());

    nGfxServer2::Instance()->SetMeshArray(meshArray);
    const nMeshGroup& curGroup = meshArray->GetMeshAt(0)->Group(this->groupIndex);
    nGfxServer2::Instance()->SetVertexRange(curGroup.GetFirstVertex(), curGroup.GetNumVertices());
    nGfxServer2::Instance()->SetIndexRange(curGroup.GetFirstIndex(), curGroup.GetNumIndices());

    // Weights must be rendered here to allow overriding from animators
    // FIXME move to nSurfaceNode::Apply for blending surfaces, like swing surfaces.
    nShader2* shader = nGfxServer2::Instance()->GetShader();
    n_assert(shader);
    
    //int numShapes = this->GetNumShapes();
    int numShapes = this->GetNumActiveTargets();

    if (shader->IsParameterUsed(nShaderState::VertexStreams))
    {
        shader->SetInt(nShaderState::VertexStreams, numShapes);
    }
    
    if (numShapes > 0)
    {
        if (shader->IsParameterUsed(nShaderState::VertexWeights1))
        {
            nFloat4 weights = {0.0f, 0.0f, 0.0f, 0.0f};
            if (numShapes > 0) weights.x = this->GetWeightAt(0);
            if (numShapes > 1) weights.y = this->GetWeightAt(1);
            if (numShapes > 2) weights.z = this->GetWeightAt(2);
            if (numShapes > 3) weights.w = this->GetWeightAt(3);
            shader->SetFloat4(nShaderState::VertexWeights1, weights);
        }
    }
    if (numShapes > 4)
    {
        if (shader->IsParameterUsed(nShaderState::VertexWeights2))
        {
            nFloat4 weights = {0.0f, 0.0f, 0.0f, 0.0f};
            if (numShapes > 4) weights.x = this->GetWeightAt(4);
            if (numShapes > 5) weights.y = this->GetWeightAt(5);
            if (numShapes > 6) weights.z = this->GetWeightAt(6);
            if (numShapes > 7) weights.w = this->GetWeightAt(7);
            
            shader->SetFloat4(nShaderState::VertexWeights2, weights);
        }
    }

    nGfxServer2::Instance()->DrawIndexedNS(nGfxServer2::TriangleList);
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nBlendShapeNode::FillInstantMeshArray( nEntityObject* entityObject, nArray<int>& curveIndices, nArray<int>& targetIndices)
{
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    nVariable var;
    var = renderContext->GetLocalVar(this->meshArrayIndex);

    nMeshArray* meshArray = static_cast<nMeshArray*>(var.GetObj());

    meshArray->Unload();

    // change targets
    for (int i = 0; i < targetIndices.Size() ; i++)
    {
        meshArray->SetFilenameAt(curveIndices[i], this->shapeArray[ targetIndices[i] ].meshName);
        meshArray->SetUsageAt(curveIndices[i], this->GetMeshUsage());
    }

    meshArray->Load();
}

//------------------------------------------------------------------------------
/**
*/
void
nBlendShapeNode::EntityCreated(nEntityObject* entityObject)
{
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    
    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    nMeshArray* refMeshArray;
    refMeshArray = nGfxServer2::Instance()->NewMeshArray(0);

    // fill for first time with all targets REMOVE IT??? DO NOT REMOVE, useful for deformeranimator
    int i;
    for (i = 0; i < this->totalNumTargets; i++)
    {
        refMeshArray->SetFilenameAt(i, this->shapeArray[i].meshName);
        refMeshArray->SetUsageAt(i, this->GetMeshUsage());
    }

    refMeshArray->Load();

    // put mesharray in render context
    this->meshArrayIndex = renderContext->AddLocalVar(nVariable(0, refMeshArray));
}

//------------------------------------------------------------------------------
/**
*/
void
nBlendShapeNode::EntityDestroyed(nEntityObject* entityObject)
{
    ncScene *renderContext = entityObject->GetComponent<ncScene>();

    nVariable var;
    var = renderContext->GetLocalVar(this->meshArrayIndex);

    nMeshArray* meshArray = static_cast<nMeshArray*>(var.GetObj());
    n_assert(meshArray);

    n_delete(meshArray);
}
