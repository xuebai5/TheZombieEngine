#include "precompiled/pchrenaissanceapp.h"
//------------------------------------------------------------------------------
//  nrnsapp_input.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nrnsapp/nrnsapp.h"
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"
#include "input/ninputserver.h"
//#include "nundo/nundoserver.h"
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
nRnsApp::HandleInput()
{
    nInputServer* inputServer = nInputServer::Instance();

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

    /*
    // show info
    static bool showInfo = false;
    if (inputServer->GetButton("info"))
    {
        showInfo = !showInfo;
        if (showInfo)
        {
            nConServer::Instance()->Watch("viewer*");
        } 
        else if (this->GetWatch())
        {
            nConServer::Instance()->Watch(this->GetWatch());
        } 
        else
        {
            nConServer::Instance()->Watch("");
        }
    }
    */

    return false;
}

//------------------------------------------------------------------------------
/**
*/
/*
void 
nRnsApp::MoveCameraToEntity(nEntityObjectId entityId)
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
*/


//------------------------------------------------------------------------------
/**
    @brief Open preview viewport for the specified class
*/
/*
void 
nRnsApp::OpenPreview(const char* className)
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
*/

//------------------------------------------------------------------------------
/**
    @brief Open preview viewport for the specified class
*/
/*
void 
nRnsApp::ClosePreview()
{
    if (this->refPreview->GetVisible())
    {
        this->refPreview->SetVisible(false);
    }
}
*/