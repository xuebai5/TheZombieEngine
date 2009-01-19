#include "precompiled/pchnnebula.h"
//------------------------------------------------------------------------------
//  ncharacter2.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "character2/ncharacter2.h"
#include "kernel/nlogclass.h"

vector4 nCharacter2::scratchKeyArray[MaxCurves];
vector4 nCharacter2::keyArray[MaxCurves];
vector4 nCharacter2::transitionKeyArray[MaxCurves];

//------------------------------------------------------------------------------
/**
    Default constructor
*/
nCharacter2::nCharacter2() :
    animStateArray(0),
    animEnabled(true),
    lastEvaluationFrameId(0),
    animEventHandler(0),
    transitionToDo(false),
    tweakableJointsArray(0,1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Copy constructor
*/
nCharacter2::nCharacter2(const nCharacter2& src) :
    animStateArray(0),
    animEnabled(true),
    lastEvaluationFrameId(0),
    animEventHandler(0),
    transitionToDo(false)
{
    *this = src;

    if (this->animEventHandler)
    {
        this->animEventHandler->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
    Default destructor
*/
nCharacter2::~nCharacter2()
{
    this->SetAnimEventHandler(0);
}

#if 0
//------------------------------------------------------------------------------
/**
    OBSOLETE
    Set a new animation state, and handle stuff necessary for
    blending between previous and current state.
*/
void
nCharacter2::SetActiveState(int stateIndex, nTime time, nTime offset)
{
    n_assert(this->IsValidStateIndex(stateIndex));

    // only overwrite previous state if the current state was active 
    // for at least a little while to prevent excessive "plopping"
    if (this->curStateInfo.IsValid())
    {
        nAnimState& curAnimState = this->animStateArray->GetStateAt(this->curStateInfo.GetStateIndex());
        float fadeInTime = curAnimState.GetFadeInTime();
        if ((time - this->curStateInfo.GetStateStarted()) > fadeInTime)
        {
            this->prevStateInfo = this->curStateInfo;
        }
    }
    else
    {
        // no valid current state
        this->prevStateInfo = this->curStateInfo;
    }
    this->curStateInfo.SetStateIndex(stateIndex);
    this->curStateInfo.SetStateStarted(float(time));
    this->curStateInfo.SetStateOffset(float(offset));
}
#endif
//------------------------------------------------------------------------------
/**
    Evaluate animation for a given state at a given time.
    This process is divided into:
    - if transition is enabled and time is within fade-in time, perform
      transition from previous frame (or no transition if no previous valid state)

    @param  curGroupIndex   index of joint group being animated
    @param  varContext      variable context for state variable channels
    @param  curStateInfo    state info for current state
    @param  prevStateInfo   state info for previous state
*/
bool
nCharacter2::EvaluateState(int curGroupIndex, float time, nVariableContext* varContext, nStateInfo& curStateInfo, nArray<nStateInfo>& prevStatesInfo)
{
    n_assert(curStateInfo.IsValid());
    n_assert(animStateArray);
    
    // check if a state transition is necessary
    nAnimState& curAnimState = this->animStateArray->GetStateAt(curStateInfo.GetStateIndex());
    float curRelTime = time - curStateInfo.GetStateStarted();

    // handle time exception (this happens when time is reset to a smaller value
    // since the last anim state switch)
    if (curRelTime < 0.0f)
    {
        curRelTime = 0.0f;
        curStateInfo.SetStateStarted(time);
    }
    
    float fadeInTime = curAnimState.GetFadeInTime();

    // check if do/notdo transition
    this->transitionToDo = curStateInfo.GetTransition() && (fadeInTime > 0.0f) && (curRelTime < fadeInTime);

    if (this->transitionToDo)
    {        
        nAnimState prevAnimState;
        float prevRelTime = 0;
        bool sample = false;
        float velocityFactor = 1.f;
        nAnimation::Group::LoopType prevloopType = curAnimState.GetAnim()->GetGroupAt(0).GetLoopType();

        // retrieve previous state for same jointgroup
        if (prevStatesInfo[curGroupIndex].IsValid())
        {
            prevAnimState = this->animStateArray->GetStateAt(prevStatesInfo[curGroupIndex].GetStateIndex());
            this->ComputeAuxiliaryStateData(prevStatesInfo[curGroupIndex], time, prevRelTime, prevloopType);
            velocityFactor = prevStatesInfo[curGroupIndex].GetVelocityFactor();
            sample = true;
        }
        // retrieve previous state from full skeleton
        /// @todo ma.garcias full-skeleton state can be sampled twice,
        /// when in transition from a full-body state to a partial-body state
        else
        {
            if (prevStatesInfo[0].IsValid())
            {
                prevAnimState = this->animStateArray->GetStateAt(prevStatesInfo[0].GetStateIndex());
                this->ComputeAuxiliaryStateData(prevStatesInfo[0], time, prevRelTime, prevloopType);
                velocityFactor = prevStatesInfo[0].GetVelocityFactor();
                sample = true;
            }
        }

        // state transition is necessary, compute a lerp value
        // and sample the previous anim state
        if (sample)
        {
            if (prevAnimState.Sample(prevRelTime, prevAnimState.GetAnim(), varContext, transitionKeyArray, scratchKeyArray, prevloopType, velocityFactor, MaxCurves))
            {
                curStateInfo.SetTransition(true);
                curStateInfo.SetLerp(curRelTime / fadeInTime);
            }
        }

        // if changed from n partial states to one full skeleton state.
        /// @todo ma.garcias- prev states had to animate full skeleton!!
        if (curGroupIndex == 0 && !prevStatesInfo[curGroupIndex].IsValid())
        {
            vector4 auxTransitionKeyArray[MaxCurves];
            for (int groupIndex = 1; groupIndex < prevStatesInfo.Size(); groupIndex++)
            {
                // was animated
                if (prevStatesInfo[groupIndex].IsValid())
                {
                    prevAnimState = this->animStateArray->GetStateAt(prevStatesInfo[groupIndex].GetStateIndex());
                    prevRelTime = time - prevStatesInfo[groupIndex].GetStateStarted();
                    velocityFactor = prevStatesInfo[groupIndex].GetVelocityFactor();
                    sample = true;

                    prevAnimState.Sample(prevRelTime, prevAnimState.GetAnim(), varContext, transitionKeyArray, scratchKeyArray, prevloopType, velocityFactor, MaxCurves);
                    curStateInfo.SetTransition(true);
                    curStateInfo.SetLerp(curRelTime / fadeInTime);

                    nJointGroup& jointGroup = this->GetSkeleton().GetJointGroupAt(groupIndex);
                    for (int jointGroupIndex = 0; jointGroupIndex < jointGroup.GetGroupSize(); jointGroupIndex++)
                    {
                        int jointIndex = jointGroup.GetJointIndexAt(jointGroupIndex);
                        int keyIndex = jointIndex * 3;
                        int prevKeyIndex = jointGroupIndex * 3;
                        auxTransitionKeyArray[keyIndex]   = transitionKeyArray[prevKeyIndex];
                        auxTransitionKeyArray[keyIndex+1] = transitionKeyArray[prevKeyIndex + 1];
                        auxTransitionKeyArray[keyIndex+2] = transitionKeyArray[prevKeyIndex + 2];
                    }
                }
            }
            memcpy(transitionKeyArray, auxTransitionKeyArray, sizeof(auxTransitionKeyArray));
        }
    }
    
    /// @todo ma.garcias- using animation group 0 only!
    nAnimation::Group::LoopType loopType = curAnimState.GetAnim()->GetGroupAt(0).GetLoopType();
    this->ComputeAuxiliaryStateData(curStateInfo, time, curRelTime, loopType);
    float velocityFactor = curStateInfo.GetVelocityFactor();

    // get samples from current anim state
    return curAnimState.Sample(curRelTime, curAnimState.GetAnim(), varContext, keyArray, scratchKeyArray, loopType, velocityFactor, MaxCurves);
}

//------------------------------------------------------------------------------
/**
    Evaluate full skeleton, when animated using jointGroups.

    @param  time            current time
    @param  varContext      variable context, to retrieve animation channels
    @param  curStatesInfo   current set of states (indexed by joint group)
    @param  prevStatesInfo  previous set of states (indexed by joint group)
*/
void 
nCharacter2::EvaluateFullSkeleton(float time, nVariableContext* varContext, nArray<nStateInfo>& curStatesInfo, nArray<nStateInfo>& prevStatesInfo)
{
    //
    for (int groupIndex = 0; groupIndex < curStatesInfo.Size(); groupIndex++)
    {
        NLOG(animation, (NLOG1|1, "EvaluateFullSkeleton: jointGroup: %d, time: %f", groupIndex, time));

        // if active state, compute new positions
        if (curStatesInfo[groupIndex].IsValid() && this->EvaluateState(groupIndex, time, varContext, curStatesInfo[groupIndex], prevStatesInfo))
        {
            const vector4* keyPtr = keyArray;
            const vector4* prevKeyPtr = transitionKeyArray;

            // compute
            vector3 translate, prevTranslate;
            quaternion rotate, prevRotate;
            vector3 scale, prevScale;

            nJointGroup jGroup = this->GetSkeleton().GetJointGroupAt(groupIndex);

            // to add weapon/accessory position offset for first person animations
            bool offsetApplied = false;
            vector3 currentOffset = curStatesInfo[groupIndex].GetOffset();
            int currentRootJointOffset = curStatesInfo[groupIndex].GetOffsetIndex();
            int prevRootJointOffset = prevStatesInfo[groupIndex].GetOffsetIndex();
            vector3 prevOffset = prevStatesInfo[groupIndex].GetOffset();

            for (int index = 0; index < jGroup.GetGroupSize(); index++)
            {
                // read sampled translation, rotation and scale
                translate.set(keyPtr->x, keyPtr->y, keyPtr->z);          keyPtr++;
                rotate.set(keyPtr->x, keyPtr->y, keyPtr->z, keyPtr->w);  keyPtr++;
                scale.set(keyPtr->x, keyPtr->y, keyPtr->z);              keyPtr++;
                
                if (this->transitionToDo)
                {
                    float lerp = curStatesInfo[groupIndex].GetLerp();

                    // if previous state was fullskeleton one get only curJointGroup joints information
                    
                    // if previous state was partial (same jointGroup)
                    if (prevStatesInfo[groupIndex].IsValid())
                    {
                        prevTranslate.set(prevKeyPtr->x, prevKeyPtr->y, prevKeyPtr->z);              prevKeyPtr++;
                        prevRotate.set(prevKeyPtr->x, prevKeyPtr->y, prevKeyPtr->z, prevKeyPtr->w);  prevKeyPtr++;
                        prevScale.set(prevKeyPtr->x, prevKeyPtr->y, prevKeyPtr->z);                  prevKeyPtr++;
                        translate.lerp(prevTranslate, lerp);
                        rotate.slerp(prevRotate, rotate, lerp);
                        scale.lerp(prevScale, lerp);
                    }
                    else
                    {
                        int jointIndex = jGroup.GetJointIndexAt(index);
                        // each joint has 3 related keys
                        int keyIndex = jointIndex *3;

                        prevTranslate.set(prevKeyPtr[keyIndex].x, prevKeyPtr[keyIndex].y, prevKeyPtr[keyIndex].z);                      keyIndex++;
                        prevRotate.set(prevKeyPtr[keyIndex].x, prevKeyPtr[keyIndex].y, prevKeyPtr[keyIndex].z, prevKeyPtr[keyIndex].w); keyIndex++;
                        prevScale.set(prevKeyPtr[keyIndex].x, prevKeyPtr[keyIndex].y, prevKeyPtr[keyIndex].z);                          keyIndex++;
                        translate.lerp(prevTranslate, lerp);
                        rotate.slerp(prevRotate, rotate, lerp);
                        scale.lerp(prevScale, lerp);
                    }
                }

                //nCharacterServer::Instance()->profAnimFullSkeleton.StartAccum();
                
                nCharJoint& joint = this->charSkeleton.GetJointAt(jGroup.GetJointIndexAt(index));
                // apply weapon/accessory offset
                if (!offsetApplied && (currentRootJointOffset == index || prevRootJointOffset == index))
                {
                    vector3 offset = currentOffset;

                    offsetApplied = true;

                    //float percent;
                    float percent = 1.0f;

                    float curRelTime = time - curStatesInfo[groupIndex].GetStateStarted();
                    float currentFadeInTime = 0.2f;

                    if ((currentFadeInTime > 0.0f) && (curRelTime < currentFadeInTime))
                    {
                        percent = curRelTime/currentFadeInTime;
                    }

                    if (currentRootJointOffset == index)
                    {
                        currentOffset *= percent;
                    }

                    if (curStatesInfo[groupIndex].GetStateIndex() != prevStatesInfo[groupIndex].GetStateIndex())
                    {
                        if (prevRootJointOffset == index)
                        {
                            prevOffset *= percent;
                        }

                        offset = currentOffset + prevStatesInfo[groupIndex].GetOffset() - prevOffset;
                    }

                    translate += offset;
                }

                joint.SetTranslate(translate);
                joint.SetRotate(rotate);
                joint.SetScale(scale);

                //nCharacterServer::Instance()->profAnimFullSkeleton.StopAccum();
            }
        }
        else
        {
            //if (! curStatesInfo[groupIndex].IsValid())
            //{
            //    // get last positions
            //    for (int index = 0; index < this->GetSkeleton().GetJointGroupAt(groupIndex).GetGroupSize(); index++)
            //    {                
            //        nCharJoint& joint = this->charSkeleton.GetJointAt(this->GetSkeleton().GetJointGroupAt(groupIndex).GetJointIndexAt(index));
            //        joint.SetTranslate(joint.GetPoseTranslate());
            //        joint.SetRotate(joint.GetPoseRotate());
            //        joint.SetScale(joint.GetPoseScale());
            //    }   
            //}
        }
        this->transitionToDo = false;
    }


    // any train has to inherit some joint movement
    for (int groupIndex = 0; groupIndex < curStatesInfo.Size(); groupIndex++)
    {
        int stateIndex = curStatesInfo[groupIndex].GetStateIndex();
        if (stateIndex != -1)
        {
            int jointIdx = this->GetStateFollowsJoint(stateIndex);
            if (jointIdx != -1)
            {
                nCharJoint & followJoint = this->charSkeleton.GetJointAt(jointIdx);
                nJointGroup jGroup = this->GetSkeleton().GetJointGroupAt(groupIndex);
                int rootIndex = jGroup.GetJointIndexAt(0);
                nCharJoint& rootJoint = this->charSkeleton.GetJointAt(rootIndex);
                n_assert(rootJoint.GetParentJointIndex() == -1);
                vector3 trConstant = (followJoint.GetTranslate() - followJoint.GetPoseTranslate())
                    - (rootJoint.GetTranslate() - rootJoint.GetPoseTranslate());
                for (int index = 0; index < jGroup.GetGroupSize(); index++)
                {
                    nCharJoint& joint = this->charSkeleton.GetJointAt(jGroup.GetJointIndexAt(index));
                    if (joint.GetParentJointIndex() == -1)
                    {
                        // current translation + jointIdx translation - initial jointIdx translation (to correct movement)
                        vector3 tr = joint.GetTranslate() + trConstant;
                        NLOG(animation, (NLOG1|0, "Follow joint %s(%d) in group %d from joint %s(%d) stateIndex=%d (%0.2f,%0.2f,%0.2f) corrected to (%0.2f,%0.2f,%0.2f)", 
                            joint.GetName().Get(), index,
                            groupIndex,
                            this->charSkeleton.GetJointAt(jointIdx).GetName().Get(), jointIdx,
                            stateIndex,
                            joint.GetTranslate().x, joint.GetTranslate().y, joint.GetTranslate().z,
                            tr.x, tr.y, tr.z ));
                        joint.SetTranslate(tr);
                    }
                }
            }
        }
    }

    // apply look-at
    // get current orientation, and compute quaternion to look-at vector
    for (int groupIndex = 0; groupIndex < curStatesInfo.Size(); groupIndex++)
    {
        int curStateIndex = curStatesInfo[groupIndex].GetStateIndex();
        if ( curStateIndex != -1 && curStatesInfo[groupIndex].HasJointLookAt() && this->tweakableJointsArray.Size())
        {
            nStateInfo& stateInfo = curStatesInfo[groupIndex];

            int lookAtGroup = stateInfo.GetJointLookAtIndex();
            quaternion lookAtQuat  = stateInfo.GetJointLookAtQuat();

            for( int twkJt = 0; twkJt < this->tweakableJointsArray[lookAtGroup].GetNumTweableJoints(); twkJt++)
            {
                vector3 position, scale;
                quaternion rotation;

                nCharJoint& joint = this->charSkeleton.GetJointAt(this->tweakableJointsArray[lookAtGroup].jointIndex[twkJt]);

                // update model transformation
                joint.Evaluate();
                joint.GetWorldTransformation(position, rotation, scale);

                if( this->tweakableJointsArray[lookAtGroup].parentIndex[twkJt] != -1 )
                {
                    nCharJoint& parentJoint = this->charSkeleton.GetJointAt(this->tweakableJointsArray[lookAtGroup].parentIndex[twkJt]);
                    parentJoint.Evaluate();
                    vector3 parentPos = parentJoint.GetMatrix().pos_component();
 
                    position = lookAtQuat.rotate( position - parentPos) + parentPos;
                }

                rotation = lookAtQuat * rotation;

                joint.SetTranslate(position);
                joint.SetRotate(rotation);
                joint.SetScale(vector3(1,1,1));

                joint.EvaluateWorldCoord();
            }
        }
    }    

    // evaluate skeleton in model coordinates, skip tweaked joints
    this->charSkeleton.Evaluate();
}

#if 0
//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
void
nCharacter2::EvaluateSkeleton(float time, nVariableContext* varContext)
{
    if (this->IsAnimEnabled() && this->curStateInfo.IsValid())
    {
        n_assert(this->animStateArray);
        n_assert(this->animation);

        // check if a state transition is necessary
        nAnimState& curAnimState = this->animStateArray->GetStateAt(this->curStateInfo.GetStateIndex());
        float curRelTime = time - this->curStateInfo.GetStateStarted();
        
        // handle time exception (this happens when time is reset to a smaller value
        // since the last anim state switch)
        if (curRelTime < 0.0f)
        {
            curRelTime = 0.0f;
            this->curStateInfo.SetStateStarted(time);
        }

        float fadeInTime = curAnimState.GetFadeInTime();
        float lerp = 1.0f;
        bool transition = false;
        float velocityFactor= 1.0f; //added to fit new implementation

        if ((fadeInTime > 0.0f) && (curRelTime < fadeInTime) && this->prevStateInfo.IsValid())
        {
            // state transition is necessary, compute a lerp value
            // and sample the previous anim state
            nAnimState& prevAnimState = this->animStateArray->GetStateAt(this->prevStateInfo.GetStateIndex());
            float prevRelTime = time - this->prevStateInfo.GetStateStarted();
            float sampleTime = prevRelTime + this->prevStateInfo.GetStateOffset();
            if (prevAnimState.Sample(sampleTime, this->animation, varContext, transitionKeyArray, scratchKeyArray, nAnimation::Group::Repeat, velocityFactor, MaxCurves))
            {
                transition = true;
                lerp = curRelTime / fadeInTime;
            }
        }

        // get samples from current anim state
        float sampleTime = curRelTime + this->curStateInfo.GetStateOffset();
        if (curAnimState.Sample(sampleTime, this->animation, varContext, keyArray, scratchKeyArray, nAnimation::Group::Repeat, velocityFactor, MaxCurves))
        {
            // transfer the sampled animation values into the character skeleton
            int numJoints = this->charSkeleton.GetNumJoints();
            int jointIndex;
            const vector4* keyPtr = keyArray;
            const vector4* prevKeyPtr = transitionKeyArray;

            vector3 translate, prevTranslate;
            quaternion rotate, prevRotate;
            vector3 scale, prevScale;
            for (jointIndex = 0; jointIndex < numJoints; jointIndex++)
            {
                // read sampled translation, rotation and scale
                translate.set(keyPtr->x, keyPtr->y, keyPtr->z);          keyPtr++;
                rotate.set(keyPtr->x, keyPtr->y, keyPtr->z, keyPtr->w);  keyPtr++;
                scale.set(keyPtr->x, keyPtr->y, keyPtr->z);              keyPtr++;

                if (transition)
                {
                    prevTranslate.set(prevKeyPtr->x, prevKeyPtr->y, prevKeyPtr->z);              prevKeyPtr++;
                    prevRotate.set(prevKeyPtr->x, prevKeyPtr->y, prevKeyPtr->z, prevKeyPtr->w);  prevKeyPtr++;
                    prevScale.set(prevKeyPtr->x, prevKeyPtr->y, prevKeyPtr->z);                  prevKeyPtr++;
                    translate.lerp(prevTranslate, lerp);
                    rotate.slerp(prevRotate, rotate, lerp);
                    scale.lerp(prevScale, lerp);
                }
                
                nCharJoint& joint = this->charSkeleton.GetJointAt(jointIndex);
                joint.SetTranslate(translate);
                joint.SetRotate(rotate);
                joint.SetScale(scale);
            }
        }
    }
    this->charSkeleton.Evaluate();
}
#endif

//------------------------------------------------------------------------------
/**
    Finds a state index by name. Returns -1 if state not found.
*/
int
nCharacter2::FindStateIndexByName(const nString& n)
{
    return this->animStateArray->FindStateIndex(n);
}

//------------------------------------------------------------------------------
/**
    Returns the duration of an animation state in seconds.
*/
nTime
nCharacter2::GetStateDuration(int stateIndex) const
{
    n_assert(this->IsValidStateIndex(stateIndex));
    n_assert(this->animStateArray);

    if (this->IsValidStateIndex(stateIndex))
    {
        nAnimState& animState = this->animStateArray->GetStateAt(stateIndex);
        int animGroupIndex = animState.GetAnimGroupIndex();
        return animState.GetAnim()->GetDuration(animGroupIndex);
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    Returns the fadein time of an animation state in seconds.
*/
nTime
nCharacter2::GetStateFadeInTime(int stateIndex) const
{
    n_assert(this->IsValidStateIndex(stateIndex));
    n_assert(this->animStateArray);

    if (this->IsValidStateIndex(stateIndex))
    {
        return this->animStateArray->GetStateAt(stateIndex).GetFadeInTime();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
    Retrieve variable information for an animation state,
    during computation of the current skeleton state-
    basically, compute a relative time since the state began,
    for the case the state is being played backwards

    @param  stateInfo   Runtime information of the state (input/output) 
    @param  time        current time (input)
    @param  relTime     time elapsed since the state started (output)
    @param  loopType    loop type (clamp/repeat) (output)
*/
void
nCharacter2::ComputeAuxiliaryStateData(nStateInfo& stateInfo, float time, float& relTime, nAnimation::Group::LoopType& loopType)
{
    int keyIndex[2];
    float inbetween;

    const nAnimation::Group& group = this->GetAnimStateArray()->GetStateAt(stateInfo.GetStateIndex()).GetAnim()->GetGroupAt(0);
    relTime = this->GetKeysFor(stateInfo, time, keyIndex, inbetween, group, loopType);

    NLOG(animation, (NLOG1|1, "ComputeAuxiliaryStateData stateindex = %d", stateInfo.GetStateIndex()));
    if (stateInfo.GetJustStarted())
    {
        stateInfo.SetJustStarted(false, relTime);
    }
    else
    {
        if (stateInfo.GetRepeat())
        {
            stateInfo.SetJustStarted(keyIndex[1], relTime);
        }
    }

    NLOG(animation, (NLOG1|1, "ComputeAuxiliaryStateData_end"));
}

//------------------------------------------------------------------------------
/**
    Emit animation event for the current animation states.
*/
void
nCharacter2::EmitAnimEvents(float /*fromTime*/, float /*toTime*/)
{
//    if (this->animEventHandler && this->curStateInfo.IsValid())
//    {
//        n_assert(this->animation);
//        float relFromTime = (fromTime - this->curStateInfo.GetStateStarted()) + this->curStateInfo.GetStateOffset();
//        float relToTime   = (toTime - this->curStateInfo.GetStateStarted()) + this->curStateInfo.GetStateOffset(); 
//        nAnimState& animState = this->animStateArray->GetStateAt(this->curStateInfo.GetStateIndex());
//        animState.EmitAnimEvents(relFromTime, relToTime, this->animation, this->animEventHandler);
//    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacter2::SetSubmissiveLeaderState(int submissiveStateIdx, int leaderStateIdx)
{
    this->animStateArray->GetStateAt(submissiveStateIdx).SetLeaderState(leaderStateIdx);
}

//------------------------------------------------------------------------------
/**
*/
int 
nCharacter2::GetLeaderState(int submissiveStateIdx) const
{
    return this->animStateArray->GetStateAt(submissiveStateIdx).GetLeaderState();
}

//------------------------------------------------------------------------------
/**
*/
int 
nCharacter2::GetLeaderState(const nString& submissiveStateName) const
{
    int submissiveStateIdx = this->animStateArray->FindStateIndex(submissiveStateName);
    return this->GetLeaderState(submissiveStateIdx);
}

//------------------------------------------------------------------------------
/**
*/
const vector3 
nCharacter2::GetStateDisplacement(const nStateInfo& stateInfo, float fromTime, float toTime)
{
    n_assert(this->IsValidStateIndex(stateInfo.GetStateIndex()));
    n_assert(this->animStateArray);

    vector3 motion;

    nAnimState& animState = this->GetAnimStateArray()->GetStateAt(stateInfo.GetStateIndex());
    if (this->IsValidStateIndex(stateInfo.GetStateIndex()) && animState.IsMotionValid())
    {
        motion = this->GetDisplacementFor( stateInfo, toTime ) - this->GetDisplacementFor( stateInfo, fromTime );

        // inverse movement
        if (stateInfo.GetBackwards())
        {
            motion *= -1;
        }
    }

    return motion;
}

//------------------------------------------------------------------------------
/**
*/
bool 
nCharacter2::HasDisplacement(int stateIndex) const
{
    n_assert(this->IsValidStateIndex(stateIndex));
    n_assert(this->animStateArray);

    nAnimState& animState = this->GetAnimStateArray()->GetStateAt(stateIndex);
    if (this->IsValidStateIndex(stateIndex) && animState.IsMotionValid())
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
vector3
nCharacter2::GetDisplacementFor(const nStateInfo& stateInfo, float atTime)
{
    int keyIndex[2];
    float inbetween;

    nAnimation::Group::LoopType loopType;

    nAnimState& animState = this->GetAnimStateArray()->GetStateAt(stateInfo.GetStateIndex());
    const nAnimation::Group& group = animState.GetMotion()->GetGroupAt(0);

    this->GetKeysFor(stateInfo, atTime, keyIndex, inbetween, group, loopType);

    /// to add  n * total animation distance
    int numberTimesPlayed = 0;
    vector3 totalAnimMotion;

    float relTime = atTime - stateInfo.GetStateStarted();
    
    if (relTime < 0)
    {
        relTime = 0;
    }

    if (stateInfo.GetRepeat())
    {
        numberTimesPlayed = static_cast<int>(relTime / this->GetStateDuration(stateInfo.GetStateIndex()));
        totalAnimMotion = this->GetTotalStateDisplacemmt(stateInfo.GetStateIndex());
    }

    return totalAnimMotion * float(numberTimesPlayed) + animState.GetMotionAt(keyIndex, inbetween);
}

//------------------------------------------------------------------------------
/**
*/
float
nCharacter2::GetKeysFor(const nStateInfo& stateInfo, float atTime, int* keyIndex, float& inbetween, const nAnimation::Group group, nAnimation::Group::LoopType& loopType)
{
    if (this->IsValidStateIndex(stateInfo.GetStateIndex()))
    {
        float relTime = atTime - stateInfo.GetStateStarted();

        if (relTime < 0)
        {
            relTime = 0;
        }

        if (stateInfo.GetBackwards())
        {
            int numberTimesPlayed = 0;
            if (stateInfo.GetRepeat())
            {
                numberTimesPlayed = static_cast<int>(relTime/this->GetStateDuration(stateInfo.GetStateIndex()));
            }
            relTime = static_cast<float>(this->GetStateDuration(stateInfo.GetStateIndex()))*(numberTimesPlayed+1) - relTime;
        }

        // check if repeat animation
        loopType = nAnimation::Group::Repeat;
        if (stateInfo.GetRepeat() == false)
        {
            loopType = nAnimation::Group::Clamp;
        }

        group.TimeToIndex(relTime, keyIndex[0], keyIndex[1], inbetween, loopType);

        return relTime;
    }

    return atTime;
}

//------------------------------------------------------------------------------
/**
*/
const vector3 
nCharacter2::GetTotalStateDisplacemmt(int stateIndex)
{
    return this->GetAnimStateArray()->GetStateAt(stateIndex).GetTotalMotionMotion();
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacter2::SetStateFollowsJoint(int stateIdx, int jointIdx)
{
    this->GetAnimStateArray()->GetStateAt(stateIdx).SetFollowJoint(jointIdx);
}

//------------------------------------------------------------------------------
/**
*/
int
nCharacter2::GetStateFollowsJoint(int stateIdx) const
{
    return this->GetAnimStateArray()->GetStateAt(stateIdx).GetFollowJoint();
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacter2::BeginTweakableJoints( int num )
{
    this->tweakableJointsArray.SetFixedSize(num);
    for( int i=0; i< num; i++)
    {
        this->tweakableJointsArray[i].InitializeData();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacter2::SetTweakableJoint(int index, int jointIndex, int parentIndex)
{
    this->tweakableJointsArray.At(index).AddTweableJoint(jointIndex, parentIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
nCharacter2::EndTweakableJoints()
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
int 
nCharacter2::GetNumTweakableJoints() const
{
    return this->tweakableJointsArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
int 
nCharacter2::GetNumJointInTweakableJoint(int index) const
{
    if( this->tweakableJointsArray.Size() > index )
    {
        return this->tweakableJointsArray[index].numJoints;
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
int 
nCharacter2::GetTweakableJointAt(int index, int twekIdx) const
{
    if( this->tweakableJointsArray.Size() > index  && this->tweakableJointsArray[index].numJoints > twekIdx )
    {
        return this->tweakableJointsArray[index].jointIndex[twekIdx];
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
int 
nCharacter2::GetParentTweakableJointAt(int index, int twekIdx) const
{
    if( this->tweakableJointsArray.Size() > index  && this->tweakableJointsArray[index].numJoints > twekIdx )
    {
        return this->tweakableJointsArray[index].parentIndex[twekIdx];
    }
    return -1;
}
