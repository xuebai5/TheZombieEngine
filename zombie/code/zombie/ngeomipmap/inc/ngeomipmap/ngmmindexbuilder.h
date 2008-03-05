#ifndef N_GMMINDEXBUILDER_H
#define N_GMMINDEXBUILDER_H
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
#include "ngeomipmap/nfloatmap.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "ngeomipmap/ngmmvertexlodsorting.h"

//------------------------------------------------------------------------------
class ncTerrainGMMClass;
class ncPhyTerrainCell;

//------------------------------------------------------------------------------
class nGMMIndexBuilder
{
public:
    /// constructor
    nGMMIndexBuilder();
    /// destructor
    ~nGMMIndexBuilder();

    /// setup the terrain where to get information from
    virtual void SetupTerrain(ncTerrainGMMClass * tgmmc);
    /// setup the storage where to store indices
    void SetupStorage(nGMMGeometryStorage * geom);
    /// setup the vertex sorting
    void SetupVertexSorting(nGMMVertexLODSorting * vertexSorting);

    /// fill all the indices for a block with the provided lod settings
    virtual void FillIndicesBlock(int lod, int lodN, int lodE, int lodS, int lodW) = 0;
    /// fill all the indices for a block (bx,bz) which might have holes
    virtual void FillIndicesHoledBlock(int bx, int bz, int lod, int lodN, int lodE, int lodS, int lodW);
    /// get max number of indices needed for a given LOD
    virtual int CountIndices(int lod) = 0;
    /// get max number of indices needed for a given LOD
    virtual int CountGroups(int lod) = 0;
    /// parse the index group number and obtain LOD values
    virtual void ParseIndexGroup(int lod, int group, int & lodN, int & lodE, int & lodS, int & lodW) = 0;
    /// get the index group number 
    virtual int CalcIndexGroup(int lod, int lodN, int lodE, int lodS, int lodW) = 0;

    /// member function type used to add triangles (used internally)
    typedef void (nGMMIndexBuilder::*AddTriangleType)(int, int, int, int, int, int);

protected:

    /// add triangle connecting points (x1,z1), (x2,z2) and (x3 z3)
    void AddTriangle(int x1, int z1, int x2, int z2, int x3, int z3);
    /// add triangle connecting points, triangle discard if 3 points are holes
    void AddTriangleHoleSafe(int x1, int z1, int x2, int z2, int x3, int z3);
    /// get physical cell for block (bx, bz)
    ncPhyTerrainCell * GetPhysicalCell(int bx, int bz);

    /// geometry storage
    nGMMGeometryStorage * geometryStorage;

    /// vertex sorting
    nGMMVertexLODSorting * vertexSorting;

    int blockSize;
    int numLODLevels;

    /// terrain geomipmap class
    ncTerrainGMMClass * tgmmc;
    /// points to the current cell being built (used for holes)
    ncPhyTerrainCell * phyCell;

};

//------------------------------------------------------------------------------
inline
void
nGMMIndexBuilder::AddTriangle(int x1, int z1, int x2, int z2, int x3, int z3)
{
    NLOG(terrain, (nTerrainLog::NLOG_BUILDINDEX | 1, "nGMMIndexBuilder::AddTriangle(%d,%d,%d,%d,%d,%d)", x1, z1, x2, z2, x3, z3));

    this->geometryStorage->AddTriangle(
        this->vertexSorting->GetVertexIndex(x1, z1),
        this->vertexSorting->GetVertexIndex(x2, z2),
        this->vertexSorting->GetVertexIndex(x3, z3)
    );
}

//------------------------------------------------------------------------------
#endif // N_GMMINDEXBUILDER_H
