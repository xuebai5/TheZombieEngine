//-----------------------------------------------------------------------------
//  ntransition_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/ntransition.h"
#include "nfsm/neventcondition.h"
#include "nfsm/nstate.h"
#include "nfsm/nfsm.h"
#include "nfsmserver/nfsmserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nTransition )
    NSCRIPT_ADDCMD('SETC', void, SetCondition, 1, (nCondition*), 0, ());
    NSCRIPT_ADDCMD('GETC', nCondition*, GetCondition , 0, (), 0, ());
    NSCRIPT_ADDCMD('ADDT', void, AddTarget, 2, (nState*, int), 0, ());
    NSCRIPT_ADDCMD('REMT', void, RemoveTarget, 1, (nState*), 0, ());
    NSCRIPT_ADDCMD('SETP', void, SetTargetProbability, 2, (const nState*, int), 0, ());
    NSCRIPT_ADDCMD('GNOT', int, GetTargetsNumber , 0, (), 0, ());
    NSCRIPT_ADDCMD('GTBI', nState*, GetTargetStateByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SSBI', void, SetTargetStateByIndex, 2, (int, nState*), 0, ());
    NSCRIPT_ADDCMD('GPBI', int, GetTargetProbabilityByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SPBI', void, SetTargetProbabilityByIndex, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('RTBI', void, RemoveTargetByIndex, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ESEC', void, SetEventCondition, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('ESSC', void, SetScriptCondition, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('ESFC', void, SetFilterCondition, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EATN', void, AddTargetByName, 4, (const char*, const char*, int, int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool nTransition::SaveCmds( nPersistServer* ps )
{
    // Condition
    if ( !this->condition )
    {
        return false;
    }
    switch ( this->condition->GetConditionType() )
    {
        case nCondition::Event:
            if ( static_cast<nEventCondition*>(this->condition)->GetFilterCondition() )
            {
                // Event conditions with filter are local to the transition
                ps->Put( this, 'ESFC', this->condition->GetName() );
            }
            else
            {
                // Event conditions without filter are shared among transitions
                ps->Put( this, 'ESEC', this->condition->GetName() );
            }
            break;
        case nCondition::Script:
            ps->Put( this, 'ESSC', this->condition->GetName() );
            break;
        default:
            n_error( "It's just impossible to reach this line?!?! Maybe has a new condition type been added?" );
            return false;
    }

    // Targets
    for ( int i(0); i < this->targets.Size(); ++i )
    {
        nState* state = this->targets[i].state;
        nFSM* fsm = nFSMServer::Instance()->FindFSMOfState( state );
        ps->Put( this, 'EATN', fsm->GetName(), state->GetName(), state->GetStateType(), this->targets[i].probability );
    }

    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
