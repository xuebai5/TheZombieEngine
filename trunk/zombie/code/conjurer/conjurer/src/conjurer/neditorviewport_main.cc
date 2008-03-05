#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  neditorviewport_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/neditorviewport.h"
#include "conjurer/nviewportdebugmodule.h"
#include "napplication/napplication.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/ncloader.h"
#include "zombieentity/ncloaderclass.h"
#include "zombieentity/ncdictionary.h"
#include "nscene/nsceneserver.h"
#include "nscene/nscenegraph.h"
#include "nscene/ncviewport.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatiallight.h"
#include "nspatial/ncspatialcamera.h"
#include "ndebug/ndebugserver.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "entity/nentity.h"

nNebulaScriptClass(nEditorViewport, "ncommonviewport");

nRef<nEntityObject> nEditorViewport::refLightEntity;
nRef<nEntityObject> nEditorViewport::refAxesEntity;

//------------------------------------------------------------------------------
/**
*/
nEditorViewport::nEditorViewport() :
    moveLight(false),
    showAxes(false),
    statsLevel(0)
{
    // bind to level creation and loading
    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelCreated, 
                                          this,
                                          &nEditorViewport::OnLevelLoaded,
                                          0);

    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelLoaded, 
                                          this,
                                          &nEditorViewport::OnLevelLoaded,
                                          0);
}

//------------------------------------------------------------------------------
/**
*/
nEditorViewport::~nEditorViewport()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    create a viewport entity
*/
bool
nEditorViewport::Open()
{
    n_assert(!this->IsOpen());

    // initialize shared light and axes entities
    if (!this->refLightEntity.isvalid())
    {
        if (nEntityClassServer::Instance()->GetEntityClass("Stdlight"))
        {
            this->refLightEntity = nEntityObjectServer::Instance()->NewLocalEntityObject("Stdlight");
            this->refLightEntity->GetComponent<ncScene>()->SetAttachFlags(ncScene::ShapeVisible|ncScene::CastShadows);
        }
    }
    else
    {
        this->refLightEntity->AddRef();
    }

    if (!this->refAxesEntity.isvalid())
    {
        if (nEntityClassServer::Instance()->GetEntityClass("Stdaxes"))
        {
            this->refAxesEntity = nEntityObjectServer::Instance()->NewLocalEntityObject("Stdaxes");
        }
    }
    else
    {
        this->refAxesEntity->AddRef();
    }

    // initialize debug module
    const char *modulename = this->GetDebugModuleName();
    this->refDebugModule = (nViewportDebugModule *) nDebugServer::Instance()->CreateDebugModule("nviewportdebugmodule", modulename);
    this->refDebugModule->SetAppViewport(this);

    if (nCommonViewport::Open())
    {
        // open the debug module - done here due to references to gfxviewport
        this->refDebugModule->Open();

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Shutdown viewport, release the scene graph, debug module,
    and the viewport and camera entities.
*/
void
nEditorViewport::Close()
{
    n_assert(this->IsOpen());

    // release debug module
    if (this->refDebugModule.isvalid())
    {
        this->refDebugModule->Close();
        this->refDebugModule->Release();
        this->refDebugModule.invalidate();
    }

    // release shared light and axes entities
    if (this->refLightEntity.isvalid())
    {
        this->refLightEntity->Release();
        if (this->refLightEntity->GetRefCount() == 1)
        {
            nEntityObjectServer::Instance()->RemoveEntityObject(this->refLightEntity);
        }
    }
    if (this->refAxesEntity.isvalid())
    {
        this->refAxesEntity->Release();
        if (this->refAxesEntity->GetRefCount() == 1)
        {
            nEntityObjectServer::Instance()->RemoveEntityObject(this->refAxesEntity);
        }
    }

    nCommonViewport::Close();
}


//------------------------------------------------------------------------------
/**
*/
bool
nEditorViewport::ReloadViewport()
{
    if (! nCommonViewport::ReloadViewport() )
    {
        return false;
    }

    // store the debug module as a local variable in the render context
    nVariable::Handle varHandle = nVariableServer::Instance()->GetVariableHandleByName("debug");
    ncScene* renderContext = this->refViewport->GetComponent<ncScene>();
    nVariable* var = renderContext->FindLocalVar(varHandle);
    if (!var)
    {
        renderContext->AddLocalVar(nVariable(varHandle, this->refDebugModule.get()));
    }
    else
    {
        var->SetObj(this->refDebugModule.get());
    }


    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorViewport::UpdateShaderOverrides()
{
    n_assert(this->refDebugModule.isvalid());
    if (this->refViewport.isvalid())
    {
        this->refDebugModule->Overrides(this->refViewport->GetComponentSafe<ncScene>());
        this->refViewport->GetComponentSafe<ncDictionary>()->SetIntVariable("statsLevel", this->statsLevel);
    }
}

//------------------------------------------------------------------------------
/**
*/
uint
nEditorViewport::GetPassEnabledFlags()
{
    n_assert(this->refViewport.isvalid());
    return this->refViewport->GetComponent<ncViewport>()->GetPassEnabledFlags();
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorViewport::OnLevelLoaded()
{
    //<HACK> this is to work with older levels, replace stdlight with mine
    nLevel* curLevel = nLevelManager::Instance()->GetCurrentLevel();
    if (curLevel && this->refLightEntity.isvalid())
    {
        nEntityObjectId axesEntityId = curLevel->FindEntity("axes");
        if (axesEntityId)
        {
            curLevel->RemoveEntityName("axes");
            nEntityObject* axesEntity = nEntityObjectServer::Instance()->GetEntityObject(axesEntityId);
            if (axesEntity)
            {
                nSpatialServer::Instance()->RemoveGlobalEntity(axesEntity);
                nEntityObjectServer::Instance()->RemoveEntityObject(axesEntity);
            }
        }

        nEntityObjectId lightEntityId = curLevel->FindEntity("stdlight");
        if (lightEntityId && lightEntityId != this->refLightEntity->GetId())
        {
            curLevel->RemoveEntityName("stdlight");
            nEntityObject* lightEntity = nEntityObjectServer::Instance()->GetEntityObject(lightEntityId);
            if (lightEntity)
            {
                nSpatialServer::Instance()->RemoveGlobalEntity(lightEntity);
                nEntityObjectServer::Instance()->RemoveEntityObject(lightEntity);
            }
        }

        //replace stdlight entity id with local- for OUTGUI only
        curLevel->SetEntityName(this->refLightEntity->GetId(), "stdlight");
    }
    //<HACK>

    nCommonViewport::OnLevelLoaded();
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorViewport::SetCollectEntities(bool collect)
{
    this->GetSpatialCamera()->SetUseCameraCollector(collect);
}

//------------------------------------------------------------------------------
/**
*/
bool
nEditorViewport::GetCollectEntities()
{
    return this->GetSpatialCamera()->GetUseCameraCollector();
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorViewport::OnRender3D()
{
    nCommonViewport::OnRender3D();

    // attach stdlight to perform per-scene lighting
    if (!this->GetObeyLightLinks())
    {
        ncScene* renderContext = this->refLightEntity->GetComponentSafe<ncScene>();
        renderContext->SetFrameId(this->GetFrameId());
        renderContext->SetTime(this->GetTime());
        renderContext->Render(this->GetSceneGraph());
    }

    // attach system axes
    if (this->refAxesEntity.isvalid() && this->showAxes)
    {
        ncScene* renderContext = this->refAxesEntity->GetComponentSafe<ncScene>();
        renderContext->SetFrameId(this->GetFrameId());
        renderContext->SetTime(this->GetTime());
        renderContext->Render(this->GetSceneGraph());
    }
}
