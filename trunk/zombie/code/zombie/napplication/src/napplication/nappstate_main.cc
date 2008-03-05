#include "precompiled/pchnapplication.h"
//------------------------------------------------------------------------------
//  nappstate_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "napplication/nappstate.h"

nNebulaScriptClass(nAppState, "nroot");

//------------------------------------------------------------------------------
/**
*/
nAppState::nAppState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nAppState::~nAppState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnCreate(nApplication* application)
{
    n_assert(application);
    n_assert(!this->app.isvalid());
    this->app = application;
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnStateEnter(const nString& /*prevState*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnStateLeave(const nString& /*nextState*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnFrame()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnRender3D()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nAppState::OnRender2D()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Callback method which is called in the render loop before
    nSceneServer::BeginScene() is called. Overwrite this method
    in a subclass as your necessary.

    - 08-Jun-05    kims    Added
*/
void
nAppState::OnFrameBefore()
{
    // empty
}
    
//------------------------------------------------------------------------------
/**
    Callback method which is called in the render loop after
    nSceneServer::RenderScene() is called. Overwrite this method
    in a subclass as your necessary.

    - 08-Jun-05    kims    Added
*/
void
nAppState::OnFrameRendered()
{
    // empty
}
