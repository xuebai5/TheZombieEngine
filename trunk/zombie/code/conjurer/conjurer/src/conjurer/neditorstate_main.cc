#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  neditorstate_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/neditorstate.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/neditorviewport.h"
#include "napplication/nappviewportui.h"
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "nscene/nscenegraph.h"
#include "nscene/ntransformnode.h"
#include "nspatial/ncspatialcamera.h"
#include "audio3/nlistener3.h"
#include "nnetworkmanager/nnetworkmanager.h"
#include "nsoundscheduler/nsoundscheduler.h"

nNebulaScriptClass(nEditorState, "ncommonstate");

//------------------------------------------------------------------------------
/**
*/
nEditorState::nEditorState() :
    watchViewerPos("viewerPos", nArg::Float4),
    watchFov("viewerFov", nArg::Float),
    watchVel("viewerVelocity", nArg::Float),
    watchVisObjects("viewerVisObjects", nArg::Int),
    watchVisCells("viewerVisTerrainCells", nArg::Int)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nEditorState::~nEditorState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
nEditorState::OnCreate(nApplication* app)
{
    nCommonState::OnCreate(app);

    this->refViewportUI = (nAppViewportUI *) app->Find("appviewportui");
    n_assert(this->refViewportUI.isvalid());
    
    this->refPreview = (nAppViewport*) this->refViewportUI->Find("preview");
    n_assert(this->refPreview.isvalid());

    this->refMapView = static_cast<nAppViewport*>(kernelServer->Lookup("/usr/mapview"));
    n_assert(this->refMapView.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorState::OnStateEnter(const nString& prevState)
{
    this->refViewportUI->SetVisible(true);//make all viewports visible

    nSoundScheduler::Instance()->SetHearingViewport( this->refViewportUI->GetCurrentViewport() );

    nCommonState::OnStateEnter(prevState);
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorState::OnStateLeave(const nString& nextState)
{
    if (nextState != "")
    {
        // set null hearing viewport to sound scheduler
        nSoundScheduler::Instance()->SetHearingViewport( 0 );

        nAppState* nextAppState = this->app->FindState(nextState);
        //turn off all editor viewport when switching to a game mode
        if (nextAppState && !nextAppState->IsA("neditorstate"))
        {
            this->refViewportUI->SetVisible(false);//make all viewports visible
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorState::OnFrame()
{
    // handle input for the current state
    if (!this->app->GetInputHandled())
    {
        if (this->HandleInput(this->app->GetFrameTime()))
        {
            this->app->SetInputHandled(true);
        }
    }

    // editor viewport
    nAppViewport* curViewport = this->refViewportUI->GetCurrentViewport();

    // set hearing viewport to sound scheduler
    nSoundScheduler::Instance()->SetHearingViewport( curViewport );

    // update frame time and frame id for all visible viewports
    int i;
    int numViewports = this->refViewportUI->GetNumViewports();
    for (i = 0; i < numViewports; i++)
    {
        nAppViewport *viewport = this->refViewportUI->GetViewportAt(i);
        if (viewport->GetVisible())
        {
            viewport->SetFrameId(this->app->GetFrameId());
            viewport->SetFrameTime(this->app->GetFrameTime());
            viewport->SetTime(this->app->GetTime());
            viewport->Trigger();
        }
    }

    // update preview viewport --FIXME move to nConjurerLayout::Trigger()
    if (this->refPreview->GetVisible())
    {
        this->refPreview->SetFrameId(this->app->GetFrameId());
        this->refPreview->SetFrameTime(this->app->GetFrameTime());
        this->refPreview->SetTime(this->app->GetTime());
        this->refPreview->Trigger();
    }

    // update map viewport --FIXME move to nConjurerLayout::Trigger()
    if (this->refMapView->GetVisible())
    {
        this->refMapView->SetFrameId(this->app->GetFrameId());
        this->refMapView->SetFrameTime(this->app->GetFrameTime());
        this->refMapView->SetTime(this->app->GetTime());
        this->refMapView->Trigger();
    }

    if( nNetworkManager::Instance() )
    {
        nNetworkManager::Instance()->Trigger( this->app->GetTime() );
    }


    // run physics if enabled
    if (this->GetApp()->IsPhysicsEnabled())
    {
        nPhysicsServer::Instance()->Run(phyreal(this->app->GetFrameTime()));
    }

    // update viewer* watchers
    const vector3& pos = curViewport->GetViewerPos();
    const polar2& angles = curViewport->GetViewerAngles();
    this->watchViewerPos->SetV4(vector4(pos.x, pos.y, pos.z, n_rad2deg(angles.rho)));
    this->watchVel->SetF(curViewport->GetViewerVelocity() * 3600.0f / 1000.0f); // convert m/s to km/h
    this->watchFov->SetF(curViewport->GetAngleOfView());

    // assume current viewport is a neditorviewport
    ncSpatialCamera* camera = static_cast<nEditorViewport*>(curViewport)->GetSpatialCamera();
    this->watchVisObjects->SetI(camera->GetNumVisibleObjects());
    this->watchVisCells->SetI(camera->GetNumVisibleTerrainCells());

    nCommonState::OnFrame();
}

//------------------------------------------------------------------------------
/**
    Perform scene attaching.
*/
void
nEditorState::OnRender3D()
{
    if (!nConjurerApp::Instance()->IsAttachEnabled())
    {
        return;
    }

    // render all visible viewports
    // FIXME move to nConjurerLayout::OnRender3D()
    int i;
    int numViewports = this->refViewportUI->GetNumViewports();
    for (i = 0; i < numViewports; i++)
    {
        nAppViewport *viewport = this->refViewportUI->GetViewportAt(i);
        if (viewport->GetVisible())
        {
            viewport->OnRender3D();
        }
    }

    // render preview viewport --FIXME
    if (this->refPreview->GetVisible())
    {
        this->refPreview->OnRender3D();
    }

    // render map viewport --FIXME
    if (this->refMapView->GetVisible())
    {
        this->refMapView->OnRender3D();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nEditorState::OnRender2D()
{
    nGfxServer2 *gfxServer = nGfxServer2::Instance();

    if (!this->GetApp()->captureFrame)
    {
        // get ready a full-window viewport to render console and GUI in it
        nViewport& vp = gfxServer->GetViewport();
        vp.x = vp.y = 0.0f;
        vp.width = (float) gfxServer->GetDisplayMode().GetWidth();
        vp.height = (float) gfxServer->GetDisplayMode().GetHeight();
        gfxServer->SetViewport(vp);

        // some more drawing...
        this->GetApp()->refViewportUI->OnRender2D(); // do it after endscene

        // render preview as overlay --FIXME
        if (this->refPreview->GetVisible())
        {
            this->refPreview->OnRender2D();
        }

        // render map as overlay --FIXME
        if (this->refMapView->GetVisible())
        {
            this->refMapView->OnRender2D();
        }
    }

    //this->GetApp()->fps.Trigger(this->app->GetTime());
}
