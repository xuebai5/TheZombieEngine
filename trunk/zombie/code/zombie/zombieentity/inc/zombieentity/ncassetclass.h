#ifndef NC_ASSETCLASS_H
#define NC_ASSETCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncAssetClass
    @ingroup Entities
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Entity component class for asset loaders.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "zombieentity/ncloaderclass.h"

//------------------------------------------------------------------------------
class nSceneNode;
class nAnimator;
class ncSceneClass;
class ncSceneLodClass;

//------------------------------------------------------------------------------
class ncAssetClass : public ncLoaderClass
{

    NCOMPONENT_DECLARE(ncAssetClass, ncLoaderClass);

public:
    /// constructor
    ncAssetClass();
    /// destructor
    virtual ~ncAssetClass();
    /// object persistency
    bool SaveCmds(nPersistServer *ps);
    /// Initializes the instance
    virtual void InitInstance(nObject::InitInstanceMsg initType);

    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// are resources valid
    virtual bool AreResourcesValid();

    /// get animator at i-th position
    nAnimator* GetAnimatorAt(int index);

    /// set resource file for level
    void SetLevelResource(int, const char *);
    /// get resource file for level
    const char * GetLevelResource(int, int) const;
    /// get resource file for level
    int GetNumLevelResources(int) const;
    /// get number of different resource levels
    int GetNumLevels() const;
    /// set resource file for ragdoll level
    void SetRagdollResource(int, const char *);
    /// get resource file for ragdoll level
    const char * GetRagdollResource(int) const;
    /// get resource file for ragdoll level
    int GetNumRagdollResources(int) const;
    /// clean ragdoll resources
    void ClearRagdollResources();

    /// create animator (for persisted animator only)
    void CreateAnimator(const char *, const char *, const char *);
    /// add animator created externally
    void AddAnimator(const char *, const char *);
    /// get num animators
    int GetNumAnimators() const;
    /// get animator node
    const char* GetAnimatorNodeAt(int) const;

    /// reset all scene dependencies
    void ClearSceneDependencies();
    /// get number of scene dependencies (level)
    int GetNumSceneDependencies(int);
    /// set scene dependency from file (level, target, file, command, server)
    void SetSceneDependency(int, const char *, const char *, const char *, const char *);
    /// add scene dependency from path (level, target, path, command, server)
    void AddSceneDependency(int, const char *, const char *, const char *, const char *);
    /// set scene dependency from file (level, target, command, value)
    void SetSceneIntOverride(int, const char *, const char *, int);

    #ifdef NGAME
    /// hack for backwards compatibility
    void SetAssetEditable(bool);
    #else
    /// helper functions for the LevelAsset property
    void SetLevelAsset(nString&, nString&, nString&);
    void GetLevelAsset(nString&, nString&, nString&);
    /// helper functions for the LevelAsset property
    void SetRagdollAsset(nString&, nString&, nString&);
    void GetRagdollAsset(nString&, nString&, nString&);
    /// helper functions for the AssetEditable property
    void SetAssetEditable(bool);
    bool GetAssetEditable() const;
    /// helper functions for the Instanced property
    void SetAssetInstanced(bool, bool, bool);
    void GetAssetInstanced(bool&, bool&, bool&);
    /// helper functions for the InstanceFrequency property
    void SetAssetInstanceFrequency(int, int, int);
    void GetAssetInstanceFrequency(int&, int&, int&);
    /// helper functions for the BatchDepthShapes property
    void SetBatchDepthShapes(bool);
    bool GetBatchDepthShapes();
    /// helper functions for the BatchShapesByMaterial property
    void SetBatchShapesByMaterial(bool);
    bool GetBatchShapesByMaterial();
    /// helper functions for the BatchDepthShapes property
    void SetBatchDepthShapesByCell(bool);
    bool GetBatchDepthShapesByCell();
    /// helper functions for the ShadowCasterLevel property
    void SetShadowCasterLevel(int);
    int GetShadowCasterLevel();
    /// helper functions for the ShadowCasterLevel property
    void SetDisableShadowLevel(int);
    int GetDisableShadowLevel();
    /// helper functions for the MaxMaterialLevel property
    void SetMaterialProfile(int, int, int);
    void GetMaterialProfile(int&, int&, int&);
    /// helper functions for the NumMaterials property
    void SetNumMaterialsByLevel(int, int, int);
    void GetNumMaterialsByLevel(int&, int&, int&);
    #endif

protected:
    /// load scene resource from resource path
    nSceneNode* LoadSceneResource(nSceneNode* libSceneRoot, const char *resourceFile);
    /// load instanced scene
    nSceneNode* LoadInstancedScene(nSceneNode* sceneRoot, const char *streamName);
    /// load class animators into the scene
    void LoadAnimators(nSceneNode* sceneRes);
    /// load scene dependencies into the scene resource
    void LoadSceneDependencies(int level, nSceneNode* sceneRes);
    /// load a fake batch level for debug
    void LoadFakeBatchLevel(ncSceneLodClass* sceneLodClass, float levelDistance);
    /// load shadow caster from a different level
    void LoadShadowCasterLevel(ncSceneLodClass* sceneLodClass, int shadowLevel, int disableLevel);
    /// load default scene values from global variables
    void LoadDefaultSceneVariables(ncSceneClass* sceneClass);

    /// find shadow resource in selected level
    nSceneNode* FindShadowResource(nSceneNode* sceneRes);
    /// clone shadow resource in selected level
    nSceneNode* LoadShadowResource(int level);

    /// find shadow resource in selected level
    void SetSkeletonLevelForSkinnedGeometry(nSceneNode* sceneRes, int level);

    /// hot reload all scene resources and entities
    virtual void ReloadResources();

    #ifndef NGAME
    /// helper methods for class inspectors
    void SetInstanceEnabled(int level, bool value);
    bool GetInstanceEnabled(int level);
    /// helper methods for class inspectors
    void SetMaxMaterialLevel(int lod, int level);
    int GetMaxMaterialLevel(int lod);
    /// helper methods for class inspectors
    void SetBoolOptionVariable(const char* key, bool value);
    bool GetBoolOptionVariable(const char* key);
    /// helper methods for class inspectors
    void SetIntOptionVariable(const char* key, int value);
    int GetIntOptionVariable(const char* key);
    
    /// notify when a global variable has changed
    void GlobalVariableChanged();

    /// get number of different materials in a scene resource
    int GetNumMaterials(nSceneNode* sceneNode);
    static void CountMaterials(nSceneNode* sceneNode);

    /// helper array to count materials
    static nArray<nString> surfaceArray;
    #endif

    struct LevelResource
    {
        int level;
        nArray<nString> resourceFiles;

    #ifndef NGAME
        int numMaterials;
    #endif
    };
    nArray<LevelResource> levelResources;
    nArray<LevelResource> ragdollResources;

    struct AnimatorEntry
    {
        AnimatorEntry();
        ~AnimatorEntry();

        nRef<nAnimator> refAnimator;
        nString targetNodePath;
        bool isValid;
    };

    nArray<AnimatorEntry> animatorArray;
    nString animatorsNOH;

    /// scene dependencies and overrides by level of detail
    struct SceneDependency
    {
        /// default constructor
        SceneDependency() :
            isDirty(false) { }

        /// constructor for dependencies
        SceneDependency(int l, const char *target, const char *file, const char *cmd, const char *server) :
            isDirty(false),
            level(l),
            refTargetNode(target),
            fileName(file),
            command(cmd),
            serverPath(server) { }

        /// constructor for int overrides
        SceneDependency(int l, const char *target, const char *cmd, const int value) :
            isDirty(false),
            level(l),
            refTargetNode(target),
            command(cmd)
            {
                this->value.SetI(value);
            }

        bool operator==(const SceneDependency& other)
        {
            return (this->level == other.level) &&
                   !strcmp(this->refTargetNode.getname(), other.refTargetNode.getname()) &&
                   (this->fileName == other.fileName) && (this->command == other.command);
        }

        /// need to re-persist the dependency to its file?
        bool isDirty;

        int level;
        nDynAutoRef<nSceneNode> refTargetNode;
        nString command;
        /// if the override is a dependency:
        nString fileName;
        nString serverPath;
        /// if the override is something else:
        nArg value;
    };

    nArray<SceneDependency> sceneDependencies;

};

//------------------------------------------------------------------------------
/**
*/
inline
ncAssetClass::AnimatorEntry::AnimatorEntry() :
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
ncAssetClass::AnimatorEntry::~AnimatorEntry()
{
    // empty
}

//------------------------------------------------------------------------------
#endif
