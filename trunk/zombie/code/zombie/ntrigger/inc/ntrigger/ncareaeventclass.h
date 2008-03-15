#ifndef NC_AREAEVENTCLASS_H
#define NC_AREAEVENTCLASS_H

//------------------------------------------------------------------------------
/**
    @class ncAreaEventClass
    @ingroup NebulaTriggerSystem

    Component for storing the default area events that an entity emit.

    When an entity with the ncAreaEvent component is instantiaded for first time
    (not restored from disk) it creates the same initial emitted events as
    contained by this component. Later changes on this component doesn't make
    its instances to update their emitted events (and it shouldn't since those
    could have been adjusted by the user for that individual instance).

    (C) 2006 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "ntrigger/nflags.h"

//------------------------------------------------------------------------------
class ncAreaEventClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncAreaEventClass, nComponentClass);

public:
    /// Default constructor
    ncAreaEventClass();
    /// Destructor
    ~ncAreaEventClass();

    /// Set all the event flags to false
    void ClearAllEmittedEvent ();
    /// Set the value of an event flag
    void SetEmittedEvent(int, bool);
    /// Get the value of an event flag
    bool GetEmittedEvent(int) const;
    /// Set the value of an event flag by its persistent id (used for persistence)
    void SetEmittedEventByPersistendId(const char*, bool);

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
ncAreaEventClass::SetEmittedEvent( int flagIndex, bool value )
{
    this->eventFlags.SetFlag( flagIndex, value );
}

//-----------------------------------------------------------------------------
/**
    Get the value of an event flag
*/
inline
bool
ncAreaEventClass::GetEmittedEvent( int flagIndex ) const
{
    return this->eventFlags.IsFlagEnabled( flagIndex );
}

//-----------------------------------------------------------------------------
#endif // NC_AREAEVENTCLASS_H
