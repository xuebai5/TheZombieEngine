//------------------------------------------------------------------------------
//  ntime_main.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/ntimeserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
#if defined(__LINUX__) || defined(__MACOSX__)
#define N_MICROSEC_INT    (1000000)
#define N_MICROSEC_FLOAT  (1000000.0)
#define tv2micro(x) (x.tv_sec * N_MICROSEC_INT + x.tv_usec);
#endif

//------------------------------------------------------------------------------
nNebulaScriptClass(nTimeServer, "nroot");
nTimeServer* nTimeServer::Singleton = 0;

//------------------------------------------------------------------------------
NCREATELOGLEVEL(time, "Time", true, 0);

//------------------------------------------------------------------------------
/**
*/
nTimeServer::nTimeServer() :
    stopped(false),
    frame_enabled(false),
    frame_time(0.0),
    lock_delta_t(0.0),
    wait_delta_t(0.0),
    time_stop(0),
    safeTime(true),
    safeProfiler(false),
    requestSafeTime(true),
    requestSafeProfiler(false)
{
    n_assert(0 == Singleton);
    Singleton = this;

#ifdef __WIN32__
    this->currentFrameQPC = 0;
    this->currentFrameRDTSC = 0;
    this->lastFreqUpdateQPC = 0;
    this->lastFreqUpdateRDTSC = 0;

    this->invQPCFrequency = 0;
    this->invRDTSCFrequency = 0;
    this->invTimeFrequency = 0;
    this->invProfilerFrequency = 0;

    this->ResetTime();
    this->CalculateFrequency();
    
#elif defined(__LINUX__) || defined(__MACOSX__)
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time_diff = tv2micro(tv);
#else
#error "Method not implemented!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
nTimeServer::~nTimeServer()
{
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
nTimeServer::ResetTime()
{
    this->lock_time = 0;

#ifdef __WIN32__
    //this->time_diff = this->GetTimeCounter();
    this->frame_time = 0;
#elif defined(__LINUX__) || defined(__MACOSX__)
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time_diff = tv2micro(tv);
#else
#error "Method not implemented!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
nTimeServer::SetTime(double t)
{
    this->lock_time = t;
    this->frame_time = t;

#ifdef __WIN32__
    // t nach Ticks umrechnen
    //nuint64 td = (nuint64) (t / (this->invTimeFrequency));
    //this->time_diff = this->GetTimeCounter();
    //this->time_stop = this->time_diff;
    //this->time_diff -= td;
    this->time_stop = this->GetTimeCounter();
#elif defined(__LINUX__) || defined(__MACOSX__)
    // t nach Microsecs umrechnen
    long long td = (long long int) (t * N_MICROSEC_FLOAT);
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time_diff = tv2micro(tv);
    this->time_stop = this->time_diff;
    this->time_diff -= td;
#else
#error "Method not implemented!"
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
nTimeServer::StopTime()
{
    if (this->stopped) 
    {
        n_printf("Time already stopped!\n");
    } 
    else 
    {
        this->stopped = true;

#ifdef __WIN32__
        this->time_stop = this->GetTimeCounter();
#elif defined(__LINUX__) || defined(__MACOSX__)
        struct timeval tv;
        gettimeofday(&tv,NULL);
        this->time_stop = tv2micro(tv);
#else
#error "Method not implemented!"
#endif
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nTimeServer::StartTime()
{
    if (!this->stopped) 
    {
        n_printf("Time already started!\n");
    } 
    else 
    {
        this->stopped = false;

#   ifdef __WIN32__
        //nuint64 time, td;
        //time = this->GetTimeCounter();
        //td = time - this->time_stop;
        //this->time_diff += td;
#   elif defined(__LINUX__) || (__MACOSX__)
        long long int time;
        long long int td;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        time = tv2micro(tv);
        td = time - this->time_stop;
        this->time_diff += td;
#   else
#error "Method not implemented!"
#   endif
    }
}

//------------------------------------------------------------------------------
/**
*/
double 
nTimeServer::GetTime()
{
    if (this->lock_delta_t > 0.0) 
    {
        return this->lock_time;
    }
    else 
    {
#       ifdef __WIN32__
        nuint64 time;
        if (this->stopped) 
        {
            time = this->time_stop;
        }
        else
        {
            time = this->GetTimeCounter();
        }

        // calculate the time from beginning of the frame
        time -= this->safeTime ? this->currentFrameQPC : this->currentFrameRDTSC;

        // add to the frame time
        nTime rettime = this->frame_time;
        rettime += max(0, this->GetTimeFromTimeCounter(time));
        return rettime;
        //nuint64 time;
        //if (this->stopped) 
        //{
        //    time = this->time_stop;
        //}
        //else
        //{
        //    time = this->GetTimeCounter();
        //}
        //n_assert(time >= this->time_diff);
        //nuint64 td = time - this->time_diff;
        //nTime rettime = this->GetTimeFromTimeCounter(td);
        //return rettime;
    }
#   elif defined(__LINUX__) || defined(__MACOSX__)
        long long int time;
        long long int td;
        double d_time;
        if (this->stopped) 
        {
            time = this->time_stop;
        }
        else 
        {
            struct timeval tv;
            gettimeofday(&tv,NULL);
            time = tv2micro(tv);
        }
        td = time - this->time_diff;
        d_time = ((double)td) / N_MICROSEC_FLOAT;
        return d_time;
    }
#   else
#error "Method not implemented!"
#   endif
}

//------------------------------------------------------------------------------
/**
*/
void 
nTimeServer::LockDeltaT(double dt)
{
    n_assert(dt >= 0.0);
    if (dt == 0.0)
    {
        this->SetTime(this->lock_time);
    }
    else
    {
        this->lock_time = this->GetTime();
    }
    this->lock_delta_t = dt;
}

//------------------------------------------------------------------------------
/**
*/
void 
nTimeServer::WaitDeltaT(double dt)
{
    n_assert(dt >= 0.0);
    this->wait_delta_t = dt;
}

//------------------------------------------------------------------------------
/**
*/
double 
nTimeServer::GetLockDeltaT()
{
    return this->lock_delta_t;
}

//------------------------------------------------------------------------------
/**
*/
double 
nTimeServer::GetWaitDeltaT()
{
    return this->wait_delta_t;
}

//------------------------------------------------------------------------------
/**
*/
void 
nTimeServer::Trigger()
{
    nTime frameTime = this->GetTime(); 
    this->ApplyRequests();

    this->CalculateFrequency();

    if (this->lock_delta_t > 0.0) 
    {
        if (!this->stopped) 
        {
            this->lock_time += this->lock_delta_t;
        }
    }
    if (this->wait_delta_t > 0.0) 
    {
        n_sleep(this->wait_delta_t);
    }

    NLOGCOND(time, frameTime < this->frame_time, (0, "oldTime=%f newTime=%f", this->frame_time, frameTime));
    if (!this->stopped)
    {
        this->frame_time = frameTime;
    }
    NLOG(time, (1, "New time = %f", this->frame_time));
}

//------------------------------------------------------------------------------
/**
*/
void nTimeServer::EnableFrameTime(void)
{
    this->frame_enabled = true;
    this->frame_time = this->GetTime();
}

//------------------------------------------------------------------------------
/**
*/
void nTimeServer::DisableFrameTime(void)
{
    this->frame_enabled = false;
}

//------------------------------------------------------------------------------
/**
*/
double nTimeServer::GetFrameTime(void)
{
    if (this->frame_enabled)
    {
        return this->frame_time;
    }
    else
    {
        return this->GetTime();
    }
}

//------------------------------------------------------------------------------
/**
    Calculate RDTSC and QPC frequency. 
*/
void
nTimeServer::CalculateFrequency()
{
#ifdef __WIN32__
    // get current value of counters
    nuint64 currentRDTSC = n_rdtsc();
    nuint64 currentQPC = GetPerformanceCounter();

    // calculate RDTSC frequency every 2 seconds or when prevRDTSC is NULL
    if (((currentQPC - this->lastFreqUpdateQPC) * this->invQPCFrequency) > 2.0 || !this->lastFreqUpdateQPC)
    {
        NLOG(time, (1, "currentRDTSC=%I64d lastFreqUpdateRDTSC=%I64d diff=%I64d time@2.8GHz=%f time?=%f", 
            currentRDTSC, 
            this->lastFreqUpdateRDTSC, 
            (currentRDTSC - this->lastFreqUpdateRDTSC),
            (currentRDTSC - this->lastFreqUpdateRDTSC) / (2.8e9),
            (currentRDTSC - this->lastFreqUpdateRDTSC) * this->invRDTSCFrequency) );

        NLOG(time, (1, "currentQPC=%I64d lastFreqUpdateQPC=%I64d diff=%I64d time=%f",
            currentQPC,
            this->lastFreqUpdateQPC,
            (currentQPC - this->lastFreqUpdateQPC),
            (currentRDTSC - this->lastFreqUpdateRDTSC) * this->invQPCFrequency) );

        NLOG(time, (1, "QPCFrequency=%I64d", this->QPCFrequency));

        // QPC frequency can change during the execution
        n_verify2( QueryPerformanceFrequency((LARGE_INTEGER *) &this->QPCFrequency) == TRUE, "QPC not supported, time system invalid");
        this->invQPCFrequency = this->QPCFrequency ? 1.0 / this->QPCFrequency : 0.0f;

        // calculates the frequency based on QPC as trusty reference
        nuint64 RDTSCFrequency = (nuint64) ( double(currentRDTSC - this->lastFreqUpdateRDTSC) / 
            double(currentQPC - this->lastFreqUpdateQPC) * double(this->QPCFrequency) );
        
        // check for frequency changes more than 1%
        N_IFDEF_NLOG(double diff = RDTSCFrequency * 100.0 / this->RDTSCFrequency);
        NLOGCOND(time, diff < 99.0f || diff > 101.0f, (0, "rdtscFrequencyOld=%I64d rdtscFrequencyNew=%I64d variation=%f", 
            RDTSCFrequency, this->RDTSCFrequency, diff));

        // store new frequency
        this->RDTSCFrequency = RDTSCFrequency;
        this->invRDTSCFrequency = this->RDTSCFrequency ? 1.0 / this->RDTSCFrequency : 0.0;

        this->lastFreqUpdateQPC = currentQPC;
        this->lastFreqUpdateRDTSC = currentRDTSC;
    }

    // store current frame counters
    this->currentFrameRDTSC = currentRDTSC;
    this->currentFrameQPC = currentQPC;

    // update frequencies of timer and profiler counters depending on current safety modes
    this->invTimeFrequency = this->safeTime ? this->invQPCFrequency : this->invRDTSCFrequency; 
    this->invProfilerFrequency = this->safeProfiler ? this->invQPCFrequency : this->invRDTSCFrequency; 
#endif
}

//------------------------------------------------------------------------------
/**
    Request change in time counter safety. Change will not take effect until
    beginning of next frame.
*/
void 
nTimeServer::ConfigTimeSafety(bool safe)
{
    this->requestSafeTime = safe;
}

//------------------------------------------------------------------------------
/**
    Request change in profiler counter safety. Change will not take effect until
    beginning of next frame.
*/
void 
nTimeServer::ConfigProfilerSafety(bool safe)
{
    this->requestSafeProfiler = safe;
}

//------------------------------------------------------------------------------
/**
    Apply changes requested
*/
void
nTimeServer::ApplyRequests()
{
    bool changedTime = (this->safeTime != this->requestSafeTime);
    this->safeTime = this->requestSafeTime;
    this->safeProfiler = this->requestSafeProfiler;

    // update frequencies of timer and profiler counters depending on current safety modes
    this->invTimeFrequency = this->safeTime ? this->invQPCFrequency : this->invRDTSCFrequency; 
    this->invProfilerFrequency = this->safeProfiler ? this->invQPCFrequency : this->invRDTSCFrequency; 

    if (changedTime)
    {
        this->SetTime(this->GetFrameTime());
    }
}

//------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------
            
