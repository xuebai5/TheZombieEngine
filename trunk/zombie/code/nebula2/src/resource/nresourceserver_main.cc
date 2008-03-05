#include "precompiled/pchnnebula.h"
//------------------------------------------------------------------------------
//  nresourceserver_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnnebula.h"
#include "resource/nresourceserver.h"
#include "resource/nresource.h"
#include "kernel/nprofiler.h"

nNebulaScriptClass(nResourceServer, "nroot");
nResourceServer* nResourceServer::Singleton = 0;
nProfiler nResourceServer::profAsyncIO;

//------------------------------------------------------------------------------
NSIGNAL_DEFINE(nResourceServer, ResourceReloaded);

//------------------------------------------------------------------------------
/**
*/
nResourceServer::nResourceServer() :
    meshPool("/sys/share/rsrc/mesh"),
    texPool("/sys/share/rsrc/tex"),
    shdPool("/sys/share/rsrc/shd"),
    animPool("/sys/share/rsrc/anim"),
    sndResPool("/sys/share/rsrc/sndrsrc"),
    sndInstPool("/sys/share/rsrc/sndinst"),
    fontPool("/sys/share/rsrc/font"),
    bundlePool("/sys/share/rsrc/bundle"),
    otherPool("/sys/share/rsrc/other"),
    uniqueId(0),
    loaderThread(0)
{
    n_assert(0 == Singleton);
    Singleton = this;

    if (!profAsyncIO.IsValid())
    {
        profAsyncIO.Initialize("profAsyncIO", true);
    }

    this->resourceClass = kernelServer->FindClass("nresource");
    n_assert(this->resourceClass);

    #ifndef __NEBULA_NO_THREADS__
    this->StartLoaderThread();
    #endif
}

//------------------------------------------------------------------------------
/**
*/
nResourceServer::~nResourceServer()
{
    n_assert(Singleton);
    Singleton = 0;

    #ifndef __NEBULA_NO_THREADS__
    this->ShutdownLoaderThread();
    #endif
    this->UnloadResources(nResource::AllResourceTypes);
}

//------------------------------------------------------------------------------
/**
    Create a resource id from a resource name. The resource name is usually
    just the filename of the resource file. The method strips off the last
    32 characters from the resource name, and replaces any invalid characters
    with underscores. It is valid to provide a 0-rsrcName for unshared resources.
    A unique rsrc identifier string will then be created.

    @param  rsrcName    pointer to a resource name (usually a file path), or 0
    @param  buf         pointer to a char buffer
    @param  bufSize     size of char buffer
    @return             a pointer to buf, which contains the result
*/
char*
nResourceServer::GetResourceId(const char* rsrcName, char* buf, int N_IFDEF_ASSERTS(bufSize) )
{
    n_assert(buf);
    n_assert(bufSize >= N_MAXNAMELEN);

    if (!rsrcName)
    {
        sprintf(buf, "unique%d", this->uniqueId++);
    }
    else
    {
        int len = static_cast<int>( strlen(rsrcName) + 1 );
        int offset = len - N_MAXNAMELEN;
        if (offset < 0)
        {
            offset = 0;
        }

        // copy string and replace illegal characters, this also copies the terminating 0
        char c;
        const char* from = rsrcName;//&(rsrcName[offset]);
        char* to   = buf;
        while ( 0 != (c = *from++))
        {
            if (('.' == c) || (c == '/') || (c == ':') || (c == '\\'))
            {
                *to++ = '_';
            }
            else
            {
                *to++ = c;
            }
        }
        *to = 0;
    }
    return buf;
}

//------------------------------------------------------------------------------
/**
    Find the right resource root object for a given resource type.

    @param  rsrcType    the resource type
    @return             the root object
*/
nResourcePool*
nResourceServer::GetResourcePool(nResource::Type rsrcType)
{
    switch (rsrcType)
    {
        case nResource::Mesh:              return &this->meshPool;
        case nResource::Texture:           return &this->texPool;
        case nResource::Shader:            return &this->shdPool;
        case nResource::Animation:         return &this->animPool;
        case nResource::SoundResource:     return &this->sndResPool;
        case nResource::SoundInstance:     return &this->sndInstPool;
        case nResource::Font:              return &this->fontPool;
        case nResource::Bundle:            return &this->bundlePool;
        case nResource::Other:             return &this->otherPool;

        default:
            // can't happen
            n_assert_always();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Find a resource object by resource type and name.

    @param  rsrcName    the rsrc name
    @param  rsrcType    resource type
    @return             pointer to resource object, or 0 if not found
*/
nResource*
nResourceServer::FindResource(const char* rsrcName, nResource::Type rsrcType)
{
    n_assert(rsrcName);
    n_assert(nResource::InvalidResourceType != rsrcType);

    nResourcePool* rsrcPool = this->GetResourcePool(rsrcType);
    n_assert(rsrcPool);

    return rsrcPool->Find(rsrcName);
}

//------------------------------------------------------------------------------
/**
    Find a resource object by resource filename.

    @param  rsrcName    the rsrc filename
    @return             pointer to resource object, or 0 if not found
*/
nResource*
nResourceServer::FindResourceByFileName(const char* fileName)
{
    nFileServer2 *fileServer = nKernelServer::Instance()->GetFileServer();
    nString strFilename(fileServer->ManglePath(fileName));
    #ifdef __WIN32__
    strFilename.ToLower();
    #endif

    int i;
    for (i = 1; i < nResource::InvalidResourceType; i <<= 1)
    {
        if (0 != (nResource::AllResourceTypes & i))
        {
            nResourcePool* rsrcPool = this->GetResourcePool((nResource::Type) i);
            n_assert(rsrcPool);
            nResource* rsrc;
            for (rsrc = (nResource*) rsrcPool->GetHead(); rsrc; rsrc = (nResource*) rsrc->GetSucc())
            {
                nString curFilename(fileServer->ManglePath(rsrc->GetFilename().Get()));
                #ifdef __WIN32__
                curFilename.ToLower();
                #endif
                if (curFilename == strFilename)
                {
                    return rsrc;
                }
            }
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nResourceServer::ReloadResource(const char* rsrcName, nResource::Type rsrcType)
{
    nResource* resource = this->FindResource(rsrcName, rsrcType);
    if (resource)
    {
        if (resource->IsValid())
        {
            resource->Unload();
            if (resource->Load())
            {
#ifndef N_GAME
                this->SignalResourceReloaded(this, resource);
#endif
                return true;
            }
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
nResourceServer::ReloadResourceByFileName(const char* fileName)
{
    nResource* resource = this->FindResourceByFileName(fileName);
    if (resource)
    {
        if (resource->IsValid())
        {
            if (resource->GetReloadOnChange())
            {
                resource->Unload();
                if (resource->Load())
                {
#ifndef N_GAME
                    this->SignalResourceReloaded(this, resource);
#endif
                    return true;
                }
                return false;
            }
            else
            {
                resource->SetReloadOnChange(true);
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    Create a new possible shared resource object. Bumps refcount on an 
    existing resource object. Pass a zero rsrcName if a (non-shared) resource 
    should be created.

    @param  className   the Nebula class name
    @param  rsrcName    the rsrc name (for resource sharing), can be 0
    @param  rsrcType    resource type
    @return             pointer to resource object
*/
nResource*
nResourceServer::NewResource(const char* className, const char* rsrcName, nResource::Type rsrcType)
{
    n_assert(className);
    n_assert(nResource::InvalidResourceType != rsrcType);

    nResourcePool* rsrcPool = this->GetResourcePool(rsrcType);
    n_assert(rsrcPool);

    nResource* obj = rsrcPool->NewResource(className, rsrcName);
    n_assert(obj);
    return obj;
}

//------------------------------------------------------------------------------
/**
    Unload all resources matching the given resource type mask.

    @param  rsrcTypeMask    a mask of nResource::Type values
*/
void
nResourceServer::UnloadResources(int rsrcTypeMask)
{
    // also unload bundles?
    if (0 != (rsrcTypeMask & (nResource::Mesh | nResource::Animation | nResource::Texture)))
    {
        rsrcTypeMask |= nResource::Bundle;
    }
    int i;
    for (i = 1; i < nResource::InvalidResourceType; i <<= 1)
    {
        if (0 != (rsrcTypeMask & i))
        {
            nResourcePool* rsrcPool = this->GetResourcePool((nResource::Type) i);
            n_assert(rsrcPool);
            nResource* rsrc;
            for (rsrc = (nResource*) rsrcPool->GetHead(); rsrc; rsrc = (nResource*) rsrc->GetSucc())
            {
                rsrc->Unload();
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Load all resources matching the given resource type mask. Returns false
    if any of the resources didn't load correctly.

    IMPLEMENTATION NOTE: since the Bundle resource type is defined
    before all other resource types, it is guaranteed that bundled
    resources are loaded before all others. 

    @param  rsrcTypeMask  a resource type
    @return               true if all resources loaded correctly
*/
bool
nResourceServer::LoadResources(int rsrcTypeMask)
{
    // also reload bundles?
    if (0 != (rsrcTypeMask & (nResource::Mesh | nResource::Animation | nResource::Texture)))
    {
        rsrcTypeMask |= nResource::Bundle;
    }

    int i;
    bool retval = true;
    for (i = 1; i < nResource::InvalidResourceType; i <<= 1)
    {
        if (0 != (rsrcTypeMask & i))
        {
            nResourcePool* rsrcPool = this->GetResourcePool((nResource::Type) i);
            n_assert(rsrcPool);

            nResource* rsrc;
            for (rsrc = (nResource*) rsrcPool->GetHead(); rsrc; rsrc = (nResource*) rsrc->GetSucc())
            {
                // NOTE: if the resource is bundled, it could've been loaded already
                // (if this is the actual resource object which has been created by the
                // bundle, thus we check if the resource has already been loaded)
                if (!rsrc->IsLoaded())
                {
                    retval &= rsrc->Load();
                }
            }
        }
    }
    return retval;
}

//------------------------------------------------------------------------------
/**
    Calls nResource::OnLost() on all resources defined in the resource
    type mask.

    @param  rsrcTypeMask    a mask of nResource::Type values
*/
void
nResourceServer::OnLost(int rsrcTypeMask)
{
    int i;
    for (i = 1; i < nResource::InvalidResourceType; i <<= 1)
    {
        if (0 != (rsrcTypeMask & i))
        {
            nResourcePool* rsrcPool = this->GetResourcePool((nResource::Type) i);
            n_assert(rsrcPool);
            nResource* rsrc;
            for (rsrc = (nResource*) rsrcPool->GetHead(); rsrc; rsrc = (nResource*) rsrc->GetSucc())
            {
                NLOGCOND(resource, rsrc->IsLost(), (0, "Resource already lost name=%s filename=%s", rsrc->GetName(), rsrc->GetFilename().Get()));
                if (!rsrc->IsLost())
                {
                    rsrc->OnLost();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Calls nResource::OnRestored() on all resources defined in the resource
    type mask.

    @param  rsrcTypeMask    a resource type
*/
void
nResourceServer::OnRestored(int rsrcTypeMask)
{
    int i;
    for (i = 1; i < nResource::InvalidResourceType; i <<= 1)
    {
        if (0 != (rsrcTypeMask & i))
        {
            nResourcePool* rsrcPool = this->GetResourcePool((nResource::Type) i);
            n_assert(rsrcPool);

            nResource* rsrc;
            for (rsrc = (nResource*) rsrcPool->GetHead(); rsrc; rsrc = (nResource*) rsrc->GetSucc())
            {
                if (rsrc->IsLost())
                {
                    rsrc->OnRestored();
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Wakeup the loader thread. This will simply signal the jobList.
*/
void
nResourceServer::ThreadWakeupFunc(nThread* thread)
{
    nResourceServer* self = (nResourceServer*) thread->LockUserData();
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
nResourceServer::LoaderThreadFunc(nThread* thread)
{
    nProfiler profLoadOp;

    // tell thread object that we have started
    thread->ThreadStarted();

    // get pointer to thread server object
    nResourceServer* self = (nResourceServer*) thread->LockUserData();
    thread->UnlockUserData();

    // sit on the jobList signal until new jobs arrive
    do
    {
        // do nothing until job list becomes signalled
        self->jobList.WaitEvent();

        // does our boss want us to shut down?
        if (!thread->ThreadStopRequested())
        {
            profAsyncIO.StartAccum();

            // get all pending jobs
            while (self->jobList.GetHead())
            {
                // keep the job object from joblist
                self->jobList.Lock();
                nNode* jobNode = self->jobList.RemHead();
                nResource* res = (nResource*) jobNode->GetPtr();
   
                // take the resource's mutex and lock the resource,
                // this prevents the resource to be deleted
                res->LockMutex();
                self->jobList.Unlock();

                profLoadOp.ResetAccum();
                profLoadOp.StartAccum();
                res->LoadResource();
                profLoadOp.StopAccum();
                NLOG(resource, (nResource::NLOG_ASYNCIO, "loaded time=%3.1f ms resource %s file %s ", profLoadOp.GetAccumTime(), res->GetName(), res->GetFilename().Get()));

                res->UnlockMutex();

                // proceed to next job
            }

            profAsyncIO.StopAccum();
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
nResourceServer::StartLoaderThread()
{
    n_assert(0 == this->loaderThread);

    // give the thread sufficient stack size (2.5 MB) and a below
    // normal priority (the purpose of the thread is to guarantee 
    // a smooth framerate despite dynamic resource loading after all)
    this->loaderThread = n_new(nThread(LoaderThreadFunc, nThread::Low, 2500000, ThreadWakeupFunc, 0, this));
}

//------------------------------------------------------------------------------
/**
    Shutdown the loader thread.
*/
void
nResourceServer::ShutdownLoaderThread()
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
    Add a resource to the job list for asynchronous loading.
*/
void
nResourceServer::AddLoaderJob(nResource* res)
{
    n_assert(res);
    n_assert(!res->IsPending());
    n_assert(!res->IsLoaded());
    this->jobList.Lock();
    this->jobList.AddTail(&(res->jobNode));
    this->jobList.Unlock();
    this->jobList.SignalEvent();
}

//------------------------------------------------------------------------------
/**
    Remove a resource from the job list for asynchronous loading.
*/
void
nResourceServer::RemLoaderJob(nResource* res)
{
    n_assert(res);
    this->jobList.Lock();
    if (res->IsPending())
    {
        res->jobNode.Remove();
    }
    this->jobList.Unlock();
}

//------------------------------------------------------------------------------
/**
    Count the number of resources of a given type.
*/
int
nResourceServer::GetNumResources(nResource::Type rsrcType)
{
    nResourcePool* rsrcPool = this->GetResourcePool(rsrcType);
    n_assert(rsrcPool);
    return rsrcPool->GetNumResources();
}

//------------------------------------------------------------------------------
/**
    Returns the number of bytes a resource type occupies in RAM.
*/
int
nResourceServer::GetResourceByteSize(nResource::Type rsrcType)
{
    nResourcePool* rsrcPool = this->GetResourcePool(rsrcType);
    n_assert(rsrcPool);
    nRoot* cur;
    int size = 0;
    for (cur = rsrcPool->GetHead(); cur; cur = cur->GetSucc())
    {
        n_assert(cur->IsA(this->resourceClass));
        nResource* res = (nResource*) cur;
        size += res->GetByteSize();
    }
    return size;
}
