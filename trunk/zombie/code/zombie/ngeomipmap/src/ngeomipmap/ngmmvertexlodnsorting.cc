/*-----------------------------------------------------------------------------
    @file ngmmvertexlodnsorting.cc
    @ingroup NebulaTerrain

    @author Mateu Batle Sastre

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmvertexlodnsorting.h"

//------------------------------------------------------------------------------
/**
*/
void
nGMMVertexLODNSorting::InitializeLODSortTable()
{
    nGMMVertexLODSorting::InitializeLODSortTable();

    // all vertices beloging to 4 edges belong to the lowest level of detail
    int key;
    for(int i = 0;i < this->numVerticesPerRow;i++) 
    {
        key = this->GetVertexKey(i,0);
        this->minLOD[key].x = i;
        this->minLOD[key].z = 0;
        this->minLOD[key].lod = this->numLODLevels - 1;

        key = this->GetVertexKey(0,i);
        this->minLOD[key].x = 0;
        this->minLOD[key].z = i;
        this->minLOD[key].lod = this->numLODLevels - 1;

        key = this->GetVertexKey(this->numVerticesPerRow - 1,i);
        this->minLOD[key].x = this->numVerticesPerRow - 1;
        this->minLOD[key].z = i;
        this->minLOD[key].lod = this->numLODLevels - 1;

        key = this->GetVertexKey(i,this->numVerticesPerRow - 1);
        this->minLOD[key].x = i;
        this->minLOD[key].z = this->numVerticesPerRow - 1;
        this->minLOD[key].lod = this->numLODLevels - 1;
    }
}
