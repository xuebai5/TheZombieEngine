//------------------------------------------------------------------------------
//  ncphyindoor_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyindoor.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyIndoor)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool ncPhyIndoor::SaveCmds(nPersistServer* /*ps*/)
{
    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------