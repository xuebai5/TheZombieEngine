#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nloaderserver_areas.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nloaderserver.h"
#include "zombieentity/nloadarea.h"

#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialspace.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialportal.h"

//TEMPORARY- until neighbor cell information is moved to nspatial
#ifndef __ZOMBIE_EXPORTER__
#include "ngeomipmap/ncterraingmmcell.h"
#endif

#include "kernel/nfileserver2.h"
#include "kernel/nlogclass.h"

namespace
{
    const char* AreasPath("/usr/areas/");
    const char* FileAreasPath("level:areas/");
    #ifndef NGAME
    const char* FileAreasDebugPath("level:areas/debug/");
    #endif //NGAME
}

//------------------------------------------------------------------------------
/**
    create and initialize a load area
*/
nRoot *
nLoaderServer::CreateLoadArea(const char *className, const char *areaName)
{
    if (!this->refLoadAreas.isvalid())
    {
        this->refLoadAreas = kernelServer->New("nroot", AreasPath);
        n_assert(this->refLoadAreas.isvalid());
    }

    if (!this->refLoadAreas->Find(areaName))
    {
        kernelServer->PushCwd(this->refLoadAreas.get());
        nRoot* loadArea = kernelServer->New(className, areaName);
        kernelServer->PopCwd();
        n_assert(loadArea);
        n_assert(loadArea->IsA("nloadarea"));

        this->UpdateAreas();

        return loadArea;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    create and initialize a load area
*/
void
nLoaderServer::DeleteLoadArea(const char *areaName)
{
    if (this->refLoadAreas.isvalid())
    {
        nLoadArea* loadArea = static_cast<nLoadArea*>(this->refLoadAreas->Find(areaName));
        if (loadArea)
        {
            loadArea->Release();
            this->UpdateAreas();

            //save area name to delete file later
            this->deletedAreas.Append(areaName);
        }
    }
}

//------------------------------------------------------------------------------
/**
    initialize all loaded areas, resolve spaces and cells.
    this is called whenever a level is loaded-
*/
bool
nLoaderServer::LoadAreas()
{
    // release existing areas
    if (this->refLoadAreas.isvalid())
    {
        this->deletedAreas.Clear();
        this->refLoadAreas->Release();
        n_assert(!this->refLoadAreas.isvalid());
    }

    nFileServer2* fileServer = kernelServer->GetFileServer();
    nArray<nString> files(fileServer->ListFiles(FileAreasPath));
    if (!files.Empty())
    {
        this->refLoadAreas = kernelServer->New("nroot", AreasPath);
        n_assert(this->refLoadAreas.isvalid());

        kernelServer->PushCwd(this->refLoadAreas);
        for (int index = 0; index < files.Size(); ++index)
        {
            nLoadArea* loadArea;
            loadArea = static_cast<nLoadArea*>(kernelServer->Load(files[index].Get()));
            n_assert(loadArea);
            n_assert(loadArea->IsA("nloadarea"));

            #ifndef NGAME
            //load previously computed debug statistics
            nString fileName(FileAreasDebugPath);
            fileName.Append(files[index].ExtractFileName());
            if (fileServer->FileExists(fileName.Get()))
            {
                loadArea->LoadDebugStats(fileName.Get());
            }
            #endif
        }
        kernelServer->PopCwd();
    }

    // initialize entries for all currently existing cameras
    this->cameraEntry.Reset();

    // schedule areas for updating at Trigger
    this->UpdateAreas();

    return true;
}

//------------------------------------------------------------------------------
/**
    this method traverses all load areas and determines which ones are
    neighbors to others, and computes any needed values for easily checking
    proximity:
    * adjacent cells between outdoor areas
    * portals leading to another space for indoor areas
*/
void
nLoaderServer::DetermineAreaNeighbors()
{
    if (!this->refLoadAreas.isvalid())
    {
        return;
    }

    //for each area, check if it is a neighbor of every other one
    nLoadArea *curArea;
    for (curArea = static_cast<nLoadArea*>(this->refLoadAreas->GetHead());
         curArea;
         curArea = static_cast<nLoadArea*>(curArea->GetSucc()))
    {
        curArea->ClearNeighborAreas();
    }

    //for each area, check if it is a neighbor of every other one
    for (curArea = static_cast<nLoadArea*>(this->refLoadAreas->GetHead());
         curArea;
         curArea = static_cast<nLoadArea*>(curArea->GetSucc()))
    {
        nEntityObject* spaceEntity = curArea->GetSpaceEntity();
        if (!spaceEntity)
        {
            NLOG(resource, (0, "Invalid space id=%d referenced in area '%s'", curArea->GetSpaceId(), curArea->GetName()))
            continue;
        }

        switch (curArea->GetType())
        {
        case nLoadArea::OutdoorArea:
            {
                //TEMPORARY- until neighbor cell information is moved to nspatial
                #ifndef __ZOMBIE_EXPORTER__
                ncTerrainGMMCell::Direction directions[4] = { ncTerrainGMMCell::North,
                                                              ncTerrainGMMCell::East,
                                                              ncTerrainGMMCell::South,
                                                              ncTerrainGMMCell::West };

                //if it is an outdoor area, find which areas are neighbors to it:
                //find outdoor areas containing adjacent terrain cells
                //find indoor spaces with portals
                ncSpatialQuadtree* quadtreeSpace = spaceEntity->GetComponentSafe<ncSpatialQuadtree>();
                for (int cellIndex = 0; cellIndex < curArea->GetNumCells(); ++cellIndex)
                {
                    ncSpatialCell* cell = quadtreeSpace->SearchCellById(curArea->GetCellAt(cellIndex));
                    if (cell)
                    {
                        //get adjacent cells in quadtree using leaf neighbors
                        ncTerrainGMMCell* neighbor;
                        for (int dirIndex = 0; dirIndex < 4; ++dirIndex)
                        {
                            ncTerrainGMMCell *terrainCell = cell->GetComponent<ncTerrainGMMCell>();
                            if (terrainCell)
                            {
                                neighbor = terrainCell->GetNeighbor(directions[dirIndex]);
                                if (neighbor)
                                {
                                    ncSpatialCell* otherCell = neighbor->GetComponentSafe<ncSpatialCell>();
                                    nLoadArea* otherArea = this->FindAreaContainingCell(spaceEntity->GetId(), otherCell->GetId());
                                    if (otherArea && otherArea != curArea)
                                    {
                                        NLOG(loaderserver, (0 | NLOGAREAS, "Found neighbor areas '%s' and '%s'", curArea->GetName(), otherArea->GetName()))
                                        curArea->AddNeighborArea(otherArea);
                                        otherArea->AddNeighborArea(curArea);
                                    }
                                }
                            }
                        }

                        //NOTE: portals to indoor spaces are handled by indoor areas,
                        //so ignore them when checking outdoors
                    }
                }
                #endif
            }
            break;

        case nLoadArea::IndoorArea:
            {
                ncSpatialIndoor* indoorSpace = spaceEntity->GetComponentSafe<ncSpatialIndoor>();
                //if it is an indoor area, traverse all of its cells
                //looking for portals into other spaces
                //...
                nArray<ncSpatialCell*>& indoorCells = indoorSpace->GetCellsArray();
                for (int cellIndex = 0; cellIndex < indoorCells.Size(); ++cellIndex)
                {
                    ncSpatialCell* cell = indoorCells.At(cellIndex);
                    const nArray<nEntityObject*>& portals = cell->GetCategory(nSpatialTypes::CAT_PORTALS);
                    for (int index = 0; index < portals.Size(); ++index)
                    {
                        ncSpatialPortal* curPortal = portals[index]->GetComponentSafe<ncSpatialPortal>();
                        ncSpatialPortal* twinPortal = curPortal->GetTwinPortal();
                        if (twinPortal->GetCell()->GetParentSpace()->GetEntityObject() != spaceEntity)
                        {
                            nEntityObject* otherSpace = twinPortal->GetCell()->GetParentSpace()->GetEntityObject();
                            nLoadArea* otherArea = this->FindAreaContainingCell(otherSpace->GetId(), twinPortal->GetCell()->GetId());
                            if (otherArea && otherArea != curArea)
                            {
                                NLOG(loaderserver, (0 | NLOGAREAS, "Found neighbor areas '%s' and '%s'", curArea->GetName(), otherArea->GetName()))
                                curArea->AddNeighborArea(otherArea);
                                otherArea->AddNeighborArea(curArea);
                            }
                        }
                    }
                }
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
/**
    persist load areas, separated by wizards to avoid conflicts
*/
bool
nLoaderServer::SaveAreas()
{
    if (!this->refLoadAreas.isvalid())
    {
        return true;
    }

    nFileServer2 *fileServer = kernelServer->GetFileServer();
    if (!fileServer->DirectoryExists(FileAreasPath))
    {
        fileServer->MakePath(FileAreasPath);
    }
    
    // save areas in separate files
    nLoadArea *curChild;
    for (curChild = static_cast<nLoadArea*>(this->refLoadAreas->GetHead());
         curChild;
         curChild = static_cast<nLoadArea*>(curChild->GetSucc()))
    {
        nString fileName(FileAreasPath);
        fileName.Append(curChild->GetName());
        fileName.Append(".n2");
        if (!curChild->SaveAs(fileName.Get()))
        {
            return false;
        }

        #ifndef NGAME
        fileName.Set(FileAreasDebugPath);
        fileName.Append(curChild->GetName());
        fileName.Append(".n2");
        curChild->SaveDebugStats(fileName.Get());
        #endif //NGAME
    }

    //delete pending areas
    for (int i = 0; i < this->deletedAreas.Size(); ++i)
    {
        if (!this->refLoadAreas->Find(this->deletedAreas[i].Get()))
        {
            nString fileName(FileAreasPath);
            fileName.Append(deletedAreas[i]);
            fileName.Append(".n2");
            if (fileServer->FileExists(fileName))
            {
                fileServer->DeleteFile(fileName);
            }
        }
    }

    this->deletedAreas.Clear();

    return true;
}

//------------------------------------------------------------------------------
/**
    release all areas for the current level
*/
void
nLoaderServer::ReleaseAreas()
{
    if (this->refLoadAreas.isvalid())
    {
        this->refLoadAreas->Release();
    }
}

//------------------------------------------------------------------------------
/**
    release all areas for the current level
*/
nLoadArea*
nLoaderServer::FindArea(const char *areaName)
{
    if (this->refLoadAreas.isvalid())
    {
        return static_cast<nLoadArea*>(this->refLoadAreas->Find(areaName));
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    find the load area for a space and cell.
    if more than one area contain the cell, only the first one found is
    returned- extend this behavior if required, for now we're not tracking
    overlapped areas-

    @return     - the first area found containing the cell, 0 if not found
*/
nLoadArea*
nLoaderServer::FindAreaContainingCell(nEntityObjectId spaceId, int cellId)
{
    if (this->refLoadAreas.isvalid())
    {
        nLoadArea* curArea;
        for (curArea = static_cast<nLoadArea*>(this->refLoadAreas->GetHead());
             curArea;
             curArea = static_cast<nLoadArea*>(curArea->GetSucc()))
        {
            if (this->AreaContainsCell(curArea, spaceId, cellId))
            {
                return curArea;
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nLoaderServer::AreaContainsCell(nLoadArea* loadArea, nEntityObjectId spaceId, int cellId)
{
    n_assert(loadArea);
    if (loadArea->GetSpaceId() == spaceId &&
        (loadArea->GetType() == nLoadArea::IndoorArea || loadArea->ContainsCell(cellId)))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    performs a distance comparison from every active camera to load areas
    in their current cells.
*/
void
nLoaderServer::TriggerAreas()
{
    if (!this->refLoadAreas.isvalid())
    {
        return;
    }

    // update areas if there has been any change
    if (this->areasDirty)
    {
        this->DetermineAreaNeighbors();

        this->areasDirty = false;
    }

    // check the cell of active camera(s)
    // if the cell camera is in not the same as last frame,
    // check current neightbors and unload them if outside distance
    // determine the new area(s) and update current neighbors
    const nArray<ncSpatialCamera*>& cameras = nSpatialServer::Instance()->GetCamerasArray();

    // if the number of cameras has changed since last frame, reset the cache
    if (cameras.Size() != this->cameraEntry.Size())
    {
        //TODO- mark for unloading all cells within range of current cameras
        this->cameraEntry.SetFixedSize(cameras.Size());
    }

    for (int i = 0; i < cameras.Size(); ++i)
    {
        // if camera is off, or in cell -1 (outside any space) do nothing
        if (!cameras[i]->IsOn() || cameras[i]->GetCell() == 0)
        {
            continue;
        }

        const vector3& cameraPos = cameras[i]->GetEntityObject()->GetComponent<ncTransform>()->GetPosition();
        ncSpatialCell* cameraCell = cameras[i]->GetCell();
        if (cameraCell && cameraCell->GetParentSpace())
        {
            ncSpatialSpace* cameraSpace = cameraCell->GetParentSpace();
            nEntityObjectId cameraSpaceId = cameraSpace->GetEntityObject()->GetId();
            int cameraCellId = cameraCell->GetId();

            // if the camera is outdoors, find the leaf cell of the terrain
            if (cameraCellId == 0 && cameraSpace->GetType() == ncSpatialSpace::N_QUADTREE_SPACE)
            {
                ncSpatialQuadtree* quadtree = static_cast<ncSpatialQuadtree*>(cameraCell->GetParentSpace());
                cameraCell = quadtree->SearchLeafCell(cameraPos);
                if (cameraCell)
                {
                    cameraCellId = cameraCell->GetId();
                }
            }

            // if camera is in the same cell as before, do nothing
            // if not, update current area and list of neighbors
            CameraEntry& cameraEntry = this->cameraEntry[i];

            if (cameraEntry.curCellId != cameraCellId)
            {
                NLOG(loaderserver, (0 | NLOGUSER | NLOGAREAS, "Camera entered cell: %u, in space: %s", cameraCellId, cameraSpace->GetEntityObject()->GetClass()->GetName()))

                cameraEntry.curCellId = cameraCellId;

                // find new cell in loading areas for the space:
                // although there can be more that one areas for the cell,
                // the current one is never re-determined unless necessary
                if (!cameraEntry.curLoadArea ||
                    !this->AreaContainsCell(cameraEntry.curLoadArea, cameraSpaceId, cameraCellId))
                {
                    nLoadArea* newLoadArea = this->FindAreaContainingCell(cameraSpaceId, cameraCellId);

                    if (newLoadArea)
                    {
                        if (cameraEntry.curLoadArea &&
                            cameraEntry.curLoadArea != newLoadArea)
                        {
                            // unload neighbors of current area not neighbors of new one
                            // (loading only reaches adjacent areas)
                            for (int i = 0; i < cameraEntry.curLoadArea->GetNumNeighborAreas(); ++i)
                            {
                                if (cameraEntry.curLoadArea->GetNeighborAreaAt(i) != newLoadArea &&
                                    !cameraEntry.curLoadArea->GetNeighborAreaAt(i)->IsNeighborArea(newLoadArea))
                                {
                                    NLOG(loaderserver, (0 | NLOGAREAS, "Triggered unloading of area: '%s' not neighbor of: '%s'", cameraEntry.curLoadArea->GetNeighborAreaAt(i)->GetName(), newLoadArea->GetName()))
                                    cameraEntry.curLoadArea->GetNeighborAreaAt(i)->Unload();
                                }
                            }

                            //temporarily, we simply keep a previous area, 
                            //and whenever the previous area has been left behind, 
                            //it is automatically unloaded.

                            //<KEEP> just in case the loading policy changes:
                            //if the new area is not the current OR the previous
                            //unload the previous
                            //if (cameraEntry.prevLoadArea &&
                            //    cameraEntry.prevLoadArea != newLoadArea)
                            //{
                            //    cameraEntry.prevLoadArea->Unload();
                            //}
                            //<KEEP>
                            
                            cameraEntry.prevLoadArea = cameraEntry.curLoadArea;
                        }

                        cameraEntry.curLoadArea = newLoadArea;
                        cameraEntry.curLoadArea->Load();
                    }
                }

                #if __NEBULA_STATS__
                this->watchCurArea->SetS(cameraEntry.curLoadArea ? cameraEntry.curLoadArea->GetName() : "");
                this->watchPrevArea->SetS(cameraEntry.prevLoadArea ? cameraEntry.prevLoadArea->GetName() : "");
                #endif

                //trigger loading/unloading of neighbor areas from the current one
                //use position and cell to trigger loading of neighbor areas
                //each different type of area may handle position and cell differently
                //check that the camera remains in the area, otherwise the cell could be invalid
                if (cameraEntry.curLoadArea &&
                    this->AreaContainsCell(cameraEntry.curLoadArea, cameraSpaceId, cameraCellId))
                {
                   cameraEntry.curLoadArea->Trigger(cameraPos, cameraCellId);
                }
            }
        }
    }
}
