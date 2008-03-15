#ifndef NC_GENERICTRIGGER_H
#define NC_GENERICTRIGGER_H

//------------------------------------------------------------------------------
/**
    @class ncGenericTrigger
    @ingroup NebulaTriggerSystem

    Trigger that fires for non area events.

    Multiple input conditions can be set for this trigger, including event
    conditions, state conditions and other trigger activation signals.
    The trigger activates when the minimum specified number of inputs evaluates
    to true. Event and trigger activations are stored when they are catched
    to increase the internal activated input count. State conditions are
    evaluated periodically, at the user specified frequency.

    Event conditions can have an associated filter script used to discard
    some events for some states (for instance, to only accept the 'death'
    event from entities of the class 'X').

    When the trigger activates, it generates the internal event/signal
    'trigger activation', so another component can catch the signal and act
    accordingly (for instance, the ncTriggerOutput component may execute the
    associated output).

    (C) 2006 Conjurer Services, S.A.
*/

#include "ntrigger/nctrigger.h"

class nTriggerEventCondition;
class nTriggerStateCondition;

//------------------------------------------------------------------------------
class ncGenericTrigger : public ncTrigger
{

    NCOMPONENT_DECLARE(ncGenericTrigger,ncTrigger);

public:
    /// Default constructor
    ncGenericTrigger();
    /// Destructor
    ~ncGenericTrigger();
    /// Answers to an event
    bool HandleEvent( nGameEvent* event );
    /// Evaluate state conditions
    void HandleState();
    /// Reset internal input event counters
    void Reset();

    /// Add an input event condition
    void AddInputEvent(int, const char*);
    /// Get the number of input event conditions
    int GetInputEventsNumber () const;
    /// Get the event for an input event condition by index
    int GetInputEvent(int) const;
    /// Set the event for an input event condition by index
    void SetInputEvent(int, int);
    /// Get the filter for an input event condition by index
    const char* GetInputEventFilter(int) const;
    /// Set the filter for an input event condition by index
    void SetInputEventFilter(int, const char*);

    /// Add a input trigger condition
/*     void AddInputTrigger(nEntityObjectId);
    /// Get the number of input trigger conditions
    int GetInputTriggersNumber () const;
    /// Get the trigger for an input trigger condition
    nEntityObjectId GetInputTrigger(int) const;
    /// Set the event for an input trigger condition
    void SetInputTrigger(int, nEntityObjectId);
*/
    /// Add an input state condition
/*     void AddInputState(const char*);
    /// Get the number of input state conditions
    int GetInputStatesNumber () const;
    /// Get the script for an input state condition by index
    const char* GetInputState(int) const;
    /// Set the script for an input state condition by index
    void SetInputState(int, const char*);*/

    /// Get the input number activation threshold
/*     int GetActivationThreshold () const;
    /// Set the input number activation threshold
    void SetActivationThreshold(int);*/

    NSIGNAL_DECLARE( 'EONA', void, OnActivation, 0, (), 0, ());

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

protected:
    /// Signal a trigger activation and resets the trigger to activate again from start
    void ActivateTrigger();

    /// Input event conditions (store if an event has been received)
    nKeyArray<nTriggerEventCondition*> inputEvents;
    /// Input state conditions (state activation isn't stored, but evaluated periodically)
    nArray<nTriggerStateCondition*> inputStates;
    /// Input trigger conditions (store if a trigger has been activated)
//    nKeyArray<bool> inputTriggers;
    /// Count of activated event/trigger inputs so far
    int activatedEventsCount;
    /// Minimum number required of activated inputs to fire the trigger
    int activationThreshold;

};

//-----------------------------------------------------------------------------
#endif // NC_GENERICTRIGGER_H
