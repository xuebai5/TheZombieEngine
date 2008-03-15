#ifndef NC_TRIGGERCLASS_H
#define NC_TRIGGERCLASS_H

//------------------------------------------------------------------------------
/**
    @class ncTriggerClass
    @ingroup NebulaTriggerSystem

    Container for default input events.

    When an entity with the ncTrigger component is instantiaded for first time
    (not restored from disk) it inits its input events from this component.
    Later changes on this component doesn't make its instances to update their
    input events (and it shouldn't since those could have been adjusted by the
    user for that individual instance).

    (C) 2006 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "ntrigger/nflags.h"

//------------------------------------------------------------------------------
class ncTriggerClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncTriggerClass, nComponentClass);

public:
    /// Default constructor
    ncTriggerClass();
    /// Destructor
    ~ncTriggerClass();

    /// Set all the event flags to false
    void ClearAllPerceivableEvents ();
    /// Set the value of an event flag
    void SetPerceivableEvent(int, bool);
    /// Get the value of an event flag
    bool GetPerceivableEvent(int) const;
    /// Set the value of an event flag by its persistent id (used for persistence)
    void SetPerceivableEventByPersistendId(const char*, bool);

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Which event types instances answers to by default
    nFlags eventFlags;

};

//-----------------------------------------------------------------------------
/**
    Set the value of an event flag
*/
inline
void
ncTriggerClass::SetPerceivableEvent( int flagIndex, bool value )
{
    this->eventFlags.SetFlag( flagIndex, value );
}

//-----------------------------------------------------------------------------
/**
    Get the value of an event flag
*/
inline
bool
ncTriggerClass::GetPerceivableEvent( int flagIndex ) const
{
    return this->eventFlags.IsFlagEnabled( flagIndex );
}

//-----------------------------------------------------------------------------
#endif // NC_TRIGGERCLASS_H
