#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncragdoll_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nchumragdoll.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncHumRagdoll)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
ncHumRagdoll::SaveCmds(nPersistServer *ps)
{
    if (ncRagDoll::SaveCmds(ps))
    {
        return true;
    }
    return false;
}