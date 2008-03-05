#include "precompiled/pchndebug.h"
//------------------------------------------------------------------------------
//  ndebugmodule_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ndebug/ndebugmodule.h"
#include "ndebug/ndebugserver.h"
#include "kernel/nkernelserver.h"
#include "variable/nvariableserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nDebugModule, "nroot");

//------------------------------------------------------------------------------
NSIGNAL_DEFINE(nDebugModule, DebugOptionChanged);

//------------------------------------------------------------------------------
/**
*/
nDebugModule::nDebugModule()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nDebugModule::~nDebugModule()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Called when the module is created through the debug server.
    Here debug options should be registered into the server.
*/
void
nDebugModule::OnDebugModuleCreate()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Called when an option is checked in.
    The module should retrieve debug options to optimize access.
*/
void
nDebugModule::OnDebugOptionUpdated()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Create and register a new debug flag in the debug server,
    and create a variable entry for the option.
*/
void
nDebugModule::RegisterDebugFlag(const char* name, const char* description, const char* groupName)
{
    nDebugServer::Instance()->RegisterDebugFlag(name, description, groupName);

    nVariable::Handle optionHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->debugOptions.GetVariable(optionHandle);

    if (!var)
    {
        this->debugOptions.AddVariable(nVariable(optionHandle, false));
    }
}

//------------------------------------------------------------------------------
/**
    Create and register a new debug colour setting on the debug server,
    and create a variable entry for the option.
*/
void
nDebugModule::RegisterDebugColourSetting(const char* name, const char* description, const char* groupName, vector4 defaultColour)
{
    nDebugServer::Instance()->RegisterDebugColourSetting(name, description, groupName);

    nVariable::Handle optionHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->debugOptions.GetVariable(optionHandle);

    if (!var)
    {
        this->debugOptions.AddVariable(nVariable( optionHandle, defaultColour ));
    }
}

//------------------------------------------------------------------------------
/**
    Get number of registered debug options
*/
int
nDebugModule::GetNumOptions() const
{
    return this->debugOptions.GetNumVariables();
}

//------------------------------------------------------------------------------
/**
    Get the option at the given index
*/
const char *
nDebugModule::GetOptionAt(const int index) const
{
    nVariable& var = this->debugOptions.GetVariableAt(index);
    return nVariableServer::Instance()->GetVariableName(var.GetHandle());
}

//------------------------------------------------------------------------------
/**
*/
bool
nDebugModule::SetFlagEnabled(const char *name, bool value)
{
    nVariable::Handle optionHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->debugOptions.GetVariable(optionHandle);
    if (var)
    {
        var->SetBool(value);
        this->OnDebugOptionUpdated();
        return true;
    }
    else
    {
        //n_printf("nDebugModule::SetFlagEnabled: option '%s' not found in debug module '%s'", name, this->GetName());
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nDebugModule::GetFlagEnabled(const char *name) const
{
    nVariable::Handle optionHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->debugOptions.GetVariable(optionHandle);
    if (var)
    {
        return var->GetBool();
    }
    else
    {
        //n_printf("nDebugModule::GetFlagEnabled: option '%s' not found in debug module '%s'", name, this->GetName());
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nDebugModule::SetColourSettingValue(const char *name, vector4 colour)
{
    nVariable::Handle optionHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->debugOptions.GetVariable(optionHandle);
    if (var)
    {
        var->SetVector4(colour);
        this->OnDebugOptionUpdated();
        return true;
    }
    else
    {
        //n_printf("nDebugModule::SetColorSettingValue: option '%s' not found in debug module '%s'", name, this->GetName());
        return false;
    }
}
//------------------------------------------------------------------------------
/**
*/
vector4
nDebugModule::GetColourSettingValue(const char *name) const
{
    nVariable::Handle optionHandle = nVariableServer::Instance()->GetVariableHandleByName(name);
    nVariable *var = this->debugOptions.GetVariable(optionHandle);
    if (var)
    {
        return var->GetVector4();
    }
    else
    {
        //n_printf("nDebugModule::GetColorSettingValue: option '%s' not found in debug module '%s'", name, this->GetName());
    }
    return vector4();
}

//------------------------------------------------------------------------------
/**
*/
const char *
nDebugModule::GetOptionDescription(const char *name) const
{
    return nDebugServer::Instance()->GetOptionDescription(name);
}
