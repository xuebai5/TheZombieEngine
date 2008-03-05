#include "precompiled/pchncommonapp.h"
//------------------------------------------------------------------------------
//  ncommonapp_main.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncommonapp/ncommonapp.h"
#include "nnetworkenet/nnetutils.h"
#include "rnsgameplay/ncgameplay.h"

#include "nanimation/nanimationserver.h"
#include "zombieentity/nloaderserver.h"
#include "nscriptclassserver/nscriptclassserver.h"
#include "nspecialfx/nfxserver.h"
#include "nsoundscheduler/nsoundscheduler.h"
#include "nmusictable/nmusictable.h"
#include "ngeomipmap/ngeomipmapresourceloader.h"
#include "nvegetation/nvegetationmeshresourceloader.h"
#include "nwaypointserver/nwaypointserver.h"
#include "nragdollmanager/nragdollmanager.h"
#include "ntrigger/ntriggerserver.h"
#include "gameplay/nmissionhandler.h"
#include "gameplay/ngamemessagewindowproxy.h"
#include "nfsmserver/nfsmserver.h"
#include "kernel/nremoteserver.h"
#include "kernel/logclassregistry.h"
#include "nmaterial/nmaterialserver.h"
#include "nphysics/nphysicsserver.h"
#include "nspatial/nspatialserver.h"
#include "entity/nentityclassserver.h"
#include "entity/nentityobjectserver.h"
#include "signals/nsignalserver.h"
#include "appinfo/appinfo.h"
#include "gameplay/ngamematerialserver.h"
#include "resource/nresourceloader.h"
#include "nlevel/nlevelmanager.h"
#include "kernel/ndependencyserver.h"
#include "npreloadmanager/npreloadmanager.h"

nNebulaClass(nCommonApp, "napplication");

namespace
{
    const char* GlobalVarsFile("wc:libs/variables/globals.n2");
}

//------------------------------------------------------------------------------
/**
*/
nCommonApp::nCommonApp()
    //percVisibility("percVisibility", nArg::Float),
    #if __NEBULA_STATS__
  : profAppDoFrameAnimation("profAppDoFrameAnimation"),
    profAppDoFramePhysics("profAppDoFramePhysics")
    #endif
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCommonApp::~nCommonApp()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nCommonApp::Open()
{
    ///@todo ma.garcias - need to keep this here, material database is assigned at OnStartup()
    ///should be done at a OnMaterialStartup
    this->refMaterialServer = (nMaterialServer*) kernelServer->New("nmaterialserver", "/sys/servers/material");
    this->refLevelManager   = (nLevelManager*) kernelServer->New("nlevelmanager", "/sys/servers/levelmanager");

    nApplication::Open();

    this->refMissionHandler = (nMissionHandler*) kernelServer->New("nmissionhandler", "/sys/servers/missionhandler");
    this->refGameMessageWindowProxy = (nGameMessageWindowProxy*) kernelServer->New("ngamemessagewindowproxy", "/sys/servers/gamemessagewindowproxy");

    // create basic dependency server
    kernelServer->New("ndependencyserver", "/sys/servers/dependency/basic");

    this->refEntityObjectServer = (nEntityObjectServer *) kernelServer->New("nentityobjectserver", "/sys/servers/entityobject");
    this->refEntityClassServer = (nEntityClassServer *) kernelServer->New("nentityclassserver", "/sys/servers/entityclass");

    this->refAnimationServer = (nAnimationServer*) kernelServer->New("nanimationserver", "/sys/servers/anim");
    this->refFXServer       = (nFXServer*) kernelServer->New("nfxserver", "/sys/servers/specialfx");
    this->refPhysicsServer  = (nPhysicsServer*) kernelServer->New("nphysicsserver", "/sys/servers/physics");
    this->refWayPointServer = (nWayPointServer*) kernelServer->New("nwaypointserver", "/sys/servers/waypoint");
    this->refSoundScheduler = (nSoundScheduler*) kernelServer->New("nsoundscheduler", "/sys/servers/soundscheduler");
    // load sound library, must be before creation of material server for nGameMaterials to load sound events
    this->refSoundScheduler->LoadSoundLibrary( soundLibraryPath );
    this->refSpatialServer  = (nSpatialServer*) kernelServer->New("nspatialserver", "/sys/servers/spatial");
    this->refRagDollManager = (nRagDollManager*) kernelServer->New("nragdollmanager", "/sys/manager/ragdoll");
    this->refGeoMipMapResourceLoader = (nGeoMipMapResourceLoader*) kernelServer->New("ngeomipmapresourceloader", "/sys/loaders/gmmresloader");
    this->refVegetationMeshResourceLoader = (nVegetationMeshResourceLoader*) kernelServer->New("nvegetationmeshresourceloader", "/sys/loaders/vegetation");
    this->refGameMaterialServer = (nGameMaterialServer*) kernelServer->New("ngamematerialserver", "/sys/servers/gamematerial");
    this->refBatchMeshLoader = (nResourceLoader*) kernelServer->New("nbatchmeshloader", "/sys/loaders/batchmesh");
    this->refLoaderServer   = (nLoaderServer*) kernelServer->New("nloaderserver", "/sys/servers/loader");
    this->refPreloadManager = (nPreloadManager*) kernelServer->New("npreloadmanager", "/sys/servers/preloadmanager");
    this->refWorldInterface = (nWorldInterface*) kernelServer->New("nworldinterface", "/sys/servers/worldinterface");
    this->refScriptClassServer = (nScriptClassServer*) kernelServer->New("nscriptclasslua", "/sys/servers/scriptclass");
    this->refFSMServer      = (nFSMServer*) kernelServer->New("nfsmserver", "/sys/servers/fsm");
    this->refTriggerServer  = (nTriggerServer*) kernelServer->New("ntriggerserver", "/sys/servers/trigger");

    // open the material server.
    if (!this->refMaterialServer->Open())
    {
        this->Close();
        return false;
    }

    LogClassRegistry::Instance()->SetUserLogOutputObject( &this->tragStateLogBuffer );

    // Init the mission handler after the level manager has been instanced
    this->refMissionHandler->Init();
    // Init the preload manager after the level manager has been instanced
    this->refPreloadManager->Init();

    // Global variables default values
    nVariable::Handle var = nVariableServer::Instance()->GetVariableHandleByName("MaxPathFindingTime");
    nVariableServer::Instance()->SetFloatVariable( var, 0.05f );
    var = nVariableServer::Instance()->GetVariableHandleByName("MaxPathSmoothingTime");
    nVariableServer::Instance()->SetFloatVariable( var, 0.05f );
    var = nVariableServer::Instance()->GetVariableHandleByName("MinAgentsSpeed");
    nVariableServer::Instance()->SetFloatVariable( var, 1.0f );
    var = nVariableServer::Instance()->GetVariableHandleByName("GroundSeekStartDistance");
    nVariableServer::Instance()->SetFloatVariable( var, -0.5f );
    var = nVariableServer::Instance()->GetVariableHandleByName("GroundSeekEndDistance");
    nVariableServer::Instance()->SetFloatVariable( var, 4.0f );

    // load global variables
    this->LoadGlobalVariables();

    // add script commands to classes
    //this->refScriptClassServer->AddClasses();

    // add unique strings for network
    this->AddNetworkUniqueStrings();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonApp::Close()
{
    if (this->refLevelManager.isvalid())
    {
        this->refLevelManager->Release();
    }

    if (this->refLoaderServer.isvalid())
    {
        this->refLoaderServer->Release();
    }

    if (this->refFXServer.isvalid())
    {
        this->refFXServer->Release();
    }

    if (this->refMaterialServer.isvalid())
    {
        this->refMaterialServer->Close();
        this->refMaterialServer->Release();
    }

    if (this->refRagDollManager.isvalid())
    {
        this->refRagDollManager->Release();
    }

    if (this->refBatchMeshLoader.isvalid())
    {
        this->refBatchMeshLoader->Release();
    }

    if (this->refGeoMipMapResourceLoader.isvalid())
    {
        this->refGeoMipMapResourceLoader->Release();
    }

    if (this->refVegetationMeshResourceLoader.isvalid())
    {
        this->refVegetationMeshResourceLoader->Release();
    }

    if (this->refSpatialServer.isvalid())
    {
        this->refSpatialServer->Release();
    }
    
    if (this->refWayPointServer.isvalid())
    {
        this->refWayPointServer->Release();
    }
    
    if (this->refEntityObjectServer.isvalid())
    {
        this->refEntityObjectServer->Release();
    }
    
    if (this->refEntityClassServer.isvalid())
    {
        this->refEntityClassServer->Release();
    }
    
    // physics server has to be released after the entity server (imperative)
    if (this->refPhysicsServer.isvalid())
    {
        this->refPhysicsServer->Release();
    }

    if (this->refGameMaterialServer.isvalid())
    {
        this->refGameMaterialServer->Release();
    }

    if (this->refAnimationServer.isvalid())
    {
        this->refAnimationServer->Release();
    }

    // the trigger server must be released before entities are unloaded
    // (beware, this is assuming that no one else has a release pending on it)
    if (this->refTriggerServer.isvalid())
    {
        this->refTriggerServer->Release();
    }

    if (this->refFSMServer.isvalid())
    {
        this->refFSMServer->Release();
    }

    if (this->refMissionHandler.isvalid())
    {
        this->refMissionHandler->Release();
    }

    if (this->refGameMessageWindowProxy.isvalid())
    {
        this->refGameMessageWindowProxy->Release();
    }

    if ( this->refMusicTable.isvalid() )
    {
        this->refMusicTable->Release();
    }

    if ( this->refSoundScheduler.isvalid() )
    {
        this->refSoundScheduler->Release();
    }

    // erase dependency server static variables
    nDependencyServer::InitGlobal();

    nApplication::Close();
}

//------------------------------------------------------------------------------
/**
    Do one complete frame.
*/
void
nCommonApp::DoFrame()
{
    #if __NEBULA_STATS__
    this->profAppDoFrameAnimation.ResetAccum();
    this->profAppDoFramePhysics.ResetAccum();
    #endif

    // determine visibility
    this->refSpatialServer->SetFrameId(this->GetFrameId());

    #if __NEBULA_STATS__
    this->profAppDoFrameAnimation.StartAccum();
    #endif
    // update animations
    this->refAnimationServer->Trigger(this->GetTime(), (float)this->GetFPS());
    #if __NEBULA_STATS__
    this->profAppDoFrameAnimation.StopAccum();
    #endif

    nApplication::DoFrame();

    #if __NEBULA_STATS__
    this->profAppDoFrameAnimation.Publish();
    this->profAppDoFramePhysics.Publish();
    #endif
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonApp::OnRenderSound()
{
    // trigger sound scheduler
    if (this->refSoundScheduler.isvalid())
    {
        this->refSoundScheduler->Trigger(this->GetTime());
    }

    // update music table
    if (this->refMusicTable.isvalid())
    {
        this->refMusicTable->Update( float(this->GetTime()) );
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nCommonApp::InitWorkingCopyAssign()
{
    nFileServer2 *fileServer = kernelServer->GetFileServer();
    nArray<nString> wcPaths;

    // if temporary working copy assigned, set first for write
    //if (this->GetTempWorkingCopyDir() && this->GetTemporaryModeEnabled())
    //{
    //    wcPaths.Append(nString(this->GetTempWorkingCopyDir()));
    //}

    // set primary working copy
    //if (this->GetWorkingCopyDir())
    //{
    //    nString wcString(this->GetWorkingCopyDir());
    //    wcString.ConvertBackslashes();
    //    wcPaths.Append(wcString);
    //}
    #ifdef __WIN32__
    //else
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
    //if (wcPaths.Size() == 0 && fileServer->DirectoryExists("home:../conjurer_wc"))
    //{
    //    wcPaths.Append(nString("home:../conjurer_wc"));
    //}

    if (wcPaths.Size() > 0)
    {
        fileServer->SetAssignGroup("wc", wcPaths);
        //#ifndef NGAME
        // shaders and textures will be monitorized
        //nArray<nString> moniPaths;
        //moniPaths.Append(nString("shaders:"));
        //moniPaths.Append(nString("wctextures:"));
        //fileServer->SetAssignGroup("dirwatches", moniPaths);
        //#endif
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
nCommonApp::AddNetworkUniqueStrings()
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

//------------------------------------------------------------------------------
/**
*/
void
nCommonApp::LoadGlobalVariables()
{
    if (kernelServer->GetFileServer()->FileExists(GlobalVarsFile))
    {
        kernelServer->PushCwd(nVariableServer::Instance());
        kernelServer->Load(GlobalVarsFile, false);
        kernelServer->PopCwd();
    }
}
