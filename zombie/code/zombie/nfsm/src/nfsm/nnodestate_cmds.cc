//-----------------------------------------------------------------------------
//  nnodestate_cmds.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nnodestate.h"
#include "nfsm/nfsm.h"
#include "nfsm/nfsmselector.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nNodeState )
    NSCRIPT_ADDCMD('SETS', void, SetFSMSelector, 1, (nFSMSelector*), 0, ());
    NSCRIPT_ADDCMD('GETS', nFSMSelector*, GetFSMSelector , 0, (), 0, ());
    NSCRIPT_ADDCMD('ESFS', void, SetFSMSelectorByName, 1, (const char*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool nNodeState::SaveCmds( nPersistServer* ps )
{
    if ( !this->fsmSelector )
    {
        return false;
    }
    ps->Put( this, 'ESFS', this->fsmSelector->GetName() );

    return nState::SaveCmds(ps);
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
