#include "precompiled/pchframework.h"
//------------------------------------------------------------------------------
//  nlevel_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nlevel/nlevel.h"
#include "kernel/npersistserver.h"
#include "entity/nentityobjectserver.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatialquadtree.h"
#include "nspatial/ncspatialindoor.h"

#ifndef NGAME
#include "nlayermanager/nlayermanager.h"
#endif

NSCRIPT_INITCMDS_BEGIN(nLevel)

    NSCRIPT_ADDCMD('SSPE', void, SetSpaceEntity, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('SGLE', void, SetGlobalEntity, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('ISGE', bool, IsGlobalEntity, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('SENN', void, SetEntityName, 2, (nEntityObjectId, const char *), 0, ());
    NSCRIPT_ADDCMD('GENN', const char *, GetEntityName, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD('FINE', nEntityObjectId, FindEntity, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('RENN', void, RemoveEntityName, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('APEN', void, AddEntity, 1, (nObject*), 0, ());
    NSCRIPT_ADDCMD('SWZK', void, SetWizardKey, 1, (int), 0, ());
    NSCRIPT_ADDCMD('AELY', void, AddEntityLayer, 4, (int, const char *, bool, const char *), 0, ());
    #ifndef NGAME
    NSCRIPT_ADDCMD('GELM', nRoot*, GetEntityLayerManager, 0, (), 0, ());
    #endif NGAME

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
int
__cdecl
nLevel::EntityIdSorter(const void* elm0, const void* elm1)
{
    nEntityObjectId i0 = *(nEntityObjectId*)elm0;
    nEntityObjectId i1 = *(nEntityObjectId*)elm1;

    if (i0 < i1)
    {
        return -1;
    }
    else if (i0 > i1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
int
__cdecl
nLevel::EntityNameSorter(const void* elm0, const void* elm1)
{
    EntityNameNode* node0 = *(EntityNameNode**)elm0;
    EntityNameNode* node1 = *(EntityNameNode**)elm1;

    if (node0->entityId < node1->entityId)
    {
        return -1;
    }
    else if (node0->entityId > node1->entityId)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
int
__cdecl
nLevel::EntityLayerSorter(const void* elm0, const void* elm1)
{
    nLayer* layer0 = *(nLayer**)elm0;
    nLayer* layer1 = *(nLayer**)elm1;

    if (layer0->GetLayerKey() < layer1->GetLayerKey())
    {
        return -1;
    }
    else if (layer0->GetLayerKey() > layer1->GetLayerKey())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif

//------------------------------------------------------------------------------
/**
*/
void
nLevel::SortKeyGroup(KeyGroup& keyGroup)
{
    // sort globals by entity id
    qsort(keyGroup.keyGlobals.Begin(), keyGroup.keyGlobals.Size(), sizeof(nEntityObjectId), EntityIdSorter);
    // sort spaces by entity id
    qsort(keyGroup.keySpaces.Begin(), keyGroup.keySpaces.Size(), sizeof(nEntityObjectId), EntityIdSorter);
    // sort names by entity id
    qsort(keyGroup.keyNames.Begin(), keyGroup.keyNames.Size(), sizeof(EntityNameNode*), EntityNameSorter);

#ifndef NGAME
    // sort layer by wizard id
    qsort(keyGroup.keyLayers.Begin(), keyGroup.keyLayers.Size(), sizeof(nLayer*), EntityLayerSorter);
#endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nLevel::SaveCmds(nPersistServer *ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nArray<KeyGroup> keyGroups;
        keyGroups.SetFixedSize(0x100);

        // persist all space entities currently in the spatial server
        // (their names should have been added to the level explicitly)
        nArray<nEntityObject*> objects;
        nSpatialServer *spatialServer = nSpatialServer::Instance();
        if (spatialServer->HasQuadtreeSpace())
        {
            nEntityObjectId entityId = spatialServer->GetOutdoorEntity()->GetId();
            int index = (entityId & nEntityObjectServer::IDHIGHMASK) >> nEntityObjectServer::IDLOWBITS;
            keyGroups[index].keySpaces.Append(entityId);
        }

        nArray<ncSpatialIndoor*>& indoors = spatialServer->GetIndoorSpaces();
        for (int i = 0; i < indoors.Size(); ++i)
        {
            nEntityObjectId entityId = indoors[i]->GetEntityObject()->GetId();
            int index = (entityId & nEntityObjectServer::IDHIGHMASK) >> nEntityObjectServer::IDLOWBITS;
            keyGroups[index].keySpaces.Append(entityId);
        }

        // persist global entities using wizards
        nArray<nEntityObject*> globals;
        nSpatialServer::Instance()->GetCommonGlobalEntities(globals);
        for (int i = 0; i < globals.Size(); ++i)
        {
            // persist only user-created global entities (eg. not cameras)
            if (globals.At(i)->GetEntityClass()->IsUserCreated() && 
                nEntityObjectServer::Instance()->GetEntityObjectType(globals.At(i)->GetId()) == nEntityObjectServer::Normal)
            {
                nEntityObjectId entityId = globals.At(i)->GetId();
                int index = (entityId & nEntityObjectServer::IDHIGHMASK) >> nEntityObjectServer::IDLOWBITS;
                keyGroups[index].keyGlobals.Append(entityId);
            }
        }

        // group entity names by wizard id
        EntityNameNode* curNode;
        for (curNode = (EntityNameNode*) this->entityNameList.GetHead(); 
             curNode;
             curNode = (EntityNameNode*) curNode->GetSucc())
        {
            int index = (curNode->entityId & nEntityObjectServer::IDHIGHMASK) >> nEntityObjectServer::IDLOWBITS;
            // avoid persisting local entities!
            if (index > 0)
            {
                keyGroups[index].keyNames.Append(curNode);
            }
        }

#ifndef NGAME
        // group entity layers by wizard id
        int numLayers = this->refEntityLayerManager->GetNumLayers();
        for (int i = 0; i < numLayers; ++i)
        {
            int layerKey;
            nLayer *curLayer = this->refEntityLayerManager->GetLayerAt(i);
            if (curLayer->GetLayerKey() == -1)
            {
                layerKey = nEntityObjectServer::Instance()->GetHighId();
            }
            else
            {
                layerKey = curLayer->GetLayerKey();
            }
            int index = (layerKey & nEntityObjectServer::IDHIGHMASK) >> nEntityObjectServer::IDLOWBITS;
            keyGroups[index].keyLayers.Append(curLayer);
        }
#endif

        // persist globals, entities and layers using wizards
        for (int i = 0; i < keyGroups.Size(); ++i)
        {
            // sort keys, to keep same persistence order
            this->SortKeyGroup(keyGroups[i]);
                
#ifndef NGAME
            // --- setwizardkey ---
            ps->Put(this, 'SWZK', (i << nEntityObjectServer::IDLOWBITS));
#endif
            int j;
            for (j = 0; j < keyGroups[i].keyGlobals.Size() ; ++j)
            {
                // --- addglobalentity ---
                nEntityObjectId entityId = keyGroups[i].keyGlobals[j];
                ps->Put(this, 'SGLE', entityId);
            }

            for (j = 0; j < keyGroups[i].keySpaces.Size() ; ++j)
            {
                // --- setspaceentity ---
                nEntityObjectId entityId = keyGroups[i].keySpaces[j];
                ps->Put(this, 'SSPE', entityId);
            }

            for (j = 0; j < keyGroups[i].keyNames.Size() ; ++j)
            {
                // --- setentityname ---
                EntityNameNode* curNode = keyGroups[i].keyNames[j];
                ps->Put(this, 'SENN', curNode->entityId, curNode->GetName());
            }
#ifndef NGAME
            for (j = 0; j < keyGroups[i].keyLayers.Size() ; ++j)
            {
                // --- addentitylayer ---
                nLayer *curLayer = keyGroups[i].keyLayers[j];
                ps->Put(this, 'AELY', curLayer->GetId(), curLayer->GetLayerName(), curLayer->IsLocked(), curLayer->GetPassword() );
            }
#endif
        }

        return true;
    }
    return false;
}
