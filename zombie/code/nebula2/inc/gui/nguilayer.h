#ifndef N_GUILAYER_H
#define N_GUILAYER_H
//-----------------------------------------------------------------------------
/**
    @class nGuiLayer
    @ingroup NebulaGuiSystem
    @brief An abstract for GUI production on real apps
*/
#include "gfx2/ngfxserver2.h"
#include "gui/nguiformlayout.h"
#include "gui/nguihorislidergroup.h"
#include "gui/nguihorisliderboxed.h"
#include "gui/nguihorisliderboxedfloat.h"
#include "gui/nguitextentry.h"
#include "gui/nguitextbutton.h"
#include "gui/nguitogglebutton.h"

#define WIDTH_MODPANEL          200
#define WIDTH_LEFTPANEL         32
#define HEIGHT_TOPPANEL         32
#define R_WIDTH_MAINPANEL       1.0
#define HEIGHT_BOTTOMPANEL      16

//-----------------------------------------------------------------------------
class nGuiLayer
{
public:
    /// constructor
    nGuiLayer();
    /// destructor
    virtual ~nGuiLayer();

    // IDs
    enum Window_ID {
        TEST_REQUESTBOX=1,
    };

    void PopRequestBox(nGuiLayer::Window_ID wid, char * title, char * text);

    static void Log(char * filename, char * string, ...);
    static void gui_tests();
                                                
    static nGuiHoriSliderBoxed * AddValueSlider(nGuiFormLayout* layout,
											    const char * objname, 
											    char * text, 
                                                int minvalue,
											    int maxvalue, 
											    nGuiWidget* other,
											    bool begingroup=false,
                                                float calcwidth=0.0f);

    static nGuiHoriSliderBoxedFloat * AddFloatValueSlider(nGuiFormLayout* layout,
											    const char * objname, 
											    char * text, 
                                                float leftWidth,
											    float rightWidth, 
											    nGuiWidget* other,
											    bool begingroup=false,
                                                float calcwidth=0.0f);

    static nGuiTextEntry * nGuiLayer::AddTextEntry(nGuiFormLayout* layout,
												    const char * objname, 
												    char * settext,
												    nGuiWidget* under_of,
                                                    const char * LeftText=NULL);

    static nGuiWidget * nGuiLayer::PutToggleButton(nGuiFormLayout* layout,
                                                    const char * objname,
                                                    const nString &brushname,
                                                    char * tooltip,
                                                    nGuiWidget* fromleft,
                                                    nGuiWidget* fromtop,
                                                    float border);

    static nGuiWidget * nGuiLayer::AddButtonTo(const char * className,
                                                nGuiFormLayout* layout,
                                                const char * objname, 
                                                const nString &brushname, 
                                                char * tooltip,
                                                nGuiWidget* other = NULL,
                                                nGuiFormLayout::Edge edge = nGuiFormLayout::Left,
                                                bool suffix = true,
                                                float border = 0.0f);

    static nGuiToggleButton* nGuiLayer::AddRadioButton(nGuiFormLayout* layout,
                                                        const char * objname,
                                                        const char * labeltext,
                                                        nGuiWidget * other,
                                                        nGuiWidget * right,
                                                        nGuiWidget * maxright);

    static nGuiTextLabel * nGuiLayer::AddTextLabel(nGuiFormLayout* layout,
                                                    const char * objname,
                                                    const char * text,
                                                    nGuiWidget * other);

    static nGuiTextButton * nGuiLayer::AddTextButton(nGuiFormLayout* layout,
                                                    const char * objname,
                                                    const char * text,
                                                    nGuiWidget * other);

    //
    // conversions between relative and absolute (screensizes related, or not)
    //
    // utils for converting between screensizes and/or widget sizes
    inline static ushort nGuiLayer::absx(float rel, ushort totalsize=0) {
	    if (totalsize)
        return (ushort)(rel * totalsize);
        else
        return (ushort)(rel * (nGfxServer2::Instance()->GetDisplayMode().GetWidth()));
    }
    inline static ushort nGuiLayer::absy(float rel, ushort totalsize=0) {
	    if (totalsize)
        return (ushort)(rel * totalsize);
        else
	    return (ushort)(rel * (nGfxServer2::Instance()->GetDisplayMode().GetHeight()));
    }
    inline static float nGuiLayer::relx(ushort abs, ushort totalsize=0) {
        ushort x = totalsize;
        if (!x)
            x = nGfxServer2::Instance()->GetDisplayMode().GetWidth();
        float res = (float)abs / (float)x;
        return res;
    }
    inline static float nGuiLayer::rely(ushort abs, ushort totalsize=0) {
        ushort y = totalsize;
        if (!y)
            y = nGfxServer2::Instance()->GetDisplayMode().GetHeight();
        float res = (float)abs / (float)y;
        return res;
    }
    inline static vector4 nGuiLayer::color24(ushort r, ushort g, ushort b)
    {
        return vector4(relx(r,255), relx(g,255), relx(b,255), 1.0f);
    }

    typedef struct {
        ushort wid;
        nGuiWidget * main_widget;
        void * next;
    } tWindow;

    tWindow * WinList_Find(nGuiLayer::Window_ID wid);
    tWindow * WinList_Find(nGuiWidget * widget);
    bool WinList_Add(nGuiLayer::tWindow * data);
    void WinList_OnEvent(const nGuiEvent& event);
    void WinList_HandleEvent(nGuiLayer::tWindow * win, const nGuiEvent& event);

    tWindow * winlist;
    ushort n_winlist;

private:

};

//-----------------------------------------------------------------------------
#endif
