#ifndef N_GMMLOD1SYSTEM_H
#define N_GMMLOD1SYSTEM_H
//------------------------------------------------------------------------------
/**
    @class nGMMLOD1System
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief LOD management system for max LOD difference of 1.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngmmlodsystem.h"

//------------------------------------------------------------------------------
class nGMMLOD1System : public nGMMLODSystem
{
public:

protected:

    /// create objects of the proper classes
    virtual nGMMVertexLODSorting * CreateVertexSorting();
    virtual nGMMIndexBuilder * CreateIndexBuilder();

};

//------------------------------------------------------------------------------
inline
nGMMVertexLODSorting * 
nGMMLOD1System::CreateVertexSorting()
{
    return n_new(nGMMVertexLODSorting);
}

//------------------------------------------------------------------------------
inline
nGMMIndexBuilder * 
nGMMLOD1System::CreateIndexBuilder()
{
    return n_new(nGMMIndexLOD1Builder);
}

//------------------------------------------------------------------------------
#endif // N_GMMLOD1SYSTEM_H
