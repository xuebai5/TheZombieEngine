//------------------------------------------------------------------------------
//  ngmmindexlodnbuilder.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmindexlodnbuilder.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "ngeomipmap/ngmmvertexlodsorting.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
*/
void
nGMMIndexLODNBuilder::FillIndicesBlock(int lod, int lodN, int lodE, int lodS, int lodW)
{
    n_assert(lod >= 0 && lod < this->numLODLevels);
    n_assert(lodN >= 0 && lod < this->numLODLevels);
    n_assert(lodS >= 0 && lod < this->numLODLevels);
    n_assert(lodE >= 0 && lod < this->numLODLevels);
    n_assert(lodW >= 0 && lod < this->numLODLevels);

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
    bool crackN = (lodstep > lodstepN);
    bool crackE = (lodstep > lodstepE);
    bool crackS = (lodstep > lodstepS);
    bool crackW = (lodstep > lodstepW);

    // adjust the initial positions of the center block depending on neighbor lod
    if (crackN)
    {
        iniz += lodstep;
    }
    if (crackE)
    {
        endx -= lodstep;
    }
    if (crackS)
    {
        endz -= lodstep;
    }
    if (crackW)
    {
        inix += lodstep;
    }
    // get and lock index buffer
    this->geometryStorage->LockIndicesGroup( max(0, 6 * (endx - inix) * (endz - iniz) / (lodstep * lodstep)) );

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
                this->AddTriangle(
                    x, z,
                    x, z + lodstep,
                    x + lodstep, z);

                this->AddTriangle(
                    x, z + lodstep,
                    x + lodstep, z + lodstep,
                    x + lodstep, z);
            }
        } 
        else 
        {
            // triangle lists (right to left)
            for (int x = endx; x > inix; x -= lodstep)
            {
                // ordering as / / / / 
                this->AddTriangle(
                    x, z,
                    x - lodstep, z + lodstep,
                    x, z + lodstep);

                this->AddTriangle(
                    x, z,
                    x - lodstep, z,
                    x - lodstep, z + lodstep);
            }
        }
        upwards = !upwards;
    }
    this->geometryStorage->UnlockIndicesGroup();

    // generate triangles to fix the cracks in each direction
    if (crackN)
    {
        FillIndicesNorth(lod, lodN, lodE, lodS, lodW);
    }
    if (crackE)
    {
        FillIndicesEast(lod, lodN, lodE, lodS, lodW);
    }
    if (crackS)
    {
        FillIndicesSouth(lod, lodN, lodE, lodS, lodW);
    }
    if (crackW)
    {
        FillIndicesWest(lod, lodN, lodE, lodS, lodW);
    }
}

//------------------------------------------------------------------------------
int 
nGMMIndexLODNBuilder::CountIndices(int lod)
{
    // calculate number of verices
    int numVertices = 1 << (numLODLevels - lod);
    // calculate number of triangles for current LOD
    int numTriangles = numVertices * numVertices * 2;
    // add enough triangles to connect with higher LOD 
    numTriangles += 4 * 3 * (1 << (lod));
    // calculate the number of different variations
    //int numVariations = (lod + 1) * (lod + 1) * (lod + 1) * (lod + 1);
    // calculate the number of indices
    //return numVariations * numTriangles * 3;
    return numTriangles * 3;
}

//------------------------------------------------------------------------------
int 
nGMMIndexLODNBuilder::CountGroups(int lod)
{
    n_assert(lod >= 0 && lod <= this->numLODLevels);

    // calculate the group
    int group = (lod + 1) * (lod + 1) * (lod + 1) * (lod + 1);

    return group;
}

//------------------------------------------------------------------------------
void
nGMMIndexLODNBuilder::ParseIndexGroup(int lod, int group, int & lodN, int & lodE, int & lodS, int & lodW)
{
    n_assert(lod >= 0 && lod <= this->numLODLevels);

    int lod1 = lod + 1;
    int val = group;
    lodN    = val / (lod1 * lod1 * lod1);
    val     = val % (lod1 * lod1 * lod1);
    lodE    = val / (lod1 * lod1);
    val     = val % (lod1 * lod1);
    lodS    = val / (lod1);
    lodW    = val % (lod1);

    n_assert(lodN >= 0 && lod <= this->numLODLevels);
    n_assert(lodE >= 0 && lod <= this->numLODLevels);
    n_assert(lodS >= 0 && lod <= this->numLODLevels);
    n_assert(lodW >= 0 && lod <= this->numLODLevels);
    n_assert(group == this->CalcIndexGroup(lod, lodN, lodE, lodS, lodW));
}

//------------------------------------------------------------------------------
int 
nGMMIndexLODNBuilder::CalcIndexGroup(int lod, int lodN, int lodE, int lodS, int lodW)
{
    n_assert(lod >= 0 && lod <= this->numLODLevels);
    n_assert(lodN >= 0 && lod <= this->numLODLevels);
    n_assert(lodE >= 0 && lod <= this->numLODLevels);
    n_assert(lodS >= 0 && lod <= this->numLODLevels);
    n_assert(lodW >= 0 && lod <= this->numLODLevels);

    // adjust the LOD to take into account in relation with the cell LOD
    lodN = min(lod, lodN);
    lodE = min(lod, lodE);
    lodS = min(lod, lodS);
    lodW = min(lod, lodW);

    // calculate the group
    int lod1 = lod + 1;
    int group = 0;
    group += lodN * (lod1) * (lod1) * (lod1);
    group += lodE * (lod1) * (lod1);
    group += lodS * (lod1);
    group += lodW;

    return group;
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMIndexLODNBuilder::FillIndicesNorth(
    int lod, int lodN, int lodE, int lodS, int lodW)
{
    // calculate steps
    int step = (1 << lod);
    int stepN = (1 << lodN);
    int stepW = (1 << lodW);
    int stepE = (1 << lodE);
    int max = this->blockSize - 1;

    // 3 indices per triangle * 2 triangles per quad * max. as many quads as levels
    this->geometryStorage->LockIndicesGroup( 3 * 2 * (1 << (this->numLODLevels)) );

    // first part, colliding west cell, goal is connect all edges from N
    // the first triangle collides with both edges N and W
    int z = stepW;
    int xn = 0;
    while(xn < step)
    {
        this->AddTriangle(
            0, z,
            xn + stepN, 0,
            xn, 0);

        xn += stepN;
        z = min(z + stepW,step);
    }

    // intermediate parts (not colliding west or east cell)
    while(xn < max)
    {
        // extra triangle for first part not colliding neither W nor N
        if (xn <= (max - step))
        {
            this->AddTriangle(
                xn - step, step,
                xn, step,
                xn, 0);
        }

        // generate one triangle for each N edge
        int x = 0;
        while(x < step)
        {
            this->AddTriangle(
                xn + x + stepN, 0,
                xn + x, 0,
                xn, step);

            x += stepN;
        }

        xn += step;
    }

    // if west band crack-fix band is not generated, we must add some triangles
    // for the northest part of the west edge 
    if (this->HasNotCrackFixBand(lod, lodW, lodN))
    {
        // generate one triangle for each W edge
        int x = stepN;
        int zw = stepW;
        while(zw < step)
        {
            this->AddTriangle(
                0, zw,
                0, zw + stepW,
                x, 0);

            x = min(x + stepN, step);
            zw += stepW;
        }
    }

    // if east band crack-fix band is not done, we must add some triangles
    // for the northest part of the east edge
    if (this->HasNotCrackFixBand(lod, lodE, lodS))
    {
        // generate one triangle for each E edge
        int x0 = max - step;
        int x1 = max;
        int z = 0;
        while(z < step)
        {
            this->AddTriangle(
                x0, step,
                x1, z + stepE,
                x1, z);

            z += stepE;
        }
    }

    this->geometryStorage->UnlockIndicesGroup();
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMIndexLODNBuilder::FillIndicesWest(
    int lod, int lodN, int lodE, int lodS, int lodW)
{
    // calculate steps
    int step = (1 << lod);
    int stepN = (1 << lodN);    
    int stepW = (1 << lodW);
    int stepS = (1 << lodS);
    int max = this->blockSize - 1;

    // 3 indices per triangle * 2 triangles per quad * max. as many quads as levels
    this->geometryStorage->LockIndicesGroup( 3 * 2 * (1 << (this->numLODLevels)) );

    // first part, colliding north cell, goal is connect all edges from W
    // we generate only one triangle with common edge W and N (upper left)
    int x = min(stepN, step);
    int zw = 0;
    while(zw < step)
    {
        this->AddTriangle(
            0, zw,
            0, zw + stepW,
            x, 0);

        if (zw != 0)
        {
            x = min(x + stepN, step);
        }
        zw += stepW;
    }

    // intermediate parts (not colliding north or south cell)
    while(zw < max)
    {
        // extra triangle for first part not colliding neither W nor N
        this->AddTriangle(
            0, zw,
            step, zw,
            step, zw - step);

        // generate one triangle for each W edge
        int z = 0;
        while(z < step)
        {
            this->AddTriangle(
                0, zw + z,
                0, zw + z + stepW,
                step, zw);

            z += stepW;
        }

        zw += step;
    }

    // if north band crack-fix band is not generated, we must add some triangles
    // for the west-most part of the north edge 
    if (this->HasNotCrackFixBand(lod, lodN, lodW))
    {
        int z = stepW;
        int xn = 0;
        while(xn < step)
        {
            this->AddTriangle(
                0, z,
                xn + stepN, 0,
                xn, 0);

            xn += stepN;
            z = min(z + stepW, step);
        }
    }

    // if south band crack-fix band is not generated, we must add some triangles
    // for the west-most part of the south edge 
    if (this->HasNotCrackFixBand(lod, lodS, lodE))
    {
        int x = 0;
        while(x < step)
        {
            this->AddTriangle(
                x, max,
                x + stepS, max,
                step, max - step);

            x += stepS;
        }
    }

    this->geometryStorage->UnlockIndicesGroup();
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMIndexLODNBuilder::FillIndicesEast(
    int lod, int lodN, int lodE, int lodS, int lodW)
{
    // calculate steps
    int step = (1 << lod);
    int stepN = (1 << lodN);
    int stepE = (1 << lodE);
    int stepS = (1 << lodS);
    int max = this->blockSize - 1;

    // 3 indices per triangle * 2 triangles per quad * max. as many quads as levels
    this->geometryStorage->LockIndicesGroup( 3 * 2 * (1 << (this->numLODLevels)) );

    // intermediate parts (not colliding neither north nor south cell)
    int x0 = max - step;
    int ze = 0;
    while(ze < (max - step))
    {
        // generate one triangle for each E edge
        int z = 0;
        while(z < step)
        {
            this->AddTriangle(
                x0, ze + step,
                max, ze + z + stepE,
                max, ze + z);

            z += stepE;
        }

        ze += step;

        // extra triangle for part not colliding neither external E, N, S edges
        this->AddTriangle(
            x0, ze + step,
            max, ze,
            x0, ze);
    }

    // last part, colliding south cell, goal is connect all edges from E
    // we generate only one triangle with common edge E and S (bottom left)
    int x = max(max - stepS, max - step);
    ze = max;
    while(ze > (max - step))
    {
        this->AddTriangle(
            max, ze,
            max, ze - stepE,
            x, max);

        x = max(x - stepS, max - step);
        ze -= stepE;
    }

    // if north band crack-fix band is not generated, we must add some triangles
    // for the east-most part of the north edge 
    if (this->HasNotCrackFixBand(lod, lodN, lodW))
    {
        // generate one triangle for each N edge
        int x = 0;
        while(x < step)
        {
            this->AddTriangle(
                x0 + x + stepN, 0,
                x0 + x, 0,
                x0, step);

            x += stepN;
        }
    }

    // if south band crack-fix band is not generated, we must add some triangles
    // for the east-most part of the south edge 
    if (this->HasNotCrackFixBand(lod, lodS, lodE))
    {
        int z = max - stepE;
        int xs = max;
        while(xs > max - step)
        {
            this->AddTriangle(
                xs - stepS, max,
                xs, max,
                max, z);

            xs -= stepS;
            z = max(z - stepE, max - step);
        }
    }

    this->geometryStorage->UnlockIndicesGroup();
}

//------------------------------------------------------------------------------
/**
*/
void
nGMMIndexLODNBuilder::FillIndicesSouth(
    int lod, int lodN, int lodE, int lodS, int lodW)
{
    // calculate steps
    int step = (1 << lod);
    int stepE = (1 << lodE);
    int stepW = (1 << lodW);
    int stepS = (1 << lodS);
    int max = this->blockSize - 1;

    // 3 indices per triangle * 2 triangles per quad * max. as many quads as levels
    this->geometryStorage->LockIndicesGroup( 3 * 2 * (1 << (this->numLODLevels)) );

    // intermediate parts (not colliding neither west nor east cells)
    int xs = 0;
    while(xs < max - step)
    {
        // generate one triangle for each S edge
        int x = 0;
        while(x < step)
        {
            this->AddTriangle(
                xs + x, max,
                xs + x + stepS, max,
                xs + step, max - step);

            x += stepS;
        }

        xs += step;

        // extra triangle for part not colliding neither external cells
        this->AddTriangle(
            xs + step, max - step,
            xs, max - step,
            xs, max);
    }

    // last part, colliding east cell, goal is connect all edges from S
    // we generate only one triangle with common edge E and S (bottom right)
    int z = max - stepE;
    xs = max;
    while(xs > max - step)
    {
        this->AddTriangle(
            xs - stepS, max,
            xs, max,
            max, z);

        if (xs != max)
        {
            z = max(z - stepE, max - step);
        }
        xs -= stepS;
    }

    // if west band crack-fix band is not generated, we must add some triangles
    // for the west-most part of the south edge 
    if (this->HasNotCrackFixBand(lod, lodW, lodN))
    {
        // generate one triangle for each W edge
        int zw = max - stepW;
        int z = 0;
        while(z < step)
        {
            this->AddTriangle(
                0, zw + z,
                0, zw + z + stepW,
                step, zw);

            z += stepW;
        }
    }

    // if west band crack-fix band is not generated, we must add some triangles
    // for the west-most part of the south edge 
    if (this->HasNotCrackFixBand(lod, lodE, lodS))
    {
        // last part, colliding south cell, goal is connect all edges from E
        // we generate only one triangle with common edge E and S (bottom left)
        int x = max;
        int ze = max;
        while(ze > max - step)
        {
            this->AddTriangle(
                max, ze,
                max, ze - stepE,
                x, max);

            x = max(x - stepS, max - step);
            ze -= stepE;
        }
    }

    this->geometryStorage->UnlockIndicesGroup();
}

//------------------------------------------------------------------------------
/**
*/
bool
nGMMIndexLODNBuilder::HasNotCrackFixBand(int lod, int lodSide1, int lodSide2)
{
    //return (lod < lodSide || (lod == lodSide && lod < (this->numLODLevels - 1)));
    return (lod < lodSide1) || (lod == lodSide1 && (lod < (this->numLODLevels - 1) || lod <= lodSide2));
    //return (lod <= lodSide1);
}
