#ifndef N_LEVELMANAGER_H
#define N_LEVELMANAGER_H
//------------------------------------------------------------------------------
/**
    @class nLevelManager
    @ingroup NebulaLevelSystem

    @brief The LevelManager is the singleton object responsible for
    managing levels, and their related objects and resources.
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"

class nLevel;

#ifndef NGAME
class nLayerManager;
#endif

//------------------------------------------------------------------------------
class nLevelManager : public nRoot
{
public:
    /// constructor
    nLevelManager();
    /// destructor
    virtual ~nLevelManager();
    /// singleton
    static nLevelManager* Instance();

    // Script commands

        /// create a new, empty level
        void NewLevel(const char *);
        /// save current level to file
        bool SaveLevel();
        /// load level object from file
        bool LoadLevel(const char *);
        /// delete all files and folders for a level
        bool DeleteLevel(const char *);
        /// find level object by name
        nObject* FindLevel(const char *);
        /// get current level object
        nObject* GetCurrentLevelObject();
        /// set resource file for current level, passing flag to show if should refresh objects
        void SetLevelResource(const char *, bool);
        /// get resource file for current level
        const char * GetLevelResource();

    NSIGNAL_DECLARE('MLCD', void, LevelCreated, 0, (), 0, ());
    NSIGNAL_DECLARE('MLLD', void, LevelLoaded, 0, (), 0, ());
    NSIGNAL_DECLARE('MLSD', void, LevelSaved, 0, (), 0, ());
    NSIGNAL_DECLARE('MLUD', void, LevelUnload, 0, (), 0, ());

    // set current level
    void SetCurrentLevel(nLevel* l);
    // get current level
    nLevel* GetCurrentLevel();

    /// create a new level with the specified class (FIXME move to level builder)
    nLevel* CreateLevel(const nString& className, const nString& name);
    /// unload current level
    void UnloadLevel();

#ifndef NGAME
    /// save a copy of the current level under a different name
    bool SaveCurrentLevelAs(const char * newLevelFullPath);
#endif

private:
    static nLevelManager* Singleton;
    nRef<nLevel> refCurrentLevel;
    nString levelResource;
#ifndef NGAME
    nAutoRef<nLayerManager> refLayerManager;
#endif
};

//------------------------------------------------------------------------------
/**
*/
inline
nLevelManager*
nLevelManager::Instance()
{
    n_assert(Singleton);
    return Singleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nLevelManager::GetLevelResource()
{
    return this->levelResource.IsEmpty() ? 0 : this->levelResource.Get();
}

//------------------------------------------------------------------------------
#endif
