//------------------------------------------------------------------------------
//  ndependencyserver_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/ndependencyserver.h"
#include "util/nstring.h"
#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"

nNebulaScriptClass( nDependencyServer, "nroot" );

//------------------------------------------------------------------------------
nString nDependencyServer::parentName;
nString nDependencyServer::parentPath;
nObject* nDependencyServer::parentObject = 0;

nStack<nDependencyServer::DepNode*> nDependencyServer::parentInfoStack;

nList nDependencyServer::savedObjects;
nList nDependencyServer::unsavedObjects;

//------------------------------------------------------------------------------
/**
*/
void
nDependencyServer::Init()
{
    counter = 0;
}

//------------------------------------------------------------------------------
/**
    @param filename file from which the object is loaded.
    @param command command that will be used to connect the loaded object
    @returns the loaded object
*/
nObject *
nDependencyServer::GetObject( const nString & filename, const nString & command )
{
    nKernelServer * ks = nKernelServer::Instance();

    nString name( filename.ExtractFileName() );

    name.StripExtension();

    nString objectName;

    objectName.Append( "/usr/deps/" );
    objectName.Append( command );
    objectName.Append( "/" );
    objectName.Append( name );

    nRoot * object = ks->Lookup( objectName.Get() );
    if( ! object )
    {
        object = ks->LoadAs( filename.Get(), objectName.Get() );
    }
    else
    {
        object->AddRef();
    }
    return object;
}

//------------------------------------------------------------------------------
/**
    @param object object to save
    @param command command that link object
    @param path (OUT) the path in which the object will be saved
    @remarks Use of Template Method design pattern. The primitive operation that
    can redefine the subclasses is GetObjectPath.
*/
void
nDependencyServer::SaveObject( nObject * const object, const nString & command, nString & path )
{
    if( ! nDependencyServer::SearchSavedObject( object, path ) )
    {
        if( this->GetObjectPath( object, command, path ) )
        {
            nDependencyServer::InsertObjectToSave( object, path );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @param object object to save
    @param command command that link object
    @param path (OUT) the path in which the object will be saved
    @retval true if the object need to be saved. No need for mantain a list
    of already saved objects. The nDependencyServer always control the already 
    saved objects.
*/
bool
nDependencyServer::GetObjectPath( nObject * const /*object*/, const nString & /*command*/, nString & path )
{
    path.Clear();
    path.Append( nDependencyServer::parentPath );
    path.Append( "depobject" );
    if( this->counter < 100 )
    {
        path.Append( "0" );
        if( this->counter < 10 )
        {
            path.Append( "0" );
        }
    }
    path.Append( this->counter++ );
    path.Append( ".n2" );

    return true;
}

//------------------------------------------------------------------------------
/**
    @remarks if objects are saved and need to be re-saved is neccesary to
    call this function again.
*/
void 
nDependencyServer::InitGlobal()
{
    DepNode * node;
    while( 0 != (node = static_cast<DepNode*>( unsavedObjects.RemHead())) )
    {
        n_delete( node );
    }

    while( 0 != (node = static_cast<DepNode*>( savedObjects.RemHead())) )
    {
        n_delete( node );
    }

    while( ! parentInfoStack.IsEmpty() )
    {
        DepNode *node = parentInfoStack.Pop();
        n_delete(node);
    }

    parentName = "";
    parentPath = "";
}

//------------------------------------------------------------------------------
/**
    @param object to search
    @param path (OUT) the path in which the object will be saved, if found 
    @retval true if object is already saved, and path set.
*/
bool 
nDependencyServer::SearchSavedObject( const nObject * const object, nString & path )
{
    // search the unsaved objects
    DepNode * node = static_cast<DepNode*>( unsavedObjects.GetHead() ); 
    while( node )
    {
        if( node->savedObject == object )
        {
            path = node->savedPath;
            return true;
        }
        node = static_cast<DepNode*>( node->GetSucc() );
    }

    // search the saved nodes
    node = static_cast<DepNode*>( savedObjects.GetHead() );
    while( node )
    {
        if( node->savedObject == object )
        {
            path = node->savedPath;
            return true;
        }
        node = static_cast<DepNode*>( node->GetSucc() );
    }

    // node not saved by now
    return false;
}

//------------------------------------------------------------------------------
/**
    @param object object to save
    @param path path in which the object will be saved
*/
void 
nDependencyServer::InsertObjectToSave( nObject * const object, const nString & path )
{
    DepNode * node = n_new( DepNode );

    node->savedObject = object;
    node->savedPath = path;

    unsavedObjects.AddTail( node );
}

//------------------------------------------------------------------------------
/**
*/
void
nDependencyServer::SaveDependencies()
{
    // save the unsaved objects
    DepNode * node;
    while( 0 != (node = static_cast<DepNode*>( unsavedObjects.RemHead() )) )
    {
        nString path(node->savedPath.ExtractDirName());
        nFileServer2::Instance()->MakePath(path);

        // save the object
        node->savedObject->SaveAs( node->savedPath.Get() );

        // insert in saved objects list
        savedObjects.AddTail( node );
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nDependencyServer::PushParentFileObject( nObject * const object, const nString & name )
{
    DepNode* newDepNode = n_new(DepNode);
    newDepNode->savedObject = object;
    newDepNode->savedPath = name;
    parentInfoStack.Push(newDepNode);

    parentObject = object;
    parentPath = name.ExtractDirName();
    parentName = name.ExtractFileName();
}

//------------------------------------------------------------------------------
/**
*/
void 
nDependencyServer::PopParentFileObject( )
{
    n_assert2(!parentInfoStack.IsEmpty(), "luis.cabellos: nDependencyServer parent info empty");

    DepNode *node = parentInfoStack.Pop();
    n_delete(node);

    nString name;
    if (!parentInfoStack.IsEmpty())
    {
        node = parentInfoStack.Top();
        name = node->savedPath.Get();
        parentObject = node->savedObject;
        parentPath = name.ExtractDirName();
        parentName = name.ExtractFileName();
    }
    else
    {
        parentPath.Clear();
        parentName.Clear();
    }
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
