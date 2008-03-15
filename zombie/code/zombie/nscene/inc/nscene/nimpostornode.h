#ifndef N_IMPOSTORNODE_H
#define N_IMPOSTORNODE_H
//------------------------------------------------------------------------------
/**
    @class nImpostorNode
    @author MA Garcias <ma.garcias@yahoo.es>
    @ingroup SceneNodes
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ngeometrynode.h"

//------------------------------------------------------------------------------
class nImpostorNode : public nGeometryNode
{
public:
    /// constructor
    nImpostorNode();
    /// destructor
    virtual ~nImpostorNode();
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// called by nSceneGraph when object is attached to scene
    virtual void Attach(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// perform pre-instancing rendering of geometry
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// perform per-instance-rendering of geometry
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    /// perform post-instancing rendering of geometry
    virtual bool Flush(nSceneGraph* sceneGraph);

protected:
    nDynamicMesh dynMesh;       ///< dynamic mesh used to render all instances
    float *curVertexPtr;        ///< current pointer of vertices
    int maxVertices;            ///< max num of vertices admitted in the stream
    int curVertex;              ///< current number of vertices written to the stream
    int curIndex;               ///< current number of floats written to the stream

    #ifndef __NEBULA_NO_LOG__
    int numInstancesDrawn;      ///< number of already drawn instances in current render
    #endif
};

//------------------------------------------------------------------------------
#endif /*N_IMPOSTORNODE_H*/
