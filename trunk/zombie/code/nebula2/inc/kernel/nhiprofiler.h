#ifndef N_HIPROFILER_H
#define N_HIPROFILER_H
//------------------------------------------------------------------------------
/**
    @class nHiProfiler
    @author Mateu Batle
    @ingroup Time

    @brief nHiProfiler provides an easy way to measure time intervals.
    HiProfiler is a short version of Hierarchical Profiler.
    Hierarchical profilers maintain profiler context information, so the times 
    obtianed are relative to their context profiler. 
    This can be thought as a stack of profilers.


    @todo
    - track maximum recursion depth (and current depth)
    - store a variable context

    How to use:

    First of all, you have to declare the profiler. To do it, use the N_HIPROF_DECLARE(name)
    macro. Don't use it as a member variable of a class. Instead, declare it at the
    beginning of a .cc file, as we do with the LOG system. Note that the profiler will 
    be accessible all around the code, not just in the same file. 
    
    Then, you have to initialize it using the N_HIPROF_INIT(name) macro. 
    Initialization has to be done just once and once all the servers are working, 
    as it needs that the kernel server is up, in order to publish the environment 
    variables with the profiler times and number of calls.

    To start the profiler use the N_HIPROF_START(name) macro, and to stop it
    use the N_HIPROF_STOP(name) macro.

    All the profiler results are published automatically and reseted each frame.

    The flag __NEBULA_NO_PROFILING__ determines whether the profiling code is 
    compiled or not, to make it easy to turn off all profiling code for final
    shippable builds.

    (C) 2006 Conjurer Services, S.A. 
*/
#include "kernel/nprofiler.h"
#include "util/narray.h"
#include "util/nkeyarray.h"
#include "util/nstack.h"

#ifndef __NEBULA_STATS__

//typedef nHiProfiler nProfiler

#define N_HIPROF_DECLARE(name)
#define N_HIPROF_INIT(name)
#define N_HIPROF_START(name)
#define N_HIPROF_STOP(name)

#else

/// declare a hierarchical profiler ( warning: don't use it as a member variable )
#define N_HIPROF_DECLARE(name) \
    nHiProfiler hiProf##name;

/// initialize a hierarchical profiler
#define N_HIPROF_INIT(name) \
    { \
        extern nHiProfiler hiProf##name; \
        hiProf##name.Initialize(#name, true);\
    }

/// start a hierarchical profiler
#define N_HIPROF_START(name) \
    if ( nProfilerManager::Instance()->IsEnabled() ) { \
        extern nHiProfiler hiProf##name;\
        hiProf##name.StartAccum(); \
    }

/// stop a hierarchical profiler
#define N_HIPROF_STOP(name) \
    if ( nProfilerManager::Instance()->IsEnabled() ) { \
        extern nHiProfiler hiProf##name;\
        hiProf##name.StopAccum(); \
    }

//------------------------------------------------------------------------------
class nHiProfiler : public nProfiler
{
public:

    /// default constructor
    nHiProfiler();
    /// constructor 2
    nHiProfiler(const char * name);
    /// destructor
    ~nHiProfiler();

    /// get profiler id
    int GetId() const;
    /// set profiler id
    void SetId(int id);

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
    /// get total accumulated time (in milliseconds)
    float GetAccumTime() const;

    /// get the children time
    nTimeServer::nProfilerCounter GetChildrenTime() const;
    /// get the children
    const nArray<nHiProfiler *> &GetChildren() const;

    /// add information
    //void AddVariable(const char * name, int val);

protected:

    /// profiler id
    int id;
    /// get the proper profiler depending on the parent context
    nProfiler * GetProfilerByContext();
    /// stop all profilers
    void StopAll();
    /// stop accumulate of all profilers
    void StopAccumAll();
    /// add time for children when this profiler is current context
    void AddChildrenTime(nHiProfiler * child, const nTimeServer::nProfilerCounter & pc);
    /// deallocate all resources 
    void Destroy();

    /// for each parent context, have a profiler
    nKeyArray<nProfiler *> contextProfilers;
    /// children time
    nTimeServer::nProfilerCounter childrenTime;
    /// start count
    int startCount;
    /// children profilers
    nArray<nHiProfiler *> children;
    

    ///////////////////////////////////////////////////////////////
    /// static stuff
    ///////////////////////////////////////////////////////////////

    /// push a profiler to the context stack
    static void PushProfiler(nHiProfiler *prof);
    /// pop a profiler from the context stack
    static nHiProfiler * PopProfiler();
    /// get current profiler in the context stack
    static nHiProfiler * GetTopProfiler();
    /// generate a new id
    static int GenerateNewId();

    /// stack of profilers
    static nStack<nHiProfiler *> profilerStack;
    /// id for a new hiProfiler
    static int currentId;
};

//------------------------------------------------------------------------------
/**
    Default constructor
*/
inline
nHiProfiler::nHiProfiler() :
    id(-1),
    contextProfilers(4,4),
    childrenTime(0),
    startCount(0),
    nProfiler()
{
    this->id = nHiProfiler::GenerateNewId();
}

//------------------------------------------------------------------------------
/**
    Constructor 2
*/
inline
nHiProfiler::nHiProfiler(const char * name) :
    id(-1),
    contextProfilers(4,4),
    childrenTime(0),
    startCount(0),
    nProfiler(name, true)
{
    this->id = nHiProfiler::GenerateNewId();
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
inline
nHiProfiler::~nHiProfiler()
{
    this->Destroy();
}

//------------------------------------------------------------------------------
/**
    get profiler id
*/
inline
int 
nHiProfiler::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
    set profiler id
*/
inline
void 
nHiProfiler::SetId(int id)
{
    this->id = id;
}

//------------------------------------------------------------------------------
/**
    get the children time
*/
inline
nTimeServer::nProfilerCounter 
nHiProfiler::GetChildrenTime() const
{
    return this->childrenTime;
}

//------------------------------------------------------------------------------
/**
    get the children
*/
inline
const nArray<nHiProfiler *> &
nHiProfiler::GetChildren() const
{
    return this->children;
}

#endif // __NEBULA_STATS__

//------------------------------------------------------------------------------
#endif //N_HIPROFILER_H
