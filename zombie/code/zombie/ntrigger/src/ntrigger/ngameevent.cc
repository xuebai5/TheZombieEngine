//-----------------------------------------------------------------------------
//  ngameevent.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ngameevent.h"
#include "ntrigger/ntriggerserver.h"

//-----------------------------------------------------------------------------
#define GameEvent nGameEvent::Info

// Type info about all the events
static nGameEvent::Info eventTypeInfo[ nGameEvent::EVENTS_NUMBER ] =
{
    // The event persistent name must only have letters, numbers and underscore
    // The event persistent name mustn't change if backwards compatibility is desired
    // This table must be in the same order as nGameEvent::Type
    { "invalid_event",  "<invalid>",          GameEvent::NO_SCOPE,                    GameEvent::NO_SENSATION },
    { "done",           "Done",               GameEvent::FSM_IN,                      GameEvent::NO_SENSATION },
    { "fail",           "Fail",               GameEvent::FSM_IN,                      GameEvent::NO_SENSATION },
    { "player",         "Player",             GameEvent::FSM_AND_AREA_TRIGGER_IN,     GameEvent::VIEW_AND_TOUCH },
    { "squad_member",   "Squad member",       GameEvent::FSM_AND_AREA_TRIGGER_IN,     GameEvent::VIEW_AND_TOUCH },
    { "enter_area",     "Enter in area",      GameEvent::AREA_TRIGGER_OUT,            GameEvent::NO_SENSATION },
    { "exit_area",      "Exit from area",     GameEvent::AREA_TRIGGER_OUT,            GameEvent::NO_SENSATION },
    { "action issued",  "Action issued",      GameEvent::ACTION_TRIGGER_OUT,          GameEvent::NO_SENSATION },
    { "trigger_activation", "Activation",     GameEvent::GENERIC_TRIGGER_OUT,         GameEvent::NO_SENSATION },
    { "human_sound",    "Human sound",        GameEvent::ALL_IN,                      GameEvent::AUDIBLE },
    { "target_lost",    "Target lost",        GameEvent::FSM_IN,                      GameEvent::NO_SENSATION },
    { "dead_human",     "Dead human",         GameEvent::ALL_IN,                      GameEvent::VIEW_AND_TOUCH_AND_HEAR },
    { "shoot_sound",    "Shoot sound",        GameEvent::ALL_IN,                      GameEvent::AUDIBLE },
    { "explosion_sound","Explosion sound",    GameEvent::ALL_IN,                      GameEvent::AUDIBLE },
    { "impact_sound",   "Impact sound",       GameEvent::ALL_IN,                      GameEvent::AUDIBLE },
    { "hit",            "Hit",                GameEvent::ALL_IN,                      GameEvent::NO_SENSATION },
    { "vehicle",        "Vehicle",            GameEvent::ALL_IN,                      GameEvent::VIEW_AND_TOUCH },
    { "vehicle_sound",  "Vehicle sound",      GameEvent::ALL_IN,                      GameEvent::AUDIBLE }
};

#undef GameEvent

//------------------------------------------------------------------------------
/**
    Start emitting the event
*/
void
nGameEvent::StartEvent( Time startTime )
{
    this->startTime = startTime;
    this->expirationTime = startTime + this->duration;
}

//-----------------------------------------------------------------------------
/**
    Return the mutable id of an event by its unmutable id
*/
nGameEvent::Type
nGameEvent::GetEventTransientId( const char* idName )
{
    nString strName( idName );
    for ( int i(0); i < EVENTS_NUMBER; ++i )
    {
        if ( strName == eventTypeInfo[i].id )
        {
            return Type(i);
        }
    }
    NLOG( trigger, (nTriggerServer::ErrorsLog | 0,
        "Found a reference to the unknown event '%s'", idName) );
    return INVALID_TYPE;
}

//-----------------------------------------------------------------------------
/**
    Return the unmutable id of an event
*/
const char*
nGameEvent::GetEventPersistentId( Type eventType )
{
    n_assert( eventType >= 0 && eventType < nGameEvent::EVENTS_NUMBER );
    return eventTypeInfo[ eventType ].id;
}

//-----------------------------------------------------------------------------
/**
    Return the label of an event
*/
const char*
nGameEvent::GetEventLabel( Type eventType )
{
    n_assert( eventType >= 0 && eventType < nGameEvent::EVENTS_NUMBER );
    return eventTypeInfo[ eventType ].label;
}

//-----------------------------------------------------------------------------
/**
    Return the scope of an event
*/
nGameEvent::Info::Scope
nGameEvent::GetEventScope( Type eventType )
{
    n_assert( eventType >= 0 && eventType < nGameEvent::EVENTS_NUMBER );
    return eventTypeInfo[ eventType ].scope;
}

//-----------------------------------------------------------------------------
/**
    Return the sensation type of an event
*/
nGameEvent::Info::Sensation
nGameEvent::GetEventSensation( Type eventType )
{
    n_assert( eventType >= 0 && eventType < nGameEvent::EVENTS_NUMBER );
    return eventTypeInfo[ eventType ].sensation;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Get a string description of the event
*/
void
nGameEvent::ToString( nString& str ) const
{
    str = "id=" + nString( int(this->eventId) ) \
        + ", type=" + nString( nGameEvent::GetEventPersistentId(this->eventType) ) \
        + ", source=" + nString( int(this->sourceId) ) \
        + ", emitter=" + nString( int(this->emitterId) ) \
        + ", begin=" + nString( int(this->startTime) ) \
        + ", end=" + nString( int(this->expirationTime) ) \
        + ", priority=" + nString( this->priority ) \
        + ", properties=" + nString(properties);
}
#endif
