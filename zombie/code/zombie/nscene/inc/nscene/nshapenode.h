#ifndef N_SHAPENODE_H
#define N_SHAPENODE_H
//------------------------------------------------------------------------------
/**
    @class nShapeNode
    @ingroup SceneNodes

    @brief A shape node is the simplest actually visible object in the
    scene node class hierarchy.
    
    It is derived from nMaterialNode, and thus inherits transform and
    shader information. It adds a simple mesh which it can render.

    See also @ref N2ScriptInterface_nshapenode
    
    (C) 2002 RadonLabs GmbH
*/
#include "nscene/ngeometrynode.h"

class nMesh2;
//------------------------------------------------------------------------------
class nShapeNode : public nGeometryNode
{
public:
    /// constructor
    nShapeNode();
    /// destructor
    virtual ~nShapeNode();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// perform pre-instancing rending of geometry
    virtual bool Apply(nSceneGraph* sceneGraph);
    /// perform per-instance-rendering of geometry
    virtual bool Render(nSceneGraph* sceneGraph, nEntityObject* entityObject);
    
    /// get the mesh usage flags required by this shape node
    virtual int GetMeshUsage() const;
    /// set the mesh usage flags required by this shape node
    void SetMeshUsage(int usage);

    /// set the mesh resource name
    void SetMesh(const char* name);
    /// get the mesh resource name
    const char* GetMesh() const;
    /// set the mesh group index
    void SetGroupIndex(int i);
    /// get the mesh group index
    int GetGroupIndex() const;
    /// set the NOH path to the mesh's resource loader
    void SetMeshResourceLoader(const char* resourceLoaderPath);
    /// get the mesh's resource loader
    const char* GetMeshResourceLoader();

    /// get primitive type
    nGfxServer2::PrimitiveType GetPrimitiveType() const;
    /// set primitive type
    void SetPrimitiveType(nGfxServer2::PrimitiveType);
    /// convert primitive type to string
    static const char *PrimitiveTypeToString(nGfxServer2::PrimitiveType type);
    /// convert string to primitive type
    static nGfxServer2::PrimitiveType StringToPrimitiveType(const char *string);

    #ifndef NGAME
    /// get the mesh object for stats purposes only
    nMesh2* GetMeshResource();
    /// resolve debug mesh for current instance
    virtual nMesh2* GetDebugMesh(nSceneGraph* sceneGraph, nEntityObject* entityObject, const char *name);
    #endif

protected:
    /// load mesh resource
    bool LoadMesh();
    /// unload mesh resource
    void UnloadMesh();

    int meshUsage;
    nRef<nMesh2> refMesh;
    nString meshName;
    int groupIndex;
    nDynAutoRef<nResourceLoader> refMeshResourceLoader;
    nGfxServer2::PrimitiveType primitiveType;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nShapeNode::SetGroupIndex(int i)
{
    this->groupIndex = i;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nShapeNode::GetGroupIndex() const
{
    return this->groupIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShapeNode::SetPrimitiveType(nGfxServer2::PrimitiveType type)
{
    this->primitiveType = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::PrimitiveType
nShapeNode::GetPrimitiveType() const
{
    return this->primitiveType;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nShapeNode::PrimitiveTypeToString(nGfxServer2::PrimitiveType type)
{
    nString usageString;
    switch (type)
    {
    case nGfxServer2::TriangleList:
        return "TriangleList";
    case nGfxServer2::TriangleFan:
        return "TriangleFan";
    case nGfxServer2::TriangleStrip:
        return "TriangleStrip";
    case nGfxServer2::LineList:
        return "LineList";
    case nGfxServer2::LineStrip:
        return "LineStrip";
    case nGfxServer2::PointList:
        return "PointList";

    default:
        return "Invalid";
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::PrimitiveType
nShapeNode::StringToPrimitiveType(const char *typeString)
{
    if (!strcmp(typeString, "TriangleList"))
    {
        return nGfxServer2::TriangleList;
    }
    else if (!strcmp(typeString, "TriangleFan"))
    {
        return nGfxServer2::TriangleFan;
    }
    else if (!strcmp(typeString, "TriangleStrip"))
    {
        return nGfxServer2::TriangleStrip;
    }
    else if (!strcmp(typeString, "LineList"))
    {
        return nGfxServer2::LineList;
    }
    else if (!strcmp(typeString, "LineStrip"))
    {
        return nGfxServer2::LineStrip;
    }
    else if (!strcmp(typeString, "PointList"))
    {
        return nGfxServer2::PointList;
    }

    return nGfxServer2::TriangleList;
}

//------------------------------------------------------------------------------
/**
    This method must return the mesh usage flag combination required by
    this shape node class. Subclasses should override this method
    based on their requirements.

    @return     a combination on nMesh2::Usage flags
*/
inline
int
nShapeNode::GetMeshUsage() const
{
    return this->meshUsage;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShapeNode::SetMeshUsage(int usage)
{
    this->meshUsage = usage;
}

//------------------------------------------------------------------------------
#endif
