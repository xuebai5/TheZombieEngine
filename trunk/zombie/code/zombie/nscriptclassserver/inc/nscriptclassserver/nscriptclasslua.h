#ifndef NSCRIPTCLASSLUA_H
#define NSCRIPTCLASSLUA_H
//------------------------------------------------------------------------------
/**
    @class nScriptClassLua
    @ingroup NebulaClassScriptServer

    Description of the porpouse of the class

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "nscriptclassserver/nscriptclassserver.h"

//------------------------------------------------------------------------------
class nScriptClassLua : public nScriptClassServer
{
public:
    /// constructor
    nScriptClassLua();
    /// destructor
    ~nScriptClassLua();
    /// initialize method
    void Initialize();

    /// reset the script class Lua
    void Reset();

protected:
    /// return the script function to be used
    bool GetScript( const char * object, const char * method, nString & function )const;
    /// add to a path the script extension template
    void AppendSearchTemplate( nString & path )const;
    /// add a function to the script context
    void AddScriptFunction( const nString & classname, const nString & path, const nString & file )const;
    /// insert class in script classes list
    bool InsertClass( const char * classname );
    /// delete from script a method
    void DeleteScriptMethod( const nString & classname, const nString & method )const;
    /// delete class from script
    void DeleteScriptClass( const nString & classname )const;
    /// get the script cmd prototype to add it to the class
    nCmdProto * GetScriptCmdProto( const nString & file )const;

private:
    /// load a script from Lua and exec it
    bool LoadAndExecScript( const nString & filepath )const;

    static const char * classTableName;
};

//------------------------------------------------------------------------------
#endif//NSCRIPTCLASSLUA_H
