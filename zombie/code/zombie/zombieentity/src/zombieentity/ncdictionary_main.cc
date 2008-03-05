#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  ncdictionary_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/ncdictionary.h"
#include "nscene/ncscene.h"
#include "variable/nvariableserver.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncDictionary, nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncDictionary)
    NSCRIPT_ADDCMD_COMPOBJECT('SBOV', void, SetBoolVariable, 2, (const char *, bool), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GBOV', bool, GetBoolVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SINV', void, SetIntVariable, 2, (const char *, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GINV', int, GetIntVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SFLV', void, SetFloatVariable, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GFLV', float, GetFloatVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SVCV', void, SetVectorVariable, 2, (const char *, const vector4&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GVCV', const vector4&, GetVectorVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SSTV', void, SetStringVariable, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GSTV', const char *, GetStringVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SLIV', int, SetLocalIntVariable, 2, (const char *, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GLIV', int, GetLocalIntVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SLFV', int, SetLocalFloatVariable, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GLFV', float, GetLocalFloatVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SLVV', int, SetLocalVectorVariable, 2, (const char *, const vector4&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GLVV', const vector4&, GetLocalVectorVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SLSV', int, SetLocalStringVariable, 2, (const char *, const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GLSV', const char *, GetLocalStringVariable, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('SLOV', int, SetLocalObjectVariable, 2, (const char *, nObject*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('GLOV', nObject*, GetLocalObjectVariable, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncDictionary::ncDictionary()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncDictionary::~ncDictionary()
{
    // empty 
}

//------------------------------------------------------------------------------
/**
    instance initialization (TEMPORARY)
*/
void
ncDictionary::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType == nObject::NewInstance)
    {
        // get the list of default variables from the class and create them
        ncDictionaryClass *compClass = this->GetEntityObject()->GetClassComponent<ncDictionaryClass>();
        //n_assert(compClass);?
        if (compClass)
        {
            this->varContext.Clear();
            nVariableContext& varContext = compClass->VariableContext();
            for (int i = 0; i < varContext.GetNumVariables(); i++)
            {
                this->varContext.AddVariable(varContext.GetVariableAt(i));
            }
        }
    }

    if (initType == nObject::ReloadedInstance)
    {
        this->ClearLocalVars();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncDictionary::SetBoolVariable(const char *name, bool value)
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
ncDictionary::GetBoolVariable(const char *name)
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
ncDictionary::SetIntVariable(const char *name, int value)
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
ncDictionary::GetIntVariable(const char *name)
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
ncDictionary::SetFloatVariable(const char *name, float value)
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
ncDictionary::GetFloatVariable(const char *name)
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
ncDictionary::SetVectorVariable(const char *name, const vector4& value)
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
const vector4&
ncDictionary::GetVectorVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        return var->GetVector4();
    }
    static vector4 nullVec;
    return nullVec;
}

//------------------------------------------------------------------------------
/**
*/
void
ncDictionary::SetStringVariable(const char *name, const char *value)
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
ncDictionary::GetStringVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->varContext.GetVariable(handle);
    if (var)
    {
        return var->GetString();
    }
    return "";
}

// local variables

//------------------------------------------------------------------------------
/**
*/
int
ncDictionary::SetLocalIntVariable(const char *name, int value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    return this->AddLocalVar(nVariable(handle, value));
}

//------------------------------------------------------------------------------
/**
*/
int
ncDictionary::GetLocalIntVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->FindLocalVar(handle);
    if (var)
    {
        return var->GetInt();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
ncDictionary::SetLocalFloatVariable(const char *name, float value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    return this->AddLocalVar(nVariable(handle, value));
}

//------------------------------------------------------------------------------
/**
*/
float
ncDictionary::GetLocalFloatVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->FindLocalVar(handle);
    if (var)
    {
        return var->GetFloat();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
ncDictionary::SetLocalVectorVariable(const char *name, const vector4& value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    return this->AddLocalVar(nVariable(handle, value));
}

//------------------------------------------------------------------------------
/**
*/
const vector4&
ncDictionary::GetLocalVectorVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->FindLocalVar(handle);
    if (var)
    {
        return var->GetVector4();
    }

    static vector4 nullVec;
    return nullVec;
}

//------------------------------------------------------------------------------
/**
*/
int
ncDictionary::SetLocalStringVariable(const char *name, const char* value)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    return this->AddLocalVar(nVariable(handle, value));
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncDictionary::GetLocalStringVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->FindLocalVar(handle);
    if (var)
    {
        return var->GetString();
    }

    return "";
}

//------------------------------------------------------------------------------
/**
*/
int
ncDictionary::SetLocalObjectVariable(const char *name, nObject *object)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    return this->AddLocalVar(nVariable(handle, object));
}

//------------------------------------------------------------------------------
/**
*/
nObject*
ncDictionary::GetLocalObjectVariable(const char *name)
{
    nVariable::Handle handle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->FindLocalVar(handle);
    if (var)
    {
        return (nObject*) var->GetObj();
    }

    return 0;
}

// commands

//------------------------------------------------------------------------------
/**
*/
bool
ncDictionary::SaveCmds(nPersistServer * ps)
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
                cmd = ps->GetCmd(this->GetEntityObject(), 'SBOV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetB(var.GetBool());
                ps->PutCmd(cmd);
                break;

            case nVariable::Int:
                // --- setintvariable ---
                cmd = ps->GetCmd(this->GetEntityObject(), 'SINV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetI(var.GetInt());
                ps->PutCmd(cmd);
                break;

            case nVariable::Float:
                // --- setfloatvariable ---
                cmd = ps->GetCmd(this->GetEntityObject(), 'SFLV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetF(var.GetFloat());
                ps->PutCmd(cmd);
                break;

            case nVariable::Vector4:
                // --- setvectorvariable ---
                cmd = ps->GetCmd(this->GetEntityObject(), 'SVCV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetF(var.GetVector4().x);
                cmd->In()->SetF(var.GetVector4().y);
                cmd->In()->SetF(var.GetVector4().z);
                cmd->In()->SetF(var.GetVector4().w);
                ps->PutCmd(cmd);
                break;

            case nVariable::String:
                // --- setstringvariable ---
                cmd = ps->GetCmd(this->GetEntityObject(), 'SSTV');
                cmd->In()->SetS(nVariableServer::Instance()->GetVariableName(var.GetHandle()));
                cmd->In()->SetS(var.GetString());
                ps->PutCmd(cmd);
                break;
            }
        }
    }

    return true;
}

