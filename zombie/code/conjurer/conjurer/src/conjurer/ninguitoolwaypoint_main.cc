#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolwaypoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitoolwaypoint.h"
#include "conjurer/ninguitoolplacer.h"
#include "kernel/nkernelserver.h"
#include "napplication/napplication.h"
#include "conjurer/nassetloadstate.h"
#include "ndebug/nceditor.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "conjurer/nobjecteditorstate.h"
#include "ndebug/nceditorclass.h"
#include "entity/nobjectinstancer.h"
#include "ncrnswaypoint/ncrnswaypoint.h"
#include "nphysics/ncphysicsobj.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/nobjecteditorstate.h"
#include "zombieentity/ncsubentity.h"
#include "zombieentity/ncsuperentity.h"
#include "zombieentity/nctransform.h"
#include "nlayermanager/nlayermanager.h"
#include "input/ninputserver.h"

//------------------------------------------------------------------------------

nNebulaScriptClass(nInguiToolWaypoint, "ninguitool");
//------------------------------------------------------------------------------

/**
*/
nInguiToolWaypoint::nInguiToolWaypoint():
    manualLinking( false )
{
    nInguiTool::label = "Create Renaissance waypoint";
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolWaypoint::~nInguiToolWaypoint()
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
void
nInguiToolWaypoint::Draw( nAppViewport* /*vp*/, nCamera2* /*camera*/ )
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
bool
nInguiToolWaypoint::Apply( nTime /*dt*/ )
{
    // In manual linking mode
    if ( nInguiTool::state <= nInguiTool::Inactive && this->manualLinking )
    {
        // If picked entity is a waypoint,
        if ( this->refLastPickedEntity.isvalid() )
        {
            ncRnsWaypoint* wpComp = this->refLastPickedEntity->GetComponent<ncRnsWaypoint>();
            ncSubentity* subEntComp = this->refLastPickedEntity->GetComponent<ncSubentity>();
            if ( wpComp )
            {
                // Link all the selection to it
                int n = this->refObjState->GetSelectionCount();
                for (int i=0; i<n; i++)
                {
                    nEntityObject* selEnt = this->refObjState->GetSelectedEntity( i );
                    n_assert( selEnt );
                    ncRnsWaypoint* wpComp2 = selEnt->GetComponent<ncRnsWaypoint>();
                    ncSubentity* subEntComp2 = this->refLastPickedEntity->GetComponent<ncSubentity>();

                    // If the selected object is a free waypoint ot is from the same superentity, link it
                    if ( wpComp2 && ( ! subEntComp2->GetSuperentity() || subEntComp2->GetSuperentity() == subEntComp->GetSuperentity() ) )
                    {
                        // Bidirectional link by default
                        wpComp2->AddLocalLink( wpComp );
                        wpComp->AddLocalLink( wpComp2 );
                    }
                }

                return true;
            }
        }
        return false;
    }

    bool didSetPosition = false;

    bool localWaypoint = this->refObjState->GetSelectionMode() == nObjectEditorState::ModeSubentity;

    if ( nInguiTool::state <= nInguiTool::Inactive )
    {
        // If picked on a waypoint, set the selection to it so next created waypoint will link to it
        if ( this->refLastPickedEntity.isvalid() )
        {
            ncRnsWaypoint* wpComp = this->refLastPickedEntity->GetComponent<ncRnsWaypoint>();
            if ( wpComp )
            {
                n_assert( this->refObjState );
                refObjState->ResetSelection();
                refObjState->AddEntityToSelection( this->refLastPickedEntity->GetId() );
                
                this->SetEntityInstance( 0 );
                return true;
            }
        }

        // Instatiate the waypoint
        nEntityObject* wp = 0;
        
        if ( localWaypoint )
        {
            wp = nEntityObjectServer::Instance()->NewLocalEntityObject( "nernswaypoint" );
        }
        else
        {
            wp = nEntityObjectServer::Instance()->NewEntityObject( "nernswaypoint" );
        }
        n_assert( wp );
        this->SetEntityInstance( wp );

        // Set waypoint position
        ncTransform* trComp = wp->GetComponent<ncTransform>();
        if ( trComp )
        {
            trComp->SetPosition( nInguiTool::lastPos );
            didSetPosition = true;
        }

        // If the waypoint is of type B or C, link the brush to it
        if ( localWaypoint )
        {
            nEntityObject* brush = refObjState->GetSelectionModeEntity();
            n_assert( brush );

            ncSuperentity* superEntComp = brush->GetComponent<ncSuperentity>();
            n_assert( superEntComp );
            superEntComp->AddSubentity( wp );
        }

        // Set current layer to the object
        nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
        n_assert( level );
        nLayerManager* layerManager = static_cast<nLayerManager*>( level->GetEntityLayerManager() );
        n_assert( layerManager );
        ncEditor* editorComp = wp->GetComponent<ncEditor>();
        if ( editorComp )
        {
            editorComp->SetLayerId( layerManager->GetSelectedLayerId() );
        }

        level->AddEntity( entityInstance );

        // Disable the picking of the waypoint
        ncPhysicsObj* physicsComp = wp->GetComponent<ncPhysicsObj>();
        if ( physicsComp )
        {
            physicsComp->Disable();
        }

        // Link all waypoints in selection to the new waypoint
        int n = this->refObjState->GetSelectionCount();
        for (int i=0; i<n; i++)
        {
            nEntityObject* selEnt = this->refObjState->GetSelectedEntity( i );
            n_assert( selEnt );
            ncRnsWaypoint* wpComp = selEnt->GetComponent<ncRnsWaypoint>();
            ncSubentity* subEntComp = selEnt->GetComponent<ncSubentity>();
            ncSubentity* instanceSubEntComp = wp->GetComponent<ncSubentity>();
            n_assert( instanceSubEntComp );

            if ( wpComp && ( ! subEntComp->GetSuperentity() || subEntComp->GetSuperentity() == instanceSubEntComp->GetSuperentity() ) )
            {
                // Bidirectional link by default
                ncNavNode* wpComp2( wp->GetComponentSafe<ncNavNode>() );
                wpComp->AddLocalLink( wpComp2 );
                wpComp2->AddLocalLink( wpComp );
            }
        }
    }

    if ( this->GetEntityInstance() )
    {
        // If picked entity is not the created one, or it was placed in the air (and not manually linking), update entity position
        if ( !didSetPosition && !this->manualLinking && ( this->refLastPickedEntity == 0 || this->GetEntityInstance() != this->refLastPickedEntity  ) )
        {
            // Set entity position and continue moving waypoint
            n_assert( this->GetEntityInstance() );
            ncTransform* trComp = this->GetEntityInstance()->GetComponent<ncTransform>();
            if ( trComp )
            {
                trComp->SetPosition( this->nInguiTool::lastPos );
            }
        }
    }

    if ( this->state == nInguiTool::Finishing )
    {
        if ( this->GetEntityInstance() )
        {   
            if ( localWaypoint )
            {
                // Temporary: save the subentities file here
                nEntityObject* brush = refObjState->GetSelectionModeEntity();
                n_assert( brush );
                ncSuperentity* superComp = brush->GetComponent<ncSuperentity>();
                n_assert( superComp );
                superComp->SaveSubentities();
            }

            // Set selection to the last created
            this->refObjState->ResetSelection();
            this->refObjState->AddEntityToSelection( this->GetEntityInstance()->GetId() );
            
            // Enable the picking of the waypoint
            ncPhysicsObj* physicsComp = this->GetEntityInstance()->GetComponent<ncPhysicsObj>();
            if ( physicsComp )
            {
                physicsComp->Enable();
            }

            // Bind the waypoint to any underlying navmesh
            this->GetEntityInstance()->GetComponentSafe<ncRnsWaypoint>()->GenerateExternalLinks();
        }

        this->refLastPickedEntity = 0;
    }

    // Signal GUI that some entity could have been created or deleted
    nObjectEditorState* objState = static_cast<nObjectEditorState*>( nConjurerApp::Instance()->FindState( "object" ) );
    n_assert( objState );
    objState->SignalEntityModified( objState );

    return true;
}
//------------------------------------------------------------------------------
/**
    @brief Handle input in a viewport.
*/
bool
nInguiToolWaypoint::HandleInput( nAppViewport* /*vp*/ )
{
    this->manualLinking = false;
    if ( nInputServer::Instance()->GetButton("buton0_ctrl") || nInputServer::Instance()->GetButton("buton0_down_ctrl") )
    {
        this->manualLinking = true;
        return true;
    }

    return false;
}
//------------------------------------------------------------------------------
