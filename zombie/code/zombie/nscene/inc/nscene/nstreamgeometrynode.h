#ifndef N_STREAMGEOMETRYNODE_H
#define N_STREAMGEOMETRYNODE_H
//------------------------------------------------------------------------------
/**
    @class nStreamGeometryNode
    @ingroup SceneNodes

    @brief A stream geometry node encapsulates the data and behavior
    for streamed geometry, collecting instance information indexed into an
    geometry stream optimized to be efficiently drawn.
    
    (C) 2006 Conjurer Services, S.A.
*/
#include "nscene/ngeometrynode.h"

//------------------------------------------------------------------------------
class nStreamGeometryNode : public nGeometryNode
{
public:
    /// enumerated value
    enum
    {
        MaxStreamSlots = 72,
    };

    /// constructor
    nStreamGeometryNode();
    /// destructor
    virtual ~nStreamGeometryNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// called by app when new render context has been created for this object
    virtual void EntityCreated(nEntityObject* entityObject);
    /// called by app when render context is going to be released
    virtual void EntityDestroyed(nEntityObject* entityObject);
    /// called by nSceneGraph when object is attached to scene
    virtual void Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// perform pre-instancing rendering of geometry
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// render geometry
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// perfor post-instancing rendering of geometry
    virtual bool Flush(nSceneGraph* sceneGraph);

    /// get shape node at index
    nGeometryNode* GetShapeNodeAt(int index);

    /// get if texture layer has coordinate transform
    bool HasUvTransformAt(int index, int layer);
    /// get coordinate transform for texture layer
    const matrix33& GetUvTransformAt(int index, int layer);

    /// begin meshes
    void BeginShapes(int);
    /// set shape by index
    void SetShapeAt(int, const char *);
    /// get shape by index
    const char * GetShapeAt(int);
    /// end shapes
    void EndShapes();
    /// get number of shapes
    int GetNumShapes();
    /// set uv position by index
    void SetUvPosAt(int, int, const vector2&);
    /// set uv scale by index
    void SetUvScaleAt(int, int, const vector2&);
    /// set frequency by index
    void SetFrequencyAt(int, int);
    /// get frequency by index
    int GetFrequencyAt(int);

private:
    /// render remaining slots as shader parameters
    bool RenderStreamSlots(nSceneGraph* sceneGraph, nShader2* curShader);

    struct StreamSlot
    {
        StreamSlot();
        StreamSlot(const char *);

        nDynAutoRef<nGeometryNode> refShapeNode;
        uint vertexComponents;
        transform33 textureTransform[nGfxServer2::MaxTextureStages];
        bool useTextureTransform[nGfxServer2::MaxTextureStages];
        /// number of instances of this shape in the stream
        int frequency;
        /// index of first slot in the map
        int firstSlot;        
    };

    /// the mesh resource
    nRef<nMesh2> refMesh;
    /// definition of stream components
    nArray<StreamSlot> streamSlots;
    /// runtime collection of stream indices (instances)
    nArray<int> instanceIndices;
    /// runtime map of slot indices to stream indices (instances)
    nArray<int> instanceSlots;
    nArray<vector4> positionPalette;
    nArray<vector4> rotationPalette;
};

//------------------------------------------------------------------------------
/**
*/
inline
nStreamGeometryNode::StreamSlot::StreamSlot() :
    vertexComponents(0),
    frequency(1),
    firstSlot(-1)
{
    for (int i = 0; i < nGfxServer2::MaxTextureStages; ++i)
    {
        this->useTextureTransform[i] = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nStreamGeometryNode::StreamSlot::StreamSlot(const char *filename) :
    refShapeNode(filename),
    vertexComponents(0),
    frequency(1),
    firstSlot(-1)
{
    for (int i = 0; i < nGfxServer2::MaxTextureStages; ++i)
    {
        this->useTextureTransform[i] = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nGeometryNode*
nStreamGeometryNode::GetShapeNodeAt(int index)
{
    return this->streamSlots[index].refShapeNode.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nStreamGeometryNode::HasUvTransformAt(int index, int layer)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    return this->streamSlots[index].useTextureTransform[layer];
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix33&
nStreamGeometryNode::GetUvTransformAt(int index, int layer)
{
    n_assert(layer < nGfxServer2::MaxTextureStages);
    return this->streamSlots[index].textureTransform[layer].getmatrix33();
}

//------------------------------------------------------------------------------
#endif
