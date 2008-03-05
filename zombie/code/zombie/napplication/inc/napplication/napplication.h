#ifndef N_APPLICATION_H
#define N_APPLICATION_H
//------------------------------------------------------------------------------
/**
    @class nApplication
    @ingroup Application

    @brief A simple application wrapper class. Derive a subclass for a
    specialized application.

    (C) 2004 RadonLabs GmbH
*/
#include "kernel/nroot.h"
#include "kernel/nref.h"
#include "gfx2/ndisplaymode2.h"
#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"
#include "misc/nwatched.h"

class nScriptServer;
class nGfxServer2;
class nInputServer;
class nConServer;
class nResourceServer;
class nSceneServer;
class nVariableServer;
class nParticleServer;
class nParticleServer2;
class nVideoServer;
class nGuiServer;
//class nShadowServer2;
class nAudioServer3;
class nListener3;
class nPrefServer;
//class nLocaleServer;
class nAppState;
class nSignalServer;
class nAppViewport;
class nWayPointServer;
class nGameMaterialServer;
class nRagDollManager;
class nProfilerManager;
class nFXServer;
class nScriptClassServer;
class nLoaderServer;
class nWorldInterface;
class nMissionHandler;
class nFSMServer;
class nTriggerServer;
class nGameMessageWindowProxy;

//------------------------------------------------------------------------------
class nApplication : public nRoot
{
public:
    /// constructor
    nApplication();
    /// destructor
    virtual ~nApplication();
    /// return instance pointer
    static nApplication* Instance();
    /// set the project directory (if different from home:)
    void SetProjectDirectory(const nString& n);
    /// get project directory
    const nString& GetProjectDirectory() const;
    /// set company name
    void SetCompanyName(const nString& n);
    /// get company name
    const nString& GetCompanyName() const;
    /// set application name
    void SetAppName(const nString& n);
    /// get application name
    const nString& GetAppName() const;
    /// set startup script name
    void SetStartupScript(const nString& s);
    /// get startup script name
    const nString& GetStartupScript() const;
    /// set display mode
    void SetDisplayMode(const nDisplayMode2& mode);
    /// get display mode
    const nDisplayMode2& GetDisplayMode() const;
    /// set optional feature set override
    void SetFeatureSetOverride(nGfxServer2::FeatureSet f);
    /// get optional feature set override
    nGfxServer2::FeatureSet GetFeatureSetOverride() const;
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// run the application
    virtual void Run();
    /// set the quit requested flag
    void SetQuitRequested(bool b);
    /// get the quit requested flag
    bool GetQuitRequested() const;
    /// set the input handled flag
    void SetInputHandled(bool b);
    /// get the input handled flag
    bool GetInputHandled() const;
    /// set current application state (will be activated at beginning of next frame)
    void SetState(const nString& n);
    /// get current application state
    nString GetCurrentState() const;
    /// find app state by name
    nAppState* FindState(const nString& name);
    /// get current system time
    nTime GetTime() const;
    /// get current frame time
    nTime GetFrameTime() const;
    /// get current state time (time since last state switch)
    nTime GetStateTime() const;
    /// get current frames per second
    double GetFPS() const;
    /// get current frameid
    uint GetFrameId() const;
    /// set the locale table filename
    void SetLocaleTable(const nString& s);
    /// get the locale table filename
    const nString& GetLocaleTable() const;
    /// get audio listener
    nListener3* GetAudioListener();
    /// set debug memory stats enabled
    void SetDebugMemoryStats(bool);
    /// get debug memory stats enabled
    bool GetDebugMemoryStats() const;

    /// get release build info
    void GetReleaseBuildInfo( int & version, int & major, int & minor, int & patch, char * versionstr, char * date, char * time, char * id ) const;
    /// get release subversion info
    void GetReleaseSubversionInfo( int & revision, char * revstr, char* range, char* url, char* date, bool & mixed, bool & localmod ) const;

protected:
    /// create a new state object
    void CreateState(const nString& className, const nString& stateName);
    /// create a new viewport object
    nAppViewport* CreateViewport(const nString& className, const nString& viewportName);

    /// create script server object, override in subclass as needed
    virtual nScriptServer* CreateScriptServer();
    /// create graphics server, override in subclass as needed
    virtual nGfxServer2* CreateGfxServer();
    /// create input server, override in subclass as needed
    virtual nInputServer* CreateInputServer();
    /// create console server, override in subclass as needed
    virtual nConServer* CreateConsoleServer();
    /// create resource server, override in subclass as needed
    virtual nResourceServer* CreateResourceServer();
    /// create scene server, override in subclass as needed
    virtual nSceneServer* CreateSceneServer();
    /// create variable server, override in subclass as needed
    virtual nVariableServer* CreateVariableServer();
    /// create particle server, override in subclass as needed
    virtual nParticleServer* CreateParticleServer();
    /// create video server, override in subclass as needed
    virtual nVideoServer* CreateVideoServer();
    /// create gui server, override in subclass as needed
    virtual nGuiServer* CreateGuiServer();
    /// create shadow server, override in subclass as needed
    //virtual nShadowServer2* CreateShadowServer();
    /// create audio server, override in subclass as needed
    virtual nAudioServer3* CreateAudioServer();
    /// create audio listener, override in subclass as needed
    virtual nListener3* CreateAudioListener();
    /// create preferences server, override in subclass as needed
    virtual nPrefServer* CreatePrefServer();
    /// create a profiler manager
    virtual nProfilerManager * CreateProfilerManager();
    /// Create the signal server
    virtual nSignalServer* CreateSignalServer();

    /// update time values
    virtual void OnUpdateTime();
    /// perform a state transition
    virtual void DoStateTransition();
    /// do one complete frame
    virtual void DoFrame();
    /// called when 3d rendering should be performed
    virtual void OnRenderSound();
    /// called when 3d rendering should be performed
    virtual void OnRender3D();
    /// called when 2d rendering should be performed
    virtual void OnRender2D();
    /// handle common input of the application
    virtual bool HandleInput();
    /// called before nSceneServer::RenderScene()
    virtual void OnFrameBefore();
    /// called after nSceneServer::RenderScene()
    virtual void OnFrameRendered();

private:
    nRef<nScriptServer>     refScriptServer;
    nRef<nGfxServer2>       refGfxServer;
    nRef<nInputServer>      refInputServer;
    nRef<nConServer>        refConServer;
    nRef<nResourceServer>   refResourceServer;
    nRef<nSceneServer>      refSceneServer;
    nRef<nVariableServer>   refVariableServer;
    nRef<nParticleServer>   refParticleServer;
    nRef<nParticleServer2>  refParticleServer2;
    nRef<nVideoServer>      refVideoServer;
    nRef<nGuiServer>        refGuiServer;
    //nRef<nShadowServer2>    refShadowServer;
    nRef<nAudioServer3>     refAudioServer;
    nRef<nProfilerManager>  refProfilerManager;
    nRef<nPrefServer>       refPrefServer;
    //nRef<nLocaleServer>     refLocaleServer;
    nRef<nSignalServer>     refSignalServer;

    nArray<nString> stateNames;
    nArray<nString> stateClasses;
    
    static nApplication* Singleton;

    bool isOpen;
    bool quitRequested;
    bool inputHandled;
    bool debugMemoryStats;

    nRef<nRoot> refAppStates;
    nRef<nRoot> refAppViewports;
    nRef<nAppState> curState;
    nRef<nAppState> nextState;

    nString projDir;
    nString companyName;
    nString appName;
    nString startupScript;
    nString localeTable;
    nDisplayMode2 displayMode;
    nGfxServer2::FeatureSet featureSetOverride;
    nCamera2 gfxCamera;

    nTime stateTransitionTime;
    nTime time;
    nTime stateTime;
    nTime frameTime;
    double fps;
    uint frameId;

    nListener3* audioListener;

    #if __NEBULA_STATS__
    nProfiler profAppStateOnFrame;
    nProfiler profAppStateRender2D;
    nProfiler profAppStateRender3D;

    nProfiler profAppDoFrameGui;
    nProfiler profAppDoFrameInput;
    nProfiler profAppDoFrameSound;
    nProfiler profAppDoFrameRender;
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nApplication*
nApplication::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nApplication::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetProjectDirectory(const nString& n)
{
    this->projDir = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nApplication::GetProjectDirectory() const
{
    return this->projDir;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetCompanyName(const nString& n)
{
    this->companyName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nApplication::GetCompanyName() const
{
    return this->companyName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetAppName(const nString& n)
{
    this->appName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nApplication::GetAppName() const
{
    return this->appName;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetStartupScript(const nString& n)
{
    this->startupScript = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nApplication::GetStartupScript() const
{
    return this->startupScript;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetDisplayMode(const nDisplayMode2& mode)
{
    this->displayMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nDisplayMode2&
nApplication::GetDisplayMode() const
{
    return this->displayMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetFeatureSetOverride(nGfxServer2::FeatureSet f)
{
    this->featureSetOverride = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
nGfxServer2::FeatureSet
nApplication::GetFeatureSetOverride() const
{
    return this->featureSetOverride;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nApplication::GetTime() const
{
    return this->time;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nApplication::GetFrameTime() const
{
    return this->frameTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
nApplication::GetStateTime() const
{
    return this->stateTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
double
nApplication::GetFPS() const
{
    return this->fps;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
nApplication::GetFrameId() const
{
    return this->frameId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetQuitRequested(bool b)
{
    this->quitRequested = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nApplication::GetQuitRequested() const
{
    return this->quitRequested;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetInputHandled(bool b)
{
    this->inputHandled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nApplication::GetInputHandled() const
{
    return this->inputHandled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nApplication::SetDebugMemoryStats(bool b)
{
    this->debugMemoryStats = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nApplication::GetDebugMemoryStats() const
{
    return this->debugMemoryStats;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nApplication::SetLocaleTable(const nString& s)
{
    this->localeTable = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString& 
nApplication::GetLocaleTable() const
{
    return this->localeTable;
}

//------------------------------------------------------------------------------
/**
*/
inline
nListener3* 
nApplication::GetAudioListener()
{
    n_assert( this->audioListener );
    return this->audioListener;
}

//------------------------------------------------------------------------------
#endif
