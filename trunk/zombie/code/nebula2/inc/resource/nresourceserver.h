#ifndef N_RESOURCESERVER_H
#define N_RESOURCESERVER_H
//------------------------------------------------------------------------------
/**
    @class nResourceServer
    @ingroup Resource
    @brief Central resource server. Creates and manages resource objects.
    
    Resources are objects which provide several types of data (or data
    streams) to the application, and can unload and reload themselves
    on request.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "resource/nresource.h"
#include "resource/nresourcepool.h"
#include "kernel/nref.h"
#include "signals/nsignalnative.h"
#include "kernel/nprofiler.h"

//------------------------------------------------------------------------------
class nResourceServer : public nRoot
{
public:
    /// constructor
    nResourceServer();
    /// destructor
    virtual ~nResourceServer();
    /// return singleton instance pointer
    static nResourceServer* Instance();
    /// find a resource object by its name and type
    virtual nResource* FindResource(const char* rsrcName, nResource::Type rsrcType);
    /// find a resource object by its filename
    virtual nResource* FindResourceByFileName(const char* fileName);
    /// create a resource object
    virtual nResource* NewResource(const char* className, const char* rsrcName, nResource::Type rsrcType);
    /// reload a resource object
    virtual bool ReloadResource(const char* rsrcName, nResource::Type rsrcType);
    /// find and reload a resource object by filename
    virtual bool ReloadResourceByFileName(const char* fileName);
    /// unload all resources matching resource type (OR'ed nResource::Type's)
    virtual void UnloadResources(int resTypeMask);
    /// reload all resources matching type
    virtual bool LoadResources(int resTypeMask);
    /// call OnLost() on all resources defined in the mask
    virtual void OnLost(int resTypeMask);
    /// call OnRestored() on all resources defined in the mask
    virtual void OnRestored(int resTypeMask);
    /// return the resource pool directory for a given resource type
    nResourcePool* GetResourcePool(nResource::Type rsrcType);
    /// generate a valid resource id from a resource path
    char* GetResourceId(const char* rsrcName, char* buf, int bufSize);

    /// return number of resources of given type
    int GetNumResources(nResource::Type rsrcType);
    /// get number of bytes of RAM occupied by resource type
    int GetResourceByteSize(nResource::Type rsrcType);

    /// @name Signals interface
    //@{
    NSIGNAL_DECLARE('RSRL', void, ResourceReloaded, 1, (nResource*), 0, ());
    //@}

protected:
    friend class nResource;
    static nResourceServer* Singleton;

    /// add a resource to the loader job list
    void AddLoaderJob(nResource* res);
    /// remove a resource from the loader job list
    void RemLoaderJob(nResource* res);
    /// start the loader thread
    void StartLoaderThread();
    /// shutdown the loader thread
    void ShutdownLoaderThread();
    /// the loader thread function
    static int N_THREADPROC LoaderThreadFunc(nThread* thread);
    /// the thread wakeup function
    static void ThreadWakeupFunc(nThread* thread);

    int uniqueId;

    nResourcePool meshPool;
    nResourcePool texPool;
    nResourcePool shdPool;
    nResourcePool animPool;
    nResourcePool sndResPool;
    nResourcePool sndInstPool;
    nResourcePool fontPool;
    nResourcePool bundlePool;
    nResourcePool otherPool;

    nThreadSafeList jobList;    // list for outstanding background loader jobs
    nThread* loaderThread;      // background thread for handling async resource loading

    nClass* resourceClass;

    static nProfiler profAsyncIO;
};

//------------------------------------------------------------------------------
/**
*/
inline
nResourceServer*
nResourceServer::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
#endif
