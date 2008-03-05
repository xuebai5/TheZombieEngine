#include "precompiled/pchngui.h"
//------------------------------------------------------------------------------
//  nguidockwindow_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "gui/nguidockwindow.h"
#include "gui/nguibutton.h"
#include "gui/nguievent.h"
#include "gui/nguiserver.h"

nNebulaClass(nGuiDockWindow, "nguiclientwindow");

//---  MetaInfo  ---------------------------------------------------------------
/**
    @scriptclass
    nguidockwindow

    @cppclass
    nGuiDockWindow
    
    @superclass
    nguiclientwindow
    
    @classinfo
    Docs needed.
*/

//------------------------------------------------------------------------------
/**
*/
nGuiDockWindow::nGuiDockWindow() :
    refScriptServer("/sys/servers/script")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGuiDockWindow::~nGuiDockWindow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDockWindow::OnShow()
{
    // update window flags
    this->SetMovable(false);
    this->SetResizable(false);
    this->SetCloseButton(false);
    this->SetTitleBar(false);

    //this->SetTitle("Nebula Viewer");

    // call parent class
    nGuiClientWindow::OnShow();

    // get client area form layout object
    nGuiFormLayout* layout = this->refFormLayout.get();
    kernelServer->PushCwd(layout);

    // horizontal size of 1 button
    vector2 btnSize = nGuiServer::Instance()->ComputeScreenSpaceBrushSize("console_n");
    //btnSize.x*=1.7f;
    //btnSize.y*=1.7f;

    // command console button
    nGuiButton* btn;
    btn = (nGuiButton*) kernelServer->New("nguibutton", "ConsoleButton");
    n_assert(btn);
    btn->SetDefaultBrush("terminal_n");
    btn->SetPressedBrush("terminal_p");
    btn->SetHighlightBrush("terminal_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Command Console");
    layout->AttachForm(btn, nGuiFormLayout::Left, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    btn->OnShow();
    this->refConsoleButton = btn;

    // texture browser button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "TexBrowserButton");
    n_assert(btn);
    btn->SetDefaultBrush("settings_n");
    btn->SetPressedBrush("settings_p");
    btn->SetHighlightBrush("settings_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Texture Browser");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refConsoleButton, 0.0f);
    btn->OnShow();
    this->refTexBrowserButton = btn;

    // gfx browser button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "GfxBrowserButton");
    n_assert(btn);
    btn->SetDefaultBrush("settings_n");
    btn->SetPressedBrush("settings_p");
    btn->SetHighlightBrush("settings_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Gfx Browser");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refTexBrowserButton, 0.0f);
    btn->OnShow();
    this->refGfxBrowserButton = btn;

    // scene control button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "SceneControlButton");
    n_assert(btn);
    btn->SetDefaultBrush("contrwindow_n");
    btn->SetPressedBrush("contrwindow_p");
    btn->SetHighlightBrush("contrwindow_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Scene Control");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refGfxBrowserButton, 0.0f);
    btn->OnShow();
    this->refSceneControlButton = btn;

    // watcher window button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "WatcherButton");
    n_assert(btn);
    btn->SetDefaultBrush("computer_n");
    btn->SetPressedBrush("computer_p");
    btn->SetHighlightBrush("computer_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Debug Watchers");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refSceneControlButton, 0.0f);
    btn->OnShow();
    this->refWatcherButton = btn;

    // system info button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "SysInfoButton");
    n_assert(btn);
    btn->SetDefaultBrush("info_n");
    btn->SetPressedBrush("info_p");
    btn->SetHighlightBrush("info_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("System Info");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refWatcherButton, 0.0f);
    btn->OnShow();
    this->refSysInfoButton = btn;

    // adjust display button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "AdjustDisplayButton");
    n_assert(btn);
    btn->SetDefaultBrush("disp_n");
    btn->SetPressedBrush("disp_p");
    btn->SetHighlightBrush("disp_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Adjust Display");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refSysInfoButton, 0.0f);
    btn->OnShow();
//    this->refAdjustButton = btn;

    // hide button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "HideButton");
    n_assert(btn);
    btn->SetDefaultBrush("eject_n");
    btn->SetPressedBrush("eject_p");
    btn->SetHighlightBrush("eject_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Hide System GUI");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refSysInfoButton, 0.0f);
    btn->OnShow();
    this->refHideButton = btn;

    // quit button
    btn = (nGuiButton*) kernelServer->New("nguibutton", "QuitButton");
    n_assert(btn);
    btn->SetDefaultBrush("quit_n");
    btn->SetPressedBrush("quit_p");
    btn->SetHighlightBrush("quit_h");
    btn->SetMinSize(btnSize);
    btn->SetMaxSize(btnSize);
    btn->SetTooltip("Quit Application");
    layout->AttachForm(btn, nGuiFormLayout::Top, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Bottom, 0.0f);
    layout->AttachWidget(btn, nGuiFormLayout::Left, this->refHideButton, 0.0f);
    layout->AttachForm(btn, nGuiFormLayout::Right, 0.0f);
    btn->OnShow();
    this->refQuitButton = btn;

    kernelServer->PopCwd();

    // set window position and size
    rectangle rect;
    const float width  = 9 * btnSize.x;
    const float height = btnSize.y;
    rect.v0.set(0.5f - (width * 0.5f), 1.0f - height);
    rect.v1.set(0.5f + (width * 0.5f), 1.0f);
    this->SetRect(rect);
}

//------------------------------------------------------------------------------
/**
*/
void
nGuiDockWindow::OnHide()
{
    this->refConsoleButton->Release();
    this->refTexBrowserButton->Release();
  //  this->refSceneControlButton->Release();
    this->refWatcherButton->Release();
    this->refSysInfoButton->Release();
    this->refHideButton->Release();
    this->refQuitButton->Release();

    this->refGfxBrowserButton->Release();

    // call parent class
    nGuiClientWindow::OnHide();
}

//------------------------------------------------------------------------------
/**
*/
void 
nGuiDockWindow::OnEvent(const nGuiEvent& event)
{
    if (event.GetType() == nGuiEvent::ButtonUp)
    {
        if (event.GetWidget() == this->refConsoleButton.get())
        {
            // open a new console window
            nGuiServer::Instance()->NewWindow("nguiconsolewindow", true);
        }
        else if (event.GetWidget() == this->refTexBrowserButton.get())
        {
            // open a texture browser window
            nGuiServer::Instance()->NewWindow("nguitexbrowserwindow", true);
        }
        else if (event.GetWidget() == this->refWatcherButton.get())
        {
            // open a graphics browser window
            nGuiServer::Instance()->NewWindow("nguigraphicsbrowserwindow", true);
        }
        else if (event.GetWidget() == this->refSceneControlButton)
        {
            // open a graphics browser window - nur mal als Test.
            nGuiServer::Instance()->NewWindow("nguiscenecontrolwindow", true);
        }
        else if (event.GetWidget() == this->refWatcherButton)
        {
            // open debug watcher window
            nGuiServer::Instance()->NewWindow("nguiwatcherwindow", true);
        }
        else if (event.GetWidget() == this->refSysInfoButton.get())
        {
            // open system info window
            nGuiServer::Instance()->NewWindow("nguisysteminfowindow", true);
        }
        else if (event.GetWidget() == this->refHideButton.get())
        {
            // hide the system gui
            nGuiServer::Instance()->ToggleSystemGui();
        }
        else if (event.GetWidget() == this->refQuitButton.get())
        {
            // quit application
            this->refScriptServer->SetQuitRequested(true);
        }
        else if (event.GetWidget() == this->refGfxBrowserButton.get())
        {
            // open a graphics browser window
            nGuiServer::Instance()->NewWindow("nguigraphicsbrowserwindow", true);
        }
    }

    nGuiClientWindow::OnEvent(event);
}
