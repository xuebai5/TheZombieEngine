#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nconjurerlayout_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nconjurerlayout.h"
#include "napplication/napplication.h"
#include "napplication/nappviewport.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxutils.h"
#include "nspatial/nspatialserver.h"

nNebulaScriptClass(nConjurerLayout, "nappviewportui");

//------------------------------------------------------------------------------
/**
*/
nConjurerLayout::nConjurerLayout() :
    previewEntry(this, "preview")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nConjurerLayout::~nConjurerLayout()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Restore all viewports to default parameters.
*/
void
nConjurerLayout::Reset()
{
    int i;
    for (i = 0; i < this->GetNumViewports(); i++)
    {
        nAppViewport* viewport = this->GetViewportAt(i);
        viewport->SetViewerPos(this->GetDefViewerPos());
        viewport->SetViewerAngles(this->GetDefViewerAngles());
        viewport->SetViewerZoom(this->GetDefViewerZoom());
        viewport->SetViewerVelocity(this->GetDefViewerVelocity());
    }
}

//------------------------------------------------------------------------------
/**
    Restore all viewports to default parameters.
*/
void
nConjurerLayout::Trigger()
{
    if (this->HandleInput())
    {
        nApplication::Instance()->SetInputHandled(true);
    }

    nAppViewportUI::Trigger();
}

//------------------------------------------------------------------------------
/**
    set which viewport is the preview one
*/
void
nConjurerLayout::VisibleChanged()
{
    this->GetPreviewViewport()->SetVisible(this->GetVisible());
}

//------------------------------------------------------------------------------
/**
    set which viewport is the preview one
*/
void
nConjurerLayout::SetPreviewViewport(const char *name)
{
    nString viewportName(name);
    this->previewEntry.SetName(viewportName);
}

//------------------------------------------------------------------------------
/**
    Restore all viewports to default parameters.
*/
nAppViewport*
nConjurerLayout::GetPreviewViewport()
{
    return this->previewEntry.GetViewport();
}

//------------------------------------------------------------------------------
/**
*/
bool
nConjurerLayout::HandleInput()
{
    bool processed = false;

    this->UpdateLayout();

    nInputServer* inputServer = nInputServer::Instance();
    vector2 mousePos = inputServer->GetMousePos();

    // reset the current viewport
    if (inputServer->GetButton("reset"))
    {
        nAppViewport* viewport = this->GetCurrentViewport();

        viewport->SetViewerPos(this->GetDefViewerPos());
        viewport->SetViewerAngles(this->GetDefViewerAngles());
        viewport->SetViewerZoom(this->GetDefViewerZoom());
        viewport->SetViewerVelocity(this->GetDefViewerVelocity());
    }

    // Handle single viewport Mode
    if (inputServer->GetButton("viewport_single_switch"))
    {
        if (!this->draggingBar)
        {
            this->SetSingleViewport(!this->GetSingleViewport());
            processed = true;
        }
    }

    // Toggle edit indoor mode on/off
    if (inputServer->GetButton("toggle_indoor_mode"))
    {
        nSpatialServer::Instance()->ToggleEditIndoor();
    }

    // Toggle see all indoors mode on/off
    if (inputServer->GetButton("toggle_all_indoor_mode"))
    {
        nSpatialServer::Instance()->ToggleEditAllSelectedIndoors();
    }

    // VIEWPORT HANDLING
    nAppViewport* preview = this->GetPreviewViewport();

    if (preview && preview->GetVisible() && processed)
    {
        preview->SetVisible(false);
    }

    // Handle viewport selection (left-click)
    if (inputServer->GetButton("buton0_down"))
    {
        // hide the preview viewport if left-click outside it
        if (preview && preview->GetVisible() && 
            !preview->GetRelSize().inside(mousePos))
        {
            preview->SetVisible(false);
        }

        if (!this->singleViewportMode && !this->draggingBar)
        {
            if (this->relClientRect.inside(mousePos))
            {
                // check for viewport selection
                for (int i = 0; i < this->numViewports; i++) 
                {
                    nAppViewport* viewport = this->GetViewportAt(i);
                    if (viewport->GetVisible() && 
                        viewport->GetRelSize().inside(mousePos))
                    {
                        this->SetCurrentViewport(viewport);
                        processed = true;
                        break;
                    }
                }
            }
        }

        this->draggingBar = 0;
    }

    // pressing Left Mouse Button
    if (inputServer->GetButton("buton0"))
    {
        // if we were dragging viewports, set new viewports size
        if (this->draggingBar && this->relClientRect.inside(mousePos))
        {
            // find out new knobs (0..1 relative to singleViewportRect)
            // check mouse move distance
            vector2 mouseDist = mousePos - this->draggingBarStart;

            uint max, may;
            float kx, ky;
            max = nGfxUtils::absx(mousePos.x);
            may = nGfxUtils::absy(mousePos.y);

            max -= nGfxUtils::absx(this->relClientRect.v0.x);
            may -= nGfxUtils::absy(this->relClientRect.v0.y);
            kx = nGfxUtils::relx(max, nGfxUtils::absx(this->relClientRect.width()));
            ky = nGfxUtils::rely(may, nGfxUtils::absy(this->relClientRect.height()));

            if (kx != 0.0f && ky != 0.0f)
            {
                this->draggingBarStart = mousePos;
         
                float max_kx = nGfxUtils::relx(4);
                float max_ky = nGfxUtils::rely(4);

                if (kx != 0.0f)
                {
                    if (kx <= max_kx)       kx = max_kx;
                    if (kx < 0.0f)          kx = max_kx;
                    if (kx > 1.0f - max_kx) kx = 1.0f - max_kx;
                }
                
                if (ky != 0.0f)
                {
                    if (ky <= max_ky)       ky = max_ky;
                    if (ky < 0.0f)          ky = max_ky;
                    if (ky > 1.0f - max_ky) ky = 1.0f - max_ky;
                }

                if (draggingBar->type == DRAG_VERTICAL)
                {
                    draggingBar->knob = kx;
                }
                else if (draggingBar->type == DRAG_HORIZONTAL)
                {
                    draggingBar->knob = ky;
                }

                this->isLayoutDirty = true;
            }
        }
    }

    // first left-mouse button down
    if (inputServer->GetButton("buton0_down"))
    {
        if (!this->singleViewportMode) // do not handle dragging on singleViewportMode
        {
            if (this->relClientRect.inside(mousePos))
            {
                for (int i = 0; i < this->dragBars.Size(); i++)
                {
                    if (this->dragBars[i].rect.inside(mousePos))
                    {
                        this->draggingBar = &this->dragBars[i];
                    }
                }

                if (this->draggingBar != 0)
                {
                    this->draggingBarStart = mousePos;
                }
            }
        }
    }

    return processed;
}

//------------------------------------------------------------------------------
/**
*/
void
nConjurerLayout::OnRender2D()
{
    static const vector4 VECTOR_DKGREY = nGfxUtils::color24(94, 92, 108);
    static const vector4 VECTOR_YELLOW = nGfxUtils::color24(225, 225, 0);//(230,160,6); for orange
    static const vector4 VECTOR_RED    = nGfxUtils::color24(225, 0, 0);
    static const vector4 VECTOR_BLUE   = nGfxUtils::color24(0, 0, 255);

    nGfxServer2::Instance()->BeginLines();

    if (!this->singleViewportMode)
    {
        // FIXME check for docked viewports
        //this->DrawRectangle(this->clientRect, 0, 0, COLOR_DKGREY);
        //for (int i = 0; i < this->numViewports; i++)
        //{
        //    this->DrawRectangle(this->viewports[i]->GetViewportRect(), 0, 0, COLOR_DKGREY);
        //}
        //draw rectangle around current viewport
        this->lineDraw.DrawRectangle2DRelBorder(this->GetCurrentViewport()->GetRelSize(), 1, 3, VECTOR_YELLOW);

        //draw dragbars when dragging or highlighted
        vector2 mousePos = nInputServer::Instance()->GetMousePos();
        //if (this->clientRect.inside(mousePos))
        {
            //bool isMouseOverGui = nGuiServer::Instance()->IsMouseOverGui();
            //if (!isMouseOverGui)
            //{
                DragBar* highlight = 0;
                for (int i = 0; i < this->dragBars.Size(); i++)
                {
                    if (dragBars[i].rect.inside(mousePos))
                    {
                        highlight = &dragBars[i];
                        break;
                    }
                }

                if (nGfxServer2::Instance()->GetCursorVisibility() != nGfxServer2::None)
                {
                    if (this->draggingBar)
                    {
                        this->lineDraw.DrawRectangle2DRelBorder(this->draggingBar->rect, -3, -1, VECTOR_RED);
                    }
                    
                    else if (highlight)
                    {
                        this->lineDraw.DrawRectangle2DRelBorder(highlight->rect, -3, -1, VECTOR_RED);
                    }
                }
            //}
        }
    }

    // render rectangle aroung preview viewport
    nAppViewport *preview = this->GetPreviewViewport();
    if (preview && preview->GetVisible())
    {
        this->lineDraw.DrawRectangle2DRelBorder(preview->GetRelSize(), -2, 0, VECTOR_BLUE);
    }

    nGfxServer2::Instance()->EndLines();
}
