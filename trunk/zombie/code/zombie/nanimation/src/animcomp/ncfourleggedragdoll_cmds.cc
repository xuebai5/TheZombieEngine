#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncragdoll_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/ncfourleggedragdoll.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncFourLeggedRagdoll)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
ncFourLeggedRagdoll::SaveCmds(nPersistServer *ps)
{
    if (ncRagDoll::SaveCmds(ps))
    {
        return true;
    }
    return false;
}