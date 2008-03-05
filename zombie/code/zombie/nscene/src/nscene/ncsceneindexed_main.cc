#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncsceneindexed_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncsceneindexed.h"
#include "nscene/ncsceneclass.h"
#include "nscene/ntransformnode.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSceneIndexed, ncScene);

//------------------------------------------------------------------------------
/**
*/
ncSceneIndexed::ncSceneIndexed()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneIndexed::~ncSceneIndexed()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneIndexed::Load()
{
    n_assert(!this->IsValid());

    ncSceneClass *componentClass = this->entityObject->GetClassComponent<ncSceneClass>();
    nSceneNode *rootNode = componentClass->GetRootNode();
    n_assert(rootNode);

    nKernelServer::Instance()->PushCwd(rootNode);
    this->refSubScene->EntityCreated(this->GetEntityObject());
    this->refSubScene->PreloadResources();
    nKernelServer::Instance()->PopCwd();

    this->refRootNode = this->refSubScene.get();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneIndexed::SaveCmds(nPersistServer* ps)
{
    if (!ncScene::SaveCmds(ps))
    {
        return false;
    }

    nCmd *cmd;

    // --- setsubscenepath ---
    cmd = ps->GetCmd(this->GetEntityObject(), 'SSSP');
    cmd->In()->SetS(this->refSubScene.getname());
    ps->PutCmd(cmd);
    
    return true;
}
