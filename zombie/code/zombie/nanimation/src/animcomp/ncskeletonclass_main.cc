#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  ncskeletonclass_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "anim2/nanimationserver.h"
#include "animcomp/ncskeletonclass.h"
#include "animcomp/nccharacterclass.h"
#include "animcomp/nccharacter.h"
#include "entity/nentityclassserver.h"
#include "zombieentity/ncassetclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncSkeletonClass,nComponentClass);

//------------------------------------------------------------------------------
/**
*/
ncSkeletonClass::ncSkeletonClass():
    dynamicAttachmentPool(0,0),
    chunkLoaded(false),
    futureSubmissiveNames(0,1),
    futureLeaderNames(0,1)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
ncSkeletonClass::~ncSkeletonClass()
{
    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::InitInstance(nObject::InitInstanceMsg initType)
{
    // Because in the load resources needs a nanimationserver
    if (initType != nObject::ReloadedInstance)
    {
        // not load resources for native class (neskeleton)
        if (!this->GetEntityClass()->IsNative())
        {
            this->LoadResources();
        }
    }
}

//------------------------------------------------------------------------------
/**
    Unload the animation resource file.
*/
void
ncSkeletonClass::UnloadAnims()
{
    int stateIndex;
    for (stateIndex = 0; stateIndex < this->GetNumStates(); stateIndex++)
    {
        if (this->animStateArray.GetStateAt(stateIndex).IsAnimValid())
        {
            this->animStateArray.GetStateAt(stateIndex).GetAnim()->Release();
            this->animStateArray.GetStateAt(stateIndex).InvalidateAnim();
        }
    }

}

//------------------------------------------------------------------------------
/**
    Load the animation resource file.
*/
bool
ncSkeletonClass::LoadAnims()
{
    int stateIndex;
    for (stateIndex = 0; stateIndex < this->GetNumStates(); stateIndex++)
    {
        nAnimState& animState = this->animStateArray.GetStateAt(stateIndex);
        if ((!animState.IsAnimValid()) && (animState.GetAnimFile()))
        {
            const char* fileName = animState.GetAnimFile();
            nAnimation* anim = nAnimationServer::Instance()->NewMemoryAnimation(fileName);
            n_assert(anim);
            if (!anim->IsValid())
            {
                anim->SetFilename(fileName);
                if (!anim->Load())
                {
                    n_printf("nSkinAnimator: Error loading anim file '%s'\n", fileName);
                    anim->Release();
                    return false;
                }
            }
            animState.SetAnim(anim);
        }
    }
    this->character.SetAnimStateArray(&this->animStateArray);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload the motion resource file.
*/
void
ncSkeletonClass::UnloadMotion()
{
    int stateIndex;
    for (stateIndex = 0; stateIndex < this->GetNumStates(); stateIndex++)
    {
        if (this->animStateArray.GetStateAt(stateIndex).IsMotionValid())
        {
            this->animStateArray.GetStateAt(stateIndex).GetMotion()->Release();
            this->animStateArray.GetStateAt(stateIndex).InvalidateMotion();
        }
    }

}

//------------------------------------------------------------------------------
/**
    Load the motion resource file.
*/
bool
ncSkeletonClass::LoadMotion()
{
    int stateIndex;
    for (stateIndex = 0; stateIndex < this->GetNumStates(); stateIndex++)
    {
        nAnimState& animState = this->animStateArray.GetStateAt(stateIndex);
        if ((!animState.IsMotionValid()) && (animState.GetMotionFile()))
        {
            const char* fileName = animState.GetMotionFile();
            nAnimation* anim = nAnimationServer::Instance()->NewMemoryAnimation(fileName);
            n_assert(anim);
            if (!anim->IsValid())
            {
                anim->SetFilename(fileName);
                if (!anim->Load())
                {
                    n_printf("nSkinAnimator: Error loading anim file '%s'\n", fileName);
                    anim->Release();
                    return false;
                }
            }
            animState.SetMotion(anim);
        }
    }
    this->character.SetAnimStateArray(&this->animStateArray);
    return true;
}

//------------------------------------------------------------------------------
/**
    Load the resources needed by this object.
*/
bool
ncSkeletonClass::LoadResources()
{
    this->LoadAnims();
    this->LoadMotion();
    this->LoadChunk();
    this->LoadTweakableJoints();
    return true;
}

//------------------------------------------------------------------------------
/**
    Unload the resources.
*/
void
ncSkeletonClass::UnloadResources()
{
    this->UnloadAnims();
    this->UnloadMotion();
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSkeletonClass::LoadUpperLevelResources()
{
    nString lowLevelSklName = this->GetEntityClass()->GetName();
    lowLevelSklName = lowLevelSklName.ExtractRange(0, lowLevelSklName.Length() - 1);
    lowLevelSklName += "0";

    nEntityClass* entityClass = nEntityClassServer::Instance()->GetEntityClass(lowLevelSklName.Get());
    if (entityClass)
    {
        ncSkeletonClass *skelComp = entityClass->GetComponent<ncSkeletonClass>();
        n_assert(skelComp);
        n_assert(skelComp->GetNumStates() == this->GetNumStates());

        for (int stateIndex = 0; stateIndex < skelComp->GetNumStates(); stateIndex++)
        {
            nAnimState& upperLodAnimState = skelComp->animStateArray.GetStateAt(stateIndex);
            nAnimState& currentLodAnimState = this->animStateArray.GetStateAt(stateIndex);
            
            //copy leader state
            currentLodAnimState.SetLeaderState(upperLodAnimState.GetLeaderState());
            //copy followed joint
            currentLodAnimState.SetFollowJoint(upperLodAnimState.GetFollowJoint());

            //copy motion file
            if( upperLodAnimState.IsMotionValid())
            {
                const char* fileName = upperLodAnimState.GetMotionFile();
                nAnimation* anim = nAnimationServer::Instance()->NewMemoryAnimation(fileName);
                n_assert(anim);
                if (!anim->IsValid())
                {
                    anim->SetFilename(fileName);
                    if (!anim->Load())
                    {
                        n_printf("nSkinAnimator: Error loading anim file '%s'\n", fileName);
                        anim->Release();
                        return false;
                    }
                }
                currentLodAnimState.SetMotion(anim);
            }
        }
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Begin configuring the joint skeleton.
*/
void
ncSkeletonClass::BeginJoints(int numJoints)
{
    this->character.GetSkeleton().Clear();
    this->character.GetSkeleton().BeginJoints(numJoints);
}

//------------------------------------------------------------------------------
/**
    Add a joint to the joint skeleton.
*/
void
ncSkeletonClass::SetJoint(int jointIndex, int& parentJointIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale)
{
    this->character.GetSkeleton().SetJoint(jointIndex, parentJointIndex, poseTranslate, poseRotate, poseScale, "");
}

//------------------------------------------------------------------------------
/**
    Add a joint to the joint skeleton
*/
void
ncSkeletonClass::SetNamedJoint(int jointIndex, int& parentJointIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale, const nString& name)
{
    this->character.GetSkeleton().SetJoint(jointIndex, parentJointIndex, poseTranslate, poseRotate, poseScale, name);
}

//------------------------------------------------------------------------------
/**
    Finish adding joints to the joint skeleton.
*/
void
ncSkeletonClass::EndJoints()
{
    this->character.GetSkeleton().EndJoints();
}

//------------------------------------------------------------------------------
/**
    Get number of joints in joint skeleton.
*/
int
ncSkeletonClass::GetNumJoints()
{
    return this->character.GetSkeleton().GetNumJoints();
}

//------------------------------------------------------------------------------
/**
    Get joint attributes.
*/
void
ncSkeletonClass::GetJoint(int index, int& parentJointIndex, vector3& poseTranslate, quaternion& poseRotate, vector3& poseScale)
{
    nCharJoint& joint = this->character.GetSkeleton().GetJointAt(index);
    parentJointIndex = joint.GetParentJointIndex();
    poseTranslate = joint.GetPoseTranslate();
    poseRotate = joint.GetPoseRotate();
    poseScale = joint.GetPoseScale();
}

//------------------------------------------------------------------------------
/**
    Add a joint name
    
    @param joint  the joint index to name
    @param name   the name to assign to the specified joint
*/
void
ncSkeletonClass::AddJointName(int joint, const char *name)
{
    this->character.GetSkeleton().GetJointAt(joint).SetName(nString(name));
}

//------------------------------------------------------------------------------
/**
    Get a joint index by name.
    
    @param name  the name of the joint index to retrieve.  Returns 65535 if joint not found.
*/
unsigned int
ncSkeletonClass::GetJointByName(const char *name)
{
    int numJoints = this->character.GetSkeleton().GetNumJoints();
    int i=0;
    for (i=0; i< numJoints; i++)
    {
        if (0 == strcmp(this->character.GetSkeleton().GetJointAt(i).GetName().Get(), name))
        {
            return (unsigned int) i;
        }
    }

    return (unsigned int) InvalidIndex;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::BeginStates(int num)
{
    this->animStateArray.Begin(num);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetState(int stateIndex, int animGroupIndex, float fadeInTime)
{
    nAnimState newState;
    newState.SetAnimGroupIndex(animGroupIndex);
    newState.SetFadeInTime(fadeInTime);
    this->animStateArray.SetState(stateIndex, newState);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetStateWithJointGroup(int stateIndex, int animGroupIndex, int jointGroupIndex, float fadeInTime)
{
    nAnimState newState;
    newState.SetAnimGroupIndex(animGroupIndex);
    newState.SetFadeInTime(fadeInTime);
    newState.SetJointGroup(jointGroupIndex);
    this->animStateArray.SetState(stateIndex, newState);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::AddState(int animGroupIndex, int jointGroupIndex, float fadeInTime)
{
    nAnimState newState;
    newState.SetAnimGroupIndex(animGroupIndex);
    newState.SetJointGroup(jointGroupIndex);
    newState.SetFadeInTime(fadeInTime);
    this->animStateArray.AddState(newState);
}
//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetStateName(int stateIndex, const char * name)
{
    this->animStateArray.GetStateAt(stateIndex).SetName(name);
}

//------------------------------------------------------------------------------
/**
    Assign per-state animation file.
*/
void
ncSkeletonClass::SetStateAnim(int stateIndex, const char *filename)
{
    this->animStateArray.GetStateAt(stateIndex).SetAnimFile(filename);
}

//------------------------------------------------------------------------------
/**
    Get per-state animation file.
*/
const char *
ncSkeletonClass::GetStateAnim(int stateIndex) const
{
    return this->animStateArray.GetStateAt(stateIndex).GetAnimFile();
}

//------------------------------------------------------------------------------
/**
    Set per-state animator channel
*/
void
ncSkeletonClass::SetStateAnimChannel(int /*stateIndex*/, const char * /*animChannel*/)
{
    // TODO: allow specifying channel for an external animator (mixer?)
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::EndStates()
{
    this->animStateArray.End();
}

//------------------------------------------------------------------------------
/**
*/
int
ncSkeletonClass::GetNumStates() const
{
    return this->animStateArray.GetNumStates();
}

//------------------------------------------------------------------------------
/**
*/
const nAnimState&
ncSkeletonClass::GetStateAt(int stateIndex) const
{
    return this->animStateArray.GetStateAt(stateIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::BeginClips(int stateIndex, int numClips)
{
    this->animStateArray.GetStateAt(stateIndex).BeginClips(numClips);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetClip(int stateIndex, int clipIndex, const char* weightChannelName)
{
    // number of anim curves in a clip is identical to number of (joints * 3)
    // (one curve for translation, rotation and scale)
    int jointGroup = this->animStateArray.GetStateAt(stateIndex).GetJointGroup();
    int numCurves;
    if (jointGroup == -1)
    {
        numCurves = this->GetNumJoints() *3;
    }
    else
    {
        numCurves= this->character.GetSkeleton().GetNumberCurvesJointGroup(jointGroup);
    }
    n_assert(numCurves > 0);

    // the first anim curve index of this clip is at (numCurves * clipIndex)
    int firstCurve = numCurves * clipIndex;

    // get the variable handle for the weightChannel
    nVariable::Handle varHandle = nVariableServer::Instance()->GetVariableHandleByName(weightChannelName);
    n_assert(nVariable::InvalidHandle != varHandle);

    nAnimClip newClip(firstCurve, numCurves, varHandle);
    this->animStateArray.GetStateAt(stateIndex).SetClip(clipIndex, newClip);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetClipAndCurves(int stateIndex, int clipIndex, int numCurves, const char* weightChannelName)
{
    // the first anim curve index of this clip is at (numCurves * clipIndex)
    int firstCurve = numCurves * clipIndex;

    // get the variable handle for the weightChannel
    nVariable::Handle varHandle = nVariableServer::Instance()->GetVariableHandleByName(weightChannelName);
    n_assert(nVariable::InvalidHandle != varHandle);

    nAnimClip newClip(firstCurve, numCurves, varHandle);
    this->animStateArray.GetStateAt(stateIndex).SetClip(clipIndex, newClip);
}
//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::EndClips(int stateIndex)
{
    this->animStateArray.GetStateAt(stateIndex).EndClips();
}

//------------------------------------------------------------------------------
/**
*/
int
ncSkeletonClass::GetNumClips(int stateIndex)
{
    return this->animStateArray.GetStateAt(stateIndex).GetNumClips();
}

//------------------------------------------------------------------------------
/**
*/
const char*
ncSkeletonClass::GetClipAt(int stateIndex, int clipIndex)
{
    nVariable::Handle varHandle = this->animStateArray.GetStateAt(stateIndex).GetClipAt(clipIndex).GetWeightChannelHandle();
    return nVariableServer::Instance()->GetVariableName(varHandle);
}

//------------------------------------------------------------------------------
/**
*/
const char *
ncSkeletonClass::GetStateName(int stateIndex)
{
    return this->animStateArray.GetStateAt(stateIndex).GetName().Get();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetStateBBox(int stateIndex, vector3& center, vector3& extents)
{
    this->animStateArray.SetStateBoundingBox(stateIndex, center, extents);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::GetStateBBox(int stateIndex, vector3& center, vector3& extents)
{
    bbox3 box;
    this->animStateArray.GetStateBoundingBox(stateIndex, box);
    center = box.center();
    extents = box.extents();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetFadeInTime(int stateIndex, float fadeinTime)
{
    this->animStateArray.GetStateAt(stateIndex).SetFadeInTime(fadeinTime);
    //return this->SaveClass(); //removed because has to be saved when they are sure it's correct
}

//------------------------------------------------------------------------------
/**
*/
float
ncSkeletonClass::GetFadeInTime(int stateIndex)
{
    return this->animStateArray.GetStateAt(stateIndex).GetFadeInTime();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::BeginJointGroups(int numGroups)
{
    this->character.GetSkeleton().BeginJointGroups(numGroups);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetGroupNumberJoints(int groupIndex, int numJoints)
{
    this->character.GetSkeleton().SetGroupNumberJoints(groupIndex, numJoints);
}
//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetGroup(int groupIndex, int joint0, int joint1, int joint2, int joint3/*, int joint4*/)
{
    this->character.GetSkeleton().SetGroup(groupIndex,joint0,joint1,joint2,joint3);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::AssignGroupToState(int stateIndex, int groupIndex)
{
    this->animStateArray.GetStateAt(stateIndex).SetJointGroup(groupIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::EndJointGroups()
{
    this->character.GetSkeleton().EndJointGroups();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::BeginTweakableJoints(int num)
{
    this->character.BeginTweakableJoints(num);
}

//------------------------------------------------------------------------------
/**
    @param  index       index of tweakable joint
    @param  jointIndex  index of skeleton joint
    @param  parentIndex joint Index of parent joint to use as pivot
*/
void
ncSkeletonClass::SetTweakableJoint(int index, int jointIndex, int parentIndex)
{
    this->character.SetTweakableJoint(index, jointIndex, parentIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::EndTweakableJoints()
{
    this->character.EndTweakableJoints();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetPerceptionOffset(int stateIndex, vector3 offset)
{
    this->animStateArray.GetStateAt(stateIndex).SetPerceptionOffset(offset);
}

//------------------------------------------------------------------------------
/**
*/
vector3
ncSkeletonClass::GetPerceptionOffset(int stateIndex) const
{
    return this->animStateArray.GetStateAt(stateIndex).GetPerceptionOffset();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::BeginAttachments(int numAttachments)
{
    this->dynamicAttachmentPool.SetFixedSize(numAttachments);
}
//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetAttachmentHelper(int attIndex, const char* helperName, int jointIndex, vector3 position, quaternion rotation)
{
    this->dynamicAttachmentPool[attIndex] =  nDynamicAttachment(helperName, jointIndex, position, rotation);
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::EndAttachments()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::RemoveAnimState(int stateIndex)
{
    // remove state
    this->animStateArray.RemoveState(stateIndex);
    this->SaveClass();
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSkeletonClass::ChangeAnimStateName(int stateIndex, const nString& newName)
{
    nFileServer2* fileServer = nFileServer2::Instance();
 
    // get state
    nAnimState& animState = this->animStateArray.GetStateAt(stateIndex);
    if ((animState.IsAnimValid()) && (animState.GetAnimFile()))
    {
        nString fileName(animState.GetAnimFile());
        nString newFileName = fileName.ExtractToLastSlash();
        newFileName += newName;
        newFileName += ".";
        newFileName += fileName.GetExtension();

        if (fileServer->FileExists(fileName))
        {
            // release old animation
            animState.GetAnim()->Release();

            // rename file
            if (fileServer->CopyFile(fileName, newFileName.Get()) && 
                fileServer->DeleteFile(fileName))
            {
                // change anim state data
                animState.SetAnimFile(newFileName.Get());
                animState.SetName(newName);

                // load animation
                nAnimation* anim = nAnimationServer::Instance()->NewMemoryAnimation(newFileName.Get());
                n_assert(anim);
                if (!anim->IsValid())
                {
                    anim->SetFilename(newFileName);
                    if (!anim->Load())
                    {
                        n_printf("ncSkeletonClass: Error loading anim file '%s'\n", newFileName);
                        anim->Release();
                        return false;
                    }
                }
                animState.SetAnim(anim);

                return this->SaveClass();
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
const nString
ncSkeletonClass::GetJointName(int jointIndex)
{
    return this->character.GetSkeleton().GetJointAt(jointIndex).GetName();
}

//------------------------------------------------------------------------------
/**
*/
#ifdef __ZOMBIE_EXPORTER__
bool
ncSkeletonClass::CleanData()
{
    this->UnloadAnims();

    this->animStateArray.ClearArray();
    this->character.GetSkeleton().Clear();

    return true;
}
#endif

//------------------------------------------------------------------------------
/**
*/
bool
ncSkeletonClass::SaveClass()
{
    // set class dirty to force persistence
    nEntityClassServer::Instance()->SetEntityClassDirty(this->entityClass, true);

    this->SaveChunk();

    //UGLY TRICK remove when saving entities independently from level
    return nEntityClassServer::Instance()->SaveEntityClass(entityClass);
}

//------------------------------------------------------------------------------
/**
*/
const nDynamicAttachment&
ncSkeletonClass::GetDynAttachment(int index)
{
    return this->dynamicAttachmentPool[index];
}

//------------------------------------------------------------------------------
/**
*/
nArray<nDynamicAttachment>
ncSkeletonClass::GetDynAttachments()
{
    return this->dynamicAttachmentPool;
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetSubmissiveLeaderState(const nString& submissiveState, const nString& leaderState)
{    
    n_assert_return3( this->character.GetAnimStateArray(),, ("Animations not found for skeleton %s", this->GetEntityClass()->GetClass()->GetName()) );
    int submissiveStateIdx = this->character.GetAnimStateArray()->FindStateIndex(submissiveState);
    int leaderStateIdx = this->character.GetAnimStateArray()->FindStateIndex(leaderState);

    //there's the possibility that that state still doesn't exist but existed in an older exportation
    if (submissiveStateIdx != -1)
    {
        this->animStateArray.GetStateAt(submissiveStateIdx).SetLeaderState(leaderStateIdx);
    }
    else //save data in an array to use when savechunk
    {
        this->futureSubmissiveNames.Append(submissiveState);
        this->futureLeaderNames.Append(leaderState);
    }
}

//------------------------------------------------------------------------------
/**
*/
int
ncSkeletonClass::GetLeaderStateOf(int submissiveState)
{
    return this->animStateArray.GetStateAt(submissiveState).GetLeaderState();
}

//------------------------------------------------------------------------------
/**
*/
const nString
ncSkeletonClass::GetLeaderStateNameOf(int submissiveState)
{
    int leaderIdx = this->animStateArray.GetStateAt(submissiveState).GetLeaderState();
    return this->animStateArray.GetStateAt(leaderIdx).GetName();
}

//------------------------------------------------------------------------------
/**
    Assign per-state motion file.
*/
void
ncSkeletonClass::SetStateMotion(int stateIndex, const char *filename)
{
    this->animStateArray.GetStateAt(stateIndex).SetMotionFile(filename);
}

//------------------------------------------------------------------------------
/**
    Get per-state motion file.
*/
const char *
ncSkeletonClass::GetStateMotion(int stateIndex) const
{
    return this->animStateArray.GetStateAt(stateIndex).GetMotionFile();
}

//------------------------------------------------------------------------------
/**
*/
void
ncSkeletonClass::SetStateFollowsJoint(const nString& stateName, const nString& jointName)
{
    int stateIdx = this->character.GetAnimStateArray()->FindStateIndex(stateName);
    int jointIdx = this->character.GetSkeleton().GetJointIndexByName(jointName);

    //there's the possibility that that state still doesn't exist but existed in an older exportation
    if (stateIdx != -1)
    {
        this->animStateArray.GetStateAt(stateIdx).SetFollowJoint(jointIdx);
    }
}

//------------------------------------------------------------------------------
/**
*/
const nString
ncSkeletonClass::GetStateFollowsJoint(const nString& stateName)
{
	n_assert( this->character.GetAnimStateArray() );
    int stateIdx = this->character.GetAnimStateArray()->FindStateIndex(stateName);
    nString jointName;
 
    if (stateIdx != -1)
    {
        int jointIdx = this->animStateArray.GetStateAt(stateIdx).GetFollowJoint();
        if (jointIdx != -1)
        {
            jointName = this->character.GetSkeleton().GetJointAt(jointIdx).GetName();
        }
    }

    return jointName;
}

//-----------------------------------------------------------------------------
/**
    Loads chunk information.
*/
bool 
ncSkeletonClass::LoadChunk()
{
    if (this->chunkLoaded)
    {
        return true;
    }

    // load resource
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);
    if (ps)
    {
        nString path = this->GetChunkPath();

        if (nFileServer2::Instance()->FileExists(path))
        {
            nKernelServer::Instance()->PushCwd(this->GetEntityClass());
            nObject* loaded = nKernelServer::Instance()->Load(path.Get(), false);
            nKernelServer::Instance()->PopCwd();
            if (loaded)
            {
                this->chunkLoaded = true;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
*/
nString
ncSkeletonClass::GetChunkPath()
{
    ncAssetClass* assetClass = this->GetComponent<ncAssetClass>();

    nString path;
    if (assetClass && assetClass->GetResourceFile())
    {
        path.Append(assetClass->GetResourceFile());
    }
    path.Append("/anims/dependencies/chunk.n2");

    return path;
}

//-----------------------------------------------------------------------------
/**
    Saves chunk information.
*/
bool
ncSkeletonClass::SaveChunk()
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // load resource
    n_assert(ps);
    if (ps)
    {
        nString path = this->GetChunkPath();

        nFileServer2::Instance()->MakePath(path.ExtractDirName());

        nCmd * cmd = ps->GetCmd(this->GetEntityClass(), 'THIS');

        if (ps->BeginObjectWithCmd(this->GetEntityClass(), cmd, path.Get())) 
        {
            ///current states
            for (int i=0; i< this->GetNumStates(); i++)
            {
                int leaderStateIdx = this->animStateArray.GetStateAt(i).GetLeaderState();
                nString leaderStateName;
                if (leaderStateIdx != -1)
                {
                    leaderStateName = this->GetStateName(leaderStateIdx);
                }

                ps->Put(this->GetEntityClass(), 'AADL', this->GetStateName(i), leaderStateName.Get());
            }

            // future or old states
            n_assert(this->futureSubmissiveNames.Size() == this->futureLeaderNames.Size());
            for (int i=0; i< this->futureLeaderNames.Size(); i++)
            {
                //could happen that an old state has just been added, so, test it.
                if (this->futureSubmissiveNames[i] != this->GetStateName(this->GetNumStates()-1))
                {
                    ps->Put(this->GetEntityClass(), 'AADL', this->futureSubmissiveNames[i].Get(), this->futureLeaderNames[i].Get());
                }
            }

            // states follow joint
            for (int i=0; i< this->GetNumStates(); i++)
            {
                nString followedjointName = this->GetStateFollowsJoint(this->GetStateAt(i).GetName());
                ps->Put(this->GetEntityClass(), 'AAFO', this->GetStateName(i), followedjointName.Get());
            }

            ps->EndObject(true);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
*/
nString
ncSkeletonClass::GetTweakableJointsPath()
{
    nString path;
    path.Format("wc:classes/%s/tweakablejoints.n2", this->GetEntityClass()->GetName());

    return path;
}

//-----------------------------------------------------------------------------
/**
    Loads chunk information.
*/
bool 
ncSkeletonClass::LoadTweakableJoints()
{
    // load resource
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);
    if (ps)
    {
        nString path = this->GetTweakableJointsPath();

        if (nFileServer2::Instance()->FileExists(path))
        {
            nKernelServer::Instance()->PushCwd(this->GetEntityClass());
            nKernelServer::Instance()->Load(path.Get(), false);
            nKernelServer::Instance()->PopCwd();
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Saves tweakablejoints information.
*/
bool
ncSkeletonClass::SaveTweakableJoints()
{
    nPersistServer *ps = nKernelServer::Instance()->GetPersistServer();
    n_assert(ps);

    // load resource
    n_assert(ps);
    if (ps && this->character.GetNumTweakableJoints())
    {
        nString path = this->GetTweakableJointsPath();

        nFileServer2::Instance()->MakePath(path.ExtractDirName());

        nCmd * cmd = ps->GetCmd(this->GetEntityClass(), 'THIS');

        if (ps->BeginObjectWithCmd(this->GetEntityClass(), cmd, path.Get())) 
        {
            // --- BeginTweakableJoints ---
            ps->Put(this->GetEntityClass(), 'MBTJ', this->character.GetNumTweakableJoints());

            // --- SetTweakableJoint ---
            for (int i = 0; i < this->character.GetNumTweakableJoints(); i++)
            {
                for( int j=0; j< this->character.GetNumJointInTweakableJoint(i); j++)
                {
                    ps->Put(this->GetEntityClass(), 'MSTJ', i, this->character.GetTweakableJointAt(i,j), this->character.GetParentTweakableJointAt(i,j));
                }
            }

            // --- EndTweakableJoints ---
            ps->Put(this->GetEntityClass(), 'METJ');

            ps->EndObject(true);
        }
    }

    return true;
}
