#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nInguiToolSphereArea.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/ninguitoolspherearea.h"
#include "kernel/nkernelserver.h"
#include "nscene/nsurfacenode.h"
#include "ngeomipmap/ncterraingmmclass.h"
#include "nphysics/ncphyterrain.h"
#include "nphysics/ncphyterraincell.h"
#include "conjurer/nconjurerapp.h"
#include "conjurer/nobjecteditorstate.h"
#include "ndebug/nceditor.h"
#include "ntrigger/nctriggershape.h"
#include "zombieentity/nctransform.h"
#include "nworldinterface/nworldinterface.h"
#include "zombieentity/ncsubentity.h"
#include "zombieentity/ncsuperentity.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nInguiToolSphereArea, "ninguitool");

//------------------------------------------------------------------------------
/**
*/
nInguiToolSphereArea::nInguiToolSphereArea()
{
    label = "Place spherical area trigger";

    this->triggerClassName = "neareatrigger";
}
//------------------------------------------------------------------------------
/**
*/
nInguiToolSphereArea::~nInguiToolSphereArea()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nInguiToolSphereArea::Apply( nTime /*dt*/ )
{
    bool isSubentity = this->refObjState->GetSelectionMode() == nObjectEditorState::ModeSubentity;

    if ( this->state == Finishing && ! this->isSticky )
    {   
        this->refObjState->SelectToolAndSignalChange( nObjectEditorState::ToolSelection );
        return false;
    }

    if ( this->state <= Inactive )
    {
        // Create trigger
        if ( isSubentity )
        {
            // Subentity
            nEntityObject* brush = this->refObjState->GetSelectionModeEntity();
            n_assert( brush );
            this->sphereTrigger = nWorldInterface::Instance()->NewLocalEntity( this->triggerClassName.Get(), this->firstPos, isSubentity, brush );
        }
        else
        {
            // Normal entity
            this->sphereTrigger = nWorldInterface::Instance()->NewEntity( this->triggerClassName.Get(), this->firstPos );
        }
        if ( ! this->sphereTrigger.isvalid() )
		{
			// Couldn't create the entity
			return false;
		}
        this->entityInstance = this->sphereTrigger;
    }

    n_assert( this->sphereTrigger.isvalid() );

    // Set sphere radius
    ncTriggerShape* ts = this->sphereTrigger.get()->GetComponent<ncTriggerShape>();
    n_assert( ts );
    vector3 r = this->lastPos - this->firstPos;
    ts->SetCircle( r.len() );

    this->refObjState->ResetSelection();
    this->refObjState->AddEntityToSelection( this->sphereTrigger->GetId() );

    if ( this->state <= Inactive )
    {
        // Signal GUI that some entity could have been created or deleted
        this->refObjState->SetEntityPlaced( this->sphereTrigger );
    }

    return true;
}
