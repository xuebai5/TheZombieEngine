#ifndef N_OUTPUTDEBUGPROXY_H
#define N_OUTPUTDEBUGPROXY_H
//------------------------------------------------------------------------------
/**
    @class nOutputDebugProxy
    @ingroup Tools

    A class for intercept  OuputDebugString function
    
    (C) 2006 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------

#pragma comment(lib, "detoured.lib")
#pragma comment(lib, "detours.lib")

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "kernel/nlogclass.h"

typedef VOID (WINAPI* OutputDebugString_Type)(IN LPCSTR lpOutputString);


class nOutputDebugProxy 
{
public:
    /// Replace the original  OutputDebugString
    nOutputDebugProxy();
    /// Restaure the original OutputDebugString
    ~nOutputDebugProxy();

    typedef enum {
        NoneBreak     =       0,
        D3DErrBreak   =  1 << 0,
        D3DWarnBreak  =  1 << 1,
        D3DOtherBreak =  1 << 2,
        OthersBreak   =  1 << 3,
    } TypeBreak;
    // Raise breakpoints
    static TypeBreak flagsBreakPoint;
    
private:
    static int countOuputDebugProxy;

    static OutputDebugString_Type  trueOutputDebugString;

    static VOID WINAPI myOutputDebugString(IN LPCSTR lpOutputString);
public:
    ///  public  because it needed create log leverl this need initialize
    static const char * logNames[];
    /// type of outputs
    typedef enum {
        D3DErr = NLOG1,
        D3DWarn = NLOG2,
        D3DOth = NLOG3,
        Others = NLOG4,
    } Type;

    /// break if it is enable
    static void Break(TypeBreak val);
    /// returnt the type
    static Type GetType(Type previousType, LPCSTR lpOutputString);
};

//------------------------------------------------------------------------------
/**
    @param val if it is enable in flags then break debugger
*/

inline void
nOutputDebugProxy::Break( TypeBreak val )
{
    if ( val && nOutputDebugProxy::flagsBreakPoint )
    {
        DebugBreak();
    } 
}


//------------------------------------------------------------------------------
#endif//N_OUTPUTDEBUGPROXY_H
