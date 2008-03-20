#include "precompiled/pchrenaissance.h"
//------------------------------------------------------------------------------
/**
    @page ZombieGameRenaissance Renaissance

    Renaissance

    Main game application.

    (C) 2004 Conjurer Services, S.A.
*/

#include "renaissance/personal.h"
// Uncomment this line for activate visual leak detector
//#include "vld.h"    

#include "kernel/nkernelServer.h"
#include "kernel/ntimeserver.h"
#include "nrnsapp/nrnsapp.h"


#ifdef __WIN32__
#include "nsingleinst/singleinst.h"
#include "kernel/nwin32loghandler.h"
#include "tools/nwinmaincmdlineargs.h"
#include "crashreportdump/exceptionhandler.h"
#else
#include "tools/ncmdlineargs.h"
#endif

nNebulaUsePackage(nnebula);
nNebulaUsePackage(nscriptclassserver);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(nmaterial);
nNebulaUsePackage(nentity);
nNebulaUsePackage(nscene);
nNebulaUsePackage(noctree);
nNebulaUsePackage(nspatial);
nNebulaUsePackage(nlua);
nNebulaUsePackage(ngeomipmap);
nNebulaUsePackage(ngrass);
nNebulaUsePackage(nphysics);
nNebulaUsePackage(framework);
nNebulaUsePackage(napplication);
nNebulaUsePackage(rnsscene);
nNebulaUsePackage(zombieentity);
nNebulaUsePackage(zombieentityexp);
nNebulaUsePackage(ncommonapp);
nNebulaUsePackage(renaissanceapp);
nNebulaUsePackage(rnsstates);
nNebulaUsePackage(nnetworkenet);
nNebulaUsePackage(nnetwork);
nNebulaUsePackage(nnavmesh);
nNebulaUsePackage(nfsm);
nNebulaUsePackage(ntrigger);
nNebulaUsePackage(ntoollib);
nNebulaUsePackage(gameplay);
nNebulaUsePackage(gpbasicactions);
nNebulaUsePackage(rnsbasicactions);
nNebulaUsePackage(ndsound);
nNebulaUsePackage(nmusictable);
nNebulaUsePackage(ncsound);
nNebulaUsePackage(ncshared);
nNebulaUsePackage(rnsentity);
nNebulaUsePackage(nanimation);
nNebulaUsePackage(nwaypoint);
nNebulaUsePackage(rnsgameplay);
nNebulaUsePackage(nspecialfx);
nNebulaUsePackage(ngui);
nNebulaUsePackage(rnsnetwork);
nNebulaUsePackage(ndshow);

//------------------------------------------------------------------------------
/**
*/
#ifdef __WIN32__
int WINAPI
NebulaMain(HINSTANCE /*hInstance*/, HINSTANCE /*prevInstance*/, LPSTR lpCmdLine, int /*nCmdShow*/)
{
    nWinMainCmdLineArgs args(lpCmdLine);

#else
int
NebulaMain(int argc, char * argv[])
{
    nCmdLineArgs args(argc, argv);
#endif

    const char* startupArg      = args.GetStringArg("-startup", "home:data/scripts/rnsstartup.lua");
    const char* levelArg        = args.GetStringArg("-level", "wc:levels/default.n2");
    bool fullscreenArg          = args.GetBoolArg("-fullscreen");
    bool alwaysOnTopArg         = args.GetBoolArg("-alwaysontop");
    bool helpArg                = args.GetBoolArg("-help");
    int xPosArg                 = args.GetIntArg("-x", 0);
    int yPosArg                 = args.GetIntArg("-y", 0);
    int widthArg                = args.GetIntArg("-w", 1024);
    int heightArg               = args.GetIntArg("-h", 768);
    const char* projDir         = args.GetStringArg("-projdir", 0);
    const char* wcDir           = args.GetStringArg("-wc", 0);
    bool sync                   = args.GetBoolArg("-sync");
    const char* newInstance     = args.GetStringArg("-newinstance", 0);

#ifdef __WIN32__
    nSingleInstance instance("Renaissance");
    if (!newInstance && !instance.IsFirstInstance())
    {
        printf("Renaissance already running, use -newinstance <InstanceName> command line argument to force new instance");
        return 5;
    }
#endif

    // If the user needs an explanation, just provide one, and don't do anything else this execution
    if (helpArg)
    {
        printf("(C) 2004 Conjurer Services, S.A.\n"
               "Conjurer Editor\n"
               "Command line args:\n"
               "------------------\n"
               "-help                   show this help\n"
               "-startup                startup script to run, default is: home:data/scripts/startup.tcl\n"
               "-level                  game level to load at startup\n"
               "-fullscreen             if present, then nviewer will go fullscreen\n"
               "-alwaysontop            present the window will be allways on top\n"
               "-w                      width of window to open (default: 1024)\n"
               "-h                      height of window to open (default: 768)\n"
               "-x                      the x position of the window (default: 0)\n"
               "-y                      y position of the window (default: 0)\n"
               "-projdir                the optional project directory (assigns it to the projdir: alias, for use in the user's scripts)\n"
               "-wc                     optional working directory (assigns it to the wc: alias, for use when loading game resources)\n"
               "-sync                   vertical retrace synch\n"
               );
        return 5;
    }
    
    // initialize a display mode object
    nString title;
    if (levelArg)
    {
        title.Append(levelArg);
        title.Append(" - ");
    }
    title.Append("Renaissance");
    
    nDisplayMode2 displayMode;
    if (fullscreenArg)
    {
        displayMode.Set(title.Get(), nDisplayMode2::Fullscreen, (short) xPosArg, (short) yPosArg, (short) widthArg, (short) heightArg, sync, false, "NEBULAICON");
    }
    else if (alwaysOnTopArg)
    {
        displayMode.Set(title.Get(), nDisplayMode2::AlwaysOnTop, (short) xPosArg, (short) yPosArg, (short) widthArg, (short) heightArg, sync, false, "NEBULAICON");
    }
    else
    {
        displayMode.Set(title.Get(), nDisplayMode2::Windowed, (short) xPosArg, (short) yPosArg, (short) widthArg, (short) heightArg, sync, false, "NEBULAICON");
    }
    
    //displayMode.SetIcon( "NEBULAICON" );

    // initialize Nebula runtime
    nAtomTable * atomTable = nAtomTable::Instance();
    n_assert(atomTable);
    nKernelServer * kernelServer = n_new(nKernelServer);
    n_assert(kernelServer);
    nComponentClassServer * compClassServer = n_new(nComponentClassServer);
    n_assert(compClassServer);
    nComponentObjectServer * compObjectServer = n_new(nComponentObjectServer);
    n_assert(compObjectServer);

    #ifdef __WIN32__
        nWin32LogHandler logHandler("nconjurer");
        kernelServer->SetLogHandler(&logHandler);
    #endif
    kernelServer->AddPackage(nnebula);
    kernelServer->AddPackage(nentity);
    kernelServer->AddPackage(nscriptclassserver);
    kernelServer->AddPackage(ndinput8);
    kernelServer->AddPackage(ndirect3d9);
    kernelServer->AddPackage(nscene);
    kernelServer->AddPackage(nmaterial);
    kernelServer->AddPackage(nspatial);
    kernelServer->AddPackage(noctree);
    kernelServer->AddPackage(nlua);
    kernelServer->AddPackage(ngeomipmap);
    kernelServer->AddPackage(ngrass);
    kernelServer->AddPackage(nphysics);
    kernelServer->AddPackage(framework);
    kernelServer->AddPackage(napplication);
    kernelServer->AddPackage(ncsound);
    kernelServer->AddPackage(ncshared);
    kernelServer->AddPackage(nanimation);
    kernelServer->AddPackage(gameplay);
    kernelServer->AddPackage(nnavmesh);
    kernelServer->AddPackage(rnsgameplay);
    kernelServer->AddPackage(nspecialfx);
    kernelServer->AddPackage(ntrigger);
    kernelServer->AddPackage(nwaypoint);
    kernelServer->AddPackage(rnsscene);
    kernelServer->AddPackage(zombieentity);
    kernelServer->AddPackage(zombieentityexp);
    kernelServer->AddPackage(ncommonapp);
    kernelServer->AddPackage(renaissanceapp);
    kernelServer->AddPackage(nnetworkenet);
    kernelServer->AddPackage(nnetwork);
    kernelServer->AddPackage(rnsstates);
    kernelServer->AddPackage(nfsm);
    kernelServer->AddPackage(ntoollib);
    kernelServer->AddPackage(gpbasicactions);
    kernelServer->AddPackage(rnsbasicactions);
    kernelServer->AddPackage(ndsound);
    //kernelServer->AddPackage(nfsaudioserver3);
    //kernelServer->AddPackage(nopenal);
    kernelServer->AddPackage(nmusictable);
    kernelServer->AddPackage(rnsnetwork);
    kernelServer->AddPackage(rnsentity);
    kernelServer->AddPackage(ngui);
    kernelServer->AddPackage(ndshow);

    // enable frame time in time server
    kernelServer->GetTimeServer()->EnableFrameTime();

    // initialize a viewer app object
    nRnsApp *app = (nRnsApp *) kernelServer->New("nrnsapp", "/app/renaissance");
    app->SetCompanyName("Conjurer Services");
    app->SetAppName("renaissance");
    app->SetDisplayMode(displayMode);
    app->SetInstanceName(newInstance);

    if (levelArg)
    {
        app->SetLevelFile(levelArg);
    }

    if (projDir)
    {
        app->SetProjectDirectory(projDir);
    }

    if (wcDir)
    {
        app->SetWorkingCopyDir(wcDir);
    }

    app->SetStartupScript(startupArg);
    app->SetInitState( "menu");

    // open and run app
    if (app->Open())
    {       
        app->Run();
        app->Close();
    }

    if (compObjectServer)
    {
        n_delete(compObjectServer);
        compObjectServer = 0;
    }

    if (compClassServer)
    {
        n_delete(compClassServer);
        compClassServer = 0;
    }

    if (kernelServer)
    {
        n_delete(kernelServer);
        kernelServer = 0;
    }

    if (atomTable)
    {
        n_delete(atomTable);
        atomTable = 0;
    }

    return 0;
}

#ifdef __WIN32__
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    __try 
    {
        return NebulaMain(hInstance, prevInstance, lpCmdLine, nCmdShow);
    }
    #ifdef __WIN32__
    __except(RecordExceptionInfo(GetExceptionInformation(), "main thread")) 
    {
    }
    #endif

    return 1;
}
#else
int
main(int argc, const char** argv)
{
    __try 
    {
        return NebulaMain(argc, argv);
    }
    __except(RecordExceptionInfo(GetExceptionInformation(), "main thread")) 
    {
    }

    return 1;
}
#endif
