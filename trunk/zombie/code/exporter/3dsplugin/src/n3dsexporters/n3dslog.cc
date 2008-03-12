#include "precompiled/pchn3dsmaxexport.h"
#include "n3dsexporters/n3dslog.h"
#include "kernel/nkernelserver.h"
#include "kernel/nloghandler.h"

unsigned int  n3dsLog::warningCount;
unsigned int  n3dsLog::errorCount;
nString n3dsLog::fileLog;

//------------------------------------------------------------------------------
/**
    @brief initialize the log params betwen export,
*/
void 
n3dsLog::Init()
{
    errorCount = 0;
    warningCount = 0;
}

void 
n3dsLog::AddWarning()
{
    if (warningCount < UINT_MAX )
    {
        warningCount++;
    }
}

bool 
n3dsLog::GetShowLog()
{
    return (warningCount > 0) || (errorCount > 0);
}

//------------------------------------------------------------------------------
/**
*/
void 
n3dsLog::AddError()
{
    if (errorCount < UINT_MAX )
    {
        errorCount++;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
n3dsLog::GetHasError()
{
    return errorCount > 0 ;
}

//------------------------------------------------------------------------------
/**
*/
void
n3dsLog::PutErrorNumbers()
{
    NLOG( exporter , ( 0 , " %u errors, %u warnings.\n" , errorCount ,  warningCount) );
}

void 
n3dsLog::SetFileLog(const char* name)
{
    fileLog = name;
}

void
n3dsLog::ShowLog()
{
    if ( nKernelServer::ks && 
         nKernelServer::ks->GetLogHandler() && 
         nKernelServer::ks->GetLogHandler()->IsOpen() 
       )
    {
        PutErrorNumbers();
        nKernelServer::ks->GetLogHandler()->Close();
    }
    if ( GetShowLog() )
    {
        ShellExecute(NULL,                   // hWnd
            "open",
            "wordpad.exe",    // lpFile
            fileLog.Get(),  // lpParameters
            "", // lpDirectory
            1);
    }
}

