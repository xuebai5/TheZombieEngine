#ifndef N_LOGCLASSREGISTRY_H
#define N_LOGCLASSREGISTRY_H
//------------------------------------------------------------------------------
/**
    @class nLogClassRegistry
    @ingroup Kernel

    it's a wraper of LogClassRegistry. for scripting commands

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "kernel/logclassregistry.h"
#include "kernel/nlogclass.h"
#include "kernel/nroot.h"

//------------------------------------------------------------------------------
class nLogClassRegistry : public nRoot
{
public:
    ~nLogClassRegistry();

    void RegisterLogClass(const char*name, const char* description);
    void Log(const char*name, int level, const char* msg);
    const char* GetDescription(const char*name) const;

    int GetWarningLevel(const char* name) const;
    void SetWarningLevel(const char* name, int level);

    void SetEnable(const char* name, bool enable);
    bool GetEnable(const char* name) const;

    void EnableLogGroup(const char* name, int mask);
    void DisableLogGroup(const char * name, int mask);
    void EnableLogGroupByIndex(const char* name, int index);
    void DisableLogGroupByIndex(const char * name, int index);

    int GetEnabledLogGroups(const char * name) const;

    const char * GetGroupName(const char * name, int index) const;
    int GetNumGroups(const char * name) const;

protected:
    nArray<nLogClass*> dynamicLog;
};

//------------------------------------------------------------------------------
#endif
