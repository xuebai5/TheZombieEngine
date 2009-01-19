#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  objectplacingundocmd_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "conjurer/objectplacingundocmd.h"
#include "entity/nentityobjectserver.h"
#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"
#include "nphysics/ncphysicsobj.h"
#include "conjurer/nobjecteditorstate.h"
#include "ndebug/ndebugcomponentserver.h"
#include "animcomp/ncharacterserver.h"
#include "ndebug/nceditor.h"
#include "zombieentity/ncsuperentity.h"
#include "nspatial/ncspatial.h"
#include "zombieentity/nctransform.h"
#include "nspatial/ncspatial.h"
#include "nspatial/ncspatialindoor.h"

//------------------------------------------------------------------------------
/**
    Constructor
    @param object Affected entity
    @param label Label of the command
    @param isDelete Flag: is a delete command?
*/
ObjectPlacingUndoCmd::ObjectPlacingUndoCmd( nEntityObject* object, const char * label, bool isDelete)
{
    if ( !object )
    {
        this->valid = false;
        return;
    }
    
    this->valid = true;
    this->firstExecution = true;
    this->isDeleteCmd = isDelete;

    this->AddEntity( object );

    this->label = label;
}
//------------------------------------------------------------------------------
/**
    Constructor
    @param objects Affected entities array
    @param label Label of the command
    @param isDelete Flag: is a delete command?
*/
ObjectPlacingUndoCmd::ObjectPlacingUndoCmd( nArray<nEntityObject*>* objects, const char * label, bool isDelete)
{
    if ( objects->Size() <= 0 )
    {
        this->valid = false;
        return;
    }
    this->valid = true;
    this->firstExecution = true;
    this->isDeleteCmd = isDelete;

    for ( int i = 0; i < objects->Size(); i++ )
    {
        nEntityObject* obj = (*objects)[i];
        this->AddEntity( obj );
    }
    this->label = label;
}

//------------------------------------------------------------------------------
/**
    Constructor
    @param objects Affected entities array
    @param label Label of the command
    @param isDelete Flag: is a delete command?
*/
ObjectPlacingUndoCmd::ObjectPlacingUndoCmd( nArray<nRefEntityObject>* objects, const char * label, bool isDelete)
{
    if ( objects->Size() <= 0 )
    {
        this->valid = false;
        return;
    }
    this->valid = true;
    this->firstExecution = true;
    this->isDeleteCmd = isDelete;

    for ( int i = 0; i < objects->Size(); i++ )
    {
        nEntityObject* obj = (*objects)[i];
        this->AddEntity( obj );
    }
    this->label = label;
}
//------------------------------------------------------------------------------
/**
    Constructor
    @param objects Affected entities
    @param label Label of the command
    @param isDelete Flag: is a delete command?

    This command is created with a pointer to an id array of the created entities

*/
ObjectPlacingUndoCmd::ObjectPlacingUndoCmd( nArray<nEntityObjectId>* objects, const char * label, bool isDelete )
{
    if ( objects->Size() <= 0 )
    {
        this->valid = false;
        return;
    }
    this->valid = true;
    this->firstExecution = true;
    this->isDeleteCmd = isDelete;

    nEntityObjectServer* entServer = nEntityObjectServer::Instance();
    for (int i= 0; i < objects->Size(); i++)
    {
        nEntityObjectId id = (*objects)[i];
        nEntityObject* ent = entServer->GetEntityObject( id );
        this->AddEntity( ent );
    }
    this->label = label;
}
//------------------------------------------------------------------------------
/**
    Destructor
*/
ObjectPlacingUndoCmd::~ObjectPlacingUndoCmd()
{
    for (int i=0;i < undoInfo.Size(); i++)
    {
        // Decrement reference count. If it is equal to one (2 before calling Release), then the entity
        // is referenced only in the entity server, so it must be deleted if the command is in the redo side
        // (it was executed but later was undone)
        ObjPlaceUndoInfo& info = this->undoInfo.At( i );
        if ( info.refEntity.isvalid() )
        {
            int n = info.refEntity->GetRefCount();
            if ( n == 1 && ( ( this->undone && ! this->isDeleteCmd ) || ( ! this->undone && this->isDeleteCmd ) ) )
            {
                nEntityObjectServer::Instance()->RemoveEntityObject( info.refEntity.get() );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ObjectPlacingUndoCmd::AddEntity(nEntityObject* object)
{
    n_assert( object );

    ObjPlaceUndoInfo& info = this->undoInfo.At( this->undoInfo.Size() );
    info.className = object->GetEntityClass()->GetProperName();
    info.entityId = object->GetId();

    info.refEntity = object;
    object->AddRef();

    ncTransform *ct = object->GetComponent<ncTransform>();
    n_assert( ct );
    info.placedInstanceTransform.settranslation( ct->GetPosition() );
    info.placedInstanceTransform.setquatrotation( ct->GetQuat() );
    info.placedInstanceTransform.setscale( ct->GetScale() );

    // add also the facade if the entity is a indoor
    ncSpatialIndoor* spatialIndoorComp = object->GetComponent<ncSpatialIndoor>();
    if (spatialIndoorComp )
    {
        nEntityObject* facade = spatialIndoorComp->GetFacade();
        if ( facade )
        {
            this->AddEntity( facade );
        }
    }
}

//------------------------------------------------------------------------------
/**
    Execute
    @return Success
*/
bool ObjectPlacingUndoCmd::Execute( void )
{
    if ( !this->valid )
    {
        return false;
    }

    // Don't execute the command when it's created, because it is executed before creation
    if ( firstExecution )
    {
        this->firstExecution = false;
        return true;
    }

    if ( this->isDeleteCmd )
    {
        this->RemoveObjects();
    }
    else
    {
        this->InsertObjects();
    }

    return true;
}
//------------------------------------------------------------------------------
/**
    Unexecute
    @return Success
*/
bool ObjectPlacingUndoCmd::Unexecute( void )
{
    if ( !this->valid )
    {
        return false;
    }

    if ( this->isDeleteCmd )
    {
        this->InsertObjects();
    }
    else
    {
        this->RemoveObjects();
    }

    this->firstExecution = false;

    return true;
}
//------------------------------------------------------------------------------

/**
*/
void
ObjectPlacingUndoCmd::InsertObjects( void )
{
    // Change selection
    nObjectEditorState* objst = static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") );
    objst->ResetSelection();

    // Loop through objects to insert
    for (int i=0;i< this->undoInfo.Size(); i++)
    {
        n_assert( this->undoInfo[i].refEntity.isvalid() );
        nEntityObject* object = this->undoInfo[i].refEntity;

        // Set initial transform (also inserts into space automatically)
        ncTransform *ct = object->GetComponent<ncTransform>();
        n_assert( ct );
        ct->SetPosition( this->undoInfo[i].placedInstanceTransform.gettranslation() );
        ct->SetQuat( this->undoInfo[i].placedInstanceTransform.getquatrotation() );
        ct->SetScale( this->undoInfo[i].placedInstanceTransform.getscale() );

        // Make object enter in the level again
        object->ReturnFromLimbo( object );

        // Add editor component to debug server. This could be done with binding to the limbo signals, but this way we save at least some bindings
        ncEditor* edComp = object->GetComponent<ncEditor>();
        if ( edComp )
        {
            nDebugComponentServer::Instance()->Add( edComp );

            // Set deletion flag
            edComp->SetDeleted( false );
        }

        // Add to selection
        objst->AddEntityToSelection( object->GetId() );
    }

    // Refresh selection object transforms
    objst->AssignToolObjectTransform();

    // Signal GUI that some entity could have been created or deleted
    nUndoServer::Instance()->SignalEntityModified( nUndoServer::Instance() );
}

//------------------------------------------------------------------------------

/**
*/
void
ObjectPlacingUndoCmd::RemoveObjects( void )
{

    // Clear selection
    nObjectEditorState* objst = static_cast<nObjectEditorState*>( nKernelServer::Instance()->Lookup("/app/conjurer/appstates/object") );
    objst->ResetSelection();

    // Loop through objects to delete. This is done in the reverse order of InsertObjects so it acts like a stack
    for (int i = this->undoInfo.Size() - 1; i >= 0; i--)
    {
        n_assert( this->undoInfo[i].refEntity.isvalid() );
        nEntityObject* object = this->undoInfo[i].refEntity;

        // Update editor component. This could be done with binding to the limbo signals, but this way we save at least some bindings
        ncEditor* edComp = object->GetComponent<ncEditor>();
        if ( edComp )
        {
            // Remove editor component from debug server
            nDebugComponentServer::Instance()->Remove( edComp );

            // Set deletion flag
            edComp->SetDeleted( true );
        }

        // Make object to don't belong to the level
        object->SendToLimbo( object );
    }

    // Refresh selection object transforms
    objst->AssignToolObjectTransform();

    // Signal GUI that some entity could have been created or deleted
    nUndoServer::Instance()->SignalEntityModified( nUndoServer::Instance() );

}

//------------------------------------------------------------------------------
/**
    Get byte size
*/
int ObjectPlacingUndoCmd::GetSize( void )
{
    return sizeof( this );
}
//------------------------------------------------------------------------------
/**
    Remove references to external objects
*/
void 
ObjectPlacingUndoCmd::RemoveReferences()
{
    for (int i=0;i < undoInfo.Size(); i++)
    {
        ObjPlaceUndoInfo& info = this->undoInfo.At( i );
        info.refEntity->Release();
    }
}

//------------------------------------------------------------------------------