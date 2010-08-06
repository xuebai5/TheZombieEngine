#include "precompiled/pchexplorer.h"
//------------------------------------------------------------------------------
//  nExplorerApp_input.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nexplorerapp.h"
#include "conjurer/npreviewviewport.h"
#include "conjurer/nobjecteditorstate.h"
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"
#include "input/ninputserver.h"
#include "misc/nconserver.h"
#include "kernel/nscriptserver.h"
#include "nspatial/nspatialserver.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphygeomsphere.h"
#include "nphysics/nphygeomray.h"
#include "entity/nentityobjectserver.h"
#include "zombieentity/nctransform.h"

//------------------------------------------------------------------------------
/**
    Handle application input.

    @return true if input has already been handled
*/
bool
nExplorerApp::HandleInput()
{
    nInputServer* inputServer = nInputServer::Instance();

    // viewports input
    this->refViewportUI->Trigger();
    if (this->GetInputHandled())
    {
        return true;
    }
    
    // Temporary key assigns to manage viewer bookmarks. 
    if (inputServer->GetButton("alt0"))
    {
        nAppViewport *vp = this->refViewportUI->GetCurrentViewport();
        this->AddBookmark(vp->GetViewerPos(), vp->GetViewerAngles());
        return true;
    }

    int numCamera = -1;
    if      (inputServer->GetButton("alt1")) numCamera = 0;
    else if (inputServer->GetButton("alt2")) numCamera = 1;
    else if (inputServer->GetButton("alt3")) numCamera = 2;
    else if (inputServer->GetButton("alt4")) numCamera = 3;
    else if (inputServer->GetButton("alt5")) numCamera = 4;
    else if (inputServer->GetButton("alt6")) numCamera = 5;
    else if (inputServer->GetButton("alt7")) numCamera = 6;
    else if (inputServer->GetButton("alt8")) numCamera = 7;
    else if (inputServer->GetButton("alt9")) numCamera = 8;

    // Set a previously saved bookmark
    if (numCamera >= 0 && numCamera < this->bookmarks.Size())
    {
        this->SetBookmark(numCamera);
        return true;
    }

    // toggle console
    if (inputServer->GetButton("console"))
    {
        nConServer::Instance()->Toggle();
        return true;
    }

    // Toggle visibility of cursor
    if (inputServer->GetButton("toggle_cursor"))
    {
        if (nGfxServer2::Instance()->GetCursorVisibility() != nGfxServer2::None)
        {
            nGfxServer2::Instance()->SetCursorVisibility(nGfxServer2::None);
        }
        else
        {
            nGfxServer2::Instance()->SetCursorVisibility(nGfxServer2::System);
        }
        return true;
    }

    // screenshot
    if (inputServer->GetButton("screenshot"))
    { 
        this->captureFrame = true;
        return true;
    }

    return false;
}
