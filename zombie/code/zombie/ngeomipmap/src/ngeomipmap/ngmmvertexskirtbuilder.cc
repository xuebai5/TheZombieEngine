//------------------------------------------------------------------------------
//  ngmmvertexskirtbuilder.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchngeomipmap.h"
#include "ngeomipmap/ngmmvertexskirtbuilder.h"
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "ngeomipmap/ngmmvertexskirtlodsorting.h"
#include "nspatial/nchorizonclass.h"
#include "nspatial/nhorizonsegment.h"

//------------------------------------------------------------------------------
/**
    Setup of vertex builder is a bit different, it's like if we had one 
    row/column more of vertices for each skirt enclosing the terrain cell block.
    For example, if we have cells of 5x5 vertices, we would have 7x7 vertices
    with skirts.

    Original:

    +-+-+-+-+
    +-+-+-+-+
    +-+-+-+-+
    +-+-+-+-+
    +-+-+-+-+

    Version with skirts

    x-#-#-#-#-#-x
    #-+-+-+-+-+-#
    #-+-+-+-+-+-#
    #-+-+-+-+-+-#
    #-+-+-+-+-+-#
    #-+-+-+-+-+-#
    x-#-#-#-#-#-x

    legend:

    + - vertices of the terrain cell, init from heightmap
    # - vertices for the skirt, init from horizon info
    x - vertices not really used, but we leave them to have easier indexing
    - - just connecting vertices horizontally, no real meaning

*/
void 
nGMMVertexSkirtBuilder::SetupTerrain(ncTerrainGMMClass * tgmmc)
{
    nGMMVertexBuilder::SetupTerrain(tgmmc);
    if (tgmmc) 
    {
        // there is 1 vertex more in each side for the skirt (+2 total)
        this->numVerticesPerRow += 2;
    }
}

//------------------------------------------------------------------------------
/**
    Fills the vertex buffer with proper (x,y,z) for a given block (bx, bz).
    @param bx block x coordinate 
    @param bz block z coordinate
    @param lod lod level used in the block
*/
void
nGMMVertexSkirtBuilder::FillVerticesBlock(int bx, int bz, int lod)
{
    NLOG(terrain, (nTerrainLog::NLOG_BUILDVERTEX | 0, "nGMMVertexSkirtBuilder::FillVerticesBlock(%d,%d,%d)", bx, bz, lod));

    int bs = this->numVerticesPerRow - 2;
    int step = (1 << lod);

    // calculate heightmap coordinates (x, z) of top-left corner
    int x = bx * (bs - 1);
    int z = bz * (bs - 1);

    this->geometryStorage->LockVerticesGroup( this->CountVertices(lod) );

    // fill the vertices of the inner block of the terrain cell
    for( int iz = 0; iz < bs; iz += step )
    {
        for( int ix = 0; ix < bs; ix += step )
        {
            // we add one due to the initial skirt vertex row/column
            int index = this->vertexSorting->GetVertexIndex(ix + 1, iz + 1);
            this->AddVertex(index, x + ix, z + iz);
        }
    }

    // fill the skirt vertices (the four edges)
    this->FillSkirtVertices(bx, bz, lod);

    this->geometryStorage->UnlockVerticesGroup();
}

//------------------------------------------------------------------------------
/**    
*/
void
nGMMVertexSkirtBuilder::FillSkirtVertices(int bx, int bz, int lod)
{
    NLOG(terrain, (nTerrainLog::NLOG_BUILDVERTEX | 0, "nGMMVertexSkirtBuilder::FillSkirtVertices(%d,%d,%d)", bx, bz, lod));

    bool useHorizonInfo = true;

    // check if possible to use horizon information
    ncHorizonClass * horClass = this->tgmmc->GetComponent<ncHorizonClass>();
    line3 lineNorth;
    line3 lineSouth;
    line3 lineWest;
    line3 lineEast;
    if (!horClass || horClass->GetNumSegmentsX() <= 0)
    {
        useHorizonInfo = false;
    }

    // extract horizon info: the segments for each edge of the skirt
    if (useHorizonInfo)
    {
        nHorizonSegment * segmentNorth = horClass->GetHorizonSegment(bx, bz, true);
        if (segmentNorth)
        {
            lineNorth.set(segmentNorth->GetStart(), segmentNorth->GetEnd());
        }
        else
        {
            useHorizonInfo = false;
        }

        nHorizonSegment * segmentSouth = horClass->GetHorizonSegment(bx, bz + 1, true);
        if (segmentSouth)
        {
            lineSouth.set(segmentSouth->GetStart(), segmentSouth->GetEnd());
        }
        else
        {
            useHorizonInfo = false;
        }

        nHorizonSegment * segmentWest = horClass->GetHorizonSegment(bx, bz, false);
        if (segmentWest)
        {
            lineWest.set(segmentWest->GetStart(), segmentWest->GetEnd());
        }
        else
        {
            useHorizonInfo = false;
        }

        nHorizonSegment * segmentEast = horClass->GetHorizonSegment(bx + 1, bz, false);
        if (segmentEast)
        {
            lineEast.set(segmentEast->GetStart(), segmentEast->GetEnd());
        }
        else
        {
            useHorizonInfo = false;
        }
    }

    /// generate
    int bs = this->numVerticesPerRow - 2;
    int step = (1 << lod);
    if (useHorizonInfo)
    {
        for(int i = 1;i < this->numVerticesPerRow - 1;i += step)
        {
            int index;
            vector3 * v;
            float t = float(i-1) / float(bs - 1);

            index = this->vertexSorting->GetVertexIndex(i, 0);
            v = this->geometryStorage->GetVertex( index );
            *v = lineNorth.point( t );
            NLOG(terrain, (nTerrainLog::NLOG_BUILDVERTEX | 1, "vertexNorth(%3.3f,%d) (%3.3f,%3.3f,%3.3f)", t, index, v->x, v->y, v->z));

            index = this->vertexSorting->GetVertexIndex(i, this->numVerticesPerRow - 1);
            v = this->geometryStorage->GetVertex( index );
            *v = lineSouth.point( t );
            NLOG(terrain, (nTerrainLog::NLOG_BUILDVERTEX | 1, "vertexSouth(%3.3f,%d) (%3.3f,%3.3f,%3.3f)", t, index, v->x, v->y, v->z));

            index = this->vertexSorting->GetVertexIndex(0, i);
            v = this->geometryStorage->GetVertex( index );
            *v = lineWest.point( t );
            NLOG(terrain, (nTerrainLog::NLOG_BUILDVERTEX | 1, "vertexWest(%3.3f,%d) (%3.3f,%3.3f,%3.3f)", t, index, v->x, v->y, v->z));

            index = this->vertexSorting->GetVertexIndex(this->numVerticesPerRow - 1, i);
            v = this->geometryStorage->GetVertex( index );
            *v = lineEast.point( t );
            NLOG(terrain, (nTerrainLog::NLOG_BUILDVERTEX | 1, "vertexEast(%3.3f,%d) (%3.3f,%3.3f,%3.3f)", t, index, v->x, v->y, v->z));
        }
    }
    else
    {
        // as the horizon information does not exist, we fill the skirt vertices 
        // with the nearest vertex from the heightmap data
        int isrc;
        int idst;
        vector3 * vsrc;
        vector3 * vdst;
        for(int i = 1;i < this->numVerticesPerRow - 1;i += step)
        {
            isrc = this->vertexSorting->GetVertexIndex(i, 1);
            vsrc = this->geometryStorage->GetVertex(isrc);
            idst = this->vertexSorting->GetVertexIndex(i, 0);
            vdst = this->geometryStorage->GetVertex(idst);
            *vdst = *vsrc;

            isrc = this->vertexSorting->GetVertexIndex(1, i);
            vsrc = this->geometryStorage->GetVertex(isrc);
            idst = this->vertexSorting->GetVertexIndex(0, i);
            vdst = this->geometryStorage->GetVertex(idst);
            *vdst = *vsrc;

            isrc = this->vertexSorting->GetVertexIndex(i, this->numVerticesPerRow - 2);
            vsrc = this->geometryStorage->GetVertex(isrc);
            idst = this->vertexSorting->GetVertexIndex(i, this->numVerticesPerRow - 1);
            vdst = this->geometryStorage->GetVertex(idst);
            *vdst = *vsrc;

            isrc = this->vertexSorting->GetVertexIndex(this->numVerticesPerRow - 2, i);
            vsrc = this->geometryStorage->GetVertex(isrc);
            idst = this->vertexSorting->GetVertexIndex(this->numVerticesPerRow - 1, i);
            vdst = this->geometryStorage->GetVertex(idst);
            *vdst = *vsrc;
        }
    }

    // fill the four dummy vertices (not really used) but needed to fill for lightmaps
    // (normals not needed by lightmaps, so they are not calculated)
    // only fill these vertices for LOD = 0
    if (!lod)
    {
        vector3 * vsrc;
        vector3 * vdst;
        int isrc;
        int idst;

        isrc = this->vertexSorting->GetVertexIndex(1, 1);
        vsrc = this->geometryStorage->GetVertex(isrc);
        idst = this->vertexSorting->GetVertexIndex(0, 0);
        vdst = this->geometryStorage->GetVertex(idst);
        *vdst = *vsrc;

        isrc = this->vertexSorting->GetVertexIndex(1, this->numVerticesPerRow - 2);
        vsrc = this->geometryStorage->GetVertex(isrc);
        idst = this->vertexSorting->GetVertexIndex(0, this->numVerticesPerRow - 1);
        vdst = this->geometryStorage->GetVertex(idst);
        *vdst = *vsrc;

        isrc = this->vertexSorting->GetVertexIndex(this->numVerticesPerRow - 2, 1);
        vsrc = this->geometryStorage->GetVertex(isrc);
        idst = this->vertexSorting->GetVertexIndex(this->numVerticesPerRow - 1, 0);
        vdst = this->geometryStorage->GetVertex(idst);
        *vdst = *vsrc;

        isrc = this->vertexSorting->GetVertexIndex(this->numVerticesPerRow - 2, this->numVerticesPerRow - 2);
        vsrc = this->geometryStorage->GetVertex(isrc);
        idst = this->vertexSorting->GetVertexIndex(this->numVerticesPerRow - 1, this->numVerticesPerRow - 1);
        vdst = this->geometryStorage->GetVertex(idst);
        *vdst = *vsrc;
    }

    // copy normals already calculated
    if (this->geometryStorage->GetFillNormals())
    {
        int isrc;
        int idst;
        vector3 * vsrc;
        vector3 * vdst;
        for(int i = 1;i < this->numVerticesPerRow - 1;i += step)
        {
            isrc = this->vertexSorting->GetVertexIndex(i, 1);
            vsrc = this->geometryStorage->GetVertex(isrc);
            idst = this->vertexSorting->GetVertexIndex(i, 0);
            vdst = this->geometryStorage->GetVertex(idst);
            *(vdst + 1) = *(vsrc + 1);

            isrc = this->vertexSorting->GetVertexIndex(1, i);
            vsrc = this->geometryStorage->GetVertex(isrc);
            idst = this->vertexSorting->GetVertexIndex(0, i);
            vdst = this->geometryStorage->GetVertex(idst);
            *(vdst + 1) = *(vsrc + 1);

            isrc = this->vertexSorting->GetVertexIndex(i, this->numVerticesPerRow - 2);
            vsrc = this->geometryStorage->GetVertex(isrc);
            idst = this->vertexSorting->GetVertexIndex(i, this->numVerticesPerRow - 1);
            vdst = this->geometryStorage->GetVertex(idst);
            *(vdst + 1) = *(vsrc + 1);

            isrc = this->vertexSorting->GetVertexIndex(this->numVerticesPerRow - 2, i);
            vsrc = this->geometryStorage->GetVertex(isrc);
            idst = this->vertexSorting->GetVertexIndex(this->numVerticesPerRow - 1, i);
            vdst = this->geometryStorage->GetVertex(idst);
            *(vdst + 1) = *(vsrc + 1);
        }
    }
}
