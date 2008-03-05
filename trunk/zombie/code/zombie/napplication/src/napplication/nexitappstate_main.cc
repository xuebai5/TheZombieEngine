#include "precompiled/pchnapplication.h"
//------------------------------------------------------------------------------
//  nexitappstate_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "napplication/nexitappstate.h"
#include "napplication/napplication.h"

nNebulaClass(nExitAppState, "nappstate");

//------------------------------------------------------------------------------
/**
*/
nExitAppState::nExitAppState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nExitAppState::~nExitAppState()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nExitAppState::OnStateEnter(const nString& /*prevState*/)
{
    this->app->SetQuitRequested(true);
}
