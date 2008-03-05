#ifndef N_GUIHORISLIDERBOXEDFLOAT_H
#define N_GUIHORISLIDERBOXEDFLOAT_H
//------------------------------------------------------------------------------
/**
    @class nGuiHoriSliderBoxedFloat
    @ingroup NebulaGuiSystem
    @Author Juan Jose Luna

    A horizontal slider group consisting of a label, a horizontal slider,
    and a text label which displays the current slider's numerical value
    as a printf-formatted string. This is the float version of nGuiHoriSliderBoxed
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "gui/nguiformlayout.h"
#include "gui/nguislider2.h"
#include "gui/nguitextlabel.h"
#include "gui/nGuiTextEntry.h"

//------------------------------------------------------------------------------
class nGuiHoriSliderBoxedFloat : public nGuiFormLayout
{
public:
    /// constructor
    nGuiHoriSliderBoxedFloat();
    /// destructor
    virtual ~nGuiHoriSliderBoxedFloat();
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
    void SetMinValue(float v);
    /// get the slider's minimum value
    float GetMinValue() const;
    /// set the slider's maximum value
    void SetMaxValue(float v);
    /// get the slider's maximum value
    float GetMaxValue() const;
    /// set the slider's current value
    void SetValue(float v);
    /// get the slider's current value
    float GetValue() const;
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
    float minValue;
    float maxValue;
    float curValue;
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
nGuiHoriSliderBoxedFloat::GetTextEntry()
{
    return(this->refTextEntry);
}
//------------------------------------------------------------------------------
/**
*/
inline
nGuiSlider2*
nGuiHoriSliderBoxedFloat::GetSlider()
{
    return this->refSlider;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxedFloat::SetLabelFont(const char* f)
{
    n_assert(f);
    this->labelFont = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiHoriSliderBoxedFloat::GetLabelFont() const
{
    return this->labelFont.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxedFloat::SetLeftWidth(float w)
{
    this->leftWidth = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderBoxedFloat::GetLeftWidth() const
{
    return this->leftWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxedFloat::SetRightWidth(float w)
{
    this->rightWidth = w;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderBoxedFloat::GetRightWidth() const
{
    return this->rightWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxedFloat::SetLeftText(const char* t)
{
    this->leftText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiHoriSliderBoxedFloat::GetLeftText() const
{
    return this->leftText.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxedFloat::SetRightText(const char* t)
{
    this->rightText = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nGuiHoriSliderBoxedFloat::GetRightText() const
{
    return this->rightText.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxedFloat::SetMinValue(float v)
{
    this->minValue = v;

    if ( this->curValue < this->minValue )
    {
        this->curValue = this->minValue;
    }
    if ( this->refSlider.isvalid() )
    {
        nGuiSlider2* slider = this->refSlider;
        slider->SetRangeSize( (this->maxValue - this->minValue) + this->knobSize );
        slider->SetVisibleRangeStart( this->curValue - this->minValue );
        slider->SetVisibleRangeSize(float(this->knobSize));
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderBoxedFloat::GetMinValue() const
{
    return this->minValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxedFloat::SetMaxValue(float v)
{
    this->maxValue = v;
    if ( this->curValue > this->maxValue )
    {
        this->curValue = this->maxValue;
    }
    if ( this->refSlider.isvalid() )
    {
        nGuiSlider2* slider = this->refSlider;
        slider->SetRangeSize( (this->maxValue - this->minValue) + this->knobSize );
        slider->SetVisibleRangeStart( this->curValue - this->minValue );
        slider->SetVisibleRangeSize(float(this->knobSize));
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderBoxedFloat::GetMaxValue() const
{
    return this->maxValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxedFloat::SetValue(float v)
{
    if (v > this->maxValue) { v=maxValue; }
    if (v < this->minValue) { v=minValue; }
    this->curValue = v;
    if (this->refSlider.isvalid())
    {
        this->refSlider->SetVisibleRangeStart( this->curValue - this->minValue );
        this->refSlider->SetVisibleRangeSize( float(this->knobSize) );
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nGuiHoriSliderBoxedFloat::GetValue() const
{
    return this->curValue;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nGuiHoriSliderBoxedFloat::SetKnobSize(int s)
{
    this->knobSize = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nGuiHoriSliderBoxedFloat::GetKnobSize() const
{
    return this->knobSize;
}

//------------------------------------------------------------------------------
#endif
