#include "precompiled/pchexplorer.h"
//------------------------------------------------------------------------------
//  nExplorerApp_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nexplorerapp.h"
#include "conjurer/nassetloadstate.h"
#include "conjurer/npreviewviewport.h"
#include "conjurer/nviewerparams.h"
#include "conjurer/neditorviewport.h"
#include "conjurer/nglobalvariableeditor.h"
#include "napplication/nappviewportui.h"
#include "napplication/nappviewport.h"
#include "appinfo/appinfo.h"

#include "kernel/nfileserver2.h"
#include "file/nmemfile.h"
#include "kernel/nscriptserver.h"
#include "kernel/nremoteserver.h"
#include "kernel/nkernelserverproxy.h"
#include "misc/nconserver.h"
#include "gfx2/ngfxutils.h"
#include "input/ninputserver.h"
#include "variable/nvariableserver.h"
#include "particle/nparticleserver.h"
#include "nscene/nscenenode.h"
#include "nscene/ntransformnode.h"
#include "ndebug/ndebugserver.h"
#include "ndebug/ndebugcomponentserver.h"
#include "tools/nmonitorserver.h"
#include "nlevel/nlevelmanager.h"
#include "nlayermanager/nlayermanager.h"
#include "nfsmserver/nfsmserver.h"
#include "ntrigger/ntriggerserver.h"
#include "nworldinterface/nworldinterface.h"
#include "nspecialfx/nfxserver.h"

#include "ngeomipmap/ngeomipmapnode.h"
#include "ngeomipmap/nfloatmap.h"
#include "nmaterial/nmaterialserver.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphysicsworld.h"
#include "nspatial/nspatialserver.h"
#include "nspatial/ncspatial.h"
#include "kernel/ndependencyserver.h"
#include "entity/nentityclassserver.h"
#include "file/nwatcherdirserver.h"
#include "nsoundscheduler/nsoundscheduler.h"
#include "nmusictable/nmusictable.h"
#include "nscriptclassserver/nscriptclassserver.h"

#include "zombieentity/nloaderserver.h"
#include "rnsstates/naitester.h"
#include "nsavemanager/nsavemanager.h"
#include "kernel/nlogclass.h"
#include "kernel/logclassregistry.h"
#include "nphysics/ncphysicsobjclass.h"
#include "nphysics/ncphypickableobj.h"

#include "nnetworkenet/nnetutils.h"
#include "rnsgameplay/ncgameplay.h"

#include "gameplay/nmissionhandler.h"

nNebulaScriptClass(nExplorerApp, "ncommonapp");

//------------------------------------------------------------------------------
static const char sessionFileName[] = "user:editorstate.n2";

//------------------------------------------------------------------------------
/**
*/
nExplorerApp::nExplorerApp() :
    isOverlayEnabled(true),
    tempModeEnabled(false),
    controlMode(FreeCam),
    screenshotID(0),
    animStateIndex(0),
    captureFrame(false),
    runPhysics(false),
    attachEnabled(true),
    renderEnabled(true),
    refScriptServer("/sys/servers/script"),
    renderWindowEmbedded(false),
    outguiDummyPath("/editor/outguiisopened"),
    saveManager(0),
    stateFile(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nExplorerApp::~nExplorerApp()
{
    this->bookmarks.Clear();

    if (this->saveManager)
    {
        this->saveManager->Release();
        this->saveManager = 0;
    }

    if (this->stateFile)
    {
        this->stateFile->Release();
        this->stateFile = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nExplorerApp::Open()
{
    // before assigning the working copy, some things could be inferred from
    // the -view parameter if it is a .n2 file (a level or a class)
    this->MangleSceneFileParameter();

    if (!this->InitWorkingCopyAssign())
    {
        n_message("No valid working copy path available!\n"
                  "(use the -wc argument, or CONJURER_WC environment var)\n"
                  "Conjurer will exit now.");
        return false;
    }
    this->kernelServer->GetFileServer()->SetAssign("outgui", "home:code/conjurer/conjurer/gui/");

    this->refDebugServer        = (nDebugServer*)   kernelServer->New("ndebugserver", "/sys/servers/debug");
    this->refLayerManager       = (nLayerManager*)  kernelServer->New("nlayermanager", "/sys/servers/layermanager");
    this->refWatcherDirServer   = (nWatcherDirServer*) kernelServer->New("nwatcherdirserver", "/sys/servers/dirwatcher");
    this->refDebugComponentServer = (nDebugComponentServer*) kernelServer->New("nconjurerdebugcompserver", "/sys/servers/debugcomp");

    kernelServer->New("nkernelserverproxy", "/sys/servers/kernelserver");

    nCommonApp::Open();

    this->refAITester = (nAITester*) kernelServer->New("naitester", "/sys/servers/aitester");

    // Monitor server uses a reference to gfx server, so this is created after the gfx one
    this->refMonitorServer = (nMonitorServer*) kernelServer->New("nmonitorserver", "/sys/servers/monitorserver");

    if (this->GetWatch())
    {
        nConServer::Instance()->Watch(this->GetWatch());
    }

    nSceneServer::Instance()->SetBgColor(nGfxUtils::color24(140, 138, 156));

    // create scene graph root node
    this->refSceneNode = (nTransformNode *) kernelServer->New("ntransformnode", "/usr/scene");

    // create root node for materials 
    this->refMaterial = kernelServer->New("nroot", "/usr/material");

    nString result;
    this->refScriptServer->RunFunction("OnMaterialStartup", result);

    // initialize viewports (set as x,y,w,h)
    const nDisplayMode2& displayMode = nGfxServer2::Instance()->GetDisplayMode();

    // load music table
    if (nFileServer2::Instance()->FileExists(musicTablePath))
    {
        this->refMusicTable = static_cast<nMusicTable*>( nKernelServer::Instance()->LoadAs( musicTablePath, musicTableNOHPath, true) );
    }

    // assign entity paths for current level (or default if none)
    n_assert(this->GetLevelFile());
    nFileServer2* fileServer = kernelServer->GetFileServer();
    if (!fileServer->FileExists(this->GetLevelFile()))
    {
        nString levelPath;
        levelPath.Format("wc:levels/%s.n2", this->GetLevelFile());
        if (fileServer->FileExists(levelPath))
        {
            this->SetLevelFile(levelPath.Get());
        }
    }

    // initialize entity class directory
    nEntityClassServer *entityServer = nEntityClassServer::Instance();
    entityServer->SetEntityClassDir("wc:classes/");

    // init sound scheduler
    this->refSoundScheduler->Init();

    if (!this->refLevelManager->LoadLevel(this->GetLevelFile()))
    {
        n_message("Couldn't load level file: '%s'\n"
                  "Conjurer will exit now.", this->GetLevelFile());
        return false;
    }
    else
    {
        // set window title to show the current level
        nString title = this->CalcTitleString("Summoner");
        this->SetWindowTitle(title.Get());
    }

    kernelServer->PushCwd(this);

    // load viewport layout from file
    n_assert(this->GetViewportLayout());
    n_assert(kernelServer->GetFileServer()->FileExists(this->GetViewportLayout()));
    this->refViewportUI = static_cast<nAppViewportUI*>(kernelServer->LoadAs(this->GetViewportLayout(), "appviewportui"));;
    n_assert(this->refViewportUI.isvalid());
    n_assert(this->refViewportUI->IsA("nappviewportui"));

    kernelServer->PopCwd();

    // load persisted viewport layout, then open
    // FIXME viewport layout is persisted using absolute screen coordinates (ma.garcias)
    this->refViewportUI->SetClientRect(0, 0, displayMode.GetWidth(), displayMode.GetHeight());
    this->refViewportUI->Open();

    // create debug module for editor component visualizations
    n_verify( nDebugServer::Instance()->CreateDebugModule("neditordebugmodule", "editor") );
    n_verify( nDebugServer::Instance()->CreateDebugModule("nscenedebugmodule", "scene") );

    // create application states
    this->CreateState("nexplorerstate", "editor");
    this->CreateState("nassetloadstate", "loader");

    // if state need to be created in the begin, use FindState to force creation
    this->FindState("editor");
    this->FindState("loader");

    this->refLoaderState = (nAssetLoadState*) this->FindState("loader");
    this->refLoaderState->SetLoadClassName(this->GetLoadClassName());

    // check that initial state was created
    n_assert2( this->FindState( this->initState ), "No exist initial State in conjurer" );

    this->SetState(this->initState);

    // load entity from file
    // @deprecated keep "this->GetSceneFile()" for backwards compatibility
    if (this->GetSceneFile() || this->GetLoadClassName())
    {
        this->refLoaderState->SetNextState(this->initState);
        this->refLoaderState->SetAnimState(this->GetAnimState());
        this->SetState("loader");
    }
    else
    {
        this->SetState(this->initState);
    }

    // reset camera position for all viewports
    this->refViewportUI->Reset();

    // perform additional custom initialization -keep always at the end
    this->refScriptServer->RunFunction("OnConjurerStartup", result);

    /// @todo ma.garcias- this only works after setting up everything else
    this->refScriptClassServer->AddClasses();

    // create global variable editor
    this->refGlobalVarEditor = (nGlobalVariableEditor*) kernelServer->New("nglobalvariableeditor", "/usr/globalvars");
    this->refGlobalVarEditor->LoadGlobalVariables();

    // load level preset
    this->refScriptServer->RunFunction("OnConjurerLoadLevel", result);

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nExplorerApp::Close()
{
    // Close OutGUI first if it's opened
    //this->CloseOutGUI(true);

    // leave current state
    nAppState* currentState = this->FindState( this->GetCurrentState() );
    if ( currentState )
    {
        currentState->OnStateLeave("");
    }

    if (this->refAITester.isvalid())
    {
        this->refAITester->Release();
    }
    if (this->refGlobalVarEditor.isvalid())
    {
        this->refGlobalVarEditor->Release();
    }
    if (this->refViewportUI.isvalid())
    {    
        this->refViewportUI->Close();
        this->refViewportUI->Release();
    }
    if (this->refMaterial.isvalid())
    {
        this->refMaterial->Release();
    }
    if (this->refSceneNode.isvalid())
    {
        this->refSceneNode->Release();
    }
    if (this->refDebugServer.isvalid())
    {
        this->refDebugServer->Release();
    }
    if (this->refMonitorServer.isvalid())
    {
        this->refMonitorServer->Release();
    }
    if (this->refWatcherDirServer.isvalid())
    {
        this->refWatcherDirServer->Release();
    }

    if (nKernelServerProxy::Instance())
    {
        nKernelServerProxy::Instance()->Release();
    }

    nCommonApp::Close();

    // destroy after level manager!
    if (this->refLayerManager.isvalid())
    {
        this->refLayerManager->Release();
    }

    // release debug component server after entity server, ncEditor components use it
    if (this->refDebugComponentServer.isvalid())
    {
        this->refDebugComponentServer->Release();
    }
}

//------------------------------------------------------------------------------
/**
    Do one complete frame.
*/
void
nExplorerApp::DoFrame()
{
    // Reload resource if have changed
    this->refWatcherDirServer->Trigger();

    // Update events and make triggers handle them
//    static nGameEvent::Time currentTime(0);
//    this->refTriggerServer->Update( currentTime++ );
    this->refAITester->Trigger();

    // perform state logic
    nCommonApp::DoFrame();

    // trigger log
    LogClassRegistry::Instance()->Trigger();

    if (this->captureFrame)
    {
        this->CaptureScreenshot();
    }
}

//------------------------------------------------------------------------------
/**
    Do one complete frame.
*/
void
nExplorerApp::OnRender2D()
{
    if (!this->captureFrame)
    {
        nCommonApp::OnRender2D();
    }

    // Trigger graphic monitor server
    this->refMonitorServer->Trigger();

    // Draw graphic monitor server
    this->refMonitorServer->Render();
}

//------------------------------------------------------------------------------
/**
*/
void
nExplorerApp::SetCurrentViewport(const char* name)
{
    nAppViewport* viewport = this->refViewportUI->FindViewport(name);
    if (viewport)
    {
        this->refViewportUI->SetCurrentViewport(viewport);
    }
}

//------------------------------------------------------------------------------
/**
*/
const char*
nExplorerApp::GetCurrentViewport()
{
    return this->refViewportUI->GetCurrentViewport()->GetName();
}

//------------------------------------------------------------------------------
/**
    Adds a viewer bookmark with the current viewport
*/
void
nExplorerApp::AddBookmark(const vector3& position, const polar2& angles)
{
    // if there is a bookmark with the exact same value, clear it
    nArray<nViewerParams>::iterator iter = this->bookmarks.Find(nViewerParams(position, angles));
    if (iter)
    {
        this->bookmarks.Erase(iter);
    }
    else
    {
        this->bookmarks.Append(nViewerParams(position, angles));
    }
}

//------------------------------------------------------------------------------
/**
    Sets a saved bookmark
*/
void
nExplorerApp::SetBookmark(int index)
{
    if (index < 0 || index > this->bookmarks.Size())
    {
        return;
    }

    nAppViewport *vp = this->refViewportUI->GetCurrentViewport();
    vp->SetViewerPos(this->bookmarks[index].position);
    vp->SetViewerAngles(this->bookmarks[index].angles);
}

//------------------------------------------------------------------------------
/**
    Sets a saved bookmark
*/
int
nExplorerApp::GetNumBookmarks()
{
    return this->bookmarks.Size();
}

//------------------------------------------------------------------------------
/**
    Sets a saved bookmark
*/
void
nExplorerApp::SaveBookmarks(const char *filename)
{
    // save bookmarks for the level, if any
    if (!this->bookmarks.Empty())
    {
        nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
        n_assert(ps);

        nCmd* cmd = ps->GetCmd(this, 'THIS');
        if (ps->BeginObjectWithCmd(this, cmd, filename))
        {
            for (int i = 0; i < this->bookmarks.Size(); ++i)
            {
                cmd = ps->GetCmd(this, 'JABM');
                cmd->In()->SetF(this->bookmarks[i].position.x);
                cmd->In()->SetF(this->bookmarks[i].position.y);
                cmd->In()->SetF(this->bookmarks[i].position.z);
                cmd->In()->SetF(this->bookmarks[i].angles.theta);
                cmd->In()->SetF(this->bookmarks[i].angles.rho);
                ps->PutCmd(cmd);
            }

            ps->EndObject(true);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Sets a saved bookmark
*/
void
nExplorerApp::LoadBookmarks(const char *filename)
{
    // load bookmarks for the level
    this->bookmarks.Clear();
    if (kernelServer->GetFileServer()->FileExists(filename))
    {
        kernelServer->PushCwd(this);
        kernelServer->Load(filename, false);
        kernelServer->PopCwd();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nExplorerApp::CaptureScreenshot()
{
    nString filename;
    const char* sceneFile = this->refLoaderState->GetSceneFile();
    if (sceneFile)
    {
        filename = sceneFile;
        filename.StripExtension();
    }
    else if (this->GetLoadClassName())
    {
        filename = this->GetLoadClassName();
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
        screenshotFile.Append(".png");
    }
    while (kernelServer->GetFileServer()->FileExists(screenshotFile.Get()));

    if (nGfxServer2::Instance()->SaveScreenshot(screenshotFile.Get()))
    {
        n_message("Screenshot saved as file '%s'", screenshotFile.Get());
    }

    this->captureFrame = false;
}

//------------------------------------------------------------------------------
/**
    Try a guess on the view parameter.
    If it inside a "classes/" or "levels/" directory, the file *could* be
    an entity class or a level file. If the parent directory *looks* like
    a working copy, assign it as such, and the file as the class or level
    file to load.
*/
void
nExplorerApp::MangleSceneFileParameter()
{
    if (!this->GetSceneFile())
    {
        return;
    }

    // is it a .n2 valid file?
    nFileServer2 *fileServer = kernelServer->GetFileServer();
    if (fileServer->FileExists(this->GetSceneFile()))
    {
        // is it a level or class in a valid working copy?
        nString pathToMangle(this->GetSceneFile());
        pathToMangle.ConvertBackslashes();
        pathToMangle = pathToMangle.ExtractDirName();
        pathToMangle.StripTrailingSlash();
        nString lastDirName(pathToMangle.ExtractFileName());

        if (lastDirName == nString("levels") || lastDirName == nString("classes"))
        {
            // is the parent path a valid working copy?
            pathToMangle = pathToMangle.ExtractDirName();

            if (fileServer->DirectoryExists(nString(pathToMangle + "classes").Get()) &&
                fileServer->DirectoryExists(nString(pathToMangle + "levels").Get()))
            {
                // set working copy path
                pathToMangle.StripTrailingSlash();
                this->SetTempWorkingCopyDir(pathToMangle.Get());
                this->SetTemporaryModeEnabled(true);
                if (lastDirName == nString("levels"))
                {
                    // set -view argument as level file path
                    this->SetLevelFile(this->GetSceneFile());
                    this->SetSceneFile("");
                }
                else if (lastDirName == nString("classes"))
                {
                    // set -view argument as class file path
                    nString className(this->GetSceneFile());
                    className = className.ExtractFileName();
                    className.StripExtension();
                    className.ToCapital();
                    this->SetLoadClassName(className.Get());
                    this->SetSceneFile("");
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Set the "wc" assign for use from conjurer
*/
bool
nExplorerApp::InitWorkingCopyAssign()
{
    nFileServer2 *fileServer = kernelServer->GetFileServer();
    nArray<nString> wcPaths;

    // if temporary working copy assigned, set first for write
    if (this->GetTempWorkingCopyDir() && this->GetTemporaryModeEnabled())
    {
        wcPaths.Append(nString(this->GetTempWorkingCopyDir()));
    }

    // set primary working copy
    if (this->GetWorkingCopyDir())
    {
        nString wcString(this->GetWorkingCopyDir());
        wcString.ConvertBackslashes();
        wcPaths.Append(wcString);
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
#ifndef NGAME
        // shaders and textures will be monitorized
        nArray<nString> moniPaths;
        moniPaths.Append(nString("shaders:"));
        moniPaths.Append(nString("wctextures:"));
        fileServer->SetAssignGroup("dirwatches", moniPaths);
#endif
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Set the window title
*/
void
nExplorerApp::SetWindowTitle( const char* title )
{
    nGfxServer2::Instance()->SetWindowTitle( title );
}

//------------------------------------------------------------------------------
/**
    Calculate the window title
*/
nString
nExplorerApp::CalcTitleString( const char* prefix )
{
    nString level( this->GetLevelFile() );
    level = level.ExtractFileName();
    level.StripExtension();

    nString title;
    title.Append(prefix);
    if (this->GetInstanceName())
    {
        title.Append( " " );
        title.Append( this->GetInstanceName() );
    }
    title.Append(" - Level ");
    title.Append( level.Get() );
    title.Append( " - " N_RELEASE_SVN_REVISION_STR );
    return title;
}

//------------------------------------------------------------------------------
/**
    Create new level
*/
void
nExplorerApp::NewLevel(const char *fileName)
{
    this->refLevelManager->NewLevel(fileName);

    // create default global entities in current level
    // @todo reimplement using signals?
    nString result;
    this->refScriptServer->RunFunction("OnConjurerNewLevel", result);
}

//------------------------------------------------------------------------------
/**
    Load level
*/
bool
nExplorerApp::LoadLevel(const char *fileName)
{
    if (this->refLevelManager->LoadLevel(fileName))
    {
        // allow customizing level loading
        // @todo reimplement using signals?
        nString result;
        this->refScriptServer->RunFunction("OnConjurerLoadLevel", result);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
    Load level
*/
void
nExplorerApp::DeleteLevel(const char *fileName)
{
    this->refLevelManager->DeleteLevel(fileName);
}

//------------------------------------------------------------------------------
/**
    save current level
*/
void
nExplorerApp::SaveLevel()
{
    this->refLevelManager->SaveLevel();
}

//------------------------------------------------------------------------------
/**
    save current level under a different name
*/
bool
nExplorerApp::SaveCurrentLevelAs(const char* newLevelName)
{
    n_assert_return2(newLevelName, false, "no name supplied");

    return this->refLevelManager->SaveCurrentLevelAs(newLevelName);
}

//------------------------------------------------------------------------------
/**
    set the current instance name
*/
void 
nExplorerApp::SetInstanceName(const char * name)
{
    if (name)
    {
        this->instanceName = name;
    }
    else
    {
        this->instanceName.Clear();
    }
}

//------------------------------------------------------------------------------
/**
    get instance name
*/
const char * 
nExplorerApp::GetInstanceName() const
{
    return this->instanceName.Get();
}

//------------------------------------------------------------------------------
/**
    save the editor state before entering game
*/
void 
nExplorerApp::SaveEditorState()
{
    this->saveManager = static_cast<nSaveManager *> (nKernelServer::Instance()->New("nsavemanager"));
    n_assert(this->saveManager);

    this->saveManager->BeginSession();
    for ( nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject(); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
    {
        if (this->ShouldSaveObjectState(entity))
        {
            this->saveManager->AddObjectState(entity);
        }
    }

    // save Mission Objectives
    nMissionHandler * missionHandler = nMissionHandler::Instance();
    if( missionHandler )
    {
        this->saveManager->AddObjectState( missionHandler );
    }

    this->saveManager->EndSession();

    if ( stateFile )
    {
        this->saveManager->SaveSession(stateFile, nPersistServer::SAVETYPE_RELOAD);
        
    } else
    {
        this->saveManager->SaveSession(sessionFileName, nPersistServer::SAVETYPE_RELOAD);
    }
}

//------------------------------------------------------------------------------
/**
    determine if an object state must be saved or not
*/
bool
nExplorerApp::ShouldSaveObjectState(nEntityObject * entity) const
{
    static nClass * clneoutdoor = nKernelServer::Instance()->FindClass("neoutdoor");
    n_assert(clneoutdoor);
    static nClass * clneindoor = nKernelServer::Instance()->FindClass("neindoor");
    n_assert(clneindoor);
    static nClass * clneindoorbrush = nKernelServer::Instance()->FindClass("neindoorbrush");
    n_assert(clneindoorbrush);
    static nClass * clneportal = nKernelServer::Instance()->FindClass("neportal");
    n_assert(clneportal);
    static nClass * clnewaypoint = nKernelServer::Instance()->FindClass("newaypoint");
    n_assert(clnewaypoint);
    static nClass * clnewaypointpath = nKernelServer::Instance()->FindClass("newaypointpath");
    n_assert(clnewaypointpath);
    static nClass * clnenavmeshnode = nKernelServer::Instance()->FindClass("nenavmeshnode");
    n_assert(clnenavmeshnode);
    static nClass * clnnavmesh = nKernelServer::Instance()->FindClass("nnavmesh");
    n_assert(clnnavmesh);
    static nClass * clneviewport = nKernelServer::Instance()->FindClass("neviewport");
    n_assert(clneviewport);
    static nClass * clnecamera = nKernelServer::Instance()->FindClass("necamera");
    n_assert(clnecamera);
    static nClass * clnehumragdoll = nKernelServer::Instance()->FindClass("nehumragdoll");
    n_assert(clnehumragdoll);
    static nClass * clnescene = nKernelServer::Instance()->FindClass("nescene");
    n_assert(clnescene);
    static nClass * clnespecialfx = nKernelServer::Instance()->FindClass("nespecialfx");
    n_assert(clnespecialfx);

    // not save outdoor, indoor, waypoints and navigation mesh
    if (entity->IsA(clneoutdoor) ||
        entity->IsA(clneindoor) ||
        entity->IsA(clnewaypoint) ||
        entity->IsA(clnewaypointpath) ||
        entity->IsA(clneportal) ||
        entity->IsA(clneindoorbrush) ||
        entity->IsA(clnenavmeshnode) ||
        entity->IsA(clnnavmesh) ||
        entity->IsA(clneviewport) ||
        entity->IsA(clnecamera) ||
        entity->IsA(clnehumragdoll) ||
        entity->IsA(clnescene) ||
        entity->IsA(clnespecialfx) )
    {
        return false;
    }

    // don't save the indoor facades (identified by having a valid ptr in GetIndoorSpace())
    ncSpatial * spatialComp = entity->GetComponent<ncSpatial>();
    if (spatialComp && spatialComp->GetIndoorSpace())
    {
        return false;
    }

    // please save anything that has gameplay compo
    if (entity->GetComponent<ncGameplay>())
    {
        return true;
    }

    // not save physic objects without mass
    if (entity->GetComponent<ncPhysicsObj>() && 
        !entity->GetComponent<ncPhyPickableObj>() )
    {
        ncPhysicsObj * phyObj = entity->GetComponent<ncPhysicsObj>();
        ncPhysicsObjClass * phyObjClass = entity->GetClassComponent<ncPhysicsObjClass>();
        if (phyObj->GetMass() > 0.0f || phyObjClass->GetMobile())
        {
            return true;
        }
        
        return false;
    }

    // save anything else
    return true;
}

//------------------------------------------------------------------------------
/**
    restore the editor state after entering game
*/
void 
nExplorerApp::RestoreEditorState()
{
    for ( nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject(); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
    {
        if (this->ShouldSaveObjectState(entity))
        {
            this->saveManager->AddObjectAvailableBeforeRestore(entity);
        }
    }

    nMissionHandler * missionHandler = nMissionHandler::Instance();
    if( missionHandler )
    {
        this->saveManager->AddObjectAvailableBeforeRestore(missionHandler);
    }

    if ( stateFile )
    {
        this->saveManager->RestoreSession(stateFile);
        
    } else
    {
        this->saveManager->RestoreSession(sessionFileName);
    }

    for ( nEntityObject* entity = nEntityObjectServer::Instance()->GetFirstEntityObject(); entity; entity = nEntityObjectServer::Instance()->GetNextEntityObject() )
    {
        if (this->ShouldSaveObjectState(entity))
        {
            this->saveManager->AddObjectAvailableAfterRestore(entity);
        }
    }

    this->saveManager->CheckRestoredObjects();

    this->saveManager->ResetSession();
    this->saveManager->Release();
    this->saveManager = 0;

    if (this->stateFile)
    {
        this->stateFile->Release();
        this->stateFile = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nExplorerApp::AddNetworkUniqueStrings()
{
    nKernelServer * ks = nKernelServer::ks;

    const nHashList * classList = ks->GetClassList();

    // classes that will be inserted like unique strings
    const char * classesTable[] = { 
        "ngpbasicaction", "neweaponaddon", "neweapon",
        "neweaponaddon", "neequipment", "neammunition",
        0
    };

    nHashNode * node = classList->GetHead();
    while( node )
    {
        nClass * clazz = ks->FindClass( node->GetName() );

        if( clazz )
        {
            for( int i = 0 ; classesTable[ i ] ; ++i )
            {
                if( clazz->IsA( classesTable[ i ] ) )
                {
                    nNetUtils::InsertUniqueString( node->GetName() );
                    break;
                }
            }
        }

        node = node->GetSucc();
    }
}
