#include "precompiled/pchnanimation.h"
//------------------------------------------------------------------------------
//  nccharacter_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "animcomp/nccharacter.h"
#include "animcomp/nccharacterclass.h"
#include "animcomp/ncskeletonclass.h"
#include "animcomp/nchumragdoll.h"
#include "ncragdoll/ncragdoll.h"
#include "animcomp/ncfourleggedragdoll.h"
#include "nragdollmanager/nragdollmanager.h"

//------------------------------------------------------------------------------
#include "entity/nentityobjectserver.h"
#include "entity/nentityclassserver.h"

#include "nscene/ncscenelod.h"
//------------------------------------------------------------------------------
#include "zombieentity/ncdictionary.h"
#include "nspatial/ncspatial.h"
#include "kernel/nlogclass.h"
//------------------------------------------------------------------------------
int ncCharacter::nextKey = 0;

//------------------------------------------------------------------------------
nNebulaComponentObject(ncCharacter,nComponentObject);

//------------------------------------------------------------------------------
/**
*/
ncCharacter::ncCharacter() :
    character(0,2),
    lastUpdatedCharacter(0),
    lastUpdatedTime(0),
    key(nextKey++),
    activeStates(0,1),
    persistedStates(0,1),
    currentStates(0,1),
    previousStates(0,1),
    attachJointName(0,1),
    entityObjId(0,1),
    pause(false),
    dirtyMorphState(true),
    activeMorphState(-1),
    ragdollActive(false),
    dynamicAttachmentPool(0,0),
    fpersonAttachmentPool(0,0),
    firstpersonCharIdx(-1),
    fpersonCurrentStates(0,1),
    fpersonPreviousStates(0,1),
    fpersonActiveStates(0,1),
    fpersonPersistedStates(0,1),
    firstpersonActive(false),
    humanoidRagdoll(false),
    updateAttachments(true),
    jointLookAtIndex(-1)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
ncCharacter::~ncCharacter()
{
    if (this->IsValid())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::InitInstance(nObject::InitInstanceMsg N_IFNDEF_NGAME(initType))
{
#ifndef NGAME
    if (initType != nObject::ReloadedInstance)
    {
        // Register to EnterLimbo and ExitLimbo signals
        this->entityObject->BindSignal(NSIGNAL_OBJECT(nEntityObject, EnterLimbo), this, &ncCharacter::DoEnterLimbo, 0);
        this->entityObject->BindSignal(NSIGNAL_OBJECT(nEntityObject, ExitLimbo), this, &ncCharacter::DoExitLimbo, 0);
    }
#endif

}

//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::UpdateCharacter(int charIndex, nTime curTime)
{
    NLOG(animation, (NLOG1|1 , "---------------------------------------------------------------------------------"));
    NLOG(animation, (NLOG1|1 , "UpdateCharacter entityobject: %s\t charIndex: %d curTime: %f", this->GetEntityClass()->GetName(), charIndex, (float) curTime));

    if ( charIndex < 0 || charIndex >= this->character.Size() ) return false;

    if (!this->pause)
    {
        if (!this->GetRagdollActive())
        {
            // animate character
            //n_assert(charIndex < this->character.Size());

            // store last updated character (will be physics one or the good one)
            this->lastUpdatedCharacter = charIndex;
            this->lastUpdatedTime = curTime;

            n_assert(this->character[charIndex]);

            //REMOVEME ma.garcias- active states automatically updated at all SetActiveState*** methods
            //this->UpdateActiveStates(curTime);

            // evaluate the current state of the character skeleton
            nVariableContext *varContext = &this->GetComponent<ncDictionary>()->VarContext();

            #if __NEBULA_STATS__
            nAnimationServer::Instance()->profAnim.StartAccum();
            #endif

            // choose which states must be used
            if (this->firstpersonCharIdx == charIndex)
            {
                this->character[charIndex]->EvaluateFullSkeleton((float) curTime, varContext, this->fpersonCurrentStates, this->fpersonPreviousStates);
            }
            else
            {
                this->UpdateActiveStatesLookAt(this->currentStates);
                this->character[charIndex]->EvaluateFullSkeleton((float) curTime, varContext, this->currentStates, this->previousStates);
            }

            #if __NEBULA_STATS__
            nAnimationServer::Instance()->profAnim.StopAccum();
            #endif
        }
        else
        {
            // animate ragdoll
            if (this->humanoidRagdoll)
            {
                this->refRagdoll->GetComponent<ncHumRagdoll>()->PhysicsAnimate();
            }
            else
            {
                this->refRagdoll->GetComponent<ncFourLeggedRagdoll>()->PhysicsAnimate();
            }
        }

        // update spatial box if necessary
        this->SetSpatialBBox();
    }

    // update attached entities
    this->UpdateAttachments();

    return true;
}

//------------------------------------------------------------------------------
/**
    first person together with third person data is only loaded when the entityclass is a necharacter.
*/
bool
ncCharacter::Load()
{
    //skeleton data
    ncCharacterClass *charClass = this->GetClassComponent<ncCharacterClass>();

    //it is a necharacter
    if (charClass)
    {
        this->LoadCharacters(this->GetEntityClass());
    }
    //it is a neskeleton
    else
    {
        this->LoadCharactersFromSkeletonClass();
    }

    this->Register();

    // create ragdoll instance
    this->LoadRagdoll();

    //initialize StatesArrays
    this->LoadStatesArrays();

    this->SetSpatialBBox();

    this->LoadAttachedData();

    /// return false it not all skeletons have the same animations
    /// @todo ma.garcias - display message for that and do not render anything (change level checkings)
    /// now -> changing to a non correct animation state will crash.
    return this->CorrectAnimations();
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::Unload()
{
    this->UnRegister();
    
    for (int i=0; i< this->character.Size(); i++)
    {
        n_delete(this->character[i]);
    }
    this->character.Clear();

    this->activeStates.Clear();
    this->currentStates.Clear();
    this->previousStates.Clear();
    this->fpersonActiveStates.Clear();
    this->fpersonCurrentStates.Clear();
    this->fpersonPreviousStates.Clear();
    this->dynamicAttachmentPool.Clear();
    this->fpersonAttachmentPool.Clear();
    this->persistedStates.Clear();
    this->fpersonPersistedStates.Clear();
    this->attachJointName.Clear();
    this->entityObjId.Clear();

    if (this->refRagdoll.isvalid())
    {
        nRagDollManager::Instance()->PushRagDoll(
            this->GetClassComponent<ncCharacterClass>()->GetRagDollManagerId(),
            this->refRagdoll->GetComponent<ncRagDoll>());

        ncRagDoll *ragComp = this->refRagdoll->GetComponent<ncRagDoll>();

        ragComp->GetEntityObject()->GetComponent<ncPhysicsObj>()->MoveToSpace(0);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::IsValid()
{
    return (!this->character.Empty() && (this->character[0] != 0));
}

//------------------------------------------------------------------------------
/**
*/
int
ncCharacter::GetPhysicsSkelIndex() const
{
    const ncCharacterClass* charClass = this->GetClassComponent<ncCharacterClass>();
    
    // if comes from a necharacterclass
    if (charClass)
    {
        return charClass->GetPhysicsSkeletonIndex();
    }

    //no fperson
    if (this->firstpersonCharIdx == -1)
    {
        return this->character.Size()-1; //the last one (minimum lod)
    }

    return this->character.Size()-2; //the minimum lod (ordered as lod0, lod1, lod2, fperson)
}

//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::CorrectAnimations()
{
    // if no lod
    if (this->character.Size() == 1)
    {
        return true;
    }

    // lod

    //get first skeleton
    ncCharacterClass *charClass = this->GetClassComponent<ncCharacterClass>();
    
    if (charClass)
    {
        ncSkeletonClass *skelComp = charClass->GetSkeletonClassPointer(0);

        //for other skeletons
        int numThirdPersonSkeletons = charClass->GetNumberLevelSkeletons();
        if (this->firstpersonCharIdx != -1)
        {
            numThirdPersonSkeletons--;//-1 coz 1person doesn't have to be tested
        }

        for (int skelIndex = 1; skelIndex < numThirdPersonSkeletons; skelIndex++) 
        {
            ncSkeletonClass *nextSkelComp = charClass->GetSkeletonClassPointer(skelIndex);
            
            //check if same states number
            if (skelComp->GetNumStates() != nextSkelComp->GetNumStates())
            {
                return false;
            }

            //for each animation
            for (int statesIndex = 0; statesIndex < skelComp->GetNumStates(); statesIndex++)
            {
                //check names
                if (0 != strcmp(skelComp->GetStateName(statesIndex),nextSkelComp->GetStateName(statesIndex)))
                {
                    //different state names
                    return false;
                }
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::Register()
{
    nAnimationServer::Instance()->Register(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::UnRegister()
{
    nAnimationServer::Instance()->Unregister(this->GetEntityObject());
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::DoEnterLimbo()
{
    this->UnRegister();
    //unregister local ragdoll entity object from spatial server
    this->GetComponentSafe<ncSpatial>()->RemoveFromSpaces();
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::DoExitLimbo()
{
    this->Register();
    //register local ragdoll entity object to spatial server
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::LoadCharacters(nEntityClass* eClass, int firstCharacterPlace)
{
    //skeleton data
    ncCharacterClass *charClass = eClass->GetComponent<ncCharacterClass>();
    ncSkeletonClass *skelComp = 0;

    // load all characters
    for (int i=0; i< charClass->GetNumberLevelSkeletons(); i++)
    {
        skelComp = charClass->GetSkeletonClassPointer(i);
        n_assert(skelComp);

        this->SetCharacter(i+firstCharacterPlace, n_new(nCharacter2(skelComp->GetCharacter())));
        n_assert(this->character[i]);

        //load animations
        //skelComp->LoadResources();

        if( i != 0 && i != charClass->GetFirstPersonLevel())
        {
            skelComp->LoadUpperLevelResources();
        }
        
        // get attachemtns array
        nArray<nDynamicAttachment> attachments = skelComp->GetDynAttachments();
        if (i== 0)
        {
            this->dynamicAttachmentPool = attachments;
        }
        else
        {
            if (i!= charClass->GetFirstPersonLevel())
            {
                for (int attachIdx=0; attachIdx<attachments.Size(); attachIdx++)
                {
                    this->dynamicAttachmentPool[attachIdx].SetJointIndex(i, attachments[attachIdx].GetJointIndex());
                }
            }
            else
            {
                attachments = skelComp->GetDynAttachments();
                this->fpersonAttachmentPool = attachments;
            }
        }       
    }

    this->firstpersonCharIdx = charClass->GetFirstPersonLevel();
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::LoadCharactersFromSkeletonClass() 
{
    ncSkeletonClass *skelComp = 0;
    skelComp = this->GetClassComponent<ncSkeletonClass>();
    n_assert(skelComp);

    this->SetCharacter(0, n_new(nCharacter2(skelComp->GetCharacter())));
    n_assert(this->character[0]);

    //load animations
    skelComp->LoadResources();

    // get attachemnts array
    nArray<nDynamicAttachment> attachments = skelComp->GetDynAttachments();
    this->dynamicAttachmentPool = attachments;

    for (int i=1; i<3; i++)
    {
        nString lowLevelSklName = this->GetEntityClass()->GetName();
        lowLevelSklName = lowLevelSklName.ExtractRange(0, lowLevelSklName.Length() - 1);
        lowLevelSklName += i;

        nEntityClass* entityClass = nEntityClassServer::Instance()->GetEntityClass(lowLevelSklName.Get());
        if (entityClass)
        {
            skelComp = entityClass->GetComponent<ncSkeletonClass>();
            n_assert(skelComp);

            this->SetCharacter(i, n_new(nCharacter2(skelComp->GetCharacter())));
            n_assert(this->character[i]);

            skelComp->LoadResources();
            skelComp->LoadUpperLevelResources();

            attachments = skelComp->GetDynAttachments();
            for (int attachIdx=0; attachIdx<attachments.Size(); attachIdx++)
            {
                this->dynamicAttachmentPool[attachIdx].SetJointIndex(i,attachments[attachIdx].GetJointIndex());
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ncCharacter::SetFirstPersonActive(bool fpersonActive)
{
    if (this->firstpersonCharIdx != -1)
    {
        this->firstpersonActive = fpersonActive;

        this->GetComponentSafe<ncSceneLod>()->SetLockedLevel(fpersonActive, this->firstpersonCharIdx);
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncCharacter::GetFirstPersonActive() const
{
    return this->firstpersonActive;
}

//------------------------------------------------------------------------------
/**
*/
nCharacter2*
ncCharacter::UpdateAndGetCharacter(int charIndex, nTime curTime)
{
    // update skeleton only if not update yet (ie. if not updated when nanimationserver::trigger)
    if (nAnimationServer::Instance()->DoUpdate())
    {    
        if (this->GetLastUpdatedCharacterIndex() != charIndex ||
            this->GetLastUpdatedTime() < curTime)
        {
            this->UpdateCharacter(charIndex, curTime);
        }
    }

    if (!this->GetRagdollActive())
    {
        return this->GetCharacter(charIndex);
    }

    return this->GetRagdollCharacter();
}

//------------------------------------------------------------------------------
/**
*/
int
ncCharacter::GetFPersonSkelIndex() const
{
    return this->firstpersonCharIdx;
}

//------------------------------------------------------------------------------
/**
*/
int
ncCharacter::GetJointGroup(int stateIndex, bool fperson) const
{
    int jointGroup = this->character[this->GetPhysicsSkelIndex()]->GetAnimStateArray()->GetStateAt(stateIndex).GetJointGroup();

    if (fperson)
    {
        jointGroup = this->character[this->firstpersonCharIdx]->GetAnimStateArray()->GetStateAt(stateIndex).GetJointGroup();
    }

    return jointGroup;
}
