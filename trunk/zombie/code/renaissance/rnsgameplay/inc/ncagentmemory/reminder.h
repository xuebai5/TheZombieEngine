#ifndef N_REMINDER_H
#define N_REMINDER_H

//------------------------------------------------------------------------------
/**
    @class Reminder

    Unit of knowledge about the world that an agent knows

    A reminder have an age that can be used as its reliability/accuracy.
    A reminder is less reliable/accurate when it's older, since the world state
    from where that reminder was extracted has got more time to change.
*/

#include "ntrigger/ngameevent.h"

//------------------------------------------------------------------------------
class Reminder
{
public:
    /// Default constructor
    Reminder();
    /// Destructor
    ~Reminder();

    /// Properties about how to use this reminder for inference processes
    enum InferenceProperties
    {
        NO_PROPERTIES = 0,
        INVESTIGATED = 1
    };
    /// Add inference properties for this reminder
    void SetProperties( InferenceProperties properties );
    /// Remove inference properties for this reminder
    void UnsetProperties( InferenceProperties properties );
    /// Tell if some inference properties are set
    bool HasProperties( InferenceProperties properties ) const;

    /// Refresh the reminder age as if it was newly perceived
    void RefreshReminder();
    /// Make the reminder older
    void AgeReminder( nGameEvent::Time timeInc );
    /// Get the current reminder's age
    nGameEvent::Time GetReminderAge() const;

private:
    /// Time since this reminder was perceived/concluded
    nGameEvent::Time age;
    /// Inference properties
    int properties;
};

//------------------------------------------------------------------------------
/**
    Default constructor
*/
inline
Reminder::Reminder() :
    age(0),
    properties( NO_PROPERTIES )
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
inline
Reminder::~Reminder()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Add inference properties for this reminder
*/
inline
void
Reminder::SetProperties( InferenceProperties properties )
{
    this->properties |= properties;
}

//------------------------------------------------------------------------------
/**
    Remove inference properties for this reminder
*/
inline
void
Reminder::UnsetProperties( InferenceProperties properties )
{
    this->properties = ~properties;
}

//------------------------------------------------------------------------------
/**
    Tell if some inference properties are set
*/
inline
bool
Reminder::HasProperties( InferenceProperties properties ) const
{
    return ( this->properties & properties ) != 0;
}

//------------------------------------------------------------------------------
/**
    Refresh the reminder age as if it was newly perceived
*/
inline
void
Reminder::RefreshReminder()
{
    this->age = 0;
}

//------------------------------------------------------------------------------
/**
    Make the reminder older
*/
inline
void
Reminder::AgeReminder( nGameEvent::Time timeInc )
{
    this->age += timeInc;
}

//------------------------------------------------------------------------------
/**
    Get the current reminder's age
*/
inline
nGameEvent::Time
Reminder::GetReminderAge() const
{
    return this->age;
}

//------------------------------------------------------------------------------
#endif // N_FACT_H
