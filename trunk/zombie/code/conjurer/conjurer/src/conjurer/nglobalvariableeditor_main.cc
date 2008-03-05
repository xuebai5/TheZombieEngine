#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nglobalvariableeditor_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nglobalvariableeditor.h"
#include "variable/nvariableserver.h"

nNebulaScriptClass(nGlobalVariableEditor, "nroot");

//------------------------------------------------------------------------------
/**
    @scriptclass
    nglobalvariableeditor

    @cppclass
    nGlobalVariableEditor

    @superclass
    nroot

    @classinfo
    A helper interface to edit global variables in conjurer
*/
NSCRIPT_INITCMDS_BEGIN(nGlobalVariableEditor)
    NSCRIPT_ADDCMD('MISD', bool, IsDirty, 0, (), 0, ());
    NSCRIPT_ADDCMD('MSGV', void, SaveGlobalVariables, 0, (), 0, ());
    NSCRIPT_ADDCMD('MSFD', void, SetFadeDistances, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD('MGFD', void, GetFadeDistances, 0, (), 2, (float&, float&));
    NSCRIPT_ADDCMD('ESFT', void, SetMaxPathFindingTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD('EGFT', float, GetMaxPathFindingTime, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESST', void, SetMaxPathSmoothingTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD('EGST', float, GetMaxPathSmoothingTime, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESAS', void, SetMinAgentsSpeed, 1, (float), 0, ());
    NSCRIPT_ADDCMD('EGAS', float, GetMinAgentsSpeed, 0, (), 0, ());
    NSCRIPT_ADDCMD('ESGD', void, SetGroundSeekDistances, 2, (float, float), 0, ());
    NSCRIPT_ADDCMD('EGGD', void, GetGroundSeekDistances, 0, (), 2, (float&, float&));
NSCRIPT_INITCMDS_END()

namespace
{
    const char* GlobalVarsPath("wc:libs/variables");
    const char* GlobalVarsFile("wc:libs/variables/globals.n2");
}

//------------------------------------------------------------------------------
/**
*/
nGlobalVariableEditor::nGlobalVariableEditor() :
    refVarServer("/sys/servers/variable"),
    isDirty(false)
{
    nVariableServer::Instance()->BindSignal(nVariableServer::SignalGlobalVariableChanged, 
                                            this,
                                            &nGlobalVariableEditor::GlobalVariableChanged,
                                            0);
}

//------------------------------------------------------------------------------
/**
*/
nGlobalVariableEditor::~nGlobalVariableEditor()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nGlobalVariableEditor::IsDirty()
{
    return this->isDirty;
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::GlobalVariableChanged()
{
    this->isDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::SetFadeDistances(float minDist, float maxDist)
{
    this->refVarServer->SetFloatVariable("FadeMinDistance", minDist);
    this->refVarServer->SetFloatVariable("FadeMaxDistance", maxDist);
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::GetFadeDistances(float& minDist, float& maxDist)
{
    minDist = this->refVarServer->GetFloatVariable("FadeMinDistance");
    maxDist = this->refVarServer->GetFloatVariable("FadeMaxDistance");
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::SetMaxPathFindingTime(float time)
{
    this->refVarServer->SetFloatVariable("MaxPathFindingTime", time);
}

//------------------------------------------------------------------------------
/**
*/
float
nGlobalVariableEditor::GetMaxPathFindingTime()
{
    return this->refVarServer->GetFloatVariable("MaxPathFindingTime");
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::SetMaxPathSmoothingTime(float time)
{
    this->refVarServer->SetFloatVariable("MaxPathSmoothingTime", time);
}

//------------------------------------------------------------------------------
/**
*/
float
nGlobalVariableEditor::GetMaxPathSmoothingTime()
{
    return this->refVarServer->GetFloatVariable("MaxPathSmoothingTime");
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::SetMinAgentsSpeed(float time)
{
    this->refVarServer->SetFloatVariable("MinAgentsSpeed", time);
}

//------------------------------------------------------------------------------
/**
*/
float
nGlobalVariableEditor::GetMinAgentsSpeed()
{
    return this->refVarServer->GetFloatVariable("MinAgentsSpeed");
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::SetGroundSeekDistances(float startDist, float endDist)
{
    this->refVarServer->SetFloatVariable("GroundSeekStartDistance", startDist);
    this->refVarServer->SetFloatVariable("GroundSeekEndDistance", endDist);
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::GetGroundSeekDistances(float& startDist, float& endDist)
{
    startDist = this->refVarServer->GetFloatVariable("GroundSeekStartDistance");
    endDist = this->refVarServer->GetFloatVariable("GroundSeekEndDistance");
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::LoadGlobalVariables()
{
    if (kernelServer->GetFileServer()->FileExists(GlobalVarsFile))
    {
        kernelServer->PushCwd(this->refVarServer);
        kernelServer->Load(GlobalVarsFile, false);
        kernelServer->PopCwd();

        this->isDirty = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGlobalVariableEditor::SaveGlobalVariables()
{
    nFileServer2* fileServer = kernelServer->GetFileServer();
    if (!fileServer->DirectoryExists(GlobalVarsPath))
    {
        fileServer->MakePath(GlobalVarsPath);
    }

    nPersistServer* ps = kernelServer->GetPersistServer();
    nCmd* cmd = ps->GetCmd(this->refVarServer, 'THIS');
    if (ps->BeginObjectWithCmd(this->refVarServer, cmd, GlobalVarsFile))
    {
        const nVariableContext& varContext = this->refVarServer->GetGlobalVariableContext();
        int numVars = varContext.GetNumVariables();
        for (int varIndex = 0; varIndex < numVars; ++varIndex)
        {
            nVariable& var = varContext.GetVariableAt(varIndex);
            const char* varName = this->refVarServer->GetVariableName(var.GetHandle());
            switch (var.GetType())
            {
            case nVariable::Float:
                // --- setfloatvariable ---
                cmd = ps->GetCmd(this->refVarServer, 'SFLV');
                cmd->In()->SetS(varName);
                cmd->In()->SetF(var.GetFloat());
                ps->PutCmd(cmd);
                break;

            case nVariable::Int:
                // --- setintvariable ---
                cmd = ps->GetCmd(this->refVarServer, 'SINV');
                cmd->In()->SetS(varName);
                cmd->In()->SetI(var.GetInt());
                ps->PutCmd(cmd);
                break;

            case nVariable::Vector4:
                // --- setvectorvariable ---
                cmd = ps->GetCmd(this->refVarServer, 'SVCV');
                cmd->In()->SetS(varName);
                cmd->In()->SetF(var.GetVector4().x);
                cmd->In()->SetF(var.GetVector4().y);
                cmd->In()->SetF(var.GetVector4().z);
                cmd->In()->SetF(var.GetVector4().w);
                ps->PutCmd(cmd);
                break;

            case nVariable::String:
                // --- setstringvariable ---
                cmd = ps->GetCmd(this->refVarServer, 'SSTV');
                cmd->In()->SetS(varName);
                cmd->In()->SetS(var.GetString());
                ps->PutCmd(cmd);
                break;
            }
        }

        ps->EndObject(true);
    }

    isDirty = false;
}
