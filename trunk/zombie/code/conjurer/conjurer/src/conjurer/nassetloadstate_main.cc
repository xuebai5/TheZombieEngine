#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nassetloadstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "conjurer/nassetloadstate.h"
#include "conjurer/nconjurerlayout.h"
#include "conjurer/neditorviewport.h"
#include "napplication/napplication.h"
#include "napplication/nappviewport.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialspace.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialindoor.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "nscene/ncsceneclass.h"
#include "nscene/ntransformnode.h"
#include "animcomp/nccharacter.h"
#include "ndebug/ndebugserver.h"
#include "entity/nentity.h"
#include "conjurer/nconjurerapp.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/nloaderserver.h"

#ifndef NGAME
#include "nphysics/ncphysicsobj.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/nspatialoctreespacebuilder.h"
#include "conjurer/nconjurerapp.h"
#endif

nNebulaScriptClass(nAssetLoadState, "nappstate");

//------------------------------------------------------------------------------
/**
*/
nAssetLoadState::nAssetLoadState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAssetLoadState::~nAssetLoadState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAssetLoadState::OnCreate(nApplication* app)
{
    nAppState::OnCreate(app);

    this->refLayout = (nConjurerLayout*) app->Find("appviewportui");
    n_assert(this->refLayout.isvalid());
}

//------------------------------------------------------------------------------
/**
    State logic. This state run for a single frame where the scene is loaded
    into the current level.
*/
void
nAssetLoadState::OnFrame()
{
    nEntityObjectServer *entityServer = nEntityObjectServer::Instance();
    nLevel *curLevel = nLevelManager::Instance()->GetCurrentLevel();
    n_assert(curLevel);

    // get some information from the scene file parameter before loading
    if (this->GetSceneFile())
    {
        nConjurerApp* app = nConjurerApp::Instance();
        app->SetSceneFile(this->GetSceneFile());
        app->SetLevelFile("");
        app->MangleSceneFileParameter();

        this->SetLoadClassName("");
        this->SetSceneFile("");
        
        if (app->GetLevelFile())
        {
            app->LoadLevel(app->GetLevelFile());
        }
        if (app->GetLoadClassName())
        {
            this->SetLoadClassName(app->GetLoadClassName());
        }
        if (app->GetSceneFile())
        {
            this->SetSceneFile(app->GetSceneFile());
        }
    }

    // load an entity
    if (this->GetLoadClassName())
    {
        nEntityObject *newEntity = entityServer->NewEntityObject(this->GetLoadClassName());
        if (newEntity)
        {
#ifndef NGAME
            bool createOctree = false;
#endif // !NGAME
            nEntityObject *indoor = 0;
            // place a new indoor entity within the outdoor space, if it exists
            ncTransform* transformComp = newEntity->GetComponent<ncTransform>();
            if (transformComp)
            {
                nEntityObjectId entityId = curLevel->FindEntity("outdoor");
                nEntityObject* outdoor = entityServer->GetEntityObject(entityId);
                vector3 viewerPos;

                // get size of outdoor space
                if (outdoor)
                {
                    ncTerrainGMMClass *terrainComp = outdoor->GetClassComponent<ncTerrainGMMClass>();
                    if (terrainComp)
                    {
                        nFloatMap* heightMap = terrainComp->GetHeightMap();
                        float terrainSize = heightMap->GetExtent();
                        viewerPos.set(terrainSize / 2, 0, terrainSize / 2);
                    }
                }

                // place the entity
                transformComp->SetPosition(viewerPos);

                // check if it's an indoor space
                ncSpatialSpace* spaceComp = newEntity->GetComponent<ncSpatialSpace>();
                if (spaceComp && spaceComp->GetType() == ncSpatialSpace::N_INDOOR_SPACE)
                {
                    indoor = spaceComp->GetEntityObject(); 
#ifndef NGAME
                    // if we are in the 'preview' mode, create an octree space for the indoor
                    if (nConjurerApp::Instance()->GetTemporaryModeEnabled())
                    {
                        // enable octree visibility
                        nDebugServer::Instance()->SetFlagEnabled("editor", "octrees", true);
                        createOctree = true;
                    }
#endif //!NGAME
                }

                // move 3D axes to entity
                nAppViewport* viewport = this->refLayout->GetCurrentViewport();
                //nEntityObjectId axesId = curLevel->FindEntity("axes");
                //if (axesId)
                nEntityObject* axesEntity = static_cast<nEditorViewport*>(viewport)->GetAxesEntity();
                if (axesEntity)
                {
                    //nEntityObject *entityObject = entityServer->GetEntityObject(axesId);
                    //ncTransform *transformComp = entityObject->GetComponent<ncTransform>();
                    ncTransform *transformComp = axesEntity->GetComponent<ncTransform>();
                    transformComp->SetPosition(viewerPos);
                }

                // set viewer inside the indoor
                matrix44 viewMatrix(viewport->GetViewMatrix());
                viewMatrix.set_translation(viewerPos);
                matrix44 tmpMatrix;
                tmpMatrix.translate(vector3(0.0f, 0.0f, 2.0f));// backup 2 units
                tmpMatrix = tmpMatrix * viewMatrix;
                this->refLayout->SetDefViewerPos(tmpMatrix.pos_component());
                this->refLayout->Reset();
            }

            // demand resource loading
            nLoaderServer::Instance()->EntityNeedsLoading(newEntity);

#ifndef NGAME
            if (createOctree)
            {
                n_assert(indoor);
                nSpatialOctreeSpaceBuilder octreeBuilder;
                octreeBuilder.BuildOctreeSpace(indoor->GetComponentSafe<ncSpatialIndoor>());
                nSpatialServer::Instance()->RegisterSpace(octreeBuilder.GetOctreeSpace());
            }
#endif // !NGAME

            // set default animation state
            ncCharacter *charComp = newEntity->GetComponent<ncCharacter>();
            if (charComp)
            {
                charComp->SetActiveStateByIndex(this->GetAnimState());
            }

#ifndef NGAME
            // Inserting the object in the physics space
            ncPhysicsObj* phyObject(newEntity->GetComponent<ncPhysicsObj>());
            if( phyObject )
            {
                phyObject->AutoInsertInSpace();
            }
            
            // Inserting the object in the spatial space
            ncSpatial *spatialComp = newEntity->GetComponent<ncSpatial>();
            if (spatialComp)
            {
                nSpatialServer::Instance()->InsertEntity(newEntity);
            }
            else
            {
                nSpatialServer::Instance()->InsertGlobalEntity(newEntity);
            }
#endif // !NGAME

            /// @todo ma.garcias remove the previous entity
            //if (this->refEntity.isvalid())
            //{
            //    entityServer->RemoveEntityObject(this->refEntity.get());
            //    n_assert(!this->refEntity.isvalid());
            //}
            //this->refEntity = newEntity;
        }
    }
    //<OBSOLETE> keep just for backwards compatibility, for graphics only
    else if (this->GetSceneFile())
    {
        // initialize the entity
        if (this->refEntity.isvalid())
        {
            //...
            //entityServer->RemoveEntityObject(this->refEntity.get());
            nLoaderServer::Instance()->EntityNeedsUnloading(this->refEntity);
        }
        else
        {
            // temporarily, all objects will be loaded as if they were just graphical
            nEntityClass *baseClass = nEntityClassServer::Instance()->GetEntityClass("nescene");
            nEntityClass *newClass = nEntityClassServer::Instance()->NewEntityClass(baseClass, "Conjurer_objects");
            
            this->refEntity = entityServer->NewLocalEntityObject("Conjurer_objects");
            n_assert(this->refEntity.isvalid());

            // FIXME ensure that this class and object is not persisted
            nEntityClassServer::Instance()->SetEntityClassDirty(newClass, false);
            entityServer->SetEntityObjectDirty(this->refEntity.get(), false);
        }

        // set the resource file for the loader class
        ncLoaderClass *loaderclass = this->refEntity->GetClassComponent<ncLoaderClass>();
        loaderclass->SetResourceFile(this->GetSceneFile());

        /// @todo ma.garcias try loading it first, if it wasn't a proper scene file, delete and ignore.
        nAutoRef<nSceneNode> refSceneRoot("/usr/scene");
        kernelServer->PushCwd(refSceneRoot.get());
        nObject* obj = kernelServer->Load(this->GetSceneFile());
        kernelServer->PopCwd();

        if (obj && obj->IsA("nscenenode"))
        {
            ncSceneClass* sceneClass = this->refEntity->GetClassComponent<ncSceneClass>();
            sceneClass->SetRootNode(static_cast<nSceneNode*>(obj));

            nLoaderServer::Instance()->EntityNeedsLoading(this->refEntity);
        }
    }
    //</OBSOLETE>

    if (this->loadNextState.IsEmpty())
    {
        this->app->SetState("editor");
    }
    else
    {
        this->app->SetState(this->loadNextState);
    }
}
