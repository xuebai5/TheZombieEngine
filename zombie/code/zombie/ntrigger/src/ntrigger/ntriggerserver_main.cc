//-----------------------------------------------------------------------------
//  ntriggerserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ntriggerserver.h"
#include "ntrigger/nctrigger.h"
#include "ntrigger/ncareaevent.h"
#include "ntrigger/nscriptoperation.h"
#include "ntrigger/nctriggeroutput.h"
#include "ntrigger/ncareatrigger.h"
#include "ntrigger/ncagenttrigger.h"
#include "ncsound/ncsound.h"

// @todo Remove when moving motion related time intervals out from the trigger server
#include "ncaimovengine/ncaimovengine.h"

#ifndef NGAME
#include "nspatial/ncspatialcamera.h"
#endif

//------------------------------------------------------------------------------
nNebulaScriptClass(nTriggerServer, "nroot");

//------------------------------------------------------------------------------
const int nTriggerServer::ErrorsLog  = NLOG1;
const int nTriggerServer::GeneralLog = NLOG2;

namespace
{

    const char * logNames[] = {
        "Errors",
        "General",
        0
    };
}

NCREATELOGLEVELGROUP( trigger, "Trigger System", true, 1, logNames, nTriggerServer::ErrorsLog );

//------------------------------------------------------------------------------
nTriggerServer* nTriggerServer::instance = 0;

//-----------------------------------------------------------------------------
/**
    Default constructor
*/
nTriggerServer::nTriggerServer()
    : nextEventId( nGameEvent::INVALID_ID + 1 ),
      currentTime(0),
      updateTimeInterval(4),
      cellsUpdateTimeInterval(16),
      avoidanceCellsUpdateInterval(5),
      gameEvents( 64, 64 )
#ifdef __NEBULA_STATS__
      , profUpdate("profAITSUpdate", true)
      , profGatherAreaEvents("profAITSGatherAreaEvents", true)
      , profHandleAreaEvents("profAITSHandleAreaEvents", true)
#endif
#ifndef NGAME
      , updatingSoundSources( false )
#endif
{
    if ( !nTriggerServer::instance )
    {
        // Initialize instance pointer
        nTriggerServer::instance = this;
    }
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
nTriggerServer::~nTriggerServer()
{
    this->Reset();
    nTriggerServer::instance = 0;
}

//-----------------------------------------------------------------------------
/**
    Do any required set up dependend of current level to begin updating the triggers.
*/
void nTriggerServer::Start()
{
    this->SpreadTriggersOverTime();
    this->SpreadPerceivableCellsOverTime();
    this->SpreadMotionCellsOverTime();
}

//-----------------------------------------------------------------------------
/**
    Do any required clean up depended of current level to stop updating the triggers
*/
void nTriggerServer::Stop()
{
    this->ClearTempData();
}

//-----------------------------------------------------------------------------
/**
    Spread out the triggers updating homogenously among several frames
*/
void nTriggerServer::SpreadTriggersOverTime()
{
    for ( int i(0); i < this->areaTriggers.Size(); ++i )
    {
        nEntityObject* trigger = nEntityObjectServer::Instance()->GetEntityObject( this->areaTriggers[i] );
        trigger->GetComponentSafe<ncTrigger>()->SetNextUpdateTime( i % this->updateTimeInterval );
    }
}

//-----------------------------------------------------------------------------
/**
    Spread out the gathering of perceivable cells among several frames
*/
void nTriggerServer::SpreadPerceivableCellsOverTime()
{
    for ( int i(0); i < this->areaTriggers.Size(); ++i )
    {
        nEntityObject* entity = nEntityObjectServer::Instance()->GetEntityObject( this->areaTriggers[i] );
        ncTrigger* trigger = entity->GetComponentSafe<ncTrigger>();

        /// Get the initial perceivable cells
        nArray<nEntityObject*> emitters;
        this->GetNearEmitters( trigger, true, emitters );

        // Set the next time the list of perceivable cells will be updated
        trigger->SetPerceivableCellsUpdateTime( i % this->cellsUpdateTimeInterval );
    }
}

//-----------------------------------------------------------------------------
/**
    Spread out the gathering of near cells for steering behaviors among several frames
*/
void nTriggerServer::SpreadMotionCellsOverTime()
{
    for ( int i(0); i < this->areaTriggers.Size(); ++i )
    {
        nEntityObject* entity = nEntityObjectServer::Instance()->GetEntityObject( this->areaTriggers[i] );
        ncAIMovEngine* motion = entity->GetComponent<ncAIMovEngine>();
        if ( motion )
        {
            /// Get the initial near cells
            nArray<nEntityObject*> entities;
            motion->GetNearDynamicObstacles( true, entities );

            // Set the next time the list of near cells will be updated
            motion->SetAvoidanceCellsUpdateTime( i % this->avoidanceCellsUpdateInterval );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Clear all temporal data, like pending events and execution orders
*/
void
nTriggerServer::ClearTempData()
{
    // Pending output executions
    for ( int i(0); i < this->pendingOutputExecutions.Size(); ++i )
    {
        this->pendingOutputExecutions[i].Destroy();
    }
    this->pendingOutputExecutions.Clear();
}

//-----------------------------------------------------------------------------
/**
    Clear all triggers and events (triggers are not deleted, just removed)
*/
void
nTriggerServer::Reset()
{
    this->ClearTempData();

    // Instant broadcast events
    for ( int i(0); i < this->instantBroadcastEvents.Size(); ++i )
    {
        this->DestroyEvent( this->instantBroadcastEvents[i] );
    }
    this->instantBroadcastEvents.Clear();

    // Expirable area events
    for ( int i(0); i < this->expirableAreaEvents.Size(); ++i )
    {
        this->DestroyEvent( this->expirableAreaEvents[i] );
    }
    this->expirableAreaEvents.Clear();

    // Permanent area events
    for ( int i(0); i < this->permanentAreaEvents.Size(); ++i )
    {
        this->DestroyEvent( this->permanentAreaEvents[i] );
    }
    this->permanentAreaEvents.Clear();
    NLOG( trigger, (nTriggerServer::GeneralLog | 1, "All events erased by call to reset") );

    // Triggers
    this->broadcastTriggers.Clear();
    this->areaTriggers.Clear();
    NLOG( trigger, (nTriggerServer::GeneralLog | 1, "All triggers removed by call to reset") );
}

//-----------------------------------------------------------------------------
/**
    Register a trigger
*/
void
nTriggerServer::RegisterTrigger( nEntityObject* entity )
{
    nEntityObjectId entityId( entity->GetId() );

    // Classify triggers as area or broadcast triggers
    if ( entity->GetComponent<ncAreaTrigger>() || entity->GetComponent<ncAgentTrigger>() )
    {
        if ( this->areaTriggers.FindIndex( entityId ) == -1 )
        {
            this->areaTriggers.Append( entityId );
            NLOG( trigger, (nTriggerServer::GeneralLog | 0,
                "Registered area trigger %d", entityId) );
        }
    }
    else
    {
        if ( this->broadcastTriggers.FindIndex( entityId ) == -1 )
        {
            this->broadcastTriggers.Append( entityId );
            NLOG( trigger, (nTriggerServer::GeneralLog | 0,
                "Registered broadcast trigger %d", entityId) );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Remove a trigger (don't delete it)
*/
void
nTriggerServer::RemoveTrigger( nEntityObject* entity )
{
    nEntityObjectId entityId( entity->GetId() );

    // Remove the trigger from its trigger type group
    int index = this->areaTriggers.FindIndex( entityId );
    if ( index != -1 )
    {
        this->areaTriggers.Erase( index );
        NLOG( trigger, (nTriggerServer::GeneralLog | 0,
            "Unregistered area trigger %d", entityId) );
    }
    else
    {
        int index = this->broadcastTriggers.FindIndex( entityId );
        if ( index != -1 )
        {
            this->broadcastTriggers.Erase( index );
            NLOG( trigger, (nTriggerServer::GeneralLog | 0,
                "Unregistered broadcast trigger %d", entityId) );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Set the time between two consecutive updates of the same trigger
*/
void
nTriggerServer::SetUpdateInterval( nGameEvent::Time interval )
{
    this->updateTimeInterval = interval;
    this->SpreadTriggersOverTime();
}

//-----------------------------------------------------------------------------
/**
    Set the time between two consecutive updates of a perceivable cells list
*/
void
nTriggerServer::SetPerceivableCellsUpdateInterval( nGameEvent::Time interval )
{
    this->cellsUpdateTimeInterval = interval;
    this->SpreadPerceivableCellsOverTime();
}

//------------------------------------------------------------------------------
/**
    Get the time between two consecutive updates of a dynamic obstacles cells list
*/
void
nTriggerServer::SetAvoidanceCellsUpdateInterval( int interval )
{
    this->avoidanceCellsUpdateInterval = interval;
    this->SpreadMotionCellsOverTime();
}

//-----------------------------------------------------------------------------
/**
    Create a new event with default parameters
*/
nGameEvent*
nTriggerServer::CreateEvent()
{
    nGameEvent* event = n_new( nGameEvent );
    event->SetId( this->nextEventId++ );
    this->gameEvents.Add( event->GetId(), event );
    return event;
}

//-----------------------------------------------------------------------------
/**
    Create a new event with user parameters
*/
nGameEvent*
nTriggerServer::CreateEvent( const nGameEvent& eventDesc )
{
    nGameEvent* event = n_new( nGameEvent )( eventDesc );
    event->SetId( this->nextEventId++ );
    this->gameEvents.Add( event->GetId(), event );
    return event;
}

//-----------------------------------------------------------------------------
/**
    Destroy an event, removing it from its emitter
*/
void
nTriggerServer::DestroyEvent( nGameEvent::Id eventId, bool removeFromEmitter )
{
    nGameEvent* event( this->GetEvent(eventId) );
    if ( event )
    {
        this->gameEvents.Rem( event->GetId() );

        if ( removeFromEmitter )
        {
            if ( event->GetEmitterEntity() != nEntityObjectServer::IDINVALID )
            {
                nEntityObject* entity = nEntityObjectServer::Instance()->GetEntityObject( event->GetEmitterEntity() );
                if ( entity )
                {
                    entity->GetComponentSafe<ncAreaEvent>()->RemoveEvent( eventId );
                }
            }
        }

        n_delete( event );
    }
}

//-----------------------------------------------------------------------------
/**
    Emit a broadcast event (an event that reaches all triggers)
*/
nGameEvent::Id
nTriggerServer::PostBroadcastEvent( nGameEvent::Type type, nEntityObjectId sourceId )
{
    nGameEvent* event = this->CreateEvent();
    event->SetType( type );
    event->SetSourceEntity( sourceId );

    this->instantBroadcastEvents.Append( event->GetId() );
    NLOG( trigger, (nTriggerServer::GeneralLog | 0,
        "Registered broadcast event( id=%d, type=%s, source=%d )",
        event->GetId(), event->GetPersistentType(), event->GetSourceEntity()) );

    return event->GetId();
}

//-----------------------------------------------------------------------------
/**
    Emit an area event (an event that reaches only to near triggers)
*/
nGameEvent::Id
nTriggerServer::PostAreaEvent( const nGameEvent& eventDesc )
{
    nGameEvent* event = this->CreateEvent( eventDesc );
    this->RegisterAreaEvent( event->GetId() );
    return event->GetId();
}

//-----------------------------------------------------------------------------
/**
    Add an area event (only locally, doesn't add it to the emitter)
*/
nGameEvent::Id
nTriggerServer::PostAreaEventUnsafe( const nGameEvent& eventDesc )
{
    nGameEvent* event = this->CreateEvent( eventDesc );
    this->RegisterAreaEvent( event->GetId(), false );
    return event->GetId();
}

//-----------------------------------------------------------------------------
/**
    Register an area event (version meant to be used from script side)
*/
nGameEvent::Id
nTriggerServer::PostAreaEvent( const char* eventType, nEntityObject* source, nEntityObject* emitter, nGameEvent::Time duration, int priority )
{
    nGameEvent event;
    event.SetType( eventType );
    if ( source )
    {
        event.SetSourceEntity( source->GetId() );
    }
    if ( emitter )
    {
        event.SetEmitterEntity( emitter->GetId() );
    }
    event.SetDuration( duration );
    event.SetPriority( priority );

    return this->PostAreaEvent( event );
}

//-----------------------------------------------------------------------------
/**
    Register an area event (an event that reaches only to near triggers)
*/
void
nTriggerServer::RegisterAreaEvent( nGameEvent::Id eventId, bool addToEmitter )
{
    nGameEvent* event( this->GetEvent(eventId) );
    n_assert( event );

    // Add the event in its emitter entity
    if ( addToEmitter )
    {
        nEntityObject* emitter = nEntityObjectServer::Instance()->GetEntityObject( event->GetEmitterEntity() );
        if ( !emitter )
        {
            // If the emitter no longer exists, discard the event
            return;
        }
        emitter->GetComponentSafe<ncAreaEvent>()->AddEvent( eventId );
    }

    // Add the event in the correct events group
    if ( event->Expires() )
    {
        this->expirableAreaEvents.Append( eventId );
    }
    else
    {
        this->permanentAreaEvents.Append( eventId );
    }

    // Start the event's life
    event->StartEvent( this->currentTime );
#ifndef NGAME
    nString str;
    event->ToString( str );
    NLOG( trigger, (nTriggerServer::GeneralLog | 0,
        "Registered area event( %s )", str.Get()) );
#endif
}

//-----------------------------------------------------------------------------
/**
    Delete an event, removing it from its emitter
*/
void
nTriggerServer::DeleteEvent( nGameEvent::Id eventId )
{
    this->RemoveEventFromEventsGroups( eventId );
    this->DestroyEvent( eventId );
}

//-----------------------------------------------------------------------------
/**
    Delete an event, not removing it from its emitter
*/
void
nTriggerServer::DeleteEventUnsafe( nGameEvent::Id eventId )
{
    this->RemoveEventFromEventsGroups( eventId );
    this->DestroyEvent( eventId, false );
}

//-----------------------------------------------------------------------------
/**
    Remove and event from the events groups
*/
void
nTriggerServer::RemoveEventFromEventsGroups( nGameEvent::Id eventId )
{
    if ( !this->DeleteInstantBroadcastEvent(eventId) )
    {
        if ( !this->DeleteExpirableAreaEvent(eventId) )
        {
            this->DeletePermanentAreaEvent(eventId);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Destroy an event if found among the instant broadcast events
*/
bool
nTriggerServer::DeleteInstantBroadcastEvent( nGameEvent::Id eventId )
{
    int index( this->instantBroadcastEvents.FindIndex(eventId) );
    if ( index == -1 )
    {
        return false;
    }

    this->instantBroadcastEvents.Erase( index );
    NLOG( trigger, (nTriggerServer::GeneralLog | 0, "Erased broadcast event %d", eventId) );
    return true;
}

//-----------------------------------------------------------------------------
/**
    Destroy an event if found among the expirable area events
*/
bool
nTriggerServer::DeleteExpirableAreaEvent( nGameEvent::Id eventId )
{
    int index( this->expirableAreaEvents.FindIndex(eventId) );
    if ( index == -1 )
    {
        return false;
    }

    this->expirableAreaEvents.Erase( index );
    NLOG( trigger, (nTriggerServer::GeneralLog | 0, "Erased area event %d", eventId) );
    return true;
}

//-----------------------------------------------------------------------------
/**
    Destroy an event if found among the permanent area events
*/
bool
nTriggerServer::DeletePermanentAreaEvent( nGameEvent::Id eventId )
{
    int index( this->permanentAreaEvents.FindIndex(eventId) );
    if ( index == -1 )
    {
        return false;
    }

    this->permanentAreaEvents.Erase( index );
    NLOG( trigger, (nTriggerServer::GeneralLog | 0, "Erased area event %d", eventId) );
    return true;
}

//-----------------------------------------------------------------------------
/**
    Main loop
*/
void
nTriggerServer::Update( const nGameEvent::Time& currentTime )
{
#ifdef __NEBULA_STATS__
    this->profUpdate.StartAccum();
#endif

    this->currentTime = currentTime;
    this->ExecuteTriggerOutputs();
    this->ExpireEvents();
    this->ProcessBroadcastEvents();
    this->ProcessAreaEvents();

#ifdef __NEBULA_STATS__
    this->profUpdate.StopAccum();
#endif
}

//-----------------------------------------------------------------------------
/**
    Delete expired events
*/
void
nTriggerServer::ExpireEvents()
{
    // Delete expired area events
    for ( int i( this->expirableAreaEvents.Size() - 1 ); i >= 0; --i )
    {
        nGameEvent* event = this->GetEvent( this->expirableAreaEvents[i] );
        n_assert( event );

        if ( event->Expires() )
        {
            if ( event->GetExpirationTime() < this->currentTime )
            {
                // Keep values for probable later emitter destroying
                bool destroyEmitter( event->GetProperties() & nGameEvent::AUTODESTROY_EMITTER );
                nEntityObjectId emitterId( event->GetEmitterEntity() );

                // It's safe to erase the event since we are iterating backwards
                // and the indexes for the next events are untouched
                NLOG( trigger, (nTriggerServer::GeneralLog | 0,
                    "Erased area event %d by expiration", event->GetId()) );
                this->DestroyEvent( event->GetId() );
                this->expirableAreaEvents.Erase(i);

                // Destroy the emitter too if it was an autodestruction event
                // Destruction is done after event deleting to avoid trying to delete
                // the event twice (one started by the emitter destructor)
                if ( destroyEmitter )
                {
                    nWorldInterface::Instance()->DeleteEntityById( emitterId );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Allow triggers to handle the broadcast events
*/
void
nTriggerServer::ProcessBroadcastEvents()
{
    // Send instant broadcast events to all triggers that may handle them, deleting the events afterwards
    // @todo Test
    // @todo Do it using the signal system or a similar way that avoids all this iterating
    for ( int i(0); i < this->instantBroadcastEvents.Size(); ++i )
    {
        nGameEvent* event = this->GetEvent( this->instantBroadcastEvents[i] );
        n_assert( event );

        // Make non area triggers handle the event
        for ( int i(0); i < this->broadcastTriggers.Size(); ++i )
        {
            nEntityObject* entity( nEntityObjectServer::Instance()->GetEntityObject( this->broadcastTriggers[i] ) );
            if ( entity )
            {
                ncTrigger* trigger( entity->GetComponentSafe<ncTrigger>() );
                if ( trigger->GetTriggerEnabled() )
                {
                    if ( trigger->GetEventFlags().IsFlagEnabled( event->GetType() ) )
                    {
                        if ( trigger->HandleEvent( event ) )
                        {
                            NLOG( trigger, (nTriggerServer::GeneralLog | 0,
                                "Trigger %d has handled the event %d",
                                trigger->GetEntityObject()->GetId(), event->GetId()) );
                        }
                    }
                }
            }
        }

        // Make area triggers handle the event
/*        for ( int i(0); i < this->areaTriggers.Size(); ++i )
        {
            ncTrigger* trigger = this->areaTriggers[i];
            if ( trigger->GetTriggerEnabled() )
            {
                if ( trigger->GetEventFlags().IsFlagEnabled( event->eventType ) )
                {
                    if ( trigger->HandleEvent( event ) )
                    {
                        NLOG( trigger, (0, "Trigger %d has handled the event %d", trigger->GetEntityObject()->GetId(), event->eventId) );
                    }
                }
            }
        }*/

        this->DestroyEvent( event->GetId() );
    }
    this->instantBroadcastEvents.Clear();
}

//-----------------------------------------------------------------------------
/**
    Allow triggers to handle the area events
*/
void
nTriggerServer::ProcessAreaEvents()
{
    // Iterate triggers to allow them to handle the area events
    for ( int i(0); i < this->areaTriggers.Size(); ++i )
    {
        nEntityObject* entity( nEntityObjectServer::Instance()->GetEntityObject( this->areaTriggers[i] ) );
        if ( !entity )
        {
            continue;
        }
        ncTrigger* trigger( entity->GetComponentSafe<ncTrigger>() ); ;

        // Skip disabled triggers
        if ( !trigger->GetTriggerEnabled() )
        {
            continue;
        }

#ifndef NGAME
        if ( !this->updatingSoundSources )
        {
#endif
            // Skip the trigger if it's not its turn
            nGameEvent::Time updateTime = (trigger->GetNextUpdateTime() + 1) % this->updateTimeInterval;
            trigger->SetNextUpdateTime( updateTime );
            if ( updateTime != 0 )
            {
                continue;
            }
#ifndef NGAME
        }
        else if ( !trigger->GetEntityObject()->GetComponent<ncSound>() )
        {
            continue;
        }
#endif

#ifdef __NEBULA_STATS__
        this->profGatherAreaEvents.StartAccum();
#endif

        // Get near perceivable entities
        nArray<nEntityObject*> entities;
        nGameEvent::Time cellsUpdateTime = trigger->GetPerceivableCellsUpdateTime() + this->updateTimeInterval;
        bool updateCells = cellsUpdateTime >= this->cellsUpdateTimeInterval;
        if ( updateCells )
        {
            cellsUpdateTime -= this->cellsUpdateTimeInterval;
        }
        trigger->SetPerceivableCellsUpdateTime( cellsUpdateTime );
        this->GetNearEmitters( trigger, updateCells, entities );

#ifndef NGAME
        // Viewports' cameras are needed in the editor for testing simulated 'player' events,
        // but they aren't in the spatial server, so check for nearness manually
        if ( this->updatingSoundSources )
        {
            ncTransform* transform = trigger->GetComponentSafe<ncTransform>();
            sphere cullingSphere( transform->GetPosition(), trigger->GetCullingRadius() );
            const nArray<ncSpatialCamera*>& cameras( nSpatialServer::Instance()->GetCamerasArray() );
            for ( int c(0); c < cameras.Size(); ++c )
            {
                nEntityObject* camera( cameras[c]->GetEntityObject() );
                if ( camera->IsA("necamera") )
                {
                    if ( cullingSphere.contains( camera->GetComponentSafe<ncTransform>()->GetPosition() ) )
                    {
                        entities.Append( camera );
                    }
                }
            }
        }
#endif

        // Get event from entities and sort them by priority
        // @todo Make it more efficient
        const int NUM_PRIORITIES = 10;
        nArray<nGameEvent*> sortedEvents[NUM_PRIORITIES];
        for ( int i(0); i < entities.Size(); ++i )
        {
            ncAreaEvent* areaEvents = entities[i]->GetComponent<ncAreaEvent>();
            if ( areaEvents )
            {
                for ( ncAreaEvent::Iterator eventsIt( areaEvents->GetEventsIterator() ); !eventsIt.IsEnd(); eventsIt.Next() )
                {
                    nGameEvent* event = this->GetEvent( eventsIt.Get() );
                    n_assert( event->GetPriority() >= 0 && event->GetPriority() < NUM_PRIORITIES );
                    sortedEvents[ event->GetPriority() ].Append( event );
                }
            }
        }

#ifdef __NEBULA_STATS__
        this->profGatherAreaEvents.StopAccum();
        this->profHandleAreaEvents.StartAccum();
#endif

        // Go iterating all the events until the trigger handles one
        // Events with greater priority are checked first
        for ( int i(0); i < NUM_PRIORITIES; ++i )
        {
            nArray<nGameEvent*> events = sortedEvents[i];
            for ( int j(0); j < events.Size(); ++j )
            {
                nGameEvent* event = events[j];

                // Cull by event type
                if ( trigger->GetEventFlags().IsFlagEnabled( event->GetType() ) )
                {
                    if ( trigger->HandleEvent( event ) )
                    {
                        // A trigger can only answer to one area event at most, each cycle
                        NLOG( trigger, (nTriggerServer::GeneralLog | 0,
                            "Trigger %d has handled the event %d",
                            trigger->GetEntityObject()->GetId(), event->GetId()) );
                        break;
                    }
                }
            }
        }

#ifdef __NEBULA_STATS__
        this->profHandleAreaEvents.StopAccum();
#endif
    }
}

//-----------------------------------------------------------------------------
/**
    Get those event emitters within the trigger's culling radius
*/
void
nTriggerServer::GetNearEmitters( ncTrigger* trigger, bool updateCells, nArray<nEntityObject*>& emitters )
{
    // Build proximity sphere
    ncTransform* transform = trigger->GetComponentSafe<ncTransform>();
    sphere cullingSphere( transform->GetPosition(), trigger->GetCullingRadius() );

    // Get near event emitters
    nArray<int> categories;
    categories.Append( nSpatialTypes::CAT_AGENTS );
    categories.Append( nSpatialTypes::CAT_AREA_EVENTS );
    categories.Append( nSpatialTypes::CAT_VEHICLES );
    nArray<ncSpatialCell*>& cells = trigger->GetPerceivableCells();
    if ( updateCells )
    {
        // Get spatial cells too to speed up future queries
        cells.Clear();
        nSpatialServer::Instance()->GetEntitiesCellsCategories(
            cullingSphere, categories, nSpatialTypes::SPF_OUTDOORS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_POSITION,
            emitters, cells );
        nSpatialServer::Instance()->GetEntitiesCellsCategories(
            cullingSphere, categories, nSpatialTypes::SPF_ALL_INDOORS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_POSITION,
            emitters, cells );
    }
    else if ( !cells.Empty() )
    {
        // Search only in spatial cells found on previous queries
        nSpatialServer::Instance()->GetEntitiesUsingCellsCategories(
            cullingSphere, categories, nSpatialTypes::SPF_OUTDOORS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_POSITION,
            emitters, cells );
        nSpatialServer::Instance()->GetEntitiesUsingCellsCategories(
            cullingSphere, categories, nSpatialTypes::SPF_ALL_INDOORS |
            nSpatialTypes::SPF_CONTAINING | nSpatialTypes::SPF_USE_POSITION,
            emitters, cells );
    }
}

//-----------------------------------------------------------------------------
/**
    Create a dummy area event entity and make it emit an event
*/
nEntityObject*
nTriggerServer::PlaceAreaEvent( const vector3& position, const nGameEvent& eventDesc )
{
    // Create a simple area event entity
    nEntityObject* eventEntity = nWorldInterface::Instance()->NewLocalEntity( "neareaevent", position, false, NULL );
    n_assert( eventEntity );

    if( eventEntity )
    {
        // Start emitting the event
        nGameEvent event( eventDesc );
        event.SetEmitterEntity( eventEntity->GetId() );
        this->PostAreaEvent( event );
    }

    return eventEntity;
}

//-----------------------------------------------------------------------------
/**
    Create a dummy area event entity and make it emit an event

    Version meant to be used from script side
*/
nEntityObject*
nTriggerServer::PlaceAreaEvent( const vector3& position, const char* eventType, nEntityObject* source, nGameEvent::Time duration, int priority )
{
    nGameEvent event;
    event.SetType( eventType );
    if ( source )
    {
        event.SetSourceEntity( source->GetId() );
    }
    event.SetDuration( duration );
    event.SetPriority( priority );
    return this->PlaceAreaEvent( position, event );
}

//-----------------------------------------------------------------------------
/**
    Queue an order to execute a trigger output after some delay time
*/
void
nTriggerServer::QueueOutputExecution( nEntityObject* trigger, nGameEvent::Type triggerEvent, nGameEvent* eventSource, const nTime& executionDelay )
{
    n_assert( trigger );

    OutputExecutionDesc desc;
    desc.triggerId = trigger->GetId();
    desc.triggerEvent = triggerEvent;
    desc.sourceEventCopy = n_new( nGameEvent )( *eventSource );
    desc.executionTime = nTimeServer::Instance()->GetFrameTime() + executionDelay;
    this->pendingOutputExecutions.Append( desc );
}

//-----------------------------------------------------------------------------
/**
    Execute those pending trigger outputs whose execution time has been reached
*/
void
nTriggerServer::ExecuteTriggerOutputs()
{
    while ( !this->pendingOutputExecutions.Empty() )
    {
        OutputExecutionDesc& desc( this->pendingOutputExecutions.Front() );
        if ( desc.executionTime > nTimeServer::Instance()->GetFrameTime() )
        {
            // Still haven't reached the output execution time
            break;
        }

        // Execute output
        nEntityObject* trigger = nEntityObjectServer::Instance()->GetEntityObject( desc.triggerId );
        if ( trigger )
        {
            trigger->GetComponentSafe<ncTriggerOutput>()->Execute( desc.triggerEvent, desc.sourceEventCopy, true );
        }

        // Remove execution order
        desc.Destroy();
        this->pendingOutputExecutions.Erase(0);
    }
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Update only sound sources
*/
void
nTriggerServer::UpdateSoundSources()
{
    this->updatingSoundSources = true;
    this->ProcessAreaEvents();
    this->updatingSoundSources = false;
}

//-----------------------------------------------------------------------------
/**
    Get the type of an event by index
*/
int
nTriggerServer::GetEventType( nGameEvent::Id eventId ) const
{
    const nGameEvent* event = this->GetEvent( eventId );
    if ( !event )
    {
        return nGameEvent::INVALID_TYPE;
    }
    return event->GetType();
}

//-----------------------------------------------------------------------------
/**
    Get the source entity id of an event
*/
nEntityObjectId
nTriggerServer::GetEventSource( nGameEvent::Id eventId ) const
{
    const nGameEvent* event = this->GetEvent( eventId );
    if ( !event )
    {
        return nEntityObjectServer::IDINVALID;
    }
    return event->GetSourceEntity();
}

//-----------------------------------------------------------------------------
/**
    Get the duration of an event by index
*/
nGameEvent::Time
nTriggerServer::GetEventDuration( nGameEvent::Id eventId ) const
{
    const nGameEvent* event = this->GetEvent( eventId );
    if ( !event )
    {
        return 0;
    }
    return event->GetDuration();
}

//-----------------------------------------------------------------------------
/**
    Get the priority of an event by index
*/
int
nTriggerServer::GetEventPriority( nGameEvent::Id eventId ) const
{
    const nGameEvent* event = this->GetEvent( eventId );
    if ( !event )
    {
        return 0;
    }
    return event->GetPriority();
}
#endif
