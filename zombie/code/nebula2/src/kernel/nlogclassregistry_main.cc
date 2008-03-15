//------------------------------------------------------------------------------
//  nlogclassregistry_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include "kernel/nkernelserver.h"
#include "kernel/nlogclassregistry.h"

nNebulaScriptClass(nLogClassRegistry, "nroot");


NCREATELOGLEVEL_REL( defaultLog , "Default", true , 1);

//------------------------------------------------------------------------------
/**
*/
nLogClassRegistry::~nLogClassRegistry()
{
    const int num = dynamicLog.Size();
    for ( int idx = 0; idx < num ; idx++)
    {
        n_delete ( dynamicLog[idx] );
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLogClassRegistry::RegisterLogClass(const char*name, const char* description)
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find( name );

    if (!logClass)
    {
        logClass = n_new( nLogClass( name , description, false ) );
        dynamicLog.Append(logClass);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
nLogClassRegistry::Log(const char*name, int level, const char* msg)
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        logClass->Log( level, msg);
    } 
    else
    {
        NLOG( defaultLog , (level, "%s: %s", name, msg) );
    }
}



//------------------------------------------------------------------------------
/**
*/
const char* 
nLogClassRegistry::GetDescription(const char*name) const
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        return logClass->GetDescription();

    } 
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }
    return 0;

}

//------------------------------------------------------------------------------
/**
*/
int 
nLogClassRegistry::GetWarningLevel(const char* name) const
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        return logClass->GetLevelLog();
    } 
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
nLogClassRegistry::SetWarningLevel(const char* name, int level)
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        logClass->SetLevelLog( level );
    } 
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }

}

//------------------------------------------------------------------------------
/**
*/
void 
nLogClassRegistry::SetEnable(const char* name, bool enable)
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        logClass->SetEnable( enable );
    } 
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }
}

//------------------------------------------------------------------------------
bool 
nLogClassRegistry::GetEnable(const char* name) const
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        return logClass->GetEnable();
    } 
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }

    return false;
}



//------------------------------------------------------------------------------
/*
    enable log groups
*/
void 
nLogClassRegistry::EnableLogGroup(const char* name, int mask)
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        return logClass->EnableLogGroup(mask);
    }
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }
}

//------------------------------------------------------------------------------
/*
    disable log group    
*/
void 
nLogClassRegistry::DisableLogGroup(const char * name, int mask)
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        logClass->DisableLogGroup(mask);
    }
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }    
}

//------------------------------------------------------------------------------
/*
    enable log groups by index
*/
void 
nLogClassRegistry::EnableLogGroupByIndex(const char* name, int index)
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        return logClass->EnableLogGroupByIndex(index);
    }
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }
}

//------------------------------------------------------------------------------
/*
    disable log group by index
*/
void 
nLogClassRegistry::DisableLogGroupByIndex(const char * name, int index)
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        logClass->DisableLogGroupByIndex(index);
    }
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }    
}

//------------------------------------------------------------------------------
/*
*/
int
nLogClassRegistry::GetEnabledLogGroups(const char * name) const
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        return logClass->GetEnabledLogGroups();
    }
    else 
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }

    return 0;
}

//------------------------------------------------------------------------------
const char * 
nLogClassRegistry::GetGroupName(const char * name, int index) const
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        return logClass->GetGroupName(index);
    }
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }

    return 0;
}

//------------------------------------------------------------------------------
int 
nLogClassRegistry::GetNumGroups(const char * name) const
{
    nLogClass* logClass = LogClassRegistry::Instance()->Find(name);
    if ( logClass )
    {
        return logClass->GetNumGroups();
    }
    else
    {
        NLOG( defaultLog , (1, " log % no exists", name ) );
    }

    return 0;
}
