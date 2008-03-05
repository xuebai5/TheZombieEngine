#ifndef N_LINESERVER_H
#define N_LINESERVER_H
//------------------------------------------------------------------------------
/**
    @class nLineServer
    @ingroup NebulaGraphicsSystem

    Unified interface for line drawing through the nGfxServer

    (C) 2005 Conjurer Services, S.A.
*/
#include "gfx2/ngfxserver2.h"
#include "gfx2/nmesh2.h"
#include "gfx2/ndynamicshadermesh.h"
#include "gfx2/nshaderparams.h"

//------------------------------------------------------------------------------
class nLineHandler
{
public:
    /// constructor
    nLineHandler();
    /// constructor 1
    nLineHandler(nGfxServer2::PrimitiveType type, int vertexComponents);
    /// destructor
    ~nLineHandler();
    /// set primitive type
    void SetPrimitiveType(nGfxServer2::PrimitiveType primitiveType);
    /// get primitive type
    nGfxServer2::PrimitiveType GetPrimitiveType();
    /// set vertex components
    void SetVertexComponents(int vertexComponents);
    /// get vertex components
    int GetVertexComponents();
    /// set shader file
    void SetShader(const char *name);
    /// get shader file
    const char *GetShader();
    /// access to shader parameters
    nShaderParams& GetShaderParams();
    /// return if the line handler has been initialized
    bool IsValid() const;

    /// begin rendering 3d lines
    void BeginLines(matrix44 &transform);
    /// add the vertices to the current buffer
    void DrawLines3d(const vector3 *coords, const vector2 *texCoords, const vector4 *color, int numVertices);
    /// add the vertices to the current buffer
    void DrawLines3d(const vector3 *coords, const vector2 *texCoords, int numVertices, const vector4& color);
    /// finish line rendering
    void EndLines();

private:
    nGfxServer2::PrimitiveType primitiveType;
    int componentFlags;
    nString shaderName;
    matrix44 transform;
    nRef<nShader2> refLineShader; ///< the shader used to render the lines
    nDynamicShaderMesh dynMesh;
    float *dstVertices;
    int maxVertices;
    int curVertex;
    int curIndex;
    bool inBeginDraw;
    nShaderParams shaderParams;
};
/*
class nLineServer
{
public:
    /// constructor
    nLineServer();
    /// destructor
    ~nLineServer();
    /// ... create or retrieve a line handler
    //nLineHandler *NewLineHandler();
};
*/
//------------------------------------------------------------------------------
/**
*/
inline
void
nLineHandler::SetVertexComponents(int vertexComponents)
{
    n_assert(!this->dynMesh.IsValid());
    this->componentFlags = vertexComponents;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nLineHandler::GetVertexComponents()
{
    return this->componentFlags;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLineHandler::SetShader(const char *filename)
{
    n_assert(!this->dynMesh.IsValid());
    this->shaderName = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nLineHandler::GetShader()
{
    return this->shaderName.Get();
}

//------------------------------------------------------------------------------
/**    
*/
inline
nShaderParams&
nLineHandler::GetShaderParams()
{
    return this->shaderParams;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLineHandler::SetPrimitiveType(nGfxServer2::PrimitiveType primType)
{
    n_assert(!this->dynMesh.IsValid());
    this->primitiveType = primType;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::PrimitiveType
nLineHandler::GetPrimitiveType()
{
    return this->primitiveType;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLineHandler::IsValid() const
{
    return this->dynMesh.IsValid();
}

//------------------------------------------------------------------------------
#endif
