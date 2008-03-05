#ifndef N_GMMDYNAMICMESHSTORAGE_H
#define N_GMMDYNAMICMESHSTORAGE_H
//------------------------------------------------------------------------------
/**
    @class nGMMMeshStorage
    @ingroup NebulaTerrain
    @author Mateu Batle

    @brief Used by heightmap mesh builder to store geometry info in nDynamicMesh.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "ngeomipmap/ngmmgeometrystorage.h"
#include "gfx2/ndynamicmesh.h"

//------------------------------------------------------------------------------
class nGMMDynamicMeshStorage : public nGMMGeometryStorage
{
public:
    /// constructor
    nGMMDynamicMeshStorage();
    /// destructor
    virtual ~nGMMDynamicMeshStorage();

    /// set dynamic mesh
    void SetDynamicMesh(nDynamicMesh * dynMesh);

    /// Begin terrain mesh usage (before starting terrain mesh groups)
    virtual void BeginTerrainMesh();
    /// End terrain mesh usage (after finishing all groups)
    virtual void EndTerrainMesh();

    /// Begin terrain mesh building process (begins new group)
    virtual void BeginTerrainGroup(int numVertices, int numIndices);
    /// End terrain mesh building process, (ends new group)
    virtual int EndTerrainGroup();

private:

    nDynamicMesh * dynMesh;

};

//------------------------------------------------------------------------------
#endif //N_GMMDYNAMICMESHSTORAGE_H