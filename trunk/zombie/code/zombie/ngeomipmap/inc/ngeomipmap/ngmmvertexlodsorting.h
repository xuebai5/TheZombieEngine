#ifndef N_GMMVERTEXLODSORTING_H
#define N_GMMVERTEXLODSORTING_H
//------------------------------------------------------------------------------
/**
    @class nGMMVertexLODSorting
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Used by gmm vertex builder to get the proper sorting of the 
    vertices. This basic version does no sorting indeed, it sorts the vertices 
    in the same order as the vertex key. The sorting it is interesting when
    LOD is applied, so vertices with low LOD are located in the first positions.
    In this way we don't have to copy all the vertices.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nreferenced.h"

//------------------------------------------------------------------------------
class ncTerrainGMMClass;

//------------------------------------------------------------------------------
/**
*/
class nGMMVertexLODSorting : public nReferenced
{
public:

    // LOD sorting information about a vertex 
    typedef struct 
    {
        // block relative position (x,z)
        int x, z;
        // lod where is used that vertex
        int lod;
    } nGMMVertexLOD;

    /// constructor
    nGMMVertexLODSorting();
    /// destructor
    virtual ~nGMMVertexLODSorting();

    /// Setup from the terrain geomipmap class
    virtual void Setup(ncTerrainGMMClass *);

    /// get vertex key
    int GetVertexKey(int x, int z) const;
    /// get vertex index
    int GetVertexIndex(int x, int z) const;
    /// get vertex lod info for vertex index N
    const nGMMVertexLOD & GetVertex(int index) const;
    /// get number of vertices in the lod specified
    int GetVertexCount(int lod) const;
    /// get number of total vertices for lod and lower lods
    int CountVertices(int lod) const;

#ifndef NGAME
    /// check all vertex indices used are below or greater than lod provided
    void CheckLOD(int lod);
#endif

protected:

    /// allocate internal structures
    void AllocateLODSortTable();
    /// deallocate internal structures
    void DeallocateLODSortTable();
    /// Initialize the table, must be overwritten
    virtual void InitializeLODSortTable();
    /// process done after InitializeLODSortTable
    void PostInitializeLODSortTable();
    /// initialize the lod for (x,z) position
    void InitMinLOD(int x, int z, int lod);

    /// function used by qsort
    static int VertexLodCompare(const void * p1, const void * p2);

    /// number of vertices in a row
    int numVerticesPerRow;
#ifndef NGAME
    int checkLOD;
#endif

    int * indexLODSort;
    nGMMVertexLOD * minLOD;
    /// number of vertices needed in each level of detail
    int numVerticesLOD[MaxLODLevels];
    /// total number of level of detail
    int numLODLevels;
    /// total number of vertices
    int numVertices;

};

//------------------------------------------------------------------------------
/**
*/
inline
int 
nGMMVertexLODSorting::GetVertexKey(int x, int z) const
{
    return (z * this->numVerticesPerRow) + x;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
inline
void 
nGMMVertexLODSorting::CheckLOD(int lod)
{
    this->checkLOD = lod;
}
#endif

//------------------------------------------------------------------------------
/**
*/
inline
const nGMMVertexLODSorting::nGMMVertexLOD & 
nGMMVertexLODSorting::GetVertex(int index) const
{
    n_assert(index >= 0 && index < this->numVertices);
    return this->minLOD[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nGMMVertexLODSorting::GetVertexCount(int lod) const
{
    n_assert(lod >= 0 && lod < this->numLODLevels);
    return this->numVerticesLOD[lod];
}

//------------------------------------------------------------------------------
/**
    Calculate the number of vertices of a block needed for a given a lod
*/
inline
int 
nGMMVertexLODSorting::CountVertices(int lod) const
{
    int numVertices = 0;
    for(int j = lod;j < this->numLODLevels;j++)
    {
        numVertices += this->GetVertexCount(j);
    }

    return numVertices;
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nGMMVertexLODSorting::GetVertexIndex(int x, int z) const
{
    int key = this->GetVertexKey(x,z);
    int index = this->indexLODSort[key];
#ifndef NGAME
    n_assert(this->minLOD[index].lod >= this->checkLOD);
    n_assert(this->minLOD[index].x == x);
    n_assert(this->minLOD[index].z == z);
#endif
    return index;
}

//------------------------------------------------------------------------------
#endif // N_GMMVERTEXLODNSORTING_H
