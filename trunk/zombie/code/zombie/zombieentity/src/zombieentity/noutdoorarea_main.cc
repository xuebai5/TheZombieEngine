#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  noutdoorarea_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/noutdoorarea.h"
#include "zombieentity/nloaderserver.h"

#include "nspatial/ncspatialspace.h"
#include "nspatial/ncspatialcell.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialbatch.h"
#include "nspatial/ncspatialportal.h"

#include "kernel/nlogclass.h"

nNebulaScriptClass(nOutdoorArea, "nloadarea");

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nOutdoorArea)
    NSCRIPT_ADDCMD('SLDT', void, SetLoadDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GLDT', float, GetLoadDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD('SUDT', void, SetUnloadDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GUDT', float, GetUnloadDistance, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nOutdoorArea::nOutdoorArea() :
    loadDistanceSq(100.0f*100.0f),
    unloadDistanceSq(150.0f*150.0f)
{
    this->areaType = OutdoorArea;
}

//------------------------------------------------------------------------------
/**
*/
nOutdoorArea::~nOutdoorArea()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Check distance to the nearest cell in the area. If the camera is near
    enough, schedule to whole area for loading, schedule for unloading if
    further away than some distance.
*/
void
nOutdoorArea::Trigger(const vector3& cameraPos, const int /*cameraCellId*/)
{
    if (this->neighbors.Empty())
    {
        return;
    }

    // refresh list of cells
    this->UpdateCells();

    //check linear distance to neighbor outdoor areas
    for (int index = 0; index < this->neighbors.Size(); ++index)
    {
        nLoadArea *curArea = this->neighbors[index];

        // find min distance squared to the area
        float minDistanceSq = curArea->GetMinDistanceSq(cameraPos);

        if (minDistanceSq < this->loadDistanceSq)
        {
            NLOG(loaderserver, (0 | nLoaderServer::NLOGAREAS, "Triggered loading of area: '%s' at distance: %.0f", curArea->GetName(), n_sqrt(minDistanceSq)))
            curArea->Load();
        }
        else if (minDistanceSq > this->unloadDistanceSq)
        {
            NLOG(loaderserver, (0 | nLoaderServer::NLOGAREAS, "Triggered unloading of area: '%s' at distance: %.0f", curArea->GetName(), n_sqrt(minDistanceSq)))
            curArea->Unload();
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Distance from camera to the nearest cell center in the area.
*/
float
nOutdoorArea::GetMinDistanceSq(const vector3& position)
{
    // refresh list of cells
    this->UpdateCells();

    float minDistanceSq = 1000000.0f;
    for (int index = 0; index < this->cellsArray.Size(); ++index)
    {
        n_assert(this->cellsArray[index].isvalid());
        ncSpatialCell *cell = this->cellsArray[index]->GetComponent<ncSpatialCell>();
        float distanceSq = (cell->GetBBoxCenter() - position).lensquared();
        if (distanceSq < minDistanceSq)
        {
            minDistanceSq = distanceSq;
        }
    }

    return minDistanceSq;
}

//------------------------------------------------------------------------------
/**
    Perform loading of all entities and their resources in the area cells.
*/
bool
nOutdoorArea::Load()
{
    if (!this->IsLoaded())
    {
        NLOG(loaderserver, (0 | NLOGUSER, "Loading outdoor area %s ...", this->GetName()))

        // refresh list of cells
        this->UpdateCells();

        // recursively load entities in all cells
        for (int index = 0; index < this->cellsArray.Size(); ++index)
        {
            ncSpatialQuadtreeCell *quadtreeCell;
            quadtreeCell = this->cellsArray[index]->GetComponentSafe<ncSpatialQuadtreeCell>();
            this->LoadEntitiesInCell(quadtreeCell);
        }

        return nLoadArea::Load();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Schedule unloading of all entities and their resources in the area cells.
*/
void
nOutdoorArea::Unload()
{
    if (this->IsLoaded())
    {
        NLOG(loaderserver, (0 | NLOGUSER, "Unloading outdoor area %s ...", this->GetName()))

        for (int index = 0; index < this->cellsArray.Size(); ++index)
        {
            ncSpatialQuadtreeCell *quadtreeCell;
            quadtreeCell = this->cellsArray[index]->GetComponentSafe<ncSpatialQuadtreeCell>();
            this->UnloadEntitiesInCell(quadtreeCell);
        }

        nLoadArea::Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nOutdoorArea::UpdateCells()
{
    // solve space and cell references if changes
    if (this->isDirty && this->refSpace.isvalid())
    {
        this->cellsArray.Reset();
        for (int index = 0; index < this->GetNumCells(); ++index)
        {
            ncSpatialSpace *space = this->refSpace->GetComponentSafe<ncSpatialSpace>();
            ncSpatialCell *cell = space->SearchCellById(this->GetCellAt(index));
            if (cell)
            {
                this->cellsArray.Append(cell->GetEntityObject());
            }
        }

        this->isDirty = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nOutdoorArea::LoadEntitiesInCell(ncSpatialQuadtreeCell* quadtreeCell)
{
    n_assert(quadtreeCell);
    ncSpatialQuadtree* quadtree = static_cast<ncSpatialQuadtree*>(quadtreeCell->GetParentSpace());
    ncSpatialQuadtreeCell *currentCell = quadtreeCell;

    while (currentCell)
    {
        //load subentities in the cell first
        currentCell->LoadEntities();

        //load resources for entities in the cell
        const nArray<nEntityObject*> *categories = currentCell->GetCategories();
        for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
        {
            const nArray<nEntityObject*> entities = categories[catIndex];
            for (int i = 0; i < entities.Size(); ++i)
            {
                ncSpatialBatch* batchComp = entities[i]->GetComponent<ncSpatialBatch>();
                if (batchComp)
                {
                    if (!currentCell->IsLeaf())
                    {
                        //check the lead the batch entity is in
                        ncSpatialQuadtreeCell* leafCell = quadtree->SearchLeafCell(batchComp->GetBBoxCenter());
                        if (!leafCell || (leafCell != quadtreeCell))
                        {
                            continue;
                        }
                    }
                    //load batched entities and their resources
                    this->LoadEntitiesInBatch(batchComp);
                }
                else
                {
                    if (!currentCell->IsLeaf())
                    {
                        //check the leaf the entity is in
                        ncTransform* transformComp = entities[i]->GetComponent<ncTransform>();
                        if (transformComp)
                        {
                            ncSpatialQuadtreeCell* leafCell = quadtree->SearchLeafCell(transformComp->GetPosition());
                            if (!leafCell || (leafCell != quadtreeCell))
                            {
                                continue;
                            }
                        }
                    }

                    nLoaderServer::Instance()->EntityNeedsLoading(entities[i]);
                }
            }
        }
        currentCell = currentCell->GetParentCell();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nOutdoorArea::UnloadEntitiesInCell(ncSpatialQuadtreeCell* quadtreeCell)
{
    n_assert(quadtreeCell);
    ncSpatialQuadtree* quadtree = static_cast<ncSpatialQuadtree*>(quadtreeCell->GetParentSpace());
    ncSpatialQuadtreeCell *currentCell = quadtreeCell;

    while (currentCell)
    {
        const nArray<nEntityObject*> *categories = currentCell->GetCategories();
        for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
        {
            const nArray<nEntityObject*> entities = categories[catIndex];
            for (int i = 0; i < entities.Size(); ++i)
            {
                ncSpatialBatch* batchComp = entities[i]->GetComponent<ncSpatialBatch>();
                if (batchComp)
                {
                    //unload batched entities
                    this->UnloadEntitiesInBatch(batchComp);
                }
                else
                {
                    if (!currentCell->IsLeaf())
                    {
                        //check the leaf the entity is in
                        ncTransform* transformComp = entities[i]->GetComponent<ncTransform>();
                        if (transformComp)
                        {
                            ncSpatialQuadtreeCell* leafCell = quadtree->SearchLeafCell(transformComp->GetPosition());
                            if (!leafCell || (leafCell != quadtreeCell))
                            {
                                continue;
                            }
                        }
                    }

                    //safely unload entity, saving it in its space
                    nLoaderServer::Instance()->UnloadEntitySafe(entities[i]);
                }
            }
        }
        currentCell = currentCell->GetParentCell();
    }
}

//------------------------------------------------------------------------------
/**
    recursively load batched entities
*/
void
nOutdoorArea::LoadEntitiesInBatch(ncSpatialBatch* spatialBatch)
{
    n_assert(spatialBatch);
    spatialBatch->LoadSubentities();

    const nArray<nEntityObject*>& subentities = spatialBatch->GetSubentities();
    for (int index = 0; index < subentities.Size(); ++index)
    {
        nEntityObject* subentity = subentities[index];
        n_assert(subentity);
        ncSpatialBatch* batch = subentities[index]->GetComponent<ncSpatialBatch>();
        if (batch)
        {
            this->LoadEntitiesInBatch(batch);
        }
        else
        {
            nLoaderServer::Instance()->EntityNeedsLoading(subentities[index]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nOutdoorArea::UnloadEntitiesInBatch(ncSpatialBatch* spatialBatch)
{
    n_assert(spatialBatch);
    const nArray<nEntityObject*>& subentities = spatialBatch->GetSubentities();
    for (int index = 0; index < subentities.Size(); ++index)
    {
        nEntityObject* subentity = subentities[index];
        n_assert(subentity);
        ncSpatialBatch* batch = subentities[index]->GetComponent<ncSpatialBatch>();
        if (batch)
        {
            this->LoadEntitiesInBatch(batch);
        }
        else
        {
            nLoaderServer::Instance()->UnloadEntitySafe(subentities[index]);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nOutdoorArea::SaveCmds(nPersistServer *ps)
{
    if (nLoadArea::SaveCmds(ps))
    {
        nCmd *cmd;

        // --- setloaddistance ---
        cmd = ps->GetCmd(this, 'SLDT');
        cmd->In()->SetF(this->GetLoadDistance());
        ps->PutCmd(cmd);

        // --- setloaddistance ---
        cmd = ps->GetCmd(this, 'SUDT');
        cmd->In()->SetF(this->GetUnloadDistance());
        ps->PutCmd(cmd);

        return true;
    }

    return false;
}

#ifndef NGAME

//------------------------------------------------------------------------------
/**
*/
void
nOutdoorArea::CreateDebugStats()
{
    //to retrieve data, the area must be fully loaded
    if (!this->IsLoaded())
    {
        return;
    }

    //track cells already visited when traversing up the quadtree
    nArray<int> visitedCells;

    //reset all existing area stats
    this->ResetStatsCounters();

    //traverse all cells up to the root counting entities and their classes
    for (int index = 0; index < this->cellsArray.Size(); ++index)
    {
        ncSpatialQuadtreeCell* quadtreeCell = this->cellsArray[index]->GetComponentSafe<ncSpatialQuadtreeCell>();

        ncSpatialQuadtreeCell* currentCell = quadtreeCell;
        while (currentCell && !visitedCells.Find(currentCell->GetId()))
        {
            visitedCells.Append(currentCell->GetId());

            const nArray<nEntityObject*> *categories = currentCell->GetCategories();
            for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
            {
                this->GetDebugStatsFromEntities(categories[catIndex]);
            }

            currentCell = currentCell->GetParentCell();
        }
    }

    //copy stats from collected arrays
    this->GetStatsCountersFromArrays();

    //mark stats as valid
    nLoadArea::CreateDebugStats();
}

//------------------------------------------------------------------------------
/**
    @todo ma.garcias - somehow determine which entities are specific to the area, 
    and which ones could appear in some other areas
    eg. mark specific unique classes, any unmarked class will be considered as shared.
*/
void
nOutdoorArea::GetDebugStatsFromEntities(const nArray<nEntityObject*>& entities)
{
    for (int i = 0; i < entities.Size(); ++i)
    {
        ncSpatialBatch* batchComp = entities[i]->GetComponent<ncSpatialBatch>();
        if (batchComp)
        {
            //recursively traverse all batched entities as well
            this->GetDebugStatsFromEntities(batchComp->GetSubentities());
        }
        else
        {
            ++this->statsCounters[StatsNumEntities];
            this->GetDebugStatsFromClass(entities[i]->GetEntityClass());
        }
    }
}

#endif //NGAME
