#include "precompiled/pchngui.h"
//-----------------------------------------------------------------------------
//  nguiserver_main.cc
//  (C) 2001 RadonLabs GmbH
//-----------------------------------------------------------------------------
#include "gui/nguiserver.h"
#include "gui/nguiwindow.h"
#include "gui/nguitooltip.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "input/ninputserver.h"
#include "gfx2/ngfxserver2.h"
#include "gfx2/nfontdesc.h"
#include "gfx2/nmesh2.h"
#include "gfx2/nshader2.h"
#include "gfx2/nfontdesc.h"
#include "gui/nguiskin.h"
#include "audio3/naudioserver3.h"
#include "gui/nguiresource.h"
#include "gui/nguibrush.h"
#include "gui/nguidragbox.h"

#include "gui/nguilayer.h"

nNebulaScriptClass(nGuiServer, "nroot");
nGuiServer* nGuiServer::Singleton = 0;

const nTime nGuiServer::toolTipFadeInTime = float(0.0);
const nTime nGuiServer::toolTipActivationTime = float(0.15);

//-----------------------------------------------------------------------------
/**
*/
nGuiServer::nGuiServer() :
    refScriptServer("/sys/servers/script"),
    refGui("/res/gui"),
    refInputServer("/sys/servers/input"),
    isOpen(false),
    curTime(0.0),
    ActiveTextEntry(false),
    referenceSize(1.0f, 1.0f),
    uniqueId(0),
    systemGuiActive(false),
    globalColor(1.0f, 1.0f, 1.0f, 1.0f),
    curTexture(0),
    curMaxNumVertices(0),
    curVertexPointer(0),
    curVertexIndex(0),
    displaySize(1024, 768),
    texelMappingRatio(1.0f),
    toolTipSet(false),
    toolTipEnabled(true),
    clipRectStack(0, 1)
{
    n_assert(0 == Singleton);
    Singleton = this;
    this->guiWindowClass = kernelServer->FindClass("nguiwindow");

    // assign layer to GuiServer
    this->Layer = n_new(nGuiLayer());
    n_assert(this->guiWindowClass);

    #if __NEBULA_STATS__
    this->profGUIDrawBrush.Initialize("profGUI_DrawBush", true);
    this->profGUIDrawText.Initialize("profGUI_DrawText", true);
    this->profGUIDrawTexture.Initialize("profGUI_DrawTexture", true);
    #endif
}

//-----------------------------------------------------------------------------
/**
*/
nGuiServer::~nGuiServer()
{
    if (this->isOpen)
    {
        this->Close();
    }

    // free layer
    n_delete(this->Layer);

    n_assert(Singleton);
    Singleton = 0;
}

//-----------------------------------------------------------------------------
/**
    Set the root directory name in Nebula's object hierarchy. This is
    where the window objects are located. Default is "/res/gui".
*/
void
nGuiServer::SetRootPath(const char* name)
{
    n_assert(name);
    this->refGui = name;
}

//-----------------------------------------------------------------------------
/**
    Get the root directory.
*/
const char*
nGuiServer::GetRootPath() const
{
    return this->refGui.getname();
}

//-----------------------------------------------------------------------------
/**
    Create a new window object
*/
nGuiWindow*
nGuiServer::NewWindow(const char* className, bool visible)
{
    char windowName[N_MAXPATH];
    snprintf(windowName, sizeof(windowName), "window%d", this->uniqueId++);
    return this->NewWindowNamed(className, visible, windowName);
}
//-----------------------------------------------------------------------------
/**
    Create a new window object with name
*/
nGuiWindow*
nGuiServer::NewWindowNamed(const char* className, bool visible, const char* windowName)
{
    n_assert(this->isOpen);
    n_assert(className);
    n_assert(windowName);

    // tell old focus window that it is about to lose focus
    if (this->refCurrentRootWindow.isvalid())
    {
        nGuiWindow* curFocusWindow = this->refCurrentRootWindow->GetTopMostWindow();
        if (curFocusWindow)
        {
            curFocusWindow->OnLoseFocus();
        }
    }

    // create new window, if no root window is set, creates a new root window
    if (this->refCurrentRootWindow.isvalid())
    {
        kernelServer->PushCwd(this->refCurrentRootWindow.get());
    }
    else
    {
        kernelServer->PushCwd(this->refGui.get());
    }
    nGuiWindow* window = (nGuiWindow*) kernelServer->New(className, windowName);
    kernelServer->PopCwd();
    if (window)
    {
        if (visible)
        {
            window->Show();
        }
        if (window->IsBackground())
        {
            // move window to back
            nRoot* parent = window->GetParent();
            window->Remove();
            parent->AddHead(window);
        }
        else
        {
            // make new focus window
            if (this->refCurrentRootWindow.isvalid())
            {
                this->refCurrentRootWindow->SetFocusWindow(window);
            }
        }
    }
    return window;
}

//-----------------------------------------------------------------------------
/**
    Create a new skin object.
*/
nGuiSkin*
nGuiServer::NewSkin(const char* skinName)
{
    n_assert(this->isOpen);
    n_assert(skinName);

    if (this->refSkins->Find(skinName))
    {
        n_error("nGuiServer::NewSkin(): skin '%s' already exists!", skinName);
        return 0;
    }

    kernelServer->PushCwd(this->refSkins.get());
    nGuiSkin* newSkin = (nGuiSkin*) kernelServer->New("nguiskin", skinName);
    kernelServer->PopCwd();
    n_assert(newSkin);
    return newSkin;
}

//-----------------------------------------------------------------------------
/**
    Open the gui server. This will initialize any required resources.
*/
bool
nGuiServer::Open()
{
    n_assert(!this->isOpen);
    n_assert(!this->refShader.isvalid());

    // create a few default fonts
//    this->AddSystemFont("GuiDefault", "Arial", 16, false, false, false);
//    this->AddSystemFont("GuiSmall", "Arial", 14, false, false, false);

    // added just for testing new Fonts
    this->AddSystemFont("GuiItalic", "Arial", 20, true, true, false);

    nFontDesc defaultFont;
    defaultFont.SetAntiAliased(true);
    defaultFont.SetHeight(16);
    defaultFont.SetWeight(nFontDesc::Normal);
    defaultFont.SetTypeFace("Arial");
    this->refDefaultFont = nGfxServer2::Instance()->NewFont("GuiDefault", defaultFont);
    if ((!this->refDefaultFont->IsValid()) && (!this->refDefaultFont->Load()))
    {
        n_error("nGuiServer: Failed to load default gui font!");
    }
    nFontDesc smallFont;
    smallFont.SetAntiAliased(true);
    smallFont.SetHeight(14);
    smallFont.SetWeight(nFontDesc::Normal);
    smallFont.SetTypeFace("Arial");
    this->refSmallFont = nGfxServer2::Instance()->NewFont("GuiSmall", smallFont);
    if ((!this->refSmallFont->Load()) && (!this->refSmallFont->Load()))
    {
        n_error("nGuiServer: Failed to load small gui font!");
    }

    if (!this->refGui.isvalid())
    {
        kernelServer->PushCwd(kernelServer->Lookup("/"));
        kernelServer->New("nroot", this->GetRootPath());
        kernelServer->PopCwd();
        n_assert(this->refGui.isvalid());
    }

    // create skin location
    this->refSkins = kernelServer->New("nroot", "/sys/share/rsrc/skins");
    n_assert(this->refSkins.isvalid());

    // create a default root window
    kernelServer->PushCwd(this->refGui.get());
    this->refSystemRootWindow = (nGuiWindow*) kernelServer->New("nguiwindow", "SystemRootWindow");
    kernelServer->PopCwd();
    this->SetRootWindow("SystemRootWindow");

    // initialize mesh and shader
    this->ValidateMesh();
    this->ValidateShader();
    this->isOpen = true;

    // call the OnGuiServerOpen script function
    nString scriptResult;
    bool r;
    r = this->refScriptServer->RunFunction("OnGuiServerOpen", scriptResult);
    if (false == r)
    {
        n_error("OnGuiServerOpenFailed: %s",
                scriptResult.IsEmpty() ? scriptResult.Get() : "Unknown error");
    }

    // set root window to 0, this will hide the GUI
    this->refCurrentRootWindow->Hide();
    this->SetRootWindow(0);

    return true;
}

//-----------------------------------------------------------------------------
/**
    Close the gui server.
*/
void
nGuiServer::Close()
{
    n_assert(this->isOpen);

    // call the OnGuiServerClose script function
    nString scriptResult;
    bool r;
    r = this->refScriptServer->RunFunction("OnGuiServerClose", scriptResult);
    if (false == r)
    {
        n_error("OnGuiServerCloseFailed: %s",
                scriptResult.IsEmpty() ? scriptResult.Get() : "Unknown error");
    }

    // clear root window
    this->SetRootWindow(0);

    // clear default system root window
    if (this->refSystemRootWindow.isvalid())
    {
        this->refSystemRootWindow->Release();
        n_assert(!this->refSystemRootWindow.isvalid());
    }

    nGfxServer2::Instance()->SetMesh(0, 0);
    nGfxServer2::Instance()->SetShader(0);
    nGfxServer2::Instance()->SetFont(0);
    if (this->refShader.isvalid())
    {
        this->refShader->Release();
        this->refShader = 0;
    }
    if (this->refDefaultFont.isvalid())
    {
        this->refDefaultFont->Release();
        this->refDefaultFont = 0;
    }
    if (this->refSmallFont.isvalid())
    {
        this->refSmallFont->Release();
        this->refSmallFont = 0;
    }
    if (this->refSkins.isvalid())
    {
        this->refSkins->Release();
        n_assert(!this->refSkins.isvalid());
    }
    this->isOpen = false;
}

//-----------------------------------------------------------------------------
/**
    Set a new root window.
*/
void
nGuiServer::SetRootWindow(const char* name)
{
    nGuiWindow* window = 0;
    if (name)
    {
        window = (nGuiWindow*) this->refGui->Find(name);
    }
    this->SetRootWindowPointer(window);
}

//-----------------------------------------------------------------------------
/**
    Get current root window.
*/
const char*
nGuiServer::GetRootWindow() const
{
    return this->refCurrentRootWindow.isvalid() ? this->refCurrentRootWindow->GetName() : 0;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiServer::SetRootWindowPointer(nGuiWindow* window)
{
    this->refCurrentRootWindow = window;
    if (window && !window->IsShown())
    {
        window->Show();
    }
}

//-----------------------------------------------------------------------------
/**
    Returns a pointer to the current root window. May return 0 if no
    root window is set.
*/
nGuiWindow*
nGuiServer::GetRootWindowPointer() const
{
    return this->refCurrentRootWindow.isvalid() ? this->refCurrentRootWindow.get() : 0;
}

//-----------------------------------------------------------------------------
/**
   Per frame actions.
*/
void
nGuiServer::Trigger()
{
    n_assert(this->guiWindowClass);

    if (this->refCurrentRootWindow.isvalid())
    {
        // the root window may change during event handling...
        nGuiWindow* rootWindow = this->refCurrentRootWindow.get();

        // check for dismissed windows from the last frame, and release them
        nGuiWidget* curWidget = (nGuiWidget*) rootWindow->GetHead();
        nGuiWidget* nextWidget;
        while (curWidget)
        {
            nextWidget = (nGuiWidget*) curWidget->GetSucc();
            if (curWidget->IsA(this->guiWindowClass))
            {
                nGuiWindow* curWindow = (nGuiWindow*) curWidget;
                if (curWindow->IsDismissed())
                {
                    if (curWindow->IsShown())
                    {
                        curWindow->Hide();
                    }
                    curWindow->Release();
                }
            }
            curWidget = nextWidget;
        }

        // check for invalid listener references (listeners which
        // have disappeared and did not unregister themselves)
        nArray< nRef<nGuiWidget> >::iterator iter = this->eventListeners.Begin();
        for (; iter != this->eventListeners.End();)
        {
            if (!iter->isvalid())
            {
                iter = this->eventListeners.Erase(iter);
            }
            else
            {
                iter++;
            }
        }

        //handle events
        for (nInputEvent* i = this->refInputServer->FirstEvent(); i != 0; i = this->refInputServer->NextEvent(i))
        {
            n_assert(i != 0);

            if (i->GetDeviceId() == N_INPUT_MOUSE(0))
            {
                // handle mouse movement
                if (i->GetType() == N_INPUT_MOUSE_MOVE)
                {
                    this->curMousePos.set(i->GetRelXPos(), i->GetRelYPos());
                    rootWindow->OnMouseMoved(this->curMousePos);
                    if (this->refToolTip.isvalid())
                    {
                        this->refToolTip->OnMouseMoved(this->curMousePos);
                    }
                    if (this->refDragBox.isvalid())
                    {
                        this->refDragBox->OnMouseMoved(this->curMousePos);
                    }
                }

                // handle button actions
                if (i->GetType() == N_INPUT_BUTTON_DOWN)
                {
                    this->curMousePos.set(i->GetRelXPos(), i->GetRelYPos());
                    if (i->GetButton() == 0)
                    {
                        rootWindow->OnButtonDown(this->curMousePos);
                        if (this->refDragBox.isvalid() && !this->IsMouseOverGui())
                        {
                            this->refDragBox->OnButtonDown(this->curMousePos);
                        }
                    }
                    else if (i->GetButton() == 1)
                    {
                        rootWindow->OnRButtonDown(this->curMousePos);
                        if (this->refDragBox.isvalid())
                        {
                            this->refDragBox->OnRButtonDown(this->curMousePos);
                        }
                    }
                }
                if (i->GetType() == N_INPUT_BUTTON_UP)
                {
                    this->curMousePos.set(i->GetRelXPos(), i->GetRelYPos());
                    if (i->GetButton() == 0)
                    {
                        rootWindow->OnButtonUp(this->curMousePos);
                        if (this->refDragBox.isvalid())
                        {
                            this->refDragBox->OnButtonUp(this->curMousePos);
                        }
                    }
                    else if (i->GetButton() == 1)
                    {
                        rootWindow->OnRButtonUp(this->curMousePos);
                        if (this->refDragBox.isvalid())
                        {
                            this->refDragBox->OnRButtonUp(this->curMousePos);
                        }
                    }
                }
            }
            if (i->GetDeviceId() == N_INPUT_KEYBOARD(0))
            {
                // handle character input
                switch (i->GetType())
                {
                    case N_INPUT_KEY_CHAR:
                        rootWindow->OnChar( static_cast<uchar>( i->GetChar() ) );
                        break;

                    case N_INPUT_KEY_DOWN:
                        rootWindow->OnKeyDown(i->GetKey());
                        break;

                    case N_INPUT_KEY_UP:
                        rootWindow->OnKeyUp(i->GetKey());
                        break;

                    default:
                        break;
                }
            }
        }

        // call the per-frame handler method
        rootWindow->OnFrame();
    }
}

//-----------------------------------------------------------------------------
/**
    Draw text through the gfx server.
*/
void
nGuiServer::DrawText(const char* text, const vector4& color, const rectangle& rect, uint flags)
{
#if __NEBULA_STATS__
    this->profGUIDrawText.StartAccum();
#endif
    rectangle cr, r;
    if (this->GetClipRect(cr))
    {
        r = rect * cr;
    }
    else
    {
        r = rect;
    }

    // need to flush graphics rendering before rendering text
    this->FlushBrushes();
    static vector4 modColor;
    modColor.set(color.x, color.y, color.z, color.w * this->globalColor.w);
    nGfxServer2::Instance()->DrawText(text, modColor, r, flags);

#if __NEBULA_STATS__
    this->profGUIDrawText.StopAccum();
#endif
}

//-----------------------------------------------------------------------------
/**
    Initialize the rectangle mesh. This method should be called per
    frame because the mesh may become invalid (for instance when
    switching display modes).
*/
void
nGuiServer::ValidateMesh()
{
    if (!this->dynMesh.IsValid())
    {
        this->dynMesh.Initialize(nGfxServer2::TriangleList, nMesh2::Coord | nMesh2::Normal | nMesh2::Uv0, 0, false);
    }
}

//-----------------------------------------------------------------------------
/**
    Initialize the gui shader. This method must be called per frame
    because the shader may become invalid (for instance when switching
    display modes).
*/
void
nGuiServer::ValidateShader()
{
    nShader2* shader = 0;
    if (!this->refShader.isvalid())
    {
        shader = nGfxServer2::Instance()->NewShader("gui");
        shader->SetFilename("shaders:gui.fx");
        this->refShader = shader;
    }
    else
    {
        shader = this->refShader.get();
    }
    if (!shader->IsLoaded())
    {
        n_verify( shader->Load() );
    }
}

//-----------------------------------------------------------------------------
/**
    Flush the brush rendering.
*/
void
nGuiServer::FlushBrushes()
{
    if (this->curVertexIndex > 0)
    {
        this->dynMesh.Swap(this->curVertexIndex, this->curVertexPointer);
        this->curVertexIndex = 0;
    }
}

//-----------------------------------------------------------------------------
/**
    Directly draw a textured rectangle to the dynamic mesh
*/
void
nGuiServer::DrawTexture(const rectangle& rect, const rectangle& uvRect, const vector4& color, nTexture2* tex)
{
#if __NEBULA_STATS__
    this->profGUIDrawTexture.StartAccum();
#endif

    n_assert(tex);

    // clipping
    rectangle rectMesh, rectUV, clipRect;
    rectMesh = rect;
    rectUV = uvRect;

    if (this->GetClipRect(clipRect))
    {
        // get intersection of brush rectangle and clipping recttangle
        rectMesh = clipRect * rect;

        // check if rectangles really intersect
        if (rectMesh.area() == 0.0f)
        {
            return;
        }

        float widthScale, heightScale;

        widthScale = rectUV.width() / rect.width();
        heightScale = rectUV.height() / rect.height();

        // scale UV rectangle (y is swapped)
        rectUV.v0.x += (rectMesh.v0.x - rect.v0.x) * widthScale;
        rectUV.v0.y -= (rectMesh.v1.y - rect.v1.y) * heightScale;
        rectUV.v1.x += (rectMesh.v1.x - rect.v1.x) * widthScale;
        rectUV.v1.y -= (rectMesh.v0.y - rect.v0.y) * heightScale;
    }

    // compute modulated color
    vector4 modColor(color.x * this->globalColor.x,
                     color.y * this->globalColor.y,
                     color.z * this->globalColor.z,
                     color.w * this->globalColor.w);

    // check swap conditions
    bool doSwap = false;
    bool updateTexture = false;
    bool updateColor   = false;
    if ((this->curTexture != 0) && (tex != this->curTexture) || ((this->curVertexIndex + 6) >= this->curMaxNumVertices))
    {
        doSwap = true;
    }
    if (tex != this->curTexture)
    {
        this->curTexture = tex;
        updateTexture = true;
        doSwap = true;
    }
    if (!this->curColor.isequal(modColor, 0.001f))
    {
        this->curColor = modColor;
        updateColor = true;
        doSwap = true;
    }

    // if state change, force rendering
    if (doSwap)
    {
        this->FlushBrushes();
    }

    // update shader parameters
    nShader2* shader = this->refShader.get();
    if (updateTexture && shader->IsParameterUsed(nShaderState::DiffMap0))
    {
        shader->SetTexture(nShaderState::DiffMap0, this->curTexture);
    }
    if (updateColor && shader->IsParameterUsed(nShaderState::MatDiffuse))
    {
        shader->SetVector4(nShaderState::MatDiffuse, modColor);
    }

    // write vertices
    float* ptr = this->curVertexPointer + this->curVertexIndex * 8; // (coord3, norm3, uv2)
    this->curVertexIndex += 6;

    rectangle r;
    r.v0.x = rectMesh.v0.x - 0.5f;
    r.v0.y = -(rectMesh.v0.y - 0.5f);
    r.v1.x = rectMesh.v1.x - 0.5f;
    r.v1.y = -(rectMesh.v1.y - 0.5f);

    rectangle uv;
    uv.v0.x = rectUV.v0.x;
    uv.v0.y = 1.0f - rectUV.v1.y;
    uv.v1.x = rectUV.v1.x;
    uv.v1.y = 1.0f - rectUV.v0.y;

    // triangle 1
    // top left
    *ptr++ = r.v0.x;  *ptr++ = r.v0.y; *ptr++ = 0.0f;  // coord
    *ptr++ = 0.0f;    *ptr++ = 0.0f;   *ptr++ = 1.0f;  // norm
    *ptr++ = uv.v0.x; *ptr++ = uv.v0.y;

    // top right
    *ptr++ = r.v1.x;  *ptr++ = r.v0.y; *ptr++ = 0.0f;  // coord
    *ptr++ = 0.0f;    *ptr++ = 0.0f;   *ptr++ = 1.0f;  // norm
    *ptr++ = uv.v1.x; *ptr++ = uv.v0.y;

    // bottom right
    *ptr++ = r.v1.x;  *ptr++ = r.v1.y; *ptr++ = 0.0f;  // coord
    *ptr++ = 0.0f;    *ptr++ = 0.0f;   *ptr++ = 1.0f;  // norm
    *ptr++ = uv.v1.x; *ptr++ = uv.v1.y;

    // triangle 2
    // top left
    *ptr++ = r.v0.x;  *ptr++ = r.v0.y; *ptr++ = 0.0f;  // coord
    *ptr++ = 0.0f;    *ptr++ = 0.0f;   *ptr++ = 1.0f;  // norm
    *ptr++ = uv.v0.x; *ptr++ = uv.v0.y;

    // bottom right
    *ptr++ = r.v1.x;  *ptr++ = r.v1.y; *ptr++ = 0.0f;  // coord
    *ptr++ = 0.0f;    *ptr++ = 0.0f;   *ptr++ = 1.0f;  // norm
    *ptr++ = uv.v1.x; *ptr++ = uv.v1.y;

    // bottom left
    *ptr++ = r.v0.x;  *ptr++ = r.v1.y; *ptr++ = 0.0f;  // coord
    *ptr++ = 0.0f;    *ptr++ = 0.0f;   *ptr++ = 1.0f;  // norm
    *ptr++ = uv.v0.x; *ptr++ = uv.v1.y;

#if __NEBULA_STATS__
    this->profGUIDrawTexture.StopAccum();
#endif
}

//-----------------------------------------------------------------------------
/**
    Render the current GUI.
*/
void
nGuiServer::Render()
{
#if __NEBULA_STATS__
    this->profGUIDrawBrush.ResetAccum();
    this->profGUIDrawTexture.ResetAccum();
    this->profGUIDrawText.ResetAccum();
#endif
    // set the current time for all following render calls
    if (this->refCurrentRootWindow.isvalid())
    {
        nGfxServer2* gfxServer = nGfxServer2::Instance();

        this->ValidateMesh();
        this->ValidateShader();

        // update gfx server's transforms
        static const matrix44 identity;
        static const rectangle screenRect(vector2(0.0f, 0.0f), vector2(1.0f, 1.0f));
        matrix44 model = this->GetViewSpaceMatrix(screenRect);
        gfxServer->PushTransform(nGfxServer2::View, identity);
        gfxServer->PushTransform(nGfxServer2::Model, model);

        nShader2* shader = this->refShader.get();
        nGfxServer2::Instance()->SetShader(shader);
        int curPass;
        int numPasses = shader->Begin(false);
        for (curPass = 0; curPass < numPasses; curPass++)
        {
            shader->BeginPass(curPass);

            // begin rendering to dynamic mesh
            this->curTexture = 0;
            this->curColor.set(0.0f, 0.0f, 0.0f, 0.0f);
            this->curVertexIndex = 0;
            this->dynMesh.Begin(this->curVertexPointer, this->curMaxNumVertices);
            this->refCurrentRootWindow->Render();

            // render optional tooltip
            if (this->toolTipEnabled && this->refToolTip.isvalid())
            {
                if (this->refToolTip->IsShown() && !this->toolTipSet)
                {
                    this->refToolTip->Hide();
                }
                this->refToolTip->Render();
                this->toolTipSet = false;
            }

            // render optional drag box
            if (this->refDragBox.isvalid())
            {
                this->refDragBox->Render();
            }

            // finish dynamic mesh rendering
            this->dynMesh.End(this->curVertexIndex);

            shader->EndPass();
        }
        shader->End();
        gfxServer->PopTransform(nGfxServer2::Model);
        gfxServer->PopTransform(nGfxServer2::View);

        // unload all gui resources which have not been rendered
        if (this->refCurrentSkin.isvalid())
        {
            this->refCurrentSkin->UnloadUntouchedGuiResources();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Declare a UI font using a system font.

    @param  fontName    a font name which identifies this font in the UI system
    @param  typeFace    the typeface name (i.e. "Arial")
    @param  height      height in pixels
    @param  bold        bold flag
    @param  italic      italic flag
    @param  underline   underline flag
*/
void
nGuiServer::AddSystemFont(const char* fontName, const char* typeFace, int height, bool bold, bool italic, bool underline)
{
    n_assert(fontName && typeFace);

    // fill a font desc...
    nFontDesc fontDesc;
    fontDesc.SetTypeFace(typeFace);
    fontDesc.SetHeight(height);
    if (bold)
    {
        fontDesc.SetWeight(nFontDesc::Bold);
    }
    else
    {
        fontDesc.SetWeight(nFontDesc::Normal);
    }
    fontDesc.SetItalic(italic);
    fontDesc.SetUnderline(underline);
    fontDesc.SetAntiAliased(true);

    // change the font's desc if it already exists
    // otherwise, create the font
    nFont2* pFont = (nFont2*)nResourceServer::Instance()->FindResource( fontName, nResource::Font );
    if (pFont)
    {
        pFont->SetFontDesc( fontDesc );
        pFont->Unload();
    }
    else
    {
        nGfxServer2::Instance()->NewFont(fontName, fontDesc);
    }
}


//-----------------------------------------------------------------------------
/**
    Declare a UI font using a custom font file.

    @param  fontName    a font name which identifies this font in the UI system
    @param  fontFile    font filename
    @param  typeFace    the typeface name (i.e. "Arial")
    @param  height      height in pixels
    @param  bold        bold flag
    @param  italic      italic flag
    @param  underline   underline flag
*/
void
nGuiServer::AddCustomFont(const char* fontName, const char* fontFile, const char* typeFace, int height, bool bold, bool italic, bool underline)
{
    n_assert(fontName && fontFile && typeFace);

    // fill a font desc...
    nFontDesc fontDesc;
    fontDesc.SetFilename(fontFile);
    fontDesc.SetTypeFace(typeFace);
    fontDesc.SetHeight(height);
    if (bold)
    {
        fontDesc.SetWeight(nFontDesc::Bold);
    }
    else
    {
        fontDesc.SetWeight(nFontDesc::Normal);
    }
    fontDesc.SetItalic(italic);
    fontDesc.SetUnderline(underline);
    fontDesc.SetAntiAliased(true);

    // change the font's desc if it already exists
    // otherwise, create the font
    nFont2* pFont = (nFont2*)nResourceServer::Instance()->FindResource( fontName, nResource::Font );
    if (pFont)
    {
        pFont->SetFontDesc( fontDesc );
        pFont->Unload();
    }
    else
    {
        nGfxServer2::Instance()->NewFont(fontName, fontDesc);
    }
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiServer::SetTime(nTime t)
{
    this->curTime = t;
}

//-----------------------------------------------------------------------------
/**
*/
nTime
nGuiServer::GetTime() const
{
    return this->curTime;
}

//-----------------------------------------------------------------------------
/**
    Convert a screen space coordinate into a valid view space
    coordinate.
*/
vector3
nGuiServer::ConvertScreenToViewSpace(const vector2& screenCoord)
{
    // get current inverted projection matrix
    matrix44 invProj = nGfxServer2::Instance()->GetTransform(nGfxServer2::Projection);
    invProj.invert();

    vector3 screenCoord3D((screenCoord.x - 0.5f) * 2.0f, (screenCoord.y - 0.5f) * 2.0f, 1.0f);
    vector3 viewCoord = invProj * screenCoord3D;

    // get near plane
    float nearZ = nGfxServer2::Instance()->GetCamera().GetNearPlane();
    return viewCoord * nearZ * 1.1f;
}

//-----------------------------------------------------------------------------
/**
    Get a 4x4 matrix representing this widgets screen space area in view space.
*/
matrix44
nGuiServer::GetViewSpaceMatrix(const rectangle& r)
{
    vector3 v0 = this->ConvertScreenToViewSpace(r.v0);
    vector3 v1 = this->ConvertScreenToViewSpace(r.v1);

    vector3 pos  = (v0 + v1) * 0.5f;
    // pos.y = -pos.y;
    vector3 size = (v1 - v0);
    matrix44 m;
    m.scale(size);
    m.translate(pos);
    return m;
}

//-----------------------------------------------------------------------------
/**
    Draw a brush of the current skin.
*/
void
nGuiServer::DrawBrush(const rectangle& rect, nGuiBrush& brush)
{
#if __NEBULA_STATS__
    this->profGUIDrawBrush.StartAccum();
#endif
    // a null brush pointer is valid, in this case, nothing is rendered
    if (brush.GetName().IsEmpty())
    {
        #if __NEBULA_STATS__
        this->profGUIDrawBrush.StopAccum();
        #endif
        return;
    }
    // get gui resource from brush
    nGuiResource* guiResource = brush.GetGuiResource();
    n_assert(guiResource);

    // validate gui resource
    if (!guiResource->IsValid())
    {
        guiResource->Load();
    }

    this->DrawTexture(rect, guiResource->GetRelUvRect(), guiResource->GetColor(), guiResource->GetTexture());

#if __NEBULA_STATS__
    this->profGUIDrawBrush.StopAccum();
#endif
}

//-----------------------------------------------------------------------------
/**
    Draw a brush of the current skin with a given colour instead of the brush color
*/
void
nGuiServer::DrawBrush(const rectangle& rect, nGuiBrush& brush, vector4 color)
{
#if __NEBULA_STATS__
    this->profGUIDrawBrush.StartAccum();
#endif
    // a null brush pointer is valid, in this case, nothing is rendered
    if (brush.GetName().IsEmpty())
    {
        #if __NEBULA_STATS__
        this->profGUIDrawBrush.StopAccum();
        #endif
        return;
    }
    // get gui resource from brush
    nGuiResource* guiResource = brush.GetGuiResource();
    n_assert(guiResource);

    // validate gui resource
    if (!guiResource->IsValid())
    {
        guiResource->Load();
    }

    this->DrawTexture(rect, guiResource->GetRelUvRect(), color, guiResource->GetTexture());

#if __NEBULA_STATS__
    this->profGUIDrawBrush.StopAccum();
#endif
}
//-----------------------------------------------------------------------------
/**
    Set a coordinate space reference size.
*/
void
nGuiServer::SetReferenceSize(const vector2& v)
{
    this->referenceSize = v;
}

//-----------------------------------------------------------------------------
/**
    Get the current coordinate space reference size.
*/
const vector2&
nGuiServer::GetReferenceSize() const
{
    return this->referenceSize;
}

//-----------------------------------------------------------------------------
/**
    Convert a reference size rectangle (i.e. 640x480) to a screen space
    rectangle (0..1).
*/
rectangle
nGuiServer::ConvertRefToScreenSpace(const rectangle& src)
{
    rectangle r;
    r.v0.x = src.v0.x / this->referenceSize.x;
    r.v0.y = src.v0.y / this->referenceSize.y;
    r.v1.x = src.v1.x / this->referenceSize.x;
    r.v1.y = src.v1.y / this->referenceSize.y;
    return r;
}

//-----------------------------------------------------------------------------
/**
    Convert a screen space rectangle to reference space.
*/
rectangle
nGuiServer::ConvertScreenToRefSpace(const rectangle& src)
{
    rectangle r;
    r.v0.x = src.v0.x * this->referenceSize.x;
    r.v0.y = src.v0.y * this->referenceSize.y;
    r.v1.x = src.v1.x * this->referenceSize.x;
    r.v1.y = src.v1.y * this->referenceSize.y;
    return r;
}

//-----------------------------------------------------------------------------
/**
    Check if a textentry is being edited at this moment.
*/
bool
nGuiServer::GetActiveTextEntry() const
{
    if (this->refCurrentRootWindow.isvalid())
    {
        // if current topmost window is modal, always return true
        nGuiWindow* topMostWindow = this->refCurrentRootWindow->GetTopMostWindow();
        if (topMostWindow && topMostWindow->IsModal())
        {
            return true;
        }

        return (this->ActiveTextEntry);
    }

    return false;
}

void
nGuiServer::SetActiveTextEntry(bool f)
{
    this->ActiveTextEntry = f;
}

//-----------------------------------------------------------------------------
/**
    Check if the mouse is currently over an GUI element.
*/
bool
nGuiServer::IsMouseOverGui() const
{
    if (this->refCurrentRootWindow.isvalid())
    {
        // if current topmost window is modal, always return true
        nGuiWindow* topMostWindow = this->refCurrentRootWindow->GetTopMostWindow();
        if (topMostWindow && topMostWindow->IsModal())
        {
            return true;
        }

        // check child widgets (including windows)
        nGuiWidget* widget;
        for (widget = (nGuiWidget*) this->refCurrentRootWindow->GetHead();
             widget;
             widget = (nGuiWidget*) widget->GetSucc())
        {
            if (widget->IsShown() &&
                (!widget->IsBackground()) &&
                (widget->Inside(this->curMousePos)))
            {
                return true;
            }
        }
    }
    // fallthrough: mouse not over any gui element
    return false;
}

//-----------------------------------------------------------------------------
/**
    Display a tooltip under the mouse.
*/
void
nGuiServer::ShowToolTip(const char* text, const vector4& textColor)
{
    n_assert(text);

    // setting current tool tip widget avoids hiding the tool tip
    this->toolTipSet = true;

    if (!this->refToolTip.isvalid())
    {
        // initialize the optional tooltip
        this->refToolTip = (nGuiToolTip*) this->refGui->Find("Tooltip");
        if(this->refToolTip.isvalid())
        {
            this->refToolTip->Hide();
        }
    }
    else
    {
        // update tool tip
        if(strcmp(this->refToolTip->GetText(), text) != 0)
        {
            this->refToolTip->SetColor(textColor);
            this->refToolTip->SetText(text);
        }
        if(!this->refToolTip->IsShown())
        {
            this->refToolTip->Show();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Hide tooltip.
*/
void
nGuiServer::HideToolTip()
{
    if (this->refToolTip.isvalid())
    {
        if (this->refToolTip->IsShown())
        {
            this->refToolTip->Hide();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Return true if tooltip currently shown.
*/
bool
nGuiServer::IsToolTipShown() const
{
    if (this->refToolTip.isvalid())
    {
        return this->refToolTip->IsShown();
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
/**
    Runs a widget script command. This is a helper method for widgets.
    Before calling the command, the widget will be made current object.
*/
bool
nGuiServer::RunCommand(nGuiWidget* who, const nString& cmd)
{
    n_assert(who);
    if (!cmd.IsEmpty())
    {
        nString result;
        kernelServer->PushCwd(who);
        this->refScriptServer->Run(cmd.Get(), result);
        kernelServer->PopCwd();
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
    Register an event listener. Event listener widgets are notified
    through the OnEvent() method when the PutEvent() method is invoked.
*/
void
nGuiServer::RegisterEventListener(nGuiWidget* w)
{
    nRef<nGuiWidget> refWidget(w);
    if (!this->eventListeners.Find(refWidget))
    {
        this->eventListeners.Append(refWidget);
    }
}

//-----------------------------------------------------------------------------
/**
    Unregister an event listener.
*/
void
nGuiServer::UnregisterEventListener(nGuiWidget* w)
{
    nRef<nGuiWidget> refWidget(w);
    nArray< nRef<nGuiWidget> >::iterator iter = this->eventListeners.Find(refWidget);
    if (0 != iter)
    {
        // NOTE: just invalidate the nRef here, the array
        // entry will be removed at the next nGuiServer::Trigger()
        iter->invalidate();
    }
}

//-----------------------------------------------------------------------------
/**
    Put an event. The event will immediately distributed to all event
    listeners.
*/
void
nGuiServer::PutEvent(const nGuiEvent& event)
{
    int num = this->eventListeners.Size();
    int i;
    for (i = 0; i < num; i++)
    {
        if (this->eventListeners[i].isvalid() )
        {
            this->eventListeners[i]->OnEvent(event);
        }
    }

    // send event to layer
    nGuiLayer::tWindow * layerwin;
    layerwin = nGuiServer::Instance()->Layer->WinList_Find(event.GetWidget()->GetOwnerWindow());
    if (layerwin)
    {
        nGuiServer::Instance()->Layer->WinList_HandleEvent(layerwin, event);
        //nGuiServer::Instance()->Layer->Log("guilayer.txt", "[LAYER] nGuiServer::PutEvent (window[%d])", layerwin->wid);
    }
}

//-----------------------------------------------------------------------------
/**
    Toggle the system GUI. This will toggle between the SystemRootWindow
    and the user set root window.
*/
void
nGuiServer::ToggleSystemGui()
{
    // cursor visibility
    static nGfxServer2::CursorVisibility userSkinCursorVisibility;
    static bool userSkinCursorVisibilityKown = false;

    static const nGfxServer2::CursorVisibility SystemSkinVisbility = nGfxServer2::System;

    nGfxServer2* gfxServer = nGfxServer2::Instance();
    n_assert(gfxServer);

    if (this->systemGuiActive)
    {
        nGuiWindow* userRootWindow = this->refUserRootWindow.isvalid() ? this->refUserRootWindow.get() : 0;
        nGuiSkin* userSkin = this->refUserSkin.isvalid() ? this->refUserSkin.get() : 0;

        // restore userSkin cursor visibility
        if (userSkinCursorVisibilityKown)
        {
            userSkinCursorVisibilityKown = false; // make shure we forget, to never set unintialized
            gfxServer->SetCursorVisibility(userSkinCursorVisibility);
        }

        this->SetSkin(userSkin);
        this->SetRootWindowPointer(userRootWindow);
        this->systemGuiActive = false;
    }
    else
    {
        this->refUserSkin = this->GetSkin();
        this->refUserRootWindow = this->GetRootWindowPointer();

        // store mouse pointer state, and activate mouse for system gui
        userSkinCursorVisibility = gfxServer->GetCursorVisibility();
        userSkinCursorVisibilityKown = true;
        gfxServer->SetCursorVisibility(SystemSkinVisbility);

        this->SetSkin(this->GetSystemSkin());
        this->SetRootWindowPointer(this->refSystemRootWindow.get());
        this->systemGuiActive = true;
    }
}

//-----------------------------------------------------------------------------
/**
    Computes the screen space size of a brush if a 1:1 mapping
    is assumed between brush texels and screen pixels. If the brush
    does not exist, or no skin is set, a null vector will be returned.
*/
vector2
nGuiServer::ComputeScreenSpaceBrushSize(const char* brushName)
{
    nGuiBrush tmpBrush(brushName);
    vector2 size;
    nGuiResource* guiResource = tmpBrush.GetGuiResource();
    n_assert(guiResource);
    const rectangle& uvRect = guiResource->GetRelUvRect();
    float texWidth  = (float) guiResource->GetTextureWidth() * this->texelMappingRatio;
    float texHeight = (float) guiResource->GetTextureHeight() * this->texelMappingRatio;
    size.x = (uvRect.width() * texWidth) / this->displaySize.x;
    size.y = (uvRect.height() * texHeight) / this->displaySize.y;
  
    
    // OPEN SOURCE??
    /*
    
    float texWidth   = (float) guiResource->GetTextureWidth();
    float texHeight  = (float) guiResource->GetTextureHeight();
    const nDisplayMode2& dispMode = nGfxServer2::Instance()->GetDisplayMode();
    float dispWidth  = (float) dispMode.GetWidth();
    float dispHeight = (float) dispMode.GetHeight();
    size.x = (uvRect.width() * texWidth) / dispWidth;
    size.y = (uvRect.height() * texHeight) / dispHeight;
    */
    return size;
}

//-----------------------------------------------------------------------------
/**
    Returns true iff the named brush exists in the current skin
*/
bool
nGuiServer::BrushExists(const char* brushName)
{
    bool retVal = false;
    nGuiSkin* pSkin = GetSkin();
    if (pSkin && pSkin->FindBrush(brushName))
    {
        retVal = true;
    }
    return retVal;
}

//-----------------------------------------------------------------------------
/**
    Play a GUI sound.
*/
void
nGuiServer::PlaySound(const char* name)
{
    nGuiSkin* curSkin = this->GetSkin();
    if (curSkin)
    {
        nSound3* soundObject = curSkin->GetSoundObject(name);
        if (soundObject)
        {
            nAudioServer3::Instance()->StartSound(soundObject);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Discard all windows.
*/
void
nGuiServer::DiscardWindows(const char* className)
{
    nGuiWindow* rootWindow = this->GetRootWindowPointer();
    if (rootWindow)
    {
        nClass* windowClass = nKernelServer::Instance()->FindClass(className);
        n_assert(windowClass);
        nGuiWidget* child = (nGuiWidget*) rootWindow->GetHead();
        nGuiWidget* nextChild = 0;
        if (child) do
        {
            nextChild = (nGuiWidget*) child->GetSucc();
            if (child->IsA(windowClass))
            {
                child->Release();
            }
        }
		while ( 0 != (child = nextChild));
    }
}

//-----------------------------------------------------------------------------
/**
    Find next window of given class.

    @param  className   window class name
    @param  curWindow   window to start searching, 0 for begin
    @return             pointer to window, or 0
*/
nGuiWindow*
nGuiServer::FindWindowByClass(const char* className, nGuiWindow* curWindow)
{
    nGuiWindow* rootWindow = this->GetRootWindowPointer();
    if (rootWindow)
    {
        nClass* windowClass = nKernelServer::Instance()->FindClass(className);
        n_assert(windowClass);
        nGuiWidget* child;
        if (curWindow)
        {
            child = (nGuiWidget*) curWindow->GetSucc();
        }
        else
        {
            child = (nGuiWidget*) rootWindow->GetHead();
        }
        nGuiWidget* nextChild = 0;
        if (child) do
        {
            nextChild = (nGuiWidget*) child->GetSucc();
            if (child->IsA(windowClass))
            {
                return (nGuiWindow*) child;
            }
        } while ( 0 != (child = nextChild));
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Make sure that the rectangle is within screen boundaries.
*/
void
nGuiServer::MoveRectToVisibleArea(rectangle& r) const
{
    vector2 size = r.v1 - r.v0;
    if (r.v0.x < 0.0f)
    {
        r.v0.x = 0.0f;
        r.v1.x = size.x;
    }
    else if (r.v1.x > 1.0f)
    {
        r.v1.x = 1.0f;
        r.v0.x = 1.0f - size.x;
    }
    if (r.v0.y < 0.0f)
    {
        r.v0.y = 0.0f;
        r.v1.y = size.y;
    }
    else if (r.v1.y > 1.0f)
    {
        r.v1.y = 1.0f;
        r.v0.y = 1.0f - size.y;
    }
}

//------------------------------------------------------------------------------
/**
    Returns the size of 1 pixel.
*/
vector2
nGuiServer::GetPixelSize() const
{
    vector2 pixelSize(1.0f / this->displaySize.x, 1.0f / this->displaySize.y);
    return pixelSize;
}

//-----------------------------------------------------------------------------
/**
*/
void
nGuiServer::SetDragBox(nGuiDragBox* dragBox)
{
    if (dragBox)
    {
        n_assert(dragBox->IsA(kernelServer->FindClass("nguidragbox")));
    }
    this->refDragBox = dragBox;
}

//-----------------------------------------------------------------------------
/**
    Find intersection with previous clipping rectangle and push result in stack
*/
void
nGuiServer::PushClipRect(rectangle& cr)
{
    if (cr.area() > 0.0f && this->clipRectStack.Size() > 0)
    {
        this->clipRectStack.Append(this->clipRectStack.Back() * cr);
    }
    else
    {
        this->clipRectStack.Append(cr);
    }
}

