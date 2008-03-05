//------------------------------------------------------------------------------
//  nlineserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "gfx2/nlineserver.h"

//------------------------------------------------------------------------------
/**
*/
nLineHandler::nLineHandler() :
    primitiveType(nGfxServer2::LineStrip),
    componentFlags(nMesh2::Coord),
    inBeginDraw(false),
    shaderName("shaders:line.fx")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLineHandler::nLineHandler(nGfxServer2::PrimitiveType type, int comp) :
    primitiveType(type),
    componentFlags(comp),
    inBeginDraw(false),
    shaderName("shaders:line.fx")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLineHandler::~nLineHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLineHandler::BeginLines(matrix44 &transform)
{
    n_assert(!this->inBeginDraw);
    this->inBeginDraw = true;
    this->transform = transform;

    if (!this->dynMesh.IsValid())
    {
        if (!this->refLineShader.isvalid())
        {
            this->refLineShader = (nShader2 *) nGfxServer2::Instance()->NewShader(this->shaderName.Get());
            this->refLineShader->SetFilename(this->shaderName.Get());
            this->refLineShader->Load();
            n_assert(this->refLineShader->IsValid());
        }
        
        this->dynMesh.SetShader(this->refLineShader.get());
        this->dynMesh.Initialize(this->primitiveType, this->componentFlags, nMesh2::WriteOnly, false);
        n_assert(this->dynMesh.IsValid());
    }

    this->dynMesh.Begin(this->dstVertices, this->maxVertices);
    this->curVertex = 0;
    this->curIndex = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nLineHandler::DrawLines3d(const vector3 *coords, const vector2 *texCoords, const vector4 *color, int numVertices)
{
    n_assert(coords);
    n_assert(numVertices > 0);
    n_assert(this->inBeginDraw);
    n_assert(this->dstVertices);

    //if (this->curVertex + numVertices > this->maxVertices)
    //this->dynMesh.Swap(this->curVertex, this->dstVertices);

    int i;
    for (i = 0; i < numVertices; i++)
    {
        this->dstVertices[this->curIndex++] = coords[i].x;
        this->dstVertices[this->curIndex++] = coords[i].y;
        this->dstVertices[this->curIndex++] = coords[i].z;

        if (this->componentFlags & nMesh2::Color)
        {
            this->dstVertices[this->curIndex++] = color[i].x;
            this->dstVertices[this->curIndex++] = color[i].y;
            this->dstVertices[this->curIndex++] = color[i].z;
            this->dstVertices[this->curIndex++] = color[i].w;
        }

        if (this->componentFlags & nMesh2::Uv0)
        {
            n_assert(texCoords);
            this->dstVertices[this->curIndex++] = texCoords[i].x;
            this->dstVertices[this->curIndex++] = texCoords[i].y;
        }
    }

    this->curVertex += numVertices;
}

//------------------------------------------------------------------------------
/**
*/
void
nLineHandler::DrawLines3d(const vector3 *coords, const vector2 *texCoords, int numVertices, const vector4& color)
{
    n_assert(coords);
    n_assert(numVertices > 0);
    n_assert(this->inBeginDraw);
    n_assert(this->dstVertices);

    //if (this->curVertex + numVertices > this->maxVertices)
    //this->dynMesh.Swap(this->curVertex, this->dstVertices);

    int i;
    for (i = 0; i < numVertices; i++)
    {
        this->dstVertices[this->curIndex++] = coords[i].x;
        this->dstVertices[this->curIndex++] = coords[i].y;
        this->dstVertices[this->curIndex++] = coords[i].z;

        if (this->componentFlags & nMesh2::Color)
        {
            this->dstVertices[this->curIndex++] = color.x;
            this->dstVertices[this->curIndex++] = color.y;
            this->dstVertices[this->curIndex++] = color.z;
            this->dstVertices[this->curIndex++] = color.w;
        }

        if (this->componentFlags & nMesh2::Uv0)
        {
            n_assert(texCoords);
            this->dstVertices[this->curIndex++] = texCoords[i].x;
            this->dstVertices[this->curIndex++] = texCoords[i].y;
        }
    }

    this->shaderParams.SetArg(nShaderState::MatDiffuse, nShaderArg(color));
    this->curVertex += numVertices;
}

//------------------------------------------------------------------------------
/**
*/
void
nLineHandler::EndLines()
{
    n_assert(this->inBeginDraw);
    this->inBeginDraw = false;

    nGfxServer2::Instance()->PushTransform(nGfxServer2::Model, this->transform);
    this->refLineShader->SetParams(this->shaderParams);
    this->dynMesh.End(this->curVertex);
    nGfxServer2::Instance()->PopTransform(nGfxServer2::Model);

    this->dstVertices = 0;
    this->maxVertices = 0;
}
