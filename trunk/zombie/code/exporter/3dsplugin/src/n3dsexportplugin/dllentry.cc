#include "precompiled/pchn3dsmaxexport.h"
/**
   @file DllEntry.cc
   @brief Main function of 3ds max exporter
   @brief $Id$ 
*/
//------------------------------------------------------------------------------
#pragma warning( push, 3 )
#include "istdplug.h"
#include "iparamb2.h"
#pragma warning( pop )

#include "n3dsexportplugin/nNebulaExporter.h"
#include "n3dsNebulaRuntime/n3dsNebulaRuntime.h"


HINSTANCE hInstance;
int controlsInit = FALSE;

//------------------------------------------------------------------------------
/**
    @class nNebulaExporterClassDesc
    @ingroup n3dsMaxExporterKernel

    @brief The application class of RenaissanceGame
*/
class nNebulaExporterClassDesc:public ClassDesc2 {
public:
    int 			IsPublic() { return TRUE; }
    void *			Create(BOOL /*loading = FALSE*/) { return new nNebulaExporter(); }
    const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
    SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
    Class_ID		ClassID() { return NEBULA_EXPORTER_CLASS_ID; }
    const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

    const TCHAR*	InternalName() { return _T("nNebulaExporter"); }	// returns fixed parsable name (scripter-visible name)
    HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static nNebulaExporterClassDesc NebulaExporterDesc;
ClassDesc2* GetNebulaExporterDesc() { return &NebulaExporterDesc; }


BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason, LPVOID /*lpvReserved*/)
{
    hInstance = hinstDLL;				// Hang on to this DLL's instance handle.

    if (!controlsInit) {
        controlsInit = TRUE;
        InitCustomControls(hInstance);	// Initialize MAX's custom controls
        InitCommonControls();			// Initialize Win95 controls
    }

    switch(fdwReason) 
    {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
    		break;
	    case DLL_PROCESS_DETACH:
            n3dsNebulaRuntime::WhenDetachProcess();
		    break;
	}

    return (TRUE);
}

__declspec( dllexport ) const TCHAR* LibDescription()
{
    return GetString(IDS_LIBDESCRIPTION);
}


__declspec( dllexport ) int LibNumberClasses()
{
    return 1;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
    switch(i) {
        case 0: return GetNebulaExporterDesc();
        default: return 0;
    }
}

__declspec( dllexport ) ULONG LibVersion()
{
    return VERSION_3DSMAX;
}

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
    return 1;
}

TCHAR *GetString(int id)
{
    static TCHAR buf[256];

    if (hInstance)
        return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
    return NULL;
}

