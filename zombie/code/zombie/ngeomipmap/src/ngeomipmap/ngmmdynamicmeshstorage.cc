/*-----------------------------------------------------------------------------
    @file ngmmdynamicmeshstorage.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmdynamicmeshstorage.h"

//------------------------------------------------------------------------------
/**
*/
nGMMDynamicMeshStorage::nGMMDynamicMeshStorage() :
    dynMesh(0)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
nGMMDynamicMeshStorage::~nGMMDynamicMeshStorage()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMDynamicMeshStorage::SetDynamicMesh(nDynamicMesh * dynMesh)
{
    n_assert(dynMesh);
    n_assert(dynMesh->IsValid());
    this->dynMesh = dynMesh;
    this->SetupVertexBuffer(0, 0);
    this->SetupIndexBuffer(0, 0);

    this->SetFillNormals( dynMesh->HasAllVertexComponents(nMesh2::Normal) );
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMDynamicMeshStorage::BeginTerrainMesh()
{
    n_assert(this->dynMesh->IsValid());
    n_assert(!this->GetVertexBuffer());
    n_assert(!this->GetIndexBuffer());

    float * vb;
    int maxVertices;
    nuint16 * ib;
    int maxIndices;
    this->dynMesh->BeginIndexed( vb, ib, maxVertices, maxIndices );

    this->SetupVertexBuffer(vb, maxVertices);
    this->SetupIndexBuffer(ib, maxIndices);

    nGMMGeometryStorage::BeginTerrainMesh();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMDynamicMeshStorage::EndTerrainMesh()
{
    n_assert(this->dynMesh->IsValid());
    this->dynMesh->EndIndexed(this->GetNumVertices(), this->GetNumIndices());

    this->SetupVertexBuffer(0, 0);
    this->SetupIndexBuffer(0, 0);

    nGMMGeometryStorage::EndTerrainMesh();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMDynamicMeshStorage::BeginTerrainGroup(int numVertices, int numIndices)
{
    n_assert(numVertices < this->GetMaxVertices());
    n_assert(numIndices < this->GetMaxIndices());
    if (    ((this->GetNumVertices() + numVertices) > this->GetMaxVertices()) 
        ||  ((this->GetNumIndices() + numIndices) > this->GetMaxIndices()) )
    {
        float * vb;
        nuint16 * ib;
        this->dynMesh->SwapIndexed(this->GetNumVertices(), this->GetNumIndices(), vb, ib);
        this->SetupVertexBuffer(vb, this->GetMaxVertices());
        this->SetupIndexBuffer(ib, this->GetMaxIndices());
    }
    this->SetVertexOffsetIndex( this->GetNumVertices() );

    nGMMGeometryStorage::BeginTerrainGroup(numVertices, numIndices);
}

//------------------------------------------------------------------------------
/**
*/
int
nGMMDynamicMeshStorage::EndTerrainGroup()
{
    return nGMMGeometryStorage::EndTerrainGroup();
}
