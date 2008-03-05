//-----------------------------------------------------------------------------
//  ncareaeventclass_main.cc
//  (C) 2006 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/ncareaeventclass.h"
#include "ntrigger/ngameevent.h"

//-----------------------------------------------------------------------------
nNebulaComponentClass(ncAreaEventClass, nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncAreaEventClass)
    NSCRIPT_ADDCMD_COMPCLASS('ECEE', void, ClearAllEmittedEvent , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ESEE', void, SetEmittedEvent, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGEE', bool, GetEmittedEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EEEI', void, SetEmittedEventByPersistendId, 2, (const char*, bool), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
   Default constructor
*/
ncAreaEventClass::ncAreaEventClass()
{
    eventFlags.Init( nGameEvent::EVENTS_NUMBER );
}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
ncAreaEventClass::~ncAreaEventClass()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Set all the event flags to false
*/
void
ncAreaEventClass::ClearAllEmittedEvent()
{
    for ( int i(0); i < nGameEvent::EVENTS_NUMBER; ++i )
    {
        this->eventFlags.DisableFlag( i );
    }
}

//------------------------------------------------------------------------------
/**
    Set the value of an event flag by its persistent id (used for persistence)
*/
void
ncAreaEventClass::SetEmittedEventByPersistendId( const char* persistentId, bool perceivable )
{
    this->SetEmittedEvent( nGameEvent::GetEventTransientId(persistentId), perceivable );
}

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncAreaEventClass::SaveCmds( nPersistServer* ps )
{
    if ( nComponentClass::SaveCmds(ps) )
    {
        // Clear all event flags
        ps->Put( this->GetEntityClass(), 'ECEE' );

        // Set enabled event flags
        for ( int i(0); i < nGameEvent::EVENTS_NUMBER; ++i )
        {
            if ( this->eventFlags.IsFlagEnabled(i) )
            {
                ps->Put( this->GetEntityClass(), 'EEEI', nGameEvent::GetEventPersistentId( nGameEvent::Type(i) ), true );
            }
        }
    }

    return true;
}
