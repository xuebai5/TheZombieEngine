#ifndef N_LOADERSERVER_H
#define N_LOADERSERVER_H
//------------------------------------------------------------------------------
/**
    @class nLoaderServer
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>
    
    @brief This class is responsible for resource loading for entities.
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentityobjectserver.h"

#include "kernel/nroot.h"
#include "kernel/nprofiler.h"

#if __NEBULA_STATS__
#include "misc/nwatched.h"
#endif

class nLoadArea;
//------------------------------------------------------------------------------
class ncSpatialQuadtreeCell;
class ncSpatialIndoor;

//------------------------------------------------------------------------------
class nLoaderServer : public nRoot
{
public:
    /// constructor
    nLoaderServer();
    /// destructor
    ~nLoaderServer();
    /// get server instance
    static nLoaderServer* Instance();

    /// trigger loading
    virtual void Trigger();

    /// set load entities in spaces
    void SetLoadSpaces(bool);
    /// get load entities in spaces
    bool GetLoadSpaces();
    /// set load entities in batches
    void SetLoadBatches(bool);
    /// get load entities in batches
    bool GetLoadBatches();
    /// set load entity resources enabled
    void SetLoadResources(bool);
    /// get load entity resources enabled
    bool GetLoadResources();
    /// set unload unused class resources
    void SetUnloadClassResources(bool);
    /// get unload unused class resources
    bool GetUnloadClassResources();
    
    /// called when an entity is created
    virtual void EntityCreated(nEntityObject* entityObject, nObject::InitInstanceMsg initType);
    /// called when an entity is deleted
    virtual void EntityDeleted(int entityObjectId);
    /// schedule entity for immediate loading
    virtual void EntityNeedsLoading(nEntityObject* entityObject);
    /// schedule entity for immediate unloading
    virtual void EntityNeedsUnloading(nEntityObject* entityObject);

    /// called when entities of a class are about to be loaded
    virtual void ClassResourcesRequired(nEntityClass* entityClass);
    /// called when number of loaded entities of a class reaches zero
    virtual void ClassResourcesUnused(nEntityClass* entityClass);

    /// load contained entities: spaces, cells, batches
    virtual bool LoadSubentities(nEntityObject* entityObject);
    /// load entities contained in a space
    virtual bool LoadSpace(nEntityObject* entityObject);
    /// load entities contained in a batch
    virtual bool LoadBatch(nEntityObject* entityObject);
    /// load resources for an entity
    virtual bool LoadClassResources(nEntityClass* entityClass);
    /// load resources for an entity
    virtual bool LoadEntityResources(nEntityObject* entityObject);
    /// unload resources for an entity
    virtual void UnloadEntityResources(nEntityObject* entityObject);
    
    /// prevent a class from being unloaded
    void RetainClassResources(nEntityClass* entityClass);

    /// utility for safe unloading of entities
    void UnloadEntitySafe(nEntityObject* entityObject);

    /// create load area from tools
    nRoot* CreateLoadArea(const char *, const char *);
    /// delete load area from tools
    void DeleteLoadArea(const char *);
    /// set load areas are enabled
    void SetLoadAreasEnabled(bool);
    /// get load areas are enabled
    bool GetLoadAreasEnabled();

    /// has load areas
    bool AreasAreLoaded();
    /// load areas
    bool LoadAreas();
    /// load areas
    bool SaveAreas();
    /// release areas
    void ReleaseAreas();
    /// trigger areas
    void TriggerAreas();

    /// notifies that an area has been created, updated or removed
    void UpdateAreas();

    /// find area by name
    nLoadArea* FindArea(const char *name);
    /// find first area containing the given cell
    nLoadArea* FindAreaContainingCell(nEntityObjectId spaceId, int cellId);
    /// find if the area contains the given cell
    bool AreaContainsCell(nLoadArea* loadArea, nEntityObjectId spaceId, int cellId);

    const static int NLOGEVENTS;
    const static int NLOGAREAS;
    const static int NLOGSTATS;

protected:
    static nLoaderServer* Singleton;

    typedef nArray<nRef<nEntityObject> > nRefEntityArray;
    typedef nArray<nRef<nEntityClass> > nRefClassArray;

    /// recursively load subentities in the quadtree cell
    void LoadQuadtreeCell(ncSpatialQuadtreeCell* quadtreeCell);
    /// load subentities in the array of containers, remove all loaded ones
    void LoadContainerArray(nRefEntityArray& entityArray);
    /// load all entities in the array, remove all loaded ones
    void LoadEntityArray(nRefEntityArray& entityArray);
    /// load all classes in the array, remove loaded ones
    void LoadClassArray(nRefClassArray& classArray);

    bool loadResources;
    bool loadSpaces;
    bool loadBatches;
    bool unloadClassResources;

    nRefEntityArray pendingImmediate;
    nRefEntityArray pendingEntities;
    nRefEntityArray pendingSpaces;
    nRefEntityArray pendingBatches;

    nRefEntityArray pendingUnload;
    nRefClassArray pendingLoadClasses;
    nRefClassArray pendingUnloadClasses;
    
    /// load areas enabled
    bool loadAreasEnabled;
    /// root node for load areas (/usr/areas)
    nRef<nRoot> refLoadAreas;
    /// areas need refreshing
    bool areasDirty;
    /// names of deleted areas to delete file at save
    nArray<nString> deletedAreas;

    /// determine which areas are neighbors of others
    void DetermineAreaNeighbors();

    /// set of cameras for loading events
    struct CameraEntry
    {
        CameraEntry() : curSpaceId(0), curCellId(-1), curLoadArea(0), prevLoadArea(0) { }

        nEntityObjectId curSpaceId;
        int curCellId;
        nLoadArea *curLoadArea;
        nLoadArea *prevLoadArea;
    };
    nArray<CameraEntry> cameraEntry;
    
    #if __NEBULA_STATS__
    nProfiler profLoaderServer;
    nWatched watchCurArea;
    nWatched watchPrevArea;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nLoaderServer*
nLoaderServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLoaderServer::SetLoadSpaces(bool value)
{
    this->loadSpaces = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLoaderServer::GetLoadSpaces()
{
    return this->loadSpaces;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLoaderServer::SetLoadBatches(bool value)
{
    this->loadBatches = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLoaderServer::GetLoadBatches()
{
    return this->loadBatches;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLoaderServer::SetLoadAreasEnabled(bool value)
{
    this->loadAreasEnabled = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLoaderServer::GetLoadAreasEnabled()
{
    return this->loadAreasEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLoaderServer::SetLoadResources(bool value)
{
    this->loadResources = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLoaderServer::GetLoadResources()
{
    return this->loadResources;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLoaderServer::SetUnloadClassResources(bool value)
{
    this->unloadClassResources = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLoaderServer::GetUnloadClassResources()
{
    return this->unloadClassResources;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nLoaderServer::AreasAreLoaded()
{
    return this->refLoadAreas.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nLoaderServer::UpdateAreas()
{
    this->areasDirty = true;
}

//------------------------------------------------------------------------------
#endif /*N_LOADERSERVER_H*/
