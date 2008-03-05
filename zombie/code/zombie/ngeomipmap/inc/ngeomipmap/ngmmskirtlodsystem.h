#ifndef N_GMMSKIRTLODSYSTEM_H
#define N_GMMSKIRTLODSYSTEM_H
//------------------------------------------------------------------------------
/**
    @class nGMMSkirtLODSystem
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief LOD management system for Skirt LOD.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngmmlodsystem.h"
#include "ngeomipmap/ngmmvertexskirtlodsorting.h"
#include "ngeomipmap/ngmmvertexskirtbuilder.h"
#include "ngeomipmap/ngmmindexskirtbuilder.h"
#include "ngeomipmap/ngmmindexcache.h"

//------------------------------------------------------------------------------
class nGMMSkirtLODSystem : public nGMMLODSystem
{
public:

    /// lod leveling for all terrain cells (not needed for skirts)
    virtual void LevelLOD(ncTerrainGMM * tgmm);

protected:

    /// create objects of the proper classes
    virtual nGMMVertexLODSorting * CreateVertexSorting();
    virtual nGMMVertexBuilder * CreateVertexBuilder();
    virtual nGMMIndexBuilder * CreateIndexBuilder();
    virtual nGMMIndexCache * CreateIndexCache();

};

//------------------------------------------------------------------------------
inline
nGMMVertexLODSorting * 
nGMMSkirtLODSystem::CreateVertexSorting()
{
    return n_new(nGMMVertexSkirtLODSorting);
}

//------------------------------------------------------------------------------
inline
nGMMVertexBuilder * 
nGMMSkirtLODSystem::CreateVertexBuilder()
{
    return n_new(nGMMVertexSkirtBuilder);
}

//------------------------------------------------------------------------------
inline
nGMMIndexBuilder * 
nGMMSkirtLODSystem::CreateIndexBuilder()
{
    return n_new(nGMMIndexSkirtBuilder);
}

//------------------------------------------------------------------------------
inline
nGMMIndexCache * 
nGMMSkirtLODSystem::CreateIndexCache()
{
    return n_new(nGMMIndexCache);
}

//------------------------------------------------------------------------------
/**
    One of the advantages of skirt lod system is that the leveling is no longer
    needed.
*/
inline
void 
nGMMSkirtLODSystem::LevelLOD(ncTerrainGMM * /*tgmm*/)
{
    /// empty
}

//------------------------------------------------------------------------------
#endif // N_GMMSKIRTLODSYSTEM_H
