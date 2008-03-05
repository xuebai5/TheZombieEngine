//-----------------------------------------------------------------------------
//  ncgenerictrigger_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ncgenerictrigger.h"
#include "ntrigger/ntriggereventcondition.h"
#include "ntrigger/ntriggerstatecondition.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGenericTrigger,ncTrigger);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGenericTrigger)

    NSCRIPT_ADDCMD_COMPOBJECT('EAIE', void, AddInputEvent, 2, (int, const char*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGEN', int, GetInputEventsNumber , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGIE', int, GetInputEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESIE', void, SetInputEvent, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGIF', const char*, GetInputEventFilter, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESIF', void, SetInputEventFilter, 2, (int, const char*), 0, ());
/*    NSCRIPT_ADDCMD_COMPOBJECT('EAIT', void, AddInputTrigger, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGTN', int, GetInputTriggersNumber , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGIT', nEntityObjectId, GetInputTrigger, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESIT', void, SetInputTrigger, 2, (int, nEntityObjectId), 0, ());*/
/*    NSCRIPT_ADDCMD_COMPOBJECT('EAIS', void, AddInputState, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGSN', int, GetInputStatesNumber , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGIS', const char*, GetInputState, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESIE', void, SetInputState, 2, (int, const char*), 0, ());*/
/*    NSCRIPT_ADDCMD_COMPOBJECT('EGAT', int, GetActivationThreshold , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESAT', void, SetActivationThreshold, 1, (int), 0, ());*/

    cl->BeginSignals(1);
    NCOMPONENT_ADDSIGNAL( OnActivation )
    cl->EndSignals();

NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
NSIGNAL_DEFINE( ncGenericTrigger, OnActivation );

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
ncGenericTrigger::ncGenericTrigger()
    : activatedEventsCount(0),
      activationThreshold(1),
      inputEvents(0,1)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
ncGenericTrigger::~ncGenericTrigger()
{
    // Delete event conditions
    for ( int i(0); i < this->inputEvents.Size(); ++i )
    {
        n_delete( this->inputEvents[i] );
    }
    this->inputEvents.Clear();

    // State conditions are freed by the trigger server
    this->inputStates.Clear();
}

//-----------------------------------------------------------------------------
/**
    Answers to an event

    Checks if the event triggers some input condition and activates the
    trigger if the activation threshold has been reached (this resets the
    input events activation count, so the trigger gets ready to activate
    if the proper input conditions are given again).
*/
bool
ncGenericTrigger::HandleEvent( nGameEvent* event )
{
    n_assert( event );

    // Get the input condition for the incoming event
    nTriggerEventCondition* cond;
    if ( this->inputEvents.Find( event->GetType(), cond ) )
    {
        // Check if this event has already being counted as triggered
        if ( !cond->HasTriggered() )
        {
            // Check if the event filter accepts the event
            nEntityObject* emitter = nEntityObjectServer::Instance()->GetEntityObject( event->GetEmitterEntity() );
            if ( cond->Evaluate( this->GetEntityObject(), emitter ) )
            {
                // Activate this trigger if the activation threshold has been reached
                if ( ++this->activatedEventsCount >= this->activationThreshold )
                {
                    this->ActivateTrigger();
                }

                // Event handled, don't check more this tick
                return true;
            }
        }
    }

    // Event not handled, allow to check some more this tick
    return false;
}

//-----------------------------------------------------------------------------
/**
    Answers to an event

    Checks which input state condition evaluate to true and activates the
    trigger if the activation threshold is reached.
*/
void
ncGenericTrigger::HandleState()
{
    int activatedInputsCount( this->activatedEventsCount );
    for ( int i(0); i < this->inputStates.Size(); ++i )
    {
        // Check if the input state condition evaluates to true
        if ( this->inputStates[i]->Evaluate( this->GetEntityObject(), NULL ) )
        {
            // Activate this trigger if the activation threshold has been reached
            if ( ++activatedInputsCount >= this->activationThreshold )
            {
                this->ActivateTrigger();
                // Trigger activated and reset, so don't check more conditions this tick
                break;
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Reset internal input event counters

    This prepares the trigger to activate again from start, as if no input
    event has been received until now.
*/
void
ncGenericTrigger::Reset()
{
    for ( int i(0); i < this->inputEvents.Size(); ++i )
    {
        this->inputEvents[i]->ResetTriggered();
    }
    this->activatedEventsCount = 0;
}

//-----------------------------------------------------------------------------
/**
    Reset internal input event counters

    This prepares the trigger to activate again from start, as if no input
    event has been received until now.
*/
void
ncGenericTrigger::ActivateTrigger()
{
    // Signals trigger activation
    this->SignalOnActivation( this->GetEntityObject() );

    // Deactivate itself if trigger once is enabled
    this->OnInternalEvent();

    // Reset input activation counters
    this->Reset();
}

//-----------------------------------------------------------------------------
/**
    Add an input event condition
*/
void
ncGenericTrigger::AddInputEvent( int eventType, const char* filterName )
{
    n_assert( !this->inputEvents.HasKey( eventType ) );

    nTriggerEventCondition* cond = n_new( nTriggerEventCondition );
    cond->SetFilterConditionByName( filterName );
    this->inputEvents.Add( eventType, cond );
}

//-----------------------------------------------------------------------------
/**
    Get the number of input event conditions
*/
int
ncGenericTrigger::GetInputEventsNumber() const
{
    return this->inputEvents.Size();
}

//-----------------------------------------------------------------------------
/**
    Get the event for an input event condition by index
*/
int
ncGenericTrigger::GetInputEvent( int index ) const
{
    return this->inputEvents.GetKeyAt( index );
}

//-----------------------------------------------------------------------------
/**
    Set the event for an input event condition by index

    All indices become invalid after a call to this method
*/
void
ncGenericTrigger::SetInputEvent( int index, int eventType )
{
    // Reinsert the event condition attaching it to the new event type
    nTriggerEventCondition* cond = this->inputEvents.GetElementAt( index );
    this->inputEvents.RemByIndex( index );
    n_assert( !this->inputEvents.HasKey( eventType ) );
    this->inputEvents.Add( eventType, cond );
}

//-----------------------------------------------------------------------------
/**
    Get the filter for an input event condition by index
*/
const char*
ncGenericTrigger::GetInputEventFilter( int index ) const
{
    nTriggerEventCondition* cond = this->inputEvents.GetElementAt( index );
    return cond->GetFilterConditionName();
}

//-----------------------------------------------------------------------------
/**
    Set the filter for an input event condition by index
*/
void
ncGenericTrigger::SetInputEventFilter( int index, const char* filterName )
{
    nTriggerEventCondition* cond = this->inputEvents.GetElementAt( index );
    cond->SetFilterConditionByName( filterName );
}

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncGenericTrigger::SaveCmds( nPersistServer* ps )
{
    if ( ncTrigger::SaveCmds(ps) )
    {
        // Input events
        for ( int i(0); i < this->inputEvents.Size(); ++i )
        {
            ps->Put( this->entityObject, 'EAIE', this->inputEvents.GetKeyAt(i), this->inputEvents[i]->GetFilterConditionName() );
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
