#ifndef NC_CHARACTER_H
#define NC_CHARACTER_H
//------------------------------------------------------------------------------
/**
    @class ncCharacter
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Character Animation component object for entities.

    (C) 2005 Conjurer Services, S.A.
*/
//------------------------------------------------------------------------------
#include "entity/nentity.h"
#include "ncharacter/ncharacter2.h"

//------------------------------------------------------------------------------
class nStateInfo;
class ncSkeletonClass;

//------------------------------------------------------------------------------
class ncCharacter : public nComponentObject
{

    NCOMPONENT_DECLARE(ncCharacter,nComponentObject);

public:
    /// constructor
    ncCharacter();
    /// destructor
    virtual ~ncCharacter();
    /// initialize entity
    void InitInstance(nObject::InitInstanceMsg initType);

    /// load
    bool Load();
    /// unload
    void Unload();
    /// is valid
    bool IsValid();

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// get physics skeleton index (takes it from class data)
    int GetPhysicsSkelIndex() const;
    /// get current character index (lod)
    int GetLastUpdatedCharacterIndex() const;
    /// get current timestamp
    nTime GetLastUpdatedTime() const;
    /// is valid state index
    bool IsValidStateIndex(int i) const;
    /// is valid state index
    bool IsValidFirstPersonStateIndex(int i) const;

    /// say if character has first person animation
    bool HasFirstPersonAnimation()const;
    /// get first person skeleton index
    int GetFPersonSkelIndex() const;

    /// get character object
    nCharacter2 *GetCharacter(int index);
    /// get ragdoll character
    nCharacter2 *GetRagdollCharacter();
    /// set character object
    void SetCharacter(nCharacter2*);
    /// set character object
    void SetCharacter(int index, nCharacter2* character);

    /// change spatial bounding box
    void SetSpatialBBox();
#if 0
    /// get animation state //OBSOLETE
    int GetAnimationState();
#endif
    /// update character
    virtual bool UpdateCharacter(int charIndex, nTime curTime);
    /// update character if must and returns it
    nCharacter2* UpdateAndGetCharacter(int charIndex, nTime curTime);

    /// enable update of attached entities
    void SetUpdateAttachments(bool update);
    /// update of attached entities is enabled
    bool GetUpdateAttachments() const;
    /// update attachments
    void UpdateAttachments();
    /// attach
    void Attach(nString attachName, nEntityObject* entityObject);
    /// dettach
    void Dettach(nEntityObject* entityObject);
    /// dettach all
    void DettachAll();
    /// get attachment world matrix
    void GetAttachmentTransforms(nEntityObject* eObj, vector3& pos, quaternion& quat, bool fperson);
    /// get attachment world rot, pos
    void GetAttachmentTransforms(int attIndex, vector3& pos, quaternion& quat, bool fperson);

    /// get active state indices
    nArray<int> GetActiveStateIndices();
    /// get actives state names
    nArray<const char*> GetActiveStates();

    /// set active state by name
    void SetActiveState(const char* stateName, bool backwards = false, bool repeat = false, bool transition = false, float speedFactor = 1.0f);
    /// set active state by index
    void SetActiveStateByIndex(int index, bool backwards = false, bool repeat = false, bool transition = false, float speedFactor = 1.0f);

    /// get first person active state index
    int GetFirstPersonActiveStateIndex();
    /// get first person active state name
    const char* GetFirstPersonActiveState();

    /// set first person active state by name
    void SetFirstPersonActiveState(const char* stateName, bool backwards = false, bool repeat = false, bool transition = false, float speedFactor = 1.0f);
    /// set first person active state by name
    void SetFirstPersonActiveState(const char* stateName, bool backwards, bool repeat, bool transition, int jointIdx, const vector3& offset, float speedFactor = 1.0f);
    /// set first person active state by index
    void SetFirstPersonActiveStateByIndex(int index, bool backwards = false, bool repeat = false, bool transition = false, float speedFactor = 1.0f);
    /// set first person active state by index
    void SetFirstPersonActiveStateByIndex(int index, bool backwards, bool repeat, bool transition, int jointIdx, const vector3& offset, float speedFactor = 1.0f);

    /// set speed factor while an animation is playing
    void SetSpeedFactor(int stateIndex, float speedFactor, bool fperson = false);
    /// get speed factor
    float GetSpeedFactor(int stateIndex, bool fperson = false) const;

    /// change morph flag
    void SetDirtyMorphActiveState(bool dirty);
    /// get morph flag
    bool GetDirtyMorphActiveState() const;

    /// register to animation server
    void Register();
    /// unregister from animation server
    void UnRegister();

    /// enter to limbo
    void DoEnterLimbo();
    /// exit to limbo
    void DoExitLimbo();
    
    /// get when state started (state for groupIndex)
    nTime GetCurrentStateStarted(int groupIndex) const;
    /// loop has just started
    bool LoopJustStarted(int stateIndex, bool fperson) const;

    /// get animation displacement vector
    const vector3 GetDisplacement(int stateIndex, float fromTime, float toTime) const;
    /// has displacement
    bool HasDisplacement(int stateIndex) const;

    /// get unique key
    int GetKey() const;

    /// set active state by index
    void SetActiveStateIndex(int, bool, bool, bool);
    /// set active state by index
    void SetActiveStateName(const char*, bool, bool, bool);

    /// pause animation. while pause, time is not stopped
    void Pause(bool);

    /// get number of states 
    int GetNumberStates() const;
    /// looped or not by index
    bool IsLoopedByIndex(int);
    /// looped or not by name
    bool IsLoopedByName(const char *);
    /// get state name
    const nString GetStateName(int) const;
    /// get state index
    int GetStateIndex(nString);
    /// is playing
    bool IsPlaying(int);

    /// sets active state by index only persisted
    void SetActiveAnimState(int, bool, bool, bool);
    /// sets active state by index only persisted 
    void SetActiveAnimStateWithSpeedFactor(int, bool, bool, bool, float);

    /// get animation duration (returns nTime)
    float GetStateDuration(int, bool) const;
    /// get elapsed time (returns nTime)
    float GetElapsedTime(int, bool) const;
    /// get remaining time (returns nTime)
    float GetRemainingTime(int,bool) const;

    /// get perceptionoffset
    vector3 GetPerceptionOffset(int) const;
    /// set perception offset
    void SetPerceptionOffset(int, vector3);

    /// get number of attachmetn points
    int GetNumberAttachments() const;
    /// get i-th attachment point name
    const char* GetAttachmentName(int) const;
    /// get attached entity id
    nEntityObjectId GetAttachedEntity(int) const;

    /// attach entity object by id
    void Attach(const char *, nEntityObjectId);
    /// dettach entity object by id
    void Dettach(nEntityObjectId);

#ifndef NGAME
    /// remove animation state from skeleton class
    bool RemoveAnimState(bool, int);
    /// set anim state name
    bool ChangeAnimStateName(bool, int, const nString&);
#endif
    /// set fade in time
    bool SetFadeInTime(bool, int, float);
    /// get fade in time
    float GetFadeInTime(bool, int) const;

    /// set active morph state by index
    void SetActiveMorphStateByIndex(int);
    /// get active morph state by index
    int GetActiveMorphStateByIndex() const;

    /// set ragdoll entity id
    void SetRagdollEntityId(nEntityObjectId);
    /// get ragdoll entity id
    nEntityObjectId GetRagdollEntityId() const;
    /// activate physics
    void SetRagdollActive(bool);
    /// get ragdoll active
    bool GetRagdollActive() const;
    /// set first person active
    void SetFirstPersonActive(bool);
    /// set first person active
    bool GetFirstPersonActive() const;

    /// set first person active state by index
    void SetFirstPersonActiveStateIndex(int, bool, bool, bool);
    /// set first person active state by index
    void SetFirstPersonActiveStateName(const char*, bool, bool, bool);
    /// sets first person active state by index only persisted
    void SetFirstPersonActiveAnimState(int, bool, bool, bool);
    /// sets first person active state by index only persisted
    void SetFirstPersonActiveAnimStateWithSpeedFactor(int, bool, bool, bool, float);
    /// get first person number of states 
    int GetFirstPersonNumberStates() const;
    /// get first person state name
    const nString GetFirstPersonStateName(int) const;
    /// get first person state index
    int GetFirstPersonStateIndex(nString);

    /// sets first person offset
    void SetFirstPersonOffset(int, const vector3&);
    /// sets first person offset
    const vector3& GetFirstPersonOffset() const;
    /// sets first person offset
    int GetFirstPersonOffsetIndex() const;

    /// set joint look at vector
    void SetJointLookAtVector(int, const vector3&);
    /// get joint look at vector
    void GetJointLookAtVector(int&, vector3&);

#ifndef NGAME
#endif

protected:
    /// find animation idx from first person one
    int FindLodAnimIdxFromFirstPersonIdx(int fpersonanimIdx, int charIdx = 0);

private:
    /// load ragdoll
    void LoadRagdoll();
    /// load states array
    void LoadStatesArrays();
    /// load attachments
    void LoadAttachedData();

    /// all lod skeleton have the same animations
    bool CorrectAnimations();

    /// 
    int GetJointGroup(int stateIndex, bool fperson) const;

    /// update active states for that moment of time
    void UpdateActiveStates(nTime curTime);
    /// set active state
    void SetActiveStateByGroup(int jointGroup, int stateIndex, nTime time, bool transition, bool repeat, bool backwards, int jointIdx, const vector3& offset, float velFactor, bool fperson);

    /// load characters for a given entity class
    void LoadCharacters(nEntityClass* eClass, int firstCharacterPlace = 0);
    /// load characters when that instance comes from a neskeletonclass
    void LoadCharactersFromSkeletonClass();

    /// get ragdoll from ragdollmanager
    void InitializeRagdoll(nEntityClass* eClass);    

#ifndef NGAME
    /// remove state given skeletonclass
    bool RemoveState(int stateIndex, ncSkeletonClass* skelComp);
#endif

    /// update current states from assigned states
    void UpdateActiveStatesArrays(nArray<nStateInfo>& active, nArray<nStateInfo>& current, nTime curTime, bool fperson);
    /// update look at vector in current states
    void UpdateActiveStatesLookAt(nArray<nStateInfo>& active);

    /// character
    nArray<nCharacter2*> character;
    /// index of last updated level (or ragdoll), required to get-set states and attachments
    int lastUpdatedCharacter; 
    /// timestamp for last update
    nTime lastUpdatedTime;

    /// active state (initially is -1)
    nArray<nStateInfo> activeStates;
    nArray<nStateInfo> currentStates;
    nArray<nStateInfo> previousStates;

    nArray<nStateInfo> fpersonActiveStates; //only needed one nstateinfo, coz fperson only has one group, but easier code if used like an array
    nArray<nStateInfo> fpersonCurrentStates;
    nArray<nStateInfo> fpersonPreviousStates;

    nArray<nDynamicAttachment> dynamicAttachmentPool;
    nArray<nDynamicAttachment> fpersonAttachmentPool;

    /// persisted data for states
    nArray<nStateInfo> persistedStates;
    nArray<nStateInfo> fpersonPersistedStates;

    /// persisted data for attachments, joint name and entityobjectid
    nArray<nString> attachJointName;
    nArray<nEntityObjectId> entityObjId;

    /// bool pause
    bool pause;

    /// first person data
    int firstpersonCharIdx;
    bool firstpersonActive;

    /// register character
    int key;
    static int      nextKey;

    /// morph data
    bool dirtyMorphState;
    int activeMorphState;

    /// ragdoll entity id
    nRef<nEntityObject> refRagdoll;
    /// is ragdoll active?
    bool ragdollActive;

    vector3 fpersonOffset;
    int fpersonOffsetJointIdx;

    int jointLookAtIndex;
    vector3 jointLookAtVector;///< look-at direction

    bool humanoidRagdoll;

    bool updateAttachments;
};

//------------------------------------------------------------------------------
/**
*/
inline
nCharacter2 *
ncCharacter::GetCharacter(int index)
{
    if (this->character.Size() > index)
    {
        return this->character[index];
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncCharacter::SetCharacter(nCharacter2* curCharacter)
{
    this->character.Set(0, curCharacter);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncCharacter::SetCharacter(int index,  nCharacter2* curCharacter)
{
    this->character.Set(index, curCharacter);
}
#if 0
//------------------------------------------------------------------------------
/**
*/
inline
int
ncCharacter::GetAnimationState()
{
    //return this->character[lastUpdatedCharacter]->GetActiveState();
}
#endif
//------------------------------------------------------------------------------
/**
*/
inline
int
ncCharacter::GetKey() const
{
    return this->key;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncCharacter::Pause(bool activate)
{
    this->pause = activate;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString
ncCharacter::GetStateName(int stateIndex) const
{
    return this->character[0]->FindStateNameByIndex(stateIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncCharacter::GetStateIndex(nString stateName)
{
    return this->character[0]->FindStateIndexByName(stateName);
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString
ncCharacter::GetFirstPersonStateName(int stateIndex) const
{
    return this->character[this->firstpersonCharIdx]->FindStateNameByIndex(stateIndex);    
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncCharacter::GetFirstPersonStateIndex(nString stateName)
{
    return this->character[this->firstpersonCharIdx]->FindStateIndexByName(stateName);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncCharacter::SetActiveMorphStateByIndex(int morphStateIndex)
{
    this->activeMorphState = morphStateIndex;
    this->dirtyMorphState = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncCharacter::GetActiveMorphStateByIndex() const
{
    return this->activeMorphState;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncCharacter::SetDirtyMorphActiveState(bool dirty)
{
    this->dirtyMorphState = dirty;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncCharacter::GetDirtyMorphActiveState() const
{
    return this->dirtyMorphState;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
ncCharacter::GetLastUpdatedCharacterIndex() const
{
    return this->lastUpdatedCharacter;
}

//------------------------------------------------------------------------------
/**
*/
inline
nTime
ncCharacter::GetLastUpdatedTime() const
{
    return this->lastUpdatedTime;
}

//------------------------------------------------------------------------------
/**
    @returns true if the character has first person animation
*/
inline
bool
ncCharacter::HasFirstPersonAnimation() const
{
    return (this->firstpersonCharIdx != -1);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ncCharacter::SetUpdateAttachments(bool update)
{
    this->updateAttachments = update;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
ncCharacter::GetUpdateAttachments() const
{
    return this->updateAttachments;
}

//------------------------------------------------------------------------------
#endif
