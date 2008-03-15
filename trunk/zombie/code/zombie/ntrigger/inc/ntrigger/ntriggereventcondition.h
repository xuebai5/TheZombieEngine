#ifndef N_TRIGGEREVENTCONDITION_H
#define N_TRIGGEREVENTCONDITION_H

//------------------------------------------------------------------------------
/**
    @class nTriggerEventCondition
    @ingroup NebulaTriggerSystem

    Event condition for a generic trigger input.

    The condition doesn't store the event itself, but a flag telling if the
    condition has been activated (so this condition must be attached somewhat
    to an event).
    
    There's the option to overwrite the Evaluate method in a scripted class,
    which will be used then as a filter for the event. As input arguments, the
    Evaluate method receives the trigger itself plus the emitter entity of the
    event. As output, Evaluate should return a boolean telling if the event
    is accepted (true) or discarded (false).

    (C) 2006 Conjurer Services, S.A.
*/

class nTriggerStateCondition;

//------------------------------------------------------------------------------
class nTriggerEventCondition
{
public:
    /// Default constructor
    nTriggerEventCondition();
    /// Tell if this condition has already being triggered
    bool HasTriggered() const;
    /// Reset the 'condition has already being triggered' flag
    void ResetTriggered();
    /// Tell if the condition filter rule evaluates to true
    bool Evaluate( nEntityObject* trigger, nEntityObject* emitter );
    /// Get the filter condition, or NULL if there isn't any
    nTriggerStateCondition* GetFilterCondition() const;
    /// Get the filter condition name, or an empty string if there isn't any
    const char* GetFilterConditionName() const;
    /// Set the filter condition
    void SetFilterCondition( nTriggerStateCondition* filter );
    /// Set the filter condition, by its name
    void SetFilterConditionByName( const char* filterName );

private:
    /// Condition already activated?
    bool triggered;
    /// Filter condition
    nTriggerStateCondition* filterCondition;
};

//-----------------------------------------------------------------------------
/**
    Tell if this condition has already being triggered
*/
inline
bool
nTriggerEventCondition::HasTriggered() const
{
    return this->triggered;
}

//-----------------------------------------------------------------------------
/**
    Reset the 'condition has already being triggered' flag
*/
inline
void
nTriggerEventCondition::ResetTriggered()
{
    this->triggered = false;
}

//------------------------------------------------------------------------------
#endif // N_TRIGGEREVENTCONDITION_H
