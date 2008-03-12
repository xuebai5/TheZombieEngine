/**********************************************************************
 *<
	FILE: mtl.cpp

	DESCRIPTION:   DLL implementation of material and textures

	CREATED BY: Dan Silva

	HISTORY: created 12 December 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#include "pluginmtltxt/maxtexture.h"
#pragma warning( push, 3 )
#include "stdmat.h"
#pragma warning( pop )
#include "../res/pluginmtltxt/resource.h"
//#include "mtlresOverride.h"

HINSTANCE hInstance;
int controlsInit = FALSE;

// orb 01-03-2001 Removing map types
static void initClassDescArray(void);  // forward declaration

/** public functions **/
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID /*lpvReserved*/) {
	hInstance = hinstDLL;

	if ( !controlsInit ) 
    {
		controlsInit = TRUE;
		
		initClassDescArray();
		
		// jaguar controls
		InitCustomControls(hInstance);

		// initialize Chicago controls
		InitCommonControls();

	    switch(fdwReason) 
        {
		    case DLL_PROCESS_ATTACH:
			    break;
		    case DLL_THREAD_ATTACH:
			    break;
		    case DLL_THREAD_DETACH:
    			break;
	    	case DLL_PROCESS_DETACH:
		    	break;
		}

	    //return(TRUE);
	}
    return(TRUE);
}
//------------------------------------------------------
// This is the interface to Max:
//------------------------------------------------------

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_DS_MTLDESC); }

// orb - 01/03/01
// The classDesc array was created because it was way to difficult
// to maintain the switch statement return the Class Descriptors
// with so many different #define used in this module.

#define MAX_MTLTEX_OBJECTS 32
static ClassDesc *classDescArray[MAX_MTLTEX_OBJECTS];
static int classDescCount = 0;

void initClassDescArray(void)
{
    classDescArray[classDescCount++] = GetRGBAddDesc();
}

__declspec( dllexport ) int LibNumberClasses() { return classDescCount; }

// This function return the ith class descriptor.
__declspec( dllexport ) ClassDesc* 
LibClassDesc(int i) {
	if( i < classDescCount )
		return classDescArray[i];
	else
		return NULL;

	}



// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

TCHAR *GetString(int id)
{
	static TCHAR buf[256];
	if(hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}
