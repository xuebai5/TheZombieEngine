//-----------------------------------------------------------------------------
//  ncagentmemory.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchrnsgameplay.h"
#include "ncagentmemory/ncagentmemory.h"
#include "ncfsm/ncfsm.h"
#include "kernel/nlogclass.h"
#include "ncaistate/ncaistate.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "ntrigger/ntriggerserver.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncAgentMemory,nComponentObject);

//-----------------------------------------------------------------------------
/**
    Set the container to iterate and set the iterator to its first element
*/
ncAgentMemory::EntitiesIterator::EntitiesIterator( const tEventsContainer& container )
    : eventsIt( container ), foundEntities( container.Size() )
{
    // Skip initial invalid events not removed from memory yet or without source entity
    while ( !this->eventsIt.IsEnd() )
    {
        if ( this->HasSourceEntity( this->eventsIt.Get() ) )
        {
            break;
        }
        this->eventsIt.Next();
    }
}

//-----------------------------------------------------------------------------
/**
    Return the current element
*/
nEntityObjectId
ncAgentMemory::EntitiesIterator::Get() const
{
    nGameEvent* event( nTriggerServer::Instance()->GetEvent( this->eventsIt.Get() ) );
    n_assert( event );
    return event->GetSourceEntity();
}

//-----------------------------------------------------------------------------
/**
    Go to the next element
*/
void
ncAgentMemory::EntitiesIterator::Next()
{
    // Store current entity and go to the next "new" entity
    this->foundEntities.Add( this->Get(), 0 );
    for ( this->eventsIt.Next(); !this->eventsIt.IsEnd(); this->eventsIt.Next() )
    {
        // Skip invalid events not removed from memory yet
        nGameEvent* event = nTriggerServer::Instance()->GetEvent( this->eventsIt.Get() );
        if ( !event )
        {
            continue;
        }

        // Skip events without a source entity
        nEntityObjectId sourceEntity = event->GetSourceEntity();
        if ( sourceEntity == nEntityObjectServer::IDINVALID )
        {
            continue;
        }

        // Stop when found an event with a source entity not found in previous events
        int dummy;
        if ( !this->foundEntities.Find( sourceEntity, dummy ) )
        {
            break;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Tell if an event has a source entity
*/
bool
ncAgentMemory::EntitiesIterator::HasSourceEntity( nGameEvent::Id eventId ) const
{
    nGameEvent* event = nTriggerServer::Instance()->GetEvent( eventId );
    if ( !event )
    {
        return false;
    }
    return event->GetSourceEntity() != nEntityObjectServer::IDINVALID;
}

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
ncAgentMemory::ncAgentMemory() :
    perceivedEvents( 16, 16 )
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
ncAgentMemory::~ncAgentMemory()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Initializes the instance
*/
void 
ncAgentMemory::InitInstance(nObject::InitInstanceMsg initType)
{
    this->UpdateRecallTime();

    if (initType == nObject::ReloadedInstance)
    {
        this->ResetMemory();
    }
}

//-----------------------------------------------------------------------------
/**
    Add an event

    Set the perception time to 0 for that event, even if it already exists.
    Return false if the event already existed, true otherwise.
*/
bool ncAgentMemory::AddEvent( const nGameEvent* event )
{
    n_assert( event );
    nGameEvent::Id eventId( event->GetId() );
    if ( this->perceivedEvents.HasKey( eventId ) )
    {
        Reminder& reminder = this->perceivedEvents.GetElement( eventId );
        reminder.RefreshReminder();
        return false;
    }
    else
    {
        this->perceivedEvents.Add( eventId, Reminder() );
        return true;
    }
}

//-----------------------------------------------------------------------------
/**
    Remove and event
*/
void ncAgentMemory::RemoveEvent( nGameEvent::Id eventId )
{
    this->perceivedEvents.Rem( eventId );
}

//-----------------------------------------------------------------------------
/**
    Update memory

    Increment the time of the last perception for all the known events.
    Those events that reach the recall expiration time are removed.
    Events that have been deleted from the trigger server are removed as well.
    A 'target lost' event is given to the FSM when there isn't any remaining
    event that has the agent's target as its source entity.
*/
void ncAgentMemory::UpdateMemory( nGameEvent::Time timeInc )
{
    // Container to keep track of expired events and remove them safely afterwards
    nArray<int> expiredEvents;
    // Tell if an event has been deleted from the trigger server
    bool eventDeleted( false );
    // Tell if an event related to the agent's target has expired
    bool targetEventExpired( false );

    // Increment time since last perception for all events and get those that expires.
    nEntityObject* target = this->GetComponentSafe<ncAIState>()->GetTarget();
    for ( int i(0); i < this->perceivedEvents.Size(); ++i )
    {
        nGameEvent::Id eventId( this->perceivedEvents.GetKeyAt(i) );
        nGameEvent* event( nTriggerServer::Instance()->GetEvent(eventId) );

        if ( !event )
        {
            expiredEvents.Append( eventId );
            eventDeleted = true;
        }
        else
        {
            Reminder& reminder = this->perceivedEvents.GetElementAt(i);
            reminder.AgeReminder( timeInc );
#ifndef NGAME
            this->UpdateRecallTime();
#endif // !NGAME
            if ( reminder.GetReminderAge() > this->recallTime )
            {
                expiredEvents.Append( eventId );

                // Check if the target entity may be forgotten
                if ( target )
                {
                    if ( target->GetId() == event->GetSourceEntity() )
                    {
                        targetEventExpired = true;
                    }
                }
            }
        }
    }

    // Remove the events that had last too long without being perceived again.
    // This is done through and auxiliary array because removing elements from a nKeyArray
    // while iterating it isn't safe.
    for ( int i(0); i < expiredEvents.Size(); ++i )
    {
        this->perceivedEvents.Rem( expiredEvents[i] );
        NLOG( perception, (0, "Entity %d forgets event %d", this->GetEntityObject()->GetId(), expiredEvents[i] ) );
    }

    // If an event whose source matches the current target has been removed,
    // look for some other event whose source is the target too,
    // otherwise forget the target and notify a 'target lost' event to the FSM.
    // This step is done after having fully updated the memory because sending
    // an event to the FSM may imply a transition and a query to the memory.
    if ( eventDeleted || targetEventExpired )
    {
        for ( int i(0); i < this->perceivedEvents.Size(); ++i )
        {
            nGameEvent* event = nTriggerServer::Instance()->GetEvent( this->perceivedEvents.GetKeyAt(i) );
            if ( target )
            {
                if ( target->GetId() == event->GetSourceEntity() )
                {
                    // Target already perceived by another event, can keep it
                    return;
                }
            }            
        }

        // Target no longer perceived by any event, so forget it
        this->GetComponentSafe<ncAIState>()->SetTarget( NULL );
        ncFSM* fsm = this->GetComponent<ncFSM>();
        n_assert2( fsm, "The entity doesn't have the FSM component" );
#ifndef NGAME
        if ( !fsm )
        {
            return;
        }
#endif
        NLOG( perception, (0, "Entity %d forgets the target", this->GetEntityObject()->GetId()) );
        fsm->OnTransitionEvent( nGameEvent::TARGET_LOST );
    }
}

//-----------------------------------------------------------------------------
/**
    Clear the whole memory, setting it in an initial state
*/
void 
ncAgentMemory::ResetMemory()
{
    this->perceivedEvents.Clear();
}

//-----------------------------------------------------------------------------
/**
    Set the time that events are kept in memory without being perceived again
*/
void 
ncAgentMemory::SetRecallTime( int time )
{
    this->recallTime = time;
}

//-----------------------------------------------------------------------------
/**
    Set the time that events are kept in memory without being perceived again 
    from the living class
*/
void 
ncAgentMemory::UpdateRecallTime()
{
    ncGameplayLivingClass* livingClass = this->GetClassComponent<ncGameplayLivingClass>();
    if ( livingClass )
    {
        this->recallTime = livingClass->GetMemoryTime();
    }    
}
