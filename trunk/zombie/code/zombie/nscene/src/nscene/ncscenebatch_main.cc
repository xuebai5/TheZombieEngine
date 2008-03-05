#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncscenebatch_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncscenebatch.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSceneBatch,ncScene);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneBatch)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSceneBatch::ncSceneBatch() :
    isBatchInitialized(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneBatch::~ncSceneBatch()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneBatch::InitInstance(nObject::InitInstanceMsg initType)
{
    ncScene::InitInstance(initType);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneBatch::SetBatchRoot(nSceneNode* sceneRoot)
{
    n_assert(!this->IsValid());
    n_assert(sceneRoot);
    sceneRoot->AddRef();//to allow the builder to release its own
    this->refRootNode = sceneRoot;
    this->isBatchInitialized = false;
}

//------------------------------------------------------------------------------
/**
    Initialize this instance.
*/
bool
ncSceneBatch::IsValid()
{
    return this->isBatchInitialized && ncScene::IsValid();
}

//------------------------------------------------------------------------------
/**
    Initialize this instance. Unlike regular scene components,
    this one requires the root node having been explicitly assigned.
    It can't retrieve it from any scene class.
*/
bool
ncSceneBatch::Load()
{
    n_assert(!this->IsValid());

    if (this->refRootNode.isvalid())
    {
        this->refRootNode->EntityCreated(this->GetEntityObject());
        this->refRootNode->PreloadResources();
        this->isBatchInitialized = true;
        return true;
    }
    
    return false;
}

//------------------------------------------------------------------------------
/**
    Initialize this instance.
*/
void
ncSceneBatch::Unload()
{
    if (this->IsValid())
    {
        this->refRootNode->EntityDestroyed(this->GetEntityObject());
        this->refRootNode->Release();
        this->refRootNode.invalidate();
        this->localVarArray.Reset();
        this->isBatchInitialized = false;
    }
}

//------------------------------------------------------------------------------
/**
    Render this instance.
*/
void
ncSceneBatch::Render(nSceneGraph * sceneGraph)
{
    ncScene::Render(sceneGraph);
}
