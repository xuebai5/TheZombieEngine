/**
   @file nkernelserverproxy.h
   @author Luis Jose Cabellos Gomez
   
   (C) 2005 Conjurer Services, S.A.
*/
#ifndef NKERNELSERVERPROXY_H
#define NKERNELSERVERPROXY_H

#include "kernel/ncmdprotonativecpp.h"

#include "signals/nsignalnative.h"

#include "util/nstring.h"

class nRoot;

class nKernelServerProxy : public nRoot
{
public:
    /// constructor
    nKernelServerProxy();
    /// destructor
    ~nKernelServerProxy();

    static nKernelServerProxy * Instance();

    /// take the kernel lock (to make the kernel multithreading safe) 
    void Lock();

    /// release the kernel lock 
    void Unlock();
    /// close and remove a class from the kernel
    void RemClass(const nString &);
    /// create a class object
    const char * CreateClass(const nString &);
    /// release a class object
    void ReleaseClass(const nString &);
    /// find a loaded class object by name
    const char * FindClass(const nString &);
    /// find a loaded class object by name
    bool IsClassLoaded(const nString &);
    /// create a Nebula object, fail hard if no object could be created
    nRoot * New(const char* , const char*);
    /// create a Nebula object, fail hard if no object could be created
    nObject* NewObject(const nString &);
    /// create a Nebula object, don't fail when no object could be created
    nRoot * NewNoFail(const nString & , const nString &);
    /// create a Nebula object, fail hard if no object could be created
    nObject* NewObjectNoFail(const nString &);
    /// create a Nebula object from a persistent object file
    nObject * Load(const nString &);
    /// create a Nebula object from a persistent object file with given name
    nRoot * LoadAs(const nString &, const nString &);
    /// lookup a Nebula object in the hierarchy
    nRoot * Lookup(const nString &);

    /// set current working object
    void SetCwd(nObject*);
    /// get current working Object
    nObject* GetCwd();
    ///  push current working object on stack, and set new cwd
    void PushCwd(nObject*);
    /// restore previous working object
    nObject* PopCwd();
    /// restore previous working object
    void SetLogHandler(nLogHandler*);
    /// get pointer to current log handler object
    nRoot* GetLogHandler();
    /// set an alternative file server
    void ReplaceFileServer(const nString &);
    /// get pointer to file server
    nRoot* GetPersistServer();
    /// get pointer to persist server
    nRoot* GetFileServer();
    /// get pointer to remote server
    nRoot* GetRemoteServer();
    /// get pointer to time server
    nRoot* GetTimeServer();
    /// optionally call to update memory useage variables
    void Trigger();

    /// tell if a class is native
    bool IsClassNative(const char *) const;
    /// tell if a class has a specific command
    bool HasClassCommand(const char *, const char*) const;
    /// get the parent class of the given class
    const char* GetSuperClass(const char *) const;

#ifndef NGAME
    // notify that an object was deleted (not specifies which one), for the GUI
    NSIGNAL_DECLARE( 'LDOB', void, ObjectDeleted, 0, (), 0, () );
    // notify a class was deleted providing the class name
    NSIGNAL_DECLARE( 'BCDE', void, ClassDeleted, 1, (const char *), 0, () );
#endif

private:

    static nKernelServerProxy * singleton;
};

//------------------------------------------------------------------------------
/**
*/
inline
nKernelServerProxy*
nKernelServerProxy::Instance()
{
    return singleton;
}

#endif//NKERNELSERVERPROXY_H
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
