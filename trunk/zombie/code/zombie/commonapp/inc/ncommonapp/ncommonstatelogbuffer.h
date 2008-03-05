#ifndef N_TRAGSTATELOGBUFFER_H
#define N_TRAGSTATELOGBUFFER_H
//------------------------------------------------------------------------------
/**
    @class nCommonStateLogBuffer
    @ingroup nApplication

    Buffer for displaying on screen log entries.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "util/narray.h"
#include "util/nstring.h"
#include "util/nstrlist.h"
#include "kernel/nlogoutputobject.h"

//------------------------------------------------------------------------------
class nCommonStateLogBuffer : public nLogOutputObject 
{
public:
    nCommonStateLogBuffer();
    ~nCommonStateLogBuffer();
    void AddLine( const char* log );

private:
    // temporal buffer for strings that could not be logged yet
    nArray<nString> strings;
};

//------------------------------------------------------------------------------
#endif
