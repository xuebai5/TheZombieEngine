//------------------------------------------------------------------------------
//  nconjurerwindowhandler_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchconjurerapp.h"
#include "conjurer/nconjurerwindowhandler.h"
#include "conjurer/nconjurerapp.h"

//------------------------------------------------------------------------------
/**
    Constructor
*/
nConjurerWindowHandler::nConjurerWindowHandler(nD3D9Server* ptr) :
    nD3D9WindowHandler(ptr)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Called when WM_CLOSE is received, return true if app should quit
*/
bool
nConjurerWindowHandler::OnClose()
{
    nConjurerApp::Instance()->SetQuitConfirmationPending(true);
    return false;
}
