#ifndef N_GUIFASTSELECTOR_H
#define N_GUIFASTSELECTOR_H
//------------------------------------------------------------------------------
/**
    @class nGuiFastSelector
    @ingroup NebulaGuiSystem
    @brief A generic listbox selection widget. Closes itself when clicked.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguitextview.h"

//------------------------------------------------------------------------------
class nGuiFastSelector : public nGuiTextView
{
public:
    /// constructor
    nGuiFastSelector();
    /// destructor
    virtual ~nGuiFastSelector();
    /// set optional pattern
    void SetPattern(const char* pattern);
    /// get optional pattern
    const char* GetPattern() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// overrides nGuiTextView::Render()
    virtual bool Render();
    /// SetAutoSize
    void SetAutoSize(bool b);
protected:
    /// autosize?
    bool autosize;
    /// pattern
    nString pattern;
    // dirty (redrawing)
    bool dirty;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiFastSelector::SetPattern(const char* p)
{
    n_assert(p);
    this->pattern = p;
    this->dirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiFastSelector::GetPattern() const
{
    return this->pattern.Get();
}

inline
void
nGuiFastSelector::SetAutoSize(bool b)
{
    this->autosize = b;
}

//------------------------------------------------------------------------------
#endif
