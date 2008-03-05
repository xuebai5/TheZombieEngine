#ifndef NC_TRIGGER_H
#define NC_TRIGGER_H

//------------------------------------------------------------------------------
/**
    @class ncTrigger
    @ingroup NebulaTriggerSystem

    Base interface for all trigger components.

    All triggers have an update time and a bitfield of flags about which event
    types they want to answer. Typically a trigger will be told by the trigger
    system to handle and event when its update time has been reached and when
    the event is among the ones that the trigger wants to answer. Other sources
    may call a trigger to handle an event too, like an area trigger generating
    an internal 'on enter' event and giving it to its registered 'event listener'
    trigger. In these cases the update time of the trigger is ignored, but not
    its event flags.

    This component also have a culling radius, only used for triggers that
    handle area events when culling these events by proximity.
*/

#include "entity/nentity.h"
#include "ntrigger/nflags.h"
#include "ntrigger/ngameevent.h"

class ncSpatialCell;

//------------------------------------------------------------------------------
class ncTrigger : public nComponentObject
{

    NCOMPONENT_DECLARE(ncTrigger,nComponentObject);

public:
    /// Default constructor
    ncTrigger();
    /// Destructor
    virtual ~ncTrigger();
    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);
    /// Get the time when this entity must be allowed to handle events
    const nGameEvent::Time& GetNextUpdateTime() const;
    /// Set the time when this entity must be allowed to handle events
    void SetNextUpdateTime( const nGameEvent::Time& time );
    /// Return the event type flags that this entity answers to, as read-only
    const nFlags& GetEventFlags() const;
    /// Return the event type flags that this entity answers to, as read-and-write
    nFlags& GetEventFlags();
    /// Get the radius used to cull events by proximity
    float GetCullingRadius() const;
    /// Set the radius used to cull events by proximity
    void SetCullingRadius( float radius );
    /// Answers to an event, first doing additional culling if necessary
    virtual bool HandleEvent( nGameEvent* event ) = 0;

    /// Set all the event flags to false
    void ClearAllEventFlags ();
    /// Set the value of an event flag
    void SetEventFlag(int, bool);
    /// Get the value of an event flag
    bool GetEventFlag(int) const;
    /// Set the value of an event flag by its persistent id (used for persistence)
    void SetPerceivableEvent(const char*, bool);
    /// Tell if this trigger activates once or multiple times
    bool GetTriggerOnce () const;
    /// Set this trigger to activate once or multiple times
    void SetTriggerOnce(bool);
    /// Tell if this trigger is enabled to accept input or it's disabled
    bool GetTriggerEnabled () const;
    /// Enable this trigger to accept input or disable it
    void SetTriggerEnabled(bool);

    /// Get the time when the list of perceivable cells must be updated
    nGameEvent::Time GetPerceivableCellsUpdateTime() const;
    /// Set the time when the list of perceivable cells must be updated
    void SetPerceivableCellsUpdateTime( nGameEvent::Time time );
    /// Get the list of perceivable cells
    nArray<ncSpatialCell*>& GetPerceivableCells();

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

protected:
    /// Deactivate itself as an answer to an activation + trigger once enabled
    void OnInternalEvent();

    /// Time when this entity will be allowed to handle events
    nGameEvent::Time nextUpdateTime;
    /// Which event types this entity answers to
    nFlags eventFlags;
    /// Radius used to cull events by proximity
    float cullingRadius;
    /// Trigger once or multiple times?
    bool triggerOnce;
    /// Accepting input or not?
    bool enabled;

    /// Time when the list of perceivable spatial cells will be updated
    nGameEvent::Time cellsUpdateTime;
    /// Perceivable cells
    nArray<ncSpatialCell*> perceivableCells;

};

//-----------------------------------------------------------------------------
/**
    Get the time when this entity must be allowed to handle events
*/
inline
const nGameEvent::Time&
ncTrigger::GetNextUpdateTime() const
{
    return this->nextUpdateTime;
}

//-----------------------------------------------------------------------------
/**
    Set the time when this entity must be allowed to handle events
*/
inline
void
ncTrigger::SetNextUpdateTime( const nGameEvent::Time& time )
{
    this->nextUpdateTime = time;
}

//-----------------------------------------------------------------------------
/**
    Return the event type flags that this entity may answer to, as read-only
*/
inline
const nFlags&
ncTrigger::GetEventFlags() const
{
    return this->eventFlags;
}

//-----------------------------------------------------------------------------
/**
    Return the event type flags that this entity may answer to, as read-and-write
*/
inline
nFlags&
ncTrigger::GetEventFlags()
{
    return this->eventFlags;
}

//-----------------------------------------------------------------------------
/**
    Set the value of an event flag
*/
inline
void
ncTrigger::SetEventFlag( int flagIndex, bool value )
{
    this->eventFlags.SetFlag( flagIndex, value );
}

//-----------------------------------------------------------------------------
/**
    Get the value of an event flag
*/
inline
bool
ncTrigger::GetEventFlag( int flagIndex ) const
{
    return this->eventFlags.IsFlagEnabled( flagIndex );
}

//-----------------------------------------------------------------------------
/**
    Get the radius used to cull events by proximity
*/
inline
float
ncTrigger::GetCullingRadius() const
{
    return this->cullingRadius;
}

//-----------------------------------------------------------------------------
/**
    Set the radius used to cull events by proximity
*/
inline
void
ncTrigger::SetCullingRadius( float radius )
{
    this->cullingRadius = radius;
}

//------------------------------------------------------------------------------
/**
    Tell if this trigger is enabled to accept input or it's disabled
*/
inline
bool
ncTrigger::GetTriggerEnabled() const
{
    return this->enabled;
}

//-----------------------------------------------------------------------------
/**
    Get the time when the list of perceivable cells must be updated
*/
inline
nGameEvent::Time
ncTrigger::GetPerceivableCellsUpdateTime() const
{
    return this->cellsUpdateTime;
}

//-----------------------------------------------------------------------------
/**
    Set the time when the list of perceivable cells must be updated
*/
inline
void
ncTrigger::SetPerceivableCellsUpdateTime( nGameEvent::Time time )
{
    this->cellsUpdateTime = time;
}

//-----------------------------------------------------------------------------
/**
    Get the list of perceivable cells
*/
inline
nArray<ncSpatialCell*>&
ncTrigger::GetPerceivableCells()
{
    return this->perceivableCells;
}

//-----------------------------------------------------------------------------
#endif // NC_TRIGGER_H
