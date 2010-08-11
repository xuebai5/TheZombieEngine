#include "precompiled/pchsummoner.h"
//------------------------------------------------------------------------------
/**
    Summoner
*/

#include "conjurer/nsummonerapp.h"
#include "conjurer/personal.h"
// Uncomment this line to activate visual leak detector
//#define VLD_AGGREGATE_DUPLICATES
//#define VLD_MAX_TRACE_FRAMES 16
//#define VLD_SHOW_USELESS_FRAMES
//#include "vld.h"    

#include "kernel/natomtable.h"
#include "kernel/nkernelserver.h"
#include "kernel/ntimeserver.h"
#include "conjurer/nconjurerapp.h"
#include "rnsstates/rnswanderstate.h"

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
nNebulaUsePackage(ngrass_conjurer);
nNebulaUsePackage(nphysics);
nNebulaUsePackage(npythonserver);
nNebulaUsePackage(framework);
nNebulaUsePackage(ndebug);
nNebulaUsePackage(napplication);
nNebulaUsePackage(ndebug);
nNebulaUsePackage(rnsscene);
nNebulaUsePackage(zombieentity);
nNebulaUsePackage(zombieentityexp);
nNebulaUsePackage(ncommonapp);
nNebulaUsePackage(conjurerapp);
nNebulaUsePackage(conjurerexp);
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
nNebulaUsePackage(nopenal);
nNebulaUsePackage(nmusictable);
nNebulaUsePackage(ncsound);
nNebulaUsePackage(ncshared);
nNebulaUsePackage(rnsentity);
nNebulaUsePackage(nanimation);
nNebulaUsePackage(nwaypoint);
nNebulaUsePackage(rnsnetwork);
nNebulaUsePackage(rnsgameplay);
nNebulaUsePackage(ngui);
nNebulaUsePackage(nspecialfx);
nNebulaUsePackage(ndshow);

nNebulaUsePackage(summoner);

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

    const char* startupArg      = args.GetStringArg("-startup", "home:data/scripts/startup.lua");
    const char* viewArg         = args.GetStringArg("-view", 0);
    const char* viewArg2        = args.GetStringArg("-view2", 0);
    const char* classArg        = args.GetStringArg("-class", 0);
    const char* levelArg        = args.GetStringArg("-level", "wc:levels/default.n2");
    const char* layoutArg       = args.GetStringArg("-layout", "home:data/appdata/conjurer/viewport/appviewportui.n2");
    const char* stateArg        = args.GetStringArg("-state", "editor");
    int animArg                 = args.GetIntArg("-anim", 0);
    bool fullscreenArg          = args.GetBoolArg("-fullscreen");
    bool alwaysOnTopArg         = args.GetBoolArg("-alwaysontop");
    bool helpArg                = args.GetBoolArg("-help");
    int xPosArg                 = args.GetIntArg("-x", 0);
    int yPosArg                 = args.GetIntArg("-y", 0);
    int widthArg                = args.GetIntArg("-w", 1024);
    int heightArg               = args.GetIntArg("-h", 768);
    const char* projDir         = args.GetStringArg("-projdir", 0);
    const char* wcDir           = args.GetStringArg("-wc", 0);
    const char* wc2Dir          = args.GetStringArg("-wc2", 0);
    const char* watch           = args.GetStringArg("-watch","");
    bool sync                   = args.GetBoolArg("-sync");
    const char* newInstance     = args.GetStringArg("-newinstance", 0);
    float lockFrameTime          = args.GetFloatArg("-lockframetime", 0.0);

#ifdef __WIN32__
    nSingleInstance instance("Conjurer");
    if (!newInstance && !instance.IsFirstInstance())
    {
        printf("Conjurer already running, use -newinstance <InstanceName> command line argument to force new instance");
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
               "-startup                startup script to run, default is: home:bin/startup.tcl\n"
               "-view                   data to load and view with the lighting setup specified (either -light or the default)\n"
               "-view2                  data to load and view, and set PROJECT to view/export  with the lighting setup specified (either -light or the default)\n"
	           "-light                  light stage to load, default is: wc:libs/system/lights/stdlight.n2\n"
               "-fullscreen             if present, then nviewer will go fullscreen\n"
               "-alwaysontop            present the window will be allways on top\n"
               "-w                      width of window to open (default: 1024)\n"
               "-h                      height of window to open (default: 768)\n"
               "-x                      the x position of the window (default: 0)\n"
               "-y                      y position of the window (default: 0)\n"
               "-projdir                the optional project directory (assigns it to the projdir: alias, for use in the user's scripts)\n"
               "-wc                     optional working directory (assigns it to the wc: alias, for use when loading game resources)\n"
               "-wc2                    optional temporary working directory\n"
               "-newinstance <NAME>     Allows to create new instances of Conjurer\n"
               "-lockframetime <VAL>    Lock frame time to VAL ms\n");
        return 5;
    }
    
    // initialize a display mode object
    nString title;
    if (viewArg)
    {
        title.Append(viewArg);
        title.Append(" - ");
    }
    title.Append("Conjurer Editor");
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
    kernelServer->AddPackage(ngrass_conjurer);
    kernelServer->AddPackage(nphysics);
    kernelServer->AddPackage(npythonserver);
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
    kernelServer->AddPackage(ndebug);
    kernelServer->AddPackage(rnsscene);
    kernelServer->AddPackage(zombieentity);
    kernelServer->AddPackage(zombieentityexp);
    kernelServer->AddPackage(ncommonapp);
    kernelServer->AddPackage(conjurerapp);
    kernelServer->AddPackage(conjurerexp);
    kernelServer->AddPackage(renaissanceapp);
    kernelServer->AddPackage(nnetworkenet);
    kernelServer->AddPackage(nnetwork);
    kernelServer->AddPackage(rnsstates);
    kernelServer->AddPackage(nfsm);
    kernelServer->AddPackage(ntoollib);
    kernelServer->AddPackage(gpbasicactions);
    kernelServer->AddPackage(rnsbasicactions);
    kernelServer->AddPackage(ndsound);
    kernelServer->AddPackage(nmusictable);
    kernelServer->AddPackage(rnsnetwork);
    kernelServer->AddPackage(rnsentity);
    kernelServer->AddPackage(ngui);
    kernelServer->AddPackage(ndshow);

    kernelServer->AddPackage(summoner);

    // enable frame time in time server
    kernelServer->GetTimeServer()->EnableFrameTime();

    if (lockFrameTime > 0.0)
    {
        kernelServer->GetTimeServer()->LockDeltaT(lockFrameTime);
    }

    // initialize a viewer app object
    nSummonerApp *app = (nSummonerApp *) kernelServer->New("nsummonerapp", "/app/conjurer");
    app->SetCompanyName("The Zombie Team");
    app->SetAppName("Summoner");
    app->SetDisplayMode(displayMode);
    app->SetInstanceName(newInstance);

    // keep -view2 for legacy
    if (!viewArg && viewArg2)
    {
        viewArg = viewArg2;
    }

    if (viewArg)
    {
        app->SetSceneFile(viewArg);
    }

    if (classArg)
    {
        app->SetLoadClassName(classArg);
    }

    if (levelArg)
    {
        app->SetLevelFile(levelArg);
    }

    if (animArg)
    {
        app->SetAnimState(animArg);
    }

    if (projDir)
    {
        app->SetProjectDirectory(projDir);
    }

    if (wcDir)
    {
        app->SetWorkingCopyDir(wcDir);
    }

    if (wc2Dir)
    {
        app->SetTempWorkingCopyDir(wc2Dir);
        app->SetTemporaryModeEnabled(true);
    }

    app->SetWatch(watch);
    app->SetViewportLayout(layoutArg);
    app->SetStartupScript(startupArg);
    app->SetInitState( stateArg );

    // open and run app
    if (app->Open())
    {       
        app->Run();
    }
    app->Close();
    // the first instance instance gets created automatically when needed

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
