#ifndef N_BLENDSHAPENODE_H
#define N_BLENDSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nBlendShapeNode
    @ingroup Scene

    See also @ref N2ScriptInterface_nblendshapenode
    
    (C) 2004 RadonLabs GmbH
*/
//------------------------------------------------------------------------------
#include "nscene/ngeometrynode.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesharray.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
/// maximum number of targets
const int MaxTargets = 32;

//------------------------------------------------------------------------------
class nBlendShapeNode : public nGeometryNode
{
public:
    /// constructor
    nBlendShapeNode();
    /// destructor
    virtual ~nBlendShapeNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// entity created
    virtual void EntityCreated(nEntityObject *entityObject);
    /// entity destroyed
    virtual void EntityDestroyed(nEntityObject *entityObject);

    /// override shader parameter rendering
    virtual bool ApplyShader(nSceneGraph* sceneGraph);
    /// get the mesh usage flags required by this shape node
    virtual int GetMeshUsage() const;
    /// perform pre-instancing rending of geometry
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// perform per-instance-rendering of geometry
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);

    /// set the mesh resource name for the specified index
    void SetMeshAt(int index, const char* name);
    /// get the mesh resource name for the specified index
    const char* GetMeshAt(int index) const;

    /// fill instant mesh array
    void FillInstantMeshArray(nEntityObject* entityObject, nArray<int>& curveIndices, nArray<int>& targetIndices);

    //<OBSOLETE>
    /// set the mesh group index
    void SetGroupIndex(int i);
    /// get the mesh group index
    int GetGroupIndex() const;
    //</OBSOLETE>

    /// set the local bounding box for the specified shape
    void SetLocalBoxAt(int index, const bbox3& localBox);
    /// get the local bounding box for the specified shape
    const bbox3& GetLocalBoxAt(int index) const;
    /// set the weight of the specified shape
    void SetWeightAt(int index, float weight);
    /// get the weight of the specified shape
    float GetWeightAt(int index) const;
    /// get number of valid shapes
    int GetNumShapes() const;
    
    /// get number of active targets
    int GetNumActiveTargets() const;
    /// set number of active targets
    void SetNumActiveTargets(int activeTargets);

    /// max number of shapes
    enum 
    {
        MaxShapes = 7,
    };

protected:
    int activeTargets;
    int totalNumTargets;
    
    /// all targets
    nRef<nMeshArray> refFullMeshArray;

    /// used at the moment of render
    int meshArrayIndex;

    /// update shader state with new weights
    void UpdateShaderState();

    class Shape 
    {
    public:
        /// constructor
        Shape();
        /// clear object
        void Clear();

        nString meshName;
        bbox3 localBox;
        float weight;
    };
    
    //<OBSOLETE> keep for backwards compatibility
    int numShapes;
    int groupIndex;
    //</OBSOLETE>
    nFixedArray<Shape> shapeArray;
    nRef<nMeshArray> refMeshArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nBlendShapeNode::Shape::Shape() :
    weight(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendShapeNode::Shape::Clear()
{
    this->localBox = bbox3();
    this->meshName = 0;
    this->weight = 0.0f;
}

//------------------------------------------------------------------------------
/**
    return the number of valid shapes
*/
inline
int
nBlendShapeNode::GetNumShapes() const
{
    return MaxShapes;
    //return this->numShapes;
}

//------------------------------------------------------------------------------
/**
    return the number of active target shapes
*/
inline
int
nBlendShapeNode::GetNumActiveTargets() const
{
    n_assert(this->activeTargets < MaxShapes );
    return this->activeTargets;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendShapeNode::SetNumActiveTargets(int numTargets)
{
    this->activeTargets = numTargets;
}
//------------------------------------------------------------------------------
/**
    <OBSOLETE>
*/
inline
int
nBlendShapeNode::GetGroupIndex() const
{
    return this->groupIndex;
}

//------------------------------------------------------------------------------
/**
    <OBSOLETE>
*/
inline
void
nBlendShapeNode::SetGroupIndex(int i)
{
    if (i != this->groupIndex)
    {
        this->groupIndex = i;
        this->resourcesValid = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nBlendShapeNode::GetMeshAt(int index) const
{
    return this->shapeArray[index].meshName.Get();
}

//------------------------------------------------------------------------------
/**
    Set the local box for the specified shape.
*/
inline
void
nBlendShapeNode::SetLocalBoxAt(int index, const bbox3& localBox)
{
    this->shapeArray[index].localBox = localBox;
}

//------------------------------------------------------------------------------
/**
*/
inline
const bbox3&
nBlendShapeNode::GetLocalBoxAt(int index) const
{
    return this->shapeArray[index].localBox;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendShapeNode::SetWeightAt(int index, float weight)
{
    this->shapeArray[index].weight = weight;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nBlendShapeNode::GetWeightAt(int index) const
{
    return this->shapeArray[index].weight;
}

//------------------------------------------------------------------------------
#endif
