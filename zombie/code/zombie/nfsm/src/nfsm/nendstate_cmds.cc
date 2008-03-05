//-----------------------------------------------------------------------------
//  nendstate_cmds.cc
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nendstate.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nEndState )
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool nEndState::SaveCmds( nPersistServer* ps )
{
    return nState::SaveCmds(ps);
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
