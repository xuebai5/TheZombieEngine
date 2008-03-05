#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  npreviewviewport_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/npreviewviewport.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/ncloader.h"
#include "zombieentity/ncloaderclass.h"
#include "nscene/nsceneserver.h"
#include "nscene/nscenegraph.h"
#include "nscene/ncviewport.h"
#include "entity/nentity.h"
#include "gfx2/nshaderparams.h"

nNebulaScriptClass(nPreviewViewport, "nappviewport");

//------------------------------------------------------------------------------
/**
*/
nPreviewViewport::nPreviewViewport() :
    viewportClassName("Stdcam"),
    lightClassName("Stdlight"),
    moveLight(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nPreviewViewport::~nPreviewViewport()
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
bool
nPreviewViewport::Open()
{
    n_assert(!this->IsOpen());

    // initialize viewport entity
    if (!this->LoadViewport())
    {
        return false;
    }

    nEntityObjectServer* entityServer = nEntityObjectServer::Instance();

    // initialize light entity
    this->refLightEntity = entityServer->NewLocalEntityObject(this->lightClassName.Get());
    n_assert(this->refLightEntity.isvalid());
    this->refLightEntity->AddRef();
    ncLoaderClass* loaderClass = this->refLightEntity->GetClassComponentSafe<ncLoaderClass>();
    if (!loaderClass->AreResourcesValid())
    {
        if (!loaderClass->LoadResources())
        {
            return false;
        }
    }

    if (!this->refLightEntity->GetComponentSafe<ncLoader>()->LoadComponents())
    {
        return false;
    }

    ncScene* lightContext = this->refLightEntity->GetComponentSafe<ncScene>();
    nShaderParams& shaderOverrides = lightContext->GetShaderOverrides();
    shaderOverrides.SetArg(nShaderState::LightDiffuse, vector4(0.5f, 0.5f, 0.5f, 1.0f));
    shaderOverrides.SetArg(nShaderState::LightSpecular, vector4(1.0f, 1.0f, 1.0f, 1.0f));
    shaderOverrides.SetArg(nShaderState::LightAmbient, vector4(0.1f, 0.1f, 0.1f, 1.0f));

    // create scenegraph
    this->refSceneGraph = nSceneServer::Instance()->NewSceneGraph(0);

    // initialize view matrix
    camControl.Initialize();

    // reset to default position and orientation
    this->Reset();

    return nAppViewport::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
nPreviewViewport::Close()
{
    n_assert(this->IsOpen());

    // release scene graph
    if (this->refSceneGraph.isvalid())
    {
        this->refSceneGraph->Release();
        this->refSceneGraph.invalidate();
    }

    // unload preview resource
    if (this->refTargetEntity.isvalid())
    {
        this->refTargetEntity->Release();
        this->refTargetEntity.invalidate();
    }

    // unload light entity
    this->refLightEntity->Release();
    nEntityObjectServer::Instance()->RemoveEntityObject(this->refLightEntity.get());
    this->refLightEntity.invalidate();

    // unload viewport entity
    this->UnloadViewport();

    nAppViewport::Close();
}

//------------------------------------------------------------------------------
/**
    FIXME duplicated in nEditorViewport::LoadViewport
*/
bool
nPreviewViewport::LoadViewport()
{
    // initialize viewport entity class from resourceFile
    // FIXME load entity from class instead
    nEntityClassServer *entityServer = nEntityClassServer::Instance();
    
    // load the viewport class resource
    nEntityClass *viewportClass = entityServer->GetEntityClass(this->viewportClassName.Get());
    ncLoaderClass *loader = viewportClass->GetComponent<ncLoaderClass>();
    if (!loader->AreResourcesValid() && !loader->LoadResources())
    {
        return false;
    }

    // load the viewport entity
    this->refViewport = nEntityObjectServer::Instance()->NewLocalEntityObject(viewportClassName.Get());
    n_assert(this->refViewport.isvalid());
    this->refViewport->AddRef();

    ncLoader* loaderComp = this->refViewport->GetComponent<ncLoader>();
    if (!loaderComp->LoadComponents())
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    FIXME duplicated in nEditorViewport::UnloadViewport
*/
void
nPreviewViewport::UnloadViewport()
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
nPreviewViewport::LoadTargetEntity()
{
    // check that there is a class to preview
    if (!this->GetClassName())
    {
        return false;
    }

    // initialize preview entity
    nEntityClassServer* entityServer = nEntityClassServer::Instance();

    // check that the class has an asset with a scene resource
    nEntityClass* entityClass = entityServer->GetEntityClass(this->GetClassName());
    if (!entityClass)
    {
        return false;
    }

    ncLoaderClass* loaderClass = entityClass->GetComponent<ncLoaderClass>();
    if (!loaderClass || !loaderClass->GetResourceFile())
    {
        return false;
    }

    // initialize class name
    this->previewClassName.Set(this->GetClassName());
    this->previewClassName.Append("_preview");

    // initialize class if not available
    nEntityClass* myClass = entityServer->GetEntityClass(this->previewClassName.Get());
    if (!myClass)
    {
        nEntityClass* parentClass = entityServer->GetEntityClass("nescene");
        myClass = entityServer->NewEntityClass(parentClass, this->previewClassName.Get());
        n_assert(myClass);
    }

    // unload class resource asset if it was valid
    ncLoaderClass* myLoaderClass = myClass->GetComponentSafe<ncLoaderClass>();

    // load resource asset for preview
    myLoaderClass->SetResourceFile(loaderClass->GetResourceFile());
    if (!myLoaderClass->LoadResources())
    {
        return false;
    }

    // refresh local entity object if it was not valid
    if (this->refTargetEntity.isvalid())
    {
        ncLoader* myLoader = this->refTargetEntity->GetComponentSafe<ncLoader>();
        myLoader->UnloadComponents();
        n_assert(!myLoader->AreComponentsValid());
        nEntityObjectServer::Instance()->RemoveEntityObject(this->refTargetEntity.get());
    }

    // initialize the target entity
    this->refTargetEntity = nEntityObjectServer::Instance()->NewLocalEntityObject(this->previewClassName.Get());
    n_assert(this->refTargetEntity.isvalid());

    // and finally load the resources for the preview object
    ncLoader* myLoader = this->refTargetEntity->GetComponentSafe<ncLoader>();
    myLoader->LoadComponents();
    n_assert(myLoader->AreComponentsValid());

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nPreviewViewport::HandleInput(nTime /*frameTime*/)
{
    //rotate, zoom in/zoom out, reset
    // give inputs to camControl
    nInputServer* inputServer = nInputServer::Instance();
    this->camControl.SetResetButton(inputServer->GetButton("prev_reset"));
    this->camControl.SetLookButton(inputServer->GetButton("prev_look"));
    this->camControl.SetPanButton(inputServer->GetButton("prev_pan"));
    this->camControl.SetZoomButton(inputServer->GetButton("prev_zoom"));
    this->camControl.SetSliderLeft(inputServer->GetSlider("prev_left"));
    this->camControl.SetSliderRight(inputServer->GetSlider("prev_right"));
    this->camControl.SetSliderUp(inputServer->GetSlider("prev_up"));
    this->camControl.SetSliderDown(inputServer->GetSlider("prev_down"));

    this->camControl.Update();

    // copy camControl parameters to viewport
    const matrix44& view = this->camControl.GetViewMatrix();
    this->SetViewerPos(view.pos_component());
    this->SetViewerAngles(this->camControl.GetViewerAngles());

    // automatically rotate the target entity

    // move light with camera
    if (inputServer->GetButton("movelight"))
    {
        this->moveLight = !this->moveLight;
    }

    return true;//FIXME allow other viewports move
}

//------------------------------------------------------------------------------
/**
*/
void
nPreviewViewport::VisibleChanged()
{
    if (this->GetVisible())
    {
        if (!this->LoadTargetEntity())
        {
            this->SetVisible(false);
            return;    
        }

        // reset initial position
        this->Reset();
    }

    // activate the scene graph if all the previous code worked fine
    if (this->refSceneGraph.isvalid())
    {
        if (this->GetVisible())
        {
            nSceneServer::Instance()->AddGraph(this->refSceneGraph.get());
        }
        else
        {
            nSceneServer::Instance()->RemoveGraph(this->refSceneGraph.get());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nPreviewViewport::Reset()
{
    this->camControl.Reset();

    //this->SetViewerPos(vector3(0.0f, 0.0f, 5.0f));
    //this->SetViewerAngles(0.0f, 0.0f);
}

//------------------------------------------------------------------------------
/**
    Perform viewport logic
*/
void
nPreviewViewport::Trigger()
{
    //update viewport entity in any case (the attached camera may be moved)
    ncTransform* viewTransform;
    viewTransform = this->refViewport->GetComponentSafe<ncTransform>();
    viewTransform->SetPosition(this->GetViewerPos());
    viewTransform->SetPolar(this->GetViewerAngles());

    float x, y, w, h;
    this->GetViewport(x, y, w, h);
    ncViewport* viewContext = this->refViewport->GetComponentSafe<ncViewport>();
    viewContext->SetViewport(x, y, w, h);
    viewContext->SetCamera(this->GetCamera());

    // move light with camera
    if (this->moveLight)
    {
        ncTransform* lightTransform = this->refLightEntity->GetComponent<ncTransform>();
        lightTransform->SetPosition(this->GetViewerPos());
        lightTransform->SetPolar(this->GetViewerAngles());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nPreviewViewport::OnRender3D()
{
    n_assert(this->IsOpen());
    n_assert(this->refViewport.isvalid());
    n_assert(this->refTargetEntity.isvalid());
    n_assert(this->refLightEntity.isvalid());

    // this only needs to be performed once!
    ncScene *renderContext;

    // attach the viewport
    renderContext = this->refViewport->GetComponentSafe<ncScene>();
    renderContext->SetTime(this->GetTime());
    renderContext->SetFrameId(this->GetFrameId());
    renderContext->Render(this->GetSceneGraph());

    // attach the object
    renderContext = this->refTargetEntity->GetComponentSafe<ncScene>();
    renderContext->SetTime(this->GetTime());
    renderContext->SetFrameId(this->GetFrameId());
    renderContext->Render(this->GetSceneGraph());

    // attach the light
    renderContext = this->refLightEntity->GetComponentSafe<ncScene>();
    renderContext->SetTime(this->GetTime());
    renderContext->SetFrameId(this->GetFrameId());
    renderContext->Render(this->GetSceneGraph());
}

//------------------------------------------------------------------------------
/**
*/
void
nPreviewViewport::OnRender2D()
{
    //render controls to close/maximize the viewport
    //show some statistics for the object, if possible:
    //geometry, cells, etc.
}
