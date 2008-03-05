#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  polygontriggerundocmd_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/polygontriggerundocmd.h"
#include "entity/nentityobjectserver.h"
#include "conjurer/ninguitoolprismarea.h"
#include "ntrigger/nctriggershape.h"

//------------------------------------------------------------------------------
/**
    Constructor
    @param prevPonts Array of trigger area points before the operation
    @param newPoints Array of trigger area points after the operation

*/
PolygonTriggerUndoCmd::PolygonTriggerUndoCmd( nEntityObject* polygonTrigger, nArray<vector3>* prevPoints, nArray<vector3>* newPoints )
{
    if ( ! prevPoints || ! newPoints || ! polygonTrigger )
    {
        this->valid = false;
    }

    this->valid = true;

    this->refPolygonTrigger = polygonTrigger;

	this->prevPoints = *prevPoints;
	this->newPoints = *newPoints;

    this->entityWasDirty = nEntityObjectServer::Instance()->GetEntityObjectDirty( this->refPolygonTrigger->GetId() );
}
//------------------------------------------------------------------------------
/**
    Destructor
*/
PolygonTriggerUndoCmd::~PolygonTriggerUndoCmd()
{
    // empty
}
//------------------------------------------------------------------------------
/**
    Execute
    @return Success
*/
bool PolygonTriggerUndoCmd::Execute( void )
{
    if ( ! this->valid )
    {
        return false;
    }

    ncTriggerShape * triggerShape = this->refPolygonTrigger->GetComponentSafe<ncTriggerShape>();
	triggerShape->SetPolygonVertices( this->newPoints );

    nEntityObjectServer::Instance()->SetEntityObjectDirty( this->refPolygonTrigger , true);

	nObjectEditorState* objst = static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") );
    objst->ResetSelection();
	objst->AddEntityToSelection( this->refPolygonTrigger->GetId() );

	return true;
}
//------------------------------------------------------------------------------
/**
    Unexecute
    @return Success
*/
bool PolygonTriggerUndoCmd::Unexecute( void )
{
    if ( ! this->valid )
    {
        return false;
    }

    ncTriggerShape * triggerShape = this->refPolygonTrigger->GetComponentSafe<ncTriggerShape>();
	triggerShape->SetPolygonVertices( this->prevPoints );

    nEntityObjectServer::Instance()->SetEntityObjectDirty( this->refPolygonTrigger , true);

	nObjectEditorState* objst = static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") );
    objst->ResetSelection();
	objst->AddEntityToSelection( this->refPolygonTrigger->GetId() );

	return true;
}
//------------------------------------------------------------------------------
/**
    Get byte size
*/
int PolygonTriggerUndoCmd::GetSize( void )
{
    return sizeof( ( this->prevPoints.Size() + this->newPoints.Size() ) * sizeof( vector3) );
}
//------------------------------------------------------------------------------