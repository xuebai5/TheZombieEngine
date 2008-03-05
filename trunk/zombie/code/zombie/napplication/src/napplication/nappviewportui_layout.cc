#include "precompiled/pchnapplication.h"
//------------------------------------------------------------------------------
//  nappviewportui_layout.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"
#include "gfx2/ngfxutils.h"

//------------------------------------------------------------------------------
/**
    Open Dragbar

    @param  name    Name of the dragbar to open. Useful for docking viewports
    @param  align   "vertical", "horizontal"
    @param  knob    initial knob, must be in the 0..1 range, clamped otherwise
*/
void
nAppViewportUI::OpenDragbar(const char *name, const char *type, float knob)
{
    // check bar doesn't exist
    DragBar* dragBar = this->FindDragBar(name);
    if (dragBar)
    {
        n_printf("nAppViewportUI::OpenDragbar: DragBar '%s' already exists\n", name);
        return;
    }

    // check drag type
    DragBarType dragBarType;
    if (0 == strcmp(type, "vertical"))
    {
        dragBarType = DRAG_VERTICAL;
    }
    else if (0 == strcmp(type, "horizontal"))
    {
        dragBarType = DRAG_HORIZONTAL;
    }
    else
    {
        n_printf("nAppViewportUI::OpenDragbar: invalid bar type: '%s'\n", type);
        return;
    }

    // add the dragbar to the list
    DragBar newDragBar;
    newDragBar.name = name;
    newDragBar.knob = knob;
    newDragBar.type = dragBarType;
    newDragBar.numDockedViewports = 0;
    dragBars.Append(newDragBar);
}

//------------------------------------------------------------------------------
/**
    Close Dragbar

    @param  name    Name of the dragbar to close
*/
void
nAppViewportUI::CloseDragbar(const char *name)
{
    // check the dragbar name does exist
    DragBar *dragBar = this->FindDragBar(name);
    if (!dragBar)
    {
        n_printf("nAppViewportUI::CloseDragbar: Dragbar '%s' not found.\n", name);
        return;
    }

    // TODO: docked viewports must be docked to the parent dragbar?

    // release the dragbar
    for (int i = 0; i < this->dragBars.Size(); i++)
    {
        if (this->dragBars[i].name == nString(name))
        {
            this->dragBars.Erase(i);
        }
    }

    this->isLayoutDirty = true;
}

//------------------------------------------------------------------------------
/**
    Close Dragbar

    @param  name    Name of the dragbar to close
*/
void
nAppViewportUI::SetDragbarKnob(const char *name, float knob)
{
    // check the dragbar name does exist
    DragBar *dragBar = this->FindDragBar(name);
    if (!dragBar)
    {
        n_printf("nAppViewportUI::SetDragbarKnob: Dragbar '%s' not found.\n", name);
        return;
    }

    dragBar->knob = knob;
    this->isLayoutDirty = true;
}

//------------------------------------------------------------------------------
/**
    Close Dragbar

    @param  name    Name of the dragbar to close
*/
float
nAppViewportUI::GetDragbarKnob(const char *name)
{
    // check the dragbar name does exist
    DragBar *dragBar = this->FindDragBar(name);
    if (!dragBar)
    {
        n_printf("nAppViewportUI::SetDragbarKnob: Dragbar '%s' not found.\n", name);
        return 0.0f;
    }

    return dragBar->knob;
}

//------------------------------------------------------------------------------
/**
    Open a new viewport

    @param  name    Name of the viewport to open
*/
void
nAppViewportUI::AddViewport(const char *name)
{
    for (int i = 0; i < this->numViewports; i++)
    {
        if (this->viewports[i].GetName() == name)
        {
            n_printf("nAppViewportUI::AddViewport: viewport '%s' already exists\n", name);
            return;
        }
    }

    if (this->numViewports == MaxViewports)
    {
        n_printf("nAppViewportUI::AddViewport: Max %d viewports allowed\n", MaxViewports);
        return;
    }
    this->viewports[this->numViewports] = ViewportEntry(this, name);
    ++this->numViewports;
}

//------------------------------------------------------------------------------
/**
    Open a new viewport

    @param  name    Name of the viewport to open
*/
//void
//nAppViewportUI::OpenViewport(const char *name)
//{
//    if (this->Find(name))
//    {
//        n_printf("nAppViewportUI::OpenViewport: Viewport '%s' already exists\n", name);
//        return;
//    }
//
//    if (this->numViewports == MaxViewports)
//    {
//        n_printf("nAppViewportUI::OpenViewport: Max %d viewports allowed\n", MaxViewports);
//        return;
//    }
//
//    int x, y, width, height;
//    this->GetClientRect(x, y, width, height);
//
//    kernelServer->PushCwd(this);
//    nAppViewport *newvp = (nAppViewport *) kernelServer->New("nappviewport", name);
//    //TODO assign default settings
//    //newvp->SetParameters(this->defViewerVelocity, this->defViewerAngles, this->defViewerPos, this->defViewerZoom);
//    newvp->SetViewport((float) x, (float) y, (float) width, (float) height);
//    newvp->Open();
//    kernelServer->PopCwd();
//
//    this->AddViewport(name);
//    this->isLayoutDirty = true;
//}

//------------------------------------------------------------------------------
/**
    Dock a viewport to a dragbar

    @param  name    Name of the viewport to open
    @param  dragbar Name of the dragbar where to dock the viewport
    @param  side    Position of the viewport in dragbar space:
                    "left", "right" for vertical dragbars
                    "up", "down" for horizontal dragbars
*/
void
nAppViewportUI::DockViewport(const char *name, const char *barname, const char *side)
{
    DragBar* dragBar = this->FindDragBar(barname);
    if (!dragBar)
    {
        n_printf("nAppViewportUI::DockViewport: Dragbar '%s' not found\n", barname);
        return;
    }

    DockSide dockSide;
    if (0 == strcmp(side, "up"))
    {
        dockSide = SIDE_UP;
    }
    else if (0 == strcmp(side, "down"))
    {
        dockSide = SIDE_DOWN;
    }
    else if (0 == strcmp(side, "left"))
    {
        dockSide = SIDE_LEFT;
    }
    else if (0 == strcmp(side, "right"))
    {
        dockSide = SIDE_RIGHT;
    }
    else
    {
        n_printf("nAppViewportUI::DockViewport: invalid dock side: '%s'\n", side);
        return;
    }

    // check whether the dragbar and the drag side are compatible
    if (dragBar->type == DRAG_HORIZONTAL)
    {
        if (dockSide != SIDE_UP && dockSide != SIDE_DOWN)
        {
            n_printf("nAppViewportUI::DockViewport: invalid dock side: '%s' for dragbar '%s'\n", side, barname);
            return;
        }
    }
    else if (dragBar->type == DRAG_VERTICAL)
    {
        if (dockSide != SIDE_LEFT && dockSide != SIDE_RIGHT)
        {
            n_printf("nAppViewportUI::DockViewport: invalid dock side: '%s' for dragbar '%s'\n", side, barname);
            return;
        }
    }

    // check whether the viewport is already docked
    for (int i = 0; i < dragBar->numDockedViewports; i++)
    {
        if (dragBar->docks[i].viewportName == name)
        {
            dragBar->docks[i].side = dockSide;
            this->isLayoutDirty = true;
            return;
        }
    }
    DockedViewport newDock;
    newDock.side = dockSide;
    newDock.viewportName = name;
    dragBar->docks[dragBar->numDockedViewports] = newDock;
    ++dragBar->numDockedViewports;

    this->isLayoutDirty = true;
}

//------------------------------------------------------------------------------
/**
    Undock a viewport from a dragbar

    @param  name    Name of the viewport to open
    @param  dragbar Name of the dragbar where to dock the viewport
*/
void
nAppViewportUI::UndockViewport(const char *name)
{
    for (int i = 0; i < this->dragBars.Size(); i++)
    {
        DragBar& dragBar = this->dragBars[i];
        for (int j = 0; j < dragBar.numDockedViewports; j++)
        {
            DockedViewport& docked = dragBar.docks[j];
            if (docked.viewportName == name)
            {
                while (j < dragBar.numDockedViewports - 2)
                {
                    dragBar.docks[j] = dragBar.docks[j + 1];
                    j++;
                }
                --dragBar.numDockedViewports;
                break;
            }
        }
    }

    this->isLayoutDirty = true;
}

//------------------------------------------------------------------------------
/**
    Close a viewport

    @param  name    Name of the viewport to close.
*/
void
nAppViewportUI::CloseViewport(const char *name)
{
    nAppViewport* vp = (nAppViewport*) this->Find(name);
    if (!vp)
    {
        n_printf("nAppViewportUI::CloseViewport: Viewport '%s' not found.\n", name);
        return;
    }

    // remove from any dragbar that references it
    this->UndockViewport(name);

    // remove from list
    for (int i = 0; i < this->numViewports; i++)
    {
        if (this->GetViewportAt(i) == vp)
        {
            for (int j = i; j < this->numViewports - 1; j++)
            {
                this->viewports[j] = this->viewports[j + 1];
            }
            break;
        }
    }
    --this->numViewports;

    vp->Release();
    
    this->isLayoutDirty = true;
}

//------------------------------------------------------------------------------
/**
    Recompute all viewport rects from the current layout.
*/
void
nAppViewportUI::UpdateLayout()
{
    n_assert(this->isOpen);

    if (!this->isLayoutDirty)
    {
        return;
    }

    // compute the relative client area, if it has been assigned from script
    this->relClientRect.v0.x = nGfxUtils::relx(int(this->absClientRect.v0.x));
    this->relClientRect.v0.y = nGfxUtils::rely(int(this->absClientRect.v0.y));
    this->relClientRect.v1.x = nGfxUtils::relx(int(this->absClientRect.v1.x));
    this->relClientRect.v1.y = nGfxUtils::rely(int(this->absClientRect.v1.y));

    // compute the total client area
    float x = this->absClientRect.v0.x;
    float y = this->absClientRect.v0.y;
    float width = this->absClientRect.width();
    float height = this->absClientRect.height();

    // if the current viewport is maximized, all others become invisible,
    // no dragbars are visible and the viewport area covers the entire client area.
    //if (this->curViewport->IsMaximized())
    if (this->singleViewportMode)
    {
        for (int i = 0; i < this->numViewports; i++)
        {
            this->GetViewportAt(i)->SetVisible(this->GetViewportAt(i) == this->curViewport);
        }
        this->curViewport->SetViewport(x, y, width, height);
        this->isLayoutDirty = false;
        return;
    }

    // compute the client area excluding borders (for multi-viewport)
    x += this->borderWidth;
    y += this->borderWidth;
    width -= 2.0f * this->borderWidth;
    height -= 2.0f * this->borderWidth;

    // initialize all viewports to cover the entire client area
    for (int i = 0; i < this->numViewports; i++)
    {
        nAppViewport *viewport = this->GetViewportAt(i);
        viewport->SetViewport(x, y, width, height);
    }

    // and now apply the dock constraints
    for (int i = 0; i < this->dragBars.Size(); i++)
    {
        DragBar& dragBar = this->dragBars[i];
        for (int j = 0; j < dragBar.numDockedViewports; j++)
        {
            float x0, y0, x1, y1, w, h;
            DockedViewport& docked = dragBar.docks[j];
            if (!docked.refViewport.isvalid())
            {
                docked.refViewport = (nAppViewport *) this->Find(docked.viewportName.Get());
            }

            if (docked.refViewport.isvalid())
            {
                docked.refViewport->GetViewport(x0, y0, w, h);
                x1 = x0 + w;
                y1 = y0 + h;
                if (dragBar.type == DRAG_VERTICAL)
                {
                    if (docked.side == SIDE_LEFT)
                    {
                        //right side of the viewport is docked to the dragbar
                        x1 = x + (float) nGfxUtils::absx(dragBar.knob, (ushort) width);
                    }
                    else if (docked.side == SIDE_RIGHT)
                    {
                        //left side of the viewport if docked to the dragbar
                        x0 = x + (float) nGfxUtils::absx(dragBar.knob, (ushort) width) + this->borderWidth;
                    }
                }
                else if (dragBar.type == DRAG_HORIZONTAL)
                {
                    if (docked.side == SIDE_UP)
                    {
                        //bottom side of the viewport is docked to the dragbar
                        y1 = y + (float) nGfxUtils::absy(dragBar.knob, (ushort) height);
                    }
                    else if (docked.side == SIDE_DOWN)
                    {
                        //top side of the viewport is docked to the dragbar
                        y0 = y + (float) nGfxUtils::absy(dragBar.knob, (ushort) height) + this->borderWidth;
                    }
                }
                docked.refViewport->SetViewport(x0, y0, x1 - x0, y1 - y0);
                docked.refViewport->SetVisible(x0 < x1 && y0 < y1);
            }
        }
    }

    // compute rect for dragging the bar using the docked viewports only
    for (int i = 0; i < this->dragBars.Size(); i++)
    {
        DragBar& dragBar = this->dragBars[i];
        dragBar.rect.set(vector2(0.0f, 0.0f), vector2(0.0f, 0.0f));

        // FIXME check for docked viewports first
        //for (int j = 0; j < dragBar.numDockedViewports; j++)
        //{
        //    float x0, y0, width, height;
        //    float x1, y1;
        //    DockedViewport& docked = dragBar.docks[j];
        //    docked.viewport->GetViewport(x0, y0, width, height);
        //    x1 = x0 + width;
        //    y1 = y0 + height;
            if (dragBar.type == DRAG_VERTICAL)
            {
                dragBar.rect.v0.x = nGfxUtils::relx(ushort(x + (float) nGfxUtils::absx(dragBar.knob, (ushort) width)));
                dragBar.rect.v0.y = nGfxUtils::rely(ushort(y));
                dragBar.rect.v1.x = nGfxUtils::relx(ushort(x + (float) nGfxUtils::absx(dragBar.knob, (ushort) width) + this->borderWidth));
                dragBar.rect.v1.y = nGfxUtils::rely(ushort(y + height));

                //if (docked.side == SIDE_LEFT)
                //else if (docked.side == SIDE_RIGHT)
            }
            else if (dragBar.type == DRAG_HORIZONTAL)
            {
                dragBar.rect.v0.x = nGfxUtils::relx(ushort(x));
                dragBar.rect.v0.y = nGfxUtils::rely(ushort(y + (float) nGfxUtils::absy(dragBar.knob, (ushort) height)));
                dragBar.rect.v1.x = nGfxUtils::relx(ushort(x + width));
                dragBar.rect.v1.y = nGfxUtils::rely(ushort(y + (float) nGfxUtils::absy(dragBar.knob, (ushort) height) + this->borderWidth));

                //if (docked.side == SIDE_UP)
                //else if (docked.side == SIDE_DOWN)
            }
        //}
    }

    this->isLayoutDirty = false;
}
