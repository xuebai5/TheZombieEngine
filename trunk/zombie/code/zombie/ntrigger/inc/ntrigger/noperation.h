#ifndef N_OPERATION_H
#define N_OPERATION_H

//------------------------------------------------------------------------------
/**
    @class nOperation
    @ingroup NebulaTriggerSystem

    Interface to generically execute some type of operation.
*/

#include "kernel/nroot.h"

//------------------------------------------------------------------------------
struct nOperation : public nRoot
{
    /// Types of operation
    enum OperationType
    {
        COMMAND,
        SCRIPT,
        TRIGGER
    };

    /// Get operation type
    virtual OperationType GetType() const = 0;
    /// Get operation type as string
    virtual const char* GetTypeLabel() const = 0;
    /// Execute the operation
    virtual void Execute( nEntityObject* trigger, nEntityObject* entity ) = 0;
};

//------------------------------------------------------------------------------
#endif // N_OPERATION_H
