#include "precompiled/pchnscene.h"
//------------------------------------------------------------------------------
//  nskingeometryanimator_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nscene/nskingeometryanimator.h"
#include "nscene/nskingeometrynode.h"
#include "nscene/ncscene.h"
#include "entity/nentity.h"
#include "variable/nvariableserver.h"
#include "nanimation/nanimationserver.h"
#include "zombieentity/ncdictionary.h"

nNebulaScriptClass(nSkinGeometryAnimator, "nanimator");

//------------------------------------------------------------------------------
/**
*/
nSkinGeometryAnimator::nSkinGeometryAnimator() :
    characterVarIndex(0),
    frameIdVarIndex(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nSkinGeometryAnimator::~nSkinGeometryAnimator()
{
    // Clear out the joint name list - but remember the node data pointers contain actual
    // integers, not pointers to anything, so don't delete them as if they were pointers.
    nStrNode *strHead = this->jointNameList.GetHead();

    while (strHead)
    {
        nStrNode *next = strHead->GetSucc();

        strHead->Remove();
        n_delete(strHead);
        strHead = next;
    }

    this->UnloadResources();
}

//------------------------------------------------------------------------------
/**
    Unload the animation resource file.
*/
void
nSkinGeometryAnimator::UnloadAnims()
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
nSkinGeometryAnimator::LoadAnims()
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
    Load the resources needed by this object.
*/
bool
nSkinGeometryAnimator::LoadResources()
{
    if (nSceneNode::LoadResources())
    {
        this->LoadAnims();
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    Unload the resources.
*/
void
nSkinGeometryAnimator::UnloadResources()
{
    nSceneNode::UnloadResources();
    this->UnloadAnims();
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryAnimator::EntityCreated(nEntityObject* entityObject)
{
    nAnimator::EntityCreated(entityObject);
    ncScene *renderContext = entityObject->GetComponent<ncScene>();

    // see if resources need to be reloaded
    if (!this->AreResourcesValid())
    {
        this->LoadResources();
    }

    // TODO: register the character thorugh the anim server
    // if there is name for it, it will be reused for several shapes (LOD)
    // the animation server needs to know that a skeleton has been already 
    // been computed for a frame, character and state.
    nCharacter2* curCharacter = n_new(nCharacter2(this->character));
    n_assert(curCharacter);

    // put frame persistent data in render context
    nVariable::Handle charHandle = nVariableServer::Instance()->GetVariableHandleByName("charPointer");
    this->frameIdVarIndex = renderContext->AddLocalVar(nVariable(0, (int) 0xffffffff));
    this->characterVarIndex = renderContext->AddLocalVar(nVariable(charHandle, curCharacter));
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryAnimator::EntityDestroyed(nEntityObject* entityObject)
{
    ncScene *renderContext = entityObject->GetComponent<ncScene>();

    nVariable var;
    var = renderContext->GetLocalVar(this->characterVarIndex);
    nCharacter2* curCharacter = (nCharacter2*) var.GetObj();
    n_assert(curCharacter);

    n_delete(curCharacter);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryAnimator::Animate(nSceneNode* sceneNode, nEntityObject* entityObject)
{
    n_assert(sceneNode);
    n_assert(entityObject);
    n_assert(nVariable::InvalidHandle != this->channelVarHandle);
#if 0
    /// @todo ma.garcias- restore to use bone animation in non-character entities
    ncScene *renderContext = entityObject->GetComponent<ncScene>();
    //nVariableContext& varContext = entityObject->GetComponent<ncDictionary>()->VarContext();
    ncDictionary *dictionary = entityObject->GetComponent<ncDictionary>();
    
    // update the source node with the new char skeleton state
    static nClass *nSkinGeometryNodeClass = nKernelServer::Instance()->FindClass("nskingeometrynode");
    nSkinGeometryNode *skinGeometryNode = 0;
    if (sceneNode->IsA(nSkinGeometryNodeClass))
    {
        skinGeometryNode = (nSkinGeometryNode *) sceneNode;
    }
    else
    {
        n_error("nSkinGeometryAnimator::Animate: can't cast sceneNode!\n");
    }
    
    nVariable::Handle animStateVarHandle = skinGeometryNode->GetAnimStateVarHandle();
    n_assert(animStateVarHandle != nVariable::InvalidHandle);
    
    // retrieve the character for this instance from the render context
    nVariable& characterVar = renderContext->GetLocalVar(this->characterVarIndex);
    nCharacter2* curCharacter = (nCharacter2*) characterVar.GetObj();
    n_assert(curCharacter);

    nVariable& frameIdVar = renderContext->GetLocalVar(this->frameIdVarIndex);
    uint frameId = frameIdVar.GetInt();
    
    // check if I am already uptodate for this frame
    uint curFrameId = renderContext->GetFrameId();
    if (frameId != curFrameId)
    {
        frameIdVar.SetInt(curFrameId);

        // get the sample time from the render context
        //nVariable* var = varContext.GetVariable(this->channelVarHandle);
        //if (var == 0)
        //{
        //    n_error("nSkinGeometryAnimator::Animate: TimeChannel Variable '%s' not found in the RenderContext!\n", nVariableServer::Instance()->GetVariableName(this->channelVarHandle));
        //}
        //float curTime = var->GetFloat();
        //float curTime = (float) renderContext->GetTime();
        
        // get the current anim state from the anim state channel
        // (assume 0 as default state index)
        nVariable* var = dictionary->GetVariable(animStateVarHandle);
        int animState = 0;
        if (var)
        {
            animState = var->GetInt();
        }
        //if (animState != curCharacter->GetActiveState())
        //{
        //    // activate new state
        //    if (curCharacter->IsValidStateIndex(animState))
        //    {
        //        // FIXME antonia.tugores - offset parameter?
        //        curCharacter->SetActiveState(animState, curTime, 0.0f);
        //    }
        //    else
        //    {
        //        n_printf("Warning: Invalid state index %d. State switch ignored. \n", animState);
        //    }
        //}
        // evaluate the current state of the character skeleton
        // TODO: perform this through animation server, to cache skeletons
        //curCharacter->EvaluateSkeleton(curTime, &varContext);
    }
    // update the target node
    //skinGeometryNode->SetCharSkeleton(&curCharacter->GetSkeleton());
#endif
}

//------------------------------------------------------------------------------
/**
    Begin configuring the joint skeleton.
*/
void
nSkinGeometryAnimator::BeginJoints(int numJoints)
{
    this->character.GetSkeleton().Clear();
    this->character.GetSkeleton().BeginJoints(numJoints);
}

//------------------------------------------------------------------------------
/**
    Add a joint to the joint skeleton.
*/
void
nSkinGeometryAnimator::SetJoint(int jointIndex, int parentJointIndex, const vector3& poseTranslate, const quaternion& poseRotate, const vector3& poseScale)
{
    this->character.GetSkeleton().SetJoint(jointIndex, parentJointIndex, poseTranslate, poseRotate, poseScale, "");
}

//------------------------------------------------------------------------------
/**
    Finish adding joints to the joint skeleton.
*/
void
nSkinGeometryAnimator::EndJoints()
{
    this->character.GetSkeleton().EndJoints();
}

//------------------------------------------------------------------------------
/**
    Get number of joints in joint skeleton.
*/
int
nSkinGeometryAnimator::GetNumJoints()
{
    return this->character.GetSkeleton().GetNumJoints();
}

//------------------------------------------------------------------------------
/**
    Get joint attributes.
*/
void
nSkinGeometryAnimator::GetJoint(int index, int& parentJointIndex, vector3& poseTranslate, quaternion& poseRotate, vector3& poseScale)
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
nSkinGeometryAnimator::AddJointName(unsigned int joint, const char *name)
{
    JointNameNode *newNameNode = n_new(JointNameNode);
    
    newNameNode->SetName(name);
    newNameNode->SetJointIndex(joint);
    
    this->jointNameList.AddHead(newNameNode);
}

//------------------------------------------------------------------------------
/**
    Get a joint index by name.
    
    @param name  the name of the joint index to retrieve.  Returns 65535 if joint not found.
*/
unsigned int
nSkinGeometryAnimator::GetJointByName(const char *name)
{
    JointNameNode *node = (JointNameNode *) this->jointNameList.Find(name);

    if (node)
    {
        return node->GetJointIndex();
    }
    else
    {
        return (unsigned int) InvalidIndex;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryAnimator::BeginStates(int num)
{
    this->animStateArray.Begin(num);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryAnimator::SetState(int stateIndex, int animGroupIndex, float fadeInTime)
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
nSkinGeometryAnimator::SetStateName(int stateIndex, const nString& name)
{
    this->animStateArray.GetStateAt(stateIndex).SetName(name);
}

//------------------------------------------------------------------------------
/**
    Assign per-state animation file.
*/
void
nSkinGeometryAnimator::SetStateAnim(int stateIndex, const char *filename)
{
    this->animStateArray.GetStateAt(stateIndex).SetAnimFile(filename);
}

//------------------------------------------------------------------------------
/**
    Get per-state animation file.
*/
const char *
nSkinGeometryAnimator::GetStateAnim(int stateIndex)
{
    return this->animStateArray.GetStateAt(stateIndex).GetAnimFile();
}

//------------------------------------------------------------------------------
/**
    Set per-state animator channel
*/
void
nSkinGeometryAnimator::SetStateAnimChannel(int /*stateIndex*/, const char * /*animChannel*/)
{
    // TODO: allow specifying channel for an external animator (mixer?)
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryAnimator::EndStates()
{
    this->animStateArray.End();
}

//------------------------------------------------------------------------------
/**
*/
int
nSkinGeometryAnimator::GetNumStates() const
{
    return this->animStateArray.GetNumStates();
}

//------------------------------------------------------------------------------
/**
*/
const nAnimState&
nSkinGeometryAnimator::GetStateAt(int stateIndex)
{
    return this->animStateArray.GetStateAt(stateIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryAnimator::BeginClips(int stateIndex, int numClips)
{
    this->animStateArray.GetStateAt(stateIndex).BeginClips(numClips);
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryAnimator::SetClip(int stateIndex, int clipIndex, const char* weightChannelName)
{
    // number of anim curves in a clip is identical to number of (joints * 3)
    // (one curve for translation, rotation and scale)
    int numCurves = this->GetNumJoints() * 3;
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
nSkinGeometryAnimator::EndClips(int stateIndex)
{
    this->animStateArray.GetStateAt(stateIndex).EndClips();
}

//------------------------------------------------------------------------------
/**
*/
int
nSkinGeometryAnimator::GetNumClips(int stateIndex) const
{
    return this->animStateArray.GetStateAt(stateIndex).GetNumClips();
}

//------------------------------------------------------------------------------
/**
*/
void
nSkinGeometryAnimator::GetClipAt(int stateIndex, int clipIndex, const char*& weightChannelName)
{
    nVariable::Handle varHandle = this->animStateArray.GetStateAt(stateIndex).GetClipAt(clipIndex).GetWeightChannelHandle();
    weightChannelName = nVariableServer::Instance()->GetVariableName(varHandle);
}
