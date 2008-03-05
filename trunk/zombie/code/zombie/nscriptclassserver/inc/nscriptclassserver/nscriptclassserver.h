#ifndef NSCRIPTCLASSSERVER_H
#define NSCRIPTCLASSSERVER_H
//------------------------------------------------------------------------------
/**
    @class nScriptClassServer
    @ingroup NebulaClassScriptServer
    
    @brief A class server is needed to manage all the scripts of a class, 
    preload them, and call properly the already loaded scripts.
    
    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "util/nmaptabletypes.h"

//------------------------------------------------------------------------------
class nScriptClassServer : public nRoot
{
public:
    /// get instance pointer
    static nScriptClassServer * Instance();

    /// constructor
    nScriptClassServer();
    /// destructor
    virtual ~nScriptClassServer();

    /// check if a class has scripting
    bool HasScript(const nString &);
    /// add a nclass to the list of scripted classes
    void AddClass(const nString &);
    /// add all nclass of kernel server
    void AddClasses ();
    /// delete a nclass from the list of scripted classes
    void DeleteClass(const nString &);
    /// delete all classes
    void DeleteClasses ();
    /// refresh the scripts from a class
    void RefreshClass(const nString &);
    /// refresh the scripts from all classes already added
    void RefreshClasses ();

    /// return the script function to be used
    virtual bool GetScript( const nObject * const object, const nString & method, nString & function )const;

protected:
    /// return the script function to be used
    virtual bool GetScript( const char * object, const char * method, nString & function )const;
    /// add to a path the script extension template
    virtual void AppendSearchTemplate( nString & path )const;
    /// add a function to the script context
    virtual void AddScriptFunction( const nString & classname, const nString & path, const nString & file )const;
    /// insert class in script classes list
    virtual bool InsertClass( const char * classname );
    /// delete from script a method
    virtual void DeleteScriptMethod( const nString & classname, const nString & method )const;
    /// delete class from script
    virtual void DeleteScriptClass( const nString & classname )const;
    /// get the script cmd prototype to add it to the class
    virtual nCmdProto * GetScriptCmdProto( const nString & file )const;

    /// get the name of the function from the file name
    bool GetFunctionName( const nString & file, nString & method )const;

    nArray<nString> directories;

private:
    struct ScriptClassDef
    {
        nClass * clazz;
        nArray<nString> methods;
    };

    /// delete the methods of a script class definition
    void DeleteMethods( ScriptClassDef & def )const;
    /// add the methods of a class
    void AddMethods( ScriptClassDef & def )const;

    /// singleton instance of class
    static nScriptClassServer* singleton;

    nMapTableTypes<ScriptClassDef>::NAtom classes;
};
//------------------------------------------------------------------------------
#endif//NSCRIPTCLASSSERVER_H
