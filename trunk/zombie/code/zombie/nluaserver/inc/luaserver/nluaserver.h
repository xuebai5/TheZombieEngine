#ifndef N_LUASERVER_H
#define N_LUASERVER_H
//--------------------------------------------------------------------
//  nluaserver.h
//
//  (c) 2003  Matthew T. Welker & Vadim Macagon
//
//  nLuaServer is licensed under the terms of the Nebula License
//--------------------------------------------------------------------
/** 
    @class nLuaServer
    @ingroup NLuaServerContribModule

    @brief Lua 5 wrapper for Nebula

    Implements an nScriptServer that runs Lua 5, extended
    by default with a few Nebula specific commands.
    
    (C) 2003  Matthew T. Welker & Vadim Macagon
*/
#include <string.h>

#include "kernel/nroot.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "kernel/nautoref.h"
#include "util/nstring.h"

// Lua requires it's headers to be explicitly extern'd
extern "C" {
    #include "lua/lua.h"
    #include "lua/lauxlib.h"
    #include "lua/lualib.h"
}

//--------------------------------------------------------------------
class nFileServer2;
class nLuaServer : public nScriptServer {
public:
    nLuaServer();
    virtual ~nLuaServer();
 
    virtual bool Run(const char *, nString&);
    virtual bool RunScript(nFile *, nString&);
    virtual bool RunFunction(const char *, nString&);
    virtual nString Prompt();
    
    const char* GenerateStackTrace();

    virtual bool BeginWrite(nFile * file, nObject* obj, nCmd * cmd);  
    virtual bool WriteComment(nFile *, const char *);
    virtual bool WriteBeginNewObject(nFile *, nObject *, nObject *);
    virtual bool WriteBeginNewObjectCmd(nFile *, nObject *, nObject *, nCmd *);
    //virtual bool WriteBeginSelObject(nFile *, nObject *, nObject *);
    virtual bool WriteCmd(nFile *, nCmd *);
    virtual bool WriteEndObject(nFile *, nObject *, nObject *);
    virtual bool EndWrite(nFile *);
 
    virtual bool Trigger(void);

    virtual lua_State* GetContext();
    
    // manipulate _nebthunks table
    static void RemoveThunk(lua_State*, void*);
    static void AddThunk(lua_State*, void*);
    static void FindThunk(lua_State*, void*);
    
    static void AddClassToCache(lua_State*, nClass*);
    static bool ThunkNebObject(lua_State*, nObject*);
    static nObject* UnpackThunkRoot( lua_State*, int );
    
    static void InArgsToStack(lua_State*, nCmd*);
    static void OutArgsToStack(lua_State*, nCmd*, bool);
    static bool StackToInArgs(lua_State*, nCmd*);
    static bool StackToOutArgs(lua_State*, nCmd*);
    static void ListArgToTable(lua_State*, nArg*, bool);

    /// set an unnamed object that will receive cmds instead of the cwd
    virtual void SetCurrentTargetObject(nObject *obj);

    // Check if object is a nRoot -- for fast testing
    static bool IsAnRoot( nObject *obj );

public:
    static nLuaServer *Instance;
    static nClass *nroot_cl;
    nString classCacheName;
    nString thunkStoreName;
    
private:
    static void ArgToStack( lua_State*, nArg* );
    static bool StackToArg( lua_State*, nArg*, int index );
    static void StackToString( lua_State*, int, nString& );
    void reg_globalfunc(lua_CFunction, const char*);
    void write_select_statement(nFile *, nRoot *, nRoot *);
    bool ExecuteLuaChunk(nString&, int errfunc);
    void SetNThis(void);
    void UnsetNThis(void);

    long indent_level;
    char indent_buf[N_MAXPATH];
    lua_State* L;   
    bool echo;
    bool selgrab;
    nString outputStr;
};

//--------------------------------------------------------------------
#endif
