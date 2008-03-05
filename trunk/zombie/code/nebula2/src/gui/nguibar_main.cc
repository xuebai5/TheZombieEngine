#include "precompiled/pchngui.h"
//-----------------------------------------------------------------------------
//  nguibar_main.cc
//  (C) 2005 Radon Labs GmbH
//-----------------------------------------------------------------------------
#include "gui/nguibar.h"
#include "gui/nguiresource.h"
#include "gui/nguiserver.h"
#include "gui/nguiwindow.h"

nNebulaScriptClass(nGuiBar, "nguiwidget");

//-----------------------------------------------------------------------------
/**
*/
nGuiBar::nGuiBar() : 
    fillMode(LeftToRight),
    fillState(0.0f),
    fillStateSegments(0)
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
nGuiBar::~nGuiBar()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiBar::Render()
{
    if (this->IsShown() && nGuiWidget::Render())
    {
        // Background brush.
        nTexture2* texture = this->disabledBrush.GetGuiResource()->GetTexture();
        n_assert(texture != 0);
        nGuiServer::Instance()->DrawTexture(this->GetScreenSpaceRect(), this->disabledBrush.GetGuiResource()->GetRelUvRect(), this->disabledBrush.GetGuiResource()->GetColor(), texture);

        // Default brush.
        texture = this->defaultBrush.GetGuiResource()->GetTexture();
        n_assert(texture != 0);

        // Compute screen rect and uv rect based on current fill state.
        rectangle screenRect = this->GetScreenSpaceRect();
        rectangle uvRect = this->defaultBrush.GetGuiResource()->GetRelUvRect();
        float visibleFillState;
        if (this->IsFillStateSegmented())
        {
            visibleFillState = SegmentedFillState(this->fillState);
        }
        else
        {
            visibleFillState = this->fillState;
        }

        switch (this->fillMode)
        {
        case TopToBottom:
            screenRect.v1.y = screenRect.v0.y + (screenRect.v1.y - screenRect.v0.y) * visibleFillState;
            uvRect.v0.y = uvRect.v1.y - (uvRect.v1.y - uvRect.v0.y) * visibleFillState;
            break;

        case BottomToTop:
            screenRect.v0.y = screenRect.v1.y - (screenRect.v1.y - screenRect.v0.y) * visibleFillState;
            uvRect.v1.y = uvRect.v0.y + (uvRect.v1.y - uvRect.v0.y) * visibleFillState;
            break;

        case RightToLeft:
            screenRect.v0.x = screenRect.v1.x - (screenRect.v1.x - screenRect.v0.x) * visibleFillState;
            uvRect.v0.x = uvRect.v1.x - (uvRect.v1.x - uvRect.v0.x) * visibleFillState;
            break;

        default:
        case LeftToRight:
            screenRect.v1.x = screenRect.v0.x + (screenRect.v1.x - screenRect.v0.x) * visibleFillState;
            uvRect.v1.x = uvRect.v0.x + (uvRect.v1.x - uvRect.v0.x) * visibleFillState;
            break;
        }

        nGuiServer::Instance()->DrawTexture(screenRect, uvRect, this->defaultBrush.GetGuiResource()->GetColor(), texture);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
void
nGuiBar::SetFillMode(nGuiBar::FillMode mode) //FillMode mode)
{
    this->fillMode = mode;
}
//-----------------------------------------------------------------------------
//nGuiBar::FillMode
nGuiBar::FillMode
nGuiBar::GetFillMode() const
{
    return this->fillMode;
}
//-----------------------------------------------------------------------------

void
nGuiBar::SetFillState(float v)
{
    n_assert(0.0f <= v && v <= 1.0f);
    this->fillState = v;
}
//-----------------------------------------------------------------------------
float
nGuiBar::GetFillState() const
{
    n_assert(0.0f <= this->fillState && this->fillState <= 1.0f);
    return this->fillState;
}
//-----------------------------------------------------------------------------
void
nGuiBar::SetFillStateSegments(int v)
{
    n_assert(v >= 0);
    this->fillStateSegments = v;
}
//-----------------------------------------------------------------------------
int
nGuiBar::GetFillStateSegments() const
{
    n_assert(this->fillStateSegments >= 0);
    return this->fillStateSegments;
}
//-----------------------------------------------------------------------------
bool
nGuiBar::IsFillStateSegmented() const
{
    return this->fillStateSegments > 0;
}

//-----------------------------------------------------------------------------
/**
*/
float
nGuiBar::SegmentedFillState(float v) const
{
    n_assert(0.0f <= v && v <= 1.0f);
    n_assert(this->IsFillStateSegmented());

    float segmentSize = 1.0f / this->fillStateSegments;
    int segmentCount = static_cast<int>(v / segmentSize);
    return static_cast<float>(segmentCount * segmentSize);
}
