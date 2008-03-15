#include "precompiled/pchngui.h"
//-----------------------------------------------------------------------------
//  nguilayer_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "gui/nguilayer.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "gui/nguiserver.h"
#include "gui/nguievent.h"
#include "gui/nguimessagebox.h"
#include "gui/nguihorislidergroup.h"
#include "gui/nguihorisliderboxed.h"
#include "gui/nguihorisliderboxedfloat.h"
#include "gui/nguitextentry.h"
#include "gui/nguitextlabel.h"
#include "gui/nguibutton.h"

//nNebulaClass(nGuiLayer, "nroot");

//-----------------------------------------------------------------------------
/**
    constructor
*/
nGuiLayer::nGuiLayer()
{
    winlist = NULL;
    n_winlist = 0;
}

//-----------------------------------------------------------------------------
/**
*/
nGuiLayer::~nGuiLayer()
{
}

//-----------------------------------------------------------------------------
/**
*/
bool
nGuiLayer::WinList_Add(nGuiLayer::tWindow * data)
{
    if (winlist) {
        winlist[n_winlist - 1].next = (void*)data;
        n_winlist++;
    }
    else
    {
        winlist = data;
        n_winlist=1;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiLayer::PopRequestBox(nGuiLayer::Window_ID wid, char * title, char * text)
{
    nGuiMessageBox * mb = (nGuiMessageBox *)nGuiServer::Instance()->NewWindow("nguimessagebox", false);
    mb->SetMessageText(text);
    mb->SetAutoSize(true);
    mb->SetOkText("OK");
    mb->SetCancelText("Cancel");
    mb->SetType(nGuiMessageBox::OkCancel);
    mb->SetTitle(title);
    mb->SetDefaultBrush("messagebox");
    mb->Show();

    // assign layer custom data
    tWindow * desc = n_new (tWindow);
    memset(desc, 0, sizeof(tWindow));
    n_assert( wid <= USHRT_MAX );
    desc->wid = static_cast<ushort>( wid );
    desc->main_widget = (nGuiWidget*)mb;
    this->WinList_Add(desc);

//    this->Log("guilayer.txt", "Created node for wid[%d]", ((tWindow*)this->windows.GetTail()->GetPtr())->wid);
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiLayer::WinList_HandleEvent(nGuiLayer::tWindow * /*win*/, const nGuiEvent& event)
{
    //nGuiLayer::Log("guilayer.txt", "HandleEvent(win[%d]), (event[%s])", win->wid, event.TypeToString(event.GetType()));

    // reminder: check if (this->refWidget.isvalid() BEFORE accesing it

    switch(event.GetType())
    {
    case nGuiEvent::DialogOk:
        {
        }
        break;
    case nGuiEvent::DialogCancel:
        {
        }
        break;
    }
}

//-----------------------------------------------------------------------------
/**
*/
nGuiLayer::tWindow *
nGuiLayer::WinList_Find(nGuiLayer::Window_ID wid)
{
    if (! winlist) return NULL;

    for (tWindow * tmp = this->winlist; tmp; tmp=(tWindow*)tmp->next) {
        if (tmp->wid == wid) return tmp;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
/**
*/
nGuiLayer::tWindow *
nGuiLayer::WinList_Find(nGuiWidget * widget)
{
    if (! winlist) return NULL;

    for (tWindow * tmp = this->winlist; tmp; tmp=(tWindow*)tmp->next) {
        if (tmp->main_widget == widget) return tmp;
    }
    return NULL;    
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiLayer::WinList_OnEvent(const nGuiEvent& /*event*/)
{
    //if (this->refAmbientR.isvalid() &&
    /*
    switch(event.GetType())
    {
        case nGuiEvent::SliderChanged:
    }
    */
    
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiLayer::gui_tests()
{
    // some of my tests

}

//-----------------------------------------------------------------------------
/**
*/
nGuiTextLabel *
nGuiLayer::AddTextLabel(nGuiFormLayout *layout,
                        const char *objname,
                        const char *text,
                        nGuiWidget *other)
{
    vector2 textSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("textentry_n");
    vector2 textMinSize(0.0f, textSize.y);
    vector2 textMaxSize(1.0f, textSize.y);

    nGuiTextLabel* tl;
    tl = (nGuiTextLabel*) nKernelServer::ks->New("nguitextlabel", objname);
    tl->SetText(text);
    tl->SetFont("GuiSmall");
    tl->SetAlignment(nGuiTextLabel::Center);
    tl->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    tl->SetMinSize(textMinSize);
    tl->SetMaxSize(textMaxSize);

    tl->SetDefaultBrush("titlebar");
    tl->SetPressedBrush("titlebar");
    tl->SetHighlightBrush("titlebar");

    float border=0.005f;
    if (other)
    {
        layout->AttachWidget(tl, nGuiFormLayout::Top, other, border*2);
    }
    else
    {
        layout->AttachForm(tl, nGuiFormLayout::Top, border*2);
    }
    layout->AttachForm(tl, nGuiFormLayout::Left, border);
    layout->AttachForm(tl, nGuiFormLayout::Right, border);

    tl->OnShow();

    return tl;
}

//-----------------------------------------------------------------------------
/**
*/
nGuiTextButton *
nGuiLayer::AddTextButton(nGuiFormLayout* layout,
                         const char *objname,
                         const char *text,
                         nGuiWidget *other)
{
    vector2 buttonSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("button_n");
    nGuiSkin* skin = nGuiServer::Instance()->GetSkin();
    n_assert(skin);

    nGuiTextButton* cb = (nGuiTextButton*) nKernelServer::ks->New("nguitextbutton", objname);
    cb->SetText(text);
    cb->SetFont("GuiSmall");
    cb->SetAlignment(nGuiTextButton::Center);
    cb->SetDefaultBrush("button_n");
    cb->SetPressedBrush("button_p");
    cb->SetHighlightBrush("button_h");
    cb->SetMinSize(buttonSize);
    cb->SetMaxSize(buttonSize);
    cb->SetColor(skin->GetButtonTextColor());
    
    float border=0.005f;
    //cb->SetRect(rectangle(vector2(border,0.0f), vector2(1.0f-border, buttonSize.y)));

    if (other)
    {
        layout->AttachWidget(cb, nGuiFormLayout::Top, other, border);
    }
    else
    {
        layout->AttachForm(cb, nGuiFormLayout::Top, border);
    }
    layout->AttachPos(cb, nGuiFormLayout::HCenter, 0.5f);
    //layout->AttachForm(cb, nGuiFormLayout::Left, border);
    //layout->AttachForm(cb, nGuiFormLayout::Right, border);
    
    cb->OnShow();
    
    return cb;
}

//-----------------------------------------------------------------------------
/**
*/
nGuiWidget *
nGuiLayer::PutToggleButton(nGuiFormLayout *layout,
                           const char *objname,
                           const nString &brushname,
                           char *tooltip,
                           nGuiWidget *fromleft,
                           nGuiWidget *fromtop,
                           float border)
{
    nGuiButton* btn;
    btn = (nGuiButton*) nKernelServer::ks->New("nguitogglebutton", objname);
    n_assert(btn);

    vector2 btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize(brushname.Get());

    btn->SetDefaultBrush( brushname.Get() );
    btn->SetPressedBrush( (brushname + "_t").Get() );
    btn->SetHighlightBrush( brushname.Get() );

    btn->SetMinSize( btnSize );
    btn->SetMaxSize( btnSize );
    btn->SetTooltip( tooltip );

    if (fromtop && fromleft)
    {
        layout->AttachWidget(btn, nGuiFormLayout::Top, fromtop, border);
        layout->AttachWidget(btn, nGuiFormLayout::Left, fromleft, border);
    }
    else
    {
        if (!fromleft) {
            layout->AttachWidget(btn, nGuiFormLayout::Top, fromtop, border);
            layout->AttachForm(btn, nGuiFormLayout::Left, border);
        }
    }

    btn->OnShow();

    return btn;

}

//-----------------------------------------------------------------------------
/**
*/
nGuiWidget *
nGuiLayer::AddButtonTo(const char * className,
                       nGuiFormLayout* layout,
                       const char * objname, 
                       const nString &brushname, 
                       char * tooltip,
                       nGuiWidget * other,
                       nGuiFormLayout::Edge edge,
                       bool suffix,
                       float border)
{

    nGuiButton* btn;
    btn = (nGuiButton*) nKernelServer::ks->New(className, objname);
    n_assert(btn);

    vector2 btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("terminal_n");

    if (suffix)
    {
        btn->SetDefaultBrush( (brushname + "_n").Get() );
        btn->SetPressedBrush( (brushname + "_p").Get() );
        btn->SetHighlightBrush( (brushname + "_h").Get() );
    }
    else
    {
        btn->SetDefaultBrush( brushname.Get() );
        btn->SetPressedBrush( (brushname + "_t").Get() );
        btn->SetHighlightBrush( brushname.Get() );
    }

    btn->SetMinSize( btnSize );
    btn->SetMaxSize( btnSize );
    btn->SetTooltip( tooltip );
    if (!other)
    {
        layout->AttachForm(btn, edge, border);
    }
    else
    {
         layout->AttachWidget(btn, edge, other, border);
    }

    switch(edge)
    {
    case nGuiFormLayout::Top:
        layout->AttachForm(btn, nGuiFormLayout::Left, 0.0f);
        break;
    case nGuiFormLayout::Left:
        {
            layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
            layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
        }
        break;
    default:
        n_error("Unexpected nGuiFormLayout::Edge at nGuiLayer::AddButtonTo");
        break;
    }

    btn->OnShow();

    return btn;
}

// groups initialization tasks for an horizontal-slider
nGuiHoriSliderBoxed * nGuiLayer::AddValueSlider(nGuiFormLayout* layout,
                                                const char * objname, 
                                                char * text, 
                                                int minvalue,
                                                int maxvalue, 
                                                nGuiWidget* other,
                                                bool begingroup,
                                                float calcwidth)
{
    nGuiHoriSliderBoxed* slider;
    slider = (nGuiHoriSliderBoxed*) nKernelServer::ks->New("nguihorisliderboxed", objname);
    slider->SetLeftText(text);
    slider->SetRightText("%d");
    slider->SetMinValue(minvalue);
    slider->SetMaxValue(maxvalue);
    slider->SetValue(minvalue);

    slider->SetKnobSize(50); // knobsize (slider size)

    if (calcwidth>0.0f) // calcwidth is maximum width where horisliderboxed will fit
    {
        nFont2 * pFont = (nFont2*) nResourceServer::Instance()->FindResource("GuiSmall", nResource::Font);
        n_assert(pFont);
        nGfxServer2::Instance()->SetFont(pFont);

        float w = (nGfxServer2::Instance()->GetTextExtent(text).x);
        ushort px = nGuiLayer::absx(w) * 2;

        w = nGuiLayer::relx(px, nGuiLayer::absx(calcwidth));
        slider->SetLeftWidth( w );
        slider->SetRightWidth(0.25f);
    }
    else
    {
        slider->SetLeftWidth(0.32f);
        slider->SetRightWidth(0.19f);
    }

    float border=0.005f;
    if (!other) 
    {
	    // first slider, top of the window ('other' widget is NULL)
	    layout->AttachForm(slider, nGuiFormLayout::Top, border);
    }
    else
    {
	    // margin if we are grouping sliders
	    if (begingroup) border*=3;

	    // when 'other' widget is passed, the slider will be attached below
	    layout->AttachWidget(slider, nGuiFormLayout::Top, other, border);
    }
    border=0.001f;
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);

    slider->OnShow();

    return slider;
}

// groups initialization tasks for an horizontal-slider with float value
/**
*/
nGuiHoriSliderBoxedFloat* nGuiLayer::AddFloatValueSlider(nGuiFormLayout* layout,
                                                const char * objname, 
                                                char * text, 
                                                float leftWidth,
                                                float rightWidth, 
                                                nGuiWidget* other,
                                                bool begingroup,
                                                float calcwidth)
{
    nGuiHoriSliderBoxedFloat* slider;
    slider = (nGuiHoriSliderBoxedFloat*)( nKernelServer::ks->New("nguihorisliderboxedfloat", objname) );
    slider->SetLeftText(text);
    slider->SetRightText("%.3f");

    slider->SetKnobSize(20); // knobsize (slider size)

    if (calcwidth>0.0f) // calcwidth is maximum width where horisliderboxedfloat will fit
    {
        nFont2 * pFont = (nFont2*) nResourceServer::Instance()->FindResource("GuiSmall", nResource::Font);
        n_assert(pFont);
        nGfxServer2::Instance()->SetFont(pFont);

        float w = (nGfxServer2::Instance()->GetTextExtent(text).x);
        ushort px = nGuiLayer::absx(w) * 2;

        w = nGuiLayer::relx(px, nGuiLayer::absx(calcwidth));
        slider->SetLeftWidth( w );
        slider->SetRightWidth(0.3f);
    }
    else
    {
        slider->SetLeftWidth( leftWidth );
        slider->SetRightWidth( rightWidth );
    }

    float border=0.005f;
    if (!other) 
    {
	    // first slider, top of the window ('other' widget is NULL)
	    layout->AttachForm(slider, nGuiFormLayout::Top, border);
    }
    else
    {
	    // margin if we are grouping sliders
	    if (begingroup) border*=3;

	    // when 'other' widget is passed, the slider will be attached below
	    layout->AttachWidget(slider, nGuiFormLayout::Top, other, border);
    }
    border=0.001f;
    layout->AttachForm(slider, nGuiFormLayout::Left, border);
    layout->AttachForm(slider, nGuiFormLayout::Right, border);

    slider->OnShow();

    slider->SetMinValue(0.0f);
    slider->SetMaxValue(100.0f);
    slider->SetValue(0.0f);

    return slider;
}

nGuiToggleButton* nGuiLayer::AddRadioButton(nGuiFormLayout* layout,
                                            const char * objname,
                                            const char * labeltext,
                                            nGuiWidget * other,
                                            nGuiWidget * right,
                                            nGuiWidget * maxright)
{
    float border = 0.005f * 2;
    vector2 btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("checkbutton_toggle");
    nGuiToggleButton* btn;
    btn = (nGuiToggleButton*)nKernelServer::ks->New("nguitogglebutton", objname);
    n_assert(btn);
    rectangle layoutrect = layout->GetRect();

    btn->SetDefaultBrush( "checkbutton_toggle" );
    btn->SetPressedBrush( "checkbutton_toggle_t" );
    btn->SetHighlightBrush( "checkbutton_toggle" );

    btn->SetMinSize( btnSize );
    btn->SetMaxSize( btnSize );

    if (other)
    layout->AttachWidget(btn, nGuiFormLayout::Top, other, border);
    else
    layout->AttachForm(btn, nGuiFormLayout::Top, border);

    if (right)
    {
        layout->AttachWidget(btn, nGuiFormLayout::Left, right, border);
    }
    else
    layout->AttachForm(btn, nGuiFormLayout::Left, border);

    btn->OnShow();

    char tl_name[128]; // TextLabel_name (object name)
    // compute font height
    vector2 textSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("textentry_n");
    vector2 textMinSize(0.0f, textSize.y);
    vector2 textMaxSize(1.0f, textSize.y);
    nGuiTextLabel* textLabel;
    sprintf(tl_name, "tl_%s", objname);
    textLabel = (nGuiTextLabel*) nKernelServer::ks->New("nguitextlabel", tl_name);
    n_assert(textLabel);
    textLabel->SetText(labeltext);
    textLabel->SetFont("GuiSmall");
    textLabel->SetDefaultBrush("titlebar");
    textLabel->SetAlignment(nGuiTextLabel::Left);
    textLabel->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textLabel->SetMinSize(textMinSize);
    textLabel->SetMaxSize(textMaxSize);

    layout->AttachWidget(textLabel, nGuiFormLayout::Top, other, border - 0.001f);

    if (maxright)
    {
    layout->AttachWidget(textLabel, nGuiFormLayout::Right, maxright, 0.0f);
    }
    else
    {
    layout->AttachWidget(textLabel, nGuiFormLayout::Left, btn, 0.005f);
    layout->AttachForm(textLabel, nGuiFormLayout::Right, border);
    }

    return (btn);
}

//-----------------------------------------------------------------------------
/**
    groups initialization tasks for a TextEntry
    + implements adding the 'lefttext' with no effort
*/
nGuiTextEntry *
nGuiLayer::AddTextEntry(nGuiFormLayout* layout,
                        const char * objname, 
                        char * settext,
                        nGuiWidget* under_of,
                        const char * LeftText)
{
    float border = 0.005f * 2;

    // textlabel
    vector2 textSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("textentry_n");
    vector2 textMinSize(0.0f, textSize.y);
    vector2 textMaxSize(1.0f, textSize.y);

    nString tl_objname = "tl_";
    tl_objname.Append(objname);
    nGuiTextLabel* tl = (nGuiTextLabel*)nKernelServer::ks->New("nguitextlabel", tl_objname.Get());
    n_assert(tl);
    tl->SetText(LeftText);
    tl->SetFont("GuiSmall");
    tl->SetAlignment(nGuiTextLabel::Right);
    tl->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    tl->SetMinSize(textMinSize);
    tl->SetMaxSize(textMaxSize);
    tl->SetDefaultBrush("titlebar");  // layout debugging

    if (under_of)
    layout->AttachWidget(tl, nGuiFormLayout::Top, under_of, border);
    else
    layout->AttachForm(tl, nGuiFormLayout::Top, border);

    layout->AttachForm(tl, nGuiFormLayout::Left, border);
    layout->AttachPos(tl, nGuiFormLayout::Right, 0.5);

    tl->OnShow();

    // textentry
    nGuiTextEntry* textEntry = (nGuiTextEntry*) nKernelServer::ks->New("nguitextentry", objname);
    n_assert(textEntry);
    textEntry->SetText(settext);
    textEntry->SetFont("GuiSmall");
    textEntry->SetAlignment(nGuiTextEntry::Left); // alignment for text inside textentry
    textEntry->SetDefaultBrush("textentry_n");
    textEntry->SetPressedBrush("textentry_p");
    textEntry->SetHighlightBrush("textentry_h");
    textEntry->SetDisabledBrush("textentry_d");
    textEntry->SetCursorBrush("textcursor");
    textEntry->SetColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    textEntry->SetMinSize(textMinSize);
    textEntry->SetMaxSize(textMaxSize);
    textEntry->SetColor(vector4(0.85f, 0.85f, 0.85f, 1.0f));

    if (!under_of)
    layout->AttachForm(textEntry, nGuiFormLayout::Top, border);
    else
    layout->AttachWidget(textEntry, nGuiFormLayout::Top, under_of, border);

    layout->AttachWidget(textEntry, nGuiFormLayout::Left, tl, border);
    //layout->AttachForm(textEntry, nGuiFormLayout::Left, border);
    layout->AttachForm(textEntry, nGuiFormLayout::Right, border);

    textEntry->OnShow();

    return textEntry;
}

//-----------------------------------------------------------------------------
/**
    simple log writer
*/
void
nGuiLayer::Log(char * filename, char * string, ...)
{
    static bool first=true;

    FILE *f;
    nString mangledPath = nFileServer2::Instance()->ManglePath(filename);

    if (first)
    {
	    first=false;
	    f=fopen(mangledPath.Get(),"wb");
    }
    else
	    f=fopen(mangledPath.Get(),"ab");

    if (f)
    {
	    char buffer[1024];
	    va_list ArgList=NULL;
	    va_start (ArgList, string);
	    vsprintf(buffer, string, ArgList);

	    fprintf( f,"%s%c%c",buffer,13,10 );
	    fclose(f);
    }
}
