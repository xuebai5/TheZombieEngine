#ifndef N_TERRAINBATCHBUILDER_H
#define N_TERRAINBATCHBUILDER_H
//------------------------------------------------------------------------------
/**
    @class nTerrainBatchBuilder
    @ingroup Conjurer
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Batches all free, static entities in an outdoor space,
    into a scene resource.

    (C) 2005 Conjurer Services, S.A.
*/
#include "conjurer/nentitybatchbuilder.h"

class ncTerrainGMMCell;
//------------------------------------------------------------------------------
class nTerrainBatchBuilder : public nEntityBatchBuilder
{
public:
    /// constructor
    nTerrainBatchBuilder(nEntityObject*);
    /// destructor
    ~nTerrainBatchBuilder();

    /// build the set of batch entities for all blocks
    void Build();

    /// set the range of terrain blocks for which to generate the terrain
    void SetSelectedCells(const nArray<nRefEntityObject> entities);

private:

    /// build the set of batch entities for a terrain block
    void BuildBatchForTerrainCell(ncTerrainGMMCell* terrainCell);

    nRef<nEntityObject> refOutdoorEntity;

    /// range of terrain cells
    nArray<nEntityObject*> selectedCells;

    int numBatchedEntities;
    int numBatchesBuilt;
};

//------------------------------------------------------------------------------
#endif /*N_TERRAINBATCHBUILDER_H*/
