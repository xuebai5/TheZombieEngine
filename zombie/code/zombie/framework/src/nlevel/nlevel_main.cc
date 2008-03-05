#include "precompiled/pchframework.h"
//------------------------------------------------------------------------------
//  nlevel_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nlevel/nlevel.h"
#include "kernel/nkernelserver.h"

#ifndef NGAME
#include "nlayermanager/nlayermanager.h"
#endif

nNebulaScriptClass(nLevel, "nroot");

//------------------------------------------------------------------------------
/**
*/
nLevel::nLevel() :
    currentWizardKey(nEntityObjectServer::IDINVALID)
{
    this->spaceObjects.Clear();
    this->globalObjects.Clear();
}

//------------------------------------------------------------------------------
/**
*/
nLevel::~nLevel()
{
    nStrNode *strHead = this->entityNameList.GetHead();
    while (strHead)
    {
        nStrNode *next = strHead->GetSucc();
        strHead->Remove();
        n_delete(strHead);
        strHead = next;
    }
#ifndef NGAME
    this->refEntityLayerManager->Clear();
#endif
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
nRoot*
nLevel::GetEntityLayerManager()
{
    return this->refEntityLayerManager.get();
}
#endif


#ifdef NGAME
//------------------------------------------------------------------------------
/**
*/
void
nLevel::AddEntityLayer(int /*layerId*/, const char * /*layerName*/, bool, const char * )
{
    // empty
}

#else
//------------------------------------------------------------------------------
/**
    add an entity layer (for persistence only)
*/
void
nLevel::AddEntityLayer(int layerId, const char * layerName, bool isLocked, const char * password)
{
    EntityLayerNode *newLayerNode = n_new(EntityLayerNode);
    newLayerNode->locked = isLocked;
    newLayerNode->SetName(layerName);
    newLayerNode->layerId = layerId;
    newLayerNode->layerKey = this->currentWizardKey;
    newLayerNode->password = password;
    this->entityLayerList.AddHead(newLayerNode);
}

//------------------------------------------------------------------------------
/**
    load entity layers (loaded from persistence)
*/
void
nLevel::LoadEntityLayers()
{
    n_assert(this->refEntityLayerManager.isvalid());
    nLayerManager *layerManager = this->refEntityLayerManager.get();
    layerManager->Clear();

    // traverse the list of layers and empty it
    nStrNode *strHead = this->entityLayerList.GetHead();
    bool alreadyCreated = false;
    nLayer *newLayer = 0;
    while (strHead)
    {
        nStrNode *strNext = strHead->GetSucc();
        EntityLayerNode *node = static_cast<EntityLayerNode*>(strHead);
        
        newLayer = layerManager->CreateNewLayer(node->GetName(), node->layerId, alreadyCreated);
        newLayer->SetLayerKey(node->layerKey);
        newLayer->SetLocked(node->locked);
        newLayer->SetPassword(node->password.Get(), false);
        n_assert(!alreadyCreated);
        strHead->Remove();
        n_delete(strHead);
        strHead = strNext;
    }

    if (this->refEntityLayerManager->GetNumLayers() == 0)
    {
        newLayer = this->refEntityLayerManager->CreateNewLayer("default", alreadyCreated);
        newLayer->SetLayerKey(nEntityObjectServer::Instance()->GetHighId());
    }

    if (newLayer)
    {
        layerManager->SelectLayer(newLayer->GetId());
    }
}
#endif
