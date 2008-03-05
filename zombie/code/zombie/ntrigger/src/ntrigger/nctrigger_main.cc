//-----------------------------------------------------------------------------
//  nctrigger_main.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "ntrigger/nctrigger.h"
#include "ntrigger/nctriggerclass.h"
#include "ntrigger/ntriggerserver.h"
#include "ntrigger/ncareatrigger.h"
#include "ncsound/ncsound.h"

//-----------------------------------------------------------------------------
nNebulaComponentObjectAbstract(ncTrigger,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncTrigger)
    NSCRIPT_ADDCMD_COMPOBJECT('ECEF', void, ClearAllEventFlags , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESFL', void, SetEventFlag, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGFL', bool, GetEventFlag, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESPE', void, SetPerceivableEvent, 2, (const char*, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGTO', bool, GetTriggerOnce , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESTO', void, SetTriggerOnce, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EGTE', bool, GetTriggerEnabled , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESTE', void, SetTriggerEnabled, 1, (bool), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    Default constructor
*/
ncTrigger::ncTrigger()
    : nextUpdateTime(0),
      cullingRadius(20),
      triggerOnce( false ),
      enabled( true )
{
    eventFlags.Init( nGameEvent::EVENTS_NUMBER );
}

//-----------------------------------------------------------------------------
/**
    Destructor
*/
ncTrigger::~ncTrigger()
{
    if ( nTriggerServer::IsValid() )
    {
        nTriggerServer::Instance()->RemoveTrigger( this->GetEntityObject() );
    }
}

//-----------------------------------------------------------------------------
/**
    Initializes the instance
*/
void 
ncTrigger::InitInstance(nObject::InitInstanceMsg initType)
{
    if ( initType == nObject::NewInstance )
    {
        // New instances take the default input events from the class
        ncTriggerClass* clazz( this->GetEntityClass()->GetComponent<ncTriggerClass>() );
        if ( clazz )
        {
            for ( int i(0); i < nGameEvent::EVENTS_NUMBER; ++i )
            {
                this->eventFlags.SetFlag( i, clazz->GetPerceivableEvent(i) );
            }
        }

        // Area triggers that aren't sound sources should have trigger once to true by default
        if ( this->GetComponent<ncAreaTrigger>() && !this->GetComponent<ncSound>() )
        {
            this->triggerOnce = true;
        }
    }

    nTriggerServer::Instance()->RegisterTrigger( this->GetEntityObject() );
}

//------------------------------------------------------------------------------
/**
    Deactivate itself as an answer to an activation + trigger once enabled
*/
void
ncTrigger::OnInternalEvent()
{
    // Disable the trigger if it must activate only once
    if ( this->triggerOnce )
    {
        this->SetTriggerEnabled( false );
    }
}

//------------------------------------------------------------------------------
/**
    Set all the event flags to false
*/
void
ncTrigger::ClearAllEventFlags()
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
ncTrigger::SetPerceivableEvent( const char* persistentId, bool perceivable )
{
    this->SetEventFlag( nGameEvent::GetEventTransientId(persistentId), perceivable );
}

//------------------------------------------------------------------------------
/**
    Tell if this trigger activates once or multiple times
*/
bool
ncTrigger::GetTriggerOnce() const
{
    return this->triggerOnce;
}

//------------------------------------------------------------------------------
/**
    Set this trigger to activate once or multiple times
*/
void
ncTrigger::SetTriggerOnce( bool triggerOnce )
{
    this->triggerOnce = triggerOnce;
}

//------------------------------------------------------------------------------
/**
    Enable this trigger to accept input or disable it
*/
void
ncTrigger::SetTriggerEnabled( bool enabled )
{
    this->enabled = enabled;
}

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool
ncTrigger::SaveCmds( nPersistServer* ps )
{
    if ( nComponentObject::SaveCmds(ps) )
    {
        // Clear all event flags
        ps->Put( this->GetEntityObject(), 'ECEF' );

        // Set enabled event flags
        for ( int i(0); i < nGameEvent::EVENTS_NUMBER; ++i )
        {
            if ( this->eventFlags.IsFlagEnabled(i) )
            {
                ps->Put( this->GetEntityObject(), 'ESPE', nGameEvent::GetEventPersistentId( nGameEvent::Type(i) ), true );
            }
        }

        // Trigger once
        ps->Put( this->GetEntityObject(), 'ESTO', this->triggerOnce );

        // Enabled
        ps->Put( this->GetEntityObject(), 'ESTE', this->enabled );
    }

    return true;
}

//-----------------------------------------------------------------------------
