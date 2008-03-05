#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncsceneragdollclass.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncsceneragdollclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSceneRagdollClass,ncSceneLodClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneRagdollClass)
    NSCRIPT_ADDCMD_COMPCLASS('AACE', void, SetRagdollRoot, 2, (int, nSceneNode*), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AACF', nSceneNode* , GetRagdollRoot, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('AADY', int , GetNumRagdollRoots, 1, (int), 0, ());
NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
*/
ncSceneRagdollClass::ncSceneRagdollClass() :
    ragdollIndices(0, NumRagdolls)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneRagdollClass::~ncSceneRagdollClass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneRagdollClass::SetRagdollRoot(int level, nSceneNode* rootNode)
{
    int ragLevel = 0;
    if (level < this->ragdollIndices.Size())
    {
        ragLevel = this->ragdollIndices.At(level);
    }
    else
    {
        ragLevel = this->GetNumLevels();
        this->ragdollIndices.Set(level, ragLevel);
    }

    this->SetLevelRoot(ragLevel, rootNode);
}

//------------------------------------------------------------------------------
/**
*/
nSceneNode*
ncSceneRagdollClass::GetRagdollRoot(int level, int resourceIdx)
{
    if (level < this->ragdollIndices.Size())
    {
        int ragLevel = this->ragdollIndices.At(level);
        return this->GetLevelRoot(ragLevel, resourceIdx);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
ncSceneRagdollClass::GetNumRagdollRoots(int level)
{
    if (level < this->ragdollIndices.Size())
    {
        int ragLevel = this->ragdollIndices.At(level);
        return this->GetNumLevelRoots(ragLevel);
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneRagdollClass::SaveCmds(nPersistServer* ps)
{
    return ncSceneLodClass::SaveCmds(ps);
}
