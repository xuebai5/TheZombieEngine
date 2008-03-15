//-----------------------------------------------------------------------------
//  nfsmselector_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnfsm.h"
#include "nfsm/nfsmselector.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nFSMSelector )

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Nebula class persistence
*/
bool nFSMSelector::SaveCmds( nPersistServer* /*ps*/ )
{
    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
