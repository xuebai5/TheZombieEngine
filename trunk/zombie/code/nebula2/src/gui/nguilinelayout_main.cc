#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguilinelayout_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "gui/nguilinelayout.h"
#include "gui/nguicheckbutton.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiLineLayout, "nguiformlayout");

//------------------------------------------------------------------------------
/**
*/
nGuiLineLayout::nGuiLineLayout() :
    orientation( ORI_HORIZONTAL ),
    order( ORDER_NORMAL ),
    aligned( true )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiLineLayout::~nGuiLineLayout()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Set horizontal or vertical orientation of layout
*/
void
nGuiLineLayout::SetOrientation( Orientation orientation )
{
    this->orientation = orientation;
}

//------------------------------------------------------------------------------
/**
    @brief Set normal or reversed order of child widgets
*/
void
nGuiLineLayout::SetOrder( Order order)
{
    this->order = order;
}

//------------------------------------------------------------------------------
/**
    @brief Set aligned
    @param aligned Aligned flag. False means childs are centered in the other axis
*/
void
nGuiLineLayout::SetAligned( bool aligned )
{
    this->aligned = aligned;
}

//------------------------------------------------------------------------------
/**
    @brief Get aligned flag
*/
bool
nGuiLineLayout::GetAligned()
{
    return this->aligned;
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiLineLayout::OnShow()
{
    nGuiFormLayout::OnShow();

    this->UpdateLayout(this->GetRect());
}

//------------------------------------------------------------------------------
/**
    Called when the size of the widget changes. Will re-apply the layout
    rules on its children widgets.
*/
void
nGuiLineLayout::OnRectChange(const rectangle& newRect)
{
    if (this->IsShown())
    {
        this->UpdateLayout( newRect );
    }
    nGuiWidget::OnRectChange(newRect);
}

//------------------------------------------------------------------------------
/**
    @brief Update child widgets position
*/
void
nGuiLineLayout::UpdateLayout(const rectangle& newRect)
{
    float rectWidth = newRect.width();
    float rectHeight = newRect.height();

    float rectCenterX = rectWidth / 2.0f;
    float rectCenterY = rectHeight / 2.0f;

    nGuiWidget* curWidget;
    float currentPos = 0.0f;
    
    for (curWidget = (nGuiWidget*) this->GetHead(); curWidget; curWidget = (nGuiWidget*) curWidget->GetSucc())
    {
        if ( curWidget->IsShown() )
        {
            rectangle widgetRect;
            if ( this->orientation == ORI_HORIZONTAL )
            {
                // get min width and max height
                float minWidth = curWidget->GetMinSize().x;
                float maxHeight = min( curWidget->GetMaxSize().y, rectHeight );
                vector2 v0(currentPos, rectCenterY - maxHeight / 2.0f );
                vector2 v1( currentPos + minWidth, rectCenterY + maxHeight / 2.0f );
                if ( this->aligned ) 
                {
                    v0.y = 0.0f;
                    v1.y = maxHeight;
                }
                widgetRect.set( v0, v1 );
                currentPos += minWidth;
            }
            else
            {
                // get max height and min width
                float maxWidth = min( curWidget->GetMaxSize().x, rectWidth );
                float minHeight = curWidget->GetMinSize().y;
                vector2 v0( rectCenterX - maxWidth / 2.0f, currentPos );
                vector2 v1( rectCenterX + maxWidth / 2.0f, currentPos + minHeight );
                if ( this->aligned ) 
                {
                    v0.x = 0.0f;
                    v1.x = maxWidth;
                }
                
                widgetRect.set( v0, v1 );
                currentPos += minHeight;
            }
            curWidget->SetRect( widgetRect );
        }
    }
}
//------------------------------------------------------------------------------
