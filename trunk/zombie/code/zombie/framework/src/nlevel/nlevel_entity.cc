#include "precompiled/pchframework.h"
//------------------------------------------------------------------------------
//  nlevel_entity.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nlevel/nlevel.h"
#include "entity/nentity.h"
#include "entity/nentityobjectserver.h"
#include "zombieentity/ncloader.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/nloaderserver.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialindoor.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatial.h"
#include "nscene/ncscene.h"

#ifndef __ZOMBIE_EXPORTER__
#ifndef NGAME
#include "ngeomipmap/ncterraingmmclass.h"
#include "ngeomipmap/ncterrainmaterialclass.h"
#include "ncnavmesh/ncnavmesh.h"
#include "nvegetation/ncterrainvegetationclass.h"
#endif
#endif

//------------------------------------------------------------------------------
/**
*/
void
nLevel::SetSpaceEntity(nEntityObjectId entityId)
{
    if (entityId != 0 && !this->spaceObjects.Find(entityId))
    {
        this->spaceObjects.Append(entityId);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLevel::SetGlobalEntity(nEntityObjectId entityId)
{
    if (entityId != 0 && !this->globalObjects.Find(entityId))
    {
        this->globalObjects.Append(entityId);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nLevel::IsGlobalEntity(nEntityObjectId entityId)
{
    return (this->globalObjects.Find(entityId) != 0);
}

//------------------------------------------------------------------------------
/**
    history:
        - 19-Jan-2006 ma.garcias    - reuse slot if it existed
*/
void
nLevel::SetEntityName(nEntityObjectId entityId, const char *name)
{
    n_assert(entityId);
    
    EntityNameNode *node = (EntityNameNode *) this->entityNameList.Find(name);
    if (node)
    {
        node->entityId = entityId;
        return;
    }

    EntityNameNode *newNameNode = n_new(EntityNameNode);
    
    newNameNode->SetName(name);
    newNameNode->entityId = entityId;
    
    this->entityNameList.AddHead(newNameNode);
}

//------------------------------------------------------------------------------
/**
*/
const char *
nLevel::GetEntityName(nEntityObjectId entityId)
{
    for (EntityNameNode *strNode = (EntityNameNode *) this->entityNameList.GetHead();
         strNode;
         strNode = (EntityNameNode *) strNode->GetSucc())
    {
        if (strNode->entityId == entityId)
        {
            return strNode->GetName();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nEntityObjectId
nLevel::FindEntity(const char *entityName)
{
    EntityNameNode *node = (EntityNameNode *) this->entityNameList.Find(entityName);

    if (node)
    {
        return node->entityId;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLevel::RemoveEntityName(const char *entityName)
{
    EntityNameNode *node = (EntityNameNode *) this->entityNameList.Find(entityName);
    if (node)
    {
        node->Remove();
        n_delete(node);
    }
}

//------------------------------------------------------------------------------
/**
    load resources for all registered entities
*/
void
nLevel::LoadEntityResources()
{
    nEntityObjectServer *entityServer = nEntityObjectServer::Instance();
    int i;

    // FIXME ensure that outdoor entity is loaded first
    nEntityObjectId outdoorEntityId = this->FindEntity("outdoor");
    if (outdoorEntityId)
    {
        nEntityObject *entity = entityServer->GetEntityObject(outdoorEntityId);
        if (entity)
        {
            this->AddEntity(entity);
        }
    }

    // load space entities through the usual interface
    for (i = 0; i < this->spaceObjects.Size(); i++)
    {
        if (spaceObjects[i] != outdoorEntityId)
        {
            nEntityObject *entity = entityServer->GetEntityObject(spaceObjects[i]);
            if (entity)
            {
                this->AddEntity(entity);
            }
        }
    }

    // try to connect indoors that share a common portal
    nSpatialServer::Instance()->ConnectIndoors();

    // register global entities in spatial server
    for (i = 0; i < this->globalObjects.Size(); i++)
    {
        nEntityObject *entity = entityServer->GetEntityObject(globalObjects[i]);
        if (entity)
        {
            this->AddEntity(entity);
        }
    }

    // load areas that reference spaces
    nLoaderServer::Instance()->LoadAreas();
}

//------------------------------------------------------------------------------
/**
    unload resources for all registered entities
*/
void
nLevel::UnloadEntityResources()
{
    nEntityObjectServer *entityServer = nEntityObjectServer::Instance();

    // remove only the entities that are registered as global
    // to avoid removing cameras or any other non-level entity
    for (int i = 0; i < this->globalObjects.Size(); ++i)
    {
        nEntityObject *globalEntity = entityServer->GetEntityObject(this->globalObjects[i]);
        if (globalEntity)
        {
            nSpatialServer::Instance()->RemoveGlobalEntity(globalEntity);
        }
    }

    //<HACK> manually release spaces in spatial and physics server
    //scene resources for the terrain needs to be unloaded before loading a level
    nSpatialServer *spatialServer = nSpatialServer::Instance();
    nEntityClass* terrainClass = 0;
    if (spatialServer->HasQuadtreeSpace())
    {
        terrainClass = spatialServer->GetOutdoorEntity()->GetEntityClass();
    }

    if (terrainClass && terrainClass->GetComponentSafe<ncLoaderClass>()->AreResourcesValid())
    {
        terrainClass->GetComponentSafe<ncLoaderClass>()->UnloadResources();
    }

    nSpatialServer::Instance()->ReleaseSpaces();
    //</HACK>

    // cleanup all pending entity objects
    entityServer->UnloadNormalEntityObjects();

    // deleted objects must be forgotten
    entityServer->DiscardDeletedObjects();
}

//------------------------------------------------------------------------------
/**
    save resources for all registered entities
*/
void
nLevel::SaveEntityResources()
{
#ifndef __ZOMBIE_EXPORTER__
#ifndef NGAME
    nEntityObjectServer *entityServer = nEntityObjectServer::Instance();
    // save the outdoor resources are stored (what has been modified from editor)
    nEntityObjectId outdoorId = this->FindEntity("outdoor");
    if (outdoorId != 0)
    {
        nEntityObject * outdoor = entityServer->GetEntityObject(outdoorId);
        if (outdoor)
        {
            nEntityClass * neclass = outdoor->GetEntityClass();
            n_assert(neclass);
            
            // save heightmap resource
            ncTerrainGMMClass * tgmmc = neclass->GetComponent<ncTerrainGMMClass>();
            n_assert(tgmmc);
            tgmmc->SaveTerrainGMM();

            // save veg class , this when save change info of ncTerrainMaterialClass.
            ncTerrainVegetationClass* vegClass = neclass->GetComponentSafe<ncTerrainVegetationClass>();
            vegClass->SaveResources();

            // save material resources
            ncTerrainMaterialClass * tmc = neclass->GetComponent<ncTerrainMaterialClass>();
            n_assert(tmc);
            tmc->SaveResources();

            // save navigation mesh
            ncNavMesh* navMesh = outdoor->GetComponent<ncNavMesh>();
            n_assert(navMesh);
            navMesh->Save();

            // save entities into the outdoor
            ncSpatialQuadtree *spatialQuadComp = outdoor->GetComponent<ncSpatialQuadtree>();
            n_assert(spatialQuadComp);
            spatialQuadComp->Save();
        }
    }

    // save entities in indoor spaces
    nArray<ncSpatialIndoor*>& indoors = nSpatialServer::Instance()->GetIndoorSpaces();
    for (int i = 0; i < indoors.Size(); ++i)
    {
        indoors[i]->Save();

        // save navigation mesh
        indoors[i]->GetComponentSafe<ncNavMesh>()->Save();
    }

    // save nav mesh of the walkable brushes
    for ( nEntityObject* entity( nEntityObjectServer::Instance()->GetFirstEntityObject() ); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
    {
        if ( entity->IsA("newalkablebrush") )
        {
            entity->GetComponentSafe<ncNavMesh>()->Save();
        }
    }

    // save load areas
    nLoaderServer::Instance()->SaveAreas();

#endif
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
nLevel::AddEntity(nObject *entity)
{
    nEntityObject *entityObject = static_cast<nEntityObject*>(entity);
    n_assert(entity->IsA("nentityobject"));

    // explicitly handle global entities
    if (this->globalObjects.Find(entityObject->GetId()))
    {
        nSpatialServer::Instance()->InsertGlobalEntity(entityObject);
    }

    // explicitly handle space entities
    if (entityObject->GetComponent<ncSpatialSpace>())
    {
        if (!this->spaceObjects.Find(entityObject->GetId()))
        {
            this->spaceObjects.Append(entityObject->GetId());
        }
    }

    //<TEMPORARY>
    //automatically guess that an entity is a global one, if not specified
    else if (entityObject->GetComponent<ncScene>() && 
             !entityObject->GetComponent<ncSpatial>())
    {
        this->SetGlobalEntity(entityObject->GetId());
        nSpatialServer::Instance()->InsertGlobalEntity(entityObject);
    }
    //</TEMPORARY>
}
