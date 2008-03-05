#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nskinragdollanimator_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nskinragdollanimator.h"
#include "kernel/npersistserver.h"

NSCRIPT_INITCMDS_BEGIN(nSkinRagdollAnimator)    
    NSCRIPT_ADDCMD('SRDJ', void, SetRagdollJoint, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('GRDJ', int, GetRagdollJoint, 1, (int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nSkinRagdollAnimator::SaveCmds(nPersistServer* ps)
{
    if (nSkinGeometryAnimator::SaveCmds(ps))
    {
        //nCmd* cmd;

        // --- xxx ---
        // ...

        return true;
    }
    return false;
}
