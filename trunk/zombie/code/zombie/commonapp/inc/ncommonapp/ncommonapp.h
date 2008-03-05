#ifndef N_COMMONAPP_H
#define N_COMMONAPP_H
//------------------------------------------------------------------------------
/**
    @class nCommonApp
    @ingroup Application
    @author MA Garcias <ma.garcias@yahoo.es>

    Common application behavior to editor and game applications.

    (C) 2006 Conjurer Services, S.A.
*/
#include "napplication/napplication.h"
#include "ncommonapp/ncommonstatelogbuffer.h"

class nMaterialServer;
class nAnimationServer;
class nSoundScheduler;
class nMusicTable;
class nSpatialServer;
class nPhysicsServer;
class nMaterialServer;
class nGeoMipMapResourceLoader;
class nVegetationMeshResourceLoader;
class nEntityClassServer;
class nEntityObjectServer;
class nWayPointServer;
class nGameMaterialServer;
class nRagDollManager;
class nFXServer;
class nScriptClassServer;
class nLoaderServer;
class nWorldInterface;
class nLevelManager;
class nMissionHandler;
class nFSMServer;
class nTriggerServer;
class nGameMessageWindowProxy;
class nPreloadManager;
//------------------------------------------------------------------------------
class nCommonApp : public nApplication
{
public:
    /// constructor
    nCommonApp();
    /// destructor
    virtual ~nCommonApp();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();
    /// do one complete frame
    virtual void DoFrame();
    /// do one complete frame
    virtual void OnRenderSound();

    /// set primary working copy
    void SetWorkingCopyDir(const char *);
    /// get primary working copy
    const char * GetWorkingCopyDir() const;

protected:
    /// set working copy assign
    bool InitWorkingCopyAssign();
    /// add network unique strings
    void AddNetworkUniqueStrings();
    /// load global variables
    void LoadGlobalVariables();

    nRef<nAnimationServer>              refAnimationServer;
    nRef<nPhysicsServer>                refPhysicsServer;
    nRef<nMaterialServer>               refMaterialServer;
    nRef<nSpatialServer>                refSpatialServer;
    nRef<nEntityClassServer>            refEntityClassServer;
    nRef<nEntityObjectServer>           refEntityObjectServer;
    nRef<nWayPointServer>               refWayPointServer;
    nRef<nGameMaterialServer>           refGameMaterialServer;
    nRef<nResourceLoader>               refBatchMeshLoader;
    nRef<nGeoMipMapResourceLoader>      refGeoMipMapResourceLoader;
    nRef<nVegetationMeshResourceLoader> refVegetationMeshResourceLoader;
    nRef<nRagDollManager>               refRagDollManager;
    nRef<nFXServer>                     refFXServer;
    nRef<nLoaderServer>                 refLoaderServer;
    nRef<nWorldInterface>               refWorldInterface;
    nRef<nLevelManager>                 refLevelManager;
    nRef<nFSMServer>                    refFSMServer;
    nRef<nTriggerServer>                refTriggerServer;
    nRef<nMissionHandler>               refMissionHandler;
    nRef<nGameMessageWindowProxy>       refGameMessageWindowProxy;
    nRef<nScriptClassServer>            refScriptClassServer;
    nRef<nPreloadManager>               refPreloadManager;

    nRef<nSoundScheduler>               refSoundScheduler;
    nRef<nMusicTable>                   refMusicTable;

    /// working copy directory
    nString workingCopyDir;

    /// Object for GUI log display
    nCommonStateLogBuffer tragStateLogBuffer;

    #if __NEBULA_STATS__
    nProfiler profAppDoFrameAnimation;
    nProfiler profAppDoFramePhysics;
    friend class RnsGameState;
    #endif
};

//------------------------------------------------------------------------------
/**
    Sets the working copy directory.
*/
inline
void 
nCommonApp::SetWorkingCopyDir(const char* dirPath)
{
    this->workingCopyDir = dirPath;
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
const char* 
nCommonApp::GetWorkingCopyDir() const
{
    return this->workingCopyDir.IsEmpty() ? 0 : this->workingCopyDir.Get();
}

//------------------------------------------------------------------------------
#endif    
