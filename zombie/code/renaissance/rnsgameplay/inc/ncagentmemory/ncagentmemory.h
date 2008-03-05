#ifndef NC_AGENTMEMORY_H
#define NC_AGENTMEMORY_H

//------------------------------------------------------------------------------
/**
    @class ncAgentMemory

    Agent's memory

    Keep perceived events and how much time ago have been last perceived.
    It recalls the events for a short time, removing those that last too long
    without being perceived again.
*/

#include "entity/nentity.h"
#include "ntrigger/ngameevent.h"
#include "ncagentmemory/reminder.h"

//------------------------------------------------------------------------------
class ncAgentMemory : public nComponentObject
{

    NCOMPONENT_DECLARE(ncAgentMemory,nComponentObject);

public:
    /// Perceived events container (key=event, element=time since last perception)
    typedef nKeyArray<Reminder> tEventsContainer;

    /// Iterator for all the events known by the memory
    class EventsIterator
    {
    public:
        /// Set the container to iterate and set the iterator to its first element
        EventsIterator( const tEventsContainer& container )
            : container(container), index(0)
        {
            // Empty
        }
        /// Assignment operator
        const EventsIterator& operator = ( const EventsIterator& /*it*/ )
        {
            n_error( "carles.ros: Shouldn't reach here, use the copy constructor instead.\nThis function exists only to avoid the warning 'assignment operator could not be generated'" );
        }
        /// Tell if the iterator has passed the last event
        bool IsEnd() const
        {
            return this->index >= this->container.Size();
        }
        /// Return the current event
        nGameEvent::Id Get() const
        {
            return this->container.GetKeyAt( this->index );
        }
        /// Go to the next event
        void Next()
        {
            ++this->index;
        }
        /// Return the known reminder associated to the current event
        Reminder& GetReminder()
        {
            return this->container.GetElementAt( this->index );
        }

    private:
        /// Container of events to iterate
        const tEventsContainer& container;
        /// Current position in the container
        int index;
    };

    /// Iterator for all the entities known by the memory, without duplicates
    class EntitiesIterator
    {
    public:
        /// Set the container to iterate and set the iterator to its first element
        EntitiesIterator( const tEventsContainer& container );
        /// Assignment operator
        const EntitiesIterator& operator = ( const EntitiesIterator& /*it*/ )
        {
            n_error( "carles.ros: Shouldn't reach here, use the copy constructor instead.\nThis function exists only to avoid the warning 'assignment operator could not be generated'" );
        }
        /// Tell if the iterator has passed the last element
        bool IsEnd() const
        {
            return this->eventsIt.IsEnd();
        }
        /// Return the current element
        nEntityObjectId Get() const;
        /// Go to the next element
        void Next();

    private:
        /// Tell if an event has a source entity
        bool HasSourceEntity( nGameEvent::Id eventId ) const;

        /// Iterator used to iterate all the events in search for entities
        EventsIterator eventsIt;
        /// Entities found while iterating the events, so they can be skipped next time they're found (a set would be nicer :(
        nKeyArray<int> foundEntities;
    };

    /// Default constructor
    ncAgentMemory();
    /// Destructor
    ~ncAgentMemory();

    /// Initializes the instance
    void InitInstance(nObject::InitInstanceMsg initType);

    /// Component persistency
    bool SaveCmds (nPersistServer* ps);

    /// Add an event
    bool AddEvent( const nGameEvent* event );
    /// Remove an event
    void RemoveEvent( nGameEvent::Id eventId );
    /// Update memory
    void UpdateMemory( nGameEvent::Time timeInc = 1 );
    /// Clear the whole memory, setting it in an initial state
    void ResetMemory();
    /// Get the time that events are kept in memory without being perceived again
    nGameEvent::Time GetRecallTime() const;
    /// Set the time that events are kept in memory without being perceived again
    void SetRecallTime(int);
    /// Set the time that events are kept in memory without being perceived again from the living class
    void UpdateRecallTime();
    /// Get an iterator over all the events known by the memory
    EventsIterator GetEventsIterator() const;
    /// Get an iterator over all the entities known by the memory, without duplicates
    EntitiesIterator GetEntitiesIterator() const;

private:
    /// Perceived events and how much time ago have been last perceived
    tEventsContainer perceivedEvents;
    /// Time amount that events are kept in memory without being perceived again
    nGameEvent::Time recallTime;
};

//-----------------------------------------------------------------------------
/**
    Get the time that events are kept in memory without being perceived again
*/
inline
nGameEvent::Time
ncAgentMemory::GetRecallTime() const
{
    return this->recallTime;
}

//-----------------------------------------------------------------------------
/**
    Get an iterator over all the events known by the memory
*/
inline
ncAgentMemory::EventsIterator
ncAgentMemory::GetEventsIterator() const
{
    return EventsIterator( this->perceivedEvents );
}

//-----------------------------------------------------------------------------
/**
    Get an iterator over all the entities known by the memory, without duplicates
*/
inline
ncAgentMemory::EntitiesIterator
ncAgentMemory::GetEntitiesIterator() const
{
    return EntitiesIterator( this->perceivedEvents );
}

//------------------------------------------------------------------------------
#endif // NC_AGENTMEMORY_H
