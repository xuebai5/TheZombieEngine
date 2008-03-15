//------------------------------------------------------------------------------
//  ngmmindexskirtbuilder.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmindexskirtbuilder.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "nphysics/ncphyterraincell.h"

//------------------------------------------------------------------------------
/**
*/
void 
nGMMIndexSkirtBuilder::SetupTerrain(ncTerrainGMMClass * tgmmc)
{
    nGMMIndexBuilder::SetupTerrain(tgmmc);
    if (tgmmc)
    {
        // there is 1 vertex more in each side for the skirt (+2 total)
        this->blockSize += 2;
    }
}

//------------------------------------------------------------------------------
/**
    FillIndicesBlockAux fills all the triangles for a block including the skirts
    for the provided level of detail.
    This method is templatized by a AddTriangleFunc, this allows to handle 
    the cases of holes.
*/
template <nGMMIndexBuilder::AddTriangleType AddTriangleFunc>
void 
nGMMIndexSkirtBuilder::FillIndicesBlockAux(int lod)
{
    NLOG(terrain, (nTerrainLog::NLOG_BUILDINDEX | 0, "nGMMIndexSkirtBuilder::FillIndicesBlockAux(%d)", lod));

    // setup positions
    int inix = 1;
    int iniz = 1;
    int endz = (this->blockSize - 2);
    int endx = (this->blockSize - 2);
    bool upwards = true;

    // calculate increments for each LOD
    int lodstep  = (1 << lod);

    // get and lock index buffer
    //this->geometryStorage->LockIndicesGroup(6 * (endx - inix) * (endz - iniz) / (lodstep * lodstep));
    this->geometryStorage->LockIndicesGroup( this->CountIndices(lod) );

#ifndef NGAME
    this->vertexSorting->CheckLOD(lod);
#endif

    // calculate triangles indices for the center block
    for(int z = iniz; z < endz; z += lodstep) 
    {
        // x in upwards direction
        if (upwards) 
        {
            // triangle strips (left to right)
            for(int x = inix;x < endx; x += lodstep) 
            {
                // ordering as / / / / 
                (this->*AddTriangleFunc)(
                    x, z,
                    x, z + lodstep,
                    x + lodstep, z
                );
                (this->*AddTriangleFunc)(
                    x, z + lodstep,
                    x + lodstep, z + lodstep,
                    x + lodstep, z
                );
            }
        } 
        else 
        {
            // triangle lists (right to left)
            for (int x = endx; x > inix; x -= lodstep)
            {
                (this->*AddTriangleFunc)(
                    x, z,
                    x - lodstep, z + lodstep,
                    x, z + lodstep
                );
                (this->*AddTriangleFunc)(
                    x, z,
                    x - lodstep, z,
                    x - lodstep, z + lodstep
                );
            }
        }
        upwards = !upwards;
    }

    // generate triangles to fix the cracks in each direction
    this->FillIndexSkirts<AddTriangleFunc>(lod);

    this->geometryStorage->UnlockIndicesGroup();

#ifndef NGAME
    this->vertexSorting->CheckLOD(0);
#endif
}

//------------------------------------------------------------------------------
/**
    Clamp a skirt coordinate to the existing heightmap coordinates.
    A vertex in the skirt would be transformed to nearest vertex in the 
    heightmap cell.
    E.g. in a terrain cell having a 5x5 heightmap, we would have 7x7 
    addressable coordinates, this would get:
    0 -> 0
    1 -> 0
    2 -> 1
    3 -> 2
    4 -> 3
    5 -> 4
    6 -> 4
*/
int
nGMMIndexSkirtBuilder::SkirtToHeightmapCoord( int coord ) const
{
    return min( max(0, coord - 1), this->blockSize - 3);
}

//------------------------------------------------------------------------------
/**
    Adds a triangle checking if the triangle lies in a hole or not.
    The 3 vertices coordinates are provided (x,z)
*/
void
nGMMIndexSkirtBuilder::AddTriangleHoleSafe(int x1, int z1, int x2, int z2, int x3, int z3)
{
    // the skirt coordinates have to be converted in heightmap coordinates
    int hmx1 = this->SkirtToHeightmapCoord( x1 );
    int hmx2 = this->SkirtToHeightmapCoord( x2 );
    int hmx3 = this->SkirtToHeightmapCoord( x3 );
    int hmz1 = this->SkirtToHeightmapCoord( z1 );
    int hmz2 = this->SkirtToHeightmapCoord( z2 );
    int hmz3 = this->SkirtToHeightmapCoord( z3 );
    
    if (!this->phyCell->GetHole(hmx1, hmz1) ||
        !this->phyCell->GetHole(hmx2, hmz2) ||
        !this->phyCell->GetHole(hmx3, hmz3))
    {
        this->geometryStorage->AddTriangle(
            this->vertexSorting->GetVertexIndex(x1, z1),
            this->vertexSorting->GetVertexIndex(x2, z2),
            this->vertexSorting->GetVertexIndex(x3, z3)
        );
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMIndexSkirtBuilder::FillIndicesBlock(int lod, int /*lodN*/, int /*lodE*/, int /*lodS*/, int /*lodW*/)
{
    this->holeN = false;
    this->holeS = false;
    this->holeW = false;
    this->holeE = false;

    this->FillIndicesBlockAux<&nGMMIndexBuilder::AddTriangle>(lod);
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMIndexSkirtBuilder::FillIndicesHoledBlock(int bx, int bz, int lod, int /*lodN*/, int /*lodE*/, int /*lodS*/, int /*lodW*/)
{
    int numBlocks = this->tgmmc->GetNumBlocks();
    ncPhyTerrainCell * neighborPhyCell;

    // north
    neighborPhyCell = this->GetPhysicalCell(bx, max(0, bz - 1));
    this->holeN = neighborPhyCell->IsThereAnyHole();
    // south
    neighborPhyCell = this->GetPhysicalCell(bx, min(numBlocks - 1, bz + 1));
    this->holeS = neighborPhyCell->IsThereAnyHole();
    // west
    neighborPhyCell = this->GetPhysicalCell(max(0, bx - 1), bz);
    this->holeW = neighborPhyCell->IsThereAnyHole();
    // east
    neighborPhyCell = this->GetPhysicalCell(min(numBlocks - 1, bx + 1), bz);
    this->holeE = neighborPhyCell->IsThereAnyHole();

    // get the physical terrain cell (accesed later by AddTriangleHoleSafe)
    this->phyCell = this->GetPhysicalCell(bx, bz);

    this->FillIndicesBlockAux<
        static_cast<nGMMIndexBuilder::AddTriangleType>(&nGMMIndexSkirtBuilder::AddTriangleHoleSafe) >(lod);

    this->phyCell = 0;
    this->holeN = false;
    this->holeS = false;
    this->holeW = false;
    this->holeE = false;
}

//------------------------------------------------------------------------------
/**
    Builds a skirt for the north part of the terrain cell block.
*/
template <nGMMIndexBuilder::AddTriangleType AddTriangleFunc>
void
nGMMIndexSkirtBuilder::FillIndexSkirts(int lod)
{
    NLOG(terrain, (nTerrainLog::NLOG_BUILDINDEX | 0, "nGMMIndexSkirtBuilder::FillIndexSkirts(%d)", lod));

    int lodstep = (1 << lod);
    int ini = 1;
    int end = this->blockSize - 2;

    for( int i = ini; i < end; i += lodstep )
    {
        // north
        if (!this->holeN)
        {
            (this->*AddTriangleFunc)(i, 1, i + lodstep, 0, i, 0);
            (this->*AddTriangleFunc)(i, 1, i + lodstep, 1, i + lodstep, 0);
        }
        // south
        if (!this->holeS)
        {
            (this->*AddTriangleFunc)(i, end + 1, i + lodstep, end, i, end);
            (this->*AddTriangleFunc)(i, end + 1, i + lodstep, end + 1, i + lodstep, end);
        }
        // west
        if (!this->holeW)
        {
            (this->*AddTriangleFunc)(0, i, 0, i + lodstep, 1, i);
            (this->*AddTriangleFunc)(0, i + lodstep, 1, i + lodstep, 1, i);
        }
        // east
        if (!this->holeE)
        {
            (this->*AddTriangleFunc)(end + 1, i + lodstep, end + 1, i, end, i);
            (this->*AddTriangleFunc)(end, i + lodstep, end + 1, i + lodstep, end, i);
        }
    }
}

//------------------------------------------------------------------------------
int 
nGMMIndexSkirtBuilder::CountSkirtIndices(int lod)
{
    return (((this->blockSize - 3) >> lod) * 2) * 3;
}

//------------------------------------------------------------------------------
int 
nGMMIndexSkirtBuilder::CountIndices(int lod)
{
    int numpoints = (this->blockSize - 2) >> lod;
    return numpoints * numpoints * 2 * 3 + 4 * this->CountSkirtIndices(lod);
}

//------------------------------------------------------------------------------
int 
nGMMIndexSkirtBuilder::CountGroups(int /*lod*/)
{
    return 1;
}

//------------------------------------------------------------------------------
void
nGMMIndexSkirtBuilder::ParseIndexGroup(int lod, int /*group*/, int & lodN, int & lodE, int & lodS, int & lodW)
{
    lodN = lod;
    lodE = lod;
    lodS = lod;
    lodW = lod;
}

//------------------------------------------------------------------------------
int 
nGMMIndexSkirtBuilder::CalcIndexGroup(int /*lod*/, int /*lodN*/, int /*lodE*/, int /*lodS*/, int /*lodW*/)
{
    return 0;
}
