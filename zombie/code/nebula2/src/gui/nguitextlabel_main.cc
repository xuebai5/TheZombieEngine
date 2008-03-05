#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguitextlabel_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguitextlabel.h"
#include "gfx2/ngfxserver2.h"
#include "gui/nguiserver.h"

nNebulaScriptClass(nGuiTextLabel, "nguilabel");

//------------------------------------------------------------------------------
/**
*/
nGuiTextLabel::nGuiTextLabel() :
    align(Center),
    border(0.005f, 0.0f),
    pressedOffset(0.0015f, 0.0015f),
    fontName("GuiDefault"),
    clipping(true),
    vCenter(true),
    wordBreak(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiTextLabel::~nGuiTextLabel()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiTextLabel::SetText(const char* text)
{
    n_assert(text);
    this->text = text;
}

//------------------------------------------------------------------------------
/**
*/
const char*
nGuiTextLabel::GetText() const
{
    return this->text.Get();
}

//------------------------------------------------------------------------------
/**
*/
vector2
nGuiTextLabel::GetTextExtent()
{
    this->ValidateFont();
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nFont2* oldFont = gfxServer->GetFont();
    gfxServer->SetFont(this->refFont.get());
    vector2 extent = gfxServer->GetTextExtent(this->GetText());
    gfxServer->SetFont(oldFont);
    return extent;
}

/**
    @brief Calculates the current number of lines (only has sense if wordBreak enabled)
    @param size Size rectangle to do the calculation

    The current text and font is used.
*/
int
nGuiTextLabel::CalculateNumberOfLines(vector2 size)
{
    if ( this->text.IsEmpty() )
    {
        return 1;
    }

    this->ValidateFont();
    nGfxServer2* gfxServer = nGfxServer2::Instance();

    // Make check rectangle of maximum size
    rectangle rect1(this->rect.v0, size);

    nString breakLine = this->text;

    // set label font
    nFont2 * oldFont = gfxServer->GetFont();
    gfxServer->SetFont(this->refFont.get());

    gfxServer->BreakLines( this->text, rect1, breakLine );

    // restore old font
    gfxServer->SetFont(oldFont);

    // count tokens in line
    int numLines = 1;
    const char * currentChar = breakLine.Get();
    for (int i = 0; i < this->text.Length(); i++)
    {
        if ( *currentChar == '\n' )
        {
            numLines ++;
        }
        currentChar ++;
    }
    
    return numLines;
}

//------------------------------------------------------------------------------
/**
    Make sure the font object is valid.
*/
void
nGuiTextLabel::ValidateFont()
{
    if (!this->refFont.isvalid())
    {
        this->refFont = (nFont2*) nResourceServer::Instance()->FindResource(this->fontName.Get(), nResource::Font);
        if (!this->refFont.isvalid())
        {
            n_error("nGuiTextLabel %s: Unknown font '%s'!", this->GetName(), this->fontName.Get()); 
        }
    }
}

//------------------------------------------------------------------------------
/**
    Render the label text only. This is called by the general Render()
    method.

    @param  pressed         true if text should appear "pressed down"
*/
void
nGuiTextLabel::RenderText(bool pressed)
{
    if (this->text.IsEmpty())
    {
        // no text, nothing to render
        return;
    }

    this->ValidateFont();

    // compute the text position
    nGfxServer2::Instance()->SetFont(this->refFont.get());
    uint renderFlags = 0;
    switch (this->align)
    {
        case Left:  renderFlags |= nFont2::Left; break;
        case Right: renderFlags |= nFont2::Right; break;
        default:    renderFlags |= nFont2::Center; break;
    }
    if (!this->clipping)
    {
        renderFlags |= nFont2::NoClip;
    }
    if (this->wordBreak)
    {
        renderFlags |= nFont2::WordBreak;
    }
    if (this->vCenter)
    {
        renderFlags |= nFont2::VCenter;
    }

    rectangle screenSpaceRect = this->GetScreenSpaceRect();
    screenSpaceRect.v0 += this->border;
    screenSpaceRect.v1 -= this->border;

    // draw text
    rectangle pressedRect = screenSpaceRect;
    pressedRect.v0 += this->pressedOffset;
    pressedRect.v1 += this->pressedOffset;
    if (pressed)
    {
        nGuiServer::Instance()->DrawText(this->GetText(), this->color, pressedRect, renderFlags);
    }
    else if (this->blinking)
    {
        nTime time = nGuiServer::Instance()->GetTime();
        if (fmod(time, this->blinkRate) > this->blinkRate/3.0)
        {
            nGuiServer::Instance()->DrawText(this->GetText(), this->blinkColor, screenSpaceRect, renderFlags);
        }
        else
        {
            nGuiServer::Instance()->DrawText(this->GetText(), this->color, screenSpaceRect, renderFlags);
        }
    }
    else
    {
        nGuiServer::Instance()->DrawText(this->GetText(), this->color, screenSpaceRect, renderFlags);
    }
}

//------------------------------------------------------------------------------
/**
    Renders the text label. This will first invoke the parent class 
    nGuiLabel to render the labels background bitmap (if one exists).
    Afterwards the actual text will be rendered through the text server.
*/
bool
nGuiTextLabel::Render()
{
    if (this->IsShown())
    {
        // render the background image, if defined
        nGuiLabel::Render();

        // render the text
        this->RenderText(false);
        return true;
    }
    return false;
}
