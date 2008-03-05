#ifndef N_PROFILER_H
#define N_PROFILER_H
//------------------------------------------------------------------------------
/**
    @class nProfiler
    @ingroup Time
    @brief nProfiler provides an easy way to measure time intervals.

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "kernel/nref.h"
#include "kernel/nenv.h"
#include "kernel/ntimeserver.h"
#include "kernel/nprofilermanager.h"

#ifdef __NEBULA_STATS__
//------------------------------------------------------------------------------
#define NPROFILER_PREFIX    "/sys/var/"

//------------------------------------------------------------------------------
class nProfiler
{
public:
    /// default constructor
    nProfiler();
    /// constuctor
    nProfiler(const char* name, bool autoreset = false);
    /// destructor
    ~nProfiler();

    /// initialize the profiler (if default constructor used)
    void Initialize(const char* name, bool autoreset = false);
    /// Get name of the profiler
    const char * GetName() const;

    /// return true if profiler has been initialized
    bool IsValid() const;
    /// start one-shot profiling 
    void Start();
    /// stop one-shot profiling, value is written to watcher variable
    const nTimeServer::nProfilerCounter Stop();
    /// return true if profiler has been started
    bool IsStarted() const;
    /// publish profiler values to environment variables
    void Publish() const;

    /// reset the accumulator
    void ResetAccum();
    /// start accumulated profiling
    void StartAccum();
    /// stop accumulated profiling (without publish to environment variable)
    const nTimeServer::nProfilerCounter StopAccum();
    /// get accumulated time (in milliseconds)
    float GetAccumTime() const;

    /// return number of calls to the profiler
    unsigned int GetNumCalls() const;

protected:

    /// profiler name
    nString name;
    
    /// nroot variable containing time information for this profiler
    nRef<nEnv> refEnv;
    bool isStarted;
    bool autoReset;
    /// total number of calls (for accumulated profiling)
    unsigned int numCalls;
    /// nroot variable containing number of calls information for this profiler
    nRef<nEnv> refEnvNumCalls;
    nTimeServer::nProfilerCounter hrCounterStart;
    nTimeServer::nProfilerCounter hrCounterAccum;

};

//------------------------------------------------------------------------------
/**
*/
inline
nProfiler::nProfiler() :
    isStarted(false),
    autoReset(false),
    hrCounterStart(0),
    hrCounterAccum(0),
    numCalls(0)
{
    /// empty
}

//------------------------------------------------------------------------------
/**
*/
inline 
nProfiler::nProfiler(const char* name, bool autoreset)
{
    this->Initialize(name, autoreset);
}

//------------------------------------------------------------------------------
/**
*/
inline 
nProfiler::~nProfiler()
{
    // empty
    if (this->autoReset)
    {
        if (nProfilerManager::Instance())
        {
            nProfilerManager::Instance()->UnregisterProfiler(this);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nProfiler::Initialize(const char* name, bool autoreset)
{
    n_assert(name);
    char buf[N_MAXPATH];

    this->name = name;

    // create nroot variable to hold time info
    snprintf(buf, sizeof(buf), NPROFILER_PREFIX "%s", name);
    this->refEnv = (nEnv*) nKernelServer::Instance()->Lookup(buf);
    if (!this->refEnv.isvalid())
    {
        this->refEnv = (nEnv *) nKernelServer::Instance()->New("nenv",buf);
    }

    // create nroot variable to hold number of calls info
    snprintf(buf, sizeof(buf), NPROFILER_PREFIX "%s_NumCalls", name);
    this->refEnvNumCalls = (nEnv*) nKernelServer::Instance()->Lookup(buf);
    if (!this->refEnvNumCalls.isvalid())
    {
        this->refEnvNumCalls = (nEnv *) nKernelServer::Instance()->New("nenv",buf);
    }

    this->autoReset = autoreset;
    if (this->autoReset && nProfilerManager::Instance())
    {
        nProfilerManager::Instance()->RegisterProfiler(this);
    }

    this->ResetAccum();
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nProfiler::GetName() const
{
    return this->name.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nProfiler::ResetAccum()
{
    this->isStarted = false;
    this->hrCounterStart = 0;
    this->hrCounterAccum = 0;
    this->numCalls = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nProfiler::IsValid() const
{
    return this->refEnv.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nProfiler::IsStarted() const
{
    return this->isStarted;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
nProfiler::Start() 
{
    n_assert(!this->IsStarted());
    if (this->IsStarted())
    {
        this->Stop();
    }

    this->StartAccum();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nProfiler::StartAccum()
{
    n_assert(!this->IsStarted());
    if (this->IsStarted())
    {
        this->StopAccum();
    }

    this->hrCounterStart = nTimeServer::Instance()->GetProfilerCounter();
    this->isStarted = true;
    this->numCalls++;
}

//------------------------------------------------------------------------------
/**
    Stop the profiler timer and returns the elapsed time since start was called
*/
inline 
const nTimeServer::nProfilerCounter
nProfiler::Stop() 
{
    nTimeServer::nProfilerCounter ret = 0;
    n_assert(this->IsStarted())
    if (this->IsStarted())
    {
        this->hrCounterAccum = 0;
        ret = this->StopAccum();
        this->Publish();
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
    Stop the profiler timer accumulator and returns the elapsed time since 
    start was called (which is different from the total accumulate time)
*/
inline
const nTimeServer::nProfilerCounter
nProfiler::StopAccum()
{
    nTimeServer::nProfilerCounter ret = 0;

    n_assert(this->IsStarted());
    if (this->IsStarted())
    {
        ret = nTimeServer::Instance()->GetProfilerCounter();
        ret -= this->hrCounterStart;
        this->hrCounterAccum += ret;
        this->isStarted = false;
    }

    return ret;
}

//------------------------------------------------------------------------------
/**
Publish accumulate time in environment variable
*/
inline
void
nProfiler::Publish() const
{
    n_assert(!this->IsStarted());
    this->refEnv->SetF(this->GetAccumTime());
    this->refEnvNumCalls->SetI(this->GetNumCalls());
}

//------------------------------------------------------------------------------
/**
    get accumulated time (in milliseconds)
*/
inline
float
nProfiler::GetAccumTime() const
{
    n_assert(!this->IsStarted());
    nTime accumTime = nTimeServer::Instance()->GetTimeFromProfilerCounter(this->hrCounterAccum);
    return static_cast<float>(accumTime) * 1000.0f;
}

//------------------------------------------------------------------------------
inline
unsigned int 
nProfiler::GetNumCalls() const
{
    return this->numCalls;
}

#else
//------------------------------------------------------------------------------
/**
    Cheap way to have inexpensive profilers when __NEBULA_STATS__ is not defined
*/
class nProfiler
{
public:
    /// default constructor
    nProfiler() {}
    /// constuctor
    nProfiler(const char* /*name*/, bool /*autoreset */= false) {}
    /// destructor
    ~nProfiler() {}

    /// initialize the profiler (if default constructor used)
    void Initialize(const char* /*name*/, bool /*autoreset*/ = false) {}

    /// return true if profiler has been initialized
    bool IsValid() const { return true; }
    /// start one-shot profiling 
    void Start() {}
    /// stop one-shot profiling, value is written to watcher variable
    void Stop() {}
    /// return true if profiler has been started
    bool IsStarted() const { return false; }
    /// publish profiler values to environment variables
    void Publish() const {}

    /// reset the accumulator
    void ResetAccum() {}
    /// start accumulated profiling
    void StartAccum() {}
    /// stop accumulated profiling (without publish to environment variable)
    void StopAccum() {}
    /// get accumulated time (in milliseconds)
    float GetAccumTime() const { return 0; }

    /// return number of calls to the profiler
    unsigned int GetNumCalls() const { return 0; }

};

#endif

//------------------------------------------------------------------------------
#endif
