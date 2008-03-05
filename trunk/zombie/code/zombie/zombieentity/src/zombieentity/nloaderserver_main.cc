#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nloaderserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nloaderserver.h"
#include "zombieentity/ncloader.h"
#include "zombieentity/ncloaderclass.h"
#include "nspatial/ncspatialspace.h"
#include "nspatial/ncspatialbatch.h"
#include "nspatial/ncspatialcamera.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/nspatialindoorcell.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "entity/nentityobjectserver.h"
#include "kernel/nlogclass.h"
#ifndef NGAME
#include "ndebug/nceditorclass.h"
#endif
#include "ngeomipmap/ncterraingmm.h"
#include "ngeomipmap/ncterraingmmcell.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nLoaderServer, "nroot");

nLoaderServer* nLoaderServer::Singleton = 0;

const int nLoaderServer::NLOGEVENTS = NLOG1;
const int nLoaderServer::NLOGAREAS  = NLOG2;
const int nLoaderServer::NLOGSTATS  = NLOG3;

static const char * loaderServerLogNames[] = { "Events", "Loading areas", "Statistics", 0 };

NCREATELOGLEVELGROUP(loaderserver, "Loader Server", false, 2, loaderServerLogNames, NLOG_GROUP_MASK);
// 0: general load/unload events
// 1: complete event log with incidencies

//------------------------------------------------------------------------------
/**
    constructor
*/
nLoaderServer::nLoaderServer() :
    loadResources(true),
    loadBatches(true),
    loadSpaces(true),
    loadAreasEnabled(true),
    areasDirty(false),
    cameraEntry(4, 4),
    pendingImmediate(1024, 1024),
    pendingEntities(1024, 1024),
    pendingSpaces(16, 16),
    pendingBatches(256, 256),
    pendingUnload(1024, 1024),
    pendingLoadClasses(64, 64),
    pendingUnloadClasses(64, 64)
    #if __NEBULA_STATS__
   ,profLoaderServer("profLoaderServer", true),
    watchCurArea("loadCurArea", nArg::String),
    watchPrevArea("loadPrevArea", nArg::String)
    #endif
{
    n_assert(0 == Singleton);
    Singleton = this;

    // bind to entity deletion signal
    nEntityObjectServer::Instance()->BindSignal(nEntityObjectServer::SignalEntityDeleted,
                                                this,
                                                &nLoaderServer::EntityDeleted,
                                                0);
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nLoaderServer::~nLoaderServer()
{
    n_assert(0 != Singleton);
    Singleton = 0;

    this->pendingEntities.Clear();
    this->pendingBatches.Clear();
    this->pendingSpaces.Clear();
}

//------------------------------------------------------------------------------
/**
    called when an entity with a loader component is created.
    Override in subclasses to refine loading behavior.
    @todo right now it is called from ncLoader::InitInstance only
*/
void
nLoaderServer::EntityCreated(nEntityObject* entityObject, nObject::InitInstanceMsg initType)
{
    n_assert(entityObject);
    NLOG(loaderserver, (1 | NLOGEVENTS, "nLoaderServer::EntityCreated(entity: %x, class: %s)", entityObject->GetId(), entityObject->GetClass()->GetName()));

    /** @todo ma.garcias- if load areas enabled, this is where to intercept
        any automatic loading process, enqueuing entities for batch loading
        if for any reason spaces, cells, batches or entities are not to be loaded */

    // load entities contained in spaces and cells if enabled
    if (entityObject->GetComponent<ncSpatialSpace>() ||
        entityObject->GetComponent<ncSpatialCell>())
    {
        // enqueue spaces for loading contained entities
        // cells are automatically handled by their spaces
        if (entityObject->GetComponent<ncSpatialSpace>())
        {
            this->pendingSpaces.Append(entityObject);
        }

        //</HACK> for now, outdoor terrain and cells are automatically loaded
        //including list of contained entities
        if (entityObject->GetComponent<ncTerrainGMM>() ||
            entityObject->GetComponent<ncTerrainGMMCell>())
        {
            this->EntityNeedsLoading(entityObject);
        }
        else
        //</HACK>
        {
            // enqueue resources for spaces and terrain cells as usual
            this->pendingEntities.Append(entityObject);
        }
    }
    else
    {
        //UGLY HACK- forgive me, this is the only way to know just-created facades that I could think of
        //when created at indoor initialization, they are not categorized yet
        ///@todo ma.garcias- find an alternative to this crappy way of identifying facades
        bool isIndoorFacade = (strstr(entityObject->GetClass()->GetName(), "_facade") != 0);

        int category = nSpatialServer::Instance()->GetCategory(entityObject);
        switch (category)
        {
        case nSpatialTypes::CAT_GRAPHBATCHES:
            this->pendingBatches.Append(entityObject);
            this->EntityNeedsLoading(entityObject);
            break;

        case nSpatialTypes::CAT_BRUSHES:
        case nSpatialTypes::CAT_INDOOR_BRUSHES:
        case nSpatialTypes::CAT_PORTALS:
        case nSpatialTypes::CAT_FACADES:

        case nSpatialTypes::CAT_AGENTS:
        case nSpatialTypes::CAT_WEAPONS:
        case nSpatialTypes::CAT_VEHICLES:
        case nSpatialTypes::CAT_WAYPOINTS:
        case nSpatialTypes::CAT_NAVMESHNODES:

            if (nEntityObjectServer::Instance()->GetEntityObjectType(entityObject->GetId()) != nEntityObjectServer::Normal &&
                initType == nObject::NewInstance && !isIndoorFacade)
            {
                // immediately load local entities that have been created on the fly
                this->EntityNeedsLoading(entityObject);
            }
            else
            {
                // enqueue entity resource loading for all entities
                this->pendingEntities.Append(entityObject);
            }
            break;

        default:
            // schedule any other entities for immediate loading
            this->EntityNeedsLoading(entityObject);
        }
    }
}

//------------------------------------------------------------------------------
/**
    this can be invoked externally to trigger loading of the param
    entity at next Trigger of loader server.

    history:
        - 23-Mar-2006   ma.garcias  optimized to remove heavy checks
*/
void
nLoaderServer::EntityNeedsLoading(nEntityObject* entity)
{
    ncLoaderClass* loaderClass = entity->GetClassComponent<ncLoaderClass>();
    if (loaderClass && !loaderClass->AreResourcesValid())
    {
        this->ClassResourcesRequired(entity->GetEntityClass());
    }

    this->pendingImmediate.Append(entity);
}

//------------------------------------------------------------------------------
/**
*/
void
nLoaderServer::EntityNeedsUnloading(nEntityObject* entity)
{
    nRefEntityArray::iterator entityIter;

    //schedule for unloading
    entityIter = this->pendingUnload.Find(entity);
    if (!entityIter)
    {
        pendingUnload.Append(entity);
    }
}

//------------------------------------------------------------------------------
/**
    this method can be invoked externally to automatically load
    entities contained in the provided container entity-
    this works with spaces, cells and batches.
*/
bool
nLoaderServer::LoadSubentities(nEntityObject* entity)
{
    if (entity->GetComponent<ncSpatialSpace>())
    {
        //load entities in space, and all of its cells
        return this->LoadSpace(entity);
    }
    else if (entity->GetComponent<ncSpatialBatch>())
    {
        //load entities in batch
        return this->LoadBatch(entity);
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nLoaderServer::ClassResourcesRequired(nEntityClass* entityClass)
{
    n_assert(entityClass);
    
    //schedule for immediate loading class resources
    nRefClassArray::iterator classIter = this->pendingLoadClasses.Find(entityClass);
    if (!classIter)
    {
        pendingLoadClasses.Append(entityClass);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLoaderServer::ClassResourcesUnused(nEntityClass* entityClass)
{
    n_assert(entityClass);

    //schedule for unloading
    nRefClassArray::iterator classIter = this->pendingUnloadClasses.Find(entityClass);
    if (!classIter)
    {
        pendingUnloadClasses.Append(entityClass);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLoaderServer::Trigger()
{
    #if __NEBULA_STATS__
    this->profLoaderServer.Start();
    #endif

    //#ifndef NGAME
    if (this->GetLoadAreasEnabled() && this->refLoadAreas.isvalid())
    //#endif
    {
        this->TriggerAreas();
    }

    // check for pending batches within the current visibility region
    if (this->loadBatches && !this->pendingBatches.Empty())
    {
        this->LoadContainerArray(this->pendingBatches);
    }

    // check for pending spaces and cells
    if (this->loadSpaces && !this->pendingSpaces.Empty())
    {
        N_IFDEF_NLOG(nTime time = nTimeServer::Instance()->GetTime());
        NLOG(loaderserver, (1 | NLOGEVENTS, "Begin loading pending spaces..."))

        this->LoadContainerArray(this->pendingSpaces);

        N_IFDEF_NLOG(time = nTimeServer::Instance()->GetTime() - time);
        NLOG(loaderserver, (1 | NLOGEVENTS, "Loaded pending spaces and contained entities, time: %f s.", time))
    }

    // load pending entities, remove if loaded correctly only
    if (this->loadResources && !this->pendingEntities.Empty())
    {
        while (!this->pendingEntities.Empty())
        {
            nRef<nEntityObject> * refEntityObject = this->pendingEntities.Begin();
            if (refEntityObject->isvalid())
            {
                this->EntityNeedsLoading(refEntityObject->get());
            }
            this->pendingEntities.Erase(0);
        }
    }

    // load immediate entities- including batched entities just loaded
    if (!this->pendingImmediate.Empty())
    {
        this->LoadEntityArray(this->pendingImmediate);
    }

    // unload pending entities- this is done after loading pending because
    // class resources for classes that are no longer used may be cleaned
    if (!this->pendingUnload.Empty())
    {
        for (int i = 0; i < this->pendingUnload.Size(); ++i)
        {
            if (this->pendingUnload[i].isvalid())
            {
                this->UnloadEntityResources(this->pendingUnload[i].get());
                this->pendingUnload[i].invalidate();
            }
        }
        this->pendingUnload.Reset();
    }

    //unload class resources for unused classes
    if (this->unloadClassResources && !this->pendingUnloadClasses.Empty())
    {
        for (int i = 0; i < this->pendingUnloadClasses.Size(); ++i)
        {
            if (this->pendingUnloadClasses[i].isvalid())
            {
                nEntityClass* entityClass = this->pendingUnloadClasses[i];
                ncLoaderClass* loaderClass = entityClass->GetComponent<ncLoaderClass>();
                if (loaderClass && loaderClass->GetNumLoadedEntities() == 0 && 
                    loaderClass->AreResourcesValid())
                {
                    loaderClass->UnloadResources();
                    NLOG(loaderserver, (1 | NLOGUSER | NLOGEVENTS, "UNLOADED class resources (class: %s)", entityClass->GetName()));
                }
                this->pendingUnloadClasses[i].invalidate();
            }
        }
        this->pendingUnloadClasses.Clear();
    }

    #if __NEBULA_STATS__
    this->profLoaderServer.Stop();
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nLoaderServer::LoadContainerArray(nRefEntityArray& entityArray)
{
    if (!entityArray.Empty())
    {
        nRefEntityArray::iterator entityIter = entityArray.Begin();
        while (entityIter != entityArray.End())
        {
            if (!entityIter->isvalid() || this->LoadSubentities(*entityIter))
            {
                entityIter->invalidate();
                entityIter = entityArray.Erase(entityIter);
            }
            else
            {
                ++entityIter;
            }
        }

        //release array memory (potentially large)
        if (entityArray.Empty())
        {
            entityArray.Clear();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLoaderServer::LoadClassArray(nRefClassArray& classArray)
{
    int curIndex = 0;
    while (curIndex < classArray.Size())
    {
        if (!classArray[curIndex].isvalid() || this->LoadClassResources(classArray[curIndex].get()))
        {
            classArray[curIndex].invalidate();
            classArray.Erase(curIndex);
            continue;
        }
        ++curIndex;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLoaderServer::LoadEntityArray(nRefEntityArray& entityArray)
{
    int curIndex = 0;
    
    N_IFDEF_NLOG(int prevPercent = -1);
    while (curIndex < entityArray.Size())
    {
        // load class resources
        if (!this->pendingLoadClasses.Empty())
        {
            this->LoadClassArray(this->pendingLoadClasses);
        }

        N_IFDEF_NLOG(int curPercent = n_fchop(((float) curIndex / (float) entityArray.Size()) * 100));
        NLOGCOND(resource, prevPercent != curPercent, (NLOGUSER, "loading entity resources... %u%%", curPercent));
        N_IFDEF_NLOG(prevPercent = curPercent);

        if (!entityArray[curIndex].isvalid() || this->LoadEntityResources(entityArray[curIndex].get()))
        {
            entityArray[curIndex].invalidate();
        }
        ++curIndex;
    }
    entityArray.Clear();
}

//------------------------------------------------------------------------------
/**
*/
bool
nLoaderServer::LoadBatch(nEntityObject* entityObject)
{
    ncSpatialBatch* batchComp = entityObject->GetComponent<ncSpatialBatch>();
    if (batchComp)
    {
        batchComp->LoadSubentities();
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    load entities in the space- used by load areas if enabled
*/
bool
nLoaderServer::LoadSpace(nEntityObject* entityObject)
{
    ncSpatialSpace* spatialSpace = entityObject->GetComponentSafe<ncSpatialSpace>();

    //ensure first that list of contained entities is loaded
    spatialSpace->Load();

    //load quadtree spaces recursively traversing their cells
    if (spatialSpace->GetType() == ncSpatialSpace::N_QUADTREE_SPACE)
    {
        ncSpatialQuadtree* quadtree = entityObject->GetComponent<ncSpatialQuadtree>();
        this->LoadQuadtreeCell(quadtree->GetRootCell());
    }

    //load indoor spaces linearly traversing their cells
    nArray<ncSpatialCell*>& cells = spatialSpace->GetCellsArray();
    for (int i = 0; i < cells.Size(); ++i)
    {
        cells[i]->LoadEntities();
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    load entities in the cell entity- used by loaded areas if enabled.
    @note ma.garcias- this is intended for quadtree cells, indoor cells
    are not handled individually for loading.
*/
void
nLoaderServer::LoadQuadtreeCell(ncSpatialQuadtreeCell* quadtreeCell)
{
    n_assert(quadtreeCell);

    //load contained entities
    quadtreeCell->LoadEntities();

    //recursively load entities in subcells
    ncSpatialQuadtreeCell** subcells = quadtreeCell->GetSubcells();
    for (int index = 0; index < quadtreeCell->GetNumSubcells(); ++index)
    {
        this->LoadQuadtreeCell(subcells[index]);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nLoaderServer::LoadClassResources(nEntityClass* entityClass)
{
    n_assert(entityClass);
    NLOG(loaderserver, (1 | NLOGEVENTS, "nLoaderServer::LoadClassResources(class: %s)", entityClass->GetName()));

    // check whether class resources are valid
    ncLoaderClass *loaderClass = entityClass->GetComponent<ncLoaderClass>();
    if (loaderClass && !loaderClass->AreResourcesValid())
    {
        NLOG(resource, (NLOGUSER, "Loading resources for class: %s", entityClass->GetName()))
        bool success = loaderClass->LoadResources();
        NLOGCOND(loaderserver, !loaderClass->AreResourcesValid(), (1, "ERROR loading class resources (class: %s)", entityClass->GetName()));
        if (!success)
        {
            NLOG(loaderserver, (1 | NLOGUSER | NLOGEVENTS, "ERROR loading resources for class: %s", entityClass->GetName()))
            return false; //do not try to load it again if it failed
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nLoaderServer::LoadEntityResources(nEntityObject* entityObject)
{
    NLOG(loaderserver, (1 | NLOGEVENTS, "nLoaderServer::LoadEntityResources(entity: %x, class: %s)", entityObject->GetId(), entityObject->GetClass()->GetName()));

    // check whether class resources are valid-
    // NOTE ma.garcias- is LoadClassResources works find, this should never happen
    ncLoaderClass *loaderComp = entityObject->GetClassComponent<ncLoaderClass>();
    if (loaderComp && !loaderComp->AreResourcesValid())
    {
        if (!this->LoadClassResources(entityObject->GetEntityClass()))
        {
            return true; //do not try to load it again if it failed
        }
    }

    // check whether the entity has been initialized
    ncLoader* loader = entityObject->GetComponent<ncLoader>();
    if (loader && !loader->AreComponentsValid())
    {
        loader->LoadComponents();
        NLOGCOND(loaderserver, !loader->AreComponentsValid(), (1, "ERROR loading object resources (id: %x, class: %s)", entityObject->GetId(), entityObject->GetClass()->GetName()));
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nLoaderServer::UnloadEntityResources(nEntityObject* entityObject)
{
    NLOG(loaderserver, (1 | NLOGEVENTS, "nLoaderServer::UnloadEntityResources(entity: %x, class: %x)", entityObject->GetId(), entityObject->GetEntityClass()->GetName()));

    // check whether the entity has been initialized
    ncLoader* loader = entityObject->GetComponent<ncLoader>();
    if (loader && loader->AreComponentsValid())
    {
        loader->UnloadComponents();
        NLOGCOND(loaderserver, loader->AreComponentsValid(), (1, "ERROR unloading object resources (id: %x)", entityObject->GetId()));

        ncLoaderClass* loaderClass = entityObject->GetClassComponent<ncLoaderClass>();
        if (loaderClass && loaderClass->GetNumLoadedEntities() <= 0 && !loaderClass->GetRetainResources())
        {
            this->ClassResourcesUnused(entityObject->GetEntityClass());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLoaderServer::RetainClassResources(nEntityClass* entityClass)
{
    ncLoaderClass* loaderClass = entityClass->GetComponent<ncLoaderClass>();
    if (loaderClass)
    {
        loaderClass->SetRetainResources(true);
    }
}

//------------------------------------------------------------------------------
/**
    Utility method to properly unload entities, assuming
    they are contained in some space (NOT the global one)
    It saves the entity id in the spatial cell in which they are contained.
*/
void
nLoaderServer::UnloadEntitySafe(nEntityObject* entity)
{
    if (nEntityObjectServer::Instance()->GetEntityObjectType(entity->GetId()) == nEntityObjectServer::Normal)
    {
        int category = nSpatialServer::Instance()->GetCategory(entity);
        switch (category)
        {
        //categories of entities that should be kept and loaded
        case nSpatialTypes::CAT_LIGHTS:
        case nSpatialTypes::CAT_GRAPHBATCHES:
        case nSpatialTypes::CAT_SOUNDSSOURCES:
        case nSpatialTypes::CAT_WAYPOINTS:
        ///@todo ma.garcias - ncGameplayLoader crashes the second time you load it:
        case nSpatialTypes::CAT_AGENTS:
            break;

        //categories for entities that should unloaded resources only
        //case nSpatialTypes::CAT_AGENTS:
        case nSpatialTypes::CAT_WEAPONS:
        case nSpatialTypes::CAT_VEHICLES:
            nLoaderServer::Instance()->EntityNeedsUnloading(entity);
            break;

        //rest of categories for entities that should be full unloaded
        case nSpatialTypes::CAT_BRUSHES:
        default:
            //save id for entities in their batch or cell before unloading
            ncSpatial* spatialComp = entity->GetComponent<ncSpatial>();
            if (spatialComp)
            {
                if (spatialComp->GetBatch())
                {
                    spatialComp->GetBatch()->AddSubentityId(entity->GetId());
                }
                else if (spatialComp->GetCell())
                {
                    spatialComp->GetCell()->AddEntityId(entity->GetId());
                }
            }
            nEntityObjectServer::Instance()->UnloadEntityObject(entity);
        }
    }
}

//------------------------------------------------------------------------------
/**
    respond to EntityDeleted signal- 
    override in subclasses to perform additional resource handling
*/
void
nLoaderServer::EntityDeleted(int entityObjectId)
{
    NLOG(loaderserver, (1 | NLOGEVENTS, "nLoaderServer::EntityDeleted(entity: %x)", entityObjectId));

    // @todo ma.garcias remove from list of pending entities
    nEntityObject* entityObject = nEntityObjectServer::Instance()->GetEntityObject(entityObjectId);
    if (entityObject)
    {
        this->UnloadEntityResources(entityObject);
    }
}
