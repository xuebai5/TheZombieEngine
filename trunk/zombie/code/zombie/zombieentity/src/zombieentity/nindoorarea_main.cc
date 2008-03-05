#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nindoorarea_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nindoorarea.h"
#include "zombieentity/nloaderserver.h"

#include "nspatial/ncspatialspace.h"
#include "nspatial/ncspatialportal.h"

#include "kernel/nlogclass.h"

nNebulaScriptClass(nIndoorArea, "nloadarea");

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nIndoorArea)
    NSCRIPT_ADDCMD('SLDT', void, SetLoadDistance, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GLDT', int, GetLoadDistance, 0, (), 0, ());
    NSCRIPT_ADDCMD('SUDT', void, SetUnloadDistance, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GUDT', int, GetUnloadDistance, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nIndoorArea::nIndoorArea() :
    visitedCells(16, 16),
    loadDistance(3),
    unloadDistance(5)
{
    this->areaType = IndoorArea;
}

//------------------------------------------------------------------------------
/**
*/
nIndoorArea::~nIndoorArea()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    compute the distance from all indoor cells to all neighbor areas.
*/
int
nIndoorArea::GetCellDistanceTo(ncSpatialCell* fromCell, ncSpatialSpace* targetSpace)
{
    n_assert(fromCell);
    n_assert(targetSpace);

    int minDistance = -1;
    const nArray<nEntityObject*>& portals = fromCell->GetCategory(nSpatialTypes::CAT_PORTALS);
    for (int index = 0; index < portals.Size(); ++index)
    {
        ncSpatialPortal* curPortal = portals[index]->GetComponentSafe<ncSpatialPortal>();
        ncSpatialPortal* otherPortal = curPortal->GetTwinPortal();
        if (otherPortal && otherPortal->GetCell())
        {
            //if the cell leads directly into the other space, return 1
            ncSpatialSpace* otherSpace = otherPortal->GetCell()->GetParentSpace();
            if (otherSpace == targetSpace)
            {
                return 1;
            }
            //if not, and we're within the same space, recursively get the distance
            if (otherSpace == fromCell->GetParentSpace())
            {
                if (!this->visitedCells.HasKey(otherPortal->GetCell()->GetId()))
                {
                    //check the current cell as visited first to avoid circular references!
                    this->visitedCells.Add(fromCell->GetId(), true);
                    
                    int cellDistance = this->GetCellDistanceTo(otherPortal->GetCell(), targetSpace);
                    if (cellDistance != -1)
                    {
                        //current cell is one cell away
                        ++cellDistance;

                        if (minDistance == -1 || cellDistance < minDistance)
                        {
                            minDistance = cellDistance;
                        }
                    }
                }
            }
            //otherwise, the portal is a dead end into an unreferenced space
            //so we simply step back to the next
        }
    }
    return minDistance;
}

//------------------------------------------------------------------------------
/**
    compute the distance from all indoor cells to all neighbor areas.
*/
void
nIndoorArea::UpdateCells()
{
    if (this->isDirty && this->refSpace.isvalid())
    {
        ncSpatialSpace* indoorSpace = this->refSpace->GetComponent<ncSpatialSpace>();
        nArray<ncSpatialCell*>& cellsArray = indoorSpace->GetCellsArray();
        this->cellDistances.SetSize(cellsArray.Size(), this->neighbors.Size());

        //for every cell, recursively traverse through portals leading to neighbor spaces
        for (int areaIndex = 0; areaIndex < this->neighbors.Size(); ++areaIndex)
        {
            ncSpatialSpace* neighborSpace = this->neighbors[areaIndex]->GetSpaceEntity()->GetComponent<ncSpatialSpace>();
            for (int cellIndex = 0; cellIndex < cellsArray.Size(); ++cellIndex)
            {
                //clear the list of visited cells
                this->visitedCells.Clear();

                //for the cell at cellIndex, get its distance to the space
                int minDistance = this->GetCellDistanceTo(cellsArray[cellIndex], neighborSpace);
                this->cellDistances.Set(cellIndex, areaIndex, minDistance);
            }
        }

        //for every cell, find which portals lead to the outdoor space
        for (int cellIndex = 0; cellIndex < cellsArray.Size(); ++cellIndex)
        {
            const nArray<nEntityObject*>& portals = cellsArray[cellIndex]->GetCategory(nSpatialTypes::CAT_PORTALS);
            for (int index = 0; index < portals.Size(); ++index)
            {
                ncSpatialPortal* curPortal = portals[index]->GetComponentSafe<ncSpatialPortal>();
                ncSpatialPortal* twinPortal = curPortal->GetTwinPortal();
                if (twinPortal->GetCell() &&
                    twinPortal->GetCell()->GetType() == ncSpatialCell::N_QUADTREE_CELL)
                {
                    if (!this->outdoorPortals.Find(twinPortal->GetEntityObject()))
                    {
                        this->outdoorPortals.Append(twinPortal->GetEntityObject());
                    }
                }
            }
        }

        this->isDirty = false;
    }
}

//------------------------------------------------------------------------------
/**
    check distance of the camera to the nearest cell in the indoor.
    In case the camera is near enough, schedule for loading-
    if it is far enough schedule for unloading.
*/
void
nIndoorArea::Trigger(const vector3& /*cameraPos*/, const int cameraCellId)
{
    if (this->neighbors.Empty())
    {
        return;
    }

    // refresh list of cells, portals and neighbor distances
    this->UpdateCells();

    //from the cell the camera is in, find the boronoi distance through portals
    //to every neighbor area (space), number of portals required to get there.
    ncSpatialSpace* indoorSpace = this->refSpace->GetComponent<ncSpatialSpace>();
    ncSpatialCell* indoorCell = indoorSpace->SearchCellById(cameraCellId);
    n_assert(indoorCell);

    int cellIndex = indoorSpace->GetCellsArray().FindIndex(indoorCell);
    for (int areaIndex = 0; areaIndex < this->neighbors.Size(); ++areaIndex)
    {
        int cellDistance = this->cellDistances.At(cellIndex, areaIndex);
        if (cellDistance <= this->GetLoadDistance())
        {
            NLOG(loaderserver, (0 | nLoaderServer::NLOGAREAS, "Triggered loading of area: '%s' at distance: %u (cells)", this->neighbors[areaIndex]->GetName(), cellDistance))
            this->neighbors[areaIndex]->Load();
        }
        else if (cellDistance > this->GetUnloadDistance())
        {
            NLOG(loaderserver, (0 | nLoaderServer::NLOGAREAS, "Triggered unloading of area: '%s' at distance: %u (cells)", this->neighbors[areaIndex]->GetName(), cellDistance))
            this->neighbors[areaIndex]->Unload();
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Distance from camera to the nearest cell center in the area.
*/
float
nIndoorArea::GetMinDistanceSq(const vector3& position)
{
    // refresh list of cells, portals and neighbor distances
    this->UpdateCells();

    float minDistanceSq = 1000000.0f;
    for (int index = 0; index < this->outdoorPortals.Size(); ++index)
    {
        n_assert(this->outdoorPortals[index].isvalid());
        ncSpatialPortal *portal = this->outdoorPortals[index]->GetComponent<ncSpatialPortal>();
        float distanceSq = (portal->GetBBoxCenter() - position).lensquared();
        if (distanceSq < minDistanceSq)
        {
            minDistanceSq = distanceSq;
        }
    }
    return minDistanceSq;
}

//------------------------------------------------------------------------------
/**
    Perform loading of all entities and their resources in the space.
*/
bool
nIndoorArea::Load()
{
    if (!this->IsLoaded())
    {
        NLOG(loaderserver, (0 | NLOGUSER | nLoaderServer::NLOGAREAS, "Loading indoor area %s ...", this->GetName()))

        //load contained entities
        nLoaderServer::Instance()->LoadSpace(this->refSpace.get());

        //load all entities in the space
        ncSpatialSpace *indoorSpace = this->refSpace->GetComponent<ncSpatialSpace>();
        nArray<ncSpatialCell*>& cells = indoorSpace->GetCellsArray();
        for (int cell = 0; cell < cells.Size(); ++cell)
        {
            const nArray<nEntityObject*> *categories = cells[cell]->GetCategories();
            for (int cat = 0; cat < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++cat)
            {
                const nArray<nEntityObject*>& entities = categories[cat];
                for (int index = 0; index < entities.Size(); ++index)
                {
                    nLoaderServer::Instance()->EntityNeedsLoading(entities[index]);
                }
            }
        }

        return nLoadArea::Load();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Schedule unloading of all entities and their resources in the space(s).
*/
void
nIndoorArea::Unload()
{
    if (this->IsLoaded())
    {
        NLOG(loaderserver, (0 | NLOGUSER | nLoaderServer::NLOGAREAS, "Unloading indoor area %s ...", this->GetName()))

        //unload resources for own space entities
        nLoaderServer::Instance()->EntityNeedsUnloading(this->refSpace.get());

        //unload resource for all entities, own or contained
        ncSpatialSpace *indoorSpace = this->refSpace->GetComponent<ncSpatialSpace>();
        nArray<ncSpatialCell*>& cells = indoorSpace->GetCellsArray();
        for (int cell = 0; cell < cells.Size(); ++cell)
        {
            const nArray<nEntityObject*> *categories = cells[cell]->GetCategories();
            for (int cat = 0; cat < nSpatialTypes::NUM_SPATIAL_CATEGORIES; ++cat)
            {
                const nArray<nEntityObject*>& entities = categories[cat];
                for (int index = 0; index < entities.Size(); ++index)
                {
                    nLoaderServer::Instance()->UnloadEntitySafe(entities[index]);
                }
            }
        }

        nLoadArea::Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nIndoorArea::SaveCmds(nPersistServer *ps)
{
    if (nLoadArea::SaveCmds(ps))
    {
        nCmd *cmd;

        // --- setloaddistance ---
        cmd = ps->GetCmd(this, 'SLDT');
        cmd->In()->SetI(this->GetLoadDistance());
        ps->PutCmd(cmd);

        // --- setunloaddistance ---
        cmd = ps->GetCmd(this, 'SUDT');
        cmd->In()->SetI(this->GetUnloadDistance());
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
nIndoorArea::CreateDebugStats()
{
    //to retrieve data, the area must be fully loaded
    if (!this->IsLoaded())
    {
        return;
    }

    //reset all existing area stats
    this->ResetStatsCounters();

    //traverse all cells up to the root counting entities and their classes
    ncSpatialSpace* indoorSpace = this->refSpace->GetComponent<ncSpatialSpace>();
    nArray<ncSpatialCell*>& cellsArray = indoorSpace->GetCellsArray();
    for (int cellIndex = 0; cellIndex < cellsArray.Size(); ++cellIndex)
    {
        ncSpatialCell* currentCell = cellsArray[cellIndex];

        const nArray<nEntityObject*> *categories = currentCell->GetCategories();
        for (int catIndex = 0; catIndex < nSpatialTypes::NUM_SPATIAL_CATEGORIES; catIndex++)
        {
            const nArray<nEntityObject*>& entities = categories[catIndex];
            for (int i = 0; i < entities.Size(); ++i)
            {
                ++this->statsCounters[StatsNumEntities];
                this->GetDebugStatsFromClass(entities[i]->GetEntityClass());
            }
        }
    }

    //copy stats from collected arrays
    this->GetStatsCountersFromArrays();

    //mark stats as valid
    nLoadArea::CreateDebugStats();

}

#endif //NGAME
