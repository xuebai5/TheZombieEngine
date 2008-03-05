#ifndef N_DEBUGMODULE_H
#define N_DEBUGMODULE_H
//------------------------------------------------------------------------------
/**
    @class nDebugModule
    @ingroup NebulaDebugSystem

    @brief An abstract class for creating application-specific modules.

    Debug Modules describe an application object that accepts some
    debug option(s) to be enabled on it through the @ref nDebugServer.

    Any kind of application object can be debugged, but some debug
    options could require some additional information on the object 
    being debugged. Debug modules allow creating specific application
    classes that can 

    (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"
#include "util/narray.h"
#include "variable/nvariablecontext.h"

//------------------------------------------------------------------------------
class nDebugModule : public nRoot
{
public:
    struct DebugOptionGroup
    {
        nString name;
        nString description;
        nArray<nString> optionNames;
    };

    /// constructor
    nDebugModule();
    /// destructor
    virtual ~nDebugModule();
    /// called when the module is created
    virtual void OnDebugModuleCreate();
    /// called when an option is assigned
    virtual void OnDebugOptionUpdated();
    /// get number of registered debug options
    int GetNumOptions() const;
    /// get debug option at given index
    const char *GetOptionAt(const int index) const;
    /// set bool value for debug option flag, returning true or false for successful update
    bool SetFlagEnabled(const char *name, bool value);
    /// get bool value for debug option flag
    bool GetFlagEnabled(const char *name) const;
    /// set vector4 colour value for debug option, returning true or false for successful update
    bool SetColourSettingValue(const char *name, vector4 colour);
    /// get vector4 colour value for debug option
    vector4 GetColourSettingValue(const char *name) const;
    /// get description for debug option
    const char* GetOptionDescription(const char *name) const;

    // Signals
    NSIGNAL_DECLARE('DMOC', void, DebugOptionChanged, 1, (const char *), 0, ());

    /// register new debug flag for this module
    void RegisterDebugFlag(const char* name, const char* description, const char* groupName);
    /// register new debug colour setting for this module
    void RegisterDebugColourSetting(const char* name, const char* description, const char* groupName, vector4 defaultColour);

private:

    nVariableContext debugOptions;

};

//------------------------------------------------------------------------------
#endif // N_DEBUGMODULE_H
