#ifndef N_EDITORSTATE_H
#define N_EDITORSTATE_H
//------------------------------------------------------------------------------
/**
    @class nEditorState
    @ingroup NebulaConjurerEditor

    Default editor state for the Conjurer Application.

    (C) 2004 Conjurer Services, S.A.
*/
#include "ncommonapp/ncommonstate.h"
#include "misc/nwatched.h"

class nConjurerApp;
class nRenderContext;
class nAppViewportUI;
class nAppViewport;
//------------------------------------------------------------------------------
class nEditorState : public nCommonState
{
public:
    /// constructor
    nEditorState();
    /// destructor
    virtual ~nEditorState();
    /// called on state to perform state logic 
    virtual void OnCreate(nApplication* app);
    /// called when state is becoming active
    virtual void OnStateEnter(const nString& prevState);
    /// called when state is becoming inactive
    virtual void OnStateLeave(const nString& nextState);
    /// called on state to perform state logic 
    virtual void OnFrame();
    /// called on state to perform 3d rendering
    virtual void OnRender3D();
    /// called on state to perform 2d rendering
    virtual void OnRender2D();

protected:
    /// handle input
    virtual bool HandleInput(nTime frameTime);
    /// get application
    nConjurerApp *GetApp() const;

    nWatched watchViewerPos;
    nWatched watchFov;
    nWatched watchVel;
	nWatched watchVisObjects;
    nWatched watchVisCells;

    nRef<nAppViewportUI> refViewportUI;
    nRef<nAppViewport> refPreview;
    nRef<nAppViewport> refMapView;
};

//------------------------------------------------------------------------------
/**
*/
inline
nConjurerApp*
nEditorState::GetApp() const
{
    return (nConjurerApp*) this->app.get();
}

//------------------------------------------------------------------------------
#endif    
