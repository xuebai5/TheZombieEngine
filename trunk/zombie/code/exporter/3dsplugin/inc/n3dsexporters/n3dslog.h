#ifndef N_3DS_LOG_H
#define N_3DS_LOG_H
//#include "n3dsexporters/n3dsExportServer.h"
#include "kernel/nlogclass.h"

class n3dsLog
{
public:
    static void Init();
    static void SetFileLog(const char* name);
    static bool GetShowLog();
    static void AddWarning();
    static void AddError();
    static bool GetHasError();
    static void ShowLog();
    static void PutErrorNumbers();

private:
    static unsigned int warningCount;
    static unsigned int errorCount;
    static nString fileLog;
    n3dsLog();
};

/// The 3ds error log
#define N3DSERROR( name, y) \
{ \
    NLOG( name , y) ; \
    n3dsLog::AddError(); \
}

/// The 3ds error log conditional, when the condition is true put a error
#define N3DSERRORCOND( name, expr, y) \
{  \
    bool expr2 = true == (expr); /* evaluate only one time the expresion */ \
    NLOGCOND(name , expr2 , y) ; \
    if ( expr2) \
    { \
        n3dsLog::AddError(); \
    } \
}

/// The 3ds warnign log
#define N3DSWARN( name, y) \
{ \
    NLOG( name , y) ; \
    n3dsLog::AddWarning(); \
}

/// The 3ds warning log conditional, when the condition is true put a error
#define N3DSWARNCOND( name, expr, y) \
{  \
    bool expr2 = true == (expr); /* evaluate only one time the expresion */ \
    NLOGCOND(name , expr2 , y) ; \
    if ( expr2) \
    { \
        n3dsLog::AddWarning(); \
    } \
}


#define N3DSCREATELOG( name, description ) NCREATELOG( name, description )
#define N3DSCREATELOGLEVEL( name, description, enable, level) NCREATELOGLEVEL( name, description, enable, level)

#endif