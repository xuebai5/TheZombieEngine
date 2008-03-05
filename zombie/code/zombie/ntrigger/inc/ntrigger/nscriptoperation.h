#ifndef N_SCRIPTOPERATION_H
#define N_SCRIPTOPERATION_H

//------------------------------------------------------------------------------
/**
    @class nScriptOperation
    @ingroup NebulaTriggerSystem

    Class that executes some script code.

    To define the script code inherit a script class from this one and implement
    the ExecuteOutput method.

    @see nScriptClassServer
*/

#include "ntrigger/noperation.h"

//------------------------------------------------------------------------------
class nScriptOperation : public nOperation
{
public:
    /// Get operation type
    OperationType GetType() const;
    /// Get operation type as string
//    const char* GetTypeLabel() const;
    /// Execute the operation
    void Execute( nEntityObject* trigger, nEntityObject* entity );

    const char* GetTypeLabel () const;

};

//------------------------------------------------------------------------------
#endif // N_SCRIPTOPERATION_H
