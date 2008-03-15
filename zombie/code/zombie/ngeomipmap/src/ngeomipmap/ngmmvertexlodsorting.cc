//------------------------------------------------------------------------------
//  ngmmvertexlodsorting.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmvertexlodsorting.h"
#include <stdlib.h>

//------------------------------------------------------------------------------
/**
*/
nGMMVertexLODSorting::nGMMVertexLODSorting() :
    numVerticesPerRow(0),
#ifndef NGAME
    checkLOD(0),
#endif
    indexLODSort(0),
    minLOD(0),
    numLODLevels(0),
    numVertices(0)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
nGMMVertexLODSorting::~nGMMVertexLODSorting()
{
    this->DeallocateLODSortTable();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMVertexLODSorting::Setup(ncTerrainGMMClass * gmm)
{
    this->numVerticesPerRow = gmm->GetBlockSize();
    this->numVertices = this->numVerticesPerRow * this->numVerticesPerRow;
    this->numLODLevels = gmm->GetNumLODLevels();
    this->AllocateLODSortTable();
    this->InitializeLODSortTable();
    this->PostInitializeLODSortTable();
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMVertexLODSorting::AllocateLODSortTable()
{
    n_assert(!this->minLOD);
    n_assert(!this->indexLODSort);
    this->minLOD = n_new_array(nGMMVertexLOD, this->numVertices);
    n_assert(this->minLOD);
    memset(this->minLOD, ~0, this->numVertices * sizeof(nGMMVertexLOD));
    this->indexLODSort = n_new_array(int, this->numVertices);
    n_assert(this->indexLODSort);
    memset(this->indexLODSort, ~0, this->numVertices * sizeof(int));
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMVertexLODSorting::DeallocateLODSortTable()
{
    if (this->minLOD)
    {
        n_delete_array(this->minLOD);
        this->minLOD = 0;
    }
    if (this->indexLODSort)
    {
        n_delete_array(indexLODSort);
        this->indexLODSort = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMVertexLODSorting::InitializeLODSortTable()
{
    n_assert(this->minLOD);
    n_assert(this->indexLODSort);

    int bs = this->numVerticesPerRow;
    int step = 1;

    // get for each vertex the lowest lod level where it is used
    // and store this in the table minLOD
    int lod = 0;
    while(bs)
    {
        for(int z = 0;z < this->numVerticesPerRow;z+=step) 
        {
            for(int x = 0;x < this->numVerticesPerRow;x+=step) 
            {
                this->InitMinLOD(x, z, lod);
            }
        }

        // advance one lod level
        lod++;
        bs >>= 1;
        step <<= 1;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMVertexLODSorting::PostInitializeLODSortTable()
{
    // sort the minLOD table
    qsort(this->minLOD, this->numVertices, sizeof(this->minLOD[0]), VertexLodCompare);

    // clean number of vertices for each LOD
    for(int i = 0;i < MaxLODLevels;i++)
    {
        this->numVerticesLOD[i] = 0;
    }

    // scan the minLOD table to fill the indexLODSort table
    for(int i = 0;i < this->numVertices;i++)
    {
        int vertexKey = this->GetVertexKey(this->minLOD[i].x, this->minLOD[i].z);
        this->indexLODSort[ vertexKey ] = i; 
        this->numVerticesLOD[ this->minLOD[i].lod ]++;
    }

    // this is just for checking everything was built correctly
    for(int z = 0;z < this->numVerticesPerRow;z++)
    {
        for(int x = 0;x < this->numVerticesPerRow;x++)
        {
            GetVertexIndex(x, z);
        }
    }   
}

//------------------------------------------------------------------------------
/**
*/
int 
nGMMVertexLODSorting::VertexLodCompare(const void * p1, const void * p2) 
{
    const nGMMVertexLOD * vl1 = reinterpret_cast<const nGMMVertexLOD *> (p1);
    const nGMMVertexLOD * vl2 = reinterpret_cast<const nGMMVertexLOD *> (p2);
    n_assert(vl1->x != ~0 || vl1->z != ~0);
    n_assert(vl2->x != ~0 || vl2->z != ~0);
    // sort in descending LOD order
    int diff = (vl2->lod - vl1->lod);
    if (!diff)
    {
        diff = (vl1->z - vl2->z);
        if (!diff)
        {
            diff = (vl1->x - vl2->x);
        }
    }
    return diff;
}

//------------------------------------------------------------------------------
/**
*/
void 
nGMMVertexLODSorting::InitMinLOD(int x, int z, int lod)
{
    int key = this->GetVertexKey(x,z);
    n_assert(this->minLOD[key].x == ~0 || this->minLOD[key].x == x);
    n_assert(this->minLOD[key].z == ~0 || this->minLOD[key].z == z);
    n_assert(this->minLOD[key].lod == ~0 || lod > this->minLOD[key].lod);
    this->minLOD[key].lod = lod;
    this->minLOD[key].x = x;
    this->minLOD[key].z = z;
}
