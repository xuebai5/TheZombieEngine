//-----------------------------------------------------------------------------
//  ntriggeroperation_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ntriggeroperation.h"
#include "ntrigger/ngameevent.h"
#include "ntrigger/nctrigger.h"
#include "kernel/nlogclass.h"

//-----------------------------------------------------------------------------
namespace
{
    const char* OperationTypeLabel = "Trigger";
}

//-----------------------------------------------------------------------------
/**
    Constructor
*/
nTriggerOperation::nTriggerOperation( nEntityObjectId targetTriggerId ) :
    targetTriggerId( targetTriggerId )
{
}

//-----------------------------------------------------------------------------
/**
    Get operation type
*/
nOperation::OperationType
nTriggerOperation::GetType() const
{
    return nTriggerOperation::TRIGGER;
}

//-----------------------------------------------------------------------------
/**
    Get operation type as string
*/
const char*
nTriggerOperation::GetTypeLabel() const
{
    return OperationTypeLabel;
}

//-----------------------------------------------------------------------------
/**
    Execute the operation

    Notify the target trigger about the given trigger being activated
*/
void
nTriggerOperation::Execute( nEntityObject* trigger, nEntityObject* /*entity*/ )
{
    // Build notification event
    nGameEvent* event = n_new( nGameEvent );
    event->SetType( nGameEvent::TRIGGER_ACTIVATION );
    event->SetSourceEntity( trigger->GetId() );
    event->SetEmitterEntity( trigger->GetId() );

    // Notify target trigger
    nEntityObject* targetTrigger = nEntityObjectServer::Instance()->GetEntityObject( this->targetTriggerId );
    NLOGCOND(trigger, !targetTrigger, (NLOGUSER | 0, "nTriggerOperation::Execute: entity not found 0x%x (%d)", this->targetTriggerId, this->targetTriggerId));
    if (targetTrigger)
    {
        targetTrigger->GetComponentSafe<ncTrigger>()->HandleEvent( event );
    }

    // Destroy notification event
    n_delete( event );
}

//-----------------------------------------------------------------------------
/**
    Return the trigger id to be notified on operation execution
*/
nEntityObjectId
nTriggerOperation::GetTargetTriggerId() const
{
    return this->targetTriggerId;
}
