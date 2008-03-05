//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nsafelogbuffer.h"

//------------------------------------------------------------------------------
void 
nSafeLogBuffer::AddLine( const char* log )
{
    this->lock.Lock();
    nLogBuffer::AddLine(log);
    this->lock.Unlock();
}

//------------------------------------------------------------------------------
void 
nSafeLogBuffer::Clear()
{
    this->lock.Lock();
    nLogBuffer::Clear();
    this->lock.Unlock();
}

//------------------------------------------------------------------------------
void 
nSafeLogBuffer::Flush()
{
    this->lock.Lock();
    nStrNode * logEntry = this->logList.GetHead();
    while ( logEntry )
    {
        LogClassRegistry::Instance()->Log(0, logEntry->GetName());
        logEntry = logEntry->GetSucc();
    }
    nLogBuffer::Clear();
    this->lock.Unlock();
}
