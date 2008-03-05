#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  neditorstate_input.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/neditorstate.h"
#include "conjurer/nconjurerapp.h"
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"
#include "input/ninputserver.h"
#include "rnsstates/rnsgamestate.h"
#include "rnsstates/rnswanderstate.h"

#ifndef NGAME
#include "rnsstates/naitester.h"
#endif//NGAME
//------------------------------------------------------------------------------
/**
    Handle general input. When overriden in subclasses, 
    always call nEditorState::HandleInput(frameTime) at the end.
*/
bool
nEditorState::HandleInput(nTime /*frameTime*/)
{
    nInputServer* inputServer = nInputServer::Instance();

    // switch to game mode (copying camera parameters)
    if (inputServer->GetButton("game_cam_navigator"))
    {
#ifndef NGAME
        if( nAITester::Instance()->IsGameplayUpdatingEnabled() )
        {
            nAITester::Instance()->DisableGameplayUpdating();
            nConjurerApp::Instance()->RestoreEditorState();
        }
#endif//NGAME

        RnsGameState * gamestate = static_cast<RnsGameState*>( nConjurerApp::Instance()->FindState("game") );
        n_assert2( gamestate, "Failed to found \"game\" state" );
        nAppViewport *curViewport = this->refViewportUI->GetCurrentViewport();
        gamestate->SetPreviousViewport( curViewport );
        gamestate->SetNextState( "editor" );

        nConjurerApp::Instance()->SetState( "menu" );

        return true;
    }

    if (inputServer->GetButton("game_cam_wander"))
    {
        // get the state
        RnsWanderState* state(static_cast<RnsWanderState*>(nConjurerApp::Instance()->FindState("wander")));
        n_assert2( state, "Failed to find the \"wander\" state." );

        // stores the previous editor
        state->SetPreviousViewport( this->refViewportUI->GetCurrentViewport() );

        // set the state to wander
        nConjurerApp::Instance()->SetState( "wander" );
        return true;
    }

    // show/hide the level map
    if (inputServer->GetButton("toggle_map"))
    {
        this->refMapView->SetVisible(!this->refMapView->GetVisible());
        return true;
    }

    // handle input for preview viewport 
    if (this->refMapView->GetVisible())
    {
        this->app->SetInputHandled(this->refMapView->HandleInput(this->app->GetFrameTime()));
    }

    // handle input for preview viewport 
    if (this->refPreview->GetVisible())
    {
        this->app->SetInputHandled(this->refPreview->HandleInput(this->app->GetFrameTime()));
    }

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
