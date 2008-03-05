#ifndef N_PROFILERMANAGER_H
#define N_PROFILERMANAGER_H
//------------------------------------------------------------------------------
/**
    @class nProfilerManager
    @ingroup Time
    @brief nProfilerManager handles profilers with autoreset option. Basically
    every time trigger is called, the autoreset profilers are reset and 
    published.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "util/narray.h"

//------------------------------------------------------------------------------
class nProfiler;
class nHiProfiler;

//------------------------------------------------------------------------------
class nProfilerManager : public nRoot
{
public:
    /// default constructor
    nProfilerManager();
    /// destructor
    ~nProfilerManager();
    
    /// return nProfilerManager instance (singleton)
    static nProfilerManager * Instance();

    /// register a profiler
    void RegisterProfiler(nProfiler * profiler);
    /// unregister a profiler
    void UnregisterProfiler(nProfiler * profiler);
    /// unregister all registered profilers
    void UnregisterAllProfilers();

    #ifdef __NEBULA_STATS__
    /// begin profiling
    void BeginProfiling();
    /// end profiling
    void EndProfiling();
    /// enable/disable profiling
    void SetEnabled(bool flag);
    /// says if the profiling is enabled or not
    bool IsEnabled() const;
    #endif //__NEBULA_STATS__
    
private:

    void ResetAndPublishProfilers() const;
    
    static nProfilerManager * instance;

    /// keep track of all the autoreset profilers
    nArray<nProfiler *> regProfilers;
    /// root of all profilers
    nHiProfiler * rootContext;

    /// profiling enabled?
    bool enabled;
};

#ifdef __NEBULA_STATS__

//------------------------------------------------------------------------------
/**
    enable/disable profiling
*/
inline
void 
nProfilerManager::SetEnabled(bool flag)
{
    this->enabled = flag;
}

//------------------------------------------------------------------------------
/**
    says if the profiling is enabled or not
*/
inline
bool
nProfilerManager::IsEnabled() const
{
    return this->enabled;
}

#endif //__NEBULA_STATS__

//------------------------------------------------------------------------------
#endif
