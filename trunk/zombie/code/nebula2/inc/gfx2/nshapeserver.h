#ifndef N_SHAPESERVER_H
#define N_SHAPESERVER_H
//------------------------------------------------------------------------------
/**
    @class nShapeServer
    @ingroup NebulaGraphicsSystem
    @author MA Garcias <ma.garcias@yahoo.es>
    @brief Unified interface for shape drawing through the nGfxServer

    This class implements an interface for drawing primitive shapes.

    (C) 2005 Conjurer Services, S.A.
*/

#include "gfx2/ngfxserver2.h"

//------------------------------------------------------------------------------
class nShapeServer
{
public:
    /// constructor
    nShapeServer();
    /// destructor
    ~nShapeServer();
    /// get instance pointer
    static nShapeServer* Instance();

    /// open and initialize shape primitives
    void Open();
    /// close and release shape primitives
    void Close();
    /// set mesh for type
    void SetShapeMesh(nGfxServer2::ShapeType, nMesh2* mesh);
    /// get mesh for type
    nMesh2* GetShapeMesh(nGfxServer2::ShapeType);
    /// set shader
    void SetShader(nShader2* shader);
    /// get shader
    nShader2* GetShader();
    /// set primitive type
    void SetPrimitiveType(nGfxServer2::PrimitiveType primtype);
    /// get primitive type
    nGfxServer2::PrimitiveType GetPrimitiveType();
    /// set indexed
    void SetIndexed(bool value);
    /// get indexed
    bool GetIndexed();

    /// begin shape rendering
    void BeginShapes();
    /// draw a shape with the given model matrix
    void DrawShape(nGfxServer2::ShapeType type, const matrix44& model, const vector4& color);
    /// draw a shape with the given model matrix
    void DrawShape(nMesh2* shape, const matrix44& model, const vector4& color);
    /// end shape rendering
    void EndShapes();

private:
    static nShapeServer* Singleton;  

    /// load mesh from file with name
    nMesh2* LoadMesh(const char *name, const char *filename);

    bool isOpen;
    bool inBeginDraw;
    bool indexed;
    nRef<nShader2> refShader;
    nGfxServer2::PrimitiveType primitiveType;
    /// shapes for standard types, to use with nGfxServer2::DrawShape
    nRef<nMesh2> refShapeMeshes[nGfxServer2::NumShapeTypes];
};

//------------------------------------------------------------------------------
/**
*/
inline
nShapeServer*
nShapeServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShapeServer::SetShader(nShader2* shader)
{
    n_assert(!this->inBeginDraw);
    this->refShader = shader;
}

//------------------------------------------------------------------------------
/**
*/
inline
nShader2*
nShapeServer::GetShader()
{
    return this->refShader.isvalid() ? this->refShader.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShapeServer::SetPrimitiveType(nGfxServer2::PrimitiveType primType)
{
    this->primitiveType = primType;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::PrimitiveType
nShapeServer::GetPrimitiveType()
{
    return this->primitiveType;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nShapeServer::SetIndexed(bool value)
{
    this->indexed = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nShapeServer::GetIndexed()
{
    return this->indexed;
}

//------------------------------------------------------------------------------
#endif
