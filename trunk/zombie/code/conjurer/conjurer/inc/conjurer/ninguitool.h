#ifndef N_NINGUI_TOOL_H
#define N_NINGUI_TOOL_H
//------------------------------------------------------------------------------
/**
    @file ninguitool.h
    @class nInguiTool
    @ingroup NebulaConjurerEditor

    @author Juan Jose Luna Espinosa

    @brief Abstract class that encapsulates a (Summoner) editor tool

    (C) 2004 Conjurer Services, S.A.

    This class encapsulates the input handling, graphic displaying and implementation of a summoner tool.

    Every tool will be a final class in a hierarchy. The base class provides the following concepts.

    1) State: The state of a tool can represent different things depending on the concrete tool. It can be:
        - NotInited The tool has not been used since it was selected
        - Inactive. The tool is not being used, though it could still have displaying behaviour
        - Active. The tool is being applied through at least two application frames
        - Finishing. The tool is in a finishing state. It's like active, but indicates the last frame in that state.

        Some tools may not need all the states, e.g. it can be sufficient with NotInited, Active and Inactive.
        
        Note that the tool state is entirely controlled from the outside of it. So there are the GetState and
        SetState methods.

    2) Four virtual methods, wich are:

        HandleInput - Tells the tool to take any data it needs to update its internal state according to user 
                      input.
        Pick - Every summoner tool can use a mouse event over a 3d viewport to actually apply itself. The Pick
               method updates the internal data needed to do so. Normally called inside HandleInput, can also
               be called where it is needed. A constant method, IsFirstPick, is associated with this. It tells
               wether the next pick (done in a given viewport) will be the first of a user action-related
               succesion of picks. It can tell by the current state and previous viewport.

        Apply - Actually apply the tool. At least one call to Pick is neede prior to calling this one.

        Draw - Called from a graphics related environment, draws on the viewport the visual helpers associated
               with the tool. Associated with this are the SetDrawEnabled and GetDrawEnabled, wich controls a 
               flag for generic drawing enabling.

*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "signals/nsignalnative.h"
#include "kernel/ncmdprotonativecpp.h"
#include "kernel/ntimeserver.h"
//------------------------------------------------------------------------------

class nAppViewport;

class nInguiTool: public nRoot
{
    public:

    enum ToolState
    {
        NotInited,
        Inactive,
        Starting,
        Active,
        Finishing
    };

    /// Standard. framerate when applying a tool
    const static nTime StdFrameRate;

    // Constructor
    nInguiTool();

    // Destructor
    virtual ~nInguiTool();

    /// Handle input in a viewport.
    virtual bool HandleInput( nAppViewport* vp );
    
    /// Do a pick in a viewport.
    virtual float Pick( nAppViewport* vp, vector2 mp, line3 ray );

    /// Apply the tool
    virtual bool Apply( nTime dt );

    /// Draw the visual elements of the tool
    virtual void Draw( nAppViewport* vp, nCamera2* camera );

	/// Called when tool has been selected
    virtual void OnSelected();

    /// Called when tool has been selected
    virtual void OnDeselected();

    /// Convert a mouse position to a world ray, given a viewport
    static void Mouse2Ray( nAppViewport* vp, vector2 mousePos, line3& ray );

    /// Tell the size an object at distance d would have if it is 1 pixel wide in the screen (optionally n pixels wide)
    static float Screen2WorldObjectSize( nAppViewport* vp, vector3 pos, float numPixels = 1 );

    /// Sets state
   virtual void SetState(int);

    /// Gets state
    int/*nInguiTool::ToolState*/ GetState();

    /// Set drawing enabled
    void SetDrawEnabled(bool);

    /// Get drawing enabled
    bool GetDrawEnabled();

    /// Get first position picked in the same tool dragging action. Returns false if in state NotInited
    bool GetFirstPosition(vector3&);

    /// Get last position picked. Returns false if in state NotInited
    bool GetLastPosition(vector3&);

    /// Get label
    nString GetLabel ();

    // Signals
    NSIGNAL_DECLARE('JRIN', void, RefreshInfo, 0, (), 0, ());

    /// Tells, depending on internal state and previous picked viewport, if this is the first pick
    bool IsFirstPick( nAppViewport* vp );

    /// Tells if the Pick method should be called while idle (mouse button not pressed)
    bool PickWhileIdle();

    /// Tells if can be used when running physics simulation
    bool CanApplyWhenRunningPhysics();

protected:
    // Previous viewport that was used for mouse input handling
    nAppViewport* previousViewport;

    /// Current tool state
    ToolState state;
    
    /// First and last picking positions in a dragging user action
    vector3 firstPos, lastPos;

    /// Global flag to indicate displaying need
    bool drawEnabled;

    /// Label
    nString label;

    /// Flag that tells if the tool needs picking while the mouse button is not pressed
    bool pickWhileIdle;

    /// Flag that tells if the tool can be used when running physics simulation
    bool canApplyWhenRunningPhysics;

private:

};
//------------------------------------------------------------------------------
/**
    @brief Get tool state
    @return The tool state
*/
inline
void
nInguiTool::SetState( int s )
{
    n_assert( ((nInguiTool::ToolState)s) >= NotInited && ((nInguiTool::ToolState)s) <= Finishing );
    this->state = ToolState(s);
}

//------------------------------------------------------------------------------
/**
    @brief Get tool state
    @return The tool state
*/
inline
int
//nInguiTool::ToolState
nInguiTool::GetState( void )
{
    return (this->state);
}

//------------------------------------------------------------------------------
/**
    @brief Set drawing enabled.
    @param en Indicates if drawing should be enabled
*/
inline
void
nInguiTool::SetDrawEnabled( bool en )
{
    this->drawEnabled = en;
}
//------------------------------------------------------------------------------
/**
    @brief Get if drawing is enabled
    @return If drawing is enabled
*/
inline
bool
nInguiTool::GetDrawEnabled( void )
{
    return this->drawEnabled;
}
//------------------------------------------------------------------------------
/**
    Get first position picked in the same tool dragging. Returns false if in state NotInited
    @param pos Vector where last position is returned
    @return False only if the state is NotInited
*/
inline
bool
nInguiTool::GetFirstPosition( vector3& pos )
{
    pos = firstPos;
    return this->state > Inactive;
}
//------------------------------------------------------------------------------
/**
    Get last position picked. Returns false if in state NotInited
    @param pos Vector where last position is returned
    @return False only if the state is NotInited
*/
inline
bool
nInguiTool::GetLastPosition( vector3& pos )
{
    pos = lastPos;
    return this->state > Inactive;
}
//------------------------------------------------------------------------------
/**
    Tells, depending on internal state and the previous picked viewport, if this is the first pick.
    @param vp Viewport to check if it's going to be the first pick.
    @return See description

    This functions tells if the next pick, if done in the viewport vp, will be the first one. If it returns
    false it means that there have been previous successive picks in this viewport.

*/
inline
bool
nInguiTool::IsFirstPick( nAppViewport* vp )
{
    return ( this->state <= Inactive || this->previousViewport != vp );
}
//------------------------------------------------------------------------------
/**
    Tells if the Pick method should be called while idle (mouse button not pressed)
*/
inline
bool
nInguiTool::PickWhileIdle()
{
    return this->pickWhileIdle;
}
//------------------------------------------------------------------------------
#endif
