#ifndef N_CONJURERAPP_H
#define N_CONJURERAPP_H
//------------------------------------------------------------------------------
/**
    @class nRnsApp
    @ingroup NebulaConjurerEditor

    Base application for the Conjurer Editor.

    (C) 2004 Conjurer Services, S.A.
*/

#include "ncommonapp/ncommonapp.h"
#include "kernel/ncmdprotonativecpp.h"
#include "entity/nentityobject.h"
#include "mathlib/polar.h"
#include "mathlib/rectangle.h"
#include "misc/nwatched.h"
#include "resource/nresourceserver.h"
#include "gfx2/ncamera2.h"
#include "gfx2/ngfxserver2.h"
#include "nphysics/nphysicsserver.h"

class nAppViewport;
class nAppViewportUI;
class nSpatialServer;
class nPhysicsServer;
class nMaterialServer;
class nMonitorServer;
class nSceneNode;
class nSceneGraph;
class nTransformNode;
class nLevelManager;
class nLevel;
class nFSMServer;
class nTriggerServer;
class nScriptClassServer;
class nLoaderServer;
class nAITester;

//------------------------------------------------------------------------------
class nRnsApp : public nCommonApp
{
public:
    /// constructor
    nRnsApp();
    /// destructor
    virtual ~nRnsApp();
    /// get instance pointer
    static nRnsApp* Instance();

    /// set instance name
    void SetInstanceName(const char *);
    /// get instance name
    const char * GetInstanceName() const;

    /// set level file name
    void SetLevelFile(const char* name);
    /// get level file name
    const char* GetLevelFile() const;

    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();

    /// set the initial state
    void SetInitState( const nString & arg );
    /// get the initial state
    const nString & GetInitState()const;

    /// set window title
    void SetWindowTitle(const char*);

    /// set fps visible
    void ShowFPS(bool enable);

protected:
    /// do one complete frame
    virtual void DoFrame();
    /// called when 2d rendering should be performed
    virtual void OnRender2D();

private:
    friend class nEditorState;

    /// set working copy assign
    bool InitWorkingCopyAssign();
    /// handle application input
    bool HandleInput();
    /// Capture ScreenShot
    void CaptureScreenshot();

    class Fps
    {
    public:
        Fps();
        void Trigger(nTime time);
        void Show(nGfxServer2 *server);
    protected:
        nWatched watchFPS;
        float accumTime;
        float countFrame;
        nTime prevTime;
        nTime updateTime;
        nString textFPS;
    };

    Fps fps;

    nAutoRef<nScriptServer> refScriptServer;
    nRef<nLevelManager> refLevelManager;

    nRef<nTransformNode> refSceneNode;
    nRef<nRoot> refMaterial;
    nRef<nAITester> refAITester;

    nString instanceName;

    nString levelFilename;

    nString workingCopyDir;
    nString initState;

    bool showFPS;
    bool captureFrame;
    int screenshotID;
    uint frameId;

    /// Path of a dummy object that exists always and only when the OutGUI is opened
    const char* outguiDummyPath;
    /// It's pending asking to the user if he/she wants to exit application? 
    /// (user has clicked close window icon)
    bool quitConfirmationPending;
    /// game viewport
    nRef<nAppViewport> refRnsViewport;      
};

//------------------------------------------------------------------------------
/**
*/
inline 
nRnsApp *
nRnsApp::Instance()
{
    return (nRnsApp *) nApplication::Instance();
}

//------------------------------------------------------------------------------
/**
    set the current instance name
*/
inline
void 
nRnsApp::SetInstanceName(const char * name)
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
inline
const char * 
nRnsApp::GetInstanceName() const
{
    return this->instanceName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRnsApp::SetLevelFile(const char* filename)
{
    this->levelFilename = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nRnsApp::GetLevelFile() const
{
    return this->levelFilename.IsEmpty() ? 0 : this->levelFilename.Get();
}

//------------------------------------------------------------------------------
/**
*/
/*
inline
void 
nRnsApp::SetWatch(const char* name)
{
    this->initialWatch = name;
}
*/

//------------------------------------------------------------------------------
/**
*/
/*
inline
const char* 
nRnsApp::GetWatch() const
{
    return this->initialWatch.IsEmpty() ? 0 : this->initialWatch.Get();
}
*/

//------------------------------------------------------------------------------
/**
*/
inline
nRnsApp::Fps::Fps() : 
    watchFPS("viewerFPS", nArg::Float),
    accumTime(0.0f),
    countFrame(0.0f),
    prevTime(0.0),
    updateTime(0.0)
{
    //buf[0]=0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRnsApp::Fps::Show(nGfxServer2 *server)
{
    server->Text(this->textFPS.Get(), vector4(1,0,0,1), -0.9f, -0.9f);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRnsApp::Fps::Trigger(nTime time)
{
    const float frameTime = float(time - prevTime);
    watchFPS->SetF(1.0f / frameTime);
    this->accumTime += frameTime;
    this->countFrame++;
    this->prevTime = time;

    if (time > this->updateTime)
    {
        this->updateTime = time + 0.2;
        int tmp = (int) (countFrame /accumTime);
        this->textFPS.Format("FPS: %03d ", tmp);
        this->countFrame = 0;
        this->accumTime = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRnsApp::ShowFPS(bool enabled)
{
    this->showFPS = enabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRnsApp::SetInitState( const nString & state )
{
    this->initState = state;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString &
nRnsApp::GetInitState() const
{
    return this->initState;
}

//------------------------------------------------------------------------------
#endif    
