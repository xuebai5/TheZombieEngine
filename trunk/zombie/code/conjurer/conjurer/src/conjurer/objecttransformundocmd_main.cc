#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  objecttransformundocmd_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/objecttransformundocmd.h"
#include "entity/nentityobjectserver.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "conjurer/nobjecteditorstate.h"
#include "zombieentity/nctransform.h"
#include "zombieentity/ncsuperentity.h"

//------------------------------------------------------------------------------
/**
    Constructor
    @param objects Array of InguiObjectTransform (not of a derived class) meaning the affected objects and their transforms before the operation

*/
ObjectTransformUndoCmd::ObjectTransformUndoCmd( nArray<InguiObjectTransform>* objects )
{
    if ( !objects )
    {
        this->valid = false;
    }

    this->valid = true;
    this->firstExecution = true;

    this->undoInfo.Reset();
    int n = objects->Size();
    for ( int i=0; i < n; i++)
    {
        ObjTransformUndoInfo& ui = undoInfo.At( i );
        ui.entityId = (*objects)[i].GetEntity()->GetId();
        ui.entityWasDirty = nEntityObjectServer::Instance()->GetEntityObjectDirty( ui.entityId );
        ui.selectedByUser = (*objects)[i].GetContainingIndoor() == 0;
        (*objects)[i].GetInitialMatrix( ui.undoTransform );
        (*objects)[i].GetTransform( ui.redoTransform );
    }
}
//------------------------------------------------------------------------------
/**
    Destructor
*/
ObjectTransformUndoCmd::~ObjectTransformUndoCmd()
{
    // empty
}
//------------------------------------------------------------------------------
/**
    Execute
    @return Success
*/
bool ObjectTransformUndoCmd::Execute( void )
{
    if ( !this->valid )
    {
        return false;
    }

    nEntityObjectServer* eServer = nEntityObjectServer::Instance();

    // Don't execute the command when it's created, because it is executed before creation
    if ( firstExecution )
    {
        firstExecution = false;
        return true;
    }

    // Change selection
    nObjectEditorState* objst = static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") );
    objst->ResetSelection();

    // Loop over all affected entities
    for (int e = 0; e < this->undoInfo.Size(); e++)
    {
        // Find entity with stored id -- temporal
        nEntityObjectId entId = this->undoInfo[e].entityId;
        nEntityObject* entity = eServer->GetEntityObject( entId );
        n_assert( entity );

        ncTransform *ct = entity->GetComponent<ncTransform>();
        n_assert( ct );
        ct->SetPosition( this->undoInfo[e].redoTransform.gettranslation() );
        ct->SetQuat( this->undoInfo[e].redoTransform.getquatrotation() );
        ct->SetScale( this->undoInfo[e].redoTransform.getscale() );

        // update subentities from this one
        ncSuperentity* supEnt = entity->GetComponent<ncSuperentity>();
        if ( supEnt )
        {
            supEnt->UpdateSubentities();
        }

        nEntityObjectServer::Instance()->SetEntityObjectDirty( entity , true);

        // Add to selection if it was selected by user
        if ( this->undoInfo[e].selectedByUser )
        {
            objst->AddEntityToSelection( entity->GetId() );
        }
    }

    // Refresh selection object transforms
    objst->AssignToolObjectTransform();

    return true;
}
//------------------------------------------------------------------------------
/**
    Unexecute
    @return Success
*/
bool ObjectTransformUndoCmd::Unexecute( void )
{
    if ( !this->valid )
    {
        return false;
    }

    nEntityObjectServer* eServer = nEntityObjectServer::Instance();

    // Change selection
    nObjectEditorState* objst = static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") );
    objst->ResetSelection();

    // Loop over all affected entities
    for (int e = 0; e < this->undoInfo.Size(); e++)
    {
        // Find entity with stored id -- temporal
        nEntityObjectId entId = this->undoInfo[e].entityId;
        nEntityObject* entity = eServer->GetEntityObject( entId );  // @fixme: this worked before
        n_assert( entity );

        ncTransform *ct = entity->GetComponent<ncTransform>();
        n_assert( ct );
        ct->SetPosition( this->undoInfo[e].undoTransform.gettranslation() );
        ct->SetQuat( this->undoInfo[e].undoTransform.getquatrotation() );
        ct->SetScale( this->undoInfo[e].undoTransform.getscale() );

        // Update subentities
        ncSuperentity* supEnt = entity->GetComponent<ncSuperentity>();
        if ( supEnt )
        {
            supEnt->UpdateSubentities();
        }
        
        if ( ! this->undoInfo[e].entityWasDirty )
        {
            nEntityObjectServer::Instance()->SetEntityObjectDirty( entity , false);
        }
        
        // Add to selection if it was selected by user
        if ( this->undoInfo[e].selectedByUser )
        {
            objst->AddEntityToSelection( entity->GetId() );
        }
    }

    // Refresh selection object transforms
    objst->AssignToolObjectTransform();

    return true;
}
//------------------------------------------------------------------------------
/**
    Get byte size
*/
int ObjectTransformUndoCmd::GetSize( void )
{
    return sizeof( this );
}
//------------------------------------------------------------------------------