#ifndef N_GMMINDEXLODNBUILDER_H
#define N_GMMINDEXLODNBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nGMMIndexHQBuilder
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Builder of index buffers for the geomipmap terrains.
    This is index provides a higher quality LOD. It allows conenction of
    blocks between LOD difference greater than one.

    worst case blocksize = 129, lod levels = 0..7 (8 levels)

    level 0 (max)   - 1 entry 128 x 128 x 2 = 32K tris * 1 (1^4) variations (3 * 64 Kbytes)
    level 1         - 1 entry 64 x 64 x 2 = 8K tris * 16 (2^4) variations (3 * 256 Kbytes)
    level 2         - 1 entry 32 x 32 x 2 = 2K tris * 81 (3^4) variations (3 * 330 Kbytes)
    level 3         - 1 entry 16 x 16 x 2 = 512 tris * 256 (4^4) variations (3 * 262 Kbytes)
    level 4         - 1 entry 8 x 8 x 2 = 128 tris * 625 (5^4) variations (3 * 160 Kbytes)
    level 5         - 1 entry 4 x 4 x 2 = 32 tris * 1296 (6^4) variations (3 * 82 Kbytes)
    level 6         - 1 entry 2 x 2 x 2 = 8 tris * 2401 (7^4) variations (3 * 38 Kbytes)
    level 7         - 1 entry 1 x 1 x 2 = 2 tris * 4096 (8^4) variations (3 * 16 KBytes)

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
class nGMMIndexLODNBuilder : public nGMMIndexBuilder
{
public:

    /// fill all the indices for a block with the provided lod settings
    virtual void FillIndicesBlock(int lod, int lodN, int lodE, int lodS, int lodW);
    /// get max number of indices needed for a given LOD
    virtual int CountIndices(int lod);
    /// get max number of indices needed for a given LOD
    virtual int CountGroups(int lod);
    /// parse the index group number and obtain LOD values
    virtual void ParseIndexGroup(int lod, int group, int & lodN, int & lodE, int & lodS, int & lodW);
    /// get the index group number 
    virtual int CalcIndexGroup(int lod, int lodN, int lodE, int lodS, int lodW);

protected:

    /// return true if has crack with neighboor
    bool HasNotCrackFixBand(int lod, int lodSide1, int lodSide2);

    void FillIndicesNorth(int lod, int lodN, int lodE, int lodS, int lodW);
    void FillIndicesWest(int lod, int lodN, int lodE, int lodS, int lodW);
    void FillIndicesSouth(int lod, int lodN, int lodE, int lodS, int lodW);
    void FillIndicesEast(int lod, int lodN, int lodE, int lodS, int lodW);

};

//------------------------------------------------------------------------------
#endif // N_GMMINDEXLODNBUILDER_H
