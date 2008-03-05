#ifndef N_GMMGEOMETRYSTORAGE_H
#define N_GMMGEOMETRYSTORAGE_H
//------------------------------------------------------------------------------
/**
    @class nGMMGeometryStorage
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief nGMMGeometryStorage and derivates provide an abstract interface to 
    store geometry (vertices and indices) by the heightmap mesh builder.
    nGMMGeometryStorage does the storage in user provided buffers, but others
    subclasses exist which hide the storage details in meshes, dynamic meshes, ...

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/ntypes.h"
#include "kernel/nlogclass.h"
#include "ngeomipmap/ncterraingmmclass.h"

//------------------------------------------------------------------------------
/**
*/
class nGMMGeometryStorage : public nReferenced
{
public:
    /// constructor
    nGMMGeometryStorage();
    /// destructor
    virtual ~nGMMGeometryStorage();

    /// setup vertex buffers
    void SetupVertexBuffer(float * vb, int maxNumVertices);
    /// get vertex buffer
    float * GetVertexBuffer() const;
    /// get max number of vertices
    int GetMaxVertices() const;
    /// setup index buffer
    void SetupIndexBuffer(nuint16 * ib, int maxNumIndices);
    /// get index buffer
    nuint16 * GetIndexBuffer() const;
    /// get max number of indices
    int GetMaxIndices() const;

    /// Begin terrain mesh usage (before starting terrain mesh groups)
    virtual void BeginTerrainMesh();
    /// End terrain mesh usage (after finishing all groups)
    virtual void EndTerrainMesh();

    /// Begin terrain mesh building process (begins new group)
    virtual void BeginTerrainGroup(int numVertices, int numIndices);
    /// End terrain mesh building process, (ends new group)
    virtual int EndTerrainGroup();

    /// lock vertex buffer
    float * LockVerticesGroup(int numVertices);
    /// unlock vertex buffer
    void UnlockVerticesGroup();

    /// lock index buffer
    ushort * LockIndicesGroup(int numIndices);
    /// unlock index buffer
    void UnlockIndicesGroup();

    /// append indices by copying from user provided buffer
    void AppendIndices(ushort * ibsrc, int numIndices);
    /// append vertices by copying from user provided buffer
    void AppendVertices(float * vbsrc, int numVertices);

    /// get first vertex index
    int GetFirstVertex() const;
    /// get number of vertices
    int GetNumVertices() const;
    /// get first index index
    int GetFirstIndex() const;
    /// get number of indices
    int GetNumIndices() const;

    /// Select if normals must be filled or not in the vertex buffer
    void SetFillNormals(bool fillnormals);
    /// get fill normals
    bool GetFillNormals() const;
    /// get the size of a vertex with all components in number of floats
    int GetVertexSizeInFloats() const;

    /// used to read / write vertices of the geometry storage in current group
    vector3 * GetVertex(int index);
    /// add primitives given by 3 indices
    void AddTriangle(int i1, int i2, int i3);

protected:

    friend class nGMMVertexBuilder;

    /// set first vertex
    void SetFirstVertex(int firstVertex);
    /// set first index
    void SetFirstIndex(int firstIndex);

    /// set vertex offset index
    void SetVertexOffsetIndex(int offsetIndex);
    /// get vertex offset index
    int GetVertexOffsetIndex() const;

private:

    /// pointer to user provided vertex buffer / locked vertex buffer
    float * vb;
    /// maximum number of vertices available in user vertex buffer
    int maxVertices;
    /// pointer to user provided index buffer / locked index buffer
    nuint16 * ib;
    /// maximum number of indices available in user index buffer
    int maxIndices;

    /// first vertex index of the group
    int firstVertex;
    /// first index of the group
    int firstIndex;
    /// number of vertices rendered in current group
    int numVertices;
    /// number of indices rendered in current group
    int numIndices;
    /// offset index is the value to be added to vertex indices for current group
    int offsetIndex;
    /// check number of vertices passed to Lock in Unlock
    int numVerticesLocked;
    /// check number of indices passed to Lock in Unlock
    int numIndicesLocked;

    /// true if normals must be filled in the vertex stream
    bool fillNormals;
    /// size of a vertex in floats
    int vertexSize;

    /// current index
    int currentIndex;
    /// locked index buffer start
    nuint16 * ibLocked;
    /// maximum current vertex used
    int currentVertexMax;
    /// locked vertex buffer start
    float * vbLocked;

};

//------------------------------------------------------------------------------
/**
*/
inline
int 
nGMMGeometryStorage::GetVertexOffsetIndex() const
{
    return this->offsetIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGMMGeometryStorage::GetVertexSizeInFloats() const
{
    return this->vertexSize;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector3 *
nGMMGeometryStorage::GetVertex(int index)
{
    n_assert(index >= 0 && (index - this->GetVertexOffsetIndex()) < this->numVerticesLocked);
    this->currentVertexMax = n_max(this->currentVertexMax, index);
    return reinterpret_cast<vector3 *> (this->vbLocked + (index * this->GetVertexSizeInFloats()));
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nGMMGeometryStorage::AddTriangle(int i1, int i2, int i3)
{
    NLOG(terrain, (nTerrainLog::NLOG_BUILDINDEX | 1, "AddTriangle(%d,%d,%d) currentIndex=%d offsetIndex=%d", i1, i2, i3, this->currentIndex, this->offsetIndex));

    n_assert((this->currentIndex + 3) <= this->numIndicesLocked);
    this->ibLocked[this->currentIndex++] = static_cast<ushort> ( this->offsetIndex + i1 );
    this->ibLocked[this->currentIndex++] = static_cast<ushort> ( this->offsetIndex + i2 );
    this->ibLocked[this->currentIndex++] = static_cast<ushort> ( this->offsetIndex + i3 );
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nGMMGeometryStorage::AppendIndices(ushort * ibsrc, int numIndices)
{
    n_assert(ibsrc);
 
    ushort * ibdst = this->LockIndicesGroup(numIndices);
    for(int index = 0;index < numIndices;index++)
    {
        ibdst[index] = ushort(ibsrc[index] + this->offsetIndex);
    }
    // update number of indices
    this->numIndices += numIndices;
    this->UnlockIndicesGroup();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nGMMGeometryStorage::AppendVertices(float * vbsrc, int numVertices)
{
    n_assert(vbsrc);

    float * vbdst = this->LockVerticesGroup(numVertices);
    memcpy(vbdst, vbsrc, numVertices * this->GetVertexSizeInFloats() * sizeof(float));
    // update number of indices
    this->numVertices += numVertices;
    this->UnlockVerticesGroup();
}

//------------------------------------------------------------------------------
/**
*/
inline
float * 
nGMMGeometryStorage::LockVerticesGroup(int numVertices)
{
    n_assert(this->vb);
    n_assert((this->firstVertex + this->numVertices + numVertices) <= this->maxVertices);
    n_assert(!this->numVerticesLocked);
    this->numVerticesLocked = numVertices;
    this->currentVertexMax = -1;
    this->vbLocked = this->vb + (this->firstVertex + this->numVertices) * this->GetVertexSizeInFloats();
    return this->vbLocked;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nGMMGeometryStorage::UnlockVerticesGroup()
{
    n_assert(this->numVerticesLocked >= this->currentVertexMax);
    this->numVerticesLocked = 0;
    this->numVertices += this->currentVertexMax + 1;
    this->currentVertexMax = -1;
    this->vbLocked = 0;
    n_assert((this->firstVertex + this->numVertices) <= this->maxVertices);
}

//------------------------------------------------------------------------------
/**
*/
inline
ushort * 
nGMMGeometryStorage::LockIndicesGroup(int numIndices)
{
    n_assert(this->ib);
    n_assert((this->firstIndex + this->numIndices + numIndices) <= this->maxIndices);
    n_assert(!this->numIndicesLocked);
    n_assert(numIndices >= 0);
    this->numIndicesLocked = numIndices;
    this->currentIndex = 0;
    this->ibLocked = this->ib + this->firstIndex + this->numIndices;
    return this->ibLocked;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nGMMGeometryStorage::UnlockIndicesGroup()
{
    n_assert(this->numIndicesLocked >= this->currentIndex);
    this->numIndicesLocked = 0;
    this->numIndices += this->currentIndex;
    this->currentIndex = 0;
    this->ibLocked = 0;
    n_assert((this->firstIndex + this->numIndices) <= this->maxIndices);
}

//------------------------------------------------------------------------------
#endif // N_GMMGEOMETRYSTORAGE_H
