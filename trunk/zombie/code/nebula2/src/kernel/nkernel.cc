//------------------------------------------------------------------------------
//  nkernel.cc
//  (C) 2002 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchnkernel.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "kernel/ntypes.h"
#ifdef __ERROR_CRASH_REPORT__
#include "kernel/nwin32stacktrace.h"
#endif
#ifndef __STANDALONE__
#include "kernel/nkernelserver.h"
#include "kernel/nloghandler.h"
#endif

#ifdef __XBxX__
#include "xbox/nxbwrapper.h"
#elif defined(__WIN32__)
#include <windows.h>
#include <signal.h>
#elif defined(__LINUX__) || defined(__MACOSX__)
#include <time.h>
#include <dlfcn.h>
#endif

static useroption debugOption = UnknowUserOption;

void SetUserDebugOption(useroption option)
{
    debugOption = option;
}

useroption GetUserDebugOption()
{
    return debugOption;
}

//------------------------------------------------------------------------------
/**
    This function is called by n_assert() when the assertion fails.
*/
void 
n_barf(const char* exp, const char* file, int line, bool *ignore)
{
#ifdef __ERROR_CRASH_REPORT__
    *ignore = n_error("*** NEBULA ASSERTION ***\nexpression: %s\nfile: %s\nline: %d\n\nChosse an action:\n * Abort: Abort the application\n * Retry: Debug application\n * SHIFT + Retry: Send report\n * Ignore: Ignore assert\n * SHIFT + Ignore: Ignore ALWAYS this assert", exp, file, line);
#else
    n_error("*** NEBULA ASSERTION ***\nexpression: %s\nfile: %s\nline: %d\n", exp, file, line);
#endif
}

//------------------------------------------------------------------------------
/**
    This function is called by n_assert2() when the assertion fails.
*/
void
n_barf2(const char* exp, const char* msg, const char* file, int line, bool *ignore)
{
#ifdef __ERROR_CRASH_REPORT__
    *ignore = n_error("*** NEBULA ASSERTION ***\nprogrammer says: %s\nexpression: %s\nfile: %s\nline: %d\n\n Chosse an action:\n * Abort: Abort the application\n * Retry: Debug application\n * SHIFT + Retry: Make dump file and send report\n * Ignore: Ignore assert\n * SHIFT + Ignore: Ignore ALWAYS this assert\0", msg, exp, file, line);
#else
    n_error("*** NEBULA ASSERTION ***\nprogrammer says: %s\nexpression: %s\nfile: %s\nline: %d\n", msg, exp, file, line);
#endif
}

//------------------------------------------------------------------------------
/**
    This function is called when a serious situation is encountered which
    not requires abortion of the program, the user can choose between report the file 
	to the developers, ignore the error or abort the execution.
*/
#ifdef __ERROR_CRASH_REPORT__
bool crashReportAbortAlways = false;

bool
n_error(const char* msg,  ...) 
{
    va_list argList;
    va_start(argList, msg);
    int retval;

    #ifndef __STANDALONE__
    if (crashReportAbortAlways)
    {
         if (nKernelServer::ks && nKernelServer::ks->GetLogHandler() )
         {
            nKernelServer::ks->GetLogHandler()->Print(msg, argList);
         }
        retval = IDABORT;
    } 
    else if ( nKernelServer::ks && nKernelServer::ks->GetLogHandler() )
    {
        retval = nKernelServer::ks->GetLogHandler()->Assert(msg, argList);	
    } 
    else
    {
        char msgBuf[2048];
        vsnprintf(msgBuf, sizeof(msgBuf), msg, argList);
        retval = MessageBox(0,msgBuf, "",
                        MB_TASKMODAL|MB_ICONHAND|MB_ABORTRETRYIGNORE|MB_SETFOREGROUND|MB_OK);
    }

    if (retval == IDRETRY) 
    {
        SHORT key = GetAsyncKeyState(VK_SHIFT);
        if ((key | 0x8000) == key) 
        {
            SetUserDebugOption(DumpUserOption);
            RaiseException (DBG_EXCEPTION_NOT_HANDLED,0,NULL,NULL);
        } 
        else
        {
            #ifdef __ZOMBIE_EXPORTER__
            DebugBreak();
            #endif
            SetUserDebugOption(DebugUserOption);                
            RaiseException (DBG_EXCEPTION_NOT_HANDLED,0,NULL,NULL);
        }
    }
    else if (retval == IDIGNORE)
    {
        SHORT key = GetAsyncKeyState(VK_SHIFT);
        if ((key | 0x8000) == key)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (retval == IDABORT)
    {
        nWin32StackTrace *stacktracer = new nWin32StackTrace();
        const char* stack = stacktracer->TraceStack();
        n_printf("\n----- Nebula stack before user abort -----\n");
        //n_printf  only strlen(messagge)<1024
        /* char* stack2=(char*)stack;
        while(*stack2)
        {
            int idx=0;
            while( idx<100 && stack2[idx])
            {
                idx++;
            }
            char c = stack2[idx];
            stack2[idx] = 0;
            n_printf(stack2);
            stack2[idx] = c;
            stack2+=idx;
        }
        */
        n_puts(stack);
        n_printf("----------- END STACK --------------\n");
        #ifdef __ZOMBIE_EXPORTER__
        //This is not a main thread, has problem with exit code
        SetUserDebugOption(DebugUserOption);                
        RaiseException (DBG_EXCEPTION_NOT_HANDLED,0,NULL,NULL);
        #else
        raise(SIGABRT); 
        _exit(3);
        #endif
    }
    #else
    vprintf(msg, argList);
    fflush(stdout);
    #endif
    return false;
}
#else

//------------------------------------------------------------------------------
/**
    This function is called when a serious situation is encountered which
    requires abortion of the program.
*/
void
__cdecl
n_error(const char* msg, ...)
{
    va_list argList;
    va_start(argList, msg);
    
    #ifndef __STANDALONE__
        if (nKernelServer::ks && nKernelServer::ks->GetLogHandler())
        {
            nKernelServer::ks->GetLogHandler()->Error(msg, argList);			            
        }
        else
        {
        #if (defined __WIN32__ && !defined(__XBxX__))
            char msgBuf[2048];
            UINT boxType = (MB_OK | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST |MB_ICONERROR);
            vsnprintf(msgBuf, sizeof(msgBuf) , msg, argList);
            MessageBox(0, msgBuf, "", boxType);
        #else
            vprintf(msg, argList);
            fflush(stdout);
        #endif
        }
    #else
        vprintf(msg, argList);
        fflush(stdout);
    #endif
    abort();
};        
#endif
//------------------------------------------------------------------------------
/**
    This function is called when an important message should be displayed
    to the user, but which does not abort the program.
*/
void
__cdecl
n_message(const char* msg, ...)
{
    va_list argList;
    va_start(argList, msg);
    #ifndef __STANDALONE__
        if (nKernelServer::ks && nKernelServer::ks->GetLogHandler())
        {
            nKernelServer::ks->GetLogHandler()->Message(msg, argList);
        }
        else
        {
            vprintf(msg, argList);
        }
    #else
        vprintf(msg, argList);
    #endif
};        

//------------------------------------------------------------------------------
/**
    Nebula's printf replacement. Will redirect the text to the console
    and/or logfile.

     - 04-Dec-04   Zombie   created
*/
void 
__cdecl
n_vprintf(const char *msg, va_list argList)
{
    #ifndef __STANDALONE__
        if (nKernelServer::ks && nKernelServer::ks->GetLogHandler())
        {
            nKernelServer::ks->GetLogHandler()->Print(msg, argList);
        }
        else
        {
            vprintf(msg, argList);
        }
    #else
        vprintf(msg, argList);
    #endif
}

//------------------------------------------------------------------------------
/**
    Nebula's printf replacement. Will redirect the text to the console
    and/or logfile.

     - 27-Nov-98   floh    created
*/
void 
__cdecl
n_printf(const char *msg, ...)
{
    va_list argList;
    va_start(argList,msg);
    n_vprintf(msg, argList);
    va_end(argList);
}


//------------------------------------------------------------------------------
/**
    Nebula's puts replacement. Will redirect the text to the console
    and/or logfile.

     - 04-Dec-04     Zombie     created
*/
void 
__cdecl
n_puts(const char *msg)
{
    #ifndef __STANDALONE__
        if (nKernelServer::ks && nKernelServer::ks->GetLogHandler())
        {
            nKernelServer::ks->GetLogHandler()->Print(msg, 0);
        }
        else
        {
            printf(msg);
        }
    #else
        printf(msg);
    #endif
}

//------------------------------------------------------------------------------
/**
    Put the message to the debug window.

    - 26-Mar-05    kims    created
*/
void
__cdecl
n_dbgout(const char *msg, ...)
{
    va_list argList;
    va_start(argList,msg);
#ifndef __STANDALONE__
    if (nKernelServer::ks && nKernelServer::ks->GetLogHandler())
    {
        nKernelServer::ks->GetLogHandler()->OutputDebug(msg, argList);
    }
    else
    {
        vprintf(msg, argList);
    }
#else
    vprintf(msg, argList);
#endif
    va_end(argList);
}

//------------------------------------------------------------------------------
/**
    Put process to sleep.

     - 21-Dec-98   floh    created
*/
void 
n_sleep(double sec)
{
#   ifdef __WIN32__
    int milli_sec = (int) (sec * 1000.0);
    Sleep(milli_sec);
#   elif defined(__LINUX__) || defined(__MACOSX__)
    struct timespec t;
    #define N_NANOSEC_INT 1000000000
    #define N_NANOSEC_FLT 1000000000.0
    long long int nano_sec = (long long int) (sec * N_NANOSEC_FLT);
    t.tv_sec  = nano_sec / N_NANOSEC_INT;
    t.tv_nsec = nano_sec % N_NANOSEC_INT;
    nanosleep(&t,NULL);
#   else
#   warning n_sleep not yet implemented for this platform.
#   endif
}

//------------------------------------------------------------------------------
/**
    A strdup() implementation using Nebula's malloc() override.
    
     - 17-Jan-99   floh    created
*/
char*
n_strdup(const char* from)
{
    n_assert(from);
    char *to = (char *) n_malloc(strlen(from) + 1);
    if (to) 
    {
        strcpy(to, from);
    }
    return to;
}

//------------------------------------------------------------------------------
/**
    A safe strncpy() implementation.

     - 19-Feb-99   floh    created
*/
char*
n_strncpy2(char *dest, const char *src, size_t size)
{
    strncpy(dest,src,size);
    dest[size - 1] = 0;
    return dest;
}

//------------------------------------------------------------------------------
/**
    A safe strcat implementation.

     - 06-Mar-00   floh    created
*/
void 
n_strcat(char *dest, const char *src, size_t N_IFDEF_ASSERTS(dest_size) )
{
    n_assert(static_cast<unsigned int>( strlen(dest) + strlen(src) + 1 ) < dest_size);
    strcat(dest, src);
}

//------------------------------------------------------------------------------
/**
    A wrapper for loading a dll.

     - 10-May-99   floh    created
     - 02-Aug-01   leaf    + error description in win32
     - 03-Aug-01   leaf    + error description in linux, 
                             thanks to Warren Baird
*/
#ifdef N_STATIC
    // NOTHING
#elif __LINUX__
void*
n_dllopen(const char* name)
{
    char dll_name[N_MAXPATH];
    strcpy(dll_name, "lib");
    strcat(dll_name, name);
    strcat(dll_name, ".so");
    void *dll = dlopen(dll_name, RTLD_NOW|RTLD_GLOBAL);
    if (!dll) 
    {
        n_printf("Could not load dll '%s'\n", dll_name);
        // Find out why we failed
        char *err = dlerror();
        if (err) 
        {
              n_printf("Error was:\n%s\n",err);
        }
    }
    return dll;
}
n_assert
#elif __WIN32__
    void*
    n_dllopen(const char *name)
    {
        HINSTANCE dll;
        dll = LoadLibrary((LPCSTR) name);
        if (!dll) 
        {
            // Find out why we failed
            // Display the string.
            n_printf("Could not load dll '%s'\nError was:\n%s\n", name, n_getlastsystemerror.Get());
        }
        return (void*) dll;
    }
#else
#error "n_dllopen() NOT IMPLEMENTED"
#endif

//------------------------------------------------------------------------------
/**
    Wrapper for unloading a dll.

     - 10-May-99   floh    created
*/
#ifdef N_STATIC
    
    // NOTHING!

#elif __LINUX__
void 
n_dllclose(void* dll)
{
    dlclose(dll);
}
#elif __WIN32__
    void 
    n_dllclose(void* dll)
    {
        FreeLibrary((HMODULE) dll);
    }
#else
#error "n_dllclose() NOT IMPLEMENTED"
#endif

//------------------------------------------------------------------------------
/**
    Wrapper for getting a symbol address in a dll.
*/
#ifdef N_STATIC

    // NOTHING

#elif __LINUX__
    void*
    n_dllsymbol(void* dll, const char* sym)
    {
        void *h = dlsym(dll, sym);
        if (!h) n_error("n_dllsymbol(%s) failed!\n", sym);    
        return h;
    }
#elif __WIN32__
    void*
    n_dllsymbol(void *dll, const char *sym)
    {
        FARPROC h = GetProcAddress((HMODULE)dll, (LPCSTR)sym);
        if (!h) n_error("n_dllsymbol(%s) failed!\n", sym);
        return (void *) h;
    }
#else
#error "n_dllsymbol() NOT IMPLEMENTED"
#endif

//------------------------------------------------------------------------------
/**
    A string matching function using Tcl's matching rules.
*/
bool 
n_strmatch(const char* str, const char* pat)
{
    char c2;

    for(;;)
    {
        if (*pat == 0) 
        {
            if (*str == 0) return true;
            else           return false;
        }
        if ((*str == 0) && (*pat != '*')) return false;
        if (*pat=='*') 
        {
            pat++;
            if (*pat==0) return true;
            for(;;) 
            {
                if (n_strmatch(str, pat)) return true;
                if (*str==0) return false;
                str++;
            }
        }
        if (*pat=='?') goto match;
        if (*pat=='[') 
        {
            pat++;
            for(;;) 
            {
                if ((*pat==']') || (*pat==0)) return false;
                if (*pat==*str) break;
                if (pat[1] == '-') 
                {
                    c2 = pat[2];
                    if (c2==0) return false;
                    if ((*pat<=*str) && (c2>=*str)) break;
                    if ((*pat>=*str) && (c2<=*str)) break;
                    pat+=2;
                }
                pat++;
            }
            while (*pat!=']') 
            {
                if (*pat==0) 
                {
                    pat--;
                    break;
                }
                pat++;
            }
            goto match;
        }
    
        if (*pat=='\\') 
        {
            pat++;
            if (*pat==0) return false;
        }
        if (*pat!=*str) return false;

match:
        pat++;
        str++;
    }
}

//------------------------------------------------------------------------------
/**
    Convert 4 character string to FourCC code.
*/
nFourCC
n_strtofourcc(const char* str)
{
    n_assert(str);
    n_assert(strlen(str) == 4);
    // Since str is guaranteed to be 4 in length...
    return MAKE_FOURCC(str[0], str[1], str[2], str[3]);
}

//------------------------------------------------------------------------------
/**
    Convert FourCC code into character string. Note: the returned string
    will be destroyed on the next function invocation.
*/
const char*
n_fourcctostr(nFourCC fourcc)
{
    static char buf[5];
    buf[0] = static_cast<char>( (fourcc)     & 0xff );
    buf[1] = static_cast<char>( (fourcc>>8)  & 0xff );
    buf[2] = static_cast<char>( (fourcc>>16) & 0xff );
    buf[3] = static_cast<char>( (fourcc>>24) & 0xff );
    buf[4] = 0;
    return buf;
}

//------------------------------------------------------------------------------
/**
    Get the last system erros as nstring
*/
nString 
n_getlastsystemerror()
{
#ifdef __WIN32__
    LPVOID lpMsgBuf;
    FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL 
    );
    nString string((const char*)lpMsgBuf);

    // Free the buffer.
    LocalFree(lpMsgBuf);
    return string;
#else
	return nString("Get last error not implemented");
#endif
}
