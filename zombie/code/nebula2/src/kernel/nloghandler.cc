//------------------------------------------------------------------------------
//  nloghandler.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nloghandler.h"
#include <stdarg.h>

//------------------------------------------------------------------------------
/**
*/
nLogHandler::nLogHandler() :
    isOpen(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nLogHandler::~nLogHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nLogHandler::Open()
{
    n_assert(!this->isOpen);
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Close()
{
    n_assert(this->isOpen);
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Print(const char* /* str*/, va_list /* argList */)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Printf(const char*  str, ...)
{
    va_list argList;
    va_start(argList,str);
    this->Print(str, argList);
    va_end(argList);
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Message(const char* /* str */, va_list /* argList */)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
nLogHandler::Error(const char* /* str */, va_list /* argList */)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
int
nLogHandler::Assert(const char* /* str */, va_list /* argList */)
{
    return 0;
}

//------------------------------------------------------------------------------
/**
    - 26-Mar-05    kims    created
*/
void 
nLogHandler::OutputDebug(const char* /* str */, va_list /* argList */)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Subclasses may log all messages in an internal line buffer. If they
    chose to do so, this must be a nLineBuffer class and override the
    GetLineBuffer() method.
*/
nLineBuffer*
nLogHandler::GetLineBuffer()
{
    return 0;
}
