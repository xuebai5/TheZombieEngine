#ifndef N_APPVIEWPORTUI_H
#define N_APPVIEWPORTUI_H
//------------------------------------------------------------------------------
/**
    @class nAppViewportUI
    @ingroup Application
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Application viewportUI handle.
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "mathlib/polar.h"
#include "mathlib/vector.h"
#include "mathlib/rectangle.h"
#include "kernel/ncmdprotonativecpp.h"

class nAppViewport;

//------------------------------------------------------------------------------
class nAppViewportUI : public nRoot
{
public:
    /// constructor
    nAppViewportUI();
    /// destructor
    virtual ~nAppViewportUI();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// reset to default state
    virtual void Reset();
    /// perform layout logic
    virtual void Trigger();
    /// called on state to perform 3d rendering
    virtual void OnRender3D();
    /// called on state to perform 2d rendering
    virtual void OnRender2D();
    /// called before nSceneServer::RenderScene()
    virtual void OnFrameBefore();
    /// called after nSceneServer::RenderScene()
    virtual void OnFrameRendered();
    /// called when visible state changes
    virtual void VisibleChanged();

    /// open viewport layout
    virtual void Open();
    /// shutdown viewport layout
    virtual void Close();

    /// set current viewport
    void SetCurrentViewport(nAppViewport *);
    /// get current viewport
    nAppViewport *GetCurrentViewport();
    /// find viewport by name
    nAppViewport *FindViewport(const char *);
    /// find viewport by index
    nAppViewport *GetViewportAt(const int);

    /// @name Script interface
    //@{

        /// set client area size in pixels
        void SetClientRect(int, int, int, int);
        /// get client area size in pixels
        void GetClientRect(int&, int&, int&, int&);
        /// set maximized viewport mode
        void SetMaximizedViewport(bool);
        /// set single viewport mode
        void SetSingleViewport(bool);
        /// get single viewport mode
        bool GetSingleViewport();
        /// set viewport visible
        void SetVisible(bool);
        /// viewport is visible
        bool GetVisible();

        /// set width of dragging bars
        void SetBorderWidth(float);
        /// get width of dragging bars
        float GetBorderWidth();
        /// open a new dragbar rectangle in the current layout: name, alignment(vertical, horizontal), knob
        void OpenDragbar(const char *, const char *, float);
        /// close dragbar
        void CloseDragbar(const char *);
        /// set dragbar knob
        void SetDragbarKnob(const char *, float);
        /// get dragbar knob
        float GetDragbarKnob(const char *);
        /// add a viewport by name (for persistence only)
        void AddViewport(const char *);
        /// dock a viewport to a dragbar in the current layout
        void DockViewport(const char *, const char *, const char *);
        /// undock a viewport from a dragbar in the current layout
        void UndockViewport(const char *);
        /// close current viewport
        void CloseViewport(const char *);
        /// get number of open viewports
        int GetNumViewports();
        // @todo z-ordering - BringToFront, SendToBack

        /// delete all viewports and dockbars
        void ClearAll();

    //@}

protected:
    enum
    {
        MaxViewports = 8,
        MaxDockedViewports = 4,
    };

    class ViewportEntry
    {
    public:
        ViewportEntry();
        ViewportEntry(nAppViewportUI* vpUI, const char* name);
        void SetViewportUI(nAppViewportUI* viewportUI);
        nAppViewport *GetViewport();
        void SetName(nString& name);
        nString& GetName();
    private:
        nAppViewportUI *viewportUI;
        nString viewportName;
        nRef<nAppViewport> refViewport;
    };

    /// @name Dragbars (Viewport resizers) management
    //@{
    enum DragBarType
    {
        DRAG_VERTICAL = 0,
        DRAG_HORIZONTAL,
    };
    enum DockSide
    {
        SIDE_NONE = 0,
        SIDE_UP,
        SIDE_DOWN,
        SIDE_LEFT,
        SIDE_RIGHT,
    };
    struct DockedViewport
    {
        DockSide side;
        nString viewportName;
        nRef<nAppViewport> refViewport;
    };
    struct DragBar
    {
        //DragBar* parent; // @todo update for nested dragbars?
        DragBarType type;
        rectangle rect;
        nString name;
        float knob;
        int numDockedViewports;
        DockedViewport docks[MaxDockedViewports];
    };

    nArray<DragBar> dragBars;
    DragBar* draggingBar;
    vector2 draggingBarStart;
    rectangle absClientRect; ///< in screen coordinates, just to set from script
    rectangle oldClientRect; ///< in screen coordinates, kept when in maximizedViewportMode
    rectangle relClientRect; ///< in the 0..1 range
    float borderWidth;       ///< width of the dragbars in pixels
    
    /// convert dragbar type to string
    static const char *DragBarTypeToString(DragBarType type);
    /// convert dock side to string
    const char *DockSideToString(DockSide side);
    /// find drag bar by name
    DragBar* FindDragBar(const char *name);
    /// update layout for all changes
    void UpdateLayout();
    //@}
    
    ViewportEntry viewports[MaxViewports];  ///< array of ViewportEntry
    nAppViewport* curViewport;              ///< pointer to current Viewport
    ushort numViewports;                    ///< # viewports
    uint viewportUID;                       ///< unique id when creating viewports (for NOH)
    bool isOpen;
    bool isVisible;
    bool isLayoutDirty;
    bool singleViewportMode;                ///< current viewport covers the entire client area
};

//------------------------------------------------------------------------------
/**
*/
inline 
bool
nAppViewportUI::GetVisible()
{
    return this->isVisible;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAppViewportUI::ViewportEntry::ViewportEntry() :
    viewportUI(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nAppViewportUI::ViewportEntry::ViewportEntry(nAppViewportUI *vpUI, const char *name) :
    viewportUI(vpUI),
    viewportName(name)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppViewportUI::ViewportEntry::SetName(nString& viewportName)
{
    this->viewportName = viewportName;
    this->refViewport.invalidate();
}

//------------------------------------------------------------------------------
/**
*/
inline
nString&
nAppViewportUI::ViewportEntry::GetName()
{
    return this->viewportName;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAppViewport *
nAppViewportUI::ViewportEntry::GetViewport()
{
    if (!refViewport.isvalid() && !this->viewportName.IsEmpty())
    {
        this->refViewport = (nAppViewport *) this->viewportUI->Find(this->viewportName.Get());
    }
    return this->refViewport.isvalid() ? this->refViewport.get() : 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAppViewportUI::SetBorderWidth(float width)
{
    this->borderWidth = width;
    this->isLayoutDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAppViewportUI::GetBorderWidth()
{
    return this->borderWidth;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAppViewportUI::DragBar*
nAppViewportUI::FindDragBar(const char *name)
{
    for (int i = 0; i < this->dragBars.Size(); i++)
    {
        if (this->dragBars[i].name == nString(name))
        {
            return &this->dragBars[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nAppViewportUI::DragBarTypeToString(DragBarType type)
{
    switch (type)
    {
    case DRAG_VERTICAL:
        return "vertical";

    case DRAG_HORIZONTAL:
        return "horizontal";
    }
    return "";
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nAppViewportUI::DockSideToString(DockSide side)
{
    switch (side)
    {
    case SIDE_UP:
        return "up";

    case SIDE_DOWN:
        return "down";

    case SIDE_LEFT:
        return "left";

    case SIDE_RIGHT:
        return "right";

    default:
        return 0;
    }
}

//------------------------------------------------------------------------------
#endif
