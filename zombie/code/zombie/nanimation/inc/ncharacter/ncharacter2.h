#ifndef N_CHARACTER2_H
#define N_CHARACTER2_H
//------------------------------------------------------------------------------
/**
    @class nCharacter2
    @ingroup Character

    @brief Holds all the data necessary to animate an character in one place.

    (C) 2003 RadonLabs GmbH
*/
#include "nanimation/nanimstatearray.h"
#include "ncharacter/ncharskeleton.h"
#include "ncharacter/nstateinfo.h"
#include "kernel/nrefcounted.h"
#include "nanimation/nanimeventhandler.h"

//------------------------------------------------------------------------------
class nCharacter2 : public nRefCounted
{
public:
#if 0 //keep for open source merges
    class StateInfo
    {
    public:
        /// default constructor
        StateInfo();
        /// destructor
        ~StateInfo();
        /// set data
        void SetData(int sIndex, float time, bool backWards, bool rep, bool doTrans);
        /// set to default data
        void SetToDefault();
        /// set state index
        void SetStateIndex(int i);
        /// get state index
        int GetStateIndex() const;
        /// set state started time
        void SetStateStarted(float t);
        /// get state started time
        float GetStateStarted() const;
        /// set state time offset
        void SetStateOffset(float t);
        /// get state time offset
        float GetStateOffset() const;

        /// is valid?
        bool IsValid() const;
        /// set transition
        void SetTransition(bool boolean);
        /// get transition
        bool GetTransition();
        /// set lerp
        void SetLerp(float lerp);
        /// get lerp
        float GetLerp();
        /// set backwards
        void SetBackwards(bool backwards);
        /// get backwards
        bool GetBackwards();
        /// set repeat
        void SetRepeat(bool repeat);
        /// get repeat
        bool GetRepeat();

    private:
        int stateIndex;
        float stateStarted;
        float stateOffset;
        
        bool transition;
        float lerp;
        bool backwards;
        bool repeat;
    };
#endif

    /// constructor
    nCharacter2();
    /// copy constructor
    nCharacter2(const nCharacter2& src);
    /// destructor
    virtual ~nCharacter2();
    /// get the embedded character skeleton
    nCharSkeleton& GetSkeleton();
    /// set pointer to an anim state array (not owned)
    void SetAnimStateArray(nAnimStateArray* animStates);
    /// get pointer to anim state array
    const nAnimStateArray* GetAnimStateArray() const;

    ///remove state
    void RemoveSkeletonAnimState(int stateIndex);

#if 0
    /// set pointer to an animation source which delivers the source data (not owned)
    void SetAnimation(nAnimation* anim);
    /// get pointer to animation source
    const nAnimation* GetAnimation() const;
    /// set the currently active state
    void SetActiveState(int stateIndex, nTime time, nTime offset);
    /// get the current state index
    int GetActiveState() const;
    /// evaluate the joint skeleton
    void EvaluateSkeleton(float time, nVariableContext* varContext);
    /// get when state started
    float GetCurrentStateStarted() const;
    /// return if animation is already playing or has finished
    bool IsPlaying(nTime curTime) const;
#endif
    /// is `i' a valid state index?
    bool IsValidStateIndex(int i) const;

    /// set optional anim event handler (incrs refcount of handler)
    void SetAnimEventHandler(nAnimEventHandler* handler);
    /// get optional anim event handler
    nAnimEventHandler* GetAnimEventHandler() const;
    /// emit animation events between 2 times
    void EmitAnimEvents(float startTime, float stopTime);
    /// find a state index by state name
    int FindStateIndexByName(const nString& n);
    /// find state name by index
    const nString FindStateNameByIndex(int stateIndex);

    /// return animation duration of an animation state
    nTime GetStateDuration(int stateIndex) const;
    /// return animation duration of an animation state in ticks
    int GetStateTicks(int stateIndex) const;
    /// return fadein time of an animation state
    nTime GetStateFadeInTime(int stateIndex) const;
    /// enable/disable animation
    void SetAnimEnabled(bool b);
    /// get manual joint animation
    bool IsAnimEnabled() const;
    /// set the frame id when the character was last evaluated
    void SetLastEvaluationFrameId(uint id);
    /// get the frame id when the character was last evaluated
    uint GetLastEvaluationFrameId() const;

    /// SetSubmissiveLeaderState
    void SetSubmissiveLeaderState(int submissiveStateIdx, int leaderStateIdx);
    /// Get leader state
    int GetLeaderState(int submissiveStateIdx) const;
    /// Get leader state
    int GetLeaderState(const nString& submissiveStateName) const;

    /// set state follows joint
    void SetStateFollowsJoint(int stateIdx, int jointIdx);
    /// get joint 
    int GetStateFollowsJoint(int stateIdx) const;

    /// get animation displacement vector
    const vector3 GetStateDisplacement(const nStateInfo& stateInfo, float fromTime, float toTime);
    /// get animation displacement vector
    const vector3 GetTotalStateDisplacemmt(int stateIndex);
    /// get animation displacement vector
    bool HasDisplacement(int stateIndex) const;
    /// get keys for, and returns relative animation time
    float GetKeysFor(const nStateInfo& stateInfo, float atTime, int* keyIndex, float& inbetween , const nAnimation::Group group, nAnimation::Group::LoopType& loopType);

    /// begin tweakable joints
    void BeginTweakableJoints( int num );
    /// set tweakable joint
    void SetTweakableJoint(int index, int jointIndex, int parentIndex);
    /// end tweadkable joints
    void EndTweakableJoints();
    /// get number of tweakable joints
    int GetNumTweakableJoints() const;
    /// get number of joints affected in tweakablejoint at index
    int GetNumJointInTweakableJoint(int index) const;
    /// get jointIndex of the tweakable joint
    int GetTweakableJointAt(int index, int twekIdx) const;
    /// get paretnIndex of the tweakable joint
    int GetParentTweakableJointAt(int index, int twekIdx) const;

    /// compute auxiliar state data
    void ComputeAuxiliaryStateData(nStateInfo& stateInfo, float time, float& relTime, nAnimation::Group::LoopType& looptype);
    /// evaluate current state placed at curStateIndex in curStatesArray
    bool EvaluateState(int curGroupIndex, float time, nVariableContext* varContext, nStateInfo& curStatesInfo, nArray<nStateInfo>& prevStatesInfo);
    /// evaluate full skeleton data
    void EvaluateFullSkeleton(float time, nVariableContext* varContext, nArray<nStateInfo>& curStatesInfo, nArray<nStateInfo>& prevStatesInfo);
    
private:

    enum {
        MaxTweakableJoints = 4,
    };

    /// ntweakable joint
    struct nTweakableJoint
    {
        int jointIndex[MaxTweakableJoints];
        int parentIndex[MaxTweakableJoints];
        int numJoints;

        /// initialize tweakablejoint
        void InitializeData();
        /// add joint
        void AddTweableJoint( int jointIdx, int parentIdx);
        /// get num joints
        int GetNumTweableJoints();
    };

    /// get displacement for a given time
    vector3 GetDisplacementFor(const nStateInfo& stateInfo, float atTime );

    enum
    {
        MaxJoints = 1024,
        MaxCurves = MaxJoints * 3,      // translate, rotate, scale per curve
    };

    nCharSkeleton charSkeleton;
    nAnimStateArray* animStateArray;
    nAnimEventHandler* animEventHandler;

#if 0
    nRef<nAnimation> animation;
    StateInfo prevStateInfo;
    StateInfo curStateInfo;
#endif

    static vector4 scratchKeyArray[MaxCurves];
    static vector4 keyArray[MaxCurves];
    static vector4 transitionKeyArray[MaxCurves];
    
    bool transitionToDo;
    bool animEnabled;
    uint lastEvaluationFrameId;

    nArray<nTweakableJoint> tweakableJointsArray;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::nTweakableJoint::InitializeData()
{
    this->numJoints = 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::nTweakableJoint::AddTweableJoint( int jointIdx, int parentIdx)
{
    n_assert(numJoints < MaxTweakableJoints);
    this->jointIndex[numJoints] = jointIdx;
    this->parentIndex[numJoints] = parentIdx;
    this->numJoints++;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nCharacter2::nTweakableJoint::GetNumTweableJoints()
{
    return this->numJoints;
}

#if 0
//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
nCharacter2::StateInfo::StateInfo() :
    stateIndex(-1),
    stateStarted(0.0f),
    stateOffset(0.0f),
    transition(false),
    lerp(1.0f),
    backwards(false),
    repeat(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
nCharacter2::StateInfo::~StateInfo()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void 
nCharacter2::StateInfo::SetData(int sIndex, float time, bool backWards, bool rep, bool doTrans)
{
    this->stateIndex = sIndex;
    this->stateStarted = time;
    this->backwards = backWards;
    this->repeat = rep;
    this->transition = doTrans;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void 
nCharacter2::StateInfo::SetToDefault()
{
    this->stateIndex = -1;
    this->stateStarted = 0.0f;
    this->backwards = false;
    this->repeat = false;
    this->transition = false;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void
nCharacter2::StateInfo::SetStateIndex(int i)
{
    this->stateIndex = i;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
int
nCharacter2::StateInfo::GetStateIndex() const
{
    return this->stateIndex;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void
nCharacter2::StateInfo::SetStateStarted(float t)
{
    this->stateStarted = t;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
float
nCharacter2::StateInfo::GetStateStarted() const
{
    return this->stateStarted;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void
nCharacter2::StateInfo::SetStateOffset(float t)
{
    this->stateOffset= t;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
float
nCharacter2::StateInfo::GetStateOffset() const
{
    return this->stateOffset;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
bool
nCharacter2::StateInfo::IsValid() const
{
    return (this->stateIndex != -1);
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void
nCharacter2::StateInfo::SetTransition(bool boolean)
{
    this->transition = boolean;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
bool
nCharacter2::StateInfo::GetTransition()
{
    return this->transition;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void
nCharacter2::StateInfo::SetLerp(float value)
{
    this->lerp = value;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
float
nCharacter2::StateInfo::GetLerp()
{
    return this->lerp;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void
nCharacter2::StateInfo::SetBackwards(bool boolean)
{
    this->backwards = boolean;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
bool
nCharacter2::StateInfo::GetBackwards()
{
    return this->backwards;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void
nCharacter2::StateInfo::SetRepeat(bool data)
{
    this->repeat = data;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
bool
nCharacter2::StateInfo::GetRepeat()
{
    return this->repeat;
}
#endif

//------------------------------------------------------------------------------
/**
*/
inline
nCharSkeleton&
nCharacter2::GetSkeleton()
{
    return this->charSkeleton;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::SetAnimStateArray(nAnimStateArray* animStates)
{
    this->animStateArray = animStates;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nAnimStateArray*
nCharacter2::GetAnimStateArray() const
{
    return this->animStateArray;
}

#if 0
//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
void
nCharacter2::SetAnimation(nAnimation* anim)
{
    n_assert(anim);
    this->animation = anim;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
const nAnimation*
nCharacter2::GetAnimation() const
{
    return this->animation;
}

//------------------------------------------------------------------------------
/**
    OBSOLETE
*/
inline
int
nCharacter2::GetActiveState() const
{
    return this->curStateInfo.GetStateIndex();
}
#endif

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacter2::IsValidStateIndex(int i) const
{
    return ((0 <= i) && (i < animStateArray->GetNumStates()));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::SetAnimEnabled(bool b)
{
    this->animEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nCharacter2::IsAnimEnabled() const
{
    return this->animEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::SetLastEvaluationFrameId(uint id)
{
    this->lastEvaluationFrameId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline
uint
nCharacter2::GetLastEvaluationFrameId() const
{
    return this->lastEvaluationFrameId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::SetAnimEventHandler(nAnimEventHandler* handler)
{
    if (this->animEventHandler)
    {
        this->animEventHandler->Release();
        this->animEventHandler = 0;
    }
    if (handler)
    {
        this->animEventHandler = handler;
        this->animEventHandler->AddRef();
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimEventHandler*
nCharacter2::GetAnimEventHandler() const
{
    return this->animEventHandler;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString
nCharacter2::FindStateNameByIndex(int stateIndex)
{
    if (this->animStateArray->GetNumStates() > stateIndex)
    {
        return this->animStateArray->GetStateAt(stateIndex).GetName();
    }
    return nString();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nCharacter2::RemoveSkeletonAnimState(int stateIndex)
{
    this->animStateArray->RemoveState(stateIndex);
}

//------------------------------------------------------------------------------
#endif
