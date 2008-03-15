//------------------------------------------------------------------------------
//  ngmmindexlod1builder.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmindexlod1builder.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "ngeomipmap/ngmmvertexlodsorting.h"
#include "kernel/nlogclass.h"

#include "nphysics/ncphyterraincell.h"

//------------------------------------------------------------------------------
/**
*/
template <nGMMIndexBuilder::AddTriangleType AddTriangleFunc>
void 
nGMMIndexLOD1Builder::FillIndicesBlockAux(int lod, int lodN, int lodE, int lodS, int lodW)
{
    n_assert(abs(lod - lodN) <= 1);
    n_assert(abs(lod - lodE) <= 1);
    n_assert(abs(lod - lodS) <= 1);
    n_assert(abs(lod - lodW) <= 1);

    // setup positions
    int inix = 0;
    int iniz = 0;
    int endz = (iniz + this->blockSize - 1);
    int endx = (inix + this->blockSize - 1);
    bool upwards = true;

    // calculate increments for each LOD
    int lodstep  = (1 << lod);
    int lodstepN = (1 << lodN);
    int lodstepE = (1 << lodE);
    int lodstepS = (1 << lodS);
    int lodstepW = (1 << lodW);

    // calculate if there is crack or not
    bool crackN = (lodstep < lodstepN);
    bool crackE = (lodstep < lodstepE);
    bool crackS = (lodstep < lodstepS);
    bool crackW = (lodstep < lodstepW);

    // adjust the initial positions of the center block depending on neighbor lod
    if (crackN)
    {
        iniz += lodstep;
    }
    if (crackS)
    {
        endz -= lodstep;
    }
    if (crackE)
    {
        endx -= lodstep;
    }
    if (crackW)
    {
        inix += lodstep;
    }

    // get and lock index buffer
    this->geometryStorage->LockIndicesGroup(6 * (endx - inix) * (endz - iniz) / (lodstep * lodstep));

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
    this->geometryStorage->UnlockIndicesGroup();

    // generate triangles to fix the cracks in each direction
    if (crackN)
    {
        FillIndexTriangleListNorth<AddTriangleFunc>(crackW, crackE, lodstep);
    }
    if (crackS)
    {
        FillIndexTriangleListSouth<AddTriangleFunc>(crackW, crackE, lodstep);
    }
    if (crackW)
    {
        FillIndexTriangleListWest<AddTriangleFunc>(crackN, crackS, lodstep);
    }
    if (crackE)
    {
        FillIndexTriangleListEast<AddTriangleFunc>(crackN, crackS, lodstep);
    }

#ifndef NGAME
    this->vertexSorting->CheckLOD(0);
#endif
}


//------------------------------------------------------------------------------
/**
*/
void
nGMMIndexLOD1Builder::FillIndicesBlock(int lod, int lodN, int lodE, int lodS, int lodW)
{
    this->FillIndicesBlockAux<&nGMMIndexBuilder::AddTriangle>(lod, lodN, lodE, lodS, lodW);
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMIndexLOD1Builder::FillIndicesHoledBlock(int bx, int bz, int lod, int lodN, int lodE, int lodS, int lodW)
{
    // get the physical terrain cell (accesed later by AddTriangleHoleSafe)
    this->phyCell = this->GetPhysicalCell(bx, bz);

    this->FillIndicesBlockAux<&nGMMIndexBuilder::AddTriangleHoleSafe>(lod, lodN, lodE, lodS, lodW);

    this->phyCell = 0;
}

//------------------------------------------------------------------------------
/**
*/
template <nGMMIndexBuilder::AddTriangleType AddTriangleFunc>
void
nGMMIndexLOD1Builder::FillIndexTriangleListNorth(bool crackW, bool crackE, int lodstep)
{
    int inix = 0;
    int endx = (inix + this->blockSize - 1);
    int dblstep = (lodstep << 1);
    int z = 0;

    if ((inix + dblstep) > endx)
    {
        return;
    }

    this->geometryStorage->LockIndicesGroup((endx - inix) * 9 / dblstep);
    for(int x = inix;x < endx; x += dblstep)
    {
            (this->*AddTriangleFunc)(
                x, z,
                x + lodstep, z + lodstep,
                x + dblstep, z
            );
            if ( !(crackE && (endx - dblstep) == x) )
            {
                (this->*AddTriangleFunc)(
                    x + lodstep, z + lodstep,
                    x + dblstep, z + lodstep,
                    x + dblstep, z
                );
            }
            if ( !(crackW && 0 == x) )
            {
                (this->*AddTriangleFunc)(
                    x, z,
                    x, z + lodstep,
                    x + lodstep, z + lodstep
                );
            }
    }
    this->geometryStorage->UnlockIndicesGroup();
}

//------------------------------------------------------------------------------
/**
*/
template <nGMMIndexBuilder::AddTriangleType AddTriangleFunc>
void
nGMMIndexLOD1Builder::FillIndexTriangleListWest(bool crackN, bool crackS, int lodstep)
{
    int iniz = 0;
    int endz = (iniz + this->blockSize - 1);
    int dblstep = (lodstep << 1);
    int x = 0;

    if ((iniz + dblstep) > endz)
    {
        return;
    }

    this->geometryStorage->LockIndicesGroup((endz - iniz) * 9 / dblstep);
    for(int z = iniz; z < endz; z += dblstep)
    {
        (this->*AddTriangleFunc)(
            x, z + dblstep,
            x + lodstep, z + lodstep,
            x, z
        );
        if ( !(crackS && (endz - dblstep) == z) )
        {
            (this->*AddTriangleFunc)(
                x, z + dblstep,
                x + lodstep, z + dblstep,
                x + lodstep, z + lodstep
            );
        }
        if ( !(crackN && 0 == z) )
        {
            (this->*AddTriangleFunc)(
                x + lodstep, z + lodstep,
                x + lodstep, z,
                x, z
            );
        }
    }
    this->geometryStorage->UnlockIndicesGroup();
}

//------------------------------------------------------------------------------
/**
*/
template <nGMMIndexBuilder::AddTriangleType AddTriangleFunc>
void
nGMMIndexLOD1Builder::FillIndexTriangleListSouth(bool crackW, bool crackE, int lodstep)
{
    int inix = 0;
    int endx = (inix + this->blockSize - 1);
    int dblstep = (lodstep << 1);
    int z = (this->blockSize - 1) - lodstep;

    if ((inix + dblstep) > endx)
    {
        return;
    }

    this->geometryStorage->LockIndicesGroup((endx - inix) * 9 / dblstep);
    for(int x = inix; x < endx; x += dblstep)
    {
        (this->*AddTriangleFunc)(
            x + dblstep, z + lodstep,
            x + lodstep, z,
            x, z + lodstep
        );
        if ( !(crackE && (endx - dblstep) == x) )
        {
            (this->*AddTriangleFunc)(
                x + dblstep, z + lodstep,
                x + dblstep, z,
                x + lodstep, z
            );
        }
        if ( !(crackW && 0 == x) )
        {
            (this->*AddTriangleFunc)(
                x + lodstep, z,
                x, z,
                x, z + lodstep
            );
        }
    }
    this->geometryStorage->UnlockIndicesGroup();
}

//------------------------------------------------------------------------------
/**
*/
template <nGMMIndexBuilder::AddTriangleType AddTriangleFunc>
void
nGMMIndexLOD1Builder::FillIndexTriangleListEast(bool crackN, bool crackS, int lodstep)
{
    int iniz = 0;
    int endz = (iniz + this->blockSize - 1);
    int dblstep = (lodstep << 1);
    int x = (this->blockSize - 1) - lodstep;

    if ((iniz + dblstep) > endz)
    {
        return;
    }

    this->geometryStorage->LockIndicesGroup((endz - iniz) * 9 / dblstep);
    for(int z = iniz; z < endz; z += dblstep)
    {
        (this->*AddTriangleFunc)(
            x + lodstep, z + dblstep,
            x + lodstep, z,
            x, z + lodstep
        );
        if ( !(crackS && (endz - dblstep) == z) )
        {
            (this->*AddTriangleFunc)(
                x + lodstep, z + dblstep,
                x, z + lodstep,
                x, z + dblstep
            );
        }
        if ( !(crackN && 0 == z) )
        {
            (this->*AddTriangleFunc)(
                x, z + lodstep,
                x + lodstep, z,
                x, z
            );
        }
    }
    this->geometryStorage->UnlockIndicesGroup();
}

//------------------------------------------------------------------------------
int 
nGMMIndexLOD1Builder::CountIndices(int lod)
{
    int nvertices = (this->blockSize - 1) >> lod;
    return (nvertices) * (nvertices) * 2 * 3;
}

//------------------------------------------------------------------------------
int 
nGMMIndexLOD1Builder::CountGroups(int lod)
{
    return (lod + 1) == this->numLODLevels ? 1 : 16;
}

//------------------------------------------------------------------------------
bool 
nGMMIndexLOD1Builder::HasCrack(int lod, int lodNeighbor)
{
    return (lod < lodNeighbor);
}

//------------------------------------------------------------------------------
void
nGMMIndexLOD1Builder::ParseIndexGroup(int group, bool & crackN, bool & crackE, bool & crackS, bool & crackW)
{
    crackN = ((group & 0x08) == 0x08);
    crackE = ((group & 0x04) == 0x04);
    crackS = ((group & 0x02) == 0x02);
    crackW = ((group & 0x01) == 0x01);
}

//------------------------------------------------------------------------------
void
nGMMIndexLOD1Builder::ParseIndexGroup(int lod, int group, int & lodN, int & lodE, int & lodS, int & lodW)
{
    bool crackN, crackE, crackS, crackW;

    ParseIndexGroup(group, crackN, crackE, crackS, crackW);

    lodN = lod + int(crackN);
    lodE = lod + int(crackE);
    lodS = lod + int(crackS);
    lodW = lod + int(crackW);
}

//------------------------------------------------------------------------------
int 
nGMMIndexLOD1Builder::CalcIndexGroup(int lod, int lodN, int lodE, int lodS, int lodW)
{
    if (this->CountGroups(lod) > 1)
    {
        bool crackN = this->HasCrack(lod, lodN);
        bool crackE = this->HasCrack(lod, lodE);
        bool crackS = this->HasCrack(lod, lodS);
        bool crackW = this->HasCrack(lod, lodW);

        return (int(crackN) << 3) | (int(crackE) << 2) | (int(crackS) << 1) | (int(crackW));
    }

    return 0;
}
