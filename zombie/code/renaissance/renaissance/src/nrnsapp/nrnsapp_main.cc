#include "precompiled/pchrenaissanceapp.h"
//------------------------------------------------------------------------------
//  nrnsapp_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nrnsapp/nrnsapp.h"
#include "rnsstates/rnsgamestate.h"
#include "rnsstates/rnsloadstate.h"
#include "rnsstates/rnsmenustate.h"

#include "napplication/nappviewport.h"
#include "napplication/nappstate.h"
#include "appinfo/appinfo.h"
#include "kernel/nfileserver2.h"
#include "kernel/nscriptserver.h"
#include "kernel/nremoteserver.h"
#include "kernel/nkernelserverproxy.h"
#include "misc/nconserver.h"
#include "gfx2/ngfxutils.h"
#include "input/ninputserver.h"
#include "nscene/nscenenode.h"
#include "nscene/ntransformnode.h"
#include "nlevel/nlevelmanager.h"
#include "nmaterial/nmaterialserver.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphysicsworld.h"
#include "nspatial/nspatialserver.h"
#include "entity/nentityclassserver.h"
#include "nsoundscheduler/nsoundscheduler.h"
#include "nmusictable/nmusictable.h"
#include "nscriptclassserver/nscriptclassserver.h"
#include "zombieentity/nloaderserver.h"

nNebulaScriptClass(nRnsApp, "ncommonapp");

//------------------------------------------------------------------------------
/**
*/
nRnsApp::nRnsApp() :
    screenshotID(0),
    frameId(0),
    captureFrame(false),
    showFPS(false),
    refScriptServer("/sys/servers/script"),
    outguiDummyPath("/editor/outguiisopened"),
    quitConfirmationPending(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nRnsApp::~nRnsApp()
{
   // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nRnsApp::Open()
{
    // before assigning the working copy, some things could be inferred from
    // the -view parameter if it is a .n2 file (a level or a class)
    //this->MangleSceneFileParameter();
    if (!this->InitWorkingCopyAssign())
    {
        n_message("No valid working copy path available!\n"
                  "(use the -wc argument, or CONJURER_WC environment var)\n"
                  "Conjurer will exit now.");
        return false;
    }

    this->kernelServer->GetFileServer()->SetAssign("outgui", "home:code/conjurer/conjurer/gui/");
        
    this->refLevelManager       = (nLevelManager*)  kernelServer->New("nlevelmanager", "/sys/servers/levelmanager");

    kernelServer->New("nkernelserverproxy", "/sys/servers/kernelserver");

    nCommonApp::Open();

    /*
    if (this->GetWatch())
    {
        nConServer::Instance()->Watch(this->GetWatch());
    }
    */

    nSceneServer::Instance()->SetBgColor(nGfxUtils::color24(140, 138, 156));

    // create scene graph root node
    this->refSceneNode = (nTransformNode *) kernelServer->New("ntransformnode", "/usr/scene");

    // create root node for materials 
    this->refMaterial = kernelServer->New("nroot", "/usr/material");

    nString result;
    this->refScriptServer->RunFunction("OnMaterialStartup", result);

    // load music table
    if (nFileServer2::Instance()->FileExists(musicTablePath))
    {
        this->refMusicTable = static_cast<nMusicTable*>( nKernelServer::Instance()->LoadAs( musicTablePath, musicTableNOHPath, true) );
    }

    // assign entity paths for current level (or default if none)
    n_assert(this->GetLevelFile());
    nString levelPath(this->GetLevelFile());
    levelPath.StripExtension();

    // initialize entity class directory
    nEntityClassServer *entityServer = nEntityClassServer::Instance();
    entityServer->SetEntityClassDir("wc:classes/");

    // init sound scheduler
    this->refSoundScheduler->Init();

    this->refRnsViewport = static_cast<nAppViewport*>(kernelServer->LoadAs("home:data/appdata/conjurer/viewport/rnsview.n2", "/usr/rnsview"));
    n_assert(this->refRnsViewport.isvalid());
    n_assert(this->refRnsViewport->IsA("nappviewport"));

    if (!this->refLevelManager->LoadLevel(this->GetLevelFile()))
    {
        n_message("Couldn't load level file: '%s'\n"
                  "Renaissance will exit now.", this->GetLevelFile());
        return false;
    }
    else
    {
        // set window title to show the current level
        nString title;
        title.Append( "Renaissance" );
        if (this->GetInstanceName())
        {
            title.Append( " " );
            title.Append( this->GetInstanceName() );
        }
        title.Append(" - Level ");
        title.Append( nString( this->GetLevelFile() ).ExtractFileName() );
        title.Append( " - " N_RELEASE_SVN_REVISION_STR );
        this->SetWindowTitle( title.Get() );
    }

    // set load states for this level
    if (this->refLoaderServer->AreasAreLoaded())
    {
        // schedule global entities for loading
        nArray<nEntityObject*> globals;
        nSpatialServer::Instance()->GetCommonGlobalEntities(globals);
        for (int index = 0; index < globals.Size(); ++index)
        {
            nLoaderServer::Instance()->EntityNeedsLoading(globals[index]);
        }

        // disable auto-loading behavior
        this->refLoaderServer->SetLoadSpaces(false);
        this->refLoaderServer->SetLoadBatches(false);
        this->refLoaderServer->SetLoadResources(false);
    }
    else
    {
        this->refLoaderServer->SetLoadAreasEnabled(false);
    }

    // load viewport for renaissance states
    this->refRnsViewport->Open();

    // load camera settings
    this->refRnsViewport->LoadState("home:data/appdata/renaissance/viewport/rnsview_stdcam.n2");

    // create application states
    this->CreateState( "rnsloadstate", "load" );
    this->CreateState( "rnsgamestate", "game" );
    this->CreateState( "rnsmenustate", "menu" );

    // if state need to be created in the begin, use FindState to force creation
    RnsGameState *rnsState = static_cast<RnsGameState*>(this->FindState("game"));
    refRnsViewport->SetViewerPos( vector3(93,12,60) );
    rnsState->SetPreviousViewport( refRnsViewport.get() );
    rnsState->SetNextState( "" );

    // check that initial state was created
    //n_assert2( this->FindState( this->initState ), "No exist initial State in conjurer" );
    RnsLoadState * loadstate = static_cast<RnsLoadState*>( this->FindState("load") );
    n_assert2( loadstate, "Failed to found \"load\" state" );
    loadstate->SetNextState( "game" );

    RnsMenuState *menuState = static_cast<RnsMenuState*>(this->FindState("menu"));
    menuState->SetExitState( "" );

    //this->SetState(this->initState);
    this->SetState( "menu" );

    // perform additional custom initialization -keep always at the end
    this->refScriptServer->RunFunction("OnConjurerStartup", result);

    /// @todo ma.garcias- this only works after setting up everything else
    // add script commands to classes
    this->refScriptClassServer->AddClasses();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsApp::Close()
{
    // leave current state
    nAppState* currentState = this->FindState( this->GetCurrentState() );
    if ( currentState )
    {
        currentState->OnStateLeave("");
    }

    this->refRnsViewport->Release();

    this->refMaterial->Release();
    this->refSceneNode->Release();

    nKernelServerProxy::Instance()->Release();

    nCommonApp::Close();
}

//------------------------------------------------------------------------------
/**
    Do one complete frame.
*/
void
nRnsApp::DoFrame()
{
/*    nSignalServer * signalServer = nSignalServer::Instance();

    signalServer->Trigger( this->GetTime());*/

    // update FPS
    this->fps.Trigger(this->GetTime());

    // perform state logic
    nCommonApp::DoFrame();

    if (this->captureFrame)
    {
        this->CaptureScreenshot();
    }

    frameId++;

    // sleep for a very little while because we
    // are multitasking friendly
    n_sleep(0.0);
}

//------------------------------------------------------------------------------
/**
    Do one complete frame.
*/
void
nRnsApp::OnRender2D()
{
    if (!this->captureFrame)
    {
        nCommonApp::OnRender2D();
    }

    if (this->showFPS)
    {
        this->fps.Show(nGfxServer2::Instance());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nRnsApp::CaptureScreenshot()
{
    nString filename;
    const char* levelFile = this->GetLevelFile();
    if (levelFile)
    {
        filename = levelFile;
        filename.StripExtension();
    }
    else
    {
        filename = "screenshot";
    }

    nString screenshotFile;
    do
    {
        screenshotFile = filename;
        screenshotFile.AppendInt(this->screenshotID++); 
        screenshotFile.Append(".bmp");
    }
    while (kernelServer->GetFileServer()->FileExists(screenshotFile.Get()));

    if (nGfxServer2::Instance()->SaveScreenshot(screenshotFile.Get()))
    {
        n_printf("Screenshot saved as file '%s'", screenshotFile.Get());
    }

    this->captureFrame = false;
}

//------------------------------------------------------------------------------
/**
    Set the "wc" assign for use from conjurer
*/
bool
nRnsApp::InitWorkingCopyAssign()
{
    nFileServer2 *fileServer = kernelServer->GetFileServer();
    nArray<nString> wcPaths;

    // if temporary working copy assigned, set first for write
    /*
    if (this->GetTempWorkingCopyDir() && this->GetTemporaryModeEnabled())
    {
        wcPaths.Append(nString(this->GetTempWorkingCopyDir()));
    }
    */

    // set primary working copy
    if (this->GetWorkingCopyDir())
    {
        wcPaths.Append(nString(this->GetWorkingCopyDir()));
    }
#ifdef __WIN32__
    else
    {
        char buf[N_MAXPATH];
        char* s = getenv("CONJURER_WC");
        if (s)
        {
            n_strncpy2(buf,s,sizeof(buf));
            nString workingCopyPath(buf);
            workingCopyPath.ConvertBackslashes();
            wcPaths.Append(workingCopyPath);
        }
    }
#endif

    // HACK default primary working copy for developers
    if (wcPaths.Size() == 0 && fileServer->DirectoryExists("home:../conjurer_wc"))
    {
        wcPaths.Append(nString("home:../conjurer_wc"));
    }

    if (wcPaths.Size() > 0)
    {
        fileServer->SetAssignGroup("wc", wcPaths);
        // Working copy and shaders will be monitorized
        wcPaths.Append(nString("shaders:"));
        fileServer->SetAssignGroup("dirwatches", wcPaths);
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Set the window title
*/
void
nRnsApp::SetWindowTitle( const char* title )
{
    nGfxServer2::Instance()->SetWindowTitle( title );
}
