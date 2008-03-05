#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncragdollclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nchumragdollclass.h"
#include "ncragdoll/ncragdollclass.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncHumRagdollClass)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
ncHumRagdollClass::SaveCmds(nPersistServer *ps)
{
    if (ncRagDollClass::SaveCmds(ps))
    {
        return true;
    }
    return false;
}
