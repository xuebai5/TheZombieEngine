#ifndef NC_SCENELODCLASS_H
#define NC_SCENELODCLASS_H
//------------------------------------------------------------------------------
/**
    @class ncSceneLodClass
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Graphics component class for entities with levels of detail.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncsceneclass.h"

class nEntityClass;
//------------------------------------------------------------------------------
class ncSceneLodClass : public ncSceneClass
{

    NCOMPONENT_DECLARE(ncSceneLodClass,ncSceneClass);

public:
    /// constructor
    ncSceneLodClass();
    /// destructor
    virtual ~ncSceneLodClass();
    /// object persistency
    bool SaveCmds(nPersistServer* ps);
    /// load class resources
    virtual bool LoadResources();
    /// unload class resources
    virtual void UnloadResources();
    /// return true if valid (if root node is set)
    virtual bool IsValid();
    /// invalidate the render context
    virtual void Invalidate();

    /// get number of detail levels
    int GetNumLevels();
    /// compute level of detail for distance
    int SelectLevelOfDetail(float viewerDist);

    /// set entity class for level of detail
    void SetLevelRoot(int, nSceneNode*);
    /// get entity class for level of detail
    nSceneNode* GetLevelRoot(int, int);
    /// get entity class for level of detail
    int GetNumLevelRoots(int);
    /// set distance range for level of detail
    void SetLevelRange(int, float, float);
    /// get distance range for level of detail
    void GetLevelRange(int, float&, float&);
    /// set instancing for level of detail
    void SetLevelInstanced(int, bool);
    /// get instancing for level of detail
    bool GetLevelInstanced(int);
    /// set max material level to use
    void SetLevelMaterialProfile(int, int);
    /// get max material level to use
    int GetLevelMaterialProfile(int);
    /// set level drawn always on top
    void SetLevelAlwaysOnTop(int, bool);
    /// get level drawn always on top
    bool GetLevelAlwaysOnTop(int);

private:
    /// structure to collect all 
    struct LevelClass
    {
        /// constructor
        LevelClass();

        /// the level is not to be persisted
        bool empty;
        /// range min distance
        float minDistance;
        /// range max distance
        float maxDistance;
        /// material profile
        int materialProfile;
        /// perform geometry instancing
        bool instanceEnabled;
        /// geometry draw on top
        bool alwaysOnTop;

        /// scene hierarchies
        nArray<nRef<nSceneNode> > refRootNodes;
    };
    
    /// information on every level of detail
    nArray<LevelClass> levelClasses;
};

//------------------------------------------------------------------------------
/**
*/
inline
int
ncSceneLodClass::GetNumLevels()
{
    return levelClasses.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
ncSceneLodClass::LevelClass::LevelClass() :
    empty(true),
    minDistance(0),
    maxDistance(0),
    materialProfile(0),
    instanceEnabled(false),
    alwaysOnTop(false),
    refRootNodes(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
#endif
