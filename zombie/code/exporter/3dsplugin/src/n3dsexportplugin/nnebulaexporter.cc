#include "precompiled/pchn3dsmaxexport.h"

//------------------------------------------------------------------------------
#pragma warning( push, 3 )
#include "IGame/IGameError.h"
#pragma warning( pop )

//------------------------------------------------------------------------------
#include "n3dsexportplugin/nNebulaExporter.h"
#include "n3dsexporters/n3dsExportServer.h"
#include "n3dsexporters/n3dslog.h"
#include "crashreportdump/exceptionhandler.h"

/*
   For Release of artist set all level 2
   convention use of N3DSERROR:
        The level 0 .. 2 is for artist messages
        the level > 3 is for programmers
*/

N3DSCREATELOGLEVEL ( visibilityexport , "Log for visibility" , true , 2 )
N3DSCREATELOGLEVEL ( meshexport , "Log for mesh export" , true , 2 )
N3DSCREATELOGLEVEL ( animexport , "Log for animation export" , true , 2 )
N3DSCREATELOGLEVEL ( materialExport , "Log for material export" , true , 2 )
N3DSCREATELOGLEVEL ( fileServer3ds , "Log for file copy" , true , 2 )
N3DSCREATELOGLEVEL ( physicExport, "Log for physics", true, 2 )
N3DSCREATELOGLEVEL ( exporter, "general Log", true, 2 )

//--- nNebulaExporter -------------------------------------------------------
nNebulaExporter::nNebulaExporter()
{
}

nNebulaExporter::~nNebulaExporter() 
{
}

//------------------------------------------------------------------------------
/**
    @returns The number of file name extensions supported by the plug-in
*/

int nNebulaExporter::ExtCount()
{
    return 1;
}

//------------------------------------------------------------------------------
/**
    @returns The 'i-th' file name extension
*/
const TCHAR *nNebulaExporter::Ext(int /*n*/)
{		
    return _T("n2");
}

//------------------------------------------------------------------------------
/**
    @returns The long description of plugin
*/
const TCHAR *nNebulaExporter::LongDesc()
{
    return _T("Nebula Exporter");
}

//------------------------------------------------------------------------------
/**
    @returns The short description of plugin
*/
const TCHAR *nNebulaExporter::ShortDesc() 
{			
    return _T("Nebula Exporter");
}

//------------------------------------------------------------------------------
/**
    @returns The author's name
*/
const TCHAR *nNebulaExporter::AuthorName()
{			
    return _T("Cristobal Castillo");
}

//------------------------------------------------------------------------------
/**
    @returns  Copyright message
*/
const TCHAR *nNebulaExporter::CopyrightMessage() 
{	
    return _T("Conjurer Services, S.A.");
}

const TCHAR *nNebulaExporter::OtherMessage1() 
{		
    return _T("");
}

const TCHAR *nNebulaExporter::OtherMessage2() 
{		
    return _T("");
}

//------------------------------------------------------------------------------
/**
    @returns  Version number * 100
    (i.e. v3.01 = 301)
*/
unsigned int nNebulaExporter::Version()
{				
    // @todo Return Version number * 100 (i.e. v3.01 = 301)
    return 1; //Version 0.01
}

void nNebulaExporter::ShowAbout(HWND /*hWnd*/)
{			
    // Optional
}

BOOL nNebulaExporter::SupportsOptions(int /*ext*/, DWORD /*options*/)
{
    // TODO Decide which options to support.  Simply return
    // true for each option supported by each Extension 
    // the exporter supports.
    return TRUE;
}

// Dummy function for progress bar
DWORD WINAPI fn(LPVOID /*arg*/)
{
    return(0);
}



class MyErrorProc : public IGameErrorCallBack
{
public:
    void ErrorProc(IGameError error)
    {
        TCHAR * buf = GetLastIGameErrorText();
        //DebugPrint("ErrorCode = %d ErrorText = %s\n", error,buf);
        //change the next line for n_error
        /*
        n_message("IGAME ERROR \n ErrorCode = %d ErrorText = %s\n", error,buf);

        UINT boxType = (MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST);

        if (MessageBox(0, "Continue?", "n3dsExporter", boxType)==IDNO)
        {
            exit(0);
        }
        */
        n_error("Nebula Exporter\n IGAME ERROR \n ErrorCode = %d\n ErrorText = %s\n", error,buf);
    }
};

//------------------------------------------------------------------------------
/**
    @param name the name of file to export
    @returns if exported
    @retval TRUE is exported
    @brief the main function called by 3dsMax when export
*/

int	nNebulaExporter::Export(const TCHAR *name, BOOL suppressPrompts, DWORD options)
{
    HRESULT hr;
    bool exportSelected = exportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;
    //int *TrickMemoryLeaks = n_new(int); //Use for detect memory leaks , base

    
    // use the tragNebula menu
    if ( suppressPrompts == FALSE  && !IsDebuggerPresent() )
    {
        //n_message (" go to TragNebula menu for export");
        MessageBox(0, " Go to TragNebula menu for export", "Nebula export error", (MB_OK | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST |MB_ICONERROR) );
        return IMPEXP_CANCEL;
    }

    MyErrorProc pErrorProc;
    SetErrorCallBack(&pErrorProc);  // Set the Igame error callback
    hr = CoInitialize(NULL); 
    if(FAILED(hr))
        return false;

    exportServer = n3dsExportServer::Instance();

    if ( exportServer->init( name, exportSelected) )
    {
        if ( exportServer->Export() )
        {
            if ( exportServer->Save() )
            {
                exportServer->RunScene();
            }
        }
    } 

    //Show  in the log the number of errors
    n3dsLog::PutErrorNumbers();

    exportServer->Close();
    CoUninitialize();	
    
    n_delete(exportServer);
    this->exportServer = 0;
    return IMPEXP_SUCCESS;
}


//------------------------------------------------------------------------------
/**
    @param name the name of file to export
    @returns if exported
    @retval TRUE is exported
    @brief the main function called by 3dsMax when export
*/

int	nNebulaExporter::DoExport(const TCHAR *name,ExpInterface* /*ei*/,Interface * /*i*/ , BOOL suppressPrompts, DWORD options)
{

    int result = IMPEXP_CANCEL;
    // Prevent export with different threads in the same time
    static bool running = false;
    if (running)
    {
        //n_message("the exporter is running");
        MessageBox(0, "The exporter is running", "Nebula export error", (MB_OK | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST |MB_ICONERROR) );
        return IMPEXP_CANCEL;
    }
    running = true;
 

    __try
    {
        __try
        {
            n3dsLog::Init();
            result = this->Export(name, suppressPrompts, options );
        } __except( RecordExceptionInfo(GetExceptionInformation(), "main thread") )
        {
            RaiseException (DBG_EXCEPTION_NOT_HANDLED,0,NULL,NULL);; 
        }
    } __except( IsDebuggerPresent() ?  EXCEPTION_CONTINUE_SEARCH : EXCEPTION_EXECUTE_HANDLER )
    {
        n3dsLog::AddError(); //  To force to show a log
        n3dsLog::ShowLog();
        RaiseException (DBG_EXCEPTION_NOT_HANDLED,0,NULL,NULL);
    }

    // Show log if it necsary
    n3dsLog::ShowLog();
    running = false;
    return result;
}
