#ifndef N_3DS_EXPORTER_SERVER_H
#define N_3DS_EXPORTER_SERVER_H
#pragma warning( push, 3 )
#include "max.h"
#include "igame/igame.h"
#pragma warning( pop )

#include "kernel/ndebug.h"
#include "kernel/ntypes.h"
#include "util/nstring.h"
#include "n3dsexportsettings/n3dsExportSettings.h"

class n3dsMaterialList;
class n3dsNebulaRuntime;
class n3dsSystemCoordinates;
class n3dsSkeleton;
class n3dsVisibilityExport;
class n3dsPhysicExport;
class n3dsGraphicsExport;
class n3dsAnimationExport;
class nAnimatorBuilderServer;
class n3dsFileServer;
class n3dsSceneList;
class nEntityClass;

//------------------------------------------------------------------------------
/**
    @class n3dsExportServer
    @ingroup n3dsMaxExporterKernel
    @brief Export server, 
    The singleton has a princiapl functions 
*/
class n3dsExportServer
{
public:
    /// Return a preoOrder scene list
    const n3dsSceneList&                GetPreOrderScene();
    /// Return a preoOrder scene list
    const n3dsSceneList&                GetPostOrderScene();
    /// the scene is order by Max Id;
    const n3dsSceneList&                GetMaxIdOrderScene();
    /// Return a singleton of this class
    static n3dsExportServer*        Instance();
    /// Get IGame Interface
    IGameScene*                     GetIGameScene();
    /// Get 3ds MAx Interface
    Interface*                      GetMaxInterface();
    /// return the queue file server
    n3dsFileServer*                 GetQueueFileServer();
    /// return the material list
    n3dsMaterialList*               GetMtlList();
    /// return the visibility exporter.
    n3dsVisibilityExport*           GetVisibility();
    /// return the physics exporter.
    n3dsPhysicExport*               GetPhysics();
    /// return the physics exporter.
    n3dsGraphicsExport*             GetGraphics();
    /// return the animation exporter.
    n3dsAnimationExport*            GetAnimation();
    /// return the interface with nebula runtime
    n3dsNebulaRuntime*              GetNebula();
    /// return this system coordinates
    n3dsSystemCoordinates*          GetSystemCoordinates();
    /// return the name of scene
    const char*                     GetName();
    /// return the windows path of scene
    const char*                     GetPath();

    /// initialize the export server
    bool init(const char* localName, bool exportSelected);
    ///
    void Close();
    /// Create class and comprobe it exist
    nEntityClass* GetExporterEntityClass(const nString& base, const nString& className, const nString& otherBase1="", const nString& otherBase2="", const nString& otherBase3="") const;

    /// set the editor info in the class
    void SetEditorInfo( nEntityClass * entityClass );
    /// export all info
    bool Export();
    /// Save all
    bool Save();
    /// Launch file with default application
    void RunScene();
    ///defualt destructor
    ~n3dsExportServer();


    /// Get settings for export
    const n3dsExportSettings& GetSettings() const;
    /// set optimize mesh data
    void SetOptimizeMeshData( bool optimize);
    /// Export neBrushClass
    bool ExportBrush();
    /// Export neIndoorClass
    bool ExportIndoor();
    /// check what part of the character do we have to export
    void ExportCharacterResources();
    /// export character
    bool ExportCharacter();
    /// export skeleton
    bool ExportSkeleton();

    void ProgressBarStart(const nString& title);
    void ProgressUpdate(float val, nString name = "");
    void ProgressBarEnd();

    /// get camera
    IGameNode* GetCamera();

protected:
    n3dsExportServer();
    void AppendToSceneList( IGameNode* node);
    /// TRanform any class in other class copy all components except physiscs
    nEntityClass* MutateClass( nEntityClass* oldClass,  nEntityClass* newBaseClass) const;
    /// Copy class components
    void CopyClassComponents( nEntityClass* srcClass,  nEntityClass* dstClass) const;
    /// Get froms scene export setting and check it
    bool GetAndCheckExportSetting(bool exportSelected);
    bool ConfigureByExportSettings();
    void PutInGrimoire();
    nArray<n3dsAssetInfo> CreateEntityClassesForLOD(nEntityClass* entityClass);
    nArray<n3dsAssetInfo> CreateEntityClassesForRagdoll(nEntityClass* entityClass);
    void  SetLODTo(nEntityClass* entityClass, nArray<nEntityClass*> entityClassArray);

private:

    static n3dsExportServer* server;
    n3dsFileServer*          queueFileServer;
    IGameScene*              iGameInterface;
    Interface*               maxInterface;
    n3dsMaterialList*        mtlList;
    n3dsVisibilityExport*    visibilityExport;
    n3dsPhysicExport*        physicExport;
    n3dsGraphicsExport*      graphicsExport;
    n3dsAnimationExport*     animationExport;
    n3dsNebulaRuntime*       nebula;
    n3dsSystemCoordinates*   coord;
    nString                  windowsPath;
    nString                  name;
    n3dsSceneList*           preOrder; 
    n3dsSceneList*           postOrder;
    n3dsSceneList*           inIdOrder;
    n3dsExportSettings       exportSettings;
    nString                  fileLog;
    nString                  barTitle;
};

//------------------------------------------------------------------------------
/**
*/
inline
const n3dsSceneList&
n3dsExportServer::GetPreOrderScene()
{
    n_assert(this->preOrder);
    return *this->preOrder;
}

//------------------------------------------------------------------------------
/**
*/
inline
const n3dsSceneList&
n3dsExportServer::GetPostOrderScene()
{
    n_assert(this->postOrder);
    return *this->postOrder;
}


//------------------------------------------------------------------------------
/**
*/
inline
const n3dsSceneList&
n3dsExportServer::GetMaxIdOrderScene()
{
    n_assert(this->inIdOrder);
    return *this->inIdOrder;
}


//------------------------------------------------------------------------------
/**
*/
inline
const n3dsExportSettings& 
n3dsExportServer::GetSettings() const
{
    return this->exportSettings;
}
#endif
//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------