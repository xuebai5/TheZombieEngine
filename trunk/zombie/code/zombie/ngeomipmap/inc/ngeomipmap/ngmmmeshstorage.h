#ifndef N_GMMMESHSTORAGE_H
#define N_GMMMESHSTORAGE_H
//------------------------------------------------------------------------------
/**
    @class nGMMMeshStorage
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Used by heightmap mesh builder to store geometry info in nMesh2.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "gfx2/nmesh2.h"

//------------------------------------------------------------------------------
class nGMMMeshStorage : public nGMMGeometryStorage
{
public:
    /// constructor
    nGMMMeshStorage();
    /// destructor
    virtual ~nGMMMeshStorage();

    /// setup vertex buffers
    void SetupMesh(nMesh2 * mesh);

    /// Begin terrain mesh usage (before starting terrain mesh groups)
    virtual void BeginTerrainMesh();
    /// End terrain mesh usage (after finishing all groups)
    virtual void EndTerrainMesh();

    /// Begin terrain mesh building process (begins new group)
    virtual void BeginTerrainGroup(int numVertices, int numIndices);
    /// End terrain mesh building process, (ends new group)
    virtual int EndTerrainGroup();

private:

    /// reference to the mesh where to store data
    nRef<nMesh2> refMesh;

    /// current group when using nMesh
    int group;
};

//------------------------------------------------------------------------------
#endif // N_GMMMESHSTORAGE_H
