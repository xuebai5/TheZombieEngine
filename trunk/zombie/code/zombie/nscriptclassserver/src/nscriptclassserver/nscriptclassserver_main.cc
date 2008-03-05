//------------------------------------------------------------------------------
//  nscriptclassserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnscriptclassserver.h"

#include "nscriptclassserver/nscriptclassserver.h"

#include "kernel/nfileserver2.h"

#include "kernel/nkernelserver.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nScriptClassServer, "nroot");

nScriptClassServer * nScriptClassServer::singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nScriptClassServer::nScriptClassServer():
    classes( 307 ) // prime number
{
    n_assert2( !nScriptClassServer::singleton , "nScriptClassServer instance already created" );

    nScriptClassServer::singleton = this;

    directories.Append( "wc:libs\\classes" );
}

//------------------------------------------------------------------------------
/**
*/
nScriptClassServer::~nScriptClassServer()
{
    nScriptClassServer::singleton = 0;
}

//------------------------------------------------------------------------------
/**
    @returns the nScriptClassServer singleton instance
*/
nScriptClassServer*
nScriptClassServer::Instance()
{
    //n_assert2( nScriptClassServer::singleton, "No instance of nScriptClassServer created" );

    return nScriptClassServer::singleton;
}

//------------------------------------------------------------------------------
/**
    @param object object that call the method
    @param method called method
    @param function equivalent function in scripting code
    @returns if the script can be founded
*/
bool
nScriptClassServer::GetScript( const char * /*object*/, const char * /*method*/, nString & /*function*/ )const
{
    return false;
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to add
    @param path where is the file in disk
    @param file name of the file
*/
void
nScriptClassServer::AddScriptFunction( const nString & /*classname*/, const nString & /*path*/, const nString & /*file*/ )const
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to add
*/
void
nScriptClassServer::DeleteScriptClass( const nString & /*classname*/ )const
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to add
    @param method method to delete
*/
void
nScriptClassServer::DeleteScriptMethod( const nString & /*classname*/, const nString & /*method*/ )const
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @param path the string where append the search template
*/
void
nScriptClassServer::AppendSearchTemplate( nString & path )const
{
    path.Append( "\\*.*" );
}

//------------------------------------------------------------------------------
/**
    @param file name of the file
*/
nCmdProto *
nScriptClassServer::GetScriptCmdProto( const nString & /*file*/ )const
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    @param def class definition
*/
void
nScriptClassServer::DeleteMethods( nScriptClassServer::ScriptClassDef & def )const
{
    // for every method in the script classs
    for( int imethod = 0 ; imethod < def.methods.Size() ; ++ imethod )
    {
        // delete cmds
        def.clazz->DeleteScriptCmd( def.methods[ imethod ].Get() );

        // delete from script
        this->DeleteScriptMethod( def.clazz->GetName(), def.methods[ imethod ] );
    }

    // clear method list
    def.methods.Clear();
}

//------------------------------------------------------------------------------
/**
    @param def class definition
*/
void
nScriptClassServer::AddMethods( nScriptClassServer::ScriptClassDef & def )const
{
    // for every search directory
    nString fullpath;
    nString searchpath;
    nString methodname;
    nArray<nString> files;
    for( int idir = 0 ; idir < this->directories.Size() ; ++idir )
    {
        // create full path 
        fullpath.Set( this->directories[ idir ].Get() );
        fullpath.Append( "\\" );
        fullpath.Append( def.clazz->GetName() );
        
        searchpath.Set( fullpath.Get() );
        this->AppendSearchTemplate( searchpath );

        nFileServer2::Instance()->GetAllFilesFrom( searchpath.Get(), files );
        // for every file in full path
        for( int ifile = 0 ; ifile < files.Size() ; ++ifile )
        {
            // add function to class definition
            if( this->GetFunctionName( files[ ifile ], methodname ) )
            {
                // check than not exist the method name
                if( 0 == def.methods.Find( methodname ) )
                {
                    // add function to nebula commands
                    nCmdProto * cmdProto = this->GetScriptCmdProto( files[ ifile ] );
                    n_assert( cmdProto );
                    if( cmdProto )
                    {
                        // add to methods list
                        def.methods.Append( methodname );

                        def.clazz->AddScriptCmd( cmdProto );

                        // add function to scripting
                        this->AddScriptFunction( def.clazz->GetName(), fullpath, files[ ifile ] );
                    }
                }
                else
                {
                    n_message( "Script method [ %s ] already exist in [ %s ]", 
                        methodname.Get(), def.clazz->GetName() );
                }
            }
            else
            {
                n_message( "Script file name invalid %s\\%s", 
                    fullpath.Get(), files[ ifile ].Get() );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param file name of the file that contains the function
    @param function name of the function extract from the file name
    @returns true if name is correct
*/
bool
nScriptClassServer::GetFunctionName( const nString & file, nString & function )const
{
    function.Set( file.Get() );
    function.StripExtension();
    int index1, index2;
    index1 = function.FindChar( '_', 0 );
    if( index1 <= 0 )
    {
        return false;
    }

    index2 = function.FindChar( '_', index1 + 1 );
    if( index2 <= (index1 + 1) && (index2 - index1 - 1) < 1 )
    {
        return false;
    }

    function = function.ExtractRange( index1 + 1, index2 - index1 - 1 );

    return true;
}

//------------------------------------------------------------------------------
/**
    @param name name of the class to check
    @returns true if class has script
*/
bool
nScriptClassServer::HasScript( const nString & name )
{
    nString fullpath;

    // check that have script
    for( int idir = 0 ; idir < this->directories.Size() ; ++idir )
    {
        // create full path 
        fullpath.Set( this->directories[ idir ].Get() );
        fullpath.Append( "\\" );
        fullpath.Append( name );

        if( nFileServer2::Instance()->DirectoryExists( fullpath ) )
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    @param name name of the class to add
*/
void
nScriptClassServer::AddClass( const nString & name )
{
    nAtom classKey( name.Get() );
    ScriptClassDef * classDef = this->classes[ classKey ];

    // if class is not in the list
    if( ! classDef )
    {
        // add class to the list
        if( this->InsertClass( name.Get() ) )
        {
            classDef = this->classes[ classKey ];
            n_assert2( classDef, "Error can not add a new class to Script Class Server" );
            if( ! classDef )
            {
                return;
            }
        }
    }

    // delete old methods if any
    this->DeleteMethods( *classDef );

    // add new methods
    this->AddMethods( *classDef );
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to add
*/
bool
nScriptClassServer::InsertClass( const char * classname )
{
    ScriptClassDef def;

    // search class in the nClass list
    nClass * clazz = nKernelServer::ks->FindClass( classname );
    if( clazz )
    {
        def.clazz = clazz;
        this->classes.Add( classname, &def );
    }
    else
    {
        n_message( "Can found nClass %s", classname );
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nScriptClassServer::AddClasses()
{
    n_assert( nKernelServer::ks );

    // get class list
    const nHashList * classlist = nKernelServer::ks->GetClassList();
    n_assert( classlist );

    // for every class in the list
    nHashNode * node = classlist->GetHead();
    while( node )
    {
        if( this->HasScript( node->GetName() ) )
        {
            // add to the script class server
            this->AddClass( node->GetName() );
        }

        node = node->GetSucc();
    }
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to delete
*/
void
nScriptClassServer::DeleteClass( const nString & name )
{
    ScriptClassDef * classDef = this->classes[ name.Get() ];

    // if class is not in the list
    if( ! classDef )
    {
        n_message( "DeleteClass: class %s is not in the script class server", name.Get() );
        return;
    }

    // delete old methods
    this->DeleteMethods( *classDef );

    // delete class
    this->DeleteScriptClass( name );

    this->classes.Remove( name.Get() );
}

//------------------------------------------------------------------------------
/**
*/
void
nScriptClassServer::DeleteClasses()
{
    ScriptClassDef * def;
    nString key;
    
    // for every class in the list
    this->classes.Begin();
    while( 0 != ( def = this->classes.Next() ) )
    {
        // delete class
        this->DeleteClass( def->clazz->GetName() );
    }
}

//------------------------------------------------------------------------------
/**
    @param classname name of the class to refresh
*/
void
nScriptClassServer::RefreshClass( const nString & name )
{
    ScriptClassDef * classDef = this->classes[ name.Get() ];

    // if class is not in the list
    if( ! classDef )
    {
        this->AddClass( name );
        return;
    }

    // delete old methods
    this->DeleteMethods( *classDef );

    // add new methods
    this->AddMethods( *classDef );
}

//------------------------------------------------------------------------------
/**
*/
void
nScriptClassServer::RefreshClasses()
{
    ScriptClassDef * def;
    nAtom key;
    
    // for every class in the list
    this->classes.Begin();
    this->classes.Next( key, def );
    while( def )
    {
        // refresh class
        this->RefreshClass( def->clazz->GetName() );

        this->classes.Next( key, def );
    }
}

//------------------------------------------------------------------------------
/**
    @param object object that call the method
    @param method called method
    @param function equivalent function in scripting code
    @returns if the script can be founded
*/
bool
nScriptClassServer::GetScript( const nObject * const object, const nString & method, nString & function )const
{
    // search the function in the correct class
    bool founded = false;
    ScriptClassDef * classDef = 0;

    // search in the hierarchi classes
    nClass * clazz = object->GetClass();
    while( ! founded && clazz )
    {
        classDef = this->classes[ clazz->GetName() ];
        if( classDef )
        {
            // search the method in the class definition
            int imethod = 0;
            while( ! founded && imethod < classDef->methods.Size() )
            {
                founded = ( method == classDef->methods[ imethod++ ] );
            }
        }

        if( ! founded )
        {
            clazz = clazz->GetSuperClass();
        }
    }

    if( founded )
    {
        return this->GetScript( clazz->GetName(), method.Get(), function );
    }

    return false;
}

//------------------------------------------------------------------------------
