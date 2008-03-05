#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncscene_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncscene.h"
#include "kernel/npersistserver.h"
#include "nscene/ncsceneclass.h"
#include "entity/nentityobjectserver.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncScene)
    NSCRIPT_ADDCMD_COMPOBJECT('SVEC', void, SetVectorOverride, 2, (const char *, vector4), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GVEC', vector4, GetVectorOverride, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SFLT', void, SetFloatOverride, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GFLT', float, GetFloatOverride, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SINT', void, SetIntOverride, 2, (const char *, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GINT', int, GetIntOverride, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AAAH', void, Plug, 2, (const char *, nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AAAI', void, UnPlug, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AACR', const nString, GetPlugName, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGPE', const nString, GetPlugNameEntity, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AACS', int, GetNumPlugs, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AACT', nEntityObjectId, GetPluggedEntity, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MSPE', void, SetPassEnabled, 2, (const char *, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGPE', bool, GetPassEnabled, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LSHE', void, SetHiddenEntity, 1, (bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('LGHE', bool, GetHiddenEntity , 0, (), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD_COMPOBJECT('MSLD', void, SetLinearFogDistances, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGLD', void, GetLinearFogDistances, 0, (), 2, (float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSYD', void, SetLayeredFogDistances, 3, (float, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGYD', void, GetLayeredFogDistances, 0, (), 3, (float&, float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSWF', void, SetWaveFrequency, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGWF', void, GetWaveFrequency, 0, (), 1, (float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSWA', void, SetWaveAmplitude, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGWA', void, GetWaveAmplitude, 0, (), 1, (float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSWV', void, SetWaveVelocity, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGWV', void, GetWaveVelocity, 0, (), 2, (float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSTS', void, SetTexScale, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGTS', void, GetTexScale, 0, (), 2, (float&, float&));
    NSCRIPT_ADDCMD_COMPOBJECT('MSFR', void, SetFresnel, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('MGFR', void, GetFresnel, 0, (), 2, (float&, float&));
#endif
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
bool
ncScene::SaveCmds(nPersistServer* ps)
{
    if (nComponentObject::SaveCmds(ps))
    {
        // --- setxxxoverride ---
        nShaderParams& shaderOverrides = this->GetShaderOverrides();
        int paramIndex;
        int numParams = shaderOverrides.GetNumValidParams();
        for (paramIndex = 0; paramIndex < numParams; paramIndex++)
        {
            nCmd *cmd;

            nShaderState::Param param = shaderOverrides.GetParamByIndex(paramIndex);
            const nShaderArg& arg = shaderOverrides.GetArg(param);
            vector4 v;

            switch (arg.GetType())
            {
            case nShaderState::Float4:
                // --- setvectoroverride ---
                cmd = ps->GetCmd(this->GetEntityObject(), 'SVEC');
                cmd->In()->SetS(nShaderState::ParamToString(param));
                cmd->In()->SetF(arg.GetFloat4().x);
                cmd->In()->SetF(arg.GetFloat4().y);
                cmd->In()->SetF(arg.GetFloat4().z);
                cmd->In()->SetF(arg.GetFloat4().w);
                ps->PutCmd(cmd);
                break;

            case nShaderState::Float:
                // --- setfloatoverride ---
                cmd = ps->GetCmd(this->GetEntityObject(), 'SFLT');
                cmd->In()->SetS(nShaderState::ParamToString(param));
                cmd->In()->SetF(arg.GetFloat());
                ps->PutCmd(cmd);
                break;

            case nShaderState::Int:
                // --- setintoverride ---
                cmd = ps->GetCmd(this->GetEntityObject(), 'SINT');
                cmd->In()->SetS(nShaderState::ParamToString(param));
                cmd->In()->SetI(arg.GetInt());
                ps->PutCmd(cmd);
                break;
            }
        }

        // --- setpassenabled ---
        int numPasses = nSceneServer::Instance()->GetNumPasses();
        for (int i = 0; i < numPasses; ++i)
        {
            nCmd *cmd;
            char buf[5];

            nFourCC fourcc = nSceneServer::Instance()->GetPassAt(i);
            if ((this->passEnabledFlags & (1<<i)) == 0)
            {
                cmd = ps->GetCmd(this->GetEntityObject(), 'MSPE');
                cmd->In()->SetS(nVariableServer::FourCCToString(fourcc, buf, sizeof(buf)));
                cmd->In()->SetB(false);
                ps->PutCmd(cmd);
            }
        }

        // --- Plugs ---
        if (!this->plugArray.Empty())
        {
            ncSceneClass* sceneClass = this->GetClassComponent<ncSceneClass>();

            for (int i = 0; i < this->plugArray.Size(); i++)
            {
                nCmd *cmd;
    
                if(this->plugArray[i] != 0 && nEntityObjectServer::Instance()->GetEntityObjectType(this->plugArray[i]) == nEntityObjectServer::Normal)
                {
                    cmd = ps->GetCmd(this->GetEntityObject(), 'AAAH');
                    cmd->In()->SetS( sceneClass->GetPlug(i).GetName().Get());
                    cmd->In()->SetI( this->plugArray[i] );
                    ps->PutCmd(cmd);
                }
            }
        }
    }

    return true;
}
