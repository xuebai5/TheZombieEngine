//-----------------------------------------------------------------------------
//  nctriggeroutput_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/nctriggeroutput.h"
#include "ntrigger/ntriggerserver.h"
#include "ntrigger/nscriptoperation.h"
#include "ntrigger/ncareatrigger.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncTriggerOutput,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTriggerOutput)
    NSCRIPT_ADDCMD_COMPOBJECT('EDOS', void, DeleteAllOutputSets , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESSC', void, SetOutputSetConfig, 4, (const char*, nTime, nTime, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGSC', void, GetOutputSetConfig, 1, (const char *), 3, (nTime&, nTime&, int&));
    NSCRIPT_ADDCMD_COMPOBJECT('ENSO', nOperation*, NewScriptOperation, 2, (const char*, const char*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ENTO', nOperation*, NewTriggerOperation, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ENCO', nOperation*, NewCommandOperation, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGON', int, GetOperationsNumber, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGOP', nOperation*, GetOperation, 2, (const char*, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESSO', void, SetScriptOperation, 3, (const char*, int, const char*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EDOP', void, DeleteOperation, 2, (const char*, int), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    Default constructor
*/
ncTriggerOutput::ncTriggerOutput()
    : outputs(1,1)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
ncTriggerOutput::~ncTriggerOutput()
{
    this->DeleteAllOutputSets();
}

//-----------------------------------------------------------------------------
/**
    Initializes the instance
*/
void
ncTriggerOutput::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType != nObject::ReloadedInstance)
    {
        // Listen to signals coming from the input trigger component
        if ( this->GetComponent<ncAreaTrigger>() )
        {
            this->GetEntityObject()->BindSignal( ncAreaTrigger::SignalOnEnterArea, this, &ncTriggerOutput::OnEnterArea, 0 );
            this->GetEntityObject()->BindSignal( ncAreaTrigger::SignalOnExitArea, this, &ncTriggerOutput::OnExitArea, 0 );
        }
    }

/*    else if ( this->GetComponent<ncGenericTrigger>() )
    {
        this->GetEntityObject()->BindSignal( ncGenericTrigger::SignalOnTriggerActivated, this, &ncTriggerOutput::OnInternalEvent, 0 );
    }*/

    if ( initType == nObject::NewInstance )
    {
        // Add some output sets by default depending on the input trigger component
        // found in this entity
        if ( this->GetComponent<ncAreaTrigger>() )
        {
            this->AddOutputSet( nGameEvent::ENTER_IN_AREA );
            this->AddOutputSet( nGameEvent::EXIT_FROM_AREA );
        }
/*        else if ( this->GetComponent<ncGenericTrigger>() )
        {
            this->AddOutputSet( nGameEvent::TRIGGER_ACTIVATION );
        }*/
    }
}

//------------------------------------------------------------------------------
/**
    Execute the output for the given event
*/
/*void
ncTriggerOutput::OnInternalEvent( nGameEvent::Type internalEventType, nGameEvent* sourceEvent )
{
    this->Execute( internalEventType, sourceEvent );
}*/

//------------------------------------------------------------------------------
/**
    Execute the output for the on enter area event
*/
void
ncTriggerOutput::OnEnterArea( nGameEvent* sourceEvent )
{
    this->Execute( nGameEvent::ENTER_IN_AREA, sourceEvent );
}

//------------------------------------------------------------------------------
/**
    Execute the output for the on exit area event
*/
void
ncTriggerOutput::OnExitArea( nGameEvent* sourceEvent )
{
    this->Execute( nGameEvent::EXIT_FROM_AREA, sourceEvent );
}

//------------------------------------------------------------------------------
/**
    Execute the output for the trigger activation event
*/
void
ncTriggerOutput::OnTriggerActivation()
{
    this->Execute( nGameEvent::TRIGGER_ACTIVATION, NULL );
}

//------------------------------------------------------------------------------
/**
    Get the output set associated to an event, returning NULL if it doesn't exist
*/
ncTriggerOutput::OutputSet*
ncTriggerOutput::GetOutputSet( nGameEvent::Type eventType )
{
    OutputSet* output;
    if ( !this->outputs.Find( eventType, output ) )
    {
        return NULL;
    }
    return output;
}

//------------------------------------------------------------------------------
/**
    Execute the output
*/
void
ncTriggerOutput::Execute( nGameEvent::Type internalEventType, nGameEvent* sourceEvent, bool executeNow )
{
    // Get the output set for the given event
    OutputSet* output = this->GetOutputSet( internalEventType );
    n_assert( output );

    // If the output execution must be delayed, enque an execution operation to the trigger server
    if ( output->maxDelayTime > 0 && !executeNow )
    {
        nTime delay( n_rand_real_in_range( float(output->minDelayTime), float(output->maxDelayTime) ) );
        nTriggerServer::Instance()->QueueOutputExecution( this->GetEntityObject(), internalEventType, sourceEvent, delay );
        return;
    }

    // Otherwise execute the output set
    this->ExecuteOutputSet( *output, sourceEvent );
}

//------------------------------------------------------------------------------
/**
    Execute an output set

    The operations choose mechanism is first applied and then all the choosen
    operations are executed.

    Delay is not considered here, it's assumed that it has been checked and
    processed previously.
*/
void
ncTriggerOutput::ExecuteOutputSet( OutputSet& output, nGameEvent* sourceEvent )
{
    switch ( output.chooseType )
    {
        case OutputSet::EXECUTE_ALL:
            this->ExecuteAllOperations( output, sourceEvent );
            break;
        case OutputSet::EXECUTE_ONE:
            this->ExecuteOneRandomOperation( output, sourceEvent );
            break;
        case OutputSet::EXECUTE_SOME:
            this->ExecuteSomeRandomOperations( output, sourceEvent );
            break;
        default:
            n_error( "carles.ros: Shouldn't reach here. Maybe a new output choose option has been added?" );
    }
}

//------------------------------------------------------------------------------
/**
    Execute all the operations within an output set
*/
void
ncTriggerOutput::ExecuteAllOperations( OutputSet& output, nGameEvent* sourceEvent )
{
    for ( int i(0); i < output.operations.Size(); ++i )
    {
        this->ExecuteOperation( *output.operations[i], sourceEvent );
    }
}

//------------------------------------------------------------------------------
/**
    Choose one operation of the output set and execute it
*/
void
ncTriggerOutput::ExecuteOneRandomOperation( OutputSet& output, nGameEvent* sourceEvent )
{
    // If there isn't any operation to execute, simply finish now
    if ( output.operations.Empty() )
    {
        return;
    }

    // Choose and execute one operation
    int index( n_rand_int( output.operations.Size() - 1 ) );
    this->ExecuteOperation( *output.operations[index], sourceEvent );
}

//------------------------------------------------------------------------------
/**
    Choose a random number of operations from the output set and execute it

    Any number from 0 to all operations can be executed, but a same operation
    cannot be executed twice.
*/
void
ncTriggerOutput::ExecuteSomeRandomOperations( OutputSet& output, nGameEvent* sourceEvent )
{
    // Fill an array with all the operation indices
    nArray<int> indices( output.operations.Size(), 0 );
    for ( int i(0); i < output.operations.Size(); ++i )
    {
        indices.Append(i);
    }

    // Choose how many operations to not execute
    int opsToDiscard( n_rand_int( output.operations.Size() ) );

    // Choose and remove the operations to not execute
    for ( int i(0); i < opsToDiscard; ++i )
    {
        indices.Erase( n_rand_int( indices.Size() - 1 ) );
    }

    // Execute all the remaining operations
    for ( int i(0); i < indices.Size(); ++i )
    {
        this->ExecuteOperation( *output.operations[ indices[i] ], sourceEvent );
    }
}

//------------------------------------------------------------------------------
/**
    Execute an operation
*/
void
ncTriggerOutput::ExecuteOperation( nOperation& operation, nGameEvent* sourceEvent )
{
    nEntityObject* sourceEntity( NULL );
    if ( sourceEvent )
    {
        sourceEntity = nEntityObjectServer::Instance()->GetEntityObject( sourceEvent->GetEmitterEntity() );
    }
    operation.Execute( this->GetEntityObject(), sourceEntity );
}

//------------------------------------------------------------------------------
/**
    Add a new output operations set with default parameters

    Default parameters:

      - Initial operations: none
      - Delay time range: [0,0]
      - Output choose mechanism: execute all
    
    If there's already another set associated to the specified event nothing
    is done and old parameters remain untouched.
*/
void
ncTriggerOutput::AddOutputSet( nGameEvent::Type eventType )
{
    if ( !this->outputs.HasKey( eventType ) )
    {
        this->outputs.Add( eventType, n_new(OutputSet) );
    }
}

//------------------------------------------------------------------------------
/**
    Delete all the output operations sets
*/
void
ncTriggerOutput::DeleteAllOutputSets()
{
    for ( int i(0); i < this->outputs.Size(); ++i )
    {
        n_delete( this->outputs[i] );
    }
    this->outputs.Clear();
}

//------------------------------------------------------------------------------
/**
    Get the output set associated to an event, creating it if it doesn't exist
*/
ncTriggerOutput::OutputSet&
ncTriggerOutput::GetSafeOutputSet( const char* event )
{
    // Make sure there's an output set for the given event
    nGameEvent::Type eventType( nGameEvent::GetEventTransientId(event) );
    this->AddOutputSet( eventType );

    // Get the output set
    OutputSet* output( NULL );
    n_verify( this->outputs.Find( eventType, output ) );
    n_assert( output );

    return *output;
}

//------------------------------------------------------------------------------
/**
    Set the global paramaters for the output operations set associated to an event
*/
void
ncTriggerOutput::SetOutputSetConfig( const char* event, nTime minDelay, nTime maxDelay, int chooseType )
{
    OutputSet& output( this->GetSafeOutputSet(event) );
    output.minDelayTime = minDelay;
    output.maxDelayTime = maxDelay;
    output.chooseType = OutputSet::ChooseMechanism( chooseType );
}

//------------------------------------------------------------------------------
/**
    Get the global paramaters for the output operations set associated to an event
*/
void
ncTriggerOutput::GetOutputSetConfig( const char* event, nTime& minDelay, nTime& maxDelay, int& chooseType )
{
    OutputSet& output( this->GetSafeOutputSet(event) );
    minDelay = output.minDelayTime;
    maxDelay = output.maxDelayTime;
    chooseType = output.chooseType;
}

//------------------------------------------------------------------------------
/**
    Create a script operation and add it to an event
*/
nOperation*
ncTriggerOutput::NewScriptOperation( const char* event, const char* script )
{
    OutputSet& output( this->GetSafeOutputSet(event) );
    nOperation* operation = nTriggerServer::Instance()->GetScriptOperation( script );
    output.operations.Append( operation );
    return operation;
}

//------------------------------------------------------------------------------
/**
    Create a trigger operation and add it to an event
*/
nOperation*
ncTriggerOutput::NewTriggerOperation( const char* event )
{
    OutputSet& output( this->GetSafeOutputSet(event) );
    nOperation* operation = static_cast<nOperation*>( nKernelServer::Instance()->New( "ntriggeroperation" ) );
    output.operations.Append( operation );
    nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad( operation, nObject::LoadedInstance );
    return operation;
}

//------------------------------------------------------------------------------
/**
    Create a command operation and add it to an event
*/
nOperation*
ncTriggerOutput::NewCommandOperation( const char* event )
{
    OutputSet& output( this->GetSafeOutputSet(event) );
    nOperation* operation = static_cast<nOperation*>( nKernelServer::Instance()->New( "ncmdoperation" ) );
    output.operations.Append( operation );
    nKernelServer::Instance()->GetPersistServer()->BeginObjectLoad( operation, nObject::LoadedInstance );
    return operation;
}

//------------------------------------------------------------------------------
/**
    Get the number of output operations for an event
*/
int
ncTriggerOutput::GetOperationsNumber( const char* event )
{
    OutputSet& output( this->GetSafeOutputSet(event) );
    return output.operations.Size();
}

//------------------------------------------------------------------------------
/**
    Get an output operation by index for an event
*/
nOperation*
ncTriggerOutput::GetOperation( const char* event, int operationIndex )
{
    OutputSet& output( this->GetSafeOutputSet(event) );
    n_assert( operationIndex >= 0 && operationIndex < output.operations.Size() );
    return output.operations[ operationIndex ];
}

//------------------------------------------------------------------------------
/**
    Replace an operation by a script operation
*/
void
ncTriggerOutput::SetScriptOperation( const char* event, int operationIndex, const char* script )
{
    // Delete old operation
    OutputSet& output( this->GetSafeOutputSet(event) );
    n_assert( operationIndex >= 0 && operationIndex < output.operations.Size() );
    OutputSet::DeleteOperation( output.operations[ operationIndex ] );

    // Create new operation
    nScriptOperation* operation = nTriggerServer::Instance()->GetScriptOperation( script );
    output.operations[ operationIndex ] = operation;
}

//------------------------------------------------------------------------------
/**
    Delete an operation from an event
*/
void
ncTriggerOutput::DeleteOperation( const char* event, int operationIndex )
{
    OutputSet& output( this->GetSafeOutputSet(event) );
    n_assert( operationIndex >= 0 && operationIndex < output.operations.Size() );
    OutputSet::DeleteOperation( output.operations[ operationIndex ] );
    output.operations.Erase( operationIndex );
}

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncTriggerOutput::SaveCmds( nPersistServer* ps )
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // Clear all commands and sets to avoid accumulating them if the state is restored twice
        ps->Put( this->GetEntityObject(), 'EDOS' );

        for ( int i(0); i < this->outputs.Size(); ++i )
        {
            // Execution config (output set creation is implicit when setting config for first time)
            OutputSet* output = this->outputs[i];
            const char* eventLabel = nGameEvent::GetEventPersistentId( nGameEvent::Type( this->outputs.GetKeyAt(i) ) );
            ps->Put( this->GetEntityObject(), 'ESSC', eventLabel, output->minDelayTime, output->maxDelayTime, output->chooseType );

            // Operations
            for ( int j(0); j < output->operations.Size(); ++j )
            {
                nOperation* operation = output->operations[j];
                nCmd* cmd( NULL );
                switch ( operation->GetType() )
                {
                    case nOperation::COMMAND:
                        cmd = ps->GetCmd( this->GetEntityObject(), 'ENCO' );
                        break;
                    case nOperation::SCRIPT:
                        ps->Put( this->GetEntityObject(), 'ENSO', eventLabel, operation->GetName() );
                        break;
                    case nOperation::TRIGGER:
                        cmd = ps->GetCmd( this->GetEntityObject(), 'ENTO' );
                        break;
                }
                if ( cmd )
                {
                    cmd->In()->SetS( eventLabel );
                    if ( ps->BeginObjectWithCmd( this->GetEntityObject(), cmd) )
                    {
                        if ( !operation->SaveCmds( ps ) )
                        {
                            return false;
                        }
                        ps->EndObject( false );
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
ncTriggerOutput::OutputSet::OutputSet()
    : minDelayTime(0), maxDelayTime(0), chooseType( EXECUTE_ALL )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
ncTriggerOutput::OutputSet::~OutputSet()
{
    for ( int i(0); i < this->operations.Size(); ++i )
    {
        this->DeleteOperation( this->operations[i] );
    }
    this->operations.Clear();
}

//-----------------------------------------------------------------------------
/**
    Destroy an operation, if it isn't a shared one
*/
void
ncTriggerOutput::OutputSet::DeleteOperation( nOperation* operation )
{
    // Do not delete script operation objects, they're shared and managed by the trigger server
    if ( operation->GetType() != nOperation::SCRIPT )
    {
        operation->Release();
    }
}
