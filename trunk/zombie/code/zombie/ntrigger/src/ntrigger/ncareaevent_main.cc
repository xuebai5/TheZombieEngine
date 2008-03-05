//-----------------------------------------------------------------------------
//  ncareaevent_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ncareaevent.h"
#include "ntrigger/ncareaeventclass.h"
#include "ntrigger/ngameevent.h"
#include "ntrigger/ntriggerserver.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncAreaEvent,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncAreaEvent)
    NSCRIPT_ADDCMD_COMPOBJECT('EEGE', nGameEvent::Id, EmitGameEvent, 4, (const char*, nEntityObject*, nGameEvent::Time, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESGE', void, StopGameEvent, 1, (nGameEvent::Id), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EDAE', void, DeleteAllEvents , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EPAE', void, PostAddEvent, 5, (const char*, nEntityObjectId, nEntityObjectId, nGameEvent::Time, int), 0, ());
    #ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('EGEC', int, GetEventsCount , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGEI', nGameEvent::Id, GetEventIdByIndex, 1, (int), 0, ());
    #endif
NSCRIPT_INITCMDS_END()

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
ncAreaEvent::ncAreaEvent()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
ncAreaEvent::~ncAreaEvent()
{
    this->DeleteAllEvents();
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
void
ncAreaEvent::DeleteAllEvents()
{
    for ( int i(0); i < this->events.Size(); ++i )
    {
        nTriggerServer::Instance()->DeleteEventUnsafe( this->events[i] );
    }
    this->events.Clear();
}

//-----------------------------------------------------------------------------
/**
    Initializes the instance
*/
void 
ncAreaEvent::InitInstance( nObject::InitInstanceMsg initType )
{
    if ( initType == nObject::NewInstance )
    {
        // New instances take the default emitted events from the class
        ncAreaEventClass* clazz( this->GetEntityClass()->GetComponent<ncAreaEventClass>() );
        if ( clazz )
        {
            nGameEvent event;
            event.SetSourceEntity( this->GetEntityObject()->GetId() );
            event.SetEmitterEntity( this->GetEntityObject()->GetId() );
            event.SetDuration( 0 );
            for ( int i(0); i < nGameEvent::EVENTS_NUMBER; ++i )
            {
                if ( clazz->GetEmittedEvent(i) )
                {
                    event.SetType( nGameEvent::Type(i) );
                    nGameEvent::Id eventId = nTriggerServer::Instance()->PostAreaEventUnsafe( event );
                    this->AddEvent( eventId );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Add an event (only locally, doesn't add it to trigger server)
*/
void
ncAreaEvent::AddEvent( nGameEvent::Id eventId )
{
    n_assert( this->events.FindIndex(eventId) == -1 );
    this->events.Append( eventId );
}

//-----------------------------------------------------------------------------
/**
    Remove an event (only locally, doesn't remove it from trigger server)
*/
void
ncAreaEvent::RemoveEvent( nGameEvent::Id eventId )
{
    int index = this->events.FindIndex(eventId);
    n_assert( index != -1 );
    this->events.Erase( index );
}

//-----------------------------------------------------------------------------
/**
    Add an event, registering it to the trigger server as well
*/
nGameEvent::Id
ncAreaEvent::EmitGameEvent( const char* eventType, nEntityObject* source, nGameEvent::Time duration, int priority )
{
    nGameEvent::Type event = nGameEvent::GetEventTransientId( eventType );
    return this->EmitGameEvent( event, source, duration, priority );
}

//-----------------------------------------------------------------------------
/**
Add an event, registering it to the trigger server as well
*/
nGameEvent::Id
ncAreaEvent::EmitGameEvent( nGameEvent::Type eventType, nEntityObject* source, nGameEvent::Time duration, int priority )
{
    nGameEvent event;
    event.SetType( eventType );
    if ( source )
    {
        event.SetSourceEntity( source->GetId() );
    }
    event.SetEmitterEntity( this->GetEntityObject()->GetId() );
    event.SetDuration( duration );
    event.SetPriority( priority );
    return nTriggerServer::Instance()->PostAreaEvent( event );
}

//-----------------------------------------------------------------------------
/**
    Remove an event, removing it from the trigger server as well
*/
void
ncAreaEvent::StopGameEvent( nGameEvent::Id eventId )
{
    nTriggerServer::Instance()->DeleteEvent( eventId );
}

//-----------------------------------------------------------------------------
/**
    Get an event by type, or NULL if there's no event for that type

    Don't use this method when performance is critical, it has a O(n/2) cost
*/
nGameEvent::Id
ncAreaEvent::GetEventByType( nGameEvent::Type eventType ) const
{
    for ( int i(0); i < this->events.Size(); ++i )
    {
        const nGameEvent* event = nTriggerServer::Instance()->GetEvent( this->events[i] );
        if ( event )
        {
            if ( event->GetType() == eventType )
            {
                return this->events[i];
            }
        }
    }
    return nGameEvent::INVALID_ID;
}

//-----------------------------------------------------------------------------
/**
    Post a command to the trigger server to create a new event
*/
void
ncAreaEvent::PostAddEvent( const char* eventTypeId, nEntityObjectId sourceId, nEntityObjectId emitterId, nGameEvent::Time duration, int priority )
{
    nGameEvent event;
    event.SetType( eventTypeId );
    event.SetSourceEntity( sourceId );
    event.SetEmitterEntity( emitterId );
    event.SetDuration( duration );
    event.SetPriority( priority );
    nGameEvent::Id eventId = nTriggerServer::Instance()->PostAreaEventUnsafe( event );
    this->AddEvent( eventId );
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Return the number of events emitted
*/
int
ncAreaEvent::GetEventsCount() const
{
    return this->events.Size();
}

//-----------------------------------------------------------------------------
/**
    Get the id of an event by index
*/
nGameEvent::Id
ncAreaEvent::GetEventIdByIndex( int index ) const
{
    const nGameEvent* event = nTriggerServer::Instance()->GetEvent( this->events[index] );
    if ( !event )
    {
        return nGameEvent::INVALID_ID;
    }
    return event->GetId();
}

#endif //NGAME

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncAreaEvent::SaveCmds( nPersistServer* ps )
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // Clear event list to avoid accumulating events if state is restored twice
        ps->Put( this->entityObject, 'EDAE' );

        // Emitted events
        for ( int i(0); i < this->events.Size(); ++i )
        {
            const nGameEvent* event = nTriggerServer::Instance()->GetEvent( this->events[i] );
            if ( event )
            {
                ps->Put( this->entityObject, 'EPAE',
                    event->GetPersistentType(),
                    event->GetSourceEntity(),
                    event->GetEmitterEntity(),
                    event->GetDuration(),
                    event->GetPriority() );
            }
        }
    }
    return true;
}
