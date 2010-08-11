#include "precompiled/pchsummoner.h"
//------------------------------------------------------------------------------
//  nsummonerstate_input.cc
//  (C) 2005 The Zombie Team
//------------------------------------------------------------------------------
#include "conjurer/nsummonerstate.h"
#include "napplication/napplication.h"
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"

nNebulaClass(nSummonerState, "neditorstate");

nSummonerState::nSummonerState()
{
}

nSummonerState::~nSummonerState()
{
}

//------------------------------------------------------------------------------
/**
    Handle general input. When overriden in subclasses, 
    always call nEditorState::HandleInput(frameTime) at the end.
*/
bool
nSummonerState::HandleInput(nTime /*frameTime*/)
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
