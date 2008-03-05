#ifndef N_GUIHORISLIDERBOXED_H
#define N_GUIHORISLIDERBOXED_H
//------------------------------------------------------------------------------
/**
    @class nGuiHoriSliderBoxed
    @ingroup NebulaGuiSystem

    A horizontal slider group consisting of a label, a horizontal slider,
    and a text label which displays the current slider's numerical value
    as a printf-formatted string.
    
    (C) 2004 RadonLabs GmbH
*/
#include "gui/nguiformlayout.h"
#include "gui/nguislider2.h"
#include "gui/nguitextlabel.h"
#include "gui/nGuiTextEntry.h"

//------------------------------------------------------------------------------
class nGuiHoriSliderBoxed : public nGuiFormLayout
{
public:
    /// constructor
    nGuiHoriSliderBoxed();
    /// destructor
    virtual ~nGuiHoriSliderBoxed();
    /// set the label font
    void SetLabelFont(const char* f);
    /// set max string length for text entry
    void SetMaxLength(int l);
    /// get the label font
    const char* GetLabelFont() const;
    /// set text to left of slider (a printf formatted string)
    void SetLeftText(const char* t);
    /// get text to left of slider
    const char* GetLeftText() const;
    /// set text to right of slider (a printf formatted string)
    void SetRightText(const char* t);
    /// get text to right of slider
    const char* GetRightText() const;
    /// set relative width of left text label
    void SetLeftWidth(float w);
    /// get relative width of left text label
    float GetLeftWidth() const;
    /// set relative width of right text label
    void SetRightWidth(float w);
    /// get relative width of right text label
    float GetRightWidth() const;
    /// set the slider's minimum value
    void SetMinValue(int v);
    /// get the slider's minimum value
    int GetMinValue() const;
    /// set the slider's maximum value
    void SetMaxValue(int v);
    /// get the slider's maximum value
    int GetMaxValue() const;
    /// set the slider's current value
    void SetValue(int v);
    /// get the slider's current value
    int GetValue() const;
    /// set the knob size
    void SetKnobSize(int s);
    /// get the knob size
    int GetKnobSize() const;
    /// called when widget is becoming visible
    virtual void OnShow();
    /// called when widget is becoming invisible
    virtual void OnHide();
    /// called per frame when parent widget is visible
    virtual void OnFrame();
    /// notify widget of an event
    virtual void OnEvent(const nGuiEvent& event);
    /// gets a pointer to the text entry widget
    nGuiTextEntry * GetTextEntry();
    /// gets a pointer to the slider
    nGuiSlider2* GetSlider();

protected:
    nString labelFont;
    nString leftText;
    nString rightText;
    float leftWidth;
    float rightWidth;
    int minValue;
    int maxValue;
    int curValue;
    int knobSize;
    nRef<nGuiTextLabel> refLeftLabel;
    nRef<nGuiTextEntry> refTextEntry;
    nRef<nGuiSlider2>   refSlider;
};

//------------------------------------------------------------------------------
/**
*/
inline
nGuiTextEntry * 
nGuiHoriSliderBoxed::GetTextEntry()
{
    return(this->refTextEntry);
}
//------------------------------------------------------------------------------
/**
*/
inline
nGuiSlider2*
nGuiHoriSliderBoxed::GetSlider()
{
    return this->refSlider;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxed::SetLabelFont(const char* f)
{
    n_assert(f);
    this->labelFont = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiHoriSliderBoxed::GetLabelFont() const
{
    return this->labelFont.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxed::SetLeftWidth(float w)
{
    this->leftWidth = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderBoxed::GetLeftWidth() const
{
    return this->leftWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxed::SetRightWidth(float w)
{
    this->rightWidth = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderBoxed::GetRightWidth() const
{
    return this->rightWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxed::SetLeftText(const char* t)
{
    this->leftText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiHoriSliderBoxed::GetLeftText() const
{
    return this->leftText.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxed::SetRightText(const char* t)
{
    this->rightText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiHoriSliderBoxed::GetRightText() const
{
    return this->rightText.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxed::SetMinValue(int v)
{
    this->minValue = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiHoriSliderBoxed::GetMinValue() const
{
    return this->minValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxed::SetMaxValue(int v)
{
    this->maxValue = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiHoriSliderBoxed::GetMaxValue() const
{
    return this->maxValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxed::SetValue(int v)
{
    if (v > this->maxValue) { v=maxValue; }
    if (v < this->minValue) { v=minValue; }
    this->curValue = v;
    if (this->refSlider.isvalid())
    {
        this->refSlider->SetVisibleRangeStart(float(this->curValue - this->minValue));
        this->refSlider->SetVisibleRangeSize(float(this->knobSize));
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiHoriSliderBoxed::GetValue() const
{
    return this->curValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxed::SetKnobSize(int s)
{
    this->knobSize = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiHoriSliderBoxed::GetKnobSize() const
{
    return this->knobSize;
}

//------------------------------------------------------------------------------
#endif
