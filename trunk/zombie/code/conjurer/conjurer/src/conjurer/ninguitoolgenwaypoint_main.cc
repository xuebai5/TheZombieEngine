#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  ninguitoolgenwaypoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitoolgenwaypoint.h"
#include "kernel/nkernelserver.h"
#include "napplication/napplication.h"
#include "conjurer/nassetloadstate.h"
#include "ndebug/nceditor.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "conjurer/nobjecteditorstate.h"
#include "ndebug/nceditorclass.h"
#include "entity/nobjectinstancer.h"
#include "ncwaypoint/ncwaypoint.h"
#include "ncwaypointpath/ncwaypointpath.h"
#include "waypoint/waypoint.h"
#include "nwaypointserver/nwaypointserver.h"
#include "nphysics/ncphysicsobj.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/nobjecteditorstate.h"
#include "zombieentity/nctransform.h"
#include "nlayermanager/nlayermanager.h"
#include "input/ninputserver.h"

//------------------------------------------------------------------------------

nNebulaScriptClass(nInguiToolGenWaypoint, "ninguitoolplacer");
//------------------------------------------------------------------------------

/**
*/
nInguiToolGenWaypoint::nInguiToolGenWaypoint()
{
    nInguiTool::label = "Create waypoint";
    
    this->pathName = "Path";
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolGenWaypoint::~nInguiToolGenWaypoint()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nInguiToolGenWaypoint::Apply( nTime /*dt*/ )
{
    bool didSetPosition = false;

    nWayPointServer* wpServer = nWayPointServer::Instance();
    n_assert( wpServer );

    if ( nInguiTool::state <= nInguiTool::Inactive )
    {
        // If picked on a waypoint, set the selection to it so next created waypoint will link to it
        if ( this->refLastPickedEntity.isvalid() )
        {
            ncWayPoint* wpComp = this->refLastPickedEntity->GetComponent<ncWayPoint>();
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
        nWayPointServer::waypointid waypId = wpServer->CreateNewWayPoint( nInguiTool::lastPos );
        WayPoint* waypoint = wpServer->GetWayPoint( waypId );
        ncWayPoint* wpComp = waypoint->GetComponent();
        n_assert( wpComp );
        nEntityObject* wpEntity = wpComp->GetEntityObject();
        n_assert( wpEntity );
        this->SetEntityInstance( wpEntity );

        // Set current layer to the waypoint
        nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
        n_assert( level );
        nLayerManager* layerManager = static_cast<nLayerManager*>( level->GetEntityLayerManager() );
        n_assert( layerManager );
        ncEditor* editorComp = wpEntity->GetComponent<ncEditor>();
        if ( editorComp )
        {
            editorComp->SetLayerId( layerManager->GetSelectedLayerId() );
        }

        level->AddEntity( entityInstance );

        // Disable the picking of the waypoint
        ncPhysicsObj* physicsComp = wpEntity->GetComponent<ncPhysicsObj>();
        if ( physicsComp )
        {
            physicsComp->Disable();
        }

        // Link first waypoint in selection to the new waypoint. Else add it to a new path
        bool makeNewPath = true;
        int n = this->refObjState->GetSelectionCount();
        if ( n > 0 )
        {
            nEntityObject* selEnt = this->refObjState->GetSelectedEntity( 0 );
            n_assert( selEnt );
            ncWayPoint* wpComp = selEnt->GetComponent<ncWayPoint>();
            if ( wpComp )
            {
                makeNewPath = false;
                WayPoint* prevWaypoint = wpServer->GetWayPoint( wpComp->GetWayPointId() );
                int pathId = prevWaypoint->GetPath();
                ncWayPointPath* path( wpServer->CheckPathExists( pathId ) );
                n_assert( path );
                const nString& pathName( path->GetName() );
                if ( pathName.Length() )
                {
                    wpServer->InsertWPAtTheBackOfPath( waypoint->GetId(), pathName );
                }
            }
        }
        if ( makeNewPath )
        {
            // Check if path already exists. if so dont create it
            if ( !wpServer->CheckPathExists( this->pathName ) )
            {
                wpServer->CreateNewPath( this->pathName );
            }
            // Insert new waypoint in path
            wpServer->InsertWPAtTheBackOfPath( waypoint->GetId(), this->pathName );
        }
    }

    if ( this->GetEntityInstance() )
    {
        // If picked entity is not the created one, or it was placed in the air, update entity position
        if ( !didSetPosition && ( this->refLastPickedEntity == 0 || this->GetEntityInstance() != this->refLastPickedEntity  ) )
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
            // Set selection to the last created
            this->refObjState->ResetSelection();
            this->refObjState->AddEntityToSelection( this->GetEntityInstance()->GetId() );
            
            // Enable the picking of the waypoint
            ncPhysicsObj* physicsComp = this->GetEntityInstance()->GetComponent<ncPhysicsObj>();
            if ( physicsComp )
            {
                physicsComp->Enable();
            }
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
    @brief Get name of path to insert waypoints into
*/
const char*
nInguiToolGenWaypoint::GetPathName()
{
    return this->pathName.Get();
}

//------------------------------------------------------------------------------
/**
    @brief Set name of path to insert waypoints into
    @param path The path name
*/
void
nInguiToolGenWaypoint::SetPathName(const char* path)
{
    this->pathName = path;
}

//------------------------------------------------------------------------------
