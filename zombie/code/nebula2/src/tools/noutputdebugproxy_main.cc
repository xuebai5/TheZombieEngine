//------------------------------------------------------------------------------
//  noutputdebugproxy_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnoutputdebugproxylib.h"
#include "tools/noutputdebugproxy.h"
#ifdef USE_DETOURS
#include <detours.h>
#endif
#include "kernel/nlogclass.h"


const char * nOutputDebugProxy::logNames[] = {
        "D3D Err",
        "D3D Warn",
        "D3D others",
        "Others",
        0
        };


/// level 0..3 Error , level 4..7 warnings  , level 8 ..11 info

NCREATELOGLEVELGROUP ( OutputDebug , "outputDebug" , true , 12 ,  nOutputDebugProxy::logNames , NLOG_GROUP_MASK)


int nOutputDebugProxy::countOuputDebugProxy = 0 ;
OutputDebugString_Type  nOutputDebugProxy::trueOutputDebugString = 0;
nOutputDebugProxy::TypeBreak nOutputDebugProxy::flagsBreakPoint = nOutputDebugProxy::NoneBreak;


// create the default proxy for replace outputdebugfunction
nOutputDebugProxy  defaultOutputDebugProxy;

//------------------------------------------------------------------------------
/**
    @param previousType the type the previous message
    @param lpOutputString the current message
    @return Evaluate lpOutputString for get the type of message, if not has type and not has new line
    return the previous type because it is in the same line.
*/
nOutputDebugProxy::Type 
nOutputDebugProxy::GetType(Type previousType, LPCSTR lpOutputString)
{
    static const char* strDirect3D9("Direct3D9:");
    static const char* strWarn(" (WARN)");
    static const char* strErr(" (ERR");
    static const size_t  lenDirect3D9(strlen(strDirect3D9));
    static const size_t  lenWarn(strlen(strWarn));
    static const size_t  lenErr(strlen(strErr));

   
    if ( (lpOutputString == 0) || lpOutputString[0] == '\n' )
    {
        return Others;
    } 
    else if (0 == strncmp( lpOutputString , strDirect3D9 , lenDirect3D9)) 
    {
        // Put breakpoint her for d3d messages
        if ( 0 == strncmp( lpOutputString + lenDirect3D9, strWarn , lenWarn) )
        {
            Break(D3DWarnBreak);
            return D3DWarn; // Put breakpoint here for break in d3d warnings
        } 
        else if ( 0 == strncmp ( lpOutputString + lenDirect3D9, strErr, lenErr ) )
        {
            Break(D3DErrBreak);
            return D3DErr; // Put breakpoint here for break in d3d errors
        } else
        {
            Break(OthersBreak);
            return D3DOth;
        }
    } 
    else
    {
        if (previousType == OthersBreak) Break(OthersBreak);
        // if not has
        return previousType;
    }
}


//------------------------------------------------------------------------------
/**
*/
nOutputDebugProxy::nOutputDebugProxy()
{
    ++nOutputDebugProxy::countOuputDebugProxy;
    // Only the first instance change the OutputDebugString hook
    if ( nOutputDebugProxy::countOuputDebugProxy == 1  &&
        nOutputDebugProxy::trueOutputDebugString == 0 )
    {
        trueOutputDebugString = OutputDebugString;
#ifdef USE_DETOURS
        if ( NO_ERROR ==  DetourTransactionBegin() )
        {
            bool isOk = (NO_ERROR == DetourUpdateThread(GetCurrentThread()) );
            isOk = isOk  && (NO_ERROR == DetourAttach(&(PVOID&)trueOutputDebugString, myOutputDebugString) );
            if ( isOk )
            {
                DetourTransactionCommit();
            } else
            {
                trueOutputDebugString = 0;
                OutputDebugString("I can't change the OutputDebugString function");
            }
        }
#endif
    } 
    else
    {
        //nothing -> no conect
    }   
}

//------------------------------------------------------------------------------
/**
*/
nOutputDebugProxy::~nOutputDebugProxy()
{
    if ( nOutputDebugProxy::countOuputDebugProxy > 0)
    {
        --nOutputDebugProxy::countOuputDebugProxy;
        if ( (nOutputDebugProxy::countOuputDebugProxy == 0) &&
             (trueOutputDebugString != 0) // the function is changed
           )
        {
#ifdef USE_DETOURS
            if ( DetourTransactionBegin() )
            {
                bool isOk = (NO_ERROR == DetourUpdateThread(GetCurrentThread()));
                isOk = isOk &&( NO_ERROR == DetourDetach(&(PVOID&)trueOutputDebugString, myOutputDebugString) );
                if (isOk)
                {
                    DetourTransactionCommit();
                    trueOutputDebugString = 0;
                } 
                else
                {
                    OutputDebugString("Can't not put the original OutputDebugString function");
                }

            }
#endif
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
VOID WINAPI 
nOutputDebugProxy::myOutputDebugString(IN LPCSTR lpOutputString)
{
    static bool preventInfiniteRecursion(false); //some fucntion call outputDebugString
    static Type type(Others);

    if ( trueOutputDebugString)
    {   
        trueOutputDebugString(lpOutputString);
        if (!preventInfiniteRecursion) 
        {
            size_t len  = strlen( lpOutputString);
            preventInfiniteRecursion = true; //Active prevent infinite recursion

            type = nOutputDebugProxy::GetType(type,lpOutputString); //Get the type

            //show the log if not is only change line
            NLOG( OutputDebug, (0 | type, lpOutputString) );
            preventInfiniteRecursion = false;

            
            // for the next time if has change the line then is other type of message
            if ( (lpOutputString + len + 1)[0] == '\n')
            {
                type = Others;
            }
        }
    }
}

