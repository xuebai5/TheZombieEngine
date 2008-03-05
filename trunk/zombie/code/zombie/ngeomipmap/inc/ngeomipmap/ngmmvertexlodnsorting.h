#ifndef N_GMMVERTEXLODNSORTING_H
#define N_GMMVERTEXLODNSORTING_H
//------------------------------------------------------------------------------
/**
    @class nGMMVertexLODNSorting
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Sorting of vertices with LOD. For neighbor blocks with any 
    LOD difference.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngmmvertexlodsorting.h"

//------------------------------------------------------------------------------
/**
*/
class nGMMVertexLODNSorting : public nGMMVertexLODSorting
{
protected:

    virtual void InitializeLODSortTable();

};

//------------------------------------------------------------------------------
#endif // N_GMMVERTEXLODNSORTING_H
