#ifndef N_STATEINFO_H
#define N_STATEINFO_H

#include "kernel/nlogclass.h"
//------------------------------------------------------------------------------
/**
    @class nStateInfo
    @ingroup Character

    @brief 

    (C) 2005 Conjurer Services, S.A.
*/

//------------------------------------------------------------------------------
class nStateInfo
{
public:
    /// default constructor
    nStateInfo();
    /// default constructor
    nStateInfo(int index, bool backwards, bool rep, bool trans);
    /// default constructor
    nStateInfo(int index, bool backwards, bool rep, bool trans, float velFactor);
    /// constructor
    nStateInfo(int index, bool backwards, bool rep, bool trans, int jointIdx, const vector3& offset, float velFactor);
    /// destructor
    ~nStateInfo();
    /// set data
    void SetData(int sIndex, bool backWards, bool rep, bool doTrans, int jointIdx, const vector3& offset, float velFactor = 1.f, float time = 0);
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
    bool GetTransition() const;
    /// set lerp
    void SetLerp(float lerp);
    /// get lerp
    float GetLerp() const;
    /// set backwards
    void SetBackwards(bool backwards);
    /// get backwards
    bool GetBackwards() const;
    /// set repeat
    void SetRepeat(bool repeat);
    /// get repeat
    bool GetRepeat() const;
    /// set lerp
    void SetVelocityFactor(float velFactor);
    /// get lerp
    float GetVelocityFactor() const;
    
    /// set justStarted
    void SetJustStarted(int currentKey, float time);
    /// set justStarted
    void SetJustStarted(bool boolean, float time);
    /// get justStarted
    bool GetJustStarted() const;

    /// sets position offset and the joint Index to be applied to
    void SetJointPositionOffset(int jointIndex, const vector3 offset);
    /// has position offset
    bool HasPositionOffset() const;
    /// returns the offset joint index
    int GetOffsetIndex() const;
    /// returns the offset
    const vector3& GetOffset() const;

    /// set joint lookat index and vector
    void SetJointLookAtVector(int jointIndex, const vector3& lookAt);
    /// get joint lookat index
    int GetJointLookAtIndex();
    /// get joint lookat vector
    const vector3& GetJointLookAtVector();
    /// set joint lookat quaternion
    void SetJointLookAtQuat(const quaternion& quat);
    /// get joint lookat quaternion
    const quaternion& GetJointLookAtQuat();
    /// has joint lookat
    bool HasJointLookAt();

private:
    /// (read/write) precreated animation control (default)
    int stateIndex;
    float stateStarted;
    float stateOffset;//OBSOLETE
    
    bool transition;
    float lerp;
    bool backwards;
    bool repeat;

    /// (read) a cycle in a looped animation has just started
    bool justStarted;
    int lastKeyUsed;
    float lastUpdatedTime;

    /// (read/write) apply offset to joint (fit hands to weapons)
    int offsetJointIdx;
    vector3 positionOffset;

    /// (read/write) adjust velocity (control animation speed from animator)
    float velocityFactor;

    /// (read/write) apply look-at rotation to joint
    int jointLookAtIndex;
    vector3 jointLookAtVector;
    quaternion jointLookAtQuat;
};

//------------------------------------------------------------------------------
/**
*/
inline
nStateInfo::nStateInfo() :
    stateIndex(-1),
    stateStarted(0.0f),
    stateOffset(0.0f),
    transition(false),
    lerp(1.0f),
    backwards(false),
    repeat(false),
    justStarted(false),
    lastKeyUsed(-1),
    offsetJointIdx(-1),
    jointLookAtIndex(-1),
    lastUpdatedTime(-1.0f),
    velocityFactor(1.f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nStateInfo::nStateInfo(int index, bool backwards, bool rep, bool trans) :
    stateIndex(index),
    stateStarted(0.0f),
    stateOffset(0.0f),
    transition(trans),
    lerp(1.0f),
    backwards(backwards),
    repeat(rep),
    justStarted(false),
    lastKeyUsed(-1),
    offsetJointIdx(-1),
    jointLookAtIndex(-1),
    lastUpdatedTime(-1.0f),
    velocityFactor(1.f)
{
    //empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nStateInfo::nStateInfo(int index, bool backwards, bool rep, bool trans, float velFactor) :
    stateIndex(index),
    stateStarted(0.0f),
    stateOffset(0.0f),
    transition(trans),
    lerp(1.0f),
    backwards(backwards),
    repeat(rep),
    justStarted(false),
    lastKeyUsed(-1),
    offsetJointIdx(-1),
    jointLookAtIndex(-1),
    lastUpdatedTime(-1.0f),
    velocityFactor(velFactor)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nStateInfo::nStateInfo(int index, bool backwards, bool rep, bool trans, int jointIdx, const vector3& offset, float velFactor) :
    stateIndex(index),
    stateStarted(0.0f),
    stateOffset(0.0f),
    transition(trans),
    lerp(1.0f),
    backwards(backwards),
    repeat(rep),
    justStarted(false),
    lastKeyUsed(-1),
    offsetJointIdx(jointIdx),
    positionOffset(offset),
    jointLookAtIndex(-1),
    lastUpdatedTime(-1.0f),
    velocityFactor(velFactor)
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
inline
nStateInfo::~nStateInfo()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
nStateInfo::SetData(int sIndex, bool backWards, bool rep, bool doTrans, int jointIdx, const vector3& offset, float velFactor, float time)
{
    this->stateIndex = sIndex;
    this->stateStarted = time;
    this->backwards = backWards;
    this->repeat = rep;
    this->transition = doTrans;
    this->justStarted = false;
    this->lastKeyUsed = -1;
    this->offsetJointIdx = jointIdx;
    this->positionOffset = offset;
    this->lastUpdatedTime = -1.0f;
    this->velocityFactor = velFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetToDefault()
{
    this->stateIndex = -1;
    this->stateStarted = 0.0f;
    this->stateOffset = 0.0f;
    this->transition = false;
    this->backwards = false;
    this->repeat = false;
    this->lerp = 1.0f;
    this->justStarted = false;
    this->lastKeyUsed = -1;
    this->offsetJointIdx = -1;
    this->positionOffset = vector3();
    this->jointLookAtIndex = -1;
    this->jointLookAtQuat.ident();
    this->lastUpdatedTime = -1.0f;
    this->velocityFactor = 1.f;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetStateIndex(int i)
{
    this->stateIndex = i;
    this->lastKeyUsed = -1;
    this->lastUpdatedTime = -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nStateInfo::GetStateIndex() const
{
    return this->stateIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetStateStarted(float t)
{
    this->stateStarted = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nStateInfo::GetStateStarted() const
{
    return this->stateStarted;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetStateOffset(float t)
{
    this->stateOffset= t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nStateInfo::GetStateOffset() const
{
    return this->stateOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nStateInfo::IsValid() const
{
    return (this->stateIndex != -1);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetTransition(bool boolean)
{
    this->transition = boolean;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nStateInfo::GetTransition() const
{
    return this->transition;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetLerp(float value)
{
    this->lerp = value;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nStateInfo::GetLerp() const 
{
    return this->lerp;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetBackwards(bool boolean)
{
    this->backwards = boolean;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nStateInfo::GetBackwards() const
{
    return this->backwards;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetRepeat(bool data)
{
    this->repeat = data;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nStateInfo::GetRepeat() const
{
    return this->repeat;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetVelocityFactor(float t)
{
    this->velocityFactor = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nStateInfo::GetVelocityFactor() const
{
    return this->velocityFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetJustStarted(bool data, float time)
{
    this->justStarted = data;
    this->lastUpdatedTime = time;
    NLOG(animation, (NLOG1|2 , "SetJustStartedBool"));
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetJustStarted(int currentKey, float time)
{
    if ((this->lastKeyUsed != currentKey || currentKey == 0) && time != this->lastUpdatedTime)
    {
        this->justStarted = currentKey < this->lastKeyUsed;

        NLOGCOND(animation, (this->justStarted),  (NLOG1|2 , "SetJustStarted justStarted: TRUE\t currentKey: %i\t lastKey: %i time: %f", currentKey, this->lastKeyUsed, time));
        NLOGCOND(animation, (!this->justStarted),  (NLOG1|2 , "SetJustStarted justStarted: FALSE\t currentKey: %i\t lastKey: %i time: %f", currentKey, this->lastKeyUsed, time));

        this->lastKeyUsed = currentKey;
        this->lastUpdatedTime = time;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nStateInfo::GetJustStarted() const
{
    NLOGCOND(animation, (this->justStarted),  (NLOG1|2 , "GetJustStarted justStarted: TRUE, stateIndex: %d", this->stateIndex));
    NLOGCOND(animation, (!this->justStarted),  (NLOG1|2 , "GetJustStarted justStarted: FALSE, stateIndex: %d", this->stateIndex));

    return this->justStarted;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetJointPositionOffset(int jointIndex, const vector3 offset)
{
    this->offsetJointIdx = jointIndex;
    this->positionOffset = offset;
}

//------------------------------------------------------------------------------
/**
*/
inline
int 
nStateInfo::GetOffsetIndex() const
{
    return this->offsetJointIdx;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3& 
nStateInfo::GetOffset() const
{
    return this->positionOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nStateInfo::HasPositionOffset() const
{
    return this->offsetJointIdx != -1;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetJointLookAtVector(int jointIndex, const vector3& lookAt)
{
    this->jointLookAtIndex = jointIndex;
    this->jointLookAtVector = lookAt;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nStateInfo::GetJointLookAtIndex()
{
    return this->jointLookAtIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nStateInfo::GetJointLookAtVector()
{
    return this->jointLookAtVector;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStateInfo::SetJointLookAtQuat(const quaternion& quat)
{
    this->jointLookAtQuat = quat;
}

//------------------------------------------------------------------------------
/**
*/
inline
const quaternion&
nStateInfo::GetJointLookAtQuat()
{
    return this->jointLookAtQuat;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nStateInfo::HasJointLookAt()
{
    return this->jointLookAtIndex != -1;
}

//------------------------------------------------------------------------------
#endif
