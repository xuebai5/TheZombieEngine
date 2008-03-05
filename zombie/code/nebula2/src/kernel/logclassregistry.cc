//------------------------------------------------------------------------------
//  logclassregistry.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/logclassregistry.h"
#include "kernel/nthread.h"
#include "kernel/nsafelogbuffer.h"

//------------------------------------------------------------------------------
LogClassRegistry* LogClassRegistry::server=0;

//------------------------------------------------------------------------------
/*
*/
LogClassRegistry*
LogClassRegistry::Instance()
{
    if(server == 0)
    {
        server = n_new(LogClassRegistry);
    }
    n_assert(server);
    return server;
}

//------------------------------------------------------------------------------
/*
*/
LogClassRegistry::LogClassRegistry() : 
    numRegistered(0) ,
    logClassHash(29),
    logOutputUser(0)
{
    this->logOutput = &logBuffer;
    this->safeLogBuffer = n_new(nSafeLogBuffer);
}

//------------------------------------------------------------------------------
/*
*/
LogClassRegistry::~LogClassRegistry()
{
    if (safeLogBuffer)
    {
        n_delete(safeLogBuffer);
    }

    server = 0;
}


//------------------------------------------------------------------------------
/*
*/
void 
LogClassRegistry::RegisterLogClass(nLogClass* logClass)
{
    n_assert(logClass)
    n_assert2( 0 == this->logClassHash.Find( logClass->GetName() ), "The same name for 2 intance o log class"); 
    this->logClassHash.Add(logClass);
    numRegistered++;
}

//------------------------------------------------------------------------------
/*
*/
nLogClass* 
LogClassRegistry::Find(const char* name)
{
    return (nLogClass*)(this->logClassHash.Find(name));
}

//------------------------------------------------------------------------------
/*
*/
void 
LogClassRegistry::UnRegisterLogClass(nLogClass* logClass)
{

    if (logClass)
    {
        n_assert( logClass == this->logClassHash.Find(logClass->GetName()) );
        logClass->Remove(); // remove of list;

        this->numRegistered--;
    }

    if ( 0 == numRegistered )
    {
        n_delete(this);
    }
}

//------------------------------------------------------------------------------
/*
*/
nArray<nLogClass*> 
LogClassRegistry::GetList()
{
    nArray<nLogClass*> tmp;
    int size = this->logClassHash.GetSize();
    for ( int i= 0 ; i< size ; i++)
    {
        nStrList* list = this->logClassHash.GetListIdx(i);
        nStrNode* node =list->GetHead();
        while ( node  )
        {
            tmp.Append( (nLogClass*) node);
            node = node->GetSucc();
        }
    }
    return tmp;
}

//------------------------------------------------------------------------------
/*
*/
nLogBuffer* LogClassRegistry::GetLogBuffer()
{
    return &this->logBuffer;
}

//------------------------------------------------------------------------------
/*
*/
void LogClassRegistry::SetLogOutputObject(nLogOutputObject* logOutput)
{
    this->logOutput = logOutput;
}

//------------------------------------------------------------------------------
/*
*/
nLogOutputObject* LogClassRegistry::GetLogOutputObject()
{
    return this->logOutput;
}

//------------------------------------------------------------------------------
/*
*/
void 
LogClassRegistry::SetUserLogOutputObject(nLogOutputObject* logOutput)
{
    this->logOutputUser = logOutput;
}

//------------------------------------------------------------------------------
/*
*/
nLogOutputObject* 
LogClassRegistry::GetUserLogOutputObject()
{
    return this->logOutputUser;
}

//------------------------------------------------------------------------------
/*
*/
void
LogClassRegistry::Log(int level, const char * msg)
{
    if  (nThread::IsTheMainThread(  nThread::GetCurrentThreadId()) )
    {
        // Print and store the log entry.
        n_printf("%s\n", msg);
        nLogOutputObject* logOutput = this->GetLogOutputObject();
        if ( logOutput )
        {
            logOutput->AddLine(msg);
        }

        nLogOutputObject* userlogOutput = this->GetUserLogOutputObject();
        if ( userlogOutput && (level & NLOGUSER) )
        {
            userlogOutput->AddLine(msg);
        }
    }
    else
    {
        if ( this->safeLogBuffer )
        {
            this->safeLogBuffer->AddLine(msg);
        }
    }
}

//------------------------------------------------------------------------------
/*
*/
void 
LogClassRegistry::Trigger()
{
    safeLogBuffer->Flush();
}
