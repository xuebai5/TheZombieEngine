/**
   @file dllmain.cc
   @author Luis Jose Cabellos Gomez
   
   (C) 2005 Conjurer Services, S.A.
*/
#include "pluginutilfunctions/utilfunctions.h"
#include "pluginutilfunctions/n3dsnebulafunctions.h"

HINSTANCE hInstance;

/* dirty hack, it rewards me with the Hell of Hackers */
TCHAR* null_string = (TCHAR*)&"";
/* end dirty hack */

/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID /*lpvReserved*/) {
	hInstance = hinstDLL;
    if ( fdwReason == DLL_PROCESS_DETACH)
    {
        n3dsNebulaFunctions::WhenDetachProcess();
    }

    return(TRUE);
}

//------------------------------------------------------
// This is the interface to Max:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return "n3dsUtilFunctions"; }

__declspec( dllexport ) int LibNumberClasses() { return 0; }

__declspec( dllexport ) ClassDesc* LibClassDesc(int /*i*/) { return NULL; }

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG LibVersion() { return VERSION_3DSMAX; }

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
