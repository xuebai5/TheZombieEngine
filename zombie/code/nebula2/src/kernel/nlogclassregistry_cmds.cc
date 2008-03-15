//------------------------------------------------------------------------------
//  nlogclassregistry_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nlogclassregistry.h"
#include "kernel/npersistserver.h"

static void n_registerlogclass(void* slf, nCmd* cmd);
static void n_log(void* slf, nCmd* cmd);
static void n_getdescription(void* slf, nCmd* cmd);
static void n_getwarninglevel(void* slf, nCmd* cmd);
static void n_setwarninglevel(void* slf, nCmd* cmd);
static void n_setlogenable(void* slf, nCmd* cmd);
static void n_getlogenable(void* slf, nCmd* cmd);
static void n_getlogclasses(void* slf, nCmd* cmd);
static void n_getlastlogs(void* slf, nCmd* cmd);
static void n_clearlogbuffer(void* slf, nCmd* cmd);
static void n_enableloggroup(void* slf, nCmd* cmd);
static void n_enableloggroupbyindex(void* slf, nCmd* cmd);
static void n_disableloggroup(void* slf, nCmd* cmd);
static void n_disableloggroupbyindex(void* slf, nCmd* cmd);
static void n_getenabledloggroups(void* slf, nCmd* cmd);
static void n_getgroupname(void* slf, nCmd* cmd);
static void n_getnumgroups(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    nlogclassregistry

    @cppclass
    nLogClassRegistry
    
    @superclass
    nroot

    @classinfo
    Is a intercace with a log system
*/
void
n_initcmds_nLogClassRegistry(nClass* cl)
{
    cl->BeginCmds();
    cl->AddCmd("v_register_ss",             'RCLG', n_registerlogclass);
    cl->AddCmd("v_log_sis",                 'LOLG', n_log);
    cl->AddCmd("s_getdescription_s",        'GDLG', n_getdescription);
    cl->AddCmd("i_getwarninglevel_s",       'GWLG', n_getwarninglevel);
    cl->AddCmd("v_setwarninglevel_si",      'SWLG', n_setwarninglevel);
    cl->AddCmd("b_getlogenable_s",          'GELG', n_getlogenable);
    cl->AddCmd("v_setlogenable_sb",         'SELG', n_setlogenable);
    cl->AddCmd("l_getlogclasses_v",         'GLLG', n_getlogclasses); 
    cl->AddCmd("l_getlastlogs_v",           'GELL', n_getlastlogs);
    cl->AddCmd("v_clearlogbuffer_v",        'CLBF', n_clearlogbuffer);
    cl->AddCmd("v_enableloggroup_si",       'ELGR', n_enableloggroup);
	cl->AddCmd("v_enableloggroupbyindex_si",'ELGI', n_enableloggroupbyindex);
    cl->AddCmd("v_disableloggroup_si",      'DLGR', n_disableloggroup);
	cl->AddCmd("v_disableloggroupbyindex_si",'DLGI', n_disableloggroupbyindex);
    cl->AddCmd("i_getenabledloggroups_s",   'GELG', n_getenabledloggroups);
    cl->AddCmd("s_getgroupname_si",         'GGRN', n_getgroupname);
    cl->AddCmd("i_getnumgroups_s",          'GNGR', n_getnumgroups);
    cl->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    register
    @input
    s(name's log)
    s(description)
    @output
    v
    @info
    create a new log.
*/

static void
n_registerlogclass(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char *name        = cmd->In()->GetS();
    const char *description = cmd->In()->GetS();

    self->RegisterLogClass( name, description);
}

//------------------------------------------------------------------------------
/**
    @cmd
    log
    @input
    s(name's log)
    i(level of this log)
    S(message)
    @output
    v
    @info
    Log 
*/

static void
n_log(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;

    const char* name  = cmd->In()->GetS();
    int         level = cmd->In()->GetI();
    const char*  msg  = cmd->In()->GetS();

    self->Log( name, level, msg );
}


//------------------------------------------------------------------------------
/**
    @cmd
    getdescription
    @input
    s(log's name)
    @output
    s(log's description)
    @info
    return the description of log
*/

static void
n_getdescription(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    cmd->Out()->SetS( self->GetDescription( name ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    getlogenable
    @input
    s(log's name)
    @output
    i(log's enable)
    @info
    return if this log is enable
*/

static void
n_getlogenable(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    cmd->Out()->SetB( self->GetEnable( name ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setlogenable
    @input
    s(log's name)
    b(enable)
    @output
    i(log's enable)
    @info
    return if this log is enable
*/

static void
n_setlogenable(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    bool      enable  = cmd->In()->GetB();
    self->SetEnable( name, enable );
}


//------------------------------------------------------------------------------
/**
    @cmd
    getwarninglevel
    @input
    s(log's name)
    @output
    i(log's warning level)
    @info
    return the maximun of warning level
*/

static void
n_getwarninglevel(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    cmd->Out()->SetI( self->GetWarningLevel( name ) );
}

//------------------------------------------------------------------------------
/**
    @cmd
    setwarninglevel
    @input
    s(log's name)
    i(warning level)
    @output
    v
    @info
    set the warning level of this log
*/

static void
n_setwarninglevel(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    int level         = cmd->In()->GetI();
    self->SetWarningLevel( name , level ) ;
}


//------------------------------------------------------------------------------
/**
    @cmd
    getlogclasses
    @input
    v
    @output
    l(list of log class)
    @info
    return list log class
*/

static void
n_getlogclasses(void* /*slf*/, nCmd* cmd)
{   
    nArray<nLogClass*> logList = LogClassRegistry::Instance()->GetList();

    nArg* nameList = n_new_array( nArg, logList.Size() );
    n_assert(nameList);

    for (int i = 0; i < logList.Size() ; i++  )
    {
        nameList[i].SetS( logList[i]->GetName() );
    }

    cmd->Out()->SetL(nameList, logList.Size());
}


//------------------------------------------------------------------------------
/**
    @cmd
    getlastlogs
    @input
    v
    @output
    l(list of log entries)
    @info
    Return a list of all the log entries in the log buffer.
    The log buffer is cleared afterwards.
*/

static void
n_getlastlogs(void* /*slf*/, nCmd* cmd)
{
    nLogBuffer* logBuffer = LogClassRegistry::Instance()->GetLogBuffer();
    nStrList& logList = logBuffer->GetLineList();

    // String list to arg array
    nArg* logArray = n_new_array( nArg, logBuffer->GetSize() );
    n_assert(logArray); 
    nStrNode* strnode = logList.GetHead();
    for ( int i = 0; i < logBuffer->GetSize(); ++i, strnode = strnode->GetSucc() )
    {
        logArray[i].SetS( strnode->GetName() );
    }

    cmd->Out()->SetL( logArray, logBuffer->GetSize() );
    logBuffer->Clear();
}


//------------------------------------------------------------------------------
/**
    @cmd
    clearlogbuffer
    @input
    v
    @output
    v
    @info
    Clear the log buffer.
*/

static void
n_clearlogbuffer(void* /*slf*/, nCmd* /*cmd*/)
{
    nLogBuffer* logBuffer = LogClassRegistry::Instance()->GetLogBuffer();
    logBuffer->Clear();
}

//------------------------------------------------------------------------------
/**
    @cmd
    enableloggroup
    @input
    s(log's name)
    i(group enable mask)
    @output
    v
    @info
    Enable the masked log groups in the named log class.
*/
static void
n_enableloggroup(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    int mask = cmd->In()->GetI();
    self->EnableLogGroup(name, mask);
}

//------------------------------------------------------------------------------
/**
    @cmd
    enableloggroupbyindex
    @input
    s(log's name)
    i(group enable index)
    @output
    v
    @info
    Enable the log group in the named log class.
*/
static void
n_enableloggroupbyindex(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    int index = cmd->In()->GetI();
    self->EnableLogGroupByIndex(name, index);
}

//------------------------------------------------------------------------------
/**
    @cmd
    disableloggroup
    @input
    s(log's name)
    i(group enable mask)
    @output
    v
    @info
    Disable the masked log groups in the named log class.
*/
static void
n_disableloggroup(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    int mask = cmd->In()->GetI();
    self->DisableLogGroup(name, mask);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getenabledloggroups
    @input
    s(log's name)
    @output
    i(group enable mask)
    @info
    Return the mask for the enabled log groups in the named log class.
*/
static void
n_getenabledloggroups(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    int mask = self->GetEnabledLogGroups(name);
    cmd->Out()->SetI(mask);
}

//------------------------------------------------------------------------------
/**
    @cmd
    disableloggroupbyindex
    @input
    s(log's name)
    i(group enable index)
    @output
    v
    @info
    Disable the log group in the named log class.
*/
static void
n_disableloggroupbyindex(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    int index = cmd->In()->GetI();
    self->DisableLogGroupByIndex(name, index);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getgroupname
    @input
    s(log's name)
    i(group mask)
    @output
    s(group name)
    @info
    Return the name of the group for the masked group in the named log class.
*/
static void
n_getgroupname(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    int index = cmd->In()->GetI();
    const char* groupName= self->GetGroupName(name, index);
    cmd->Out()->SetS(groupName);
}

//------------------------------------------------------------------------------
/**
    @cmd
    getnumgroups
    @input
    s(log's name)
    @output
    i(Number of groups)
    @info
    Return the number of groups in the named log class.
*/
static void
n_getnumgroups(void* slf, nCmd* cmd)
{
    nLogClassRegistry* self = (nLogClassRegistry*) slf;
    const char* name  = cmd->In()->GetS();
    int numGroups = self->GetNumGroups(name);
    cmd->Out()->SetI(numGroups);
}
