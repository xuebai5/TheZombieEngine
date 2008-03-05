#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ncdictionaryclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/ncdictionaryclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncDictionaryClass, nComponentClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncDictionaryClass)
    NSCRIPT_ADDCMD_COMPCLASS('MIVV', bool, IsVariableValid, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SBOV', void, SetBoolVariable, 2, (const char *, bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GBOV', bool, GetBoolVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SINV', void, SetIntVariable, 2, (const char *, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GINV', int, GetIntVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SFLV', void, SetFloatVariable, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GFLV', float, GetFloatVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SVCV', void, SetVectorVariable, 2, (const char *, vector4), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GVCV', vector4, GetVectorVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('SSTV', void, SetStringVariable, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('GSTV', const char *, GetStringVariable, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncDictionaryClass::ncDictionaryClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncDictionaryClass::~ncDictionaryClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ncDictionaryClass::IsVariableValid(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    return (this->varContext.GetVariable(handle) != 0);
}

//------------------------------------------------------------------------------
/**
*/
void
ncDictionaryClass::SetBoolVariable(const char *name, bool value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        var->SetBool(value);
    }
    else
    {
        this->varContext.AddVariable(nVariable(handle, value));
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncDictionaryClass::GetBoolVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        return var->GetBool();
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncDictionaryClass::SetIntVariable(const char *name, int value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        var->SetInt(value);
    }
    else
    {
        this->varContext.AddVariable(nVariable(handle, value));
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncDictionaryClass::GetIntVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        return var->GetInt();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncDictionaryClass::SetFloatVariable(const char *name, float value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        var->SetFloat(value);
    }
    else
    {
        this->varContext.AddVariable(nVariable(handle, value));
    }
}

//------------------------------------------------------------------------------
/**
*/
float
ncDictionaryClass::GetFloatVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        return var->GetFloat();
    }
    else
    {
        return 0.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncDictionaryClass::SetVectorVariable(const char *name, vector4 value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        var->SetVector4(value);
    }
    else
    {
        this->varContext.AddVariable(nVariable(handle, value));
    }
}

//------------------------------------------------------------------------------
/**
*/
vector4
ncDictionaryClass::GetVectorVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        return var->GetVector4();
    }
    return vector4();
}

//------------------------------------------------------------------------------
/**
*/
void
ncDictionaryClass::SetStringVariable(const char *name, const char *value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        var->SetString(value);
    }
    else
    {
        this->varContext.AddVariable(nVariable(handle, value));
    }
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncDictionaryClass::GetStringVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        return var->GetString();
    }
    return "";
}

// commands

//------------------------------------------------------------------------------
/**
*/
bool
ncDictionaryClass::SaveCmds(nPersistServer * ps)
{
    // --- setxxxvariable ---
    int varIndex;
    int numVars = this->varContext.GetNumVariables();
    for (varIndex = 0; varIndex < numVars; varIndex++)
    {
        nVariable::Handle timeHandle = nVariableServer::Instance()->GetVariableHandleByName("time");
        nVariable& var = this->varContext.GetVariableAt(varIndex);
        if (var.GetHandle() != timeHandle)
        {
            //
            //  TODO: convert to new persistence format
            //
            nCmd* cmd;

            switch (var.GetType())
            {
            case nVariable::Bool:
                // --- setboolvariable ---
                cmd = ps->GetCmd(this->GetEntityClass(), 'SBOV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetB(var.GetBool());
                ps->PutCmd(cmd);
                break;

            case nVariable::Int:
                // --- setintvariable ---
                cmd = ps->GetCmd(this->GetEntityClass(), 'SINV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetI(var.GetInt());
                ps->PutCmd(cmd);
                break;

            case nVariable::Float:
                // --- setfloatvariable ---
                cmd = ps->GetCmd(this->GetEntityClass(), 'SFLV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetF(var.GetFloat());
                ps->PutCmd(cmd);
                break;

            case nVariable::Vector4:
                // --- setvectorvariable ---
                cmd = ps->GetCmd(this->GetEntityClass(), 'SVCV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetF(var.GetVector4().x);
                cmd->In()->SetF(var.GetVector4().y);
                cmd->In()->SetF(var.GetVector4().z);
                cmd->In()->SetF(var.GetVector4().w);
                ps->PutCmd(cmd);
                break;

            case nVariable::String:
                // --- setstringvariable ---
                cmd = ps->GetCmd(this->GetEntityClass(), 'SSTV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetS(var.GetString());
                ps->PutCmd(cmd);
                break;
            }
        }
    }

    return true;
}

