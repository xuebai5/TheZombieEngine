//------------------------------------------------------------------------------
//  nscriptclasslua_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnscriptclassserver.h"

#include "nscriptclassserver/nscriptclasslua.h"

#include "kernel/nfile.h"
#include "kernel/nfileserver2.h"

#include "luaserver/nluaserver.h"
#include "luaserver/ncmdprotolua.h"

//------------------------------------------------------------------------------
nNebulaClass(nScriptClassLua, "nscriptclassserver");

const char * nScriptClassLua::classTableName = "nebscriptclasses";

//------------------------------------------------------------------------------
/**
*/
nScriptClassLua::nScriptClassLua()
{
}

//------------------------------------------------------------------------------
/**
*/
nScriptClassLua::~nScriptClassLua()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nScriptClassLua::Initialize()
{
    this->Reset();
}

//------------------------------------------------------------------------------
/**
*/
void
nScriptClassLua::Reset()
{
    lua_State * L = nLuaServer::Instance->GetContext();
    n_assert2( L, "Can not get Lua Context" );
    if( ! L )
    {
        return;
    }

    // add table for scripting in Lua
    lua_pushstring( L, nScriptClassLua::classTableName );
    lua_newtable( L );
    lua_settable( L, LUA_GLOBALSINDEX );
}

//------------------------------------------------------------------------------
/**
    @param path the string where append the search template
*/
void
nScriptClassLua::AppendSearchTemplate( nString & path )const
{
    path.Append( "\\*.lua" );
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to add
*/
void
nScriptClassLua::DeleteScriptClass( const nString & classname )const
{
    bool valid;

    // get lua context
    lua_State * L = nLuaServer::Instance->GetContext();
    n_assert2( L, "Can not get Lua Context" );
    if( ! L )
    {
        return;
    }

    // get table with class scripting
    lua_pushstring( L, nScriptClassLua::classTableName );
    lua_gettable( L, LUA_GLOBALSINDEX );

    valid = lua_istable( L, -1 );
    n_assert2( valid, "Table with Lua class scripts isn't created" );
    if( ! valid )
    {
        // restore stack
        lua_pop( L, 1 );
        return;
    }

    // set class index to nil
    lua_pushstring( L, classname.Get() );
    lua_pushnil( L );
    lua_settable( L, -3 );

    lua_pop( L, 1 );
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to add
    @param method method to delete
*/
void
nScriptClassLua::DeleteScriptMethod( const nString & classname, const nString & method )const
{
    bool valid;

    // get lua context
    lua_State * L = nLuaServer::Instance->GetContext();
    n_assert2( L, "Can not get Lua Context" );
    if( ! L )
    {
        return;
    }

    // get table with class scripting
    lua_pushstring( L, nScriptClassLua::classTableName );
    lua_gettable( L, LUA_GLOBALSINDEX );

    valid = lua_istable( L, -1 );
    n_assert2( valid, "Table with Lua class scripts isn't created" );
    if( ! valid )
    {
        // restore stack
        lua_pop( L, 1 );
        return;
    }

    // get table of the class
    lua_pushstring( L, classname.Get() );
    lua_gettable( L, -2 );

    valid = lua_istable( L, -1 );
    n_assert2( valid, "Table with the class isn't created" );
    if( ! valid )
    {
        // restore stack
        lua_pop( L, 2 );
        return;
    }

    // set nil index in the table
    lua_pushstring( L, method.Get() );
    lua_pushnil( L );
    lua_settable( L, -3 );

    lua_pop( L, 2 );
}

//------------------------------------------------------------------------------
/**
    @param object object that call the method
    @param method called method
    @param function equivalent function in scripting code
    @returns if the script cen be founded
*/
bool
nScriptClassLua::GetScript( const char * object, const char * method, nString & function )const
{
    function.Set( nScriptClassLua::classTableName );
    function.Append( "." );
    function.Append( object );
    function.Append( "." );
    function.Append( method );

    return true;
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to add
*/
bool
nScriptClassLua::InsertClass( const char * classname )
{
    bool valid;

    // insert in the parent
    valid = nScriptClassServer::InsertClass( classname );
    if( valid )
    {
        // get lua context
        lua_State * L = nLuaServer::Instance->GetContext();
        n_assert2( L, "Can not get Lua Context" );
        if( ! L )
        {
            return false;
        }

        // get table with class scripting
        lua_pushstring( L, nScriptClassLua::classTableName );
        lua_gettable( L, LUA_GLOBALSINDEX );

        valid = lua_istable( L, -1 );
        n_assert2( valid, "Table with Lua class scripts isn't created" );
        if( valid )
        {
            // add class table for scripting in Lua
            lua_pushstring( L, classname );
            lua_newtable( L );
            lua_settable( L, -3 );
        }

        // restore stack
        lua_pop( L, 1 );
    }

    return valid;
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to add
    @param path where is the file in disk
    @param file name of the file
*/
void
nScriptClassLua::AddScriptFunction( const nString & classname, const nString & path, const nString & file )const
{
    bool valid;

    // load script
    nString filepath( path );
    filepath.Append( "\\" );
    filepath.Append( file );

    if( ! this->LoadAndExecScript( filepath ) )
    {
        n_message( "Invalid definition of script function %s\\%s", path.Get(), file.Get() );
        return;
    }

    // get function name from filename
    nString functionName( file );
    valid = this->GetFunctionName( file, functionName );
    if( ! valid )
    {
        n_message( "Script file name invalid %s\\%s", path.Get(), file.Get() );
        return;
    }

    // get lua context
    lua_State * L = nLuaServer::Instance->GetContext();
    n_assert2( L, "Can not get Lua Context" );
    if( ! L )
    {
        return;
    }

    // get table with class scripting
    lua_pushstring( L, nScriptClassLua::classTableName );
    lua_gettable( L, LUA_GLOBALSINDEX );

    valid = lua_istable( L, -1 );
    n_assert2( valid, "Table with Lua class scripts isn't created" );
    if( ! valid )
    {
        // restore stack
        lua_pop( L, 1 );
        return;
    }

    // get table of the class
    lua_pushstring( L, classname.Get() );
    lua_gettable( L, -2 );

    valid = lua_istable( L, -1 );
    n_assert2( valid, "Table with the class isn't created" );
    if( ! valid )
    {
        // restore stack
        lua_pop( L, 2 );
        return;
    }

    // set new index in the table
    lua_pushstring( L, functionName.Get() );

    // get new function
    lua_pushstring( L, "__scriptfunction" );
    lua_gettable( L, LUA_GLOBALSINDEX );

    if( ! lua_isfunction( L, -1 ) )
    {
        n_message( "Can load script function of %s\\%s", path.Get(), file.Get() );
        // restore stack
        lua_pop( L, 4 );
        return;
    }

    // set function in the table
    lua_settable( L, -3 );

    // clean function from global space
    lua_pushstring( L, "__scriptfunction" );
    lua_pushnil( L );
    lua_settable( L, LUA_GLOBALSINDEX );

    // restore stack
    lua_pop( L, 2 );

}

//------------------------------------------------------------------------------
/**
    @param filepath file no load
    @return true if the file was load
*/
bool
nScriptClassLua::LoadAndExecScript( const nString & filepath )const
{
    nString result;
    int filesize;
    char *cmdbuf;
    bool retval;
    const char * header = "__scriptfunction =\n";
    
    nFile* nfile = nFileServer2::Instance()->NewFileObject();
    nString path = nFileServer2::Instance()->ManglePath(filepath);
    if (!nfile->Open(path, "r"))
    {
        result.Clear();
        nfile->Release();
        return false;
    }
    
    nfile->Seek(0, nFile::END);
    filesize = nfile->Tell();
    nfile->Seek(0, nFile::START);
       
    cmdbuf = n_new_array( char, strlen(header) + filesize + 1 );
    n_assert2(cmdbuf, "Failed to allocate command buffer!");
    strcpy(cmdbuf, header);
    nfile->Read(cmdbuf + strlen(header), filesize + 1);
    cmdbuf[strlen(header) + filesize] = 0;
    
    nfile->Close();
    nfile->Release();

    retval = nLuaServer::Instance->Run( cmdbuf, result );

    n_delete_array( cmdbuf );

    return retval;
}

//------------------------------------------------------------------------------
/**
    @param file name of the file
*/
nCmdProto *
nScriptClassLua::GetScriptCmdProto( const nString & file )const
{
    nString functiondef( file );
    functiondef.StripExtension();
    nCmdProtoLua * protoLua = n_new( nCmdProtoLua( functiondef.Get() ) );
    n_assert( protoLua );

    return protoLua;
}

//------------------------------------------------------------------------------
