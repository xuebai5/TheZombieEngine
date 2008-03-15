#ifndef N_LOGCLASS_H
#define N_LOGCLASS_H
//------------------------------------------------------------------------------
/**
    @class nLogClass
    @ingroup Kernel

    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

//#include <stdarg.h>
//------------------------------------------------------------------------------
/**
    This defines are better placed here (before the include) to allow logging
    in headers.
*/
#define _NLOG(name, y) \
    { \
        extern nLogClass classlog_##name; \
        if (classlog_##name.GetEnable()) \
        { \
            classlog_##name.Log y ; \
        } \
    }

#define _NLOGCOND(name, expr, y) \
    { \
        extern nLogClass classlog_##name; \
        if (classlog_##name.GetEnable() && expr) \
        { \
            classlog_##name.Log y ; \
        } \
    }

#define _NCREATELOG(name, description)  \
    nLogClass classlog_##name (#name,  description, false, 1, 0);

#define _NCREATELOGLEVEL(name, description, enable, level)  \
    nLogClass classlog_##name (#name,  description, enable, level, 0);

#define _NCREATELOGLEVELGROUP(name, description, enable, level, groups, groupMask)  \
    nLogClass classlog_##name (#name,  description, enable, level, groups, groupMask);

//------------------------------------------------------------------------------
/**
    Macros enabled always, meant for logging useful for user.
*/
#define NLOG_REL(name, y)                      _NLOG(name, y)
#define NLOGCOND_REL(name, expr, y)            _NLOGCOND(name, expr, y)
#define NCREATELOG_REL(name, description)      _NCREATELOG(name, description)
#define NCREATELOGLEVEL_REL(name, description, enable, level) _NCREATELOGLEVEL(name, description, enable, level)
#define NCREATELOGLEVELGROUP_REL(name, description, enable, level, groups, groupMask)  _NCREATELOGLEVELGROUP(name, description, enable, level, groups, groupMask)

//------------------------------------------------------------------------------
/**
    Macros enabled only in __NEBULA_NO_LOG__, meant for for the programmers.
*/
#ifdef __NEBULA_NO_LOG__

#define NLOG( name, y)
#define NLOGCOND( name, expr, y)
#define NCREATELOG( name, description )
#define NCREATELOGLEVEL( name, description, enable, level)
#define NCREATELOGLEVELGROUP(name, description, enable, level, groups, groupMask)
#define N_IFDEF_NLOG(exp)

#else

#define NLOG(name, y)                       _NLOG(name,y)
#define NLOGCOND(name, expr, y)             _NLOGCOND(name,expr,y)
#define NCREATELOG(name, description)       _NCREATELOG(name, description)
#define NCREATELOGLEVEL(name, description, enable, level)  _NCREATELOGLEVEL(name, description, enable, level)
#define NCREATELOGLEVELGROUP(name, description, enable, level, groups, groupMask)  _NCREATELOGLEVELGROUP(name, description, enable, level, groups, groupMask)
#define N_IFDEF_NLOG(exp) exp

#endif //__NEBULA_NO_LOG__

//------------------------------------------------------------------------------
/**
    Log groups.
*/
#define NLOG_GROUP_MASK  0x7FFFFF00
#define NLOG_LEVEL_MASK  (~NLOG_GROUP_MASK)

#define NLOG1    (1<<30)
#define NLOG2    (1<<29)
#define NLOG3    (1<<28)
#define NLOG4    (1<<27)
#define NLOG5    (1<<26)
#define NLOG6    (1<<25)
#define NLOG7    (1<<24)
#define NLOG8    (1<<23)
#define NLOG9    (1<<22)
#define NLOG10   (1<<21)
#define NLOG11   (1<<20)
#define NLOG12   (1<<19)
#define NLOG13   (1<<18)
#define NLOG14   (1<<17)
#define NLOG15   (1<<16)
#define NLOG16   (1<<15)
#define NLOG17   (1<<14)
#define NLOG18   (1<<13)
#define NLOG19   (1<<12)
#define NLOG20   (1<<11)
#define NLOG21   (1<<10)
#define NLOGDEF  (1<< 9)
#define NLOGUSER (1<< 8)
#define NLOGNUMGROUPS 23

//------------------------------------------------------------------------------
#include "util/nstrnode.h"
//#include "kernel/ntimeserver.h"

//------------------------------------------------------------------------------
class nLogClass : public nStrNode
{
public:
    ///constructor
    nLogClass( const char* name, const char* description, bool enable, int level = 1, const char ** groupNames = 0, int groupsEnabled = NLOG_GROUP_MASK);
    /// destructor
    ~nLogClass();

    /// print a message to the log dump
    void Log( int level, const char *msg, ...);
    /// Get a description of this log
    const char* GetDescription() const;

    /// set if this log is enable
    void SetEnable(bool enable);
    //// get if this log is enable
    bool GetEnable() const;

    /// set maximun level of log
    void SetLevelLog(int maxLevelLog);
    /// get the current maximun level log
    int GetLevelLog() const;

	/// return the mask for the index provided
	int GetMaskForIndex(int index) const;
    /// enable log groups
    void EnableLogGroup(int mask);
    /// disable log group
    void DisableLogGroup(int mask);
    /// enable log groups by index
    void EnableLogGroupByIndex(int index);
    /// disable log group by index
    void DisableLogGroupByIndex(int index);
    /// get enabled log groups mask
    int GetEnabledLogGroups() const;
    /// get group name
    const char * GetGroupName(int index) const;
    /// get number of groups
    int GetNumGroups() const;

protected:
    /// get group name
    const char * GetGroupNameByLevelMask(int level) const;
    ///description name
    nString description;
    /// maximum level log
    int maxLevelLog;
    /// enable this log
    bool enable;
    /// log groups enabled (bitmask)
    int groups;
    /// group names
    nArray<nString> groupNames;
};

//------------------------------------------------------------------------------
/*
*/
inline
void 
nLogClass::SetEnable( bool enable )
{
    this->enable = enable;
}

//------------------------------------------------------------------------------
/*
*/
inline
bool 
nLogClass::GetEnable() const
{
    return this->enable;
}

//------------------------------------------------------------------------------
/*
*/
inline
void 
nLogClass::SetLevelLog( int maxLevelLog )
{
     this->maxLevelLog = maxLevelLog;
}

//------------------------------------------------------------------------------
/*
*/
inline
int  
nLogClass::GetLevelLog() const
{
    return this->maxLevelLog;
}

//------------------------------------------------------------------------------
/*
    enable log groups
*/
inline
void 
nLogClass::EnableLogGroup(int mask)
{
    this->groups = this->groups | mask;
}

//------------------------------------------------------------------------------
/*
    disable log group    
*/
inline
void 
nLogClass::DisableLogGroup(int mask)
{
    this->groups = this->groups & ~mask;
}

//------------------------------------------------------------------------------
/*
    enable log groups by index (starting with one)
*/
inline
void 
nLogClass::EnableLogGroupByIndex(int index)
{
	this->groups = this->groups | this->GetMaskForIndex(index);
}

//------------------------------------------------------------------------------
/*
    disable log group by index (starting with one)
*/
inline
void 
nLogClass::DisableLogGroupByIndex(int index)
{
    this->groups = this->groups & ~this->GetMaskForIndex(index);
}

//------------------------------------------------------------------------------
/*
    Get the bitmask for a given group index
*/
inline
int
nLogClass::GetMaskForIndex(int index) const
{
	if (index >= 1 && index <= NLOGNUMGROUPS)
	{
		return (NLOG1 >> (index - 1));
	}
	return 0;
}

//------------------------------------------------------------------------------
/*
    get log group enabled mask
*/
inline
int
nLogClass::GetEnabledLogGroups() const
{
    return this->groups;
}

//------------------------------------------------------------------------------
/**
	returns the group name for the group specified (starting with one)
*/
inline
const char * 
nLogClass::GetGroupName(int index) const
{
    if (index >= 1 && index <= this->groupNames.Size())
    {
        return this->groupNames[index - 1].Get();
    }

    return 0;
}

//------------------------------------------------------------------------------
inline
int 
nLogClass::GetNumGroups() const
{
    return this->groupNames.Size();
}

//------------------------------------------------------------------------------
#endif // N_LOGCLASS_H
