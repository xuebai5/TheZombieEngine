//-----------------------------------------------------------------------------
//  ncphysea_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphysea.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhySea)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool 
ncPhySea::SaveCmds(nPersistServer* ps)
{
    return ncPhyAreaDensity::SaveCmds( ps );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
