#include "precompiled/pchngui.h"
//-----------------------------------------------------------------------------
//  nGuiToggleButton_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "gui/nGuiToggleButton.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiToggleButton, "nguiwidget");

//-----------------------------------------------------------------------------
/**
*/
nGuiToggleButton::nGuiToggleButton() :
    focus(false),
    pressed(false)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
nGuiToggleButton::~nGuiToggleButton()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    If mouse is over sensitive area, set the focus flag. Clear focus and
    pressed flag otherwise.
*/
void
nGuiToggleButton::OnFrame()
{
    const vector2 mousePos = nGuiServer::Instance()->GetMousePos();
    if (this->Inside(mousePos) && this->GetOwnerWindow()->HasFocus())
    {
        this->focus = true;
    }
    else
    {
        if (!this->IsStickyMouse())
        {
            this->focus = false;
            //this->pressed = false;
        }
    }
    nGuiWidget::OnFrame();
}

//-----------------------------------------------------------------------------
/**
    If button has focus, set the pressed flag.
*/
bool
nGuiToggleButton::OnButtonDown(const vector2& mousePos)
{
    if (this->Inside(mousePos) && this->GetOwnerWindow()->HasFocus())
    {
        this->focus = true;
        nGuiWidget::OnButtonDown(mousePos);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    If pressed flag is set, execute the associated command.
*/
bool
nGuiToggleButton::OnButtonUp(const vector2& mousePos)
{
    if (this->focus)
    {
        //this->pressed = false;
        this->pressed = !this->pressed;
        this->focus = false;
        nGuiWidget::OnButtonUp(mousePos);
        this->OnAction();
    }
    return true;
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiToggleButton::Render()
{
    if (this->IsShown())
    {
        nGuiBrush* brush = &this->defaultBrush;
        if (this->pressed)
        {
            brush = &this->pressedBrush;
        }
        if (this->blinking)
        {
            double time = nGuiServer::Instance()->GetTime();
            // just a little hack to allow leaving a button at highlighted state (set blinkRate to Zero)
            if (! this->blinkRate ||
                fmod(time, this->blinkRate) > this->blinkRate/1.3)
            {
                brush = &this->defaultBrush;
            }
        }
        nGuiServer::Instance()->DrawBrush(this->GetScreenSpaceRect(), *brush);
        return true;
    }
    return false;
}

