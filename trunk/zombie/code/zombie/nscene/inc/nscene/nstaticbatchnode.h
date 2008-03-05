#ifndef N_STATICBATCHNODE_H
#define N_STATICBATCHNODE_H
//------------------------------------------------------------------------------
/**
    @class nStaticBatchNode
    @ingroup SceneNodes
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief A batch node is a geometry node that loads itself from a bunch
    of static shapes, thus not having to replicate on disk information on
    individual meshes. It is a plain copy of the original geometry 
    conveniently batched for performance purposes.
       
    (C) 2005 Conjurer Services, S.A.
*/

#include "nscene/nshapenode.h"

//------------------------------------------------------------------------------
class nStaticBatchNode : public nShapeNode
{
public:
    /// constructor
    nStaticBatchNode();
    /// destructor
    virtual ~nStaticBatchNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();

    /// set array of source shapes
    void SetMeshArray(nArray<nString>& meshArray);
    /// get if selected mesh has a transform
    bool HasTransformAt(int index);
    /// get transform at index
    const matrix44& GetTransformAt(int index);

    /// begin meshes
    void BeginMeshes(int);
    /// set mesh resource by index
    void SetMeshAt(int, const char *);
    /// get mesh resource by index
    const char * GetMeshAt(int);
    /// end meshes
    void EndMeshes();
    /// get number of meshes
    int GetNumMeshes();
    /// set position by index
    void SetPositionAt(int, const vector3&);
    /// set euler rotation by index
    void SetEulerAt(int, const vector3&);
    /// set quaternion by index
    void SetQuatAt(int, const quaternion&);
    /// set scale by index
    void SetScaleAt(int, float);

protected:
    //TODO- use a structure to load meshes from, create a proper resource loader
    //that can automatically reload the mesh if it crashes.
    struct BatchedMesh
    {
        BatchedMesh();
        BatchedMesh(nString&);
        nString meshFile;
        bool hasTransform;
        transform44 modelTransform;
    };
    nArray<BatchedMesh> meshArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
nStaticBatchNode::BatchedMesh::BatchedMesh() :
    hasTransform(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nStaticBatchNode::BatchedMesh::BatchedMesh(nString& mesh) :
    meshFile(mesh),
    hasTransform(false)
{
    //empty
}

//------------------------------------------------------------------------------
#endif /*N_STATICBATCHNODE_H*/
