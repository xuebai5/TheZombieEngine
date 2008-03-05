#ifndef N_GMMVERTEXBUILDER_H
#define N_GMMVERTEXBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nGMMVertexBuilder
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Builder of vertex buffers for the geomipmap terrains.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nref.h"
#include "ngeomipmap/nfloatmap.h"

//------------------------------------------------------------------------------
class ncTerrainGMMClass;
class nGMMGeometryStorage;
class nGMMVertexLODSorting;

//------------------------------------------------------------------------------
class nGMMVertexBuilder
{
public:
    /// constructor
    nGMMVertexBuilder();
    /// destructor
    ~nGMMVertexBuilder();

    /// setup the terrain where to get information from
    virtual void SetupTerrain(ncTerrainGMMClass * tgmmc);
    /// setup the storage where to store vertices
    void SetupStorage(nGMMGeometryStorage * geom);
    /// setup the vertex sorting
    void SetupVertexSorting(nGMMVertexLODSorting * vertexSorting);

    /// fill all vertices for a given block
    virtual void FillVerticesBlock(int bx, int bz, int lod);

    /// count vertices needed for a given lod
    int CountVertices(int lod) const;

protected:

    /// add the vertex for position ix, iz
    void AddVertex(int index, int ix, int iz);

    /// terrain heightmap
    nRef<nFloatMap> refHeightMap;
    /// scale used in the heightmap
    float scale;
    /// number of vertices per row
    int numVerticesPerRow;
    /// number of lod levels
    int numLODLevels;

    /// geometry storage
    nGMMGeometryStorage * geometryStorage;

    /// vertex sorting
    nGMMVertexLODSorting * vertexSorting;

    /// terrain geomipmap class
    ncTerrainGMMClass * tgmmc;

};

//------------------------------------------------------------------------------
#endif // N_GMMVERTEXBUILDER_H
