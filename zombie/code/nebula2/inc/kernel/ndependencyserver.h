#ifndef N_DEPENDENCYSERVER_H
#define N_DEPENDENCYSERVER_H
//------------------------------------------------------------------------------
/**
   @file ndependencyserver.h
   @author Luis Jose Cabellos Gomez
   @brief Declaration of nDependencyServer class
   @brief $Id$ 
   
   (C) 2005 Conjurer Services, S.A.
*/

#include "kernel/nroot.h"
#include "util/nstack.h"

class nString;

//------------------------------------------------------------------------------
/**
    @class nDependencyServer
    @ingroup NebulaDependencyServer
    @brief nDependencyServer allow to disconnect the dependencies from an object to 
    another.

    Using the commands of nObject SetDependency and SetDependencyObject, the 
    Save process can obtain where a nObject will be saved and in the load 
    process can obtain where a file will be loaded.
*/
class nDependencyServer : public nRoot
{
public:
    /// constructor
    nDependencyServer();
    /// destructor
    virtual ~nDependencyServer();

    /// Init the dependency server
    virtual void Init();
    /// Get a object from a file if it isn't loaded
    virtual nObject * GetObject( const nString & filename, const nString & command );
    
    /// save an object to a file and return the path where the object is saved
    void SaveObject( nObject * const object, const nString & command , nString & path);

    /// init the list of object to save an already saved
    static void InitGlobal();
    /// push the caller object and path to use in dependencies generator
    static void PushParentFileObject( nObject * const object, const nString & name );
    /// pop the caller object and path to use in dependencies generator
    static void PopParentFileObject( );
    /// save the objects unsaved
    static void SaveDependencies();

protected:
    /// get the path in which the object will be saved
    virtual bool GetObjectPath( nObject * const object, const nString & command, nString & path );

    static nString parentName;
    static nString parentPath;
    static nObject* parentObject;

private:
    /// Search for an object in the to save or already saved lists
    static bool SearchSavedObject( const nObject * const object, nString & path );
    /// insert the object in the to save list
    static void InsertObjectToSave( nObject * const object, const nString & path );

    int counter;

    /// node with an object and where will be saved
    struct DepNode : nNode
    {
        nObject * savedObject;
        nString savedPath;
    };

    /// objects already saved
    static nList savedObjects;
    /// objects to save
    static nList unsavedObjects;
    /// parent information
    //static nStack<nString> parentInfoStack;
    static nStack<DepNode*> parentInfoStack;
};

//------------------------------------------------------------------------------
/**
*/
inline
nDependencyServer::nDependencyServer()
{
    this->Init();
}

//------------------------------------------------------------------------------
/**
*/
inline
nDependencyServer::~nDependencyServer()
{
}

#endif // N_DEPENDENCYSERVER_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
