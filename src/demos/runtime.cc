/**
    Zombie Runtime
    Initialize minimal nebula runtime to run application(s)

*/
//------------------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "demos/demoapp.h"
#include "snakes/snakesapp.h"
#include "cameras/camerasapp.h"
#include "transform/transformapp.h"
#include "shaders/shadersapp.h"

#ifdef __WIN32__
#include "kernel/nwin32loghandler.h"
#include "tools/nwinmaincmdlineargs.h"
#else
#include "tools/ncmdlineargs.h"
#endif

#include "gfx2/ndisplaymode2.h"
#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"
#include "kernel/nscriptserver.h"
#include "kernel/nfileserver2.h"
#include "input/ninputserver.h"
#include "misc/nconserver.h"
#include "resource/nresourceserver.h"
#include "variable/nvariableserver.h"
#include "misc/nprefserver.h"

nNebulaUsePackage(nnebula);
nNebulaUsePackage(ndinput8);
nNebulaUsePackage(ndirect3d9);
nNebulaUsePackage(nlua);

//------------------------------------------------------------------------------
/**
*/
#ifdef __WIN32__
int WINAPI
WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*prevInstance*/, LPSTR lpCmdLine, int /*nCmdShow*/)
{
    nWinMainCmdLineArgs args(lpCmdLine);
#else
int
main(int argc, const char** argv)
{
    nCmdLineArgs args(argc, argv);
#endif

    //TODO- store rest of command line arguments as environment variables?
	const char* scriptserverArg = args.GetStringArg("-scriptserver", "nluaserver");
	const char* projDirArg = args.GetStringArg("-proj", "home:data/demos");
	const char* startupArg = args.GetStringArg("-startup", "proj:scripts/startup.lua");
    const char* appArg = args.GetStringArg("-app", "shaders");

    bool fullscreenArg	= args.GetBoolArg("-fullscreen");
    bool alwaysOnTopArg = args.GetBoolArg("-alwaysontop");
    ushort xPosArg      = (ushort) args.GetIntArg("-x", 0);
    ushort yPosArg      = (ushort) args.GetIntArg("-y", 0);
    ushort widthArg     = (ushort) args.GetIntArg("-w", 640);
    ushort heightArg    = (ushort) args.GetIntArg("-h", 480);

    // initialize Nebula runtime
    nKernelServer* kernel = n_new( nKernelServer );
    #ifdef __WIN32__
        nWin32LogHandler logHandler("demoapp");
        kernel->SetLogHandler(&logHandler);
    #endif
    kernel->AddPackage(nnebula);
    kernel->AddPackage(ndinput8);
    kernel->AddPackage(ndirect3d9);
    kernel->AddPackage(nlua);

    // initialize a display mode object
    nString title("Zombie Demo");
    nDisplayMode2 displayMode;
    displayMode.SetIcon("NebulaIcon");
    displayMode.SetDialogBoxMode(true);
    if (fullscreenArg)
    {
        displayMode.Set(title.Get(), nDisplayMode2::Fullscreen, xPosArg, yPosArg, widthArg, heightArg, false, true, "Icon");
    }
    else if (alwaysOnTopArg)
    {
        displayMode.Set(title.Get(), nDisplayMode2::AlwaysOnTop, xPosArg, yPosArg, widthArg, heightArg, false, true, "Icon");
    }
    else
    {
        displayMode.Set(title.Get(), nDisplayMode2::Windowed, xPosArg, yPosArg, widthArg, heightArg, false, true, "Icon");
    }

    //initialize systems
    nScriptServer* pScriptServer = (nScriptServer*) kernel->New( scriptserverArg, "/sys/servers/script" );
    nGfxServer2* pGfxServer	= (nGfxServer2*) kernel->New("nd3d9server", "/sys/servers/gfx");
    nInputServer* pInputServer = (nInputServer*) kernel->New("ndi8server", "/sys/servers/input");
    nConServer* pConServer = (nConServer*) kernel->New("nconserver", "/sys/servers/console");
    nResourceServer* pResourceServer = (nResourceServer*) kernel->New("nresourceserver", "/sys/servers/resource");
    nVariableServer* pVarServer = (nVariableServer*) kernel->New("nvariableserver", "/sys/servers/variable");

	//set project directory
	kernel->GetFileServer()->SetAssign( "proj", projDirArg ? projDirArg : kernel->GetFileServer()->GetAssign( "home" ) );

	//run startup scripts
	if ( startupArg )
	{
		nString scriptResult;
		pScriptServer->RunScript(startupArg, scriptResult);
		pScriptServer->RunFunction("OnStartup", scriptResult);
		pScriptServer->RunFunction("OnGraphicsStartup", scriptResult);
	}

    DemoApp* app(0);
    if (!strcmp( appArg, "snakes" ))
        app = n_new(SnakesApp);
    else if (!strcmp( appArg, "cameras" ))
        app = n_new(CamerasApp);
    else if (!strcmp( appArg, "transform" ))
        app = n_new(TransformApp);
    else if (!strcmp( appArg, "shaders" ))
        app = n_new(ShadersApp);
    else
        app = n_new(DemoApp);

    pGfxServer->SetDisplayMode(displayMode);
    nCamera2 camera(n_deg2rad(60), float(displayMode.GetHeight())/float(displayMode.GetWidth()),1.f,100.f);
    pGfxServer->SetCamera(camera);
	if ( !pGfxServer->OpenDisplay() ) goto Exit;

    // define the input mapping
    pInputServer->Open();
    if ( startupArg )
    {
        nString scriptResult;
        pScriptServer->RunFunction("OnMapInput", scriptResult);
    }

	//TODO- run startup script, use for initialization of structures, loading data, etc.
    //maybe even for initializing other modules?

    //call game loop
	app->Init();

    if ( app->Open() )
    {
        // run the render loop
        bool running = true;
        float frameTime;
        nTime prevTime = 0.0;
        //uint frameId = 0;
        while ( pGfxServer->Trigger() && running )
        {
            nTimeServer::Instance()->Trigger();
            nTime time = nTimeServer::Instance()->GetTime();
            if (prevTime == 0.0)
            {
                prevTime = time;
            }
			
            frameTime = (float) (time - prevTime);

            //TODO- tick the script server
            //TODO- tick other systems, if necessary
            
            pInputServer->Trigger(time);
            
            //TODO- toggle console

            app->Tick( frameTime );

            if ( pGfxServer->BeginFrame() )
            {
                if ( pGfxServer->BeginScene() )
                {
                    app->Render();
                    pGfxServer->DrawTextBuffer();
                    pGfxServer->EndScene();
                    pGfxServer->PresentScene();
                }
                pGfxServer->EndFrame();
            }

            prevTime = time;

            pInputServer->FlushEvents();

            n_sleep(0.0);
        }

        app->Close();
    }

    //shutdown systems
	pGfxServer->CloseDisplay();

    n_delete(app);

Exit:
	//
	//if ( pScriptServer ) pScriptServer->Release();
	if ( pInputServer ) pInputServer->Release();
	if ( pConServer ) pConServer->Release();
	if ( pVarServer ) pVarServer->Release();
	if ( pResourceServer ) pResourceServer->Release();
	if ( pGfxServer ) pGfxServer->Release();

	n_delete( kernel );
}
