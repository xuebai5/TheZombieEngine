#include "precompiled/pchnapplication.h"
//------------------------------------------------------------------------------
//  nappviewportui_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"

nNebulaScriptClass(nAppViewportUI, "nroot");

//------------------------------------------------------------------------------
/**
*/
nAppViewportUI::nAppViewportUI() :
    singleViewportMode(true),
    isLayoutDirty(true),
    isOpen(false),
    isVisible(false),
    numViewports(0),
    curViewport(0),
    draggingBar(0),
    borderWidth(4.0f)
{
    // Do not save children. This behavior is needed to persist/restore presets.
    this->SetSaveModeFlags(this->N_FLAG_SAVESHALLOW);
}

//------------------------------------------------------------------------------
/**
*/
nAppViewportUI::~nAppViewportUI()
{
    if (this->isOpen)
    {
        this->Close();
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nAppViewportUI::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;

    // set default client rectangle if none set
    if (this->absClientRect.width() == 0.0f && this->absClientRect.height() == 0.0f)
    {
        const nDisplayMode2& displayMode = nGfxServer2::Instance()->GetDisplayMode();
        this->SetClientRect(0, 0, displayMode.GetWidth(), displayMode.GetHeight());
    }

    // load existing viewports
    if (this->GetNumViewports() > 0)
    {
        for (int i = 0; i < this->GetNumViewports(); i++)
        {
            this->viewports[i].GetViewport()->Open();
        }
        this->SetCurrentViewport(this->viewports[0].GetViewport());
    }

    // FIXME! initialize maximized mode
    //this->SwitchMaximized(true);
    this->UpdateLayout();
}

//------------------------------------------------------------------------------
/**
    Shutdown
*/
void 
nAppViewportUI::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;

    // close viewports
    if (this->GetNumViewports() > 0)
    {
        for (int i = 0; i < this->GetNumViewports(); i++)
        {
            this->viewports[i].GetViewport()->Close();
        }
    }

    this->numViewports = 0;
}

//------------------------------------------------------------------------------
/**
    Set client area rectangle in pixels.

    @param 
*/
void
nAppViewportUI::SetClientRect(int x, int y, int width, int height)
{
    this->absClientRect.set(vector2(float(x), float(y)), vector2(float(x + width), float(y + height)));
    this->isLayoutDirty = true;
}

//------------------------------------------------------------------------------
/**
    Get client area rectangle.
    All parameters are in the 0..1 range, relative to the size of the window
*/
void
nAppViewportUI::GetClientRect(int& x, int& y, int& width, int& height)
{
    x = int(this->absClientRect.v0.x);
    y = int(this->absClientRect.v0.y);
    width = int(this->absClientRect.width());
    height = int(this->absClientRect.height());
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewportUI::SetVisible(const bool visible)
{
    if (visible != this->isVisible)
    {
        this->isVisible = visible;

        if (visible)
        {
            this->isLayoutDirty = true;
        }
        else
        {
            //hide all viewports
            int i;
            for (i = 0; i < this->numViewports; i++)
            {
                this->GetViewportAt(i)->SetVisible(false);
            }
        }
        this->VisibleChanged();
    }
}

//------------------------------------------------------------------------------
/**
    Called when the viewport layout has changed the visible attribute.
    Override in subclasses.
*/
void
nAppViewportUI::VisibleChanged()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Set maximized viewport.
*/
void
nAppViewportUI::SetMaximizedViewport(bool /*value*/)
{
    /// @todo REMOVE this function. It wasn't removed for compatibility with old scripts
}

//------------------------------------------------------------------------------
/**
    Set single viewport.
*/
void
nAppViewportUI::SetSingleViewport(bool value)
{
    if (this->singleViewportMode != value)
    {
        //this->SwitchSingleSize();
        this->singleViewportMode = value;

        this->isLayoutDirty = true;
    }
}

//------------------------------------------------------------------------------
/**
    Get single viewport.
*/
bool
nAppViewportUI::GetSingleViewport()
{
    return this->singleViewportMode;
}

//------------------------------------------------------------------------------
/**
    Set current viewport.
*/
void
nAppViewportUI::SetCurrentViewport(nAppViewport* viewport)
{
    n_assert(viewport);
    this->curViewport = viewport;
}


//------------------------------------------------------------------------------
/**
    Set current viewport.
*/
nAppViewport*
nAppViewportUI::GetCurrentViewport()
{
    return this->curViewport;
}

//------------------------------------------------------------------------------
/**
    Set current viewport.
*/
nAppViewport*
nAppViewportUI::FindViewport(const char *name)
{
    for (int i = 0; i < this->numViewports; i++)
    {
        if (this->viewports[i].GetName() == name)
        {
            return this->GetViewportAt(i);
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    GetNumViewports
*/
int
nAppViewportUI::GetNumViewports()
{
    return this->numViewports;
}

//------------------------------------------------------------------------------
/**
    GetNumViewports
*/
nAppViewport*
nAppViewportUI::GetViewportAt(const int index)
{
    n_assert(index < this->numViewports);
    return this->viewports[index].GetViewport();
}

//------------------------------------------------------------------------------
/**
    Clear all viewports and dockbars
*/
void
nAppViewportUI::ClearAll()
{
    //FIXME
    this->numViewports = 0;
    this->dragBars.Clear();
}

//------------------------------------------------------------------------------
/**
    Reset to default state. Reimplement in subclasses.
*/
void 
nAppViewportUI::Reset()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Perform input handling and viewport layout. Reimplement in subclasses.
*/
void
nAppViewportUI::Trigger()
{
    if (this->GetVisible())
    {
        this->UpdateLayout();
    }
}

//------------------------------------------------------------------------------
/**
    Reimplement in subclasses.
*/
void
nAppViewportUI::OnRender3D()
{
    if (!this->GetVisible())
    {
        return;
    }

    int i;
    for (i = 0; i < this->GetNumViewports(); ++i)
    {
        if (this->GetViewportAt(i)->GetVisible())
        {
            this->GetViewportAt(i)->OnRender3D();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Draw viewport GUI: frames around viewports, selected one, etc.
*/
void
nAppViewportUI::OnRender2D()
{
    if (!this->GetVisible())
    {
        return;
    }
/*
    static const vector4 VECTOR_DKGREY = nGfxUtils::color24(94, 92, 108);
    static const vector4 VECTOR_YELLOW = nGfxUtils::color24(225, 225, 0);//(230,160,6); for orange
    static const vector4 VECTOR_RED    = nGfxUtils::color24(225, 0, 0);

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
        this->lineDraw.DrawRectangle2DRelBorder(this->curViewport->GetViewportRect(), 1, 3, VECTOR_YELLOW);

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
    nGfxServer2::Instance()->EndLines();
    */
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewportUI::OnFrameBefore()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppViewportUI::OnFrameRendered()
{
    // empty
}
