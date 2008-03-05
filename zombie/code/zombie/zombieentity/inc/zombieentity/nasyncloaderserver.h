#ifndef N_ASYNCLOADERSERVER_H
#define N_ASYNCLOADERSERVER_H
//------------------------------------------------------------------------------
/**
    @class nAsyncLoaderServer
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>
    
    @brief Loads entity resources in a background thread.
    
    (C) 2005 Conjurer Services, S.A.
*/

#include "zombieentity/nloaderserver.h"
#include "kernel/nthread.h"
#include "kernel/nthreadsafelist.h"

//------------------------------------------------------------------------------
class nAsyncLoaderServer : public nLoaderServer
{
public:
    /// constructor
    nAsyncLoaderServer();
    /// destructor
    ~nAsyncLoaderServer();

    /// trigger loading
    virtual void Trigger();

protected:
    /// add a resource to the loader job list
    void AddLoaderJob(ncLoader* loader);
    /// remove a resource from the loader job list
    void RemLoaderJob(ncLoader* loader);
    /// start the loader thread
    void StartLoaderThread();
    /// shutdown the loader thread
    void ShutdownLoaderThread();
    /// the loader thread function
    static int N_THREADPROC LoaderThreadFunc(nThread* thread);
    /// the thread wakeup function
    static void ThreadWakeupFunc(nThread* thread);

    nThreadSafeList jobList;    // list for outstanding background loader jobs
    nThread* loaderThread;      // background thread for handling async resource loading
};

//------------------------------------------------------------------------------
#endif /* N_ASYNCLOADERSERVER_H */
