#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ncloader_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/ncloader.h"
#include "zombieentity/ncsuperentity.h"
#include "zombieentity/nloaderserver.h"

#include "nscene/ncscene.h"
#include "nscene/nscenenode.h"
#include "nscene/ncscenebatch.h"
#include "animcomp/nccharacter.h"
#include "animcomp/nccharacterclass.h"
#include "animcomp/ncskeletonclass.h"
#include "nspatial/ncspatialspace.h"
#include "nspatial/nspatialserver.h"

#include "nphysics/ncphyindoor.h"
#include "nphysics/ncphyvehicle.h"
#include "nphysics/ncphyvehicleclass.h"
#include "nphysics/ncphywheel.h"

#include "nspatial/ncspatial.h"
#include "nspatial/ncspatiallightenv.h"
#include "nspatial/ncspatialportal.h"
#include "nspatial/ncspatialindoorclass.h"
#include "nspatial/nspatialindoorcell.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialquadtreecell.h"
#include "entity/nobjectinstancer.h"
#include "entity/nentityobjectserver.h"
#include "kernel/nlogclass.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#include "ndebug/nceditorclass.h"
#endif

//------------------------------------------------------------------------------
nNebulaComponentObject(ncLoader, nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncLoader)
    #ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('MSRL', void, SetReload, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGRL', bool, GetReload, 0, (), 0, ());
    #endif
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
NCREATELOGLEVEL(ncloader, "Entity Resource Load", false, 2)
// log levels:
// 0 - general per entity info on loading, unloading
// 1 - detailed per entity info: space cells and portals

//------------------------------------------------------------------------------
/**
    constructor
*/
ncLoader::ncLoader() :
    jobNode(this),
    isCharacterValid(true),
    areComponentsValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncLoader::~ncLoader()
{
    if (this->areComponentsValid)
    {
        //FIXME this doesn't work because GetComponent returns 0 for all components
        this->UnloadComponents();
    }
}

//------------------------------------------------------------------------------
/**
    Initialize all components requiring resources to be loaded.

    history:
        - 22-Mar-2006   ma.garcias  check valid character loaded.
*/
void
ncLoader::InitInstance(nObject::InitInstanceMsg initType)
{
    #ifndef __ZOMBIE_EXPORTER__
    if (initType != nObject::ReloadedInstance)
    {
        // register for resource loading
        nLoaderServer::Instance()->EntityCreated(this->GetEntityObject(), initType);
    }
    #endif

    if (!this->GetEntityClass()->IsUserCreated())
    {
        // the native class has not a resource
        return; 
    }

    if (initType != nObject::ReloadedInstance)
    {
        nString spatialPath;
        ncSpatialSpace *spaceComp = this->entityObject->GetComponent<ncSpatialSpace>();
        if (spaceComp)
        {
            if (spaceComp->GetType() == ncSpatialSpace::N_INDOOR_SPACE)
            {
                ncSpatialIndoor * indoorSpace = spaceComp->GetComponentSafe<ncSpatialIndoor>();
                this->InitIndoorSpace(indoorSpace);

                // connect indoor portals
                spaceComp->SolvePortalsPointers();

                ncTransform *transfComp = this->GetComponentSafe<ncTransform>();
                spaceComp->Update(transfComp->GetTransform());
            }
            else if (spaceComp->GetType() == ncSpatialSpace::N_QUADTREE_SPACE)
            {
                // check the outdoor class resources are loaded
                ncLoaderClass * loaderClassComponent = this->GetClassComponent<ncLoaderClass>();
                n_assert(loaderClassComponent);
                loaderClassComponent->LoadResources();
            }
        }

        // load character component
        ncCharacter *charComp = this->entityObject->GetComponent<ncCharacter>();
        if (charComp && !charComp->IsValid())
        {
            this->isCharacterValid = charComp->Load();
            NLOGCOND(ncloader, !this->isCharacterValid,
                     (NLOGUSER, "Invalid animation set for character class: %s\n", this->GetEntityClass()->GetName()))
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncLoader::LoadComponents()
{
    // load space entities
    ncSpatialSpace *spaceComp = this->entityObject->GetComponent<ncSpatialSpace>();
    if (spaceComp)
    {
        if (spaceComp->GetType() == ncSpatialSpace::N_INDOOR_SPACE)
        {
            if (!this->LoadIndoorSpace(static_cast<ncSpatialIndoor*>(spaceComp)))
            {
                n_message("ncLoader::LoadIndoorSpace: failed loading of indoor space entity");
                return false;
            }
        }
        else if (spaceComp->GetType() == ncSpatialSpace::N_QUADTREE_SPACE)
        {
            if (!this->LoadOutdoorSpace(spaceComp))
            {
                n_message("ncLoader::LoadOutdoorSpace: failed loading of outdoor space entity");
                return false;
            }
        }
    }

    // load quadtree cell component
    ncSpatialQuadtreeCell *quadtreeCell = this->entityObject->GetComponent<ncSpatialQuadtreeCell>();
    if (quadtreeCell)
    {
        // recursively load resources for subcells of the current one
        int numSubCells = quadtreeCell->GetNumSubcells();
        if (numSubCells > 0)
        {
            ncSpatialQuadtreeCell** subCells = quadtreeCell->GetSubcells();
            n_assert(subCells);
            for (int i = 0; i < numSubCells; ++i)
            {
                if (subCells[i]->GetEntityObject())
                {
                    nLoaderServer::Instance()->EntityNeedsLoading(subCells[i]->GetEntityObject());
                }
            }
        }
    }

    // load graphic component
    ncScene *sceneComp = this->entityObject->GetComponent<ncScene>();
    if (sceneComp && !sceneComp->IsValid())
    {
        // if the character component could not be initialized, do not load its scene
        // or it will crash
        ncCharacter *charComp = this->entityObject->GetComponent<ncCharacter>();

        if (!charComp || this->isCharacterValid)
        {
            this->GetEntityObject()->LockMutex();
            bool success = sceneComp->Load();
            this->GetEntityObject()->UnlockMutex();

            if (!success)
            {
                return false;
            }
        }
    }

    // load light environment component
    ncSpatialLightEnv* lightEnv = this->entityObject->GetComponent<ncSpatialLightEnv>();
    if (lightEnv)// && !lightEnv->IsValid())
    {
        if (!lightEnv->Load())
        {
            return false;
        }
    }

    ncPhysicsObj* physicsObj(this->GetComponent<ncPhysicsObj>());

    if(physicsObj)
    {
        #ifndef NGAME
        if(physicsObj->GetType() != ncPhysicsObj::Pickeable )
        {
            //HACK- initialize a brush entity that was persisted as a mirage
            if (!physicsObj->IsLoaded())
            {
                ncTransform* transform = this->GetComponentSafe<ncTransform>();
                vector3 phypos = transform->GetPosition();
                vector3 euler = transform->GetEuler();
                physicsObj->PreparePhyObject();//load the chunk, this overrides the transform
                physicsObj->SetPositionPhyObj(phypos);            
                physicsObj->SetOrientation(phyreal(euler.x), phyreal(euler.y), phyreal(euler.z));
                physicsObj->InsertInTheWorld();
                nEntityObjectServer::Instance()->SetEntityObjectDirty(this->GetEntityObject(), true);
            }
        }
        #endif
        physicsObj->Load();

        // load wheels for vehicles
        ncPhyVehicle* vehicle = this->GetComponent<ncPhyVehicle>();
        if (vehicle)
        {
            int numWheels = this->GetClassComponentSafe<ncPhyVehicleClass>()->GetNumWheels();
            for (int i = 0; i < numWheels; ++i)
            {
                ncPhyVehicle::Wheel* wheel = (*vehicle)[i];
                nLoaderServer::Instance()->EntityNeedsLoading(wheel->GetWheelObject()->GetEntityObject());
            }

            // load steering wheel
            nEntityObject* steeringWheel = vehicle->GetSteeringWheel();
            if (steeringWheel)
            {
                nLoaderServer::Instance()->EntityNeedsLoading(steeringWheel);
            }
        }
    }

    // load subentities loader components
    ncSuperentity* superComp = this->entityObject->GetComponent<ncSuperentity>();
    if (superComp)
    {
        superComp->LoadSubentitiesLoaderComponents();
    }

    // signal the class that resources for some entity have been loaded
    ncLoaderClass* loaderClass = this->GetEntityClass()->GetComponent<ncLoaderClass>();
    if (loaderClass && !this->areComponentsValid)
    {
        loaderClass->EntityLoaded(this->GetEntityObject());
    }

    this->areComponentsValid = true;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncLoader::UnloadComponents()
{
    // unload space entities
    ncSpatialSpace *spaceComp = this->entityObject->GetComponent<ncSpatialSpace>();
    if (spaceComp)
    {
        if (spaceComp->GetType() == ncSpatialSpace::N_INDOOR_SPACE)
        {
            this->UnloadIndoorSpace(static_cast<ncSpatialIndoor*>(spaceComp));
        }
        else if (spaceComp->GetType() == ncSpatialSpace::N_QUADTREE_SPACE)
        {
            this->UnloadOutdoorSpace(spaceComp);
        }
    }

    // load quadtree cell component
    ncSpatialQuadtreeCell *quadtreeCell = this->entityObject->GetComponent<ncSpatialQuadtreeCell>();
    if (quadtreeCell)
    {
        // recursively unload resources for subcells of the current one
        int numSubCells = quadtreeCell->GetNumSubcells();
        if (numSubCells > 0)
        {
            ncSpatialQuadtreeCell** subCells = quadtreeCell->GetSubcells();
            n_assert(subCells);
            
            for (int i = 0; i < numSubCells; ++i)
            {
                nLoaderServer::Instance()->EntityNeedsUnloading(subCells[i]->GetEntityObject());
            }
        }
    }

    // load light environment component
    ncSpatialLightEnv* lightEnv = this->entityObject->GetComponent<ncSpatialLightEnv>();
    if (lightEnv)
    {
        lightEnv->Unload();
    }

    // unload scene component
    ncScene *sceneComp = this->entityObject->GetComponent<ncScene>();
    if (sceneComp && sceneComp->IsValid())
    {
        sceneComp->Unload();
    }

    // unload character component (FIXME: make character derive from scene)
    ncCharacter *charComp = this->entityObject->GetComponent<ncCharacter>();
    if (charComp)
    {
        charComp->Unload();  
    }

    // signal the class that resources for some entity have been unloaded
    ncLoaderClass* loaderClass = this->GetEntityClass()->GetComponent<ncLoaderClass>();
    if (loaderClass && this->areComponentsValid)
    {
        loaderClass->EntityUnloaded(this->GetEntityObject());
    }

    ncPhysicsObj* physicsObj(this->entityObject->GetComponent<ncPhysicsObj>());
    if (physicsObj)
    {
        // unload wheels for vehicles
        ncPhyVehicle* vehicle = this->entityObject->GetComponent<ncPhyVehicle>();
        if (vehicle)
        {
            int numWheels = this->entityObject->GetClassComponentSafe<ncPhyVehicleClass>()->GetNumWheels();
            for (int i = 0; i < numWheels; ++i)
            {
                ncPhyVehicle::Wheel* wheel = (*vehicle)[i];
                if (wheel && wheel->GetWheelObject())
                {
                    nLoaderServer::Instance()->EntityNeedsUnloading(wheel->GetWheelObject()->GetEntityObject());
                }
            }

            // unload steering wheel
            nEntityObject* steeringWheel = vehicle->GetSteeringWheel();
            if (steeringWheel)
            {
                nLoaderServer::Instance()->EntityNeedsUnloading(steeringWheel);
            }
        }
    }

    this->areComponentsValid = false;
}

//------------------------------------------------------------------------------
/**
    Load indoor space component from asset
*/
bool
ncLoader::InitIndoorSpace(ncSpatialIndoor *indoorSpace)
{
    nString spatialPath;

    NLOG(ncloader, (NLOGUSER, "Loading indoor space: %s", indoorSpace->GetEntityClass()->GetName()))
    NLOG(ncloader, (0, "ncLoader::InitIndoorSpace(entity: %x)", indoorSpace->GetEntityObject()->GetId()))

    // load the cells structure
    ncLoaderClass * loaderClass = this->GetClassComponent<ncLoaderClass>();
    spatialPath = loaderClass->GetResourceFile();
    spatialPath.Append("/spatial/");

    if (!indoorSpace->LoadCellsStructure(spatialPath.Get()))
    {
        return false;
    }

    // register space
    nSpatialServer::Instance()->RegisterSpace(indoorSpace);

    // run the instancers for entities and portals
    nString fileName;
    nObjectInstancer *instancer;
    nSpatialIndoorCell *indoorCell = 0;
    n_verify2(indoorSpace->GetClassComponent<ncSpatialIndoorClass>() != 0,
              "miquelangel.rujula: entity that has indoor space component hasn't its corresponding class component!");

    #ifndef NGAME
    ncEditor *spaceEditorComp = indoorSpace->GetComponent<ncEditor>();
    n_assert2(spaceEditorComp, 
              "miquelangel.rujula: indoor space hasn't editor component!");
    int indoorLayerId = spaceEditorComp->GetEditorKeyInt("layerId");
    #endif // !NGAME

    ncTransform *indoorTransform = this->GetComponent<ncTransform>();

    N_IFDEF_NLOG(int numBrushes = 0);
    N_IFDEF_NLOG(int numPortals = 0);
    bbox3 indoorBox;
    indoorBox.begin_extend();
    for (nArray<ncSpatialCell*>::iterator pCell = indoorSpace->GetCellsArray().Begin();
                                          pCell != indoorSpace->GetCellsArray().End();
                                          pCell++)
    {
        bbox3 cellBox;
        cellBox.begin_extend();

        indoorCell = reinterpret_cast<nSpatialIndoorCell*>((*pCell));
        NLOG(ncloader, (1, "loading indoor cell id: %d", indoorCell->GetId()))

        // load indoor brushes instancer for the current cell
        fileName.Set(spatialPath.Get());
        fileName.Append("IBInstancer_");
        fileName.AppendInt(indoorCell->GetId());
        fileName.Append(".n2");

        instancer = 0;
        instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->Load(fileName.Get()));
        if (instancer)
        {
            nEntityObject *indoorBrush = 0;
            ncSpatial *spatialComp = 0;
            for (int i = 0; i < instancer->Size(); i++)
            {
                indoorBrush = static_cast<nEntityObject*>(instancer->At(i));

                #ifndef NGAME
                ncEditor *editorComp = indoorBrush->GetComponent<ncEditor>();
                n_assert2(editorComp, "miquelangel.rujula: indoor brush has no editor component!");
                editorComp->SetLayerId(indoorLayerId);
                #endif //!NGAME

                spatialComp = indoorBrush->GetComponent<ncSpatial>();
                n_assert2(spatialComp, "miquelangel.rujula: trying to load an indoor brush without spatial component!");
                spatialComp->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);
                indoorCell->AddEntity(indoorBrush);
                cellBox.extend(indoorBrush->GetComponentSafe<ncSpatial>()->GetBBox());
            }
            NLOG(ncloader, (1, "%d indoor brushes instanced.", instancer->Size()))
            N_IFDEF_NLOG(numBrushes += instancer->Size());
            instancer->Release();
        }

        // load portals instancer for the current cell
        fileName.Set(spatialPath.Get());
        fileName.Append("PortalsInstancer_");
        fileName.AppendInt(indoorCell->GetId());
        fileName.Append(".n2");

        instancer = 0;
        instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->Load(fileName.Get()));
        if (instancer)
        {
            nEntityObject *entity = 0;
            ncSpatialPortal *portalComp = 0;
            for (int i = 0; i < instancer->Size(); i++)
            {
                entity = static_cast<nEntityObject*>(instancer->At(i));
                indoorCell->AddEntity(entity);
                n_assert2(entity->GetClass()->IsA("neportal"), 
                        "miquelangel.rujula: portal instancer has returned an entity that isn't a portal!");

                #ifndef NGAME
                entity->GetComponentSafe<ncEditor>()->SetLayerId(indoorLayerId);
                #endif //!NGAME

                entity->GetComponentSafe<ncSpatial>()->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);
                portalComp = entity->GetComponentSafe<ncSpatialPortal>();
                indoorSpace->AddUnsolvedPortal(portalComp);
                cellBox.extend(portalComp->GetBBox());
            }
            NLOG(ncloader, (1, "%d indoor portals instanced.", instancer->Size()))
            N_IFDEF_NLOG(numPortals += instancer->Size());
            instancer->Release();
        }

        indoorCell->SetOriginalBBox(cellBox);
        indoorBox.extend(cellBox);
        
        //#ifndef NGAME
        // load the wrapper mesh
        nString wrapperPath;
        wrapperPath.Format("%s/wrapperMesh%d.n3d2", spatialPath.Get(), indoorCell->GetId());
        nWrapperMesh& wrapperMesh = indoorCell->GetWrapperMesh();
        wrapperMesh.SetMeshFile(wrapperPath.Get());
        wrapperMesh.LoadResources();

        n_assert2(indoorTransform, "miquelangel.rujula: indoor entity has no transform component!");
        wrapperMesh.SetTransform(indoorTransform->GetTransform());
        //#endif //!NGAME
    }

    indoorSpace->SetOriginalBBox(indoorBox);

    NLOG(ncloader, (NLOGUSER, "%d indoor entities loaded.", numBrushes))
    NLOG(ncloader, (NLOGUSER, "%d indoor portals loaded.", numPortals))

    // load instancer for the outdoor portals of this indoor
    fileName.Set(spatialPath.Get());
    fileName.Append("PortalsInstancer_0.n2");

    instancer = 0;
    instancer = static_cast<nObjectInstancer*>(nKernelServer::Instance()->Load(fileName.Get()));
    if (instancer)
    {
        nEntityObject *entity = 0;
        ncSpatialPortal *portalComp = 0;
        for (int i = 0; i < instancer->Size(); i++)
        {
            entity = static_cast<nEntityObject*>(instancer->At(i));
            portalComp = entity->GetComponentSafe<ncSpatialPortal>();
            portalComp->UpdateBBox(indoorTransform->GetTransform());
            n_assert2(entity->GetClass()->IsA("neportal"), 
                    "miquelangel.rujula: portal instancer has returned an entity that isn't a portal!");

            #ifndef NGAME
            entity->GetComponentSafe<ncEditor>()->SetLayerId(indoorLayerId);
            #endif //!NGAME

            entity->GetComponentSafe<ncSpatial>()->SetDetermineSpaceFlag(ncSpatial::DS_FALSE);
            indoorSpace->AddUnsolvedPortal(entity->GetComponentSafe<ncSpatialPortal>());
        }
        NLOG(ncloader, (1, "%d indoor portals instanced.", instancer->Size()))
        NLOG(ncloader, (NLOGUSER, "%d outdoor portals loaded.", instancer->Size()))
        instancer->Release();
    }

    #ifndef NGAME
    nEntityObject *facade = indoorSpace->GetFacade();
    if (facade)
    {
        facade->GetComponentSafe<ncEditor>()->SetLayerId(indoorLayerId);
    }
    #endif //!NGAME

    // load physics component for indoor
    ncPhyIndoor* phyIndoor(this->GetComponent<ncPhyIndoor>());
    if (phyIndoor)
    {
        nString fileName;
        phyIndoor->BeginRunInstancier();
        for (nArray<ncSpatialCell*>::iterator pCell  = indoorSpace->GetCellsArray().Begin();
                                              pCell != indoorSpace->GetCellsArray().End();
                                              pCell++)
        {
            // load physics objects instancer correponding to the current cell
            fileName.Set(spatialPath.Get());
            fileName.Append("PhysicsInstancer_");
            fileName.AppendInt((*pCell)->GetId());
            fileName.Append(".n2");
            phyIndoor->RunInstancier(fileName.Get());
        }
        phyIndoor->EndRunInstancier();

        NLOG(ncloader, (1, "indoor physics object instanced."))
        NLOG(ncloader, (NLOGUSER, "%d indoor physics objects loaded.", phyIndoor->GetNumGeometries()))
    }

    NLOG(ncloader, (0, "ncLoader::InitIndoorSpace finished."))

    return true;
}

//------------------------------------------------------------------------------
/**
    Load resources for indoor space component
*/
bool
ncLoader::LoadIndoorSpace(ncSpatialIndoor *indoorSpace)
{
    nString spatialPath;

    NLOG(ncloader, (0, "ncLoader::LoadIndoorSpace(entity: %x)", indoorSpace->GetEntityObject()->GetId()))

    // load the cells structure
    ncLoaderClass* loaderClass = this->GetClassComponent<ncLoaderClass>();
    spatialPath.Format("%s/spatial", loaderClass->GetResourceFile());

    nLoaderServer* loaderServer = nLoaderServer::Instance();

    // load resources for own indoor brushes and portals
    nArray<ncSpatialCell*>::iterator cellIter = indoorSpace->GetCellsArray().Begin();
    while (cellIter != indoorSpace->GetCellsArray().End())
    {
        nSpatialIndoorCell *indoorCell = static_cast<nSpatialIndoorCell*>(*cellIter);
        NLOG(ncloader, (1, "loading indoor cell id: %d", indoorCell->GetId()))

        // load indoor brushes
        const nArray<nEntityObject*> &indoorBrushes = indoorCell->GetCategory(nSpatialTypes::CAT_INDOOR_BRUSHES);
        for (int i = 0; i < indoorBrushes.Size(); i++)
        {
            loaderServer->EntityNeedsLoading(indoorBrushes[i]);
        }

        // load indoor portals
        const nArray<nEntityObject*> &portals = indoorCell->GetCategory(nSpatialTypes::CAT_PORTALS);
        for (int i = 0; i < portals.Size(); i++)
        {
            // backwards compatibility: ensure that all portals are connected
            if (!portals[i]->GetComponentSafe<ncSpatialPortal>()->GetTwinPortal())
            {
                nSpatialServer::Instance()->ConnectSpace(indoorSpace);
            }

            loaderServer->EntityNeedsLoading(portals[i]);
            // do not handle the twin portal, it's up to the other space / cell
        }

        ++cellIter;
    }

    // do not handle the facade- it's up to the outdoor space

    // load entities contained in this indoor
    bool success = indoorSpace->Load();

    n_assert2(success, "miquelangel.rujula: can't load entities contained in an indoor!");

    NLOG(ncloader, (0, "ncLoader::LoadIndoorSpace finished."))

    return success;
}

//------------------------------------------------------------------------------
/**
    Unload resources for indoor space component.
*/
void
ncLoader::UnloadIndoorSpace(ncSpatialIndoor *indoorSpace)
{
    NLOG(ncloader, (0, "ncLoader::UnloadIndoorSpace(entity: %x, class: %s)", indoorSpace->GetEntityObject()->GetId(), indoorSpace->GetEntityClass()->GetName()))
    
    nLoaderServer* loaderServer = nLoaderServer::Instance();

    nArray<ncSpatialCell*>::iterator cellIter = indoorSpace->GetCellsArray().Begin();
    while (cellIter != indoorSpace->GetCellsArray().End())
    {
        nSpatialIndoorCell *indoorCell = static_cast<nSpatialIndoorCell*>(*cellIter);
        NLOG(ncloader, (1, "unloading indoor cell id: %d", indoorCell->GetId()))

        // unload brush entities
        const nArray<nEntityObject*> &indoorBrushes = indoorCell->GetCategory(nSpatialTypes::CAT_INDOOR_BRUSHES);
        for (int i = 0; i < indoorBrushes.Size(); i++)
        {
            loaderServer->EntityNeedsUnloading(indoorBrushes[i]);
        }

        // unload portal entities
        const nArray<nEntityObject*> &portals = indoorCell->GetCategory(nSpatialTypes::CAT_PORTALS);
        for (int i = 0; i < portals.Size(); i++)
        {
            loaderServer->EntityNeedsUnloading(portals[i]);
            // do not unload the twin portal, it's up to the other space / cell
        }

        ++cellIter;
    }
}

//------------------------------------------------------------------------------
/**
    Load resources for outdoor space component
*/
bool
ncLoader::LoadOutdoorSpace(ncSpatialSpace *spaceComp)
{
    ncSpatialQuadtree* quadtree = spaceComp->GetComponentSafe<ncSpatialQuadtree>();
    ncSpatialQuadtreeCell *quadtreeCell = quadtree->GetRootCell();
    n_assert(quadtreeCell);
    NLOG(ncloader, (0, "ncLoader::LoadOutdoorSpace(entity: %x)", spaceComp->GetEntityObject()->GetId()))

    // load resources for all quadtree cells for the root
    nLoaderServer::Instance()->EntityNeedsLoading(quadtreeCell->GetEntityObject());

    // load list of entities contained in this outdoor
    bool success = spaceComp->Load();

    n_assert2(success, "miquelangel.rujula: can't load entities in the outdoor!");
    NLOG(ncloader, (0, "ncLoader::LoadOutdoorSpace finished."))

    return success;
}

//------------------------------------------------------------------------------
/**
    Unload resources for outdoor space component.
*/
void
ncLoader::UnloadOutdoorSpace(ncSpatialSpace *spaceComp)
{
    NLOG(ncloader, (0, "ncLoader::UnloadOutdoorSpace(entity: %x)", spaceComp->GetEntityObject()->GetId()))

    // unload resources for all quadtree cells under the root
    ncSpatialQuadtree* quadtree = static_cast<ncSpatialQuadtree*>(spaceComp);
    ncSpatialQuadtreeCell *quadtreeCell = quadtree->GetRootCell();
    if (quadtreeCell)
    {
        nLoaderServer::Instance()->EntityNeedsUnloading(quadtreeCell->GetEntityObject());
    }

    NLOG(ncloader, (0, "ncLoader::UnloadOutdoorSpace finished."))
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
void
ncLoader::SetReload(bool /*dummy*/)
{
    //force reinitialization of entity resources
    if (this->AreComponentsValid())
    {
        this->UnloadComponents();
        nLoaderServer::Instance()->EntityNeedsLoading(this->entityObject);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncLoader::GetReload() const
{
    return true;
}
#endif
