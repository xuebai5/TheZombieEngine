#ifndef N_GMMINDEXSKIRTBUILDER_H
#define N_GMMINDEXSKIRTBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nGMMIndexBuilder
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Builder of index buffers for the geomipmap terrains.
    This is just a common interface to develop other builders.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nref.h"
#include "kernel/nreferenced.h"
#include "ngeomipmap/ngmmvertexskirtlodsorting.h"
#include "ngeomipmap/ngmmindexbuilder.h"

//------------------------------------------------------------------------------
class ncTerrainGMMClass;
class ncPhyTerrainCell;

//------------------------------------------------------------------------------
class nGMMIndexSkirtBuilder : public nGMMIndexBuilder
{
public:

    /// setup the index builder with terrain provided
    virtual void SetupTerrain(ncTerrainGMMClass * tgmmc);

    /// fill all the indices for a block with the provided lod settings
    virtual void FillIndicesBlock(int lod, int lodN, int lodE, int lodS, int lodW);
    /// fill all the indices for a block (bx,bz) which might have holes
    virtual void FillIndicesHoledBlock(int bx, int bz, int lod, int lodN, int lodE, int lodS, int lodW);
    /// get max number of indices needed for a given LOD
    virtual int CountIndices(int lod);
    /// get max number of groups in a given LOD
    virtual int CountGroups(int lod);
    /// parse the index group number and obtain LOD values
    virtual void ParseIndexGroup(int lod, int group, int & lodN, int & lodE, int & lodS, int & lodW);
    /// get the index group number 
    virtual int CalcIndexGroup(int lod, int lodN, int lodE, int lodS, int lodW);

protected:

    /// convet from skirt coordinates to heightmap coordinates
    int SkirtToHeightmapCoord( int coord ) const;

    /// fill all the indices for a block with the provided lod settings
    template <nGMMIndexBuilder::AddTriangleType>
    void FillIndicesBlockAux(int lod);

    /// add triangle checking if each vertex is holed or not
    void AddTriangleHoleSafe(int x1, int z1, int x2, int z2, int x3, int z3);

    /// counter number of skirt vertices
    int CountSkirtIndices(int lod);

    /// fill all indices for the four skirts (NSWE)
    template <nGMMIndexBuilder::AddTriangleType>
    void FillIndexSkirts(int lod);

    /// true if cell has a hole at north, south, west or east
    bool holeN, holeS, holeW, holeE;

};

//------------------------------------------------------------------------------
#endif // N_GMMINDEXSKIRTBUILDER_H
