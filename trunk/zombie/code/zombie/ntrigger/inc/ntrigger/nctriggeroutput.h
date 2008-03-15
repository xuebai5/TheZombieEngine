#ifndef NC_TRIGGEROUTPUT_H
#define NC_TRIGGEROUTPUT_H

//------------------------------------------------------------------------------
/**
    @class ncTriggerOutput
    @ingroup NebulaTriggerSystem

    Component to specify an output for a trigger.

    Not all triggers execute the output the same way, so this component is only
    needed for those triggers that have enough with a generic output mechanism
    (for instance, ncAgentTrigger uses its own mechanism to execute the output).
    So, since not all triggers may use this component, it has been decoupled
    from ncTrigger to avoid memory waste (specially true if we consider the
    huge amount of agent entities that may populate a level).

    (C) 2006 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "ntrigger/ngameevent.h"
#include "ntrigger/noperation.h"

//------------------------------------------------------------------------------
class ncTriggerOutput : public nComponentObject
{

    NCOMPONENT_DECLARE(ncTriggerOutput,nComponentObject);

public:
    /// Default constructor
    ncTriggerOutput();
    /// Destructor
    ~ncTriggerOutput();
    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);
    /// Execute the output
    void Execute( nGameEvent::Type internalEventType, nGameEvent* sourceEvent, bool executeNow = false );
    /// Add a new output operations set with default parameters
    void AddOutputSet( nGameEvent::Type eventType );

    /// Delete all the output operations sets
    void DeleteAllOutputSets ();
    /// Set the global paramaters for the output operations set associated to an event
    void SetOutputSetConfig(const char*, nTime, nTime, int);
    /// Get the global paramaters for the output operations set associated to an event
    void GetOutputSetConfig(const char*, nTime&, nTime&, int&);
    /// Create a script operation and add it to an event
    nOperation* NewScriptOperation(const char*, const char*);
    /// Create a trigger operation and add it to an event
    nOperation* NewTriggerOperation(const char*);
    /// Create a command operation and add it to an event
    nOperation* NewCommandOperation(const char*);
    /// Get the number of output operations for an event
    int GetOperationsNumber(const char*);
    /// Get an output operation by index for an event
    nOperation* GetOperation(const char*, int);
    /// Replace an operation by a script operation
    void SetScriptOperation(const char*, int, const char*);
    /// Delete an output operation by index from an event
    void DeleteOperation(const char*, int);

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

protected:
    /// Params and operations of the output associated to an event
    struct OutputSet
    {
        /// Minimum time to delay the output execution
        nTime minDelayTime;
        /// Maximum time to delay the output execution
        nTime maxDelayTime;
        /// Mechanism to choose the outputs to execute
        enum ChooseMechanism
        {
            EXECUTE_ALL,
            EXECUTE_ONE,
            EXECUTE_SOME
        } chooseType;
        /// Output operations
        nArray<nOperation*> operations;

        /// Default constructor
        OutputSet();
        /// Destructor
        ~OutputSet();
        /// Destroy an operation, if it isn't a shared one
        static void DeleteOperation( nOperation* operation );
    };

    /// Execute the output for the on enter area event
    void OnEnterArea( nGameEvent* sourceEvent );
    /// Execute the output for the on exit area event
    void OnExitArea( nGameEvent* sourceEvent );
    /// Execute the output for the trigger activation event
    void OnTriggerActivation();
    /// Execute an output set
    void ExecuteOutputSet( OutputSet& output, nGameEvent* sourceEvent );
    /// Execute all the operations within an output set
    void ExecuteAllOperations( OutputSet& output, nGameEvent* sourceEvent );
    /// Choose one operation of the output set and execute it
    void ExecuteOneRandomOperation( OutputSet& output, nGameEvent* sourceEvent );
    /// Choose a random number of operations from the output set and execute it
    void ExecuteSomeRandomOperations( OutputSet& output, nGameEvent* sourceEvent );
    /// Execute an operation
    void ExecuteOperation( nOperation& operation, nGameEvent* sourceEvent );
    /// Get the output set associated to an event, returning NULL if it doesn't exist
    OutputSet* GetOutputSet( nGameEvent::Type event );
    /// Get the output set associated to an event, creating it if it doesn't exist
    OutputSet& GetSafeOutputSet( const char* event );

    /// Outputs for each event
    nKeyArray<OutputSet*> outputs;

};

//-----------------------------------------------------------------------------
#endif // NC_TRIGGEROUTPUT_H
