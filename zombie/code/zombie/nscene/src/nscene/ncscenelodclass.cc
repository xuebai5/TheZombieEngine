#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncscenelodclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncscenelodclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSceneLodClass,ncSceneClass);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneLodClass)

    NSCRIPT_ADDCMD_COMPCLASS('MSLN', void, SetLevelRoot, 2, (int, nSceneNode*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLN', nSceneNode*, GetLevelRoot, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AAEA', int, GetNumLevelRoots, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSLR', void, SetLevelRange, 3, (int, float, float), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLR', void, GetLevelRange, 1, (int), 2, (float&, float&));
    NSCRIPT_ADDCMD_COMPCLASS('MSLI', void, SetLevelInstanced, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLI', bool, GetLevelInstanced, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSLW', void, SetLevelAlwaysOnTop, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLW', bool, GetLevelAlwaysOnTop, 1, (int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MSLP', void, SetLevelMaterialProfile, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('MGLP', int, GetLevelMaterialProfile, 1, (int), 0, ());

NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSceneLodClass::ncSceneLodClass() :
    levelClasses(0, 4)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneLodClass::~ncSceneLodClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLodClass::SetLevelRoot(int level, nSceneNode* rootNode)
{
    LevelClass& levelClass = this->levelClasses.At(level);
    //check that same nodes are not added twice
    if (!levelClass.refRootNodes.Find(rootNode))
    {
        levelClass.refRootNodes.Append(rootNode);
    }
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode*
ncSceneLodClass::GetLevelRoot(int level, int resourceIdx)
{
    if (level < this->GetNumLevels())
    {
        LevelClass& levelClass = this->levelClasses.At(level);
        if (resourceIdx < levelClass.refRootNodes.Size())
        {
            if (levelClass.refRootNodes[resourceIdx].isvalid())
            {
                return levelClass.refRootNodes[resourceIdx].get();
            }
            //cleanup invalid references
            levelClass.refRootNodes.Erase(resourceIdx);
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
ncSceneLodClass::GetNumLevelRoots(int level)
{
    if (level < this->GetNumLevels())
    {
        LevelClass& levelClass = this->levelClasses.At(level);
        return levelClass.refRootNodes.Size();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLodClass::SetLevelRange(int level, float minDist, float maxDist)
{
    LevelClass& levelClass = this->levelClasses.At(level);
    levelClass.minDistance = minDist;
    levelClass.maxDistance = maxDist;
    levelClass.empty = false;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLodClass::GetLevelRange(int level, float& minDist, float& maxDist)
{
    if (level < this->GetNumLevels())
    {
        LevelClass& levelClass = this->levelClasses.At(level);
        minDist = levelClass.minDistance;
        maxDist = levelClass.maxDistance;
    }
    else
    {
        minDist = 0;
        maxDist = 0;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLodClass::SetLevelInstanced(int level, bool instanced)
{
    LevelClass& levelClass = this->levelClasses.At(level);
    levelClass.instanceEnabled = instanced;
    levelClass.empty = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneLodClass::GetLevelInstanced(int level)
{
    if (level < this->GetNumLevels())
    {
        LevelClass& levelClass = this->levelClasses.At(level);
        return levelClass.instanceEnabled;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLodClass::SetLevelMaterialProfile(int level, int index)
{
    LevelClass& levelClass = this->levelClasses.At(level);
    levelClass.materialProfile = index;
    levelClass.empty = false;
    #ifndef NGAME
    this->SetLastEditedTime(nTimeServer::Instance()->GetFrameTime());
    #endif //NGAME
}

//------------------------------------------------------------------------------
/**
*/
int
ncSceneLodClass::GetLevelMaterialProfile(int level)
{
    if (level < this->GetNumLevels())
    {
        LevelClass& levelClass = this->levelClasses.At(level);
        return levelClass.materialProfile;
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLodClass::SetLevelAlwaysOnTop(int level, bool value)
{
    LevelClass& levelClass = this->levelClasses.At(level);
    levelClass.alwaysOnTop = value;
    levelClass.empty = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneLodClass::GetLevelAlwaysOnTop(int level)
{
    if (level < this->GetNumLevels())
    {
        LevelClass& levelClass = this->levelClasses.At(level);
        return levelClass.alwaysOnTop;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
*/
int
ncSceneLodClass::SelectLevelOfDetail(float viewerDist)
{
    n_assert(this->GetNumLevels() > 0);
    int level = 0;
    while (level < this->GetNumLevels())
    {
        LevelClass& levelClass = this->levelClasses.At(level);
        if (!levelClass.empty &&
            viewerDist >= levelClass.minDistance &&
            viewerDist < levelClass.maxDistance)
        {
            return level;
        }

        ++level;
    }

    return -1;
}

//------------------------------------------------------------------------------
/**
    history:
        - 13-Mar-2006   ma.garcias  Added preloading of class resources
*/
bool
ncSceneLodClass::LoadResources()
{
    if (this->GetNumLevels() == 0)
    {
        return ncSceneClass::LoadResources();
    }

    // load instanced scene resource for all levels
    int numLevels = this->GetNumLevels();
    for (int level = 0; level < numLevels; ++level)
    {
        // preload class resources
        LevelClass& levelClass = this->levelClasses.At(level);
        for (int index = 0; index < levelClass.refRootNodes.Size(); ++index)
        {
            if (levelClass.refRootNodes[index].isvalid())
            {
                levelClass.refRootNodes[index]->PreloadResources();
            }
        }

        if (!levelClass.empty && levelClass.instanceEnabled)
        {
            /// @todo load instance stream -should be the same for all lods!
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLodClass::UnloadResources()
{
    if (this->GetNumLevels() == 0)
    {
        ncSceneClass::UnloadResources();
    }

    // clear root nodes from all levels, keep own data
    int numLevels = this->GetNumLevels();
    for (int level = 0; level < numLevels; ++level)
    {
        LevelClass& levelClass = this->levelClasses.At(level);
        levelClass.refRootNodes.Clear();
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneLodClass::IsValid()
{
    if (this->GetNumLevels() == 0)
    {
        return ncSceneClass::IsValid();
    }
    
    // check if resources are valid for all detail levels
    int numLevels = this->GetNumLevels();
    for (int level = 0; level < numLevels; level++)
    {
        if (this->GetNumLevelRoots(level) == 0)
        {
            return false;
        }

        for (int index = 0; index < this->GetNumLevelRoots(level); ++index)
        {
            if (!this->GetLevelRoot(level, index) && !ncSceneClass::IsValid())
            {
                return false;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLodClass::Invalidate()
{
    if (this->GetNumLevels() == 0)
    {
        ncSceneClass::Invalidate();
        return;
    }

    // invalidate resources for all detail levels
    int numLevels = this->GetNumLevels();
    for (int level = 0; level < numLevels; level++)
    {
        LevelClass& levelClass = this->levelClasses.At(level);
        for (int index = 0; index < levelClass.refRootNodes.Size(); ++index)
        {
            levelClass.refRootNodes[index].invalidate();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneLodClass::SaveCmds(nPersistServer* ps)
{
    if (ncSceneClass::SaveCmds(ps))
    {
        if (this->GetNumLevels() > 0)
        {
            for (int level = 0; level < this->GetNumLevels(); level++)
            {
                nCmd* cmd;
                
                LevelClass& levelClass = this->levelClasses.At(level);
                if (!levelClass.empty)
                {
                    // --- setlevelrange ---
                    cmd = ps->GetCmd(this->GetEntityClass(), 'MSLR');
                    cmd->In()->SetI(level);
                    cmd->In()->SetF(levelClass.minDistance);
                    cmd->In()->SetF(levelClass.maxDistance);
                    ps->PutCmd(cmd);

                    // --- setlevelinstanced ---
                    cmd = ps->GetCmd(this->GetEntityClass(), 'MSLI');
                    cmd->In()->SetI(level);
                    cmd->In()->SetB(levelClass.instanceEnabled);
                    ps->PutCmd(cmd);

                    // --- setlevelmaterialprofile ---
                    cmd = ps->GetCmd(this->GetEntityClass(), 'MSLP');
                    cmd->In()->SetI(level);
                    cmd->In()->SetI(levelClass.materialProfile);
                    ps->PutCmd(cmd);

                    // --- setlevelalwaysontop ---
                    cmd = ps->GetCmd(this->GetEntityClass(), 'MSLW');
                    cmd->In()->SetI(level);
                    cmd->In()->SetB(levelClass.alwaysOnTop);
                    ps->PutCmd(cmd);
                }
            }
        }

        return true;
    }

    return false;
}
