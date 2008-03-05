#ifndef N_LOGBUFFER_H
#define N_LOGBUFFER_H
//------------------------------------------------------------------------------
/**
    @class nLogBuffer
    @ingroup Kernel

    Buffer for storing log entries.

    The buffer stores a maximum of log entries. When the limit is exceeded the
    oldest entry is automatically removed.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/nstrlist.h"
#include "kernel/nlogoutputobject.h"

//------------------------------------------------------------------------------
class nLogBuffer : public nLogOutputObject
{
public:
    /// constuctor
    nLogBuffer( int maxSize = 1000 );
    /// destrcutor
    ~nLogBuffer();

    /// add a line to the bufefr
    virtual void AddLine( const char* log );
    /// get all the lines
    nStrList & GetLineList();

    /// clear all the buffered output 
    virtual void Clear();
    /// get the size in number of entries
    int GetSize() const;

protected :
    nStrList logList;
    int maxSize;
    int size;
};

//------------------------------------------------------------------------------
/*
*/
inline
nStrList&
nLogBuffer::GetLineList()
{
    return logList;
}

//------------------------------------------------------------------------------
/*
*/
inline
int
nLogBuffer::GetSize() const
{
    return size;
}

#endif
