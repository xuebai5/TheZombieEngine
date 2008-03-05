#ifndef N_GUILABEL_H
#define N_GUILABEL_H
//------------------------------------------------------------------------------
/**
    @class nGuiLabel
    @ingroup NebulaGuiSystem
    @brief A widget which is a graphical label.
    
    (C) 2003 RadonLabs GmbH
*/
#include "gui/nguiwidget.h"

//------------------------------------------------------------------------------
class nGuiLabel : public nGuiWidget
{
public:
    /// constructor
    nGuiLabel();
    /// destructor
    virtual ~nGuiLabel();
    /// render the widget
    virtual bool Render();
    /// handle mouse move
    virtual bool OnMouseMoved(const vector2& mousePos);
    /// set color
    virtual void SetColor(const vector4& c);
    /// get color
    virtual const vector4& GetColor() const;
    /// set blinking color
    void SetBlinkingColor(vector4 color);
    /// get blinking color
    vector4 GetBlinkingColor() const;

protected:
    vector4 color;
    vector4 blinkColor;

    // color is set so don't use brush resource color
    bool colorSet;

private:
    bool mouseOver;
};
//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiLabel::SetColor(const vector4& c)
{
    this->color = c;
    this->colorSet = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector4&
nGuiLabel::GetColor() const
{
    return this->color;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiLabel::SetBlinkingColor(vector4 color)
{
    this->blinkColor = color;
}

//------------------------------------------------------------------------------
/**
*/
inline
vector4
nGuiLabel::GetBlinkingColor() const
{
    return this->blinkColor;
}
//------------------------------------------------------------------------------
#endif
