//------------------------------------------------------------------------------
//  nshapeserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "gfx2/nshapeserver.h"

nShapeServer* nShapeServer::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nShapeServer::nShapeServer() :
    isOpen(false),
    inBeginDraw(false),
    indexed(true),
    primitiveType(nGfxServer2::TriangleList)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nShapeServer::~nShapeServer()
{
    if (this->isOpen)
    {
        this->Close();
    }

    n_assert(0 != Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nShapeServer::SetShapeMesh(nGfxServer2::ShapeType type, nMesh2* mesh)
{
    if (this->refShapeMeshes[type].isvalid())
    {
        this->refShapeMeshes[type]->Release();
    }
    this->refShapeMeshes[type] = mesh;
    this->refShapeMeshes[type]->AddRef();
}

//------------------------------------------------------------------------------
/**
*/
nMesh2*
nShapeServer::GetShapeMesh(nGfxServer2::ShapeType type)
{
    return this->refShapeMeshes[type].isvalid() ? this->refShapeMeshes[type].get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
void
nShapeServer::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;

    nMesh2* shape;
    shape = LoadMesh("shape_box", "wc:libs/system/meshes/shape_box.n3d2");
    this->refShapeMeshes[nGfxServer2::Box] = shape;
    shape = LoadMesh("shape_cylinder", "wc:libs/system/meshes/shape_cylinder.n3d2");
    this->refShapeMeshes[nGfxServer2::Cylinder] = shape;
//  shape = LoadMesh("shape_plane", "wc:libs/system/meshes/shape_plane.n3d2");
//  this->refShapeMeshes[nGfxServer2::Plane] = shape;
    shape = LoadMesh("shape_cone", "wc:libs/system/meshes/cone.nvx2");
    this->refShapeMeshes[nGfxServer2::Cone] = shape;
    shape = LoadMesh("shape_sphere", "wc:libs/system/meshes/shape_sphere.n3d2");
    this->refShapeMeshes[nGfxServer2::Sphere] = shape;
    shape = LoadMesh("shape_torus", "wc:libs/system/meshes/shape_torus.n3d2");
    this->refShapeMeshes[nGfxServer2::Torus] = shape;
}

//------------------------------------------------------------------------------
/**
*/
void
nShapeServer::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;

    // release mesh resources
    int i;
    for (i = 0; i < nGfxServer2::NumShapeTypes; i++)
    {
        if (this->refShapeMeshes[i].isvalid())
        {
            this->refShapeMeshes[i]->Release();
            this->refShapeMeshes[i].invalidate();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
nMesh2*
nShapeServer::LoadMesh(const char *name, const char *filename)
{
    nMesh2* mesh = nGfxServer2::Instance()->NewMesh(name);
    n_assert(mesh);
    if (!mesh->IsValid())
    {
        mesh->SetFilename(filename);
        mesh->Load();
        n_assert(mesh->IsValid());
    }
    return mesh;
}

//------------------------------------------------------------------------------
/**
*/
void
nShapeServer::BeginShapes()
{
    n_assert(this->isOpen);
    n_assert(!this->inBeginDraw);
    this->inBeginDraw = true;

    // save model transform
    nGfxServer2::Instance()->PushTransform(nGfxServer2::Model, matrix44());

    // render shader with state backup
    nShader2* shd = this->refShader.get();
    n_verify( shd->Begin(true) == 1);
}

//------------------------------------------------------------------------------
/**
*/
void
nShapeServer::DrawShape(nGfxServer2::ShapeType type, const matrix44& model, const vector4& color)
{
    n_assert(this->isOpen);
    n_assert(this->refShapeMeshes[type].isvalid());
    this->DrawShape(this->refShapeMeshes[type].get(), model, color);
}

//------------------------------------------------------------------------------
/**
*/
void
nShapeServer::DrawShape(nMesh2* shape, const matrix44& model, const vector4& color)
{
    n_assert(this->isOpen);
    n_assert(shape);

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    gfxServer->SetMesh(shape, shape);
    gfxServer->SetVertexRange(0, shape->GetNumVertices());
    gfxServer->SetIndexRange(0, shape->GetNumIndices());

    // update color in shader
    nShader2* shd = this->refShader.get();
    if (shd->IsParameterUsed(nShaderState::MatDiffuse))
    {
        shd->SetVector4(nShaderState::MatDiffuse, color);
    }

    // update model matrix
    gfxServer->SetTransform(nGfxServer2::Model, model);
    gfxServer->SetShader(shd);
    shd->BeginPass(0);
    if (this->indexed)
    {
        gfxServer->DrawIndexedNS(this->primitiveType);
    }
    else
    {
        gfxServer->DrawNS(this->primitiveType);
    }

    shd->EndPass();
}

//------------------------------------------------------------------------------
/**
*/
void
nShapeServer::EndShapes()
{
    n_assert(this->inBeginDraw);
    this->inBeginDraw = false;

    this->refShader->End();

    // restore model transform
    nGfxServer2::Instance()->PopTransform(nGfxServer2::Model);

    // restore default primitive type
    this->primitiveType = nGfxServer2::TriangleList;
    this->indexed = true;
}
