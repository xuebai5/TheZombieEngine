#ifndef N_GMMVERTEXSKIRTBUILDER_H
#define N_GMMVERTEXSKIRTBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nGMMVertexSkirtBuilder
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Builder of vertex buffers for Skirt LOD

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngmmvertexbuilder.h"

//------------------------------------------------------------------------------
class nGMMVertexSkirtBuilder : public nGMMVertexBuilder
{
public:

    /// setup the terrain where to get information from
    virtual void SetupTerrain(ncTerrainGMMClass * tgmmc);

    /// fill all vertices for a given block
    virtual void FillVerticesBlock(int bx, int bz, int lod);

protected:

    /// fill the vertices in the skirt
    void FillSkirtVertices(int bx, int bz, int lod);

};

//------------------------------------------------------------------------------
#endif // N_GMMVERTEXSKIRTBUILDER_H
