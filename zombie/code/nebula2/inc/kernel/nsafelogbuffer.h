#ifndef N_NSAFELOGBUFFER_H
#define N_NSAFELOGBUFFER_H
//------------------------------------------------------------------------------
/**
    @class nSafeLogBuffer
    @author Mateu Batle Sastre
    @ingroup Kernel

    Thread-safe log buffer.

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nlogbuffer.h"
#include "kernel/logclassregistry.h"

//------------------------------------------------------------------------------
class nSafeLogBuffer : public nLogBuffer
{
public:

    /// add log line
    virtual void AddLine( const char* log );
    /// clear all the buffered output 
    virtual void Clear();

    /// print all line content into another log output object
    void Flush();

protected:
    
    /// mutex to ensure mutual exclusion
    nMutex lock;

};

//------------------------------------------------------------------------------
#endif
