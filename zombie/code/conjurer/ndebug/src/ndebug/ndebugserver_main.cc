//------------------------------------------------------------------------------
//  ndebugserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchndebug.h"

#include "ndebug/ndebugserver.h"
#include "ndebug/ndebugmodule.h"
#include "kernel/nkernelserver.h"
#include "variable/nvariableserver.h"

nNebulaScriptClass(nDebugServer, "nroot");

nDebugServer* nDebugServer::Singleton = 0;

//------------------------------------------------------------------------------
const char* DefaultGroupName = "General";

//------------------------------------------------------------------------------
/**
*/
nDebugServer::nDebugServer()
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
void 
nDebugServer::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    kernelServer->PushCwd(this);
    this->refModules = kernelServer->New("nroot", "modules");
    kernelServer->PopCwd();
}

//------------------------------------------------------------------------------
/**
*/
nDebugServer::~nDebugServer()
{
    n_assert(0 != Singleton);
    Singleton = 0;

    this->refModules->Release();
}

//------------------------------------------------------------------------------
/**
*/
nDebugModule* 
nDebugServer::CreateDebugModule(const char* moduleClass, const char* moduleName)
{
    n_assert(moduleName);
    n_assert(!this->refModules->Find(moduleName));

    kernelServer->PushCwd(this->refModules.get());
    nDebugModule* newModule = (nDebugModule*) kernelServer->New(moduleClass, moduleName);
    kernelServer->PopCwd();
    n_assert(newModule);
    n_assert(newModule->IsA(kernelServer->FindClass("ndebugmodule")));
    newModule->OnDebugModuleCreate();

    return newModule;
}

//------------------------------------------------------------------------------
/**
*/
void
nDebugServer::RegisterDebugColourSetting(const char* optionName, const char* description, const char* groupName)
{
    this->RegisterDebugOption(optionName, description, DebugOptionTypeColour, groupName);
}

//------------------------------------------------------------------------------
/**
*/
void
nDebugServer::RegisterDebugFlag(const char* optionName, const char* description, const char* groupName)
{
    this->RegisterDebugOption(optionName, description, DebugOptionTypeFlag, groupName);
}

//------------------------------------------------------------------------------
/**
*/
void
nDebugServer::RegisterDebugOption(const char* optionName, const char* description, int type, const char* groupName)
{
    n_assert(optionName);

    DebugOptionInfo newOption;
    newOption.name = nString(optionName);
    newOption.description = nString(description);
    newOption.type = type;
    if ( !groupName )
    {
        groupName = DefaultGroupName;
    }
    newOption.group = groupName;

    this->debugOptions.Add(optionName, &newOption);
    
}

//------------------------------------------------------------------------------
/**
    @param modulename name of the module
    @returns number of options
*/
int
nDebugServer::GetNumOptions(const char* modulename) const
{
    nDebugModule *module = (nDebugModule *) this->refModules->Find(modulename);
    if (module)
    {
        return module->GetNumOptions();
    }
    //n_printf("nDebugServer::GetNumOptions: module '%s' not found.", modulename);
    return 0;
}

//------------------------------------------------------------------------------
/**
    @param modulename name of the module
    @param index option index
    @returns the option name
*/
const char *
nDebugServer::GetOptionAt(const char* modulename, int index) const
{
    nDebugModule *module = (nDebugModule *) this->refModules->Find(modulename);
    if (module)
    {
        return module->GetOptionAt(index);
    }
    //n_printf("nDebugServer::GetOptionAt: module '%s' not found.", modulename);
    return 0;
}

//------------------------------------------------------------------------------
/**
    @param modulename name of the module
    @param optionname the name of the option
    @param enabled new option value
    @returns true if successful, else false
*/
bool
nDebugServer::SetFlagEnabled(const char* modulename, const char* optionname, bool enabled)
{
    nDebugModule *module = (nDebugModule *) this->refModules->Find(modulename);
    if (module)
    {
        bool updatedOK = module->SetFlagEnabled(optionname, enabled);
        return updatedOK;
    }
    else
    {
        //n_printf("nDebugServer::SetFlagEnabled: module '%s' not found.", modulename);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    @param modulename name of the module
    @param optionname the name of the option
    @returns the option value
*/
bool
nDebugServer::GetFlagEnabled(const char* modulename, const char* optionname) const
{
    nDebugModule *module = (nDebugModule *) this->refModules->Find(modulename);
    if (module)
    {
        return module->GetFlagEnabled(optionname);
    }
    else
    {
        //n_printf("nDebugServer::GetFlagEnabled: module '%s' not found.", modulename);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    @param modulename name of the module
    @param optionname the name of the option
    @param colour new option value
    @returns true if successful, else false
*/
bool
nDebugServer::SetColourSettingValue(const char* modulename, const char* optionname, vector4 colour)
{
    nDebugModule *module = (nDebugModule *) this->refModules->Find(modulename);
    if (module)
    {
        bool updatedOK = module->SetColourSettingValue(optionname, colour);
        return updatedOK;
    }
    else
    {
        //n_printf("nDebugServer::SetColourSettingValue: module '%s' not found.", modulename);
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    @param modulename name of the module
    @param optionname the name of the option
    @returns the option value
*/
vector4
nDebugServer::GetColourSettingValue(const char* modulename, const char* optionname) const
{
    nDebugModule *module = (nDebugModule *) this->refModules->Find(modulename);
    if (module)
    {
        return module->GetColourSettingValue(optionname);
    }
    else
    {
        //n_printf("nDebugServer::GetColourSettingValue: module '%s' not found.", modulename);
        return vector4();
    }
}

//------------------------------------------------------------------------------
/**
    @param optionname the name of the option
    @returns the option info
*/
nDebugServer::DebugOptionInfo*
nDebugServer::GetOptionInfo(const char* optionName) const
{
    n_assert(optionName);

    return this->debugOptions[optionName];
}

//------------------------------------------------------------------------------
/**
    @param optionname the name of the option
    @returns the option description
*/
const char *
nDebugServer::GetOptionDescription(const char* optionName) const
{
    n_assert(optionName);

    DebugOptionInfo* debugOptionInfo = this->GetOptionInfo(optionName);

    n_assert(debugOptionInfo);
    
    return debugOptionInfo->description.Get();
}

//------------------------------------------------------------------------------
/**
    @param optionname the name of the option
    @returns the option type
*/
int
nDebugServer::GetOptionType(const char* optionName) const
{
    n_assert(optionName);

    DebugOptionInfo* debugOptionInfo = this->GetOptionInfo(optionName);

    n_assert(debugOptionInfo);

    return debugOptionInfo->type;
}

//------------------------------------------------------------------------------
/**
    @param optionname the name of the option
    @returns the option group name
*/
const char*
nDebugServer::GetOptionGroupName(const char* optionName) const
{
    n_assert(optionName);

    DebugOptionInfo* debugOptionInfo = this->GetOptionInfo(optionName);

    n_assert(debugOptionInfo);

    return debugOptionInfo->group;
}
