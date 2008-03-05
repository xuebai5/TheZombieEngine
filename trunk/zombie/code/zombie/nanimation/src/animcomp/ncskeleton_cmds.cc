#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncskeleton_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/ncskeleton.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSkeleton)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
ncSkeleton::SaveCmds(nPersistServer *ps)
{
    if (nComponentObject::SaveCmds(ps))
    {
        return true;
    }
    return false;
}