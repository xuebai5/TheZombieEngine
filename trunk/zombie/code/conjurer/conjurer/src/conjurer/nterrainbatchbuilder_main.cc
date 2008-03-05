#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nterrainbatchbuilder_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nterrainbatchbuilder.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialbatch.h"
#include "ngeomipmap/ncterraingmm.h"
#include "ngeomipmap/ncterraingmmcell.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nscene/ncscenelodclass.h"
#include "zombieentity/nctransform.h"
#include "ndebug/nceditorclass.h"
#include "ndebug/nceditor.h"

//------------------------------------------------------------------------------
/**
*/
nTerrainBatchBuilder::nTerrainBatchBuilder(nEntityObject* outdoor) :
    refOutdoorEntity(outdoor)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nTerrainBatchBuilder::~nTerrainBatchBuilder()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainBatchBuilder::SetSelectedCells(const nArray<nRefEntityObject> entities)
{
    this->selectedCells.Reset();
    int i;
    for (i = 0; i < entities.Size(); ++i)
    {
        if (entities[i] && entities[i]->GetComponent<ncTerrainGMMCell>())
        {
            this->selectedCells.Append(entities[i]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nTerrainBatchBuilder::Build()
{
    if (!this->refOutdoorEntity.isvalid())
    {
        return;
    }

    this->numBatchedEntities = 0;
    this->numBatchesBuilt = 0;

    // traverse all terrain blocks and get from them the list of static entities
    // (get their "static" class key) with no batch so far.
    // also, get entities from non-leaf terrain cells that happen to have their 
    // position within the current leaf cell
    // with all these, find a batch in that cell with the batchblock of the cell
    // or create a new one with all collected entities.
    ncTerrainGMM* terrainGMM = this->refOutdoorEntity->GetComponentSafe<ncTerrainGMM>();
    if (terrainGMM)
    {
        ncTerrainGMMClass* terrainClass = this->refOutdoorEntity->GetClassComponentSafe<ncTerrainGMMClass>();
        ncTerrainGMMCell* firstCell = terrainGMM->GetNorthWestCell();

        int numBlocks = terrainClass->GetNumBlocks();
        int bz;
        for (bz = 0; bz < numBlocks; bz++)
        {
            ncTerrainGMMCell *currentCell = firstCell;
            for (int bx = 0; bx < numBlocks; bx++)
            {
                n_assert(currentCell);
                if (this->selectedCells.Empty() || this->selectedCells.Find(currentCell->GetEntityObject()))
                {
                    this->BuildBatchForTerrainCell(currentCell);
                }

                currentCell = currentCell->GetNeighbor(ncTerrainGMMCell::East);
            }
            firstCell = firstCell->GetNeighbor(ncTerrainGMMCell::South);
        }
    }

    // show some statistics
    n_message("Automatic terrain batching process completed:\n\n"
              "%u entities batched.\n\n"
              "%u batches built.\n\n", this->numBatchedEntities, this->numBatchesBuilt);
}

//------------------------------------------------------------------------------
/**
    build the set of batch entities for a terrain block

    @todo ma.garcias - if some cell happens to have an empty batch (with a block key) remove it
    @todo ma.garcias - if some cell happens to not have any entities on it, skip it
*/
void
nTerrainBatchBuilder::BuildBatchForTerrainCell(ncTerrainGMMCell *terrainCell)
{
    // clear list of entities from previous batch
    this->Clear();

    ncSpatialQuadtreeCell *quadtreeCell = terrainCell->GetComponent<ncSpatialQuadtreeCell>();
    int blockId = quadtreeCell->GetId();

    //TODO- set batch id from lead quadtreeCell
    //collect entities for batching, from this cell and any other above
    //(whose position falls within this one)
    while (quadtreeCell)
    {
        const nArray<nEntityObject*> *categories = quadtreeCell->GetCategories();
        for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
        {
            const nArray<nEntityObject*> entities = categories[catIndex];
            for (int i = 0; i < entities.Size(); ++i)
            {
                ncSpatialBatch* batchComp = entities[i]->GetComponent<ncSpatialBatch>();
                if (batchComp)
                {
                    //if the entity pos is within the current leaf cell,
                    //look for for the "BatchBlockId" id in the editor component
                    ncEditor* editorComp = entities[i]->GetComponent<ncEditor>();
                    if (editorComp && editorComp->IsSetEditorKey("BatchBlockId"))
                    {
                        if (editorComp->GetEditorKeyInt("BatchBlockId") == blockId)
                        {
                            this->SetBatchEntity(entities[i]);
                        }
                    }
                }
                else
                {
                    //test if the entity falls within the current leaf cell
                    ncTransform* transformComp = entities[i]->GetComponent<ncTransform>();
                    if (transformComp)
                    {
                        ncSpatialQuadtree* quadtree = this->refOutdoorEntity->GetComponent<ncSpatialQuadtree>();
                        ncSpatialQuadtreeCell* leafCell = quadtree->SearchLeafCell(transformComp->GetPosition());
                        if (leafCell && (leafCell->GetEntityObject() == terrainCell->GetEntityObject()))
                        {
                            //check the scene component
                            ncScene *sceneComp = entities[i]->GetComponent<ncScene>();
                            if (sceneComp)
                            {
                                ncEditorClass* editorComp = entities[i]->GetClassComponent<ncEditorClass>();
                                if (editorComp && editorComp->IsSetClassKey("BatchStatic"))
                                {
                                    ncSceneLodClass* lodClass = entities[i]->GetClassComponent<ncSceneLodClass>();
                                    int level = (lodClass && lodClass->GetNumLevels() > 0) ? lodClass->GetNumLevels() - 1 : 0;
                                    this->AddEntity(entities[i], level);
                                }
                            }
                        }
                    }
                }
            }
        }
        quadtreeCell = quadtreeCell->GetParentCell();
    }

    //has the batch for this cell been found? if so, set as current for rebuild
    //TODO- set an editor key for all entities to know to which automatic batch they belong?
    if (this->GetNumEntities() > 0 || this->GetBuildSceneResource())
    {
        //TODO- if the batch already exists, load subentities, add the new ones, and rebuild
        //TODO- if there is only one entity to batch, skip (including already batched ones)
        if (nEntityBatchBuilder::Build())
        {
            this->numBatchedEntities += this->GetNumEntities();
            ++this->numBatchesBuilt;

            nEntityObject* batchEntity = this->GetBatchEntity();
            n_assert(batchEntity);
            //set block id as editor key to identify this batch in case when rebuilding
            batchEntity->GetComponentSafe<ncEditor>()->SetEditorKeyInt("BatchBlockId", blockId);
        }
    }
}
