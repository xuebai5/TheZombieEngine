#include "precompiled/pchnapplication.h"
//------------------------------------------------------------------------------
//  napplication_main.cc
//  (C) 2004 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "napplication/napplication.h"
#include "napplication/nappstate.h"

#include "kernel/nfileserver2.h"
#include "kernel/ntimeserver.h"
#include "kernel/nkernelserver.h"
#include "kernel/nscriptserver.h"
#include "input/ninputserver.h"
#include "misc/nconserver.h"
#include "resource/nresourceserver.h"
#include "nscene/nsceneserver.h"
#include "variable/nvariableserver.h"
#include "particle/nparticleserver.h"
#include "particle/nparticleserver2.h"
#include "video/nvideoserver.h"
#include "gui/nguiserver.h"
#include "audio3/nlistener3.h"
//#include "shadow2/nshadowserver2.h"
#include "kernel/nremoteserver.h"
#include "kernel/logclassregistry.h"
#include "misc/nprefserver.h"
#include "gui/nguiwindow.h"
//#include "locale/nlocaleserver.h"
#include "signals/nsignalserver.h"
#include "appinfo/appinfo.h"

nNebulaScriptClass(nApplication, "nroot");

nApplication* nApplication::Singleton = 0;

//------------------------------------------------------------------------------
/**
*/
nApplication::nApplication() :
    isOpen(false),
    quitRequested(false),
    inputHandled(false),
    debugMemoryStats(false),
    companyName("The Zombie Team"),
    appName("Generic"),
    startupScript("proj:data/scripts/startup.tcl"),
    featureSetOverride(nGfxServer2::InvalidFeatureSet),
    stateTransitionTime(0.0),
    time(0.0),
    frameId(0),
    stateTime(0.0),
    frameTime(0.0),
    audioListener(0)
    #if __NEBULA_STATS__
   ,profAppStateOnFrame("profAppStateOnFrame"),
    profAppStateRender3D("profAppStateRender3D"),
    profAppStateRender2D("profAppStateRender2D"),

    profAppDoFrameGui("profAppDoFrameGui"),
    profAppDoFrameInput("profAppDoFrameInput"),
    profAppDoFrameSound("profAppDoFrameSound"),
    profAppDoFrameRender("profAppDoFrameRender")
    #endif
{
    n_assert(Singleton == 0);
    Singleton = this;
    nThread::MarkAsMainThread();
}

//------------------------------------------------------------------------------
/**
*/
nApplication::~nApplication()
{
    n_assert(!this->IsOpen());

    if (this->audioListener)
    {
        n_delete(this->audioListener);
    }

    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
    Open the application. This will initialize the Nebula2 runtime, load
    the startup script, and invoke the script functions OnStartup,
    OnGraphicsStartup, OnGuiStartup and OnMapInput.

    - 07-Jun-05    kims    Removed nGfxServer2::OpenDisplay() since nSceneServer::Open()
                           calls it internally and moved nSceneServer::Open() before
                           nInputServer is created because input server needs to have
                           the display aleady set up.
                           This changes came from Dec.2004 merge of RL.
*/
bool
nApplication::Open()
{
    n_assert(!this->isOpen);

    kernelServer->PushCwd(this);
    this->refAppStates = kernelServer->New("nroot", "appstates");
    kernelServer->PopCwd();

    this->quitRequested = false;

    // create default dependency server
    kernelServer->New("ndependencyserver", "/sys/servers/dependency/basic");

    this->refProfilerManager = this->CreateProfilerManager();

    // create preferences server
    this->refPrefServer = this->CreatePrefServer();

    // initialize preferences server
    this->refPrefServer->SetCompanyName(this->GetCompanyName());
    this->refPrefServer->SetApplicationName(this->GetAppName());
    this->refPrefServer->Open();

    // initialize Nebula2 runtime
    this->refScriptServer    = this->CreateScriptServer();
    this->refScriptServer->SetFailOnError(false);

    this->refGfxServer       = this->CreateGfxServer();
    this->refConServer       = this->CreateConsoleServer();
    this->refResourceServer  = this->CreateResourceServer();
    this->refSceneServer     = this->CreateSceneServer();
    this->refVariableServer  = this->CreateVariableServer();
    this->refParticleServer  = this->CreateParticleServer();
    this->refParticleServer2 = (nParticleServer2*) kernelServer->New("nparticleserver2", "/sys/servers/particle2");
    this->refVideoServer     = this->CreateVideoServer();
    this->refGuiServer       = this->CreateGuiServer();
    //this->refShadowServer    = this->CreateShadowServer();
    this->refAudioServer     = this->CreateAudioServer();
    this->audioListener      = this->CreateAudioListener();
    //this->refLocaleServer    = this->CreateLocaleServer();
    this->refInputServer     = this->CreateInputServer();
    this->refSignalServer    = this->CreateSignalServer();

    // create viewports object
    kernelServer->PushCwd(this);
    this->refAppViewports = kernelServer->New("nroot", "appviewports");
    kernelServer->PopCwd();

    // initialize the proj: assign
    nFileServer2* fileServer = nFileServer2::Instance();
    if (!this->projDir.IsEmpty())
    {
        fileServer->SetAssign("proj", this->GetProjectDirectory().Get());
    }
    else
    {
        fileServer->SetAssign("proj", kernelServer->GetFileServer()->GetAssign("home"));
    }

    // run startup script (assigns must be setup before opening the display!)
    nString scriptResult;
    this->refScriptServer->RunScript(this->startupScript.Get(), scriptResult);
    this->refScriptServer->RunFunction("OnStartup", scriptResult);

    // initialize locale server
    //if (!this->localeTable.IsEmpty())
    //{
    //    this->refLocaleServer->SetLocaleTableFilename(this->localeTable.Get());
    //    this->refLocaleServer->Open();
    //}

    // make sure the read/write appdata directories exists
    //fileServer->MakePath("appdata:");
    //fileServer->MakePath("save:");

    // initialize graphics
    // set the gfx server feature set override
    if (this->featureSetOverride != nGfxServer2::InvalidFeatureSet)
    {
        this->refGfxServer->SetFeatureSetOverride(this->featureSetOverride);
    }
    this->refGfxServer->SetDisplayMode(this->displayMode);
    this->refGfxServer->SetCamera(this->gfxCamera);
    this->refScriptServer->RunFunction("OnGraphicsStartup", scriptResult);

    // open the scene server.
    // this also opens the display, which must be done before the input server is created
    this->refSceneServer->SetBgColor(vector4(0.0f, 0.0f, 0.0f, 1.0f));
    if (!this->refSceneServer->Open())
    {
        this->Close();
        return false;
    }

    // open the video server
    if (this->refVideoServer.isvalid())
    {
        this->refVideoServer->Open();
    }

    // late initialization of input server, because it relies on
    // refGfxServer->OpenDisplay having been called
    this->refInputServer->Open();
    this->refScriptServer->RunFunction("OnMapInput", scriptResult);

    // initialize audio
    if (!this->refAudioServer->Open())
    {
        //this->Close();
        //return false;
    }

    // initialize gui
    this->refGuiServer->SetRootPath("/gui");
    this->refGuiServer->SetDisplaySize(vector2(float(this->displayMode.GetWidth()), float(this->displayMode.GetHeight())));
    this->refGuiServer->Open();

    // create a user root window
    nGuiWindow* userRootWindow = this->refGuiServer->GetRootWindowPointer();
    if ( !userRootWindow )
    {
        this->refGuiServer->SetRootWindowPointer(0);
        userRootWindow = this->refGuiServer->NewWindow("nguiwindow", true);
        this->refGuiServer->SetRootWindowPointer(userRootWindow);
        userRootWindow->Show();
    }

    // Show nebula ngui
    this->refGuiServer->ToggleSystemGui();

    #if __NEBULA_STATS__
    this->profAppStateOnFrame.Initialize("profAppStateOnFrame");
    this->profAppStateRender2D.Initialize("profAppStateRender2D");
    this->profAppStateRender3D.Initialize("profAppStateRender3D");
    #endif

    this->isOpen = true;

    return true;
}

//------------------------------------------------------------------------------
/**
    Close the application.

    - 07-Jun-05    kims    Removed calling of CloseDisplay() func of gfx server.
                           The display is closed when scene server is closed.
                           This change came from Dec.2004 merge of RL.
*/
void
nApplication::Close()
{
    if (this->refAppStates.isvalid())
    {
        this->refAppStates->Release();
    }

    if (this->refGuiServer.isvalid())
    {
        this->refGuiServer->Close();
    }

    if (this->refSceneServer.isvalid())
    {
        this->refSceneServer->Close();
    }

    if (this->refAudioServer.isvalid())
    {
       this->refAudioServer->Close();
    }

    if (this->refVideoServer.isvalid())
    {
        this->refVideoServer->Close();
    }
    //if (this->refLocaleServer.isvalid())
    //{
    //    if (this->refLocaleServer->IsOpen())
    //    {
    //        this->refLocaleServer->Close();
    //    }
    //    this->refLocaleServer->Release();
    //}

    if (this->refAudioServer.isvalid())
    {
        this->refAudioServer->Release();
    }

    //this->refShadowServer->Release();

    if (this->refGuiServer.isvalid())
    {
        this->refGuiServer->Release();
    }
    
    if (this->refVideoServer.isvalid())
    {
        this->refVideoServer->Release();
    }

    if (this->refParticleServer2.isvalid())
    {
        this->refParticleServer2->Release();
    }

    if (this->refParticleServer.isvalid())
    {
        this->refParticleServer->Release();
    }
    
    if (this->refVariableServer.isvalid())
    {
        this->refVariableServer->Release();
    }
    
    if (this->refSceneServer.isvalid())
    {
        this->refSceneServer->Release();
    }
    
    if (this->refConServer.isvalid())
    {
        this->refConServer->Release();
    }
    
    if (this->refInputServer.isvalid())
    {
        this->refInputServer->Release();
    }
    
    if (this->refResourceServer.isvalid())
    {
        this->refResourceServer->Release();
    }
    
    if (this->refGfxServer.isvalid())
    {
        this->refGfxServer->Release();
    }
    
    if (this->refSignalServer.isvalid())
    {
        this->refSignalServer->Release();
    }
    
    if (this->refScriptServer.isvalid())
    {
        this->refScriptServer->Release();
    }
    
    if (this->refPrefServer.isvalid())
    {
        this->refPrefServer->Close();
        this->refPrefServer->Release();
    }

    if (this->refProfilerManager.isvalid())
    {
        this->refProfilerManager->Release();
    }

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
    Run the application, this method will return when the application
    should quit.
*/
void
nApplication::Run()
{
    while (!this->GetQuitRequested())
    {
        // update time values
        this->OnUpdateTime();
        ++this->frameId;

        // perform state transition
        if ((!this->curState.isvalid()) ||
            (this->nextState.isvalid() && (this->nextState.get() != this->curState.get())))
        {
            this->DoStateTransition();
        }

        // perform current frame
        this->DoFrame();

        // give up time slice
        n_sleep(0.0f);
    }
}

//------------------------------------------------------------------------------
/**
    Update time values.
*/
void
nApplication::OnUpdateTime()
{
    nTimeServer* timeServer = nTimeServer::Instance();
    timeServer->Trigger();
    nTime curTime = timeServer->GetFrameTime();
    nTime diff    = curTime - this->time;

    // catch time exceptions
    if (diff <= 0.0)
    {
        diff = 0.0001f;
        curTime = this->time = diff;
    }
    this->frameTime = diff;
    this->time = curTime;
    this->stateTime = this->time - this->stateTransitionTime;
    this->fps = 1.0f / this->frameTime;
}

//------------------------------------------------------------------------------
/**
    Perform a state transition.
*/
void
nApplication::DoStateTransition()
{
    n_assert(this->nextState.isvalid());

    if (this->curState.isvalid())
    {
        n_assert(this->curState.get() != this->nextState.get());
        this->curState->OnStateLeave(this->nextState->GetName());
        this->nextState->OnStateEnter(this->curState->GetName());
    }
    else
    {
        this->nextState->OnStateEnter("");
    }
    this->curState = this->nextState;
    this->stateTransitionTime = this->time;
}

//------------------------------------------------------------------------------
/**
    Set a new application state. The state will be activated during the
    next DoStateTransition().
*/
void
nApplication::SetState(const nString& stateName)
{
    n_assert(!stateName.IsEmpty());
    nAppState* appState = this->FindState(stateName);
    if (0 != appState)
    {
        if (this->curState.isvalid())
        {
            if (appState != this->curState)
            {
                this->nextState = appState;
            }
        }
        else
        {
            this->nextState = appState;
        }
    }
    else
    {
        n_printf("nApplication::SetState(): invalid app state '%s'!\n", stateName.Get());
    }
}

//------------------------------------------------------------------------------
/**
    Returns the currently active application state.
*/
nString
nApplication::GetCurrentState() const
{
    nString stateName;
    if (this->curState.isvalid())
    {
        stateName = this->curState->GetName();
    }
    return stateName;
}

//------------------------------------------------------------------------------
/**
    Find an application state by name.
*/
nAppState*
nApplication::FindState(const nString& stateName) 
{
    if (!this->refAppStates.isvalid())
    {
        return 0;
    }

    nAppState * state = static_cast<nAppState*>( this->refAppStates->Find( stateName.Get() ) );
    if( ! state )
    {
        int index = this->stateNames.FindIndex( stateName );
        if( index != -1 )
        {
            kernelServer->PushCwd(this->refAppStates);
            nString className = this->stateClasses[index];
            state = static_cast<nAppState*>( kernelServer->New(className.Get(), stateName.Get()) );
            kernelServer->PopCwd();
            state->OnCreate(this);

            this->stateNames.Erase( index );
            this->stateClasses.Erase( index );
        }
    }
    return state;
}

//------------------------------------------------------------------------------
/**
    Create a new application state object of the given class and name.
    This method do not instance the state object. The state is instanced when
    needed in the FindState method or in the SetState method (that use FindState
    internally)
*/
void
nApplication::CreateState(const nString& className, const nString& stateName)
{
    n_assert(!className.IsEmpty());
    n_assert(!stateName.IsEmpty());
    n_assert(!this->refAppStates->Find(stateName.Get()));
    n_assert( this->stateNames.FindIndex( stateName ) == -1 );

    // save class and name of state to create later
    this->stateNames.Append( stateName );
    this->stateClasses.Append( className );
}

//------------------------------------------------------------------------------
/**
    Create a new application viewport of the given class and name.
*/
nAppViewport*
nApplication::CreateViewport(const nString& className, const nString& viewportName)
{
    n_assert(!className.IsEmpty());
    n_assert(!viewportName.IsEmpty());
    n_assert(!this->refAppViewports->Find(viewportName.Get()));

    kernelServer->PushCwd(this->refAppViewports);
    nAppViewport* newViewport = (nAppViewport*) kernelServer->New(className.Get(), viewportName.Get());
    kernelServer->PopCwd();

    return newViewport;
}

//------------------------------------------------------------------------------
/**
    Do one complete frame.
*/
void
nApplication::DoFrame()
{
    n_assert(this->curState.isvalid());

    #if __NEBULA_STATS__
    // start profiling
    this->refProfilerManager->BeginProfiling();

    this->profAppDoFrameInput.ResetAccum();
    this->profAppDoFrameGui.ResetAccum();
    this->profAppDoFrameSound.ResetAccum();
    this->profAppDoFrameRender.ResetAccum();
    #endif

    // distribute time to subsystems
    nGuiServer::Instance()->SetTime(this->time);

    // trigger signal server
    this->refSignalServer->Trigger(this->time);

    // trigger the various servers
    if (!this->refScriptServer->Trigger())
    {
        this->SetQuitRequested(true);
    }

    if (!this->refGfxServer->Trigger())
    {
        this->SetQuitRequested(true);
    }

    if (this->GetDebugMemoryStats())
    {
        kernelServer->Trigger();
    }

    #if __NEBULA_STATS__
    this->profAppDoFrameInput.StartAccum();
    #endif
    nInputServer::Instance()->Trigger(this->time);
    #if __NEBULA_STATS__
    this->profAppDoFrameInput.StopAccum();
    #endif

    #if __NEBULA_STATS__
    this->profAppDoFrameGui.StartAccum();
    #endif
    nGuiServer::Instance()->Trigger();
    #if __NEBULA_STATS__
    this->profAppDoFrameGui.StopAccum();
    #endif

    // trigger video players
    if (this->refVideoServer.isvalid() && this->refVideoServer->IsOpen())
    {
        this->refVideoServer->Trigger();
    }

    // perform application's own input handling
    this->SetInputHandled(false);

    if (this->HandleInput())
    {
        this->SetInputHandled(true);
    }

    // perform logic triggering
    #if __NEBULA_STATS__
    this->profAppStateOnFrame.ResetAccum();
    this->profAppStateOnFrame.StartAccum();
    #endif
    this->curState->OnFrame();
    #if __NEBULA_STATS__
    this->profAppStateOnFrame.StopAccum();
    this->profAppStateOnFrame.Publish();
    #endif

    // perform rendering
    nAudioServer3* audioServer = nAudioServer3::Instance();
    nGfxServer2* gfxServer = nGfxServer2::Instance();
    nSceneServer* sceneServer = nSceneServer::Instance();

    #if __NEBULA_STATS__
    this->profAppDoFrameSound.StartAccum();
    #endif
    audioServer->BeginScene(this->time);

    n_assert( this->audioListener );
    this->refAudioServer->UpdateListener( *this->audioListener );

    this->OnRenderSound();
    #if __NEBULA_STATS__
    this->profAppDoFrameSound.StopAccum();
    #endif

    if (!gfxServer->InDialogBoxMode())
    {
        #if __NEBULA_STATS__
        this->profAppDoFrameRender.StartAccum();
        #endif
        this->OnFrameBefore();
        if (sceneServer->BeginScene())
        {
            nParticleServer::Instance()->Trigger();
            nParticleServer2::Instance()->Trigger();
            this->OnRender3D();
            sceneServer->EndScene();
            sceneServer->RenderScene();
            this->OnFrameRendered();
            this->OnRender2D();
            nConServer::Instance()->Render();
            nGuiServer::Instance()->Render();
            sceneServer->PresentScene();
        }
        #if __NEBULA_STATS__
        this->profAppDoFrameRender.StopAccum();
        #endif
    }

    audioServer->EndScene();

    nInputServer::Instance()->FlushEvents();

    // this must be done at the end, 
    // in order to publish all values and reset accumulators
    #if __NEBULA_STATS__
    this->refProfilerManager->EndProfiling();

    this->profAppDoFrameInput.Publish();
    this->profAppDoFrameGui.Publish();
    this->profAppDoFrameSound.Publish();
    this->profAppDoFrameRender.Publish();
    #endif
}

//------------------------------------------------------------------------------
/**
    Perform sound rendering.
*/
void
nApplication::OnRenderSound()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Perform 3d rendering. This calls the OnRender3d() method of the current
    app state object. Usually, all that needs to be done is to attach
    Nebula2 scene graph nodes to the scene server.
*/
void
nApplication::OnRender3D()
{
    n_assert(this->curState.isvalid());
    #if __NEBULA_STATS__
    this->profAppStateRender3D.ResetAccum();
    this->profAppStateRender3D.StartAccum();
    #endif
    this->curState->OnRender3D();
    #if __NEBULA_STATS__
    this->profAppStateRender3D.StopAccum();
    this->profAppStateRender3D.Publish();
    #endif
}

//------------------------------------------------------------------------------
/**
    Perform 2d rendering. This calls the OnRender2d() method of the current
    app state object.
*/
void
nApplication::OnRender2D()
{
    n_assert(this->curState.isvalid());
    #if __NEBULA_STATS__
    this->profAppStateRender2D.ResetAccum();
    this->profAppStateRender2D.StartAccum();
    #endif
    this->curState->OnRender2D();
    #if __NEBULA_STATS__
    this->profAppStateRender2D.StopAccum();
    this->profAppStateRender2D.Publish();
    #endif
}

//------------------------------------------------------------------------------
/**
*/
bool
nApplication::HandleInput()
{
    return false;
}

//------------------------------------------------------------------------------
/**
*/
nScriptServer*
nApplication::CreateScriptServer()
{
    return (nScriptServer*) kernelServer->New("nluaserver", "/sys/servers/script");
}

//------------------------------------------------------------------------------
/**
*/
nGfxServer2*
nApplication::CreateGfxServer()
{
    return (nGfxServer2*) kernelServer->New("nd3d9server", "/sys/servers/gfx");
}

//------------------------------------------------------------------------------
/**
*/
nInputServer*
nApplication::CreateInputServer()
{
    return (nInputServer*) kernelServer->New("ndi8server", "/sys/servers/input");
}

//------------------------------------------------------------------------------
/**
*/
nConServer*
nApplication::CreateConsoleServer()
{
    return (nConServer*) kernelServer->New("nconserver", "/sys/servers/console");
}

//------------------------------------------------------------------------------
/**
*/
nResourceServer*
nApplication::CreateResourceServer()
{
    return (nResourceServer*)  kernelServer->New("nresourceserver", "/sys/servers/resource");
}

//------------------------------------------------------------------------------
/**
*/
nSceneServer*
nApplication::CreateSceneServer()
{
    return (nSceneServer*) kernelServer->New("nstdsceneserver", "/sys/servers/scene");
}

//------------------------------------------------------------------------------
/**
*/
nVariableServer*
nApplication::CreateVariableServer()
{
    return (nVariableServer*)  kernelServer->New("nvariableserver", "/sys/servers/variable");
}

//------------------------------------------------------------------------------
/**
*/
nParticleServer*
nApplication::CreateParticleServer()
{
    return (nParticleServer*) kernelServer->New("nparticleserver", "/sys/servers/particle");
}

//------------------------------------------------------------------------------
/**
*/
nVideoServer*
nApplication::CreateVideoServer()
{
    return (nVideoServer*) kernelServer->New("ndshowserver", "/sys/servers/video");
}

//------------------------------------------------------------------------------
/**
*/
nGuiServer*
nApplication::CreateGuiServer()
{
    return (nGuiServer*) kernelServer->New("nguiserver", "/sys/servers/gui");
}

//------------------------------------------------------------------------------
/**
*/
//nShadowServer2*
//nApplication::CreateShadowServer()
//{
//    return (nShadowServer2*) kernelServer->New("nshadowserver", "/sys/servers/shadow");
//}

//------------------------------------------------------------------------------
/** 
*/
nAudioServer3*
nApplication::CreateAudioServer()
{
    return (nAudioServer3*) kernelServer->New("ndsoundserver3", "/sys/servers/audio");
    //return (nAudioServer3*) kernelServer->New("nopenalserver", "/sys/servers/audio");
}

//------------------------------------------------------------------------------
/**
*/
nListener3*
nApplication::CreateAudioListener()
{
    return n_new( nListener3 );
    //@todo: OpenAl has its own Listener3 class, but dsound implementation does not
}

//------------------------------------------------------------------------------
/**
*/
nPrefServer*
nApplication::CreatePrefServer()
{
#if defined(WIN32)
    return (nPrefServer*) kernelServer->New("nwin32prefserver", "/sys/servers/nwin32prefserver");
#else
    return (nPrefServer*) kernelServer->New("nprefserver", "/sys/servers/nwin32prefserver");
#endif
}

//------------------------------------------------------------------------------
nProfilerManager *
nApplication::CreateProfilerManager()
{
    return (nProfilerManager*) kernelServer->New("nprofilermanager", "/sys/servers/profilemanager");
}

//------------------------------------------------------------------------------
/**
*/
nSignalServer*
nApplication::CreateSignalServer()
{
    return (nSignalServer*) kernelServer->New("nsignalserver", "/sys/servers/signal");
}

//------------------------------------------------------------------------------
/**
*/
void
nApplication::OnFrameBefore()
{
    n_assert(this->curState.isvalid());
    this->curState->OnFrameBefore();
}

//------------------------------------------------------------------------------
/**
*/
void
nApplication::OnFrameRendered()
{
    n_assert(this->curState.isvalid());
    this->curState->OnFrameRendered();
}

//------------------------------------------------------------------------------
/**
    Get release build info (those found in appinfo.h)
*/
void
nApplication::GetReleaseBuildInfo( int & version, int & major, int & minor, int & patch, char * versionstr, char * date, char * time, char * id ) const
{
    version = N_RELEASE_NUMBER;
    major = atoi(N_RELEASE_MAJOR);
    minor = atoi(N_RELEASE_MINOR);
    patch = atoi(N_RELEASE_PATCH);
    strcpy(versionstr, N_RELEASE_NUMBER_STR);
    strcpy(date, N_RELEASE_BUILD_DATE);
    strcpy(time, N_RELEASE_BUILD_TIME);
    strcpy(id, N_RELEASE_BUILD_ID);
}

//------------------------------------------------------------------------------
/**
    Get release subversion info (those found in appinfo.h)
*/
void
nApplication::GetReleaseSubversionInfo( int & revision, char * revstr, char* range, char* url, char* date, bool & mixed, bool & localmod ) const
{
    revision = N_RELEASE_SVN_REVISION;
    strcpy(range, N_RELEASE_SVN_RANGE);
    strcpy(url, N_RELEASE_SVN_HEADURL);
    strcpy(date, N_RELEASE_SVN_DATE);
    mixed = N_RELEASE_SVN_MIXED;
    localmod = N_RELEASE_SVN_LOCALMOD;
    strcpy(revstr, N_RELEASE_SVN_REVISION_STR);
}
