#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nconjurerlayout_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nconjurerlayout.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nconjurerlayout

    @cppclass
    nConjurerLayout

    @superclass
    nappviewportui

    @classinfo
    An encapsulation of viewport layout for Conjurer.
*/
NSCRIPT_INITCMDS_BEGIN(nConjurerLayout)
    NSCRIPT_ADDCMD('SDVV', void, SetDefViewerVelocity, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GDVV', float, GetDefViewerVelocity, 0, (), 0, ());
    NSCRIPT_ADDCMD('SDVA', void, SetDefViewerAngles, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD('GDVA', void, GetDefViewerAngles, 0, (), 2, (float&, float&));
    NSCRIPT_ADDCMD('SDVP', void, SetDefViewerPos, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('GDVP', const vector3&, GetDefViewerPos, 0, (), 0, ());
    NSCRIPT_ADDCMD('SDVZ', void, SetDefViewerZoom, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('GDVZ', const vector3&, GetDefViewerZoom, 0, (), 0, ());
    NSCRIPT_ADDCMD('SPRV', void, SetPreviewViewport, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nConjurerLayout::SaveCmds(nPersistServer* ps)
{
    if (nAppViewportUI::SaveCmds(ps))
    {
        nCmd* cmd;

        // --- setdefviewervelocity ---
        cmd = ps->GetCmd(this, 'SDVV');
        cmd->In()->SetF(this->GetDefViewerVelocity());
        ps->PutCmd(cmd);

        // --- setdefviewerangles ---
        cmd = ps->GetCmd(this, 'SDVA');
        cmd->In()->SetF(this->defViewerAngles.theta);
        cmd->In()->SetF(this->defViewerAngles.rho);
        ps->PutCmd(cmd);

        // --- setdefviewerpos ---
        cmd = ps->GetCmd(this, 'SDVP');
        cmd->In()->SetF(this->defViewerPos.x);
        cmd->In()->SetF(this->defViewerPos.y);
        cmd->In()->SetF(this->defViewerPos.z);
        ps->PutCmd(cmd);

        // --- setdefviewerzoom ---
        cmd = ps->GetCmd(this, 'SDVZ');
        cmd->In()->SetF(this->defViewerZoom.x);
        cmd->In()->SetF(this->defViewerZoom.y);
        cmd->In()->SetF(this->defViewerZoom.z);
        ps->PutCmd(cmd);

        // --- setpreview ---
        // @todo ma.garcias

        // --- setmap ---
        // @todo ma.garcias

        return true;
    }
    return false;
}
