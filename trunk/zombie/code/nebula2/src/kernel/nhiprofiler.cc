//------------------------------------------------------------------------------
//  nhiprofiler.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nhiprofiler.h"

#ifdef __NEBULA_STATS__

//------------------------------------------------------------------------------
nStack<nHiProfiler *> nHiProfiler::profilerStack;

int nHiProfiler::currentId = 0;

//------------------------------------------------------------------------------
/**
    Return true if the profiler was already started
*/
bool 
nHiProfiler::IsStarted() const
{
    return (this->startCount > 0);
}

//------------------------------------------------------------------------------
/**
    Publish accumulate time in environment variable
*/
void
nHiProfiler::Publish() const
{
    // go through all children and accumulate times
    // calculate total time, self time, time in children, total calls
    n_assert(!this->IsStarted());
    this->refEnv->SetF(this->GetAccumTime());
    this->refEnvNumCalls->SetI(this->GetNumCalls());
}

//------------------------------------------------------------------------------
/**
    get accumulated time (in milliseconds)
*/
float
nHiProfiler::GetAccumTime() const
{
    //nTime accumTime = nTimeServer::Instance()->GetTimeFromProfilerCounter(this->hrCounterAccum);
    //return static_cast<float>(accumTime) * 1000.0f;

    float totalTime = 0;
    for (int i = 0; i < this->contextProfilers.Size(); ++i)
    {
        totalTime += this->contextProfilers.GetElementAt(i)->GetAccumTime();
    }

    return totalTime;
}

//------------------------------------------------------------------------------
/**
    iterate all profilers and stop 
*/
void
nHiProfiler::StopAll()
{
    for(int i = 0;i < this->contextProfilers.Size();i++)
    {
        this->contextProfilers.GetElementAt(i)->Stop();
    }
}

//------------------------------------------------------------------------------
/**
    iterate all profilers and stop 
*/
void
nHiProfiler::StopAccumAll()
{
    for(int i = 0;i < this->contextProfilers.Size();i++)
    {
        this->contextProfilers.GetElementAt(i)->StopAccum();
    }
}

//------------------------------------------------------------------------------
/**
    push a profiler to the context stack
*/
void 
nHiProfiler::PushProfiler(nHiProfiler * prof)
{
    nHiProfiler::profilerStack.Push(prof);
}

//------------------------------------------------------------------------------
/**
    pop a profiler from the context stack
*/
nHiProfiler * 
nHiProfiler::PopProfiler()
{
    nHiProfiler::profilerStack.Pop();
    return nHiProfiler::GetTopProfiler();
}

//------------------------------------------------------------------------------
/**
    get current profiler in the context stack
*/
nHiProfiler * 
nHiProfiler::GetTopProfiler()
{
    if ( !nHiProfiler::profilerStack.IsEmpty() )
    {
        return nHiProfiler::profilerStack.Top();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    generate a new id
*/
int 
nHiProfiler::GenerateNewId()
{
    return ++nHiProfiler::currentId;
}

//------------------------------------------------------------------------------
/**
*/
void 
nHiProfiler::AddChildrenTime(nHiProfiler * child, const nTimeServer::nProfilerCounter & chTime)
{
    this->childrenTime += chTime;
    // add the child profiler to the list if not already into it
    if (this->children.FindIndex(child) == -1)
    {
        this->children.Append(child);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nHiProfiler::Destroy()
{
    for(int i = 0;i < this->contextProfilers.Size();i++)
    {
        n_delete( this->contextProfilers.GetElementAt(i) );
    }

    this->id = -1;
}

//------------------------------------------------------------------------------
/**
    Reset a hierarchical profiler (including all context based profilers)
*/
void
nHiProfiler::ResetAccum()
{
    nProfiler::ResetAccum();
    this->childrenTime = 0;
    for(int i(0);i < this->contextProfilers.Size();i++)
    {
        this->contextProfilers.GetElementAt(i)->ResetAccum();
    }
    this->children.Reset();
}

//------------------------------------------------------------------------------
/**
    Obtain the profiler relative to current profiler context.
*/
nProfiler * 
nHiProfiler::GetProfilerByContext()
{
    // find a profiler for current context
    nHiProfiler * context = this->GetTopProfiler();
    //int key = static_cast<int> ( reinterpret_cast<size_t>(context) );

    nProfiler * profilerInContext = 0;
    int contextId = context ? context->GetId() : 0;
    //if (this->contextProfiler.Find(key, profilerInContext))
    if (this->contextProfilers.Find(contextId, profilerInContext))
    {
        return profilerInContext;
    }

    // if not found, create a new one
    profilerInContext = n_new(nProfiler);
    
    //profilerInContext->SetId(nProfiler::GenerateNewId());
    this->contextProfilers.Add(contextId, profilerInContext);
    nString name;
    if (context)
    {
        name.Format("%s/%s", context->GetName(), this->GetName());
    }
    else
    {
        name.Format("%s", this->GetName());
    }
    profilerInContext->Initialize(name.Get(), true);
    return profilerInContext;
}

//------------------------------------------------------------------------------
/**
    Starts the profiler for current context.
*/
void 
nHiProfiler::Start() 
{
    n_assert(!this->IsStarted());
    if (this->IsStarted())
    {
        this->StopAll();
    }

    this->StartAccum();
}

//------------------------------------------------------------------------------
/**
    Start the profiler with accumulation
*/
void
nHiProfiler::StartAccum()
{
    n_assert(!this->IsStarted());
    if (this->IsStarted())
    {
        this->StopAccumAll();
    }

    nProfiler * prof = this->GetProfilerByContext();
    n_assert_return(prof, );

    prof->StartAccum();
    this->startCount++;
    this->numCalls++;

    this->PushProfiler(this);
}

//------------------------------------------------------------------------------
/**
*/
const nTimeServer::nProfilerCounter 
nHiProfiler::Stop() 
{
    nTimeServer::nProfilerCounter ret = 0;

    n_assert(this->IsStarted());
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
*/
const nTimeServer::nProfilerCounter 
nHiProfiler::StopAccum()
{
    nTimeServer::nProfilerCounter diff = 0;

    n_assert(this->IsStarted());
    if (this->IsStarted())
    {
        // first pop itself
        n_assert3(this->GetTopProfiler() == this, ("profilers intermixed start-stop not allowed, Stop called on %s when %s was on the stack", this->GetName(), this->GetTopProfiler()->GetName()));
        nHiProfiler * parentHiProf = this->PopProfiler();

        // stop accum profiler in current context
        nProfiler * prof = this->GetProfilerByContext();
        n_assert(prof);
        diff = prof->StopAccum();
        this->startCount--;
        
        // add the children time to current context
        if (parentHiProf)
        {
            parentHiProf->AddChildrenTime(this, diff);
        }
    }

    return diff;
}

#endif
