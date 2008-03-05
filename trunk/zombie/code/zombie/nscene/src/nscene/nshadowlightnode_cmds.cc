#include "precompiled/pchrnsscene.h"
//------------------------------------------------------------------------------
//  nshadowlightnode_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nshadowlightnode.h"
#include "kernel/npersistserver.h"


NSCRIPT_INITCMDS_BEGIN( nShadowLightNode )
    NSCRIPT_ADDCMD('AABE', void, SetOrthogonal, 4, (float, float, float, float), 0, ());
    NSCRIPT_ADDCMD('AABF', void, SetEuler, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('AABG', const vector3, GetEuler, 0, (), 0, ());
    NSCRIPT_ADDCMD('AABH', void, SetPosition, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD('AABI', const vector3, GetPosition, 0, (), 0, ());
    NSCRIPT_ADDCMD('CSDE', void, SetDeformation, 1, (const vector4&), 0, ());
    NSCRIPT_ADDCMD('CGDE', const vector4, GetDeformation, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
bool
nShadowLightNode::SaveCmds(nPersistServer *ps)
{
    if (nAbstractShaderNode::SaveCmds(ps))
    {
        nCmd* cmd;
        
        ////---- SetPerspective ----
        //cmd = ps->GetCmd(this, 'AABD');
        //cmd->In()->SetF(this->GetCamera()->GetAngleOfView());
        //cmd->In()->SetF(this->GetCamera()->GetAspectRatio());
        //cmd->In()->SetF(this->GetCamera()->GetNearPlane());
        //cmd->In()->SetF(this->GetCamera()->GetFarPlane());
        //ps->PutCmd(cmd);

        //---- SetOrthogonal ----
        cmd = ps->GetCmd(this, 'AABE');
        cmd->In()->SetF(this->GetCamera().GetWidth());
        cmd->In()->SetF(this->GetCamera().GetHeight());
        cmd->In()->SetF(this->GetCamera().GetNearPlane());
        cmd->In()->SetF(this->GetCamera().GetFarPlane());
        ps->PutCmd(cmd);

        //---- SetEuler ----
        cmd = ps->GetCmd(this, 'AABF');
        cmd->In()->SetF(this->euler.x);
        cmd->In()->SetF(this->euler.y);
        cmd->In()->SetF(this->euler.z);
        ps->PutCmd(cmd);

        //---- SetPosition ----
        cmd = ps->GetCmd(this, 'AABH');
        cmd->In()->SetF(this->position.x);
        cmd->In()->SetF(this->position.y);
        cmd->In()->SetF(this->position.z);
        ps->PutCmd(cmd);

        //---- SetDeformation ----
        cmd = ps->GetCmd(this, 'CSDE');
        cmd->In()->SetF(this->deformation.x);
        cmd->In()->SetF(this->deformation.y);
        cmd->In()->SetF(this->deformation.z);
        cmd->In()->SetF(this->deformation.w);
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
