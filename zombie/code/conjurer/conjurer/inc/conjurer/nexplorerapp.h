#ifndef N_EXPLORERAPP_H
#define N_EXPLORERAPP_H
//------------------------------------------------------------------------------
/**
    @class nExplorerApp
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
class nDebugServer;
class nMonitorServer;
class nLayerManager;
class nSceneNode;
class nSceneGraph;
class nTransformNode;
class nAssetLoadState;
class nLevelManager;
class nLevel;
class nViewerParams;
class nFSMServer;
class nTriggerServer;
class nWatcherDirServer;
class nScriptClassServer;
class nPreviewViewport;
class nLoaderServer;
class nAITester;
class nWorldInterface;
class nFXServer;
class nSaveManager;
class nMemFile;
class nDebugComponentServer;
class nGlobalVariableEditor;

//------------------------------------------------------------------------------
class nExplorerApp : public nCommonApp
{
public:
    /// control mode
    enum ControlMode
    {
        FreeCam,
        GameCam,
    };

    /// constructor
    nExplorerApp();
    /// destructor
    virtual ~nExplorerApp();
    /// get instance pointer
    static nExplorerApp* Instance();

    /// set instance name
    void SetInstanceName(const char *);
    /// get instance name
    const char * GetInstanceName() const;

    /// set control mode
    void SetControlMode(ControlMode c);
    /// get control mode
    ControlMode GetControlMode() const;
    /// set level file name
    void SetLevelFile(const char* name);
    /// get level file name
    const char* GetLevelFile() const;
    /// set scene file name
    void SetSceneFile(const char* name);
    /// get scene file name
    const char* GetSceneFile() const;
    /// set entity class to instance
    void SetLoadClassName(const char* name);
    /// get entity class to instance
    const char* GetLoadClassName() const;
    /// set anim state index
    void SetAnimState(const int animStateIndex);
    /// get anim state index
    const int GetAnimState() const;
    /// set the script type (ntclserver/nluaserver/etc - required)
    void SetScriptServerClass(const char* name);
    /// get the script type
    const char* GetScriptServerClass() const;
    /// set the viewport layout
    void SetViewportLayout(const char *layoutFile);
    /// get the viewport layout
    const char *GetViewportLayout() const;

    /// enable/disable the logo overlay
    void SetOverlayEnabled(bool b);
    /// get overlay enabled status
    bool GetOverlayEnabled() const;

    /// set the initial watch
    void SetWatch(const char* name);
    /// get the initial watch
    const char* GetWatch() const;

    /// try to close outgui, returning false if user vetoes it
    //bool CloseOutGUI( bool forceToClose = false ) const;

    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();

    /// runs a script with the specific script server for the gui
    //bool RunGUIScript(const char* scriptfilename);
    /// stores the gui script file
    //void SetGUIScriptFile(const char* scriptfilename);
    /// retrieves the gui script file
    //const char* GetGuiScriptFile() const;
    /// set the quit confirmation flag
    //void SetQuitConfirmationPending( bool b );
    /// enable / disable physics (for editor states only)
    void SetPhysicsEnabled( bool b );
    /// is physics enabled (for editor states only)
    bool IsPhysicsEnabled() const;
    /// enable / disable attach (for editor states only)
    void SetAttachEnabled(bool b);
    /// is attach enabled (for editor states only)
    bool IsAttachEnabled() const;
    /// enable / disable render (for editor states only)
    void SetRenderEnabled(bool b);
    /// is render enabled (for editor states only)
    bool IsRenderEnabled() const;

    /// accessor to know if the render window must be embedded
    const bool IsRenderWindowEmbedded() const;
    /// to set if the render window must be embedded
    void SetRenderWindowEmbedded( const bool is );

    /// set the initial state
    void SetInitState( const nString & arg );
    /// get the initial state
    const nString & GetInitState()const;

    /// try to get some paths from the scene file parameter
    void MangleSceneFileParameter();

    /// @name Script interface
    //@{

        /// get current viewport by name
        void SetCurrentViewport(const char *);
        /// get current viewport name
        const char * GetCurrentViewport();
        /// move current viewport to entity position
        //void MoveCameraToEntity(nEntityObjectId);
        /// makes the current viewport camera to face the specified position
        //void MoveCameraToSelection ();

        /// add a camera bookmark
        void AddBookmark(const vector3&, const polar2&);
        /// set a camera bookmark
        void SetBookmark(int);
        /// get number of camera bookmarks
        int GetNumBookmarks();
        /// save camera bookmarks to file
        void SaveBookmarks(const char *);
        /// load camera bookmarks from file
        void LoadBookmarks(const char *);

        /// open a viewport to preview a class asset
        //void OpenPreview(const char *);
        /// close the preview viewport
        //void ClosePreview();

        /// add temporary working copy
        void SetTempWorkingCopyDir(const char *);
        /// get temporary working copy
        const char * GetTempWorkingCopyDir() const;
        /// set temporary mode on/off (if secondary wc assigned)
        void SetTemporaryModeEnabled(bool);
        /// get temporary mode on/off (if secondary wc assigned)
        bool GetTemporaryModeEnabled();

        /// set window title
        void SetWindowTitle(const char*);
        /// calculate title string with prefix
        nString CalcTitleString(const char*);

        /// new level
        void NewLevel(const char*);
        /// load level
        bool LoadLevel(const char*);
        /// delete level
        void DeleteLevel(const char*);
        /// save level
        void SaveLevel();
        /// save the current level under a different name
        bool SaveCurrentLevelAs(const char *);

        /// save the editor state before entering game
        void SaveEditorState();
        /// restore the editor state after entering game
        void RestoreEditorState();

    //@}

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
    /// add network unique strings
    void AddNetworkUniqueStrings();
    /// return true if the object state must be saved
    bool ShouldSaveObjectState(nEntityObject * obj) const;

    nAutoRef<nScriptServer> refScriptServer;
    nRef<nDebugServer> refDebugServer;
    nRef<nDebugComponentServer> refDebugComponentServer;
    nRef<nMonitorServer> refMonitorServer;
    nRef<nLayerManager> refLayerManager;
    nRef<nWatcherDirServer>  refWatcherDirServer;

    nRef<nTransformNode> refSceneNode;
    nRef<nRoot> refMaterial;
    nRef<nAITester> refAITester;
    nRef<nGlobalVariableEditor> refGlobalVarEditor;

    nString instanceName;

    nString levelFilename;

    nString loadClassName;
    nString sceneFilename;
    nString scriptserverClass;
    nString initialWatch;
    nString guiScriptFile;
    nString tempWorkingCopyDir;
    nString viewportLayoutFile;
    nString initState;
    int animStateIndex;

    bool tempModeEnabled;
    bool isOverlayEnabled;
    bool captureFrame;
    bool renderWindowEmbedded;
    bool runPhysics;
    bool attachEnabled;
    bool renderEnabled;
    ControlMode controlMode;
    int screenshotID;

    /// Path of a dummy object that exists always and only when the OutGUI is opened
    const char* outguiDummyPath;
    /// It's pending asking to the user if he/she wants to exit application? 
    /// (user has clicked close window icon)
    //bool quitConfirmationPending;

    nRef<nAssetLoadState> refLoaderState;   ///< asset load state
    nRef<nAppViewportUI> refViewportUI;     ///< viewport layout
    //nRef<nAppViewport> refRnsViewport;      ///< game viewport
    //nRef<nAppViewport> refMapViewport;      ///< map viewport
    //nRef<nPreviewViewport> refPreview;      ///< preview viewport
    nArray<nViewerParams> bookmarks;        ///< bookmark array

    // save manager used to save the editor state
    nSaveManager * saveManager;
    // memory file used to save the editor state
    nMemFile * stateFile;

};

//------------------------------------------------------------------------------
/**
*/
inline 
nExplorerApp *
nExplorerApp::Instance()
{
    return (nExplorerApp *) nApplication::Instance();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetControlMode(ControlMode mode)
{
    this->controlMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
inline
nExplorerApp::ControlMode
nExplorerApp::GetControlMode() const
{
    return this->controlMode;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetLevelFile(const char* filename)
{
    this->levelFilename = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nExplorerApp::GetLevelFile() const
{
    return this->levelFilename.IsEmpty() ? 0 : this->levelFilename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetSceneFile(const char* filename)
{
    this->sceneFilename = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nExplorerApp::GetSceneFile() const
{
    return this->sceneFilename.IsEmpty() ? 0 : this->sceneFilename.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetLoadClassName(const char* classname)
{
    this->loadClassName = classname;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nExplorerApp::GetLoadClassName() const
{
    return this->loadClassName.IsEmpty() ? 0 : this->loadClassName.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetAnimState(const int animStateIndex)
{
    this->animStateIndex = animStateIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
const int
nExplorerApp::GetAnimState() const
{
    return this->animStateIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetScriptServerClass(const char* type)
{
    this->scriptserverClass = type;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nExplorerApp::GetScriptServerClass() const
{
    return this->scriptserverClass.IsEmpty() ? 0 : this->scriptserverClass.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetViewportLayout(const char* layoutFile)
{
    this->viewportLayoutFile = layoutFile;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nExplorerApp::GetViewportLayout() const
{
    return this->viewportLayoutFile.IsEmpty() ? 0 : this->viewportLayoutFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetOverlayEnabled(bool b)
{
    this->isOverlayEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nExplorerApp::GetOverlayEnabled() const
{
    return this->isOverlayEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nExplorerApp::SetWatch(const char* name)
{
    this->initialWatch = name;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char* 
nExplorerApp::GetWatch() const
{
    return this->initialWatch.IsEmpty() ? 0 : this->initialWatch.Get();
}

//------------------------------------------------------------------------------
/**
    Stores the gui script file.
*/
//inline
//void 
//nExplorerApp::SetGUIScriptFile(const char* scriptfilename)
//{
//    this->guiScriptFile = scriptfilename;
//}

//------------------------------------------------------------------------------
/**
    Retrieves the gui script file.
*/
//inline
//const char* 
//nExplorerApp::GetGuiScriptFile() const
//{
//    return this->guiScriptFile.IsEmpty() ? 0 : this->guiScriptFile.Get();
//}

//------------------------------------------------------------------------------
/**
    Accessor to know if the render window must be embedded.
*/
//inline 
//const bool 
//nExplorerApp::IsRenderWindowEmbedded() const
//{
//    return this->renderWindowEmbedded;
//}

//------------------------------------------------------------------------------
/**
    To set if the render window must be embedded.
*/
//inline
//void 
//nExplorerApp::SetRenderWindowEmbedded( const bool is )
//{
//    this->renderWindowEmbedded = is;
//}

//------------------------------------------------------------------------------
/**
    Sets the working copy directory.
*/
inline
void 
nExplorerApp::SetTempWorkingCopyDir(const char* dirPath)
{
    this->tempWorkingCopyDir = dirPath;
    if (this->IsOpen())
    {
        this->InitWorkingCopyAssign();
    }
}

//------------------------------------------------------------------------------
/**
    Retrieves the working copy directory.
*/
inline
const char * 
nExplorerApp::GetTempWorkingCopyDir() const
{
    return this->tempWorkingCopyDir.IsEmpty() ? 0 : this->tempWorkingCopyDir.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nExplorerApp::SetTemporaryModeEnabled(bool enabled)
{
    if (enabled != this->tempModeEnabled)
    {
        this->tempModeEnabled = enabled;
        if (this->IsOpen())
        {
            this->InitWorkingCopyAssign();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Retrieves the working copy directory.
*/
inline
bool
nExplorerApp::GetTemporaryModeEnabled()
{
    return this->tempModeEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetInitState( const nString & state )
{
    this->initState = state;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString &
nExplorerApp::GetInitState() const
{
    return this->initState;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetPhysicsEnabled(bool value)
{
    this->runPhysics = value;

    if( !value )
    {
        // restoring the collision grid.
        nPhysicsWorld* world(nPhysicsServer::Instance()->GetDefaultWorld());

        if( world )
        {
            world->GetObjectManager()->Restore();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nExplorerApp::IsPhysicsEnabled() const
{
    return this->runPhysics;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetAttachEnabled(bool value)
{
    this->attachEnabled = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nExplorerApp::IsAttachEnabled() const
{
    return this->attachEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nExplorerApp::SetRenderEnabled(bool value)
{
    this->renderEnabled = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nExplorerApp::IsRenderEnabled() const
{
    return this->renderEnabled;
}

//------------------------------------------------------------------------------
#endif    
