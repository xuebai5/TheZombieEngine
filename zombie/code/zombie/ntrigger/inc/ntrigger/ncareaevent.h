#ifndef NC_AREAEVENT_H
#define NC_AREAEVENT_H

//------------------------------------------------------------------------------
/**
    @class ncAreaEvent
    @ingroup NebulaTriggerSystem

    Component for storing the area events that an entity emit.

    Entities that may be perceived must have this component. Note that the
    source of the event may not be necessarily the entity which owns this
    component (usually this is the case of sound events).

    Never add or remove entities by yourself, do it through the trigger server.
    This assures that all events become logged and that events with a finite
    live time are removed when they expire.
*/

#include "entity/nentity.h"
#include "ntrigger/ngameevent.h"

//------------------------------------------------------------------------------
class ncAreaEvent : public nComponentObject
{

    NCOMPONENT_DECLARE(ncAreaEvent,nComponentObject);

public:
    /// Iterator for all the events contained in this component
    class Iterator
    {
    public:
        /// Set the array to iterate and set the iterator to its first element
        Iterator( const nArray<nGameEvent::Id>& array )
            : container(array), index(0)
        {
            // Empty
        }
        /// Assignment operator
        const Iterator& operator = ( const Iterator& /*it*/ )
        {
            n_error( "carles.ros: Shouldn't reach here, this function exists only to avoid the warning:\n'assignment operator could not be generated'" );
        }
        /// Tell if the iterator has passed the last element
        bool IsEnd() const
        {
            return this->index >= this->container.Size();
        }
        /// Return the current element
        nGameEvent::Id Get() const
        {
            return this->container[ this->index ];
        }
        /// Go to the next element
        void Next()
        {
            ++this->index;
        }

    private:
        /// Array to iterate
        const nArray< nGameEvent::Id >& container;
        /// Current position in the array
        int index;
    };

    /// Default constructor
    ncAreaEvent();
    /// Destructor
    ~ncAreaEvent();
    /// Initializes the instance
    void InitInstance( nObject::InitInstanceMsg initType );

    /// Add an event (only locally, doesn't add it to trigger server)
    void AddEvent( nGameEvent::Id eventId );
    /// Remove an event (only locally, doesn't remove it from trigger server)
    void RemoveEvent( nGameEvent::Id eventId );
    /// Get an iterator over all the events contained in this component
    Iterator GetEventsIterator() const;
    /// Get an event by type, or nGameEvent::NO_EVENT if there's no event for that type
    nGameEvent::Id GetEventByType( nGameEvent::Type eventType ) const;

    // -- Scripting interface
    /// Add an event, registering it to the trigger server as well
    nGameEvent::Id EmitGameEvent( const char* eventType, nEntityObject* source, nGameEvent::Time duration, int priority );
    nGameEvent::Id EmitGameEvent( nGameEvent::Type eventType, nEntityObject* source, nGameEvent::Time duration, int priority );
    /// Remove an event, removing it from the trigger server as well
    void StopGameEvent( nGameEvent::Id eventId );

    // -- Persistence interface
    /// Remove all events locally and from the trigger server
    void DeleteAllEvents();
    /// Post a command to the trigger server to create a new event
    void PostAddEvent(const char*, nEntityObjectId, nEntityObjectId, nGameEvent::Time, int);

#ifndef NGAME
    // -- Editor interface
    /// Get the number of events emitted
    int GetEventsCount() const;
    /// Get the id of an event by index
    nGameEvent::Id GetEventIdByIndex(int) const;
#endif

    /// Nebula class persistence
    bool SaveCmds( nPersistServer* ps );

private:
    /// Events emitted by the entity this component belongs to
    nArray< nGameEvent::Id > events;

};

//-----------------------------------------------------------------------------
/**
    Get an iterator over all the events contained in this component
*/
inline
ncAreaEvent::Iterator
ncAreaEvent::GetEventsIterator() const
{
    return Iterator( this->events );
}

//-----------------------------------------------------------------------------
#endif // NC_AREAEVENT_H
