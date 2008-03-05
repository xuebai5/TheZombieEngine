#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntransformswitchanimator_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ntransformswitchanimator.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nTransformSwitchAnimator)
    NSCRIPT_ADDCMD('APOS', void, AddPosition, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('AEUL', void, AddEuler, 1, (const vector3&), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nTransformSwitchAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- addposition ---
        int i;
        for (i = 0; i < this->positionArray.Size(); ++i)
        {
            cmd = ps->GetCmd(this, 'APOS');
            const vector3& pos = this->positionArray.At(i);
            cmd->In()->SetF(pos.x);
            cmd->In()->SetF(pos.y);
            cmd->In()->SetF(pos.z);
            ps->PutCmd(cmd);
        }

        // --- addrotation ---
        for (i = 0; i < this->rotationArray.Size(); ++i)
        {
            cmd = ps->GetCmd(this, 'AEUL');
            const vector3& euler = this->rotationArray.At(i);
            cmd->In()->SetF(euler.x);
            cmd->In()->SetF(euler.y);
            cmd->In()->SetF(euler.z);
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
