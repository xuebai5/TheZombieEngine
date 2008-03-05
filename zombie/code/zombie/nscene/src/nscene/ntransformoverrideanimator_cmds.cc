#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ntransformoverrideanimator_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ntransformoverrideanimator.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nTransformOverrideAnimator)
    NSCRIPT_ADDCMD('SPCH', void, SetPositionChannel, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('SRCH', void, SetRotationChannel, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nTransformOverrideAnimator::SaveCmds(nPersistServer* ps)
{
    if (nAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- setpositionchannel ---
        if (this->positionChannel != nVariable::InvalidHandle)
        {
            cmd = ps->GetCmd(this, 'SRCH');
            cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(this->positionChannel));
            ps->PutCmd(cmd);
        }

        // --- setrotationchannel ---
        if (this->rotationChannel != nVariable::InvalidHandle)
        {
            cmd = ps->GetCmd(this, 'SRCH');
            cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(this->rotationChannel));
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
