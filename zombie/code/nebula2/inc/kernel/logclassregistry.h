#ifndef LOGCLASSREGISTRY_H
#define LOGCLASSREGISTRY_H
//------------------------------------------------------------------------------
/**
    @class nLogClassRegistry
    @ingroup Kernel

    Registry of all the log classes.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/nlogclass.h"
#include "util/nhashtable.h"
#include "util/narray.h"
#include "kernel/nlogbuffer.h"

//------------------------------------------------------------------------------
class nSafeLogBuffer;

//------------------------------------------------------------------------------
class LogClassRegistry
{
public:
    /// Return a singleton
    static LogClassRegistry*  Instance();
    /// Destructor
    ~LogClassRegistry();

    /// register a log class
    void RegisterLogClass(nLogClass* logClass);
    /// unregister a log class
    void UnRegisterLogClass(nLogClass* logClass);
    /// find a log class by name
    nLogClass* Find(const char* name);
    /// return array of registered class
    nArray<nLogClass*> GetList();

    /// return the buffer with the last log entries
    nLogBuffer* GetLogBuffer();

    /// set the object where to redirect log entries (by default the internal LogBuffer)
    void SetLogOutputObject(nLogOutputObject* logOutput);
    /// return the current object used to redirect log entries to
    nLogOutputObject* GetLogOutputObject();

    /// set the object where to redirect user log entries (by default the internal LogBuffer)
    void SetUserLogOutputObject(nLogOutputObject* logOutput);
    /// return the current object used to redirect user log entries to
    nLogOutputObject* GetUserLogOutputObject();

    /// to be used to print the output of the safe log buffer in main thread
    void Trigger();

protected:

    friend class nLogClass;
    friend class nSafeLogBuffer;

    /// not used by the user, called from log classes
    virtual void Log(int level, const char *);

    ///protected constructor
    LogClassRegistry();
    ///pointer to singleton instance
    static LogClassRegistry* server;
    ///hastable of logclass
    nHashTable logClassHash;
    /// number of registering 
    int numRegistered;
    /// buffer to store log entries
    nLogBuffer logBuffer;
    /// buffer to store log entries of non-main threads
    nSafeLogBuffer * safeLogBuffer;
    /// object where log entries are redirected to
    nLogOutputObject* logOutput;
    /// object where log entries are redirected to
    nLogOutputObject* logOutputUser;
};

#endif // LOGCLASSREGISTRY_H
