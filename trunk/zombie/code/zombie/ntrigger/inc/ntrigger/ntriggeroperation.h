#ifndef N_TRIGGEROPERATION_H
#define N_TRIGGEROPERATION_H

//------------------------------------------------------------------------------
/**
    @class nTriggerOperation
    @ingroup NebulaTriggerSystem

    Class that notifies a target trigger about another trigger being activated.

    The target trigger is notified by a call to HandleEvents with the special
    event 'trigger activation' and the activated trigger as the event source.

    (C) 2006 Conjurer Services, S.A.
*/

#include "ntrigger/noperation.h"

//------------------------------------------------------------------------------
class nTriggerOperation : public nOperation
{
public:
    /// Constructor
    nTriggerOperation( nEntityObjectId targetTriggerId );
    /// Get operation type
    OperationType GetType() const;
    /// Get operation type as string
    const char* GetTypeLabel() const;
    /// Execute the operation
    void Execute( nEntityObject* trigger, nEntityObject* entity );
    /// Return the trigger id to be notified on operation execution
    nEntityObjectId GetTargetTriggerId() const;

private:
    /// Trigger id of the trigger to be notified about another trigger activation
    nEntityObjectId targetTriggerId;
};

//------------------------------------------------------------------------------
#endif // N_TRIGGEROPERATION_H
