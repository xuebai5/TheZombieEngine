#include "precompiled/pchncshared.h"
//------------------------------------------------------------------------------
//  nasyncloaderserver_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nasyncloaderserver.h"

nNebulaScriptClass(nAsyncLoaderServer, "nroot");

//------------------------------------------------------------------------------
/**
    constructor
*/
nAsyncLoaderServer::nAsyncLoaderServer() :
    loaderThread(0)
{
    #ifndef __NEBULA_NO_THREADS__
    this->StartLoaderThread();
    #endif
}

//------------------------------------------------------------------------------
/**
    destructor
*/
nAsyncLoaderServer::~nAsyncLoaderServer()
{
    #ifndef __NEBULA_NO_THREADS__
    this->ShutdownLoaderThread();
    #endif
}

//------------------------------------------------------------------------------
/**
    trigger
*/
void
nAsyncLoaderServer::Trigger()
{
    #ifndef __NEBULA_NO_THREADS__
    int i;
    for (i = 0; i < this->pendingEntities.Size(); ++i)
    {
        if (this->pendingEntities[i].isvalid())
        {
            
            nEntityObject* entity = this->pendingEntities[i].get();
            //ncLoaderClass* loaderClass = entity->GetComponent<ncLoaderClass>();
            //if (loaderClass && loaderClass->GetAsyncEnabled())
            {
                this->AddLoaderJob(entity->GetComponentSafe<ncLoader>());
            }
            //else
            //{
            //    this->LoadEntity(entity);
            //}
        }
    }
    this->pendingEntities.Reset();
    #else
    nLoaderServer::Trigger();
    #endif
}

//------------------------------------------------------------------------------
/**
    Wakeup the loader thread. This will simply signal the jobList.
*/
void
nAsyncLoaderServer::ThreadWakeupFunc(nThread* thread)
{
    nAsyncLoaderServer* self = (nAsyncLoaderServer*) thread->LockUserData();
    thread->UnlockUserData();
    self->jobList.SignalEvent();
}

//------------------------------------------------------------------------------
/**
    The background loader thread func. This will sit on the jobList until
    it is signaled (when new jobs arrive), and for each job in the job
    list, it will invoke the LoadResource() method of the resource object
    and remove the resource object from the job list.
*/
int
N_THREADPROC
nAsyncLoaderServer::LoaderThreadFunc(nThread* thread)
{
    // tell thread object that we have started
    thread->ThreadStarted();

    // get pointer to thread server object
    nAsyncLoaderServer* self = (nAsyncLoaderServer*) thread->LockUserData();
    thread->UnlockUserData();

    // sit on the jobList signal until new jobs arrive
    do
    {
        // do nothing until job list becomes signalled
        self->jobList.WaitEvent();

        // does our boss want us to shut down?
        if (!thread->ThreadStopRequested())
        {
            // get all pending jobs
            while (self->jobList.GetHead())
            {
                // keep the job object from joblist
                self->jobList.Lock();
                nNode* jobNode = self->jobList.RemHead();
                ncLoader* loader = (ncLoader*) jobNode->GetPtr();
                
                // take the resource's mutex and lock the resource,
                // this prevents the resource to be deleted
                loader->GetEntityObject()->LockMutex();
                self->jobList.Unlock();

                //...load class resources before
                //loader->LoadComponents();
                self->LoadEntityResources(loader->GetEntityObject());

                loader->GetEntityObject()->UnlockMutex();

                // proceed to next job
            }
        }
    }
    while (!thread->ThreadStopRequested());

    // tell thread object that we are done
    thread->ThreadHarakiri();

    return 0;
}

//------------------------------------------------------------------------------
/**
    Start the loader thread.
*/
void
nAsyncLoaderServer::StartLoaderThread()
{
    n_assert(0 == this->loaderThread);

    // give the thread sufficient stack size (2.5 MB) and a below
    // normal priority (the purpose of the thread is to guarantee 
    // a smooth framerate despite dynamic resource loading after all)
    this->loaderThread = n_new(nThread(LoaderThreadFunc, nThread::Normal, 2500000, ThreadWakeupFunc, 0, this));
}

//------------------------------------------------------------------------------
/**
    Shutdown the loader thread.
*/
void
nAsyncLoaderServer::ShutdownLoaderThread()
{
    n_assert(this->loaderThread);
    n_delete(this->loaderThread);
    this->loaderThread = 0;

    // clear the job list
    this->jobList.Lock();
    while (this->jobList.RemHead());
    this->jobList.Unlock();
}

//------------------------------------------------------------------------------
/**
*/
void
nAsyncLoaderServer::AddLoaderJob(ncLoader* loader)
{
    n_assert(loader);
    //n_assert(!loader->IsPending());
    //n_assert(!loader->AreComponentsValid());
    //<TEMP>
    if (loader->IsPending() || loader->AreComponentsValid())
    {
        return;
    }
    //</TEMP>
    this->jobList.Lock();
    this->jobList.AddTail(&(loader->jobNode));
    this->jobList.Unlock();
    this->jobList.SignalEvent();
}

//------------------------------------------------------------------------------
/**
*/
void
nAsyncLoaderServer::RemLoaderJob(ncLoader* loader)
{
    n_assert(loader);
    this->jobList.Lock();
    if (loader->IsPending())
    {
        loader->jobNode.Remove();
    }
    this->jobList.Unlock();
}
