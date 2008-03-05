#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nccharacter_setstates.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nccharacter.h"
#include "kernel/nlogclass.h"

//---------------------------------------------------------------------------
//------------------------SET THIRD PERSON ACTIVE STATES---------------------
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    Set current state for internal third-person state(s).
    It assumes that the state index is valid for all third-person characters.

    @param  stateIndex  index of state in third-person skeleton(s)
    @param  backwards   reverse the animation
    @param  repeat      restart animation at end of cycle
    @param  transi      perform state transition
    @param  speedFactor factor to multiply the animation speed

    history:
        - 17-Mar-2006   ma.garcias  Update third person state(s) immediately.
*/
void
ncCharacter::SetActiveStateByIndex(int stateIndex, bool backwards, bool repeat, bool transi, float speedFactor)
{
    if ( this->character.Empty() ) return;

    n_assert(this->character[0]);
    if (stateIndex < this->character[0]->GetAnimStateArray()->GetNumStates())
    {
        //NLOG(animation, (0, "SetActiveStateByIndex entityobj: %s\t stateIdx: %i\t backwards: %i\t repeat: %i\t transition: %i\t",this->GetEntityClass()->GetName(), stateIndex, backwards, repeat, transi));
        nAnimState& animState = this->character[0]->GetAnimStateArray()->GetStateAt(stateIndex);

        if (speedFactor <= 0)
        {
            speedFactor = 1.0f;
        }

        this->activeStates[animState.GetJointGroup()].SetData(stateIndex, backwards, repeat, transi, -1, vector3(), speedFactor);

        //if the state selected is not full-skeleton, restart full-skeleton state
        if (animState.GetJointGroup() != 0)
        {
            // copy joint look-at from previous full-body state
            // TODO- temporary hack, joint index will be a lod-independent index
            int jointIndex = this->activeStates[0].GetJointLookAtIndex();
            if (jointIndex != -1 && this->character[0]->GetSkeleton().GetJointGroupAt(animState.GetJointGroup()).FindJoint(jointIndex) != -1)
            {
                const vector3& lookAtVector = this->activeStates[0].GetJointLookAtVector();
                this->activeStates[animState.GetJointGroup()].SetJointLookAtVector(jointIndex, lookAtVector);
            }
            this->activeStates[0].SetToDefault();
        }
        else
        {
            for (int i = 1; i < this->activeStates.Size(); i++)
            {
                // copy current look-at joint and vector from any state that may have it valid
                int jointIndex = this->activeStates[i].GetJointLookAtIndex();
                if (this->activeStates[i].HasJointLookAt())
                {
                    this->activeStates[0].SetJointLookAtVector(jointIndex, this->activeStates[i].GetJointLookAtVector());
                }
                this->activeStates[i].SetToDefault();
            }
        }
    }

    //this->UpdateActiveStates(this->GetComponentSafe<ncScene>()->GetTime());
    nTime curTime = nTimeServer::Instance()->GetFrameTime();
    this->UpdateActiveStatesArrays(this->activeStates, this->currentStates, curTime, false);
}

//------------------------------------------------------------------------------
/**
    Set current state, used for scripting
*/
void
ncCharacter::SetActiveStateIndex(int stateIndex, bool backwards, bool repeat, bool transi)
{
    this->SetActiveStateByIndex(stateIndex, backwards, repeat, transi, 1.0f);
}

//------------------------------------------------------------------------------
/**
    Set active state by name
*/
void
ncCharacter::SetActiveState(const char * stateName, bool backwards, bool repeat, bool transi, float speedFactor)
{
    int stateIdx = this->character[0]->GetAnimStateArray()->FindStateIndex(stateName);
    if (this->IsValidStateIndex(stateIdx))
    {
        this->SetActiveStateByIndex(stateIdx, backwards, repeat, transi, speedFactor);
    }
}

//------------------------------------------------------------------------------
/**
    Set active state by name, used for scripting
*/
void
ncCharacter::SetActiveStateName(const char * stateName, bool backwards, bool repeat, bool transi)
{
    this->SetActiveState(stateName, backwards, repeat, transi, 1.0f);
}

//------------------------------------------------------------------------------
/**
    only for persistence
*/
void
ncCharacter::SetActiveAnimState(int animState, bool backwards, bool repeat, bool transi)
{
    this->persistedStates.Append(nStateInfo(animState, backwards, repeat, transi));
}

//------------------------------------------------------------------------------
/**
    only for persistence
*/
void
ncCharacter::SetActiveAnimStateWithSpeedFactor(int animState, bool backwards, bool repeat, bool transi, float speedFactor)
{
    this->persistedStates.Append(nStateInfo(animState, backwards, repeat, transi, speedFactor));
}

//---------------------------------------------------------------------------
//------------------------SET FIRST PERSON ACTIVE STATES---------------------
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    Set current state for first-person character, if any.

    @param  stateIndex  index of first-person state, must be a valid index.
    @param  backwards   reverse the animation
    @param  repeat      restart animation at end of cycle
    @param  transition  use transition from current state
    @param  jointIdx    index of joint to apply offset
    @param  offset      vector offset for the joint at jointIdx
    @param  speedFactor factor to multiply the animation speed

    history:
        - 17-Mar-2006   ma.garcias  Update first person state immediately.
*/
void
ncCharacter::SetFirstPersonActiveStateByIndex(int stateIndex, bool backwards, bool repeat, bool transition, int jointIdx, const vector3& offset, float speedFactor)
{
    n_assert(this->firstpersonCharIdx != -1);
    n_assert(this->character[this->firstpersonCharIdx]);

    if (stateIndex < this->character[this->firstpersonCharIdx]->GetAnimStateArray()->GetNumStates())
    {
        //NLOG(animation, (0 , "SetActiveStateByIndex entityobj: %s\t stateIdx: %i\t backwards: %i\t repeat: %i\t transition: %i\t",this->GetEntityClass()->GetName(), stateIndex, backwards, repeat, transi));
        nAnimState& animState = this->character[this->firstpersonCharIdx]->GetAnimStateArray()->GetStateAt(stateIndex);
        this->fpersonActiveStates[animState.GetJointGroup()].SetData(stateIndex, backwards, repeat, transition, jointIdx, offset, speedFactor);

        if (animState.GetJointGroup() != 0) 
        {
            this->fpersonActiveStates[0].SetToDefault();
        }
        else
        {
            for (int i = 1; i < this->fpersonActiveStates.Size(); i++)
            {
                this->fpersonActiveStates[i].SetToDefault();
            }
        }
    }

    nTime curTime = nTimeServer::Instance()->GetFrameTime();
    this->UpdateActiveStatesArrays(this->fpersonActiveStates, this->fpersonCurrentStates, curTime, true);
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::SetFirstPersonActiveStateByIndex(int stateIndex, bool backwards, bool repeat, bool transition, float speedFactor)
{
    this->SetFirstPersonActiveStateByIndex(stateIndex, backwards, repeat, transition, fpersonOffsetJointIdx, fpersonOffset, speedFactor);
}

//------------------------------------------------------------------------------
/**
    used for scripting
*/
void
ncCharacter::SetFirstPersonActiveStateIndex(int stateIndex, bool backwards, bool repeat, bool transition)
{
    this->SetFirstPersonActiveStateByIndex(stateIndex, backwards, repeat, transition, 1.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::SetFirstPersonActiveState(const char* stateName, bool backwards, bool repeat, bool transition, float speedFactor)
{
    this->SetFirstPersonActiveState(stateName, backwards, repeat, transition, -1, vector3(0,0,0), speedFactor);
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::SetFirstPersonActiveState(const char* stateName, bool backwards, bool repeat, bool transition, int jointIdx, const vector3& offset, float speedFactor)
{
    int stateIdx = this->character[this->firstpersonCharIdx]->GetAnimStateArray()->FindStateIndex(stateName);
    if (this->IsValidFirstPersonStateIndex(stateIdx))
    {
        this->SetFirstPersonActiveStateByIndex(stateIdx, backwards, repeat, transition,jointIdx, offset, speedFactor);
    }
}

//------------------------------------------------------------------------------
/**
    used for scripting
*/
void
ncCharacter::SetFirstPersonActiveStateName(const char* stateName, bool backwards, bool repeat, bool transition)
{
    this->SetFirstPersonActiveState(stateName, backwards, repeat, transition, 1.0f);
}

//------------------------------------------------------------------------------
/**
    only for persistance (kept for backwards compatibility)
*/
void
ncCharacter::SetFirstPersonActiveAnimState(int stateIndex, bool backwards, bool repeat, bool transition)
{
    this->fpersonPersistedStates.Append(nStateInfo(stateIndex, backwards, repeat, transition));
}

//------------------------------------------------------------------------------
/**
    only for persistance
*/
void
ncCharacter::SetFirstPersonActiveAnimStateWithSpeedFactor(int stateIndex, bool backwards, bool repeat, bool transition, float speedFactor)
{
    this->fpersonPersistedStates.Append(nStateInfo(stateIndex, backwards, repeat, transition, speedFactor));
}

//---------------------------------------------------------------------------
//------------------------GET ACTIVE STATES----------------------------------
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
*/
nArray<int>
ncCharacter::GetActiveStateIndices()
{
    nArray<int> indexArray;
    for (int i=0; i< this->activeStates.Size(); i++)
    {
        indexArray.Append(this->activeStates[i].GetStateIndex());
    }
    return indexArray;
}

//------------------------------------------------------------------------------
/**
*/
nArray<const char*>
ncCharacter::GetActiveStates()
{
    nArray<const char*> activeStateNames;
    for (int i=0; i< this->activeStates.Size(); i++)
    {
        activeStateNames.Append(this->character[this->lastUpdatedCharacter]->GetAnimStateArray()->GetStateAt(this->activeStates[i].GetStateIndex()).GetName().Get());
    }
    return activeStateNames;
}

//------------------------------------------------------------------------------
/**
*/
int
ncCharacter::GetFirstPersonActiveStateIndex()
{
    return this->fpersonActiveStates[0].GetStateIndex();
}

//------------------------------------------------------------------------------
/**
*/
const char* 
ncCharacter::GetFirstPersonActiveState()
{
    return this->character[this->firstpersonCharIdx]->GetAnimStateArray()->GetStateAt(this->fpersonActiveStates[0].GetStateIndex()).GetName().Get();
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::SetFirstPersonOffset(int skelJointIdx, const vector3& offset)
{
    this->fpersonOffset = offset;
    this->fpersonOffsetJointIdx = skelJointIdx;
}

//------------------------------------------------------------------------------
/**
*/
const vector3&
ncCharacter::GetFirstPersonOffset() const
{
    return this->fpersonOffset;
}

//------------------------------------------------------------------------------
/**
*/
int
ncCharacter::GetFirstPersonOffsetIndex() const
{
    return this->fpersonOffsetJointIdx;
}

//------------------------------------------------------------------------------
/**
    Force a given joint to point in the given direction vector.

    @param  jointIndex  index of joint to rotate
    @param  lookAt      look-at vector in world coordinates
*/
void
ncCharacter::SetJointLookAtVector(int jointIndex, const vector3& lookAt)
{
    this->jointLookAtIndex = jointIndex;
    this->jointLookAtVector = lookAt;

    // set lookat to current state for the joint group to which the joint belongs
    // if it has some state, or the full-body state if it has not
    for (int i = 0; i < this->currentStates.Size(); ++i)
    {
        // find the joint group in a valid state
        if (jointIndex != -1 && this->currentStates[i].GetStateIndex() != -1)
        {
            //HACK- check if the joint is in the high-detail skeleton only
            //this is a temporary hack, joint index will be lod-independent
            if (this->currentStates[i].GetJointLookAtIndex() == jointIndex ||
                this->character[0]->GetSkeleton().GetJointGroupAt(i).FindJoint(jointIndex) != -1)
            {
                this->currentStates[i].SetJointLookAtVector(jointIndex, lookAt);
                continue;
            }
        }
        // disable look-at in all other cases
        this->currentStates[i].SetJointLookAtVector(-1, vector3());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::GetJointLookAtVector(int& jointIndex, vector3& lookAt)
{
    jointIndex = this->jointLookAtIndex;
    lookAt = this->jointLookAtVector;
}
