#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nrenderpathnode_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nrenderpathnode.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nRenderPathNode)
    NSCRIPT_ADDCMD('SRPF', void, SetRenderPathFilename, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('GRPF', const nString&, GetRenderPathFilename, 0, (), 0, ());
    NSCRIPT_ADDCMD('SOLL', void, SetObeyLightLinks, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GPSL', bool, GetObeyLightLinks, 0, (), 0, ());
    NSCRIPT_ADDCMD('SMLP', void, SetMaxMaterialLevel, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GMLP', int, GetMaxMaterialLevel, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nRenderPathNode::SaveCmds(nPersistServer* ps)
{
    if (nSceneNode::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- setrenderpathfilename ---
        cmd = ps->GetCmd(this, 'SRPF');
        cmd->In()->SetS(this->GetRenderPathFilename().Get());
        ps->PutCmd(cmd);

        // --- setobeylightlinks ---
        if (this->GetObeyLightLinks())
        {
            cmd = ps->GetCmd(this, 'SOLL');
            cmd->In()->SetB(this->GetObeyLightLinks());
            ps->PutCmd(cmd);
        }

        // --- setmaxmateriallevel ---
        cmd = ps->GetCmd(this, 'SMLP');
        cmd->In()->SetI(this->GetMaxMaterialLevel());
        ps->PutCmd(cmd);
 
        return true;
    }
    return false;
}
