#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  ncscenelod_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/ncscenelod.h"
#include "nscene/ncscenelodclass.h"
#include "nscene/nscenegraph.h"

#include "animcomp/nccharacter.h"
#include "entity/nrefentityobject.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSceneLod,ncScene);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSceneLod)
    NSCRIPT_ADDCMD_COMPOBJECT('AADJ', void, SetLockedLevel, 2, (bool,int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('AADK', int, GetLockedLevel, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
ncSceneLod::ncSceneLod() :
    levelRootNodes(MaxLevels),
    numDetailLevels(0),
    lockedLod(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ncSceneLod::~ncSceneLod()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLod::InitInstance(nObject::InitInstanceMsg initType)
{
    ncScene::InitInstance(initType);
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneLod::Load()
{
    // try to load class resource if possible
    ncScene::Load();
    
    // initialize root node for all levels
    ncSceneLodClass* lodClass = this->GetClassComponent<ncSceneLodClass>();
    int numLevels = lodClass->GetNumLevels();
    for (int level = 0; level < numLevels; level++)
    {
        for (int i = 0; i < lodClass->GetNumLevelRoots(level); i++)
        {
            nSceneNode* rootNode = lodClass->GetLevelRoot(level, i);
            n_assert(rootNode);

            rootNode->EntityCreated(this->GetEntityObject());
            rootNode->PreloadResources();

            bool found = false;
            for (int i=0; !found && i< this->levelRootNodes.Size(); i++)
            {
                int key = this->levelRootNodes.GetKeyAt(i);
                if(key == level)
                {
                    found = true;
                }
            }
            //if there's some rootnodes for that level
            if (found)
            {
                nArray<nSceneNode*>& scenenodes = this->levelRootNodes.GetElement(level);
                scenenodes.Append(rootNode);
            }
            else //the first root node for that level
            {
                nArray<nSceneNode*> scenenodes;
                scenenodes.Append(rootNode);
                this->levelRootNodes.Add(level, scenenodes);
            }
        }
    }

    //UGLY HACK! this is to avoid clashing with ragdoll levels (see ticket #2374)
    this->numDetailLevels = this->levelRootNodes.Size();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLod::Unload()
{
    // unload inconditionally class resources
    if (this->GetNumLevels() == 0)
    {
        ncScene::Unload();
        return;
    }

    // unload all detail levels, even the ones for subclasses
    int numLevels = this->levelRootNodes.Size();
    for (int i = 0; i < numLevels; i++)
    {
        nArray<nSceneNode*>& rootArray = this->levelRootNodes.GetElementAt(i);
        for (int j = 0; j < rootArray.Size(); j++)
        {
            rootArray[j]->EntityDestroyed(this->GetEntityObject());
        }
    }

    this->localVarArray.Reset();
    this->levelRootNodes.Clear();
    this->numDetailLevels = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSceneLod::IsValid()
{
    if (this->numDetailLevels == 0)
    {
        return ncScene::IsValid();
    }

    // simply assert that some level has been loaded (check for all?)
    return (this->numDetailLevels > 0);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSceneLod::Render(nSceneGraph *sceneGraph)
{
    // check for hidden entity
    if (this->GetHiddenEntity())
    {
        return;
    }

    //HACK- update transform now if attached
    if (this->GetParentEntity())
    {
        this->UpdateParentTransform();

        //UGLY- prevent attached entities from being rendered except by their parent
        ncScene* sceneComp = this->GetParentEntity()->GetComponent<ncScene>();
        if (sceneComp && sceneComp->GetFrameId() < this->GetFrameId())
        {
            this->SetFrameId(sceneComp->GetFrameId());
            return;
        }
    }

    // default behavior if there are no levels
    if (this->numDetailLevels == 0)
    {
        ncScene::Render(sceneGraph);
        return;
    }

    ncSceneLodClass* lodClass = this->GetClassComponent<ncSceneLodClass>();

    // TODO- move to shared ::DoRender method
    this->SortLinks();

    // computed in visibility test, copied in nXXXViewport
    int curLevel = this->GetAttachIndex();

    // assign special lod level
    if (this->GetLockedLevel() != -1)
    {
        curLevel = this->lockedLod;
    }

    // tweak visible flag for AlwaysOnTop levels
    //if (lodClass->GetLevelAlwaysOnTop(curLevel))
    //{
    //    this->attachFlags |= AlwaysOnTop;
    //}

    if (curLevel != -1 && curLevel < this->numDetailLevels)
    {
        // TEMPORARY this will work for single viewport ONLY
        // also, it is quite inefficient, find a better alternative
        //float minDist, maxDist;
        //this->lodClass->GetLevelRange(curLevel, minDist, maxDist);
        //this->shaderOverrides.SetArg(nShaderState::MinDist, nShaderArg(minDist));
        //this->shaderOverrides.SetArg(nShaderState::MaxDist, nShaderArg(maxDist));
        
        #ifndef NGAME
        if (lodClass && lodClass->GetLastEditedTime() > this->lastTimeRendered)
        {
            //apply edited class properties
            this->SetMaxMaterialLevel(lodClass->GetLevelMaterialProfile(curLevel));
            this->shaderOverrides.SetParams(sceneClass->GetShaderOverrides());
            this->lastTimeRendered = this->GetTime();
        }
        #endif //NGAME

        const nArray<nSceneNode*> &rootnodes = this->levelRootNodes.GetElementAt(curLevel);
        int numRoots = rootnodes.Size();
        for (int i = 0; i < numRoots; ++i)
        {
            this->refRootNode = rootnodes[i];
            n_assert(this->refRootNode.isvalid());
            this->SetAttachIndex(curLevel);
            sceneGraph->Attach(this->GetEntityObject());
        }

        // handle rendering of children entities if not rendered this frame yet
        this->RenderAttachedEntities(sceneGraph);
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
ncSceneLod::SetLockedLevel(bool lock, int lockedLevel)
{
    if (!lock)
    {
        this->lockedLod = -1;
    }
    else
    {
        this->lockedLod = lockedLevel;
    }
}

//------------------------------------------------------------------------------
/**
*/
int 
ncSceneLod::GetLockedLevel() const
{
   return this->lockedLod;
}
