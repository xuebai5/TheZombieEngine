#ifndef N_GMMLODNSYSTEM_H
#define N_GMMLODNSYSTEM_H
//------------------------------------------------------------------------------
/**
    @class nGMMLODNSystem
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief LOD management system for any LOD difference between neighbor cells.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngmmlodsystem.h"

//------------------------------------------------------------------------------
class nGMMLODNSystem : public nGMMLODSystem
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
nGMMLODNSystem::CreateVertexSorting()
{
    return n_new(nGMMVertexLODNSorting);
}

//------------------------------------------------------------------------------
inline
nGMMIndexBuilder * 
nGMMLODNSystem::CreateIndexBuilder()
{
    return n_new(nGMMIndexLODNBuilder);
}

//------------------------------------------------------------------------------
#endif // N_GMMLODNSYSTEM_H
