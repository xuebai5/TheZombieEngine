//-----------------------------------------------------------------------------
//  nscriptoperation_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/nscriptoperation.h"

nNebulaScriptClass(nScriptOperation, "nroot");

//-----------------------------------------------------------------------------
namespace
{
    const char* OperationTypeLabel = "Script";
}

//-----------------------------------------------------------------------------
/**
    Get operation type
*/
nOperation::OperationType
nScriptOperation::GetType() const
{
    return nOperation::SCRIPT;
}

//-----------------------------------------------------------------------------
/**
    Get operation type as string
*/
const char*
nScriptOperation::GetTypeLabel() const
{
    return OperationTypeLabel;
}

//-----------------------------------------------------------------------------
/**
    Execute the operation

    Call the scripted method ExecuteOutput
*/
void
nScriptOperation::Execute( nEntityObject* trigger, nEntityObject* entity )
{
    // Set command's input parameters
    nArg inArgs[2];
    inArgs[0].SetO( trigger );
    inArgs[1].SetO( entity );

    // Execute command
    this->CallArgs( "ExecuteOutput", 0, NULL, 2, inArgs );
}
