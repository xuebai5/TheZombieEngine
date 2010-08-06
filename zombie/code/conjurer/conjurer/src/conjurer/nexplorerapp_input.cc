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
#include "nundo/nundoserver.h"
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

    // Undo
    if (inputServer->GetButton("undo"))
    {
        nUndoServer::Instance()->Undo();
        return true;
    }

    // Redo
    if (inputServer->GetButton("redo"))
    {
        nUndoServer::Instance()->Redo();
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

//------------------------------------------------------------------------------
/**
*/
void 
nExplorerApp::MoveCameraToEntity(nEntityObjectId entityId)
{
    nAppViewport *curViewport = this->refViewportUI->GetCurrentViewport();
    nEntityObject *entity = nEntityObjectServer::Instance()->GetEntityObject(entityId);
    if (entity)
    {
        ncTransform *transform = entity->GetComponent<ncTransform>();
        if (transform)
        {
            vector3 position = transform->GetPosition();
            curViewport->SetViewerPos(position);
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Move camera so selection is visible
*/
void 
nExplorerApp::MoveCameraToSelection()
{
    nAppViewport* curViewport = this->refViewportUI->GetCurrentViewport();

    nObjectEditorState* objState = static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") );
    float radius;
    vector3 center = objState->GetSelectionCenterPoint( radius );

    if ( curViewport->GetCamera().GetType() == nCamera2::Perspective )
    {
        center += curViewport->GetViewMatrix().z_component() * radius * 3;
    }
    else
    {
        // In orthogonal camera, the camera position is equal to the selection position.
        //@todo: adjust viewport size to radius

    }
    curViewport->SetViewerPos( center );
}

//------------------------------------------------------------------------------
/**
    @brief Open preview viewport for the specified class
*/
void 
nExplorerApp::OpenPreview(const char* className)
{
    nAppState* curState = this->FindState(this->GetCurrentState());
    if (curState->IsA("neditorstate"))
    {
        n_assert(this->refPreview.isvalid());
        //HACK close it before, so that it is guaranteed to be on top when shown
        this->refPreview->SetVisible(false); 
        this->refPreview->SetClassName(className);
        this->refPreview->SetVisible(true);
        this->refPreview->Reset();//reset camera position
    }
}

//------------------------------------------------------------------------------
/**
    @brief Open preview viewport for the specified class
*/
void 
nExplorerApp::ClosePreview()
{
    if (this->refPreview->GetVisible())
    {
        this->refPreview->SetVisible(false);
    }
}
