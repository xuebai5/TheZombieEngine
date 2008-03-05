#ifndef N_GAMEEVENT_H
#define N_GAMEEVENT_H

//------------------------------------------------------------------------------
/**
    @class nGameEvent
    @ingroup NebulaTriggerSystem

    Events related to the game and handled by the trigger system.
*/

#include "entity/nentityobject.h"
#include "mathlib/vector.h"

//------------------------------------------------------------------------------
struct nGameEvent
{
    /// Event id type
    typedef unsigned int Id;
    /// Invalid event code
    static const Id INVALID_ID = 0;

    /// Event type enum
    enum Type
    {
        // When adding/removing/sorting event types remember to update the event type info as well
        // (event type info is placed in an array found in ngameevent.cc)
        INVALID_TYPE,
        ACTION_DONE,
        ACTION_FAIL,
        PLAYER,
        SQUAD_MEMBER,
/** ZOMBIE REMOVE
        SCAVENGER,
        STRIDER,
*/
        ENTER_IN_AREA,
        EXIT_FROM_AREA,
        ACTION_ISSUED,
        TRIGGER_ACTIVATION,
        HUMAN_SOUND,
        ALIEN_SOUND,
        TARGET_LOST,
        DEAD_HUMAN,
        DEAD_ALIEN,
        SHOOT_SOUND,
        EXPLOSION_SOUND,
        IMPACT_SOUND,
        HIT,
        VEHICLE,
        VEHICLE_SOUND,

        // This must be the last event, so add new events any place before this one
        EVENTS_NUMBER
    };
    /// Time in ticks
    typedef unsigned int Time;

    /// Struct to hold some type info about an event
    struct Info
    {
        /// Enum of the components that may handle/generate an event (postfix in=handle, out=generate)
        enum Scope
        {
            NO_SCOPE = 0,
            FSM_IN = 1<<0,
            AREA_TRIGGER_IN = 1<<1,
            AREA_TRIGGER_OUT = 1<<2,
            /* Action trigger in = area trigger in */
            ACTION_TRIGGER_OUT = 1<<3,
            GENERIC_TRIGGER_IN = 1<<4,
            GENERIC_TRIGGER_OUT = 1<<5,
            FSM_AND_AREA_TRIGGER_IN = FSM_IN | AREA_TRIGGER_IN,
            ALL_TRIGGERS_IN = AREA_TRIGGER_IN | GENERIC_TRIGGER_IN,
            ALL_IN = FSM_IN | ALL_TRIGGERS_IN,
            ALL = ~0
        };
        /// Enum of the senses that may perceive the event
        enum Sensation
        {
            NO_SENSATION = 0,
            VISIBLE = 1<<0,
            AUDIBLE = 1<<1,
            TOUCHABLE = 1<<2,
            VIEW_AND_TOUCH = VISIBLE | TOUCHABLE,
            VIEW_AND_TOUCH_AND_HEAR = VISIBLE | TOUCHABLE | AUDIBLE
        };
        // String used as an unmutable id over project life for this event
        const char* id;
        // Event name showed to the user
        const char* label;
        // Components that may handle/generate this event
        Scope scope;
        // Senses that may perceive the event
        Sensation sensation;
    };
    /// Return the mutable id of an event by its unmutable id
    static Type GetEventTransientId( const char* idName );
    /// Return the unmutable id of an event
    static const char* GetEventPersistentId( Type eventType );
    /// Return the label of an event
    static const char* GetEventLabel( Type eventType );
    /// Return the scope of an event
    static Info::Scope GetEventScope( Type eventType );
    /// Return the sensation type of an event
    static Info::Sensation GetEventSensation( Type eventType );
#ifndef NGAME
    /// Get a string description of the event
    void ToString( nString& str ) const;
#endif

    /// Special properties
    enum Properties
    {
        NO_PROPERTIES = 0,
        AUTODESTROY_EMITTER = 1 // When the event expires, the emitter should be automatically destroyed
    };

    /// Default constructor
    nGameEvent();
    /// Constructor for broadcast events
    nGameEvent( Type eventType, nEntityObjectId sourceId );
    /// Constructor for area events
    nGameEvent( Type eventType, nEntityObjectId sourceId, nEntityObjectId emitterId, Time duration, int priority, Properties properties );
    /// Destructor
    ~nGameEvent();

    /// Set the event id
    void SetId( Id eventId );
    /// Get the event id
    Id GetId() const;
    /// Set the event type
    void SetType( Type eventType );
    /// Get the even type
    Type GetType() const;
    /// Set the event type by its persistent type id
    void SetType( const char* eventType );
    /// Get the event type as a persistent type id
    const char* GetPersistentType() const;
    /// Set the source entity id
    void SetSourceEntity( nEntityObjectId sourceId );
    /// Get the source entity id
    nEntityObjectId GetSourceEntity() const;
    /// Set the emitter entity id
    void SetEmitterEntity( nEntityObjectId emitterId );
    /// Get the emitter entity id
    nEntityObjectId GetEmitterEntity() const;
    /// Set the duration of the event
    void SetDuration( Time duration );
    /// Get the duration of the event
    Time GetDuration() const;
    /// Tell if the event expires
    bool Expires() const;
    /// Set the event priority
    void SetPriority( int priority );
    /// Get the event priority
    int GetPriority() const;
    /// Set event properties
    void SetPorperties( Properties properties );
    /// Get event properties
    Properties GetProperties() const;

    /// Start emitting the event
    void StartEvent( Time startTime );
    /// Get the time the event started
    Time GetStartTime() const;
    /// Get the time the event will expire
    Time GetExpirationTime() const;

private:
    /// Event id
    Id eventId;
    /// Event type id
    Type eventType;
    /// Id of the entity that generated this event
    nEntityObjectId sourceId;
    /// Id of the entity that emits this event
    nEntityObjectId emitterId;
    /// Priority of this event
    int priority;
    /// Special properties of this event
    Properties properties;

    /// Life time of the event (0 for infite duration)
    Time duration;
    /// Time when this event started
    Time startTime;
    /// Time when this event will expire (0 = event doesn't expire)
    Time expirationTime;
};

//------------------------------------------------------------------------------
/**
    Default constructor
*/
inline
nGameEvent::nGameEvent() :
    eventId( INVALID_ID ),
    eventType( INVALID_TYPE ),
    sourceId( 0 ),
    emitterId( 0 ),
    duration( 0 ),
    priority( 0 ),
    properties( NO_PROPERTIES )
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Constructor for broadcast events
*/
inline
nGameEvent::nGameEvent( Type eventType, nEntityObjectId sourceId )
    : eventId( INVALID_ID ),
      eventType( eventType ),
      sourceId( sourceId )
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Constructor for area events
*/
inline
nGameEvent::nGameEvent( Type eventType, nEntityObjectId sourceId, nEntityObjectId emitterId, Time duration, int priority, Properties properties )
    : eventId( INVALID_ID ),
      eventType( eventType ),
      sourceId( sourceId ),
      emitterId( emitterId ),
      duration( duration ),
      priority( priority ),
      properties( properties )
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
inline
nGameEvent::~nGameEvent()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Set the event id
    
    Do not set this yourself, the trigger server will override it
*/
inline
void
nGameEvent::SetId( Id eventId )
{
    this->eventId = eventId;
}

//------------------------------------------------------------------------------
/**
    Get the event id
*/
inline
nGameEvent::Id
nGameEvent::GetId() const
{
    return this->eventId;
}

//------------------------------------------------------------------------------
/**
    Set the event type
*/
inline
void
nGameEvent::SetType( Type eventType )
{
    this->eventType = eventType;
}

//------------------------------------------------------------------------------
/**
    Get the event type
*/
inline
nGameEvent::Type
nGameEvent::GetType() const
{
    return this->eventType;
}

//------------------------------------------------------------------------------
/**
    Set the event type by its persistent type id
*/
inline
void
nGameEvent::SetType( const char* eventType )
{
    this->eventType = GetEventTransientId( eventType );
}

//------------------------------------------------------------------------------
/**
    Get the event type as a persistent type id
*/
inline
const char*
nGameEvent::GetPersistentType() const
{
    return GetEventPersistentId( this->eventType );
}

//------------------------------------------------------------------------------
/**
    Set the source entity id
*/
inline
void
nGameEvent::SetSourceEntity( nEntityObjectId sourceId )
{
    this->sourceId = sourceId;
}

//------------------------------------------------------------------------------
/**
    Get the source entity id
*/
inline
nEntityObjectId
nGameEvent::GetSourceEntity() const
{
    return this->sourceId;
}

//------------------------------------------------------------------------------
/**
    Set the emitter entity id
*/
inline
void
nGameEvent::SetEmitterEntity( nEntityObjectId emitterId )
{
    this->emitterId = emitterId;
}

//------------------------------------------------------------------------------
/**
    Get the emitter entity id
*/
inline
nEntityObjectId
nGameEvent::GetEmitterEntity() const
{
    return this->emitterId;
}

//------------------------------------------------------------------------------
/**
    Set the duration of the event
    
    0 means infinite duration
*/
inline
void
nGameEvent::SetDuration( Time duration )
{
    this->duration = duration;
}

//------------------------------------------------------------------------------
/**
    Get the duration of the event
*/
inline
nGameEvent::Time
nGameEvent::GetDuration() const
{
    return this->duration;
}

//------------------------------------------------------------------------------
/**
    Tell if the event expires
    
    More readable shortcut for GetDuration() > 0
*/
inline
bool
nGameEvent::Expires() const
{
    return this->duration > 0;
}

//------------------------------------------------------------------------------
/**
    Set the event priority
*/
inline
void
nGameEvent::SetPriority( int priority )
{
    this->priority = priority;
}

//------------------------------------------------------------------------------
/**
    Get the event priority
*/
inline
int
nGameEvent::GetPriority() const
{
    return this->priority;
}

//------------------------------------------------------------------------------
/**
    Set event properties
*/
inline
void
nGameEvent::SetPorperties( Properties properties )
{
    this->properties = properties;
}

//------------------------------------------------------------------------------
/**
    Get event properties
*/
inline
nGameEvent::Properties
nGameEvent::GetProperties() const
{
    return this->properties;
}

//------------------------------------------------------------------------------
/**
    Get the time the event started
    
    The value returned is only valid after calling to StartEvent
*/
inline
nGameEvent::Time
nGameEvent::GetStartTime() const
{
    return this->startTime;
}

//------------------------------------------------------------------------------
/**
    Get the time the event will expire
    
    The value returned is only valid after calling to StartEvent
*/
inline
nGameEvent::Time
nGameEvent::GetExpirationTime() const
{
    return this->expirationTime;
}

//------------------------------------------------------------------------------

N_CMDARGTYPE_NEW_TYPE(nGameEvent::Type, "i", (value = (nGameEvent::Type) cmd->In()->GetI()), (cmd->Out()->SetI(value))  );

#endif // !N_GAMEEVENT_H
