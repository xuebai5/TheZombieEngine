#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nlensflarenode_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nlensflarenode.h"

//------------------------------------------------------------------------------
/**
*/
NSCRIPT_INITCMDS_BEGIN(nLensFlareNode)
    NSCRIPT_ADDCMD('SBCL', void, SetBaseColor, 1, (const vector4&), 0, ());
    NSCRIPT_ADDCMD('GBCL', const vector4&, GetBaseColor, 0, (), 0, ());
    NSCRIPT_ADDCMD('SBLC', void, SetBlindColor, 1, (const vector4&), 0, ());
    NSCRIPT_ADDCMD('GBLC', const vector4&, GetBlindColor, 0, (), 0, ());
    NSCRIPT_ADDCMD('BFLR', void, BeginFlares, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SFS_', void, SetFlareSizeAt, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('GFS_', float, GetFlareSizeAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SFPA', void, SetFlarePosAt, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('GFPA', float, GetFlarePosAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD('SFCA', void, SetFlareColorAt, 2, (int, const vector4&), 0, ());
    NSCRIPT_ADDCMD('GFCA', const vector4&, GetFlareColorAt, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EFLR', void, EndFlares, 0, (), 0, ());
    NSCRIPT_ADDCMD('GNOF', int, GetNumFlares, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nLensFlareNode::SaveCmds(nPersistServer* ps)
{
    if (nGeometryNode::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- setbasecolor ---
        cmd = ps->GetCmd(this, 'SBCL');
        cmd->In()->SetF(this->baseColor.x);
        cmd->In()->SetF(this->baseColor.y);
        cmd->In()->SetF(this->baseColor.z);
        cmd->In()->SetF(this->baseColor.w);
        ps->PutCmd(cmd);

        // --- setblindcolor ---
        cmd = ps->GetCmd(this, 'SBCL');
        cmd->In()->SetF(this->blindColor.x);
        cmd->In()->SetF(this->blindColor.y);
        cmd->In()->SetF(this->blindColor.z);
        cmd->In()->SetF(this->blindColor.w);
        ps->PutCmd(cmd);

        // --- beginflares ---
        cmd = ps->GetCmd(this, 'BFLR');
        cmd->In()->SetI(this->GetNumFlares());
        ps->PutCmd(cmd);

        int i;
        for (i = 0; i < this->GetNumFlares(); ++i)
        {
            // --- setflaresizeat ---
            cmd = ps->GetCmd(this, 'SFS_');
            cmd->In()->SetI(i);
            cmd->In()->SetF(this->GetFlareSizeAt(i));
            ps->PutCmd(cmd);

            // --- setflareposat ---
            cmd = ps->GetCmd(this, 'SFPA');
            cmd->In()->SetI(i);
            cmd->In()->SetF(this->GetFlarePosAt(i));
            ps->PutCmd(cmd);

            // --- setflarecolorat ---
            cmd = ps->GetCmd(this, 'SFCA');
            const vector4& color = this->GetFlareColorAt(i);
            cmd->In()->SetI(i);
            cmd->In()->SetF(color.x);
            cmd->In()->SetF(color.y);
            cmd->In()->SetF(color.z);
            cmd->In()->SetF(color.w);
            ps->PutCmd(cmd);
        }

        // --- endflares ---
        cmd = ps->GetCmd(this, 'EFLR');
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
