#include "precompiled/pchexplorer.h"
//------------------------------------------------------------------------------
//  nexplorerstate_input.cc
//  (C) 2005 The Zombie Team
//------------------------------------------------------------------------------
#include "conjurer/nexplorerstate.h"
#include "napplication/napplication.h"
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"

nNebulaClass(nExplorerState, "neditorstate");

nExplorerState::nExplorerState()
{
}

nExplorerState::~nExplorerState()
{
}

//------------------------------------------------------------------------------
/**
    Handle general input. When overriden in subclasses, 
    always call nEditorState::HandleInput(frameTime) at the end.
*/
bool
nExplorerState::HandleInput(nTime /*frameTime*/)
{
    // handle viewport input
    nAppViewport* curViewport = this->refViewportUI->GetCurrentViewport();
    if (!this->app->GetInputHandled())
    {
        if (curViewport->HandleInput(this->app->GetFrameTime()))
        {
            return true;
        }
    }

    return false;
}
