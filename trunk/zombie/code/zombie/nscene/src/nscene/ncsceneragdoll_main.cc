#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncsceneragdoll_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncsceneragdoll.h"
#include "nscene/ncsceneragdollclass.h"
#include "animcomp/nccharacter.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSceneRagdoll,ncSceneLod);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneRagdoll)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSceneRagdoll::ncSceneRagdoll() :
    ragdollLevels(MaxRagdolls),
    characterComp(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneRagdoll::~ncSceneRagdoll()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneRagdoll::InitInstance(nObject::InitInstanceMsg initType)
{
    ncSceneLod::InitInstance(initType);
    this->characterComp = this->GetComponentSafe<ncCharacter>();
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneRagdoll::Load()
{
    // load LOD levels as usual
    if (!ncSceneLod::Load())
    {
        return false;
    }

    // load ragdoll levels as additional detail levels
    ncSceneRagdollClass* ragdollClass = this->GetClassComponentSafe<ncSceneRagdollClass>();
    int numRagdollLevels = ragdollClass->GetNumRagdollLevels();

    //UGLY HACK- see ticket #2374
    this->numDetailLevels -= numRagdollLevels;

    int level;
    for (level = 0; level < numRagdollLevels; ++level)
    {
        for( int i=0; i< ragdollClass->GetNumRagdollRoots(level); i++)
        {
            nSceneNode* rootNode = ragdollClass->GetRagdollRoot(level, i);
            n_assert(rootNode);

            rootNode->EntityCreated(this->GetEntityObject());
            rootNode->PreloadResources();

            int ragdollLevel = this->numDetailLevels + level;
            //int ragdollLevel = this->levelRootNodes.Size();

            //duplicated code from ncscenelod
            //bool found = false;
            //for( int i=0; !found && i< this->levelRootNodes.Size(); i++)
            //{
            //    int key = this->levelRootNodes.GetKeyAt(i);
            //    if(key == level)
            //    {
            //        found = true;
            //    }
            //}
            //if there's some rootnodes for that level
            //if( found)
            //{
            //    nArray<nSceneNode*>& scenenodes = this->levelRootNodes.GetElement(level);
            //    scenenodes.Append( rootNode );
            //}
            //else //the first root node for that level
            //{
                //nArray<nSceneNode*> scenenodes;
                //scenenodes.Append(rootNode);
                //this->levelRootNodes.Add(ragdollLevel, scenenodes);
            if( !this->ragdollLevels.HasKey(level))
            {
                this->ragdollLevels.Add(level, ragdollLevel);
            }
            //}
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneRagdoll::Unload()
{
    // unload ragdoll levels first
    this->ragdollLevels.Clear();

    // unload detail levels as usual, including ragdolls
    ncSceneLod::Unload();
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneRagdoll::IsValid()
{
    return ncSceneLod::IsValid();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneRagdoll::Render(nSceneGraph *sceneGraph)
{
    // TODO select the right resource for the current ragdoll, if any
    //ncRagdoll* ragdollComp = ...
    if (this->characterComp->GetRagdollActive())
    {
        int curLevel;
        if (this->ragdollLevels.Find(0, curLevel))
        {
            for( int i=0; i< this->levelRootNodes.GetElementAt(curLevel).Size(); i++)
            {
                this->refRootNode = this->levelRootNodes.GetElementAt(curLevel)[i]; //only one ragdoll mesh for level
                n_assert(this->refRootNode.isvalid());
                this->SetAttachIndex(curLevel);
                sceneGraph->Attach(this->GetEntityObject());
            }
        }
        return;
    }
    ncSceneLod::Render(sceneGraph);
}
