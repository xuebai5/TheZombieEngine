#ifndef N_INSTSHAPENODE_H
#define N_INSTSHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nShapeNode
    @ingroup SceneNodes
    @author MA Garcias <ma.garcias@yahoo.es>
    
    @brief An instanced version of a regular shape node, performs
    shader instancing using a static batch mesh with instance indices.
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ngeometrynode.h"

//------------------------------------------------------------------------------
class nInstShapeNode : public nGeometryNode
{
public:
    /// constructor
    nInstShapeNode();
    /// destructor
    virtual ~nInstShapeNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// perform pre-instancing rendering of geometry
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// perform per-instance-rendering of geometry
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// perform post-instancing rendering of geometry
    virtual bool Flush(nSceneGraph* sceneGraph);

    /// get original shape node
    nGeometryNode* GetShapeNode();

    /// set max number of batched instances
    void SetNumInstances(const int num);
    /// get max number of batched instances
    int GetNumInstances() const;
    /// set number of vertices in every batched instance
    void SetNumVerticesPerInstance(const int numVertices);
    /// get number of vertices in every batched instance
    int GetNumVerticesPerInstance() const;
    /// set number of indices in every batched instance
    void SetNumIndicesPerInstance(const int numIndices);
    /// get number of indices in every batched instance
    int GetNumIndicesPerInstance() const;

    /// set instance stream
    void SetInstanceStream(const char *);
    /// get instance stream
    const char * GetInstanceStream();
    /// set primitive node
    void SetShape(const char *);
    /// get primitive node
    const char * GetShape();
    /// set num instances
    void SetFrequency(int);
    /// get num instances
    int GetFrequency() const;

    #ifndef NGAME
    /// get the mesh object for stats purposes only
    nMesh2* GetMeshResource();
    #endif//NGAME

protected:
    enum
    {
        /// magic number, same as in instanced shader
        /// @todo ma.garcias - this should be customized for every class
        MaxBatchedInstances = 16,//72,
    };

    nDynAutoRef<nGeometryNode> refShapeNode;
    nRef<nMesh2> refBatchMesh;

    nString instanceStreamName;
    nRef<nInstanceStream> refInstanceStream;

    int numBatchedInstances;    ///< number of batched instances in batched mesh
    int numVerticesPerInstance; ///< number of vertices of every mesh instance
    int numIndicesPerInstance;  ///< number of indices of every mesh instance
    int numInstancesDrawn;      ///< number of already drawn instances in current render

    nArray<vector4> positionPalette;
    nArray<vector4> rotationPalette;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nInstShapeNode::SetShape(const char *path)
{
    this->refShapeNode.set(path);
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nInstShapeNode::GetShape()
{
    return this->refShapeNode.getname();
}

//------------------------------------------------------------------------------
/**
*/
inline
nGeometryNode *
nInstShapeNode::GetShapeNode()
{
    return this->refShapeNode.isvalid() ? this->refShapeNode.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInstShapeNode::SetInstanceStream(const char *name)
{
    this->instanceStreamName = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nInstShapeNode::GetInstanceStream()
{
    return this->instanceStreamName.IsEmpty() ? 0 : this->instanceStreamName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInstShapeNode::SetFrequency(int frequency)
{
    this->numBatchedInstances = frequency;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInstShapeNode::GetFrequency() const
{
    return this->numBatchedInstances;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInstShapeNode::SetNumInstances(const int num)
{
    this->numBatchedInstances = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInstShapeNode::GetNumInstances() const
{
    return this->numBatchedInstances;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInstShapeNode::SetNumVerticesPerInstance(const int num)
{
    this->numVerticesPerInstance = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInstShapeNode::GetNumVerticesPerInstance() const
{
    return this->numVerticesPerInstance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nInstShapeNode::SetNumIndicesPerInstance(const int num)
{
    this->numIndicesPerInstance = num;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nInstShapeNode::GetNumIndicesPerInstance() const
{
    return this->numIndicesPerInstance;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
inline
nMesh2*
nInstShapeNode::GetMeshResource()
{
    return this->refBatchMesh.get_unsafe();
}

#endif

//------------------------------------------------------------------------------
#endif
