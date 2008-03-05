#include "precompiled/pchnspecialfx.h"
//------------------------------------------------------------------------------
//  nfxspawn_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nspecialfx/nfxspawn.h"
#include "nspecialfx/ncspecialfxclass.h"
#include "entity/nentityclassserver.h"
#include "entity/nentityobjectserver.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/ncdictionary.h"
#include "nspatial/ncspatialcell.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialspace.h"
#include "nscene/ncscene.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/ncloader.h"
#include "zombieentity/nloaderserver.h"
#include "util/nrandomlogic.h"

nNebulaScriptClass(nFXSpawn, "nfxobject");

//------------------------------------------------------------------------------
/**
*/
nFXSpawn::nFXSpawn() :
    global(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nFXSpawn::~nFXSpawn()
{
    if (this->refSpawnedEntity.isvalid())
    {
        nEntityObjectServer::Instance()->RemoveEntityObject(this->refSpawnedEntity);
        this->refSpawnedEntity.invalidate();
    }

    if (this->refSpawnClass.isvalid())
    {
        //decrement entity count to make class resources unload
        ncLoaderClass* loaderClass = this->refSpawnClass->GetComponentSafe<ncLoaderClass>();
        loaderClass->EntityUnloaded(0);
        this->refSpawnClass.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
    Preload class resources
*/
bool
nFXSpawn::Load()
{
    n_assert(!this->IsLoaded());
    if (!this->GetFXClass())
    {
        return false;
    }
    
    //preload class resources
    this->refSpawnClass = nEntityClassServer::Instance()->GetEntityClass(this->GetFXClass());
    n_assert(this->refSpawnClass.isvalid());
    ncLoaderClass* loaderClass = this->refSpawnClass->GetComponent<ncLoaderClass>();
    if (loaderClass)
    {
        loaderClass->EntityLoaded(0);
        if (!loaderClass->AreResourcesValid())
        {
            loaderClass->LoadResources();
        }
    }

    ncSpecialFXClass* fxClass = this->refSpawnClass->GetComponent<ncSpecialFXClass>();
    if (fxClass)
    {
        n_assert(fxClass->GetType() == ncSpecialFXClass::FX_SPAWN);
        if (fxClass->GetType() != ncSpecialFXClass::FX_SPAWN)
        {
            return false;
        }

        this->SetUnique(fxClass->GetUnique());
        this->SetLifeTime(fxClass->GetLifeTime());
        this->SetFXGlobal(fxClass->GetFXGlobal());
        this->SetStartRandom(fxClass->GetStartRandom());
    }

    return nFXObject::Load();
}

//------------------------------------------------------------------------------
/**
*/
void
nFXSpawn::Trigger(nTime curTime)
{
    nFXObject::Trigger(curTime);

    if (this->refSpawnedEntity.isvalid())
    {
        if (this->IsAlive())
        {
            vector3 pos;
            quaternion quat;

            /** @todo ma.garcias- this should be turned into a general behavior for spawned
                effects to keep updated when these transforms are connected */

            //<HACK> to keep the effect synchronized with the entity where this is plugged
            //in case it is also attached to an skinned entity
            //(ie. muzzle flash plugged to weapon attached to first-person scene)
            if (this->parentEntity.isvalid())
            {
                ncScene* parentScene = this->parentEntity->GetComponentSafe<ncScene>();
                if (parentScene->GetParentEntity())
                {
                    parentScene->SetTime(curTime);
                    parentScene->UpdateParentTransform();
                }

                parentScene->GetWorldPlugData(nString("front"), pos, quat);
            }
            else
            //</HACK>
            {
                const transform44& transform = this->GetTransform();
                pos = transform.gettranslation();
                quat = transform.getquatrotation();
            }

            ncTransform* transformComp = this->refSpawnedEntity->GetComponent<ncTransform>();
            if (transformComp)
            {
                transformComp->SetPosition(pos);
                transformComp->SetQuat(quat);
            }
        }
    }
    else
    {
        this->alive = false; //set as dead, the FXServer returns it to the pool
    }
}

//------------------------------------------------------------------------------
/**
    Create or restart the entity, insert into its global space
*/
void
nFXSpawn::Restart()
{
    if (this->refSpawnedEntity.isvalid())
    {
        //force reinitialization of entity resources
        ncLoader* loader = this->refSpawnedEntity->GetComponent<ncLoader>();
        if (loader && loader->AreComponentsValid())
        {
            loader->UnloadComponents();
        }
        nLoaderServer::Instance()->EntityNeedsLoading(this->refSpawnedEntity);
    }
    else
    {
        //spawn entity first time it is triggered
        this->refSpawnedEntity = nEntityObjectServer::Instance()->NewLocalEntityObject(this->GetFXClass());
        if (!this->refSpawnedEntity.isvalid())
        {
            return;
        }
    }

    if (this->refSpawnedEntity.isvalid())
    {
        ncSpatial* spatialComp = this->refSpawnedEntity->GetComponent<ncSpatial>();
        if (spatialComp && this->GetFXGlobal() && 
            (nSpatialServer::Instance()->GetGlobalAlwaysVisibleEntities().FindIndex(this->refSpawnedEntity.get()) == -1))
        {
            //insert as global entity, always visible
            nSpatialServer::Instance()->InsertGlobalEntity(this->refSpawnedEntity.get(), true);
        }

        //HACK- reset time counter for animators and particle systems
        //TODO- allow customizing how to properly restart the effect
        ncDictionary* varContext = this->refSpawnedEntity->GetComponent<ncDictionary>();
        if (varContext)
        {
            float randomOffset = 0;
            if (this->GetStartRandom())
            {
                randomOffset = n_rand_real_in_range(0,1.f);
            }

            varContext->SetLocalFloatVariable("timeOffset", (float) this->aliveTime - randomOffset);
        }
    }
}

//------------------------------------------------------------------------------
/**
    remove dead effect from spatial server
*/
void
nFXSpawn::Stop()
{
    if (this->refSpawnedEntity.isvalid())
    {
        //unload the entity
        ncLoader* loader = this->refSpawnedEntity->GetComponent<ncLoader>();
        if (loader && loader->AreComponentsValid())
        {
            loader->UnloadComponents();
        }

        //remove from its containing space (make it invisible)
        ncSpatial* spatialComp = this->refSpawnedEntity->GetComponent<ncSpatial>();
        if (spatialComp && spatialComp->GetSpace())
        {
            spatialComp->GetSpace()->RemoveEntity(this->refSpawnedEntity.get());
        }
        else
        {
            nSpatialServer::Instance()->RemoveGlobalEntity(this->refSpawnedEntity.get());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nFXSpawn)
NSCRIPT_INITCMDS_END()
