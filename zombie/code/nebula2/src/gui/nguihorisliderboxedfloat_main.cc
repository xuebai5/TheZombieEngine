#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguihorisliderboxedfloat_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nGuiHoriSliderBoxedFloat.h"
#include "gui/nguiserver.h"
#include "gui/nguiskin.h"

nNebulaClass(nGuiHoriSliderBoxedFloat, "nguiformlayout");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nGuiHoriSliderBoxedFloat

    @cppclass
    nGuiHoriSliderBoxedFloat
    
    @superclass
    nguiformlayout
    
    @classinfo
    Docs needed.
*/

//------------------------------------------------------------------------------
/**
*/
nGuiHoriSliderBoxedFloat::nGuiHoriSliderBoxedFloat() :
    labelFont("GuiSmall"),
    minValue(0.0f),
    maxValue(10.0f),
    curValue(0.0f),
    knobSize(1),
    leftWidth(0.2f),
    rightWidth(0.1f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiHoriSliderBoxedFloat::~nGuiHoriSliderBoxedFloat()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHoriSliderBoxedFloat::OnShow()
{
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    nGuiFormLayout::OnShow();
    kernelServer->PushCwd(this);

    // text entry sizes
    vector2 textSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("textentry_n");
    vector2 textMinSize(0.0f, textSize.y);
    vector2 textMaxSize(1.0f, textSize.y);

    // create slider widget
    nGuiSlider2* slider = (nGuiSlider2*) kernelServer->New("nguislider2", "Slider");
    n_assert(slider);
    slider->SetRangeSize( (this->maxValue - this->minValue) + this->knobSize );
    slider->SetVisibleRangeStart( this->curValue - this->minValue );
    slider->SetVisibleRangeSize(float(this->knobSize));
    slider->SetHorizontal(true);
    this->AttachForm(slider, Top, 0.0f);
    this->AttachPos(slider, Left, this->leftWidth);
    this->AttachPos(slider, Right, 1.0f - this->rightWidth);
    slider->OnShow();
    this->refSlider = slider;
    const vector2& sliderMinSize = slider->GetMinSize();
    const vector2& sliderMaxSize = slider->GetMaxSize();

    // create left text label
    nGuiTextLabel* leftLabel = (nGuiTextLabel*) kernelServer->New("nguitextlabel", "LeftLabel");
    n_assert(leftLabel);
    leftLabel->SetText(this->leftText.Get());
    leftLabel->SetFont(this->GetLabelFont());
    leftLabel->SetAlignment(nGuiTextLabel::Right);
    leftLabel->SetColor(skin->GetLabelTextColor());
    leftLabel->SetMinSize(vector2(0.0f, sliderMinSize.y));
    leftLabel->SetMaxSize(vector2(1.0f, sliderMaxSize.y));
    this->AttachForm(leftLabel, Top, 0.0f);
    this->AttachForm(leftLabel, Left, 0.0f);
    this->AttachPos(leftLabel, Right, this->leftWidth);
    leftLabel->OnShow();
    this->refLeftLabel = leftLabel;

    // create right text entry
    nGuiTextEntry* textEntry = (nGuiTextEntry*) kernelServer->New("nguitextentry", "RightEntry");
    n_assert(textEntry);
    textEntry->SetText("0");
    textEntry->SetFont("GuiSmall");
    textEntry->SetAlignment(nGuiTextLabel::Left);
    textEntry->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textEntry->SetMinSize(textMinSize);
    textEntry->SetMaxSize(textMaxSize);
    textEntry->SetDefaultBrush("textentry_n");
    textEntry->SetPressedBrush("textentry_p");
    textEntry->SetHighlightBrush("textentry_h");
    textEntry->SetDisabledBrush("textentry_d");
    textEntry->SetCursorBrush("textcursor");
    textEntry->SetColor(vector4(0.85f, 0.85f, 0.85f, 1.0f));

    this->AttachForm(textEntry, Top, 0.0f);
    this->AttachForm(textEntry, Right, 0.005f);
    this->AttachPos(textEntry, Left, 1.0f - this->rightWidth);

    textEntry->OnShow();
    this->refTextEntry = textEntry;

    kernelServer->PopCwd();

    this->SetMinSize(sliderMinSize);
    this->SetMaxSize(sliderMaxSize);

    this->UpdateLayout(this->rect);
    nGuiServer::Instance()->RegisterEventListener(this);
}
//------------------------------------------------------------------------------
/**
*/
void
nGuiHoriSliderBoxedFloat::SetMaxLength(int l)
{
    this->refTextEntry->SetMaxLength(l);
}
//------------------------------------------------------------------------------
/**
*/
void
nGuiHoriSliderBoxedFloat::OnHide()
{
    nGuiServer::Instance()->UnregisterEventListener(this);

    this->ClearAttachRules();
    if (this->refSlider.isvalid())
    {
        this->refSlider->Release();
        n_assert(!this->refSlider.isvalid());
    }
    if (this->refLeftLabel.isvalid())
    {
        this->refLeftLabel->Release();
        n_assert(!this->refLeftLabel.isvalid());
    }
    if (this->refTextEntry.isvalid())
    {
        this->refTextEntry->Release();
        n_assert(!this->refTextEntry.isvalid());
    }

    nGuiFormLayout::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiHoriSliderBoxedFloat::OnFrame()
{
    if (this->refSlider.isvalid())
    {
        this->curValue = this->minValue + this->refSlider->GetVisibleRangeStart();

        // update left and right label formatted strings
        char buf[1024];
        snprintf(buf, sizeof(buf), this->leftText.Get(), this->curValue);
        this->refLeftLabel->SetText(buf);
        snprintf(buf, sizeof(buf), this->rightText.Get(), this->curValue);
        
        if (! this->refTextEntry->GetActive())
        {
            this->refTextEntry->SetText(buf);
            this->refTextEntry->SetInitialCursorPos(nGuiTextLabel::Right);
        }
    }
    nGuiFormLayout::OnFrame();
}

// juanga (from Nebula2SDK_2004)
// added 29sept04: replicates the event, allowing handle values WHILE dragging the slider knob
void
nGuiHoriSliderBoxedFloat::OnEvent(const nGuiEvent& event)
{
    switch(event.GetType())
    {

    case nGuiEvent::Char:
    case nGuiEvent::KeyUp:
        {
            if (this->refTextEntry.isvalid() && (event.GetWidget() == this->refTextEntry))
            {
                // update slider
                this->SetValue( float( atof( this->refTextEntry->GetText() ) ) );
                // replicate event, to process some other changes if needed on derivated classes
                nGuiEvent event(this, nGuiEvent::SliderChanged);
                nGuiServer::Instance()->PutEvent(event);
            }
        }
        break;

    case nGuiEvent::SliderChanged:
        {
            if (this->refSlider.isvalid() && (event.GetWidget() == this->refSlider))
            {
                // replicate event
                nGuiEvent event(this, nGuiEvent::SliderChanged);
                nGuiServer::Instance()->PutEvent(event);

                // if the user touches the slider, disable cursor on textentry, so the value will be updated
                //this->refTextEntry->SetActive(false);
            }
        }
        break;

    default:
        {
            if (event.GetWidget() == this->refTextEntry)
            {
                //this->SetValue(atoi(this->refTextEntry->GetText()));
            }
        }
        break;
    }
}
