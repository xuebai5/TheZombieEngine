//-----------------------------------------------------------------------------
//  neventcondition_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/neventcondition.h"
#include "nfsmserver/nfsmserver.h"
#include "ntrigger/ngameevent.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nEventCondition )
    NSCRIPT_ADDCMD('SETE', void, SetEvent, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SEBI', void, SetEventByPersistentId, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('GETE', int, GetEvent , 0, (), 0, ());
    NSCRIPT_ADDCMD('SETF', void, SetFilterCondition, 1, (nCondition*), 0, ());
    NSCRIPT_ADDCMD('GETF', nCondition*, GetFilterCondition , 0, (), 0, ());
    NSCRIPT_ADDCMD('EVAL', bool, Evaluate, 1, (nEntityObject*), 0, ());
    NSCRIPT_ADDCMD('ESEC', void, SetEventFilterCondition, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('ESSC', void, SetScriptFilterCondition, 1, (const char*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool nEventCondition::SaveCmds( nPersistServer* ps )
{
    // Event
    ps->Put( this, 'SEBI', nGameEvent::GetEventPersistentId( nGameEvent::Type(this->eventType) ) );

    // Filter condition
    if ( this->filterCondition )
    {
        switch ( this->filterCondition->GetConditionType() )
        {
            case nCondition::Event:
                ps->Put( this, 'ESEC', this->filterCondition->GetName() );
                break;
            case nCondition::Script:
                ps->Put( this, 'ESSC', this->filterCondition->GetName() );
                break;
            default:
                n_error( "It's just impossible to reach this line?!?! Maybe has a new condition type been added?" );
                return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
