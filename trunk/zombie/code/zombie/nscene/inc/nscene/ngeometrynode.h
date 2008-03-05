#ifndef N_GEOMETRYNODE_H
#define N_GEOMETRYNODE_H
//------------------------------------------------------------------------------
/**
    @class nGeometryNode
    @ingroup SceneNodes

    @brief A geometry node is an abstract node that encapsulates the
    functionality of all nodes containing renderable geometry.
    
    It is derived from nAbstractShaderNode, and thus inherits shader parameters.
    It must be assigned a surface, from which it inherits its render passes.

    See also @ref N2ScriptInterface_ngeometrynode
    
    (C) 2004 Conjurer Services, S.A.
*/
#include "nscene/nabstractshadernode.h"
#include "kernel/ndynautoref.h"
#include "util/nmaptabletypes.h"
#include "util/nsafekeyarray.h"

class nSurfaceNode;

//------------------------------------------------------------------------------
class nGeometryNode : public nAbstractShaderNode
{
public:
    /// constructor
    nGeometryNode();
    /// destructor
    virtual ~nGeometryNode();
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

    #ifndef NGAME
    /// resolve debug mesh for current instance
    virtual nMesh2 *GetDebugMesh(nSceneGraph* sceneGraph, nEntityObject* entityObject, const char *name);
    #endif

    /// resolve surface node
    nSurfaceNode *GetSurfaceNode();

    /// get node for stream rendering
    nGeometryNode* GetStreamGeometry();
    /// get geometry id
    uint GetStreamId() const;

    /// set the surface
    void SetSurface(const char *relPath);
    /// get the surface
    const char *GetSurface();
    /// set the stream geometry path
    void SetStream(const char *relPath);
    /// get the stream geometry path
    const char *GetStream();
    /// set index of the geometry in the stream
    void SetStreamIndex(int index);
    /// get index of the geometry in the stream
    int GetStreamIndex();
    /// set whether the geometry is in world coordinates
    void SetWorldCoord(bool worldCoord);
    /// get whether the geometry is in world coordinates
    bool GetWorldCoord();
    #ifndef NGAME
    /// Set If this is dirty then receiver is dirty ( Use NOH)
    virtual void BindDirtyDependence(nObject* receiver);
    /// Set dirty or clean ( Use NOH)
    virtual void RecruseSetObjectDirty(bool dirty);
    #endif

protected:
    /// cache render levels and passes for fast attach
    void CacheSurfacePassesByLevel(nSurfaceNode* surfaceNode);

    nDynAutoRef<nSurfaceNode> refSurfaceNode;
    nDynAutoRef<nGeometryNode> refStreamGeometry;

    bool worldCoord;
    nArray<nArray<int> > passesByLevel;

    uint geometryId;
    uint streamId;
    int streamIndex;
    static uint uniqueGeometryId;

#ifndef NGAME
    int wireframeShaderIndex;
#endif
};

//------------------------------------------------------------------------------
/**
    Specifies the surface node for this geometry.
*/
inline
void
nGeometryNode::SetSurface(const char *relPath)
{
    this->refSurfaceNode.set(relPath);
    this->resourcesValid = false;
}

//------------------------------------------------------------------------------
/**
    Get path to surface node.
*/
inline
const char*
nGeometryNode::GetSurface()
{
    return this->refSurfaceNode.getname();
}

//------------------------------------------------------------------------------
/**
    Set whether the geometry handles its own coordinates.
*/
inline
void
nGeometryNode::SetWorldCoord(bool worldCoord)
{
    this->worldCoord = worldCoord;
}

//------------------------------------------------------------------------------
/**
    Get whether the geometry handles its own coordinates.
*/
inline
bool
nGeometryNode::GetWorldCoord()
{
    if (this->refStreamGeometry.isvalid())
    {
        return this->refStreamGeometry->GetWorldCoord();
    }
    return this->worldCoord;
}

//------------------------------------------------------------------------------
/**
    Resolve current shader object through surface node.
*/
inline
nSurfaceNode *
nGeometryNode::GetSurfaceNode()
{
    if (this->refStreamGeometry.isvalid())
    {
        return this->refStreamGeometry->GetSurfaceNode();
    }

    return this->refSurfaceNode.isvalid() ? this->refSurfaceNode.get() : 0;
}

//------------------------------------------------------------------------------
/**
    Specifies the stream node for this geometry.
*/
inline
void
nGeometryNode::SetStream(const char *relPath)
{
    this->refStreamGeometry.set(relPath);
    this->resourcesValid = false;
}

//------------------------------------------------------------------------------
/**
    Get path to stream node for this geometry.
*/
inline
const char *
nGeometryNode::GetStream()
{
    return this->refStreamGeometry.getname();
}

//------------------------------------------------------------------------------
/**
    Resolve current shader object through surface node.
*/
inline
nGeometryNode*
nGeometryNode::GetStreamGeometry()
{
    return this->refStreamGeometry.get_unsafe();
}

//------------------------------------------------------------------------------
/**
    get id for geometry, be it local of parent stream
*/
inline
uint
nGeometryNode::GetStreamId() const
{
    return this->streamId;
}

//------------------------------------------------------------------------------
/**
    set index in the geometry stream.
*/
inline
void
nGeometryNode::SetStreamIndex(int streamIndex)
{
    this->streamIndex = streamIndex;
}

//------------------------------------------------------------------------------
/**
    get index in the geometry stream.
*/
inline
int
nGeometryNode::GetStreamIndex()
{
    return this->streamIndex;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Resolve current shader object through surface node.
*/
inline
nMesh2*
nGeometryNode::GetDebugMesh(nSceneGraph* /*sceneGraph*/, nEntityObject* /*entityObject*/, const char * /*name*/)
{
    return 0;
}
#endif //NGAME

//------------------------------------------------------------------------------
#endif
