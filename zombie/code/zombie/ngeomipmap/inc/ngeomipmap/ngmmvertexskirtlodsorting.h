#ifndef N_GMMVERTEXSKIRTLODSORTING_H
#define N_GMMVERTEXSKIRTLODSORTING_H
//------------------------------------------------------------------------------
/**
    @class nGMMVertexSkirtLODSorting
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Vertex LOD Sorting for Skirt LOD

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngmmvertexlodsorting.h"

//------------------------------------------------------------------------------
class ncTerrainGMMClass;

//------------------------------------------------------------------------------
/**
*/
class nGMMVertexSkirtLODSorting : public nGMMVertexLODSorting
{
public:

    /// Setup from the terrain geomipmap class
    virtual void Setup(ncTerrainGMMClass *);

protected:

    void InitializeLODSortTable();

};

//------------------------------------------------------------------------------
#endif // N_GMMVERTEXSKIRTLODSORTING_H
