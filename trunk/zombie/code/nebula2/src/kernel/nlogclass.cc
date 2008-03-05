#include "precompiled/pchnkernel.h"
#include "kernel/nlogclass.h"
#include "kernel/logclassregistry.h"
#include "kernel/nkernelserver.h"
#include "kernel/nloghandler.h"
#include "kernel/ntimeserver.h"
#include "kernel/nthread.h"

//------------------------------------------------------------------------------
const int MAX_LOG_REF_LENGTH = 1 * 1024;	// time + module name
const int MAX_LOG_MSG_LENGTH = 2 * 1024;	// message
const int MAX_LOG_TOTAL_LENGTH = 3 * 1024;	// whole log line

//------------------------------------------------------------------------------
/*
*/
nLogClass::nLogClass( const char* name, const char* description, bool enable, int level, const char ** groupNames, int groupsEnabled) : 
    nStrNode(name),
    maxLevelLog(level), 
    enable(enable),
    description(description),
    groups(groupsEnabled | NLOGUSER | NLOGDEF)
{
    LogClassRegistry::Instance()->RegisterLogClass(this);

    if (groupNames)
    {
        int i = 0;
        while(groupNames[i])
        {
            this->groupNames.Append(groupNames[i]);
            ++i;
        }
    }
}


//------------------------------------------------------------------------------
/*
*/
const char* 
nLogClass::GetDescription() const
{
    return this->description.Get();
}

//------------------------------------------------------------------------------
/*
*/
nLogClass::~nLogClass()
{
    LogClassRegistry::Instance()->UnRegisterLogClass(this);
}

//------------------------------------------------------------------------------
/*
*/
void 
nLogClass::Log( int level, const char *msg, ...)
{
    if (!(level & NLOG_GROUP_MASK))
    {
        level |= NLOGDEF;
    }

    if ( this->enable && 
         int(level & NLOG_LEVEL_MASK) <= this->maxLevelLog && 
         (level & NLOG_GROUP_MASK) & this->groups ) 
    {
        // Build up the log entry.
        va_list argList;
        va_start(argList,msg);
        char charBuffer1[MAX_LOG_REF_LENGTH];
        char charBuffer2[MAX_LOG_MSG_LENGTH];
        char charBuffer3[MAX_LOG_TOTAL_LENGTH];

         // Only enable groups
        const char * groupName = this->GetGroupNameByLevelMask(level & this->groups);
        if (groupName)
        {
#ifndef __ZOMBIE_EXPORTER__
            snprintf(charBuffer1, sizeof(charBuffer1)-1, "%s:%s:%i: ", this->GetName(), groupName, level & NLOG_LEVEL_MASK);
#else
            snprintf(charBuffer1, sizeof(charBuffer1)-1, "%i %s:%s: ", level & NLOG_LEVEL_MASK, this->GetName(), groupName);
#endif
        }
        else
        {
#ifndef __ZOMBIE_EXPORTER__
            snprintf(charBuffer1, sizeof(charBuffer1)-1, "%s:%i: ", this->GetName(), level & NLOG_LEVEL_MASK, groupName);
#else
            snprintf(charBuffer1, sizeof(charBuffer1)-1, "%i %s: ", level & NLOG_LEVEL_MASK, this->GetName());
#endif
        }
        vsnprintf(charBuffer2, sizeof(charBuffer2)-1, msg, argList);
        snprintf(charBuffer3, sizeof(charBuffer3)-1, "%s%s", charBuffer1, charBuffer2);
        va_end(argList);

        if (LogClassRegistry::Instance())
        {
            LogClassRegistry::Instance()->Log(level, charBuffer3);
        }
    }
}

//------------------------------------------------------------------------------
/*
*/
const char * 
nLogClass::GetGroupNameByLevelMask(int level) const
{
    level &= NLOG_GROUP_MASK;
    if (  0 == level )
    {
        return 0;
    }
    else
    {
        // search the first bit is enable by left. Index contains the bit positon 0 1 2 3 ...
        int index = 0;
        while ( index < 22 && ( 0 == (level & 0x80000000 ) ) )
        {
            level = level << 1;
            index++;
        }
        return this->GetGroupName( index);
    }   
}
