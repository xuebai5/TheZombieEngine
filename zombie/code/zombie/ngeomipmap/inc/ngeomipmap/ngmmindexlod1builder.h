#ifndef N_GMMINDEXLOD1BUILDER_H
#define N_GMMINDEXLOD1BUILDER_H
//------------------------------------------------------------------------------
/**
    @class nGMMIndexLOD1Builder
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Builder of index buffers for the geomipmap terrains.
    There is a difference LOD 1.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nref.h"
#include "kernel/nreferenced.h"
#include "ngeomipmap/nfloatmap.h"
#include "ngeomipmap/ngmmindexbuilder.h"

//------------------------------------------------------------------------------
class ncTerrainGMMClass;
class nGMMGeometryStorage;
class nGMMVertexLODSorting;

//------------------------------------------------------------------------------
class nGMMIndexLOD1Builder : public nGMMIndexBuilder
{
public:

    /// fill all the indices for a block with the provided lod settings
    virtual void FillIndicesBlock(int lod, int lodN, int lodE, int lodS, int lodW);
    /// fill all the indices for a block (bx,bz) which might have holes
    virtual void FillIndicesHoledBlock(int bx, int bz, int lod, int lodN, int lodE, int lodS, int lodW);
    /// get max number of indices needed for a given LOD
    virtual int CountIndices(int lod);
    /// get max number of indices needed for a given LOD
    virtual int CountGroups(int lod);
    /// parse the index group number and obtain LOD values
    virtual void ParseIndexGroup(int lod, int group, int & lodN, int & lodE, int & lodS, int & lodW);
    /// get the index group number 
    virtual int CalcIndexGroup(int lod, int lodN, int lodE, int lodS, int lodW);

protected:

    /// fill all the indices for a block with the provided lod settings
    template <nGMMIndexBuilder::AddTriangleType>
    void FillIndicesBlockAux(int lod, int lodN, int lodE, int lodS, int lodW);

    void ParseIndexGroup(int group, bool & crackN, bool & crackE, bool & crackS, bool & crackW);

    /// return true if has crack with neighboor
    bool HasCrack(int lod, int lodN);

    template <nGMMIndexBuilder::AddTriangleType>
    void FillIndexTriangleListNorth(bool crackW, bool crackE, int lodstep);
    template <nGMMIndexBuilder::AddTriangleType>
    void FillIndexTriangleListWest(bool crackN, bool crackS, int lodstep);
    template <nGMMIndexBuilder::AddTriangleType>
    void FillIndexTriangleListSouth(bool crackW, bool crackE, int lodstep);
    template <nGMMIndexBuilder::AddTriangleType>
    void FillIndexTriangleListEast(bool crackN, bool crackS, int lodstep);

};

//------------------------------------------------------------------------------
#endif // N_GMMINDEXLOD1BUILDER_H
