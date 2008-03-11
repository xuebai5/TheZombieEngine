/*-----------------------------------------------------------------------------
    @file ngmmgeometrystorage.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
*/
nGMMGeometryStorage::nGMMGeometryStorage() :
    vb(0),
    maxVertices(0),
    ib(0),
    maxIndices(0),
    firstVertex(0),
    firstIndex(0),
    numVertices(0),
    numIndices(0),
    offsetIndex(0),
    numVerticesLocked(0),
    numIndicesLocked(0),
    fillNormals(false),
    vertexSize(0),
    currentIndex(0),
    currentVertexMax(-1),
    ibLocked(0),
    vbLocked(0)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
nGMMGeometryStorage::~nGMMGeometryStorage()
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMGeometryStorage::SetupVertexBuffer(float * vb, int numMaxVertices)
{
    n_assert((vb && numMaxVertices > 0) || !numMaxVertices);
    this->vb = vb;
    this->maxVertices = numMaxVertices;
    this->numVertices = 0;
}

//------------------------------------------------------------------------------
/**
*/
float * 
nGMMGeometryStorage::GetVertexBuffer() const
{
    return this->vb;
}

//------------------------------------------------------------------------------
/**
*/
int 
nGMMGeometryStorage::GetMaxVertices() const
{
    return this->maxVertices;
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMGeometryStorage::SetupIndexBuffer(nuint16 * ib, int numMaxIndices)
{
    n_assert((ib && numMaxIndices > 0) || !numMaxIndices);
    this->ib = ib;
    this->maxIndices = numMaxIndices;
    this->numIndices = 0;
}

//------------------------------------------------------------------------------
/**
*/
nuint16 * 
nGMMGeometryStorage::GetIndexBuffer() const
{
    return this->ib;
}

//------------------------------------------------------------------------------
/**
*/
int 
nGMMGeometryStorage::GetMaxIndices() const
{
    return this->maxIndices;
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMGeometryStorage::BeginTerrainMesh()
{
    // reset the terrain mesh
    this->firstVertex = 0;
    this->firstIndex = 0;
    this->numVertices = 0;
    this->numIndices = 0;
    this->offsetIndex = 0;

    n_assert(this->vb || !this->maxVertices);
    n_assert(this->ib || !this->maxIndices);
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMGeometryStorage::EndTerrainMesh()
{
    // reset the terrain mesh
    this->firstVertex = 0;
    this->firstIndex = 0;
    this->numVertices = 0;
    this->numIndices = 0;
    this->offsetIndex = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMGeometryStorage::BeginTerrainGroup(
	int N_IFDEF_ASSERTS(numVertices), 
	int N_IFDEF_ASSERTS(numIndices) )
{
    n_assert((this->numVertices + numVertices) <= this->maxVertices);
    n_assert((this->numIndices + numIndices) <= this->maxIndices);
}

//------------------------------------------------------------------------------
/**
*/
int
nGMMGeometryStorage::EndTerrainGroup()
{
    this->SetVertexOffsetIndex( 0 );

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int 
nGMMGeometryStorage::GetFirstVertex() const
{
    return this->firstVertex;
}

//------------------------------------------------------------------------------
/**
*/
int 
nGMMGeometryStorage::GetNumVertices() const
{
    return this->numVertices;
}

//------------------------------------------------------------------------------
/**
*/
int 
nGMMGeometryStorage::GetFirstIndex() const
{
    return this->firstIndex;
}

//------------------------------------------------------------------------------
/**
*/
int 
nGMMGeometryStorage::GetNumIndices() const
{
    return this->numIndices;
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMGeometryStorage::SetFirstVertex(int firstVertex)
{
    this->firstVertex = firstVertex;
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMGeometryStorage::SetFirstIndex(int firstIndex)
{
    this->firstIndex = firstIndex;
}

//------------------------------------------------------------------------------
/**
    Returns true if normals are filled in the vertex buffer
*/
bool 
nGMMGeometryStorage::GetFillNormals() const
{
    return this->fillNormals;
}

//------------------------------------------------------------------------------
/**
    Set to true if you want normals to be filled in the vertex buffer,
    otherwise false.
*/
void 
nGMMGeometryStorage::SetFillNormals(bool fillnormals)
{
    this->fillNormals = fillnormals;
    if (this->fillNormals)
    {
        this->vertexSize = 6;
    }
    else
    {
        this->vertexSize = 3;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMGeometryStorage::SetVertexOffsetIndex(int offsetIndex)
{
    this->offsetIndex = offsetIndex;
}
