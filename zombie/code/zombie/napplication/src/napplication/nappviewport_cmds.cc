#include "precompiled/pchnapplication.h"
//------------------------------------------------------------------------------
//  nappviewport_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "napplication/nappviewport.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nappviewport

    @cppclass
    nAppViewport

    @superclass
    nroot

    @classinfo
    An encapsulation of application viewport.
*/
NSCRIPT_INITCMDS_BEGIN(nAppViewport)
    NSCRIPT_ADDCMD('SVRS', void, SetRelSize, 4, (float, float, float, float), 0, ());
    NSCRIPT_ADDCMD('GVRS', void, GetRelSize, 0, (), 4, (float&, float&, float&, float&));
    NSCRIPT_ADDCMD('SVPR', void, SetViewport, 4, (float, float, float, float), 0, ());
    NSCRIPT_ADDCMD('GVPR', void, GetViewport, 0, (), 4, (float&, float&, float&, float&));
    NSCRIPT_ADDCMD('SVIS', void, SetVisible, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GVIS', bool, GetVisible, 0, (), 0, ());
    NSCRIPT_ADDCMD('SVWP', void, SetViewerPos, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('GVWP', const vector3&, GetViewerPos, 0, (), 0, ());
    NSCRIPT_ADDCMD('SVWA', void, SetViewerAngles, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD('GVWA', void, GetViewerAngles, 0, (), 2, (float&, float&));
    NSCRIPT_ADDCMD('SNPL', void, SetNearPlane, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GNPL', float, GetNearPlane, 0, (), 0, ());
    NSCRIPT_ADDCMD('SFPL', void, SetFarPlane, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GFPL', float, GetFarPlane, 0, (), 0, ());
    NSCRIPT_ADDCMD('SAOV', void, SetAngleOfView, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GAOV', float, GetAngleOfView, 0, (), 0, ());
    NSCRIPT_ADDCMD('JSIV', void, SetIsometricView, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('JGIV', bool, GetIsometricView, 0, (), 0, ());
    NSCRIPT_ADDCMD('SVWV', void, SetViewerVelocity, 1, (float), 0, ());
    NSCRIPT_ADDCMD('GVWV', float, GetViewerVelocity, 0, (), 0, ());
    NSCRIPT_ADDCMD('SVWZ', void, SetViewerZoom, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('GVWZ', const vector3&, GetViewerZoom, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nAppViewport::SaveCmds(nPersistServer* ps)
{
    if (nRoot::SaveCmds(ps))
    {
        nCmd* cmd;

        float x, y, width, height;
        this->GetRelSize(x, y, width, height);
        if (x == 0.0f && y == 0.0f && width == 0.0f && height == 0.0f)
        {
            // --- setviewport ---
            this->GetViewport(x, y, width, height);
            cmd = ps->GetCmd(this, 'SVPR');
            cmd->In()->SetF(x);
            cmd->In()->SetF(y);
            cmd->In()->SetF(width);
            cmd->In()->SetF(height);
            ps->PutCmd(cmd);
        }
        else
        {
            // --- setrelsize ---
            cmd = ps->GetCmd(this, 'SVRS');
            cmd->In()->SetF(x);
            cmd->In()->SetF(y);
            cmd->In()->SetF(width);
            cmd->In()->SetF(height);
            ps->PutCmd(cmd);
        }

        // --- setvisible ---
        cmd = ps->GetCmd(this, 'SVIS');
        cmd->In()->SetB(this->GetVisible());
        ps->PutCmd(cmd);

        // --- setviewerpos ---
        const vector3& pos = this->GetViewerPos();
        cmd = ps->GetCmd(this, 'SVWP');
        cmd->In()->SetF(pos.x);
        cmd->In()->SetF(pos.y);
        cmd->In()->SetF(pos.z);
        ps->PutCmd(cmd);

        // --- setviewerangles ---
        const polar2& angles = this->GetViewerAngles();
        cmd = ps->GetCmd(this, 'SVWA');
        cmd->In()->SetF(angles.theta);
        cmd->In()->SetF(angles.rho);
        ps->PutCmd(cmd);

        // --- setviewervelocity ---
        cmd = ps->GetCmd(this, 'SVWV');
        cmd->In()->SetF(this->GetViewerVelocity());
        ps->PutCmd(cmd);

        // --- setviewerzoom ---
        const vector3& zoom = this->GetViewerZoom();
        cmd = ps->GetCmd(this, 'SVWZ');
        cmd->In()->SetF(zoom.x);
        cmd->In()->SetF(zoom.y);
        cmd->In()->SetF(zoom.z);
        ps->PutCmd(cmd);

        const nCamera2& camera = this->GetCamera();

        // --- setnearplane ---
        cmd = ps->GetCmd(this, 'SNPL');
        cmd->In()->SetF(camera.GetNearPlane());
        ps->PutCmd(cmd);

        // --- setfarplane ---
        cmd = ps->GetCmd(this, 'SFPL');
        cmd->In()->SetF(camera.GetFarPlane());
        ps->PutCmd(cmd);

        // --- setangleofview ---
        cmd = ps->GetCmd(this, 'SAOV');
        cmd->In()->SetF(camera.GetAngleOfView());
        ps->PutCmd(cmd);

        // --- setisometricview ---
        cmd = ps->GetCmd(this, 'JSIV');
        cmd->In()->SetB(this->GetIsometricView());
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
