#ifndef N_DEBUGSERVER_H
#define N_DEBUGSERVER_H
//------------------------------------------------------------------------------
/**
    @class nDebugServer
    @ingroup NebulaDebugSystem

    @brief The main Debug system class. 

    This server allows creating any number of debug options that can register 
    themselves onto this server using an option id (a string) and description. 
    These can be enabled on a per-object basis. When debug rendering is invoked
    for an object (it is up to the client application when this is done), the
    active options for the object are triggered, and each derived class is
    responsible for implementing the right behavior.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "variable/nvariablecontext.h"
#include "kernel/ncmdprotonativecpp.h"
#include "util/nmaptabletypes.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nDebugOption;
class nDebugModule;

//------------------------------------------------------------------------------
class nDebugServer : public nRoot
{
public:

    struct DebugOptionInfo
    {
        /// debug option unique name
        nString name;

        /// debug option description
        nString description;

        // debug option type
        int type;

        // debug option group
        const char* group;
    };

    enum DebugOptionType
    {
        DebugOptionTypeFlag,
        DebugOptionTypeColour
    };

    /// constructor
    nDebugServer();
    /// destructor
    virtual ~nDebugServer();
    /// Init instance
    virtual void InitInstance(nObject::InitInstanceMsg);
    /// get instance pointer
    static nDebugServer* Instance();
    
    /// create a debug module for any application object
    nDebugModule* CreateDebugModule(const char* moduleClass, const char* moduleName);
    /// register a debug option by name
    void RegisterDebugOption(const char* optionName, const char* description, int type, const char* groupName);
    /// register a debug flag with name, description and group
    void RegisterDebugFlag(const char* optionName, const char* description, const char* groupName);
    /// register a debug colour setting with name, description and group
    void RegisterDebugColourSetting(const char* optionName, const char* description, const char* groupName);
    
    /// @name Script interface
    //@{
    /// get number of debug options for named module
    int GetNumOptions(const char *) const;
    /// get option by index for named module
    const char * GetOptionAt(const char *, int) const;
    /// set option flag enabled for named module, returning true or false for successful update
    bool SetFlagEnabled(const char *, const char *, bool);
    /// get option flag enabled for named module
    bool GetFlagEnabled(const char *, const char *) const;
    /// set colour setting for named module, returning true or false for successful update
    bool SetColourSettingValue(const char * modulename, const char * optionname, vector4 colour);
    /// get colour setting value for named module
    vector4 GetColourSettingValue(const char * modulename, const char * optionname) const;
    /// get option description for named option
    const char * GetOptionDescription(const char *) const;
    /// get option type for named option
    int GetOptionType(const char *) const;
    /// get group name for named option
    const char* GetOptionGroupName(const char *) const;
    //@}

private:

    static nDebugServer* Singleton;
    nRef<nRoot> refModules;
    nMapTableTypes<DebugOptionInfo>::NString debugOptions; // dictionary of string variables

    /// get named option 
    DebugOptionInfo* GetOptionInfo(const char *) const;

};

//------------------------------------------------------------------------------
/**
*/
inline
nDebugServer *
nDebugServer::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
#endif // N_DEBUGSERVER_H
