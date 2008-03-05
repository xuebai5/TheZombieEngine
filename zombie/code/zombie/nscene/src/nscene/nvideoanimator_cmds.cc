#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nvideoanimator_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nvideoanimator.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nVideoAnimator)
    NSCRIPT_ADDCMD('SSCH', void, SetStateChannel, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GSCH', const char *, GetStateChannel, 0, (), 0, ());
    NSCRIPT_ADDCMD('SVFL', void, SetVideoFile, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GVFL', const char *, GetVideoFile, 0, (), 0, ());
    //NSCRIPT_ADDCMD('XXXX', void, SetXXX, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nVideoAnimator::SaveCmds(nPersistServer* ps)
{
    if (nShaderAnimator::SaveCmds(ps))
    {
        nCmd* cmd;

        //--- setvideofile ---
        if (this->GetVideoFile())
        {
            cmd = ps->GetCmd(this, 'SVFL');
            cmd->In()->SetS(this->GetVideoFile());
            ps->PutCmd(cmd);
        }

        //--- setstatechannel ---
        if (this->GetStateChannel())
        {
            cmd = ps->GetCmd(this, 'SSCH');
            cmd->In()->SetS(this->GetStateChannel());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}
