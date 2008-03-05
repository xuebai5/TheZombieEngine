#ifndef N_TRIGGERSERVER_H
#define N_TRIGGERSERVER_H

//------------------------------------------------------------------------------
/**
    @class nTriggerServer
    @ingroup NebulaTriggerSystem

    Trigger server.

    Trigger server functions:

      - Sort events by priority
      - Remove expired events
      - Synchronize dynamic events with their source entity
      - Notify triggers about events, culled by type and proximity
      - Peristence of trigger scripts

    (C) 2005 Conjurer Services, S.A.
*/

#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "entity/nrefentityobject.h"
#include "util/narray.h"
#include "util/nsortedarray.h"
#include "ntrigger/ngameevent.h"
#include "kernel/nprofiler.h"

class ncTrigger;
class nScriptOperation;
class nTriggerStateCondition;

//------------------------------------------------------------------------------
class nTriggerServer : public nRoot
{
public:
    /// Default constructor
    nTriggerServer();
    /// Destructor
    virtual ~nTriggerServer();

    /// Return singleton trigger server
    static nTriggerServer* Instance();
    /// Tell if there exists an instance of the trigger server
    static bool IsValid();

    /// Do any required set up dependend of current level to begin updating the triggers
    void Start();
    /// Do any required clean up depended of current level to stop updating the triggers
    void Stop();
    /// Main loop
    void Update(const nGameEvent::Time&);
    /// Clear all triggers and events (triggers are not deleted, just removed)
    void Reset ();

    // -- Game events
    /// Emit a broadcast event (an event that reaches all triggers)
    nGameEvent::Id PostBroadcastEvent( nGameEvent::Type type, nEntityObjectId sourceId );
    /// Emit an area event (an event that reaches only to near triggers)
    nGameEvent::Id PostAreaEvent( const nGameEvent& event );
    /// Add an area event (only locally, doesn't add it to the emitter)
    nGameEvent::Id PostAreaEventUnsafe( const nGameEvent& event );
    /// Create a dummy area event entity and make it emit an event
    nEntityObject* PlaceAreaEvent( const vector3& position, const nGameEvent& event );
    /// Get an event by its event id
    nGameEvent* GetEvent( nGameEvent::Id eventId ) const;
    /// Delete an event, removing it from its emitter
    void DeleteEvent( nGameEvent::Id eventId );
    /// Delete an event, not removing it from its emitter
    void DeleteEventUnsafe( nGameEvent::Id eventId );

    // -- Triggers
    /// Register a trigger
    void RegisterTrigger(nEntityObject*);
    /// Remove a trigger (don't delete it)
    void RemoveTrigger(nEntityObject*);
    /// Queue an order to execute a trigger output after some delay time
    void QueueOutputExecution( nEntityObject* trigger, nGameEvent::Type triggerEvent, nGameEvent* eventSource, const nTime& executionDelay );
    /// Set the time between two consecutive updates of the same trigger
    void SetUpdateInterval( nGameEvent::Time interval );
    /// Get the time between two consecutive updates of the same trigger
    nGameEvent::Time GetUpdateInterval() const;
    /// Set the time between two consecutive updates of a perceivable cells list
    void SetPerceivableCellsUpdateInterval( nGameEvent::Time interval );
    /// Get the time between two consecutive updates of a perceivable cells list
    nGameEvent::Time GetPerceivableCellsUpdateInterval() const;
    // @todo Move the following methods to somewhere else (to a future motion server?)
    /// Set the time between two consecutive updates of a dynamic obstacles cells list
    void SetAvoidanceCellsUpdateInterval( int interval );
    /// Get the time between two consecutive updates of a dynamic obstacles cells list
    int GetAvoidanceCellsUpdateInterval() const;

    // -- Scripting interface
    /// Register an area event (version meant to be used from script side)
    nGameEvent::Id PostAreaEvent( const char* eventType, nEntityObject* source, nEntityObject* emitter, nGameEvent::Time duration, int priority );
    /// Create a dummy area event entity and make it emit an event (version meant to be used from script side)
    nEntityObject* PlaceAreaEvent( const vector3& position, const char* eventType, nEntityObject* source, nGameEvent::Time duration, int priority );

    // -- Persistence interface
    /// Return a script operation object, loading it if needed
    nScriptOperation* GetScriptOperation(const char*) const;
    /// Return a trigger condition script object, loading it if needed
    nTriggerStateCondition* GetTriggerCondition(const char*) const;

#ifndef NGAME
    // -- Editor interface
    /// Update only sound sources
    void UpdateSoundSources();

    /// Get the type of an event
    int GetEventType( nGameEvent::Id eventId ) const;
    /// Get the source entity id of an event
    nEntityObjectId GetEventSource( nGameEvent::Id eventId ) const;
    /// Get the duration of an event
    nGameEvent::Time GetEventDuration( nGameEvent::Id eventId ) const;
    /// Get the priority of an event
    int GetEventPriority( nGameEvent::Id eventId ) const;

    /// Get the number of available event types
    int GetEventTypesNumber() const;
    /// Tell if an event may be handled by a FSM
    bool IsAFsmInEvent(int) const;
    /// Tell if an event may be handled by an area trigger
    bool IsAnAreaTriggerInEvent(int) const;
    /// Tell if an event may be generated by an area trigger
    bool IsAnAreaTriggerOutEvent(int) const;
    /// Tell if an event may be handled by a generic trigger
    bool IsAGenericTriggerInEvent(int) const;
    /// Return the persistent id of an event type
    const char* GetEventPersistentId(int) const;
    /// Return the transient id of an event type
    int GetEventTransientId(const char*) const;
    /// Return the label of an event type
    const char* GetEventLabel(int) const;
#endif

    // -- Log
    /// Log used for errors
    static const int ErrorsLog;
    /// Log used for general information
    static const int GeneralLog;

private:
    /// Spread out the triggers updating homogenously among several frames
    void SpreadTriggersOverTime();
    /// Spread out the gathering of perceivable cells among several frames
    void SpreadPerceivableCellsOverTime();
    /// Clear all temporal data, like pending events and execution orders
    void ClearTempData();
    // @todo Move the following method to somewhere else (to a future motion server?)
    /// Spread out the gathering of near cells for steering behaviors among several frames
    void SpreadMotionCellsOverTime();

    /// Create a new event with default parameters
    nGameEvent* CreateEvent();
    /// Create a new event with user parameters
    nGameEvent* CreateEvent( const nGameEvent& eventDesc );
    /// Destroy an event, removing it from its emitter
    void DestroyEvent( nGameEvent::Id eventId, bool removeFromEmitter = true );

    /// Register an area event (an event that reaches only to near triggers)
    void RegisterAreaEvent( nGameEvent::Id eventId, bool addToEmitter = true );
    /// Destroy an event if found among the instant broadcast events
    bool DeleteInstantBroadcastEvent( nGameEvent::Id eventId );
    /// Destroy an event if found among the expirable area events
    bool DeleteExpirableAreaEvent( nGameEvent::Id eventId );
    /// Destroy an event if found among the permanent area events
    bool DeletePermanentAreaEvent( nGameEvent::Id eventId );
    /// Remove an event from the events groups
    void RemoveEventFromEventsGroups( nGameEvent::Id eventId );

    /// Delete expired events
    void ExpireEvents();
    /// Allow triggers to handle the broadcast events
    void ProcessBroadcastEvents();
    /// Allow triggers to handle the area events
    void ProcessAreaEvents();
    /// Get those event emitters within the trigger's culling radius
    void GetNearEmitters( ncTrigger* trigger, bool updateCells, nArray<nEntityObject*>& emitters );
    /// Execute those pending trigger outputs whose execution time has been reached
    void ExecuteTriggerOutputs();

    /// Build a NOH path from subpath and name (path = obj_root/subpath/name)
    void MakeObjPath( const char* subpath, const char* name, nString& resultingPath ) const;
    /// Return an object, creating it if needed
    nRoot* GetObject( const char* subpath, const char* name ) const;

    /// Singleton instance
    static nTriggerServer* instance;

    /// All the game events
    nKeyArray< nGameEvent* > gameEvents;
    /// All instant (broadcast) events in the game
    nArray< nGameEvent::Id > instantBroadcastEvents;
    /// All finite time area events in the game
    nArray< nGameEvent::Id > expirableAreaEvents;
    /// All infinite time area events in the game
    nArray< nGameEvent::Id > permanentAreaEvents;
    /// Id for the next event
    nGameEvent::Id nextEventId;

    /// Triggers that answer only to broadcast event
    nArray< nEntityObjectId > broadcastTriggers;
    /// Triggers that want to answer to area events (may answer to broadcast events as well)
    nArray< nEntityObjectId > areaTriggers;
    /// Current time
    nGameEvent::Time currentTime; // @todo Remove when a time server or a similar way to know the current time is available
    /// Time interval between two consecutive opportunities given to the same trigger to handle events
    nGameEvent::Time updateTimeInterval;

    /// Struct to hold information to execute a trigger output
    struct OutputExecutionDesc
    {
        /// Constructor
        OutputExecutionDesc() : sourceEventCopy(NULL)
        {
            // empty
        }
        /// Destructor
        ~OutputExecutionDesc()
        {
            // empty
        }

        /// Explicit destructor (to avoid accidentally destroying the source event)
        void Destroy()
        {
            if ( this->sourceEventCopy )
            {
                n_delete( sourceEventCopy );
            }
        }

        nEntityObjectId triggerId;
        nGameEvent::Type triggerEvent;
        nGameEvent* sourceEventCopy;
        nTime executionTime;

        /// Compare function needed for nSortedArray
        static int compare( const OutputExecutionDesc* desc1, const OutputExecutionDesc* desc2 )
        {
            if ( desc1->executionTime < desc2->executionTime )
            {
                return -1;
            }
            else if ( desc1->executionTime > desc2->executionTime )
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    };
    /// References to trigger outputs to execute at a delayed time
    nSortedArray< OutputExecutionDesc, OutputExecutionDesc::compare > pendingOutputExecutions;

    /// Time interval between two consecutive operations of gathering the spatial cells perceivable by a trigger
    nGameEvent::Time cellsUpdateTimeInterval;
    // @todo Move the following attribute to somewhere else (to a future motion server?)
    /// Time interval between consecutive updates of the dynamic obstacles cells list
    nGameEvent::Time avoidanceCellsUpdateInterval;

#ifdef __NEBULA_STATS__
    // Profilers
    nProfiler profUpdate;
    nProfiler profGatherAreaEvents;
    nProfiler profHandleAreaEvents;
#endif

#ifndef NGAME
    bool updatingSoundSources;
#endif
};

//-----------------------------------------------------------------------------
/**
    Return singleton trigger server
*/
inline
nTriggerServer*
nTriggerServer::Instance()
{
    n_assert( nTriggerServer::instance );
    return nTriggerServer::instance;
}

//-----------------------------------------------------------------------------
/**
    Tell if there exists an instance of the trigger server
*/
inline
bool
nTriggerServer::IsValid()
{
    return nTriggerServer::instance != NULL;
}

//-----------------------------------------------------------------------------
/**
    Get an event by its event id
*/
inline
nGameEvent*
nTriggerServer::GetEvent( nGameEvent::Id eventId ) const
{
    nGameEvent* event;
    if ( !this->gameEvents.Find( eventId, event ) )
    {
        return NULL;
    }
    return event;
}

//-----------------------------------------------------------------------------
/**
    Get the time between two consecutive updates of the same trigger
*/
inline
nGameEvent::Time
nTriggerServer::GetUpdateInterval() const
{
    return this->updateTimeInterval;
}

//-----------------------------------------------------------------------------
/**
    Get the time between two consecutive updates of a perceivable cells list
*/
inline
nGameEvent::Time
nTriggerServer::GetPerceivableCellsUpdateInterval() const
{
    return this->cellsUpdateTimeInterval;
}

//------------------------------------------------------------------------------
/**
    Get the time between two consecutive updates of a dynamic obstacles cells list
*/
inline
int
nTriggerServer::GetAvoidanceCellsUpdateInterval() const
{
    return this->avoidanceCellsUpdateInterval;
}

//------------------------------------------------------------------------------
#endif // N_TRIGGERSERVER_H
