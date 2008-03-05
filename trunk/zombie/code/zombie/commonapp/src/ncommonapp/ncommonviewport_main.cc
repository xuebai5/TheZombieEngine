#include "precompiled/pchncommonapp.h"
//------------------------------------------------------------------------------
//  ncommonviewport_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncommonapp/ncommonviewport.h"
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
#include "nspatial/nvisiblefrustumvisitor.h"
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"
#include "entity/nentityclassserver.h"
#include "nspecialfx/nfxserver.h"
#include "kernel/nprofiler.h"

nNebulaScriptClass(nCommonViewport, "nappviewport");

//------------------------------------------------------------------------------
/**
*/
nCommonViewport::nCommonViewport() :
    resourceFile("wc:libs/system/cameras/stdcam.n2"),
    reloadViewport(false),
    obeyLightLinks(false),
    drawDecals(true),
    mouseXfactor(1.0f),
    mouseYfactor(1.0f),
    maxRenderDistance(100.f),
    obeyMaxDistance(false)
#if __NEBULA_STATS__
    ,profAttachTrigger("profSceneAttachTrigger", true)
    ,profAttachViewport("profSceneAttachViewport", true)
#endif
{
    // bind to level creation and loading
    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelCreated, 
                                          this,
                                          &nCommonViewport::OnLevelLoaded,
                                          0);

    nLevelManager::Instance()->BindSignal(nLevelManager::SignalLevelLoaded, 
                                          this,
                                          &nCommonViewport::OnLevelLoaded,
                                          0);
}

//------------------------------------------------------------------------------
/**
*/
nCommonViewport::~nCommonViewport()
{
    if (this->IsOpen())
    {
        this->SetVisible(false);
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::SetMaxRenderDistance(float dist)
{
    this->maxRenderDistance = dist;
    if (this->GetObeyMaxDistance())
    {
        this->refSpatialCamera->GetComponent<ncSpatialCamera>()->SetMaxTestOutdoorDist(dist);
    }
}

//------------------------------------------------------------------------------
/**
*/
float
nCommonViewport::GetMaxRenderDistance()
{
    return this->maxRenderDistance;
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::SetObeyMaxDistance(bool value)
{
    this->obeyMaxDistance = value;
    if (this->obeyMaxDistance)
    {
        this->refSpatialCamera->GetComponent<ncSpatialCamera>()->SetMaxTestOutdoorDist(this->maxRenderDistance);
    }
    else
    {
        this->refSpatialCamera->GetComponent<ncSpatialCamera>()->SetMaxTestOutdoorDist(FLT_MAX);
    }
}

//------------------------------------------------------------------------------
/**
    create a viewport entity
*/
bool
nCommonViewport::Open()
{
    n_assert(!this->IsOpen());

    // initialize viewport entity
    if (!this->LoadViewport())
    {
        return false;
    }

    // create scenegraph
    this->refSceneGraph = nSceneServer::Instance()->NewSceneGraph(0);

    // create camera entity
    matrix44 viewMatrix;
    this->refSpatialCamera = nSpatialServer::Instance()->CreateCamera(this->GetCamera(), viewMatrix, false);
    this->refSpatialCamera->GetComponent<ncSpatialCamera>()->SetMaxTestOutdoorDist(this->maxRenderDistance);

    // FIXME set own camera for this viewport only if not persisted
    //this->gfxViewport->GetComponentSafe<ncViewport>()->SetCamera(this->spatialCamera.get());

    // HACK refresh level entities- viewports are created after the level is loaded
    this->OnLevelLoaded();

    return nAppViewport::Open();
}

//------------------------------------------------------------------------------
/**
    Shutdown viewport, release the scene graph, debug module,
    and the viewport and camera entities.
*/
void
nCommonViewport::Close()
{
    n_assert(this->IsOpen());

    // release scene graph
    if (this->refSceneGraph.isvalid())
    {
        this->refSceneGraph->Release();
        this->refSceneGraph.invalidate();
    }

    // release spatial camera
    if (this->refSpatialCamera.isvalid())
    {
        nSpatialServer::Instance()->DestroyCamera(this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>());
        this->refSpatialCamera.invalidate();
    }

    // unload viewport entity
    this->UnloadViewport();

    nAppViewport::Close();
}

//------------------------------------------------------------------------------
/**
    If the viewport is currently open, refresh the resource file for
    the entity class just as it would have been loaded at first.
    Unload first the current viewport object.
*/
void 
nCommonViewport::Refresh()
{
    if (!this->IsOpen())
    {
        return;
    }

    // load from viewport resource
    if (!this->LoadViewport())
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nCommonViewport::LoadViewport()
{
    // initialize viewport entity class from resourceFile
    // FIXME load entity from class instead
    nEntityClassServer *entityServer = nEntityClassServer::Instance();
    nString className = this->resourceFile.ExtractFileName();
    className.StripExtension();
    className.ToCapital();

    nEntityClass *viewportClass = entityServer->GetEntityClass(className.Get());
    if (!viewportClass)
    {
        nEntityClass *entityClass = entityServer->GetEntityClass("neviewport");
        viewportClass = entityServer->NewEntityClass(entityClass, className.Get());
        n_assert(viewportClass);
    }

    // load the viewport entity
    nEntityObject* viewport = nEntityObjectServer::Instance()->NewLocalEntityObject(className.Get());
    n_assert(viewport);
    viewport->AddRef();

    // if there is a current viewport, copy common components before reloading
    if (this->refViewport.isvalid())
    {
        viewport->CopyCommonComponentsFrom(this->refViewport.get());
        this->UnloadViewport();
    }

    // finally, replace the viewport reference
    this->refViewport = viewport;

    // initialize viewport scene resources
    if (!this->ReloadViewport())
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::UnloadViewport()
{
    // release viewport entity
    if (this->refViewport.isvalid())
    {
        this->refViewport->Release();
        nEntityObjectServer::Instance()->RemoveEntityObject(this->refViewport.get());
        this->refViewport.invalidate();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nCommonViewport::ReloadViewport()
{
    n_assert(this->refViewport.isvalid());

    // load the viewport class resource
    ncLoaderClass *loader = this->refViewport->GetClassComponentSafe<ncLoaderClass>();
    if (!loader->AreResourcesValid())
    {
        loader->SetResourceFile(this->resourceFile.Get());
        if (!loader->LoadResources())
        {
            return false;
        }
    }

    // load the viewport object resource
    ncLoader* loaderComp = this->refViewport->GetComponentSafe<ncLoader>();
    if (!loaderComp->LoadComponents())
    {
        return false;
    }

    this->reloadViewport = false;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
nEntityObject*
nCommonViewport::BeginViewportEntity(const char* className)
{
    nEntityObject* viewport = nEntityObjectServer::Instance()->NewLocalEntityObject(className);
    n_assert(viewport);
    viewport->AddRef();

    if (this->refViewport.isvalid())
    {
        viewport->CopyCommonComponentsFrom(this->refViewport.get());
        this->UnloadViewport();
    }

    this->refViewport = viewport;

    nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad(this->refViewport.get(), nObject::LoadedInstance);

    this->reloadViewport = true;

    return this->refViewport.get();
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::SetObeyLightLinks(bool obeyLightLinks)
{
    this->obeyLightLinks = obeyLightLinks;
    if (this->refViewport.isvalid())
    {
        this->refViewport->GetComponent<ncDictionary>()->SetBoolVariable("obeyLightLinks", obeyLightLinks);
    }
#ifndef NGAME
    nSpatialServer::Instance()->GetVisibilityVisitor().SetLightsDisabled(!obeyLightLinks);
#endif // !NGAME
}

//------------------------------------------------------------------------------
/**
*/
bool
nCommonViewport::GetObeyLightLinks()
{
    return this->obeyLightLinks;
}

//------------------------------------------------------------------------------
/**
    TODO replace by a VisibleChanged
*/
void
nCommonViewport::VisibleChanged()
{
    if (this->refSceneGraph.isvalid())
    {
        if (this->GetVisible())
        {
            nSceneServer::Instance()->AddGraph(this->refSceneGraph.get());
            this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->TurnOn();
        }
        else
        {
            nSceneServer::Instance()->RemoveGraph(this->refSceneGraph.get());
            this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->TurnOff();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::SetSpatialCamera(ncSpatialCamera *camera)
{
    this->refSpatialCamera = camera->GetEntityObject();
}

//------------------------------------------------------------------------------
/**
*/
ncSpatialCamera*
nCommonViewport::GetSpatialCamera()
{
    return this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>();
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::SetPassEnabled(nFourCC pass, bool passEnabled)
{
    n_assert(this->refViewport.isvalid());
    this->refViewport->GetComponent<ncViewport>()->SetPassEnabled(pass, passEnabled);
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::SetPassEnabledFlags(uint passEnabledFlags)
{
    n_assert(this->refViewport.isvalid());
    this->refViewport->GetComponent<ncViewport>()->SetPassEnabledFlags(passEnabledFlags);
}

//------------------------------------------------------------------------------
/**
*/
uint
nCommonViewport::GetPassEnabledFlags()
{
    n_assert(this->refViewport.isvalid());
    return this->refViewport->GetComponent<ncViewport>()->GetPassEnabledFlags();
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::Reset()
{
    //this->Refresh();

    //TODO reset to default state- which is...?
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::SetUseOctreeVisibility(bool value)
{
    if (value != nSpatialServer::Instance()->GetUseOctrees())
    {
        nSpatialServer::Instance()->ToggleUseOctrees();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::OnLevelLoaded()
{
    nLevel* curLevel = nLevelManager::Instance()->GetCurrentLevel();
    if (curLevel)
    {
        if (this->GetVisible())
        {
            this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->TurnOn();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Perform viewport logic- all viewports
*/
void
nCommonViewport::Trigger()
{
    // reload viewport resource if it has been re-assigned
    if (this->reloadViewport)
    {
        this->ReloadViewport();
    }

    #if __NEBULA_STATS__
    this->profAttachTrigger.StartAccum();
    #endif

    //update viewport entity in any case (the attached camera may be moved)
    ncTransform* viewTransform;
    viewTransform = this->refViewport->GetComponentSafe<ncTransform>();
    viewTransform->SetPosition(this->GetViewerPos());
    viewTransform->SetEuler(this->Transform().geteulerrotation());

    float x, y, w, h;
    this->GetViewport(x, y, w, h);
    ncViewport* viewContext = this->refViewport->GetComponentSafe<ncViewport>();
    viewContext->SetViewport(x, y, w, h);
    viewContext->SetCamera(this->GetCamera());
    viewTransform->SetEuler(this->Transform().geteulerrotation());

    //update camera entity
    viewTransform = this->refSpatialCamera->GetComponentSafe<ncTransform>();
    viewTransform->SetPosition(this->GetViewerPos());
    viewTransform->SetEuler(this->Transform().geteulerrotation());

    ncSpatialCamera* spatialCamera = this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>();
    spatialCamera->SetFrameId(this->GetFrameId());
    spatialCamera->SetNearPlaneDist(this->GetNearPlane());
    spatialCamera->Update(this->GetCamera());

    #if __NEBULA_STATS__
    this->profAttachTrigger.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonViewport::OnRender3D()
{
    n_assert(this->IsOpen());
    n_assert(this->refViewport.isvalid());
    n_assert(this->refSpatialCamera.isvalid());

    #if __NEBULA_STATS__
    this->profAttachViewport.StartAccum();
    #endif
    
    this->numVisibleEntities = 0;
    this->numVisibleDecals = 0;

    nSceneGraph* sceneGraph = this->GetSceneGraph();

    // attach the viewport
    // IMPORTANT always attach the viewport first, for it will set some parameters
    // that are important for later attaching the rest of the scene.
    ncScene* renderContext = this->refViewport->GetComponent<ncScene>();
    renderContext->SetFrameId(this->GetFrameId());
    renderContext->SetTime(this->GetTime());
    renderContext->Render(sceneGraph);

    ++this->numVisibleEntities;

    ncSpatialCamera* spatialCamera = this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>();
    nSpatialServer::Instance()->DetermineVisibility(spatialCamera, this->refSceneGraph);

    //TODO- MA render decals for visible entities

    #if __NEBULA_STATS__
    this->profAttachViewport.StopAccum();
    #endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nCommonViewport::HandleInput(nTime /*frameTime*/)
{
    return false;
}

//------------------------------------------------------------------------------
/**
    set the spatial camera's occlusion radius
*/
void 
nCommonViewport::SetOcclusionRadius(float rad)
{
    n_assert(this->refSpatialCamera.isvalid());
    n_assert2(rad >= 0, "Negative radius. It has to be positive or zero!");
    if ( rad >= 0 )
    {
        this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->SetOcclusionRadius(rad);
    }
}

//------------------------------------------------------------------------------
/**
    get the spatial camera's occlusion radius
*/
float 
nCommonViewport::GetOcclusionRadius() const
{
    n_assert(this->refSpatialCamera.isvalid());
    return this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->GetOcclusionRadius();
}

//------------------------------------------------------------------------------
/**
    set the maximum number of occluders to take into account for occlusion
*/
void 
nCommonViewport::SetMaxNumOccluders(int num)
{
    n_assert(this->refSpatialCamera.isvalid());
    this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->SetMaxNumOccluders(num);
}

//------------------------------------------------------------------------------
/**
    get the maximum number of occluders to take into account for occlusion
*/
int 
nCommonViewport::GetMaxNumOccluders() const
{
    n_assert(this->refSpatialCamera.isvalid());
    return this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->GetMaxNumOccluders();
}

//------------------------------------------------------------------------------
/**
    set the minimum value of the area-angle test to take into account an occluder
*/
void 
nCommonViewport::SetAreaAngleMin(float min)
{
    n_assert(this->refSpatialCamera.isvalid());
    this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->SetAreaAngleMin(min);
}

//------------------------------------------------------------------------------
/**
    get the minimum value of the area-angle test to take into account an occluder
*/
float
nCommonViewport::GetAreaAngleMin() const
{
    n_assert(this->refSpatialCamera.isvalid());
    return this->refSpatialCamera->GetComponentSafe<ncSpatialCamera>()->GetAreaAngleMin();
}
