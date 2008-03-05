#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncviewport_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncviewport.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncViewport)
    NSCRIPT_ADDCMD_COMPOBJECT('MSVP', void, SetViewport, 4, (float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGVP', void, GetViewport, 0, (), 4, (float&, float&, float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSPP', void, SetPerspective, 4, (float, float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSOP', void, SetOrthogonal, 4, (float, float, float, float), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('MSMP', void, SetMaterialProfile, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGMP', int, GetMaterialProfile, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSBG', void, SetBackgroundColor, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGBG', void, GetBackgroundColor, 0, (), 3, (float&, float&, float&));
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
bool
ncViewport::SaveCmds(nPersistServer *ps)
{
    if (ncScene::SaveCmds(ps))
    {
        nCmd *cmd;
        
        // --- setviewport ---
        cmd = ps->GetCmd(this->entityObject, 'MSVP');
        float x, y, w, h;
        this->GetViewport(x, y, w, h);
        cmd->In()->SetF(x);
        cmd->In()->SetF(y);
        cmd->In()->SetF(w);
        cmd->In()->SetF(h);
        ps->PutCmd(cmd);

        if (this->camera.GetType() == nCamera2::Perspective)
        {
            //--- setperspective ---
            cmd = ps->GetCmd(this->GetEntityObject(), 'MSPP');
            cmd->In()->SetF(this->camera.GetAngleOfView());
            cmd->In()->SetF(this->camera.GetAspectRatio());
            cmd->In()->SetF(this->camera.GetNearPlane());
            cmd->In()->SetF(this->camera.GetFarPlane());
            ps->PutCmd(cmd);
        }
        else
        {
            //--- setorthogonal ---
            cmd = ps->GetCmd(this->GetEntityObject(), 'MSOP');
            cmd->In()->SetF(this->camera.GetWidth());
            cmd->In()->SetF(this->camera.GetHeight());
            cmd->In()->SetF(this->camera.GetNearPlane());
            cmd->In()->SetF(this->camera.GetFarPlane());
            ps->PutCmd(cmd);
        }

        return true;
    }
    return false;
}