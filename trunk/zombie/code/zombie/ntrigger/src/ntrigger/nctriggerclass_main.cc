//-----------------------------------------------------------------------------
//  nctriggerclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/nctriggerclass.h"
#include "ntrigger/ngameevent.h"

//-----------------------------------------------------------------------------
nNebulaComponentClass(ncTriggerClass, nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTriggerClass)
    NSCRIPT_ADDCMD_COMPCLASS('ECPE', void, ClearAllPerceivableEvents , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('ESPE', void, SetPerceivableEvent, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EGPE', bool, GetPerceivableEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('EPEI', void, SetPerceivableEventByPersistendId, 2, (const char*, bool), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
   Default constructor
*/
ncTriggerClass::ncTriggerClass()
{
    eventFlags.Init( nGameEvent::EVENTS_NUMBER );
}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
ncTriggerClass::~ncTriggerClass()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Set all the event flags to false
*/
void
ncTriggerClass::ClearAllPerceivableEvents()
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
ncTriggerClass::SetPerceivableEventByPersistendId( const char* persistentId, bool perceivable )
{
    this->SetPerceivableEvent( nGameEvent::GetEventTransientId(persistentId), perceivable );
}

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncTriggerClass::SaveCmds( nPersistServer* ps )
{
    if ( nComponentClass::SaveCmds(ps) )
    {
        // Clear all event flags
        ps->Put( this->GetEntityClass(), 'ECPE' );

        // Set enabled event flags
        for ( int i(0); i < nGameEvent::EVENTS_NUMBER; ++i )
        {
            if ( this->eventFlags.IsFlagEnabled(i) )
            {
                ps->Put( this->GetEntityClass(), 'EPEI', nGameEvent::GetEventPersistentId( nGameEvent::Type(i) ), true );
            }
        }
    }

    return true;
}
