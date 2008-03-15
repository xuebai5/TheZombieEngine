//------------------------------------------------------------------------------
//  ngmmmeshstorage.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmmeshstorage.h"

//------------------------------------------------------------------------------
/**
*/
nGMMMeshStorage::nGMMMeshStorage() :
    group(0)
{
}

//------------------------------------------------------------------------------
/**
*/
nGMMMeshStorage::~nGMMMeshStorage()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMMeshStorage::SetupMesh(nMesh2 * mesh)
{
    n_assert(mesh);
    this->refMesh = mesh;

    this->SetupVertexBuffer(0, 0);
    this->SetupIndexBuffer(0, 0);

    this->SetFillNormals( mesh->HasAllVertexComponents(nMesh2::Normal) );
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMMeshStorage::BeginTerrainMesh()
{
    n_assert(this->refMesh.isvalid());

    n_assert(!this->GetVertexBuffer());
    int maxVertices = this->refMesh->GetNumVertices();
    if (maxVertices > 0)
    {
        float * vb = this->refMesh->LockVertices();
        n_assert(vb);

        this->SetupVertexBuffer(vb, maxVertices);
    }

    n_assert(!this->GetIndexBuffer());
    int maxIndices = this->refMesh->GetNumIndices();
    if (maxIndices > 0)
    {
        nuint16 * ib = this->refMesh->LockIndices();
        n_assert(ib);

        this->SetupIndexBuffer(ib, maxIndices);
    }

    this->group = 0;
    nGMMGeometryStorage::BeginTerrainMesh();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMMeshStorage::EndTerrainMesh()
{
    n_assert(this->refMesh.isvalid());
    if (this->GetVertexBuffer())
    {
        this->refMesh->UnlockVertices();
        this->SetupVertexBuffer(0, 0);
    }

    if (this->GetIndexBuffer())
    {
        this->refMesh->UnlockIndices();
        this->SetupIndexBuffer(0, 0);
    }

    this->group = 0;
    nGMMGeometryStorage::EndTerrainMesh();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMMeshStorage::BeginTerrainGroup(int numVertices, int numIndices)
{
    n_assert(this->refMesh.isvalid());
    n_assert(group >= 0 && (this->group + 1) <= this->refMesh->GetNumGroups());

    // @todo check there is space for the total number of vertices and indices
    //n_assert((this->numVertices + numVertices) <= this->maxVertices);
    //n_assert((this->numIndices + numIndices) <= this->maxIndices);

    this->SetupVertexBuffer(this->GetVertexBuffer(), this->GetMaxVertices());
    this->SetupIndexBuffer(this->GetIndexBuffer(), this->GetMaxIndices());

    nGMMGeometryStorage::BeginTerrainGroup(numVertices, numIndices);
}

//------------------------------------------------------------------------------
/**
*/
int
nGMMMeshStorage::EndTerrainGroup()
{
    n_assert(this->refMesh.isvalid());
    n_assert(this->group >= 0 && (this->group + 1) <= this->refMesh->GetNumGroups());

    // set the vertices and indices written in the current mesh group
    nMeshGroup & meshGroup = this->refMesh->Group( this->group );
    meshGroup.SetFirstVertex( this->GetFirstVertex() );
    meshGroup.SetNumVertices( this->GetNumVertices() );
    meshGroup.SetFirstIndex( this->GetFirstIndex() );
    meshGroup.SetNumIndices( this->GetNumIndices() );

    // keep the group to return
    int retgroup = this->group;

    // advance to following group
    this->group++;
    if (this->group >= this->refMesh->GetNumGroups())
    {
        this->group = 0;
    }

    this->SetFirstVertex( this->GetFirstVertex() + this->GetNumVertices() );
    this->SetFirstIndex( this->GetFirstIndex() + this->GetNumIndices() );

    return retgroup;
}

