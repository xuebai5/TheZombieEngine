#include "precompiled/pchframework.h"
//------------------------------------------------------------------------------
//  nlevelmanager_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nlevel/nlevelmanager.h"
#include "nlevel/nlevel.h"

#include "kernel/nkernelserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/nautoref.h"

#include "entity/nentityobjectserver.h"
#include "entity/nentityclassserver.h"
#include "kernel/nlogclass.h"
#include "kernel/ndependencyserver.h"

nNebulaScriptClass(nLevelManager, "nroot");

nLevelManager* nLevelManager::Singleton = 0;

static const char *levelsRoot = "/usr/levels";

NSIGNAL_DEFINE(nLevelManager, LevelCreated);
NSIGNAL_DEFINE(nLevelManager, LevelLoaded);
NSIGNAL_DEFINE(nLevelManager, LevelSaved);
NSIGNAL_DEFINE(nLevelManager, LevelUnload);

NCREATELOGLEVEL(nLevelManager, "Level Manager", true, 2)

//------------------------------------------------------------------------------
/**
*/
nLevelManager::nLevelManager() 
#ifndef NGAME
    :
    // TEMP! replace if a different layer manager is required
    refLayerManager("/sys/servers/layermanager")
#endif
{
    n_assert(Singleton == 0);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
nLevelManager::~nLevelManager()
{
    // unload current level
    this->UnloadLevel();

    nAutoRef<nRoot> refLevels(levelsRoot);
    if (refLevels.isvalid())
    {
        refLevels->Release();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nLevelManager::SetCurrentLevel(nLevel *level)
{
    n_assert(level);
    this->refCurrentLevel = level;
}

//------------------------------------------------------------------------------
/**
*/
void
nLevelManager::SetLevelResource(const char *filename, bool refreshObjects = true)
{
    n_assert(filename);

    this->levelResource = filename;

    nString levelPath(filename);
    levelPath.StripExtension();

    nEntityObjectServer *entityServer = nEntityObjectServer::Instance();
    entityServer->SetConfigDir(nString(levelPath + "/config").Get());
    if (refreshObjects)
    {
        entityServer->LoadConfig();
    }
    entityServer->SetEntityObjectDir( levelPath.Get() );

    // set the "level:" assign to simplify asset management
    kernelServer->GetFileServer()->SetAssign("level", levelPath.Get());
}

//------------------------------------------------------------------------------
/**
*/
nLevel*
nLevelManager::GetCurrentLevel()
{
    if (this->refCurrentLevel.isvalid())
    {
        return this->refCurrentLevel.get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nLevelManager::GetCurrentLevelObject()
{
    if (this->refCurrentLevel.isvalid())
    {
        return this->refCurrentLevel.get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
nObject*
nLevelManager::FindLevel(const char *name)
{
    nAutoRef<nRoot> refLevels(levelsRoot);
    if (refLevels.isvalid())
    {
        return refLevels->Find(name);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
nLevelManager::LoadLevel(const char *filename)
{
    nTime time = nTimeServer::Instance()->GetTime();
    NLOG(ncloader, (1, "Begin load Level %s", filename ));


    nRoot* levels = kernelServer->Lookup(levelsRoot);
    if (!levels)
    {
        levels = kernelServer->New("nroot", levelsRoot);
        n_assert(levels);
    }

    if (!kernelServer->GetFileServer()->FileExists(filename))
    {
        return false;
    }

    // unload current level
    this->UnloadLevel();

    // set entities directories
    this->SetLevelResource(filename);

    // load the level
    kernelServer->PushCwd(levels);
    nLevel* level = (nLevel*) kernelServer->Load(filename);
    kernelServer->PopCwd();
    n_assert(level);

#ifndef NGAME
    // set layer manager for entities before actually loading them
    level->SetEntityLayerManager(this->refLayerManager);

    // load entity layers
    level->LoadEntityLayers();
#endif

    // make level current
    this->SetCurrentLevel(level);

    // perform loading of persisted boot entities
    level->LoadEntityResources();

    // signal that a level has just been loaded
    this->SignalLevelLoaded(this);

    time = nTimeServer::Instance()->GetTime() - time;
    NLOG(ncloader, (1, "End load Level %s , total time %f", filename , time ));

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nLevelManager::NewLevel(const char *filename)
{
    // unload current level
    this->UnloadLevel();

    // set entities directories
    this->SetLevelResource(filename);

    nString levelName(filename);
    levelName = levelName.ExtractFileName();
    levelName.StripExtension();

    nLevel *newLevel = this->CreateLevel("nlevel", levelName);
    n_assert(newLevel);

#ifndef NGAME
    // set layer manager for entities before actually loading them
    newLevel->SetEntityLayerManager(this->refLayerManager);

    // create default layer
    newLevel->LoadEntityLayers();
#endif



    // make level current
    this->SetCurrentLevel(newLevel);

    // signal that a level has just been loaded
    // TODO create specific signal for newly created level
    this->SignalLevelCreated(this);
}

//------------------------------------------------------------------------------
/**
*/
nLevel*
nLevelManager::CreateLevel(const nString& className, const nString& name)
{
    nRoot* levels = kernelServer->Lookup(levelsRoot);
    if (!levels)
    {
        levels = kernelServer->New("nroot", levelsRoot);
        n_assert(levels);
    }

    // unload current level
    if (this->refCurrentLevel.isvalid())
    {
        this->refCurrentLevel->Release();
    }

    // create a new level with the specified class
    kernelServer->PushCwd(levels);
    nLevel* level = (nLevel*) kernelServer->New(className.Get(), name.Get());
    kernelServer->PopCwd();
    n_assert(level);

    return level;
}

//------------------------------------------------------------------------------
/**
    Save current level to file.
*/
bool
nLevelManager::SaveLevel()
{
    nDependencyServer::InitGlobal(); // Empty the dependencyServer

    n_assert(this->refCurrentLevel.isvalid());
    n_assert(!this->levelResource.IsEmpty());

    // save entity resources
    this->refCurrentLevel->SaveEntityResources();

    // save level file
    this->refCurrentLevel->SaveAs(this->levelResource.Get());

    // persist current entity system
    nEntityClassServer::Instance()->SaveEntityClasses();
    nEntityObjectServer::Instance()->SaveEntityObjects();
    nEntityObjectServer::Instance()->SaveConfig();
    nDependencyServer::InitGlobal(); // Empty the dependencyServer

    this->SignalLevelSaved(this);
    
    return true;
}

//------------------------------------------------------------------------------
/**
    Delete a level along with all of its files and folder.
*/
bool
nLevelManager::DeleteLevel(const char* filename)
{
    nFileServer2* fileServer = kernelServer->GetFileServer();
    nString levelFile(filename);

    // check that the file exists
    if (!fileServer->FileExists(levelFile))
    {
        return false;
    }

    // check that the level to delete is not the current
    nString levelName(levelFile.ExtractFileName());
    levelName.StripExtension();
    if (this->refCurrentLevel.isvalid())
    {
        if (nString(this->refCurrentLevel->GetName()) == levelName)
        {
            return false;
        }
    }

    // delete the level folder
    nString levelPath(levelFile);
    levelPath.StripExtension();
    if (!fileServer->DeleteDirectoryRecursive(levelPath.Get()))
    {
        return false;
    }

    // and the level file
    if (!fileServer->DeleteFile(levelFile))
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    Unload current level.
*/
void
nLevelManager::UnloadLevel()
{
    
    // signal that a level it's been unload
    this->SignalLevelUnload(this);

    if (this->refCurrentLevel.isvalid())
    {
        this->refCurrentLevel->UnloadEntityResources();
        this->refCurrentLevel->Release();
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
*/
bool
nLevelManager::SaveCurrentLevelAs(const char * newLevelFullPath)
{
    n_assert_return2(newLevelFullPath, false, "No new level name supplied");

    n_assert_return2(this->refCurrentLevel.isvalid(), false, "Invalid ref to current level");
    
    n_assert_return2(!this->levelResource.IsEmpty(), false, "No level resource found.");

    nString currentLevelPath = this->levelResource;
    /// remove the .n2 suffix to get the current level directory path
    currentLevelPath.StripExtension();

    nString destinationLevelPath = newLevelFullPath;
    /// remove the .n2 suffix to get the destination directory path
    destinationLevelPath.StripExtension();

    /// copy all the files in the current level directory to the destination path
    nFileServer2* fileServer = kernelServer->GetFileServer();
    n_assert_return2(fileServer, false, "Unable to access file server");

    bool directoryCopiedOK = fileServer->CopyDirectoryRecursively(currentLevelPath, destinationLevelPath);
    n_assert_return2(directoryCopiedOK, false, "Unable to copy level directory");

    /// get the actual level name from the full path
    nString newLevelName = newLevelFullPath;
    newLevelName = newLevelName.ExtractFileName();
    newLevelName.StripExtension();

    /// change the name of the current level to the new level name
    this->refCurrentLevel->SetName(newLevelName.Get());

    /// set the current level to use the new .n2 file
    this->SetLevelResource(newLevelFullPath, false);
  
    bool savedLevelOK = this->SaveLevel();
    n_assert_return2(savedLevelOK, false, "Unable to save level to resource file.");

    return true;
}
#endif