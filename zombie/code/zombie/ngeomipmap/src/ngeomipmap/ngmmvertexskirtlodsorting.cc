/*-----------------------------------------------------------------------------
    @file ngmmvertexskirtlodsorting.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmvertexskirtlodsorting.h"

//------------------------------------------------------------------------------
/**
*/
void 
nGMMVertexSkirtLODSorting::Setup(ncTerrainGMMClass * gmm)
{
    // blocksize is extended by 2, because of the extra edge lines of vertices
    // for the skirt
    this->numVerticesPerRow = gmm->GetBlockSize() + 2;
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
nGMMVertexSkirtLODSorting::InitializeLODSortTable()
{
    n_assert(this->minLOD);
    n_assert(this->indexLODSort);

    int bs = this->numVerticesPerRow - 2;
    int step = 1;

    // get for each vertex the lowest lod level where it is used
    // and store this in the table minLOD
    int lod = 0;
    while(bs)
    {
        for(int z = 1;z < this->numVerticesPerRow - 1;z+=step) 
        {
            // initialize the inner vertices
            for(int x = 1;x < this->numVerticesPerRow - 1;x+=step) 
            {
                this->InitMinLOD(x, z, lod);
            }

            // initialize the outer vertices
            this->InitMinLOD(z, 0, lod);
            this->InitMinLOD(0, z, lod);
            this->InitMinLOD(z, this->numVerticesPerRow - 1, lod);
            this->InitMinLOD(this->numVerticesPerRow - 1, z, lod);
        }

        // advance one lod level
        lod++;
        bs >>= 1;
        step <<= 1;
    }

    // the points in the corners are re not really used, put them in LOD 0
    this->InitMinLOD(0, 0, 0);
    this->InitMinLOD(0, this->numVerticesPerRow - 1, 0);
    this->InitMinLOD(this->numVerticesPerRow - 1, 0, 0);
    this->InitMinLOD(this->numVerticesPerRow - 1, this->numVerticesPerRow - 1, 0);
}
