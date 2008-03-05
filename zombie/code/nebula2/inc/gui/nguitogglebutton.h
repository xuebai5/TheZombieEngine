#ifndef N_GUITOGGLEBUTTON_H
#define N_GUITOGGLEBUTTON_H
//-----------------------------------------------------------------------------
/**
    @class nGuiButton
    @ingroup NebulaGuiSystem
    @brief A GUI button class that uses 'pressed' to mantain a toggle state

    (C) 2004 Conjurer Services, S.A.
*/
#include "gui/nguiwidget.h"

//-----------------------------------------------------------------------------
class nGuiToggleButton : public nGuiWidget
{
public:
    /// constructor
    nGuiToggleButton();
    /// destructor
    virtual ~nGuiToggleButton();
    /// Rendering.
    virtual bool Render();
    /// handle mouse moved event
    //virtual bool OnMouseMoved(const vector2& mousePos);
    /// handle button down event
    virtual bool OnButtonDown(const vector2& mousePos);
    /// handle button up event
    virtual bool OnButtonUp(const vector2& mousePos);
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// set pressed
    void SetPressed(bool f);
    /// get pressed state
    bool IsPressed();

protected:
    bool focus;
    bool pressed;
};
//-----------------------------------------------------------------------------

inline void nGuiToggleButton::SetPressed(bool f)
{
    this->pressed = f;
}

inline bool nGuiToggleButton::IsPressed()
{
    return ( this->pressed );
}

#endif

