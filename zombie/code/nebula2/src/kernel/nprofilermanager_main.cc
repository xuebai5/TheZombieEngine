//------------------------------------------------------------------------------
//  nprofilermanager_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nprofilermanager.h"
#include "kernel/nprofiler.h"
#include "kernel/nhiprofiler.h"

//------------------------------------------------------------------------------
nNebulaClass(nProfilerManager, "nroot");

//------------------------------------------------------------------------------
nProfilerManager* nProfilerManager::instance = 0;

//------------------------------------------------------------------------------
/**
    default constructor
*/
nProfilerManager::nProfilerManager() :
    rootContext(0),
    enabled(true)
{
    if (!nProfilerManager::instance)
    {
        nProfilerManager::instance = this;
        #ifdef __NEBULA_STATS__
        this->rootContext = n_new( nHiProfiler )("root");
        #endif //__NEBULA_STATS__
    }
}


//------------------------------------------------------------------------------
/**
    destructor
*/
nProfilerManager::~nProfilerManager()
{
    this->UnregisterAllProfilers();

    if (nProfilerManager::instance == this)
    {
        nProfilerManager::instance = 0;
    }

    #ifdef __NEBULA_STATS__
    if (this->rootContext)
    {
        n_delete( this->rootContext );
        this->rootContext = 0;
    }
    #endif //__NEBULA_STATS__
}

//------------------------------------------------------------------------------
/**
    Return singleton instance
*/
nProfilerManager * 
nProfilerManager::Instance()
{
    return nProfilerManager::instance;
}

//------------------------------------------------------------------------------
/**
    Register an autoreset profiler
*/
void 
nProfilerManager::RegisterProfiler(nProfiler * profiler)
{
    this->regProfilers.Append(profiler);
}

//------------------------------------------------------------------------------
/**
    Unregister an autoreset profiler
*/
void 
nProfilerManager::UnregisterProfiler(nProfiler * profiler)
{
    int i = 0;
    while(i < this->regProfilers.Size())
    {
        if (this->regProfilers[i] == profiler)
        {
            this->regProfilers.EraseQuick(i);
        }
        else
        {
            ++i;
        }
    }
}

#ifdef __NEBULA_STATS__
//------------------------------------------------------------------------------
/**
    begin profiling
*/
void 
nProfilerManager::BeginProfiling()
{
    if ( this->IsEnabled() )
    {
        n_assert(this->rootContext);
        this->rootContext->StartAccum();
    }
}

//------------------------------------------------------------------------------
/**
    end profiling
*/
void 
nProfilerManager::EndProfiling()
{
    if ( this->IsEnabled() )
    {
        if (this->rootContext->IsStarted())
        {
            this->rootContext->StopAccum();
        }

        this->ResetAndPublishProfilers();
    }
}
#endif __NEBULA_STATS__

//------------------------------------------------------------------------------
/**
    Reset and publish all registered profilers
*/
void 
nProfilerManager::ResetAndPublishProfilers() const
{
    for(int i = 0;i < this->regProfilers.Size();i++)
    {
        bool isStarted = this->regProfilers[i]->IsStarted();
        if (isStarted)
        {
            this->regProfilers[i]->StopAccum();
        }
        this->regProfilers[i]->Publish();
        this->regProfilers[i]->ResetAccum();
        if (isStarted)
        {
            this->regProfilers[i]->StartAccum();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Unregister all profilers
*/
void 
nProfilerManager::UnregisterAllProfilers()
{
    while(this->regProfilers.Size() > 0)
    {
        this->regProfilers.EraseQuick(0);
    }
}
