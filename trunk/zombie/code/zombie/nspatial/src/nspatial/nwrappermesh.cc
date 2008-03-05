#include "precompiled/pchnspatial.h"
//------------------------------------------------------------------------------
//  nwrappermesh.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspatial/nwrappermesh.h"

//------------------------------------------------------------------------------
/**
*/
nWrapperMesh::nWrapperMesh() :
    meshBuilder( 32 , 32 , 32)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nWrapperMesh::~nWrapperMesh()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nWrapperMesh::LoadResources()
{
    n_assert(this->GetMeshFile());

    // load mesh builder for editing
    this->meshBuilder.Load(nFileServer2::Instance(), this->GetMeshFile());

    // load mesh resource for debugging
    if (!this->refMesh.isvalid())
    {
        nMesh2* mesh = nGfxServer2::Instance()->NewMesh(this->GetMeshFile());
        n_assert(mesh);
        if (!mesh->IsValid())
        {
            mesh->SetFilename(this->GetMeshFile());
            if (!mesh->Load())
            {
                return false;
            }
        }
        this->refMesh = mesh;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nWrapperMesh::UnloadResources()
{
    // release mesh builder
    this->meshBuilder.Clear();

    // release mesh resource
    if (this->refMesh.isvalid())
    {
        this->refMesh->Release();
        this->refMesh.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Set mesh data from mesh builder.

    This is intended for use from indoor exporter only.
    The internal mesh structure is loaded from the builder,
    but not the mesh resource, that can be loaded from file
    and is only required for debug visualization.
*/
void
nWrapperMesh::SetMeshBuilder(nMeshBuilder& meshBuilder)
{
    this->meshBuilder = meshBuilder;
}

//------------------------------------------------------------------------------
/**
*/
nMeshBuilder&
nWrapperMesh::GetMeshBuilder()
{
    return this->meshBuilder;
}

//------------------------------------------------------------------------------
/**
    Check whether the mesh contains a position.
    Point is transformed to local space before testing.
*/
bool
nWrapperMesh::Contains(vector3 const& pos) const
{
    vector3 transformedPos = this->invTransform * pos;
    return this->meshBuilder.Contains(transformedPos);
}

//------------------------------------------------------------------------------
/**
*/
float
nWrapperMesh::GetIntersectionDist(const vector3 &point, const vector3 &direction) const
{
    vector3 transformedPoint = this->invTransform * point;
    vector3 transformedDir = this->invTransform * direction;
    return this->meshBuilder.GetIntersectionDist(point, direction);
}

//------------------------------------------------------------------------------
/**
*/
void
nWrapperMesh::RenderDebug()
{
    if (this->refMesh.isvalid())
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();

        gfxServer->SetTransform(nGfxServer2::Model, this->transform);
        gfxServer->SetMesh(this->refMesh, this->refMesh);
        gfxServer->SetVertexRange(0, this->refMesh->GetNumVertices());
        gfxServer->SetIndexRange(0, this->refMesh->GetNumIndices());
        gfxServer->DrawIndexedNS(nGfxServer2::TriangleList);
    }
}
