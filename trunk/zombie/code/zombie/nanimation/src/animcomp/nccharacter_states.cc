#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nccharacter_states.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nccharacter.h"
#include "animcomp/nccharacterclass.h"
#include "entity/nentityclassserver.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::LoadStatesArrays()
{
    if ( this->character.Size() > 0 )
    {
        int numGroups = this->character[0]->GetSkeleton().GetNumberJointGroups();
        this->activeStates.SetFixedSize(numGroups);
        this->currentStates.SetFixedSize(numGroups);
        this->previousStates.SetFixedSize(numGroups);
        for (int i=0; i< this->character[0]->GetSkeleton().GetNumberJointGroups(); i++)
        {
            this->activeStates[i].SetToDefault();
            this->currentStates[i].SetToDefault();
            this->previousStates[i].SetToDefault();
        }
    }

    if (this->firstpersonCharIdx != -1)
    {
        this->fpersonCurrentStates.SetFixedSize(1);
        this->fpersonPreviousStates.SetFixedSize(1);
        this->fpersonActiveStates.SetFixedSize(1);

        this->fpersonCurrentStates[0].SetToDefault();
        this->fpersonPreviousStates[0].SetToDefault();
        this->fpersonActiveStates[0].SetToDefault();
    }

    // preparate for active states
    for (int i=0; i< this->persistedStates.Size(); i++)
    {
        this->SetActiveStateByIndex(this->persistedStates[i].GetStateIndex(), this->persistedStates[i].GetBackwards(), this->persistedStates[i].GetRepeat(), this->persistedStates[i].GetTransition());
    }

    // preparate for first person active states
    for (int i=0; i< this->fpersonPersistedStates.Size(); i++)
    {
        this->SetFirstPersonActiveStateByIndex(this->fpersonPersistedStates[i].GetStateIndex(), this->fpersonPersistedStates[i].GetBackwards(), this->fpersonPersistedStates[i].GetRepeat(), this->fpersonPersistedStates[i].GetTransition(), this->fpersonPersistedStates[i].GetOffsetIndex(), this->fpersonPersistedStates[i].GetOffset());
    }

    if (this->persistedStates.Empty())
    {
        this->SetActiveStateByIndex(0);
    }
    else
    {
        this->persistedStates.Clear();
    }

    if (this->fpersonPersistedStates.Empty() && this->firstpersonCharIdx != -1)
    {
        this->SetFirstPersonActiveStateByIndex(0);
    }
    else
    {
        this->fpersonPersistedStates.Clear();
    }

}

//------------------------------------------------------------------------------
/**
    true if has not finished 
    NOT WORKING need data to know if first or third person, it's correct if asked for third person
*/
bool
ncCharacter::IsPlaying(int groupIndex)
{
    nTime curTime = nTimeServer::Instance()->GetFrameTime();

    //return this->GetCharacter()->IsPlaying(curTime, groupIndex);
    if (!this->currentStates[groupIndex].IsValid())
    {
        return false;
    }

    int activeStateIndex = this->currentStates[groupIndex].GetStateIndex();
    // if loop animation
    if (this->character[this->lastUpdatedCharacter]->GetAnimStateArray()->GetStateAt(activeStateIndex).GetAnim()  &&
        this->character[this->lastUpdatedCharacter]->GetAnimStateArray()->GetStateAt(activeStateIndex).GetAnim()->GetGroupAt(0).GetLoopType() == nAnimation::Group::Repeat)
    {
        return true;
    }

    //else
    nTime willEnd = this->currentStates[groupIndex].GetStateStarted() + this->GetStateDuration(this->currentStates[groupIndex].GetStateIndex(), false);
    return (curTime - willEnd)< 0;
}

//------------------------------------------------------------------------------
/**
    it doesn't return correct values if fperson, gives last updated character information
*/
bool
ncCharacter::IsLoopedByIndex(int stateIndex)
{
    return this->character[this->lastUpdatedCharacter]->GetAnimStateArray()->GetStateAt(stateIndex).IsLooped();
}

//------------------------------------------------------------------------------
/**
    it doesn't return correct values if fperson, gives last updated character information
*/
bool
ncCharacter::IsLoopedByName(const char * stateName)
{
    int stateIndex = this->character[this->lastUpdatedCharacter]->FindStateIndexByName(nString(stateName));
    return this->character[this->lastUpdatedCharacter]->GetAnimStateArray()->GetStateAt(stateIndex).IsLooped();
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::UpdateActiveStates(nTime curTime)
{
    //if it's first person
    if (this->firstpersonCharIdx != -1)
    {
        this->UpdateActiveStatesArrays(this->fpersonActiveStates, this->fpersonCurrentStates, curTime, true);
    }

    //always test third person
    this->UpdateActiveStatesArrays(this->activeStates, this->currentStates, curTime, false);
}

//------------------------------------------------------------------------------
/**
    Set a new animation state, and handle stuff necessary for
    blending between previous and current state.
*/
void
ncCharacter::SetActiveStateByGroup(int jointGroup, int stateIndex, nTime time, bool transition, bool repeat, bool backwards, int jointIdx, const vector3& offset, float velFactor, bool fperson)
{
    NLOG(animation, ( NLOG1 | 0, "SetActiveStateByGroup entity: %s, jointGroup: %d, stateIndex: %d (%s)", 
         this->GetEntityClass()->GetName(), jointGroup, stateIndex,
         this->GetEntityClass()->GetComponent<ncCharacterClass>()->GetSkeletonClassPointer()->GetStateName(stateIndex)));

    nArray<nStateInfo>* current = 0;
    nArray<nStateInfo>* previous = 0;
    nArray<nStateInfo>* active = 0;

    if (fperson)
    {
        current = &this->fpersonCurrentStates;
        previous = &this->fpersonPreviousStates;
        active = &this->fpersonActiveStates;
    }
    else
    {
        current = &this->currentStates;
        previous = &this->previousStates;
        active = &this->activeStates;
    }

    //there's a valid current state
    if (current->At(jointGroup).IsValid())
    {
        previous->At(jointGroup).SetData(current->At(jointGroup).GetStateIndex(), current->At(jointGroup).GetBackwards(), 
                                         current->At(jointGroup).GetRepeat(), current->At(jointGroup).GetTransition(), 
                                         current->At(jointGroup).GetOffsetIndex(), current->At(jointGroup).GetOffset(),
                                         current->At(jointGroup).GetVelocityFactor(), current->At(jointGroup).GetStateStarted());

        // only synchronize partial animations
        if (jointGroup != 0)
        {
            //synchronize only when where leader-submissive animations, ifnot, use current time
            bool submissive = false;
            if (!fperson)
            {
                submissive = this->character[0]->GetLeaderState(stateIndex) != -1;
            }

            if (submissive)
            {
                current->At(jointGroup).SetData(stateIndex, backwards, repeat, transition, jointIdx, offset, velFactor, previous->At(jointGroup).GetStateStarted());
            }
            else
            {
                current->At(jointGroup).SetData(stateIndex, backwards, repeat, transition, jointIdx, offset, velFactor, float(time));
            }
        }
        else
        {
            current->At(jointGroup).SetData(stateIndex, backwards, repeat, transition, jointIdx, offset, velFactor, float(time));
        }
    }
    // no valid current state
    else
    {
        for (int i=0; i< current->Size(); i++)
        {
            previous->At(i) = current->At(i);
        }
        current->At(jointGroup).SetData(stateIndex, backwards, repeat, transition,jointIdx, offset, velFactor, float(time));
    }

    // if not full skeleton state, set to false full skeleton animations
    if (jointGroup != 0)
    {
        current->At(0).SetToDefault();
    }
    else
    {
        // if full skeleton state, set to false partial skeleton animations
        for (int i=1; i< current->Size(); i++)
        {
            current->At(i).SetToDefault();
        }
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/** 
    when removed from skeleton class it's removed from ncharacter2 too.

    Return true if the animation has been deleted successfully, false otherwise
    (active animations cannot be deleted).

    can be problems if different order animations for skeletons
*/
bool
ncCharacter::RemoveAnimState(bool fperson, int stateIndex)
{
    //first person
    if (fperson)
    {
        //skeleton data
        ncCharacterClass *charClass = this->GetClassComponent<ncCharacterClass>();
        n_assert(charClass);

        ncSkeletonClass *skelComp = charClass->GetSkeletonClassPointer(charClass->GetFirstPersonLevel());
        n_assert(skelComp);

        this->RemoveState(stateIndex, skelComp);
        return true;
    }

    //third person

    //skeleton data
    ncCharacterClass *charClass = this->GetClassComponent<ncCharacterClass>();

    ncSkeletonClass *skelComp = 0;
    if (charClass)
    {
        for (int i=0; i<charClass->GetNumberLevelSkeletons(); i++)
        {
            skelComp = charClass->GetSkeletonClassPointer(i);
            n_assert(skelComp);
            this->RemoveState(stateIndex, skelComp);
        }

        //update this->activeStates array
        for (int i=0; i< this->activeStates.Size(); i++)
        {
            if (this->activeStates[i].GetStateIndex() >= stateIndex)
            {
                this->activeStates[i].SetStateIndex(this->activeStates[i].GetStateIndex() -1);
            }
        }
    }
    else
    {
        for (int i=0; i<this->character.Size(); i++)
        {
            if (this->firstpersonCharIdx != i)
            {
                if (i==0)
                {
                    skelComp = this->GetClassComponent<ncSkeletonClass>();
                    this->RemoveState(stateIndex, skelComp);
                }
                else
                {
                    nString lowLevelSklName = this->GetEntityClass()->GetName();
                    lowLevelSklName = lowLevelSklName.ExtractRange(0, lowLevelSklName.Length() - 1);
                    lowLevelSklName += i;

                    nEntityClass* entityClass = nEntityClassServer::Instance()->GetEntityClass(lowLevelSklName.Get());
                    if (entityClass)
                    {
                        skelComp = entityClass->GetComponent<ncSkeletonClass>();
                        n_assert(skelComp);
                        this->RemoveState(stateIndex, skelComp);
                    }
                }
            }
        }

        //update this->activeStates array
        for (int i=0; i< this->activeStates.Size(); i++)
        {
            if (this->activeStates[i].GetStateIndex() >= stateIndex)
            {
                this->activeStates[i].SetStateIndex(this->activeStates[i].GetStateIndex() -1);
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/** 
*/
bool
ncCharacter::RemoveState(int stateIndex, ncSkeletonClass* skelComp)
{
    n_assert(skelComp);

    //if it's not active
    int index=0;
    for (; index< this->activeStates.Size(); index++)
    {
        if (this->activeStates[index].GetStateIndex() == stateIndex)
        {
            break;
        }
    }

    if (index == this->activeStates.Size())
    {
        skelComp->RemoveAnimState(stateIndex);


        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    when changed in skeleton class it's changed in ncharacter2 too.
    it's not changed in fperson animations only third person
*/
bool
ncCharacter::ChangeAnimStateName(bool fperson, int stateIndex, const nString& stateName)
{
    //first person
    if (fperson)
    {
        //skeleton data
        ncCharacterClass *charClass = this->GetClassComponent<ncCharacterClass>();
        n_assert(charClass);

        ncSkeletonClass *skelComp = charClass->GetSkeletonClassPointer(charClass->GetFirstPersonLevel());
        n_assert(skelComp);

        skelComp->ChangeAnimStateName(stateIndex, stateName);
        return true;
    }

    ///third person

    //skeleton data
    ncCharacterClass *charClass = this->GetClassComponent<ncCharacterClass>();

    ncSkeletonClass *skelComp = 0;
    if (charClass)
    {
        for (int i=0; i<charClass->GetNumberLevelSkeletons(); i++)
        {
            skelComp = charClass->GetSkeletonClassPointer();
            n_assert(skelComp);
            skelComp->ChangeAnimStateName(stateIndex, stateName);
        }        
    }
    else
    {
        for (int i=0; i<this->character.Size(); i++)
        {
            if (this->firstpersonCharIdx != i)
            {
                if (i==0)
                {
                    skelComp = this->GetClassComponent<ncSkeletonClass>();
                    n_assert(skelComp);
                    skelComp->ChangeAnimStateName(stateIndex, stateName);
                }
                else
                {
                    nString lowLevelSklName = this->GetEntityClass()->GetName();
                    lowLevelSklName = lowLevelSklName.ExtractRange(0, lowLevelSklName.Length() - 1);
                    lowLevelSklName += i;

                    nEntityClass* entityClass = nEntityClassServer::Instance()->GetEntityClass(lowLevelSklName.Get());
                    if (entityClass)
                    {
                        skelComp = entityClass->GetComponent<ncSkeletonClass>();
                        n_assert(skelComp);
                        skelComp->ChangeAnimStateName(stateIndex, stateName);
                    }
                }
            }
        }
    }
    
    return true;
}
#endif

//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::SetFadeInTime(bool fperson, int stateIndex, float fade)
{
    //skeleton data
    ncCharacterClass *charClass = this->GetClassComponent<ncCharacterClass>();

    ncSkeletonClass *skelComp = 0;
    if (charClass)
    {
        if (fperson)
        {
            skelComp = charClass->GetSkeletonClassPointer(this->firstpersonCharIdx);
            n_assert(skelComp);
            skelComp->SetFadeInTime(stateIndex, fade);
        }
        else
        {
            for (int i=0; i < charClass->GetNumberLevelSkeletons(); i++)
            {
                if (this->firstpersonCharIdx != i)
                {
                    skelComp = charClass->GetSkeletonClassPointer(i);
                    n_assert(skelComp);
                    skelComp->SetFadeInTime(stateIndex, fade);
                }
            }
        }
    }
    else
    {
        ///if we are visualizating an skeleton or a set of skeletons, it's not possible to have first person character
        for (int i=0; i<this->character.Size(); i++)
        {
            if (this->firstpersonCharIdx != i)
            {
                if (i==0)
                {
                    skelComp = this->GetClassComponent<ncSkeletonClass>();
                    skelComp->SetFadeInTime(stateIndex, fade);
                }
                else
                {
                    nString lowLevelSklName = this->GetEntityClass()->GetName();
                    lowLevelSklName = lowLevelSklName.ExtractRange(0, lowLevelSklName.Length() - 1);
                    lowLevelSklName += i;

                    nEntityClass* entityClass = nEntityClassServer::Instance()->GetEntityClass(lowLevelSklName.Get());
                    if (entityClass)
                    {
                        skelComp = entityClass->GetComponent<ncSkeletonClass>();
                        n_assert(skelComp);
                        skelComp->SetFadeInTime(stateIndex, fade);
                    }
                }
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
*/
float
ncCharacter::GetFadeInTime(bool fperson, int stateIndex) const
{
    nTime time(0);

    if (fperson)
    {
        time = this->character[this->firstpersonCharIdx]->GetStateFadeInTime(stateIndex);
    }
    else
    {
        time = this->character[0]->GetStateFadeInTime(stateIndex);
    }

    return float(time);
}

//------------------------------------------------------------------------------
/**
*/
float
ncCharacter::GetStateDuration(int stateIndex, bool fperson) const
{
    if( stateIndex < 0 )
    {
        return 0.0f;
    }

    int jointGroup = this->GetJointGroup(stateIndex, fperson);
    float stateDuration = 0;

    if (fperson)
    {
        stateDuration = (float) this->character[this->firstpersonCharIdx]->GetStateDuration(stateIndex);
    }
    else
    {
        stateDuration = (float) this->character[this->GetPhysicsSkelIndex()]->GetStateDuration(stateIndex);
    }

    return stateDuration /  this->currentStates[jointGroup].GetVelocityFactor();
}
 
//------------------------------------------------------------------------------
/**
*/
float
ncCharacter::GetElapsedTime(int stateIndex, bool fperson) const
{
    if( stateIndex < 0 )
    {
        return 0.0f;
    }

    // get the sample time from scene component
    n_assert( this->GetEntityObject()->GetComponent<ncScene>() );
    nTime curTime = nTimeServer::Instance()->GetFrameTime();

    int jointGroup = this->GetJointGroup(stateIndex, fperson);

    if (fperson)
    {
        return  (float) curTime - this->fpersonCurrentStates[jointGroup].GetStateStarted();
    }

    return  (float) curTime - this->currentStates[jointGroup].GetStateStarted();
}

//------------------------------------------------------------------------------
/**
*/
float
ncCharacter::GetRemainingTime(int stateIndex, bool fperson) const
{
    if( stateIndex < 0 )
    {
        return 0.0f;
    }

    nArray<nStateInfo> current;
    nArray<nStateInfo> previous;
    nArray<nStateInfo> active;
    int activeCharacter = 2;

    if (fperson)
    {
        current = this->fpersonCurrentStates;
        previous = this->fpersonPreviousStates;
        active = this->fpersonActiveStates;
        activeCharacter = this->firstpersonCharIdx;
    }
    else
    {
        current = this->currentStates;
        previous = this->previousStates;
        active = this->activeStates;
    }

    float curTime = (float)nTimeServer::Instance()->GetFrameTime();

    int jointGroup = this->character[activeCharacter]->GetAnimStateArray()->GetStateAt(stateIndex).GetJointGroup();

    float remaining = 0.0f; //any positive value is good

    n_assert(current[jointGroup].GetStateIndex() == stateIndex);
    if (current[jointGroup].GetStateIndex() == stateIndex)
    {
        float relTime = curTime - current[jointGroup].GetStateStarted();
        float finalTime = this->GetStateDuration(stateIndex, fperson);

        //if looped animation return remaing with respect to current loop
        if (current[jointGroup].GetRepeat())
        {
            finalTime *= static_cast<int>(relTime/this->GetStateDuration(stateIndex, fperson)) +1;
            remaining = finalTime - relTime;
        }
        else
        {
            //if finished
            if (finalTime < relTime)
            {
                remaining = 0.0f;
            }
            //if not finished
            else
            {
                remaining = finalTime - relTime;
            }
        }
    }

    //NLOGCOND(animation, (strcmp(this->GetEntityClass()->GetName(), "Localplayer") == 0),  (1 , "GetRemainingTime fperson: %d\t stateIdx: %i\t started: %f\t duration: %f\t curTime: %f\t remaining: %f", fperson, stateIndex, current[jointGroup].GetStateStarted(), this->GetStateDuration(stateIndex, fperson), curTime, remaining));
    NLOG(animation, (NLOG1 | 2, "GetRemainingTime stateindex = %d, fperson= %d remaining = %f", stateIndex, fperson, remaining));

    return remaining;
}

//------------------------------------------------------------------------------
/**
    Returns the the time an animation state started in seconds.
*/
nTime 
ncCharacter::GetCurrentStateStarted(int groupIndex) const
{
    return this->currentStates[groupIndex].GetStateStarted();
}

//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::IsValidStateIndex(int i) const
{
    return ((i >=0) && (i < this->character[this->GetPhysicsSkelIndex()]->GetAnimStateArray()->GetNumStates()));
}
//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::IsValidFirstPersonStateIndex(int i) const
{
    return ((i >=0) && (i < this->character[this->firstpersonCharIdx]->GetAnimStateArray()->GetNumStates()));
}
//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::LoopJustStarted(int stateIndex, bool fperson) const
{
    NLOG(animation, ( NLOG1 | 2, "LoopJustStarted stateindex = %d, fperson= %d", stateIndex, fperson));

    int jointGroup = this->GetJointGroup(stateIndex, fperson);

    if (!fperson)
    {
        //NLOG(animation, (1, "LoopJustStarted 3person currentStateIndex = %d, stateindex = %d, jointGroup= %d, justStarted = %d ", this->currentStates[jointGroup].GetStateIndex(), stateIndex, jointGroup, this->currentStates[jointGroup].GetJustStarted()));
        return (this->currentStates[jointGroup].GetStateIndex() == stateIndex) && this->currentStates[jointGroup].GetJustStarted();
    }
    else
    {
        //NLOG(animation, (1, "LoopJustStarted 1person currentStateIndex = %d, stateindex = %d, jointGroup= %d, justStarted = %d ", this->fpersonCurrentStates[jointGroup].GetStateIndex(), stateIndex, jointGroup, this->fpersonCurrentStates[jointGroup].GetJustStarted()));
        return (this->fpersonCurrentStates[jointGroup].GetStateIndex() == stateIndex) && this->fpersonCurrentStates[jointGroup].GetJustStarted();
    }
}

//------------------------------------------------------------------------------
/**
    NOT USED
*/
int 
ncCharacter::FindLodAnimIdxFromFirstPersonIdx(int fpersonanimIdx, int charIdx)
{
    if (fpersonanimIdx < this->character[this->firstpersonCharIdx]->GetAnimStateArray()->GetNumStates())
    {
        nString fpanimStateName = this->character[this->firstpersonCharIdx]->GetAnimStateArray()->GetStateAt(fpersonanimIdx).GetName();

        for (int i=0; i< this->character[charIdx]->GetAnimStateArray()->GetNumStates(); i++)
        {
            nString animSName = this->character[charIdx]->GetAnimStateArray()->GetStateAt(i).GetName();
            nString fName(fpanimStateName.ExtractRange (7, fpanimStateName.Length() - 7));
            nString tName(animSName.ExtractRange (8, animSName.Length() - 8));

            // must match the name and the state
            if (fName  == tName)
            {
                return i;
            }
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
/**
*/
int
ncCharacter::GetNumberStates() const
{
    const ncCharacterClass* charClass = this->GetClassComponent<ncCharacterClass>();

    if ( charClass->GetSkeletonClassPointer() )
    {
        return charClass->GetSkeletonClassPointer()->GetNumStates();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
ncCharacter::GetFirstPersonNumberStates() const
{
    const ncCharacterClass* charClass = this->GetClassComponent<ncCharacterClass>();
    
    if (charClass && charClass->GetFirstPersonLevel() != -1)
    {
        return charClass->GetSkeletonClassPointer(charClass->GetFirstPersonLevel())->GetNumStates();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::UpdateActiveStatesArrays(nArray<nStateInfo>& active, nArray<nStateInfo>& current, nTime curTime, bool fperson)
{
    for (int i = 0; i< active.Size(); i++)
    {
        if ((active[i].GetStateIndex() != current[i].GetStateIndex()) || 
            (active[i].GetBackwards() != current[i].GetBackwards()) || 
            (active[i].GetRepeat() != current[i].GetRepeat()) || 
            (!active[i].GetOffset().isequal(current[i].GetOffset(), 0.00001f)) ||
            (active[i].GetVelocityFactor() != current[i].GetVelocityFactor()))
        {
            int affectedCharacter = 0;
            int stateIdx = active[i].GetStateIndex();

            int jointIdx = active[i].GetOffsetIndex();
            vector3 offset = active[i].GetOffset();
            if (fperson) //first person
            {
                affectedCharacter = this->firstpersonCharIdx;
            }
            else //third person (group synchronization, for first person is not necessary because there's only one group)
            {
                if ((stateIdx != -1) && (this->character[affectedCharacter]->GetLeaderState(stateIdx) != -1))
                {
                    int leaderState = this->character[affectedCharacter]->GetLeaderState(stateIdx);
                    int leaderJointGroup = this->character[affectedCharacter]->GetAnimStateArray()->GetStateAt(leaderState).GetJointGroup();
                    
                    if (current[leaderJointGroup].GetStateIndex() == leaderState)
                    {
                        curTime = current[leaderJointGroup].GetStateStarted();
                    }
                }
            }

            if (this->character[affectedCharacter]->IsValidStateIndex(stateIdx))
            {
                this->SetActiveStateByGroup(i, stateIdx, curTime, active[i].GetTransition(), active[i].GetRepeat(), active[i].GetBackwards(), jointIdx, offset, active[i].GetVelocityFactor(), fperson);
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::SetSpeedFactor(int stateIndex, float speedFactor, bool fperson)
{
    if ( stateIndex == -1 )
    {
        return;
    }

    int jointGroup = this->GetJointGroup(stateIndex, fperson);

    if (fperson && this->fpersonCurrentStates[jointGroup].GetStateIndex() == stateIndex) 
    {
        this->fpersonCurrentStates[jointGroup].SetVelocityFactor(speedFactor);
    }

    if (!fperson && this->currentStates[jointGroup].GetStateIndex() == stateIndex)
    {
        this->currentStates[jointGroup].SetVelocityFactor(speedFactor);
    }

    NLOG(animation, (NLOG1 | 1, "SetSpeedFactor That index is not active!!! stateIndex= %d, fperson= %d", stateIndex, fperson));
}

//------------------------------------------------------------------------------
/**
*/
float
ncCharacter::GetSpeedFactor(int stateIndex, bool fperson) const
{
    int jointGroup = this->GetJointGroup(stateIndex, fperson);

    if (fperson && this->fpersonCurrentStates[jointGroup].GetStateIndex() == stateIndex) 
    {
        return this->fpersonCurrentStates[jointGroup].GetVelocityFactor();
    }

    if (!fperson && this->currentStates[jointGroup].GetStateIndex() == stateIndex)
    {
        return this->currentStates[jointGroup].GetVelocityFactor();
    }

    return 1.0f;
}

//------------------------------------------------------------------------------
/**
    Update look-at quaternion in all active states
*/
void
ncCharacter::UpdateActiveStatesLookAt(nArray<nStateInfo>& currentStates)
{
    for (int i = 0; i < currentStates.Size(); ++i)
    {
        nStateInfo& stateInfo = currentStates[i];
        if (stateInfo.GetStateIndex() != -1 && stateInfo.HasJointLookAt())
        {
            // convert look-at world vector to model space
            const vector3& lookAtVector = stateInfo.GetJointLookAtVector();
            quaternion invQuat = this->GetComponentSafe<ncTransform>()->GetQuat();
            invQuat.conjugate();
            vector3 modelLookAt(invQuat.rotate(lookAtVector));

            quaternion lookAtQuat;
            lookAtQuat.set_from_axes(vector3(0.0f, 0.0f, 1.0f), modelLookAt);

            stateInfo.SetJointLookAtQuat(lookAtQuat);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
const vector3
ncCharacter::GetDisplacement(int stateIndex, float fromTime, float toTime) const
{
    int jointGroup = this->GetJointGroup(stateIndex, false);

    // TODO: This is an ugly hack!
    // n_assert(this->currentStates[jointGroup].GetStateIndex() == stateIndex);
    if(this->currentStates[jointGroup].GetStateIndex() == stateIndex)
    {
        /// @todo ma.garcias - with 0!!
        const vector3 value = this->character[0]->GetStateDisplacement(this->currentStates[jointGroup], fromTime, toTime);
        NLOG(animation, (NLOG1 | 1, "GetDisplacement entity: %s, fromTime: %.2f, toTime: %.2f, value: (%.2f, %.2f, %.2f)", 
             this->GetEntityClass()->GetName(), fromTime, toTime, value.x, value.y, value.z))
        return value;
    }
    return vector3(); 
}

//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::HasDisplacement(int stateIndex) const
{
    return this->character[0]->HasDisplacement(stateIndex);
}
