#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncragdollclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/ncfourleggedragdollclass.h"
#include "ncragdoll/ncragdollclass.h"

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncFourLeggedRagdollClass)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
ncFourLeggedRagdollClass::SaveCmds(nPersistServer *ps)
{
    if (ncRagDollClass::SaveCmds(ps))
    {
        return true;
    }
    return false;
}
