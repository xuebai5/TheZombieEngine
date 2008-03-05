#ifndef N_TIMESERVER_H
#define N_TIMESERVER_H
//------------------------------------------------------------------------------
/**
    @class nTimeServer
    @ingroup Time
    @brief nTimeServer provides a high resolution time source.

    See also @ref N2ScriptInterface_ntimeserver

    (C) 2002 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

#ifdef __XBxX__
#   include "xbox/nxbwrapper.h"
#elif __WIN32__
#   ifndef _INC_WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h> 
#   endif
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "kernel/nenv.h"

//------------------------------------------------------------------------------
class nTimeServer : public nRoot 
{
public:

    /// Counter used for time
    typedef nuint64 nTimeCounter;
    /// Counter used for profiling
    typedef nuint64 nProfilerCounter;

    /// constructor
    nTimeServer();
    /// destructor
    virtual ~nTimeServer();
    /// get instance pointer
    static nTimeServer* Instance();
    /// trigger time server
    void Trigger();
    /// reset time to 0
    void ResetTime();
    /// stop time
    void StopTime();
    /// start time
    void StartTime();
    /// get current time
    double GetTime();
    /// set current time
    void SetTime(double);
    /// lock frame time, every trigger will increment this value regardless of real time
    void LockDeltaT(double);
    /// set a wait period
    void WaitDeltaT(double);
    /// get locked frame time
    double GetLockDeltaT();
    /// get frame wait period
    double GetWaitDeltaT();
    /// enable frame based timing
    void EnableFrameTime();
    /// get current frame time
    double GetFrameTime();
    /// disable frame based timing
    void DisableFrameTime();

    /// Get counter used for timing 
    nTimeCounter GetTimeCounter() const;
    /// Convert counter to times
    double GetTimeFromTimeCounter(const nTimeCounter counter) const;
    /// Setup time counter as safe or non-safe
    void ConfigTimeSafety(bool safe);

    /// Get counter used for profiling
    nProfilerCounter GetProfilerCounter() const;
    /// Convert profiling counter to times
    double GetTimeFromProfilerCounter(const nProfilerCounter counter) const;
    /// Setup profiling counter as safe or non-safe
    void ConfigProfilerSafety(bool safe);

private:

    /// recalculate frequency of the internal timer
    void CalculateFrequency();
    /// apply the pending requests
    void ApplyRequests();

    static nTimeServer* Singleton;

    // true if time has been stopped
    bool stopped;
    // true if frame time provided at the beginning of the frame
    bool frame_enabled;
    // time at the beginning of current frame
    double frame_time;
    // duration of a frame (regardless of real time elapsed)
    double lock_delta_t;
    // value used to wait in order to reach the lcck FPS
    double wait_delta_t;
    double lock_time;

    // true if safe version used for timing
    bool safeTime;
    // true if safe version used for profiling
    bool safeProfiler;
    // request to change safe time counter
    bool requestSafeTime;
    // request to cahnge safe profiling counter
    bool requestSafeProfiler;

#   ifdef __WIN32__
    // initial time counter at start time
    //nTimeCounter time_diff;
    // time counter at stop time
    nTimeCounter time_stop;

    // performance counter value of current frame
    nuint64 currentFrameQPC;
    // time stamp counter value of current frame
    nuint64 currentFrameRDTSC;
    // performance counter value of last freq update
    nuint64 lastFreqUpdateQPC;
    // time stmap counter value of last freq update
    nuint64 lastFreqUpdateRDTSC;

    // curent frequency of QPC
    nuint64 QPCFrequency;
    // inverse of performance counter frequency
    double invQPCFrequency;
    // curent frequency of RDTSC
    nuint64 RDTSCFrequency;
    // inverse of timestamp counter of frequency
    double invRDTSCFrequency;
    // inverse of time counter frequency
    double invTimeFrequency;
    // inverse of profiler counter frequency
    double invProfilerFrequency;
#   else
    long long int time_diff;
    long long int time_stop; 
#   endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nTimeServer*
nTimeServer::Instance()
{
    n_assert(0 != Singleton);
    return Singleton;
}

#ifdef __WIN32__
//------------------------------------------------------------------------------
/**
    Read Time Stamp Counter. The CPU has an internal counter which counts all 
    the cycles passed. It is very very fast to read, and very precise. 
    The major problem of using this lies with system which
    vary the frequency in runtime like notebooks (SpeedStep, ...).

    There are two versions, the difference is one uses CPUID which avoid the 
    effect of the CPU pipeline (CPUID makes a pipeline flush).
    Only available for Pentium processors and up (X86 architecture),
    at the moment protected with __WIN32__ which is not correct.

    Problem of naked functions is that they cannot be inlined.
*/
inline __declspec(naked) nuint64 n_rdtsc(void)
{
#if 1
    __asm
    {
            // RDTSC: and get the current cycle into edx:eax
            __asm __emit 0x0F __asm __emit 0x31
            ret
    }
#else
    __asm
    {
            push ebx    // cpuid overwrites ebx
            push ecx    // and ecx

            // CPUID: flush the out-of-order instruction pipeline
            __asm __emit 0x0F __asm __emit 0xA2

            // RDTSC: and get the current cycle into edx:eax
            __asm __emit 0x0F __asm __emit 0x31

            pop ecx
            pop ebx

            ret
    }
#endif
}

//------------------------------------------------------------------------------
inline
nuint64 
GetPerformanceCounter()
{
    nuint64 time;
    QueryPerformanceCounter((LARGE_INTEGER *)&time);
    return time;
}

#endif

//------------------------------------------------------------------------------
/**
    Get counter used for timing. Nothing can be assumed about the units, to
    convert to time use GetTimeFromTimeCounter.
*/
inline
nTimeServer::nTimeCounter 
nTimeServer::GetTimeCounter() const
{
#ifdef WIN32
    //return (this->safeTime) ? GetPerformanceCounter() : n_rdtsc();
    if (this->safeTime)
    {
        return GetPerformanceCounter();
    }
    else
    {
        nuint64 ts;
        __asm rdtsc
        __asm mov DWORD PTR [ts], eax
        __asm mov DWORD PTR [ts+4], edx
        return ts;
    }
#else
#error not implemented GetTimeCounter
#endif
}

//------------------------------------------------------------------------------
/**
    Convert time counter to times
*/
inline
double 
nTimeServer::GetTimeFromTimeCounter(const nTimeServer::nTimeCounter counter) const
{
    return ((double)(counter)) * (this->invTimeFrequency);
}

//------------------------------------------------------------------------------
/**
    Get counter used for profiler. Nothing can be assumed about the units, to
    convert to time use GetTimeFromProfilerCounter.
*/
inline
nTimeServer::nProfilerCounter
nTimeServer::GetProfilerCounter() const
{
#ifdef WIN32
    //return (this->safeProfiler) ? GetPerformanceCounter() : n_rdtsc();
    if (this->safeProfiler)
    {
        return GetPerformanceCounter();
    }
    else
    {
        nuint64 ts;
        __asm rdtsc
        __asm mov DWORD PTR [ts], eax
        __asm mov DWORD PTR [ts+4], edx
        return ts;
    }
#else
#error not implemented GetProfilerCounter
#endif
}

//------------------------------------------------------------------------------
/**
    Convert time counter to times
*/
inline
double 
nTimeServer::GetTimeFromProfilerCounter(const nTimeServer::nProfilerCounter counter) const
{
    return ((double)(counter)) * (this->invProfilerFrequency);
}

//------------------------------------------------------------------------------
#endif
