#ifndef N_NCTERRAINGMM_H
#define N_NCTERRAINGMM_H
//------------------------------------------------------------------------------
/**
    @file ncterraingmm.h
    @class ncTerrainGMM
    @ingroup NebulaTerrain
    @author Mateu Batle Sastre
    @brief Object component class for terrain graphics using geomipmap algorithm.    

    (c) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "util/nstring.h"
#include "kernel/nref.h"
#include "kernel/ncmdprotonativecpp.h"
#include "ngeomipmap/nfloatmap.h"

//------------------------------------------------------------------------------
class nFloatMap;
class nPersistServer;
class neOutdoorCell;
class nTerrainCellInfo;
class ncTerrainGMMCell;
class ncSpatialQuadtreeCell;
class ncSpatialQuadtree;

//------------------------------------------------------------------------------
class ncTerrainGMM : public nComponentObject
{

    NCOMPONENT_DECLARE(ncTerrainGMM, nComponentObject);

    friend class ncOutdoorBuilder;

public:
    /// constructor
    ncTerrainGMM();
    /// destructor
    ~ncTerrainGMM();

    /// initialize component pointers from entity object
    void InitInstance(nObject::InitInstanceMsg initType);

    // get north west cell
    ncTerrainGMMCell * GetNorthWestCell() const;

    /// disable unpainted cells (make the invisible, not handled by visibilty nor rendered)
    void SetUnpaintedCellsInvisible(bool enable);
    /// get current state about if unpainted cells are invisible or not
    bool GetUnpaintedCellsInvisible();

    /// return the terrain cell for the block (bx, bz)
    ncTerrainGMMCell * GetTerrainCell(int bx, int bz);

    /// to be called when the heightmap has been updated, the updated area is specified
    void UpdateGeometry(int x0, int z0, int x1, int z1);

protected:

    /// get a cell
    ncTerrainGMMCell* GetLeafCell(nArray<ncSpatialQuadtreeCell*> * cells, int bx, int bz) const;
    /// Initialize the leaf cells
    void InitLeafCells(ncSpatialQuadtree * qt);
    /// Initialize the inner cells from the quadtree
    void InitInnerCells(ncSpatialQuadtree * qt);
    /// Initialize the inner cells from the root cell down to set parent / child relations
    void InitInnerCellsParents(ncSpatialQuadtreeCell * root);
    /// Initialize the inner cells from the root cell down to set neighbor relations (N,W,E,S)
    void InitInnerCellsNeighbors(ncSpatialQuadtreeCell * root);
    /// Init LOD params
    void InitLODParams(ncSpatialQuadtree * qt);

    bool InitTerrainCell(nEntityObject *entityObject);

    /// signals when we are in attach state 
    bool attaching;

    /// statistics for LOD leveling
    int blocksLeveled;
    int lodsLeveled;
    int trianglesLeveled;

    /// number of blocks in one direction
    int numBlocks;
    // pointer to north west (top left) cell entity object
    ncTerrainGMMCell * cellNW;

    /// state to know if unpainted cells are invisible or not
    bool unpaintedCellsInvisible;

    /// array of terrain cell info structures

};

//---------------------------------------------------------------------------
#endif // N_NCTERRAINGMM_H
