#ifndef N_ANIMSTATE_H
#define N_ANIMSTATE_H
//------------------------------------------------------------------------------
/**
    @class nAnimState
    @ingroup Anim2
    
    @brief An animation state contains any number of nAnimClip objects of
    identical size (number of anim curves contained in the clip) and can
    sample a weight-blended result from them. Each anim clip is associated
    with a weight value between 0 and 1 which defines how much that anim
    clip influences the resulting animation.
    
    An nAnimation object connected to the nAnimState object delivers
    the actual animation data.
    
    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "anim2/nanimclip.h"
#include "variable/nvariablecontext.h"
#include "mathlib/quaternion.h"
#include "mathlib/bbox.h"
#include "anim2/nanimeventhandler.h"

//------------------------------------------------------------------------------
class nAnimState
{
public:
    /// constructor
    nAnimState();
    /// set state name
    void SetName(const nString& n);
    /// get state name
    const nString& GetName() const;
    
    /// set motion file
    void SetMotionFile(const char *name);
    /// get motion file
    const char *GetMotionFile() const;
    /// set motion resource
    void SetMotion(nAnimation *displ);
    /// get motion resource
    nAnimation *GetMotion();
    /// is motion ref valid?
    bool IsMotionValid() const;
    /// invalidate motion entry
    void InvalidateMotion();

    /// set animation file
    void SetAnimFile(const char *name);
    /// get animation file
    const char *GetAnimFile() const;
    /// set animation resource
    void SetAnim(nAnimation *anim);
    /// get animation resource
    nAnimation *GetAnim();
    /// is anim ref valid?
    bool IsAnimValid() const;
    /// invalidate entry
    void InvalidateAnim();
    
    /// set index of animation group in animation resource file
    void SetAnimGroupIndex(int index);
    /// get animation group index
    int GetAnimGroupIndex() const;
    /// set the fadein time
    void SetFadeInTime(float t);
    /// get the fadein time
    float GetFadeInTime() const;
    /// begin defining animation clips
    void BeginClips(int num);
    /// set an animation clip
    void SetClip(int index, const nAnimClip& clip);
    /// finish defining animation clips
    void EndClips();
    /// get number of animation clips
    int GetNumClips() const;
    /// get anim clip at index
    nAnimClip& GetClipAt(int index) const;
    /// sample weighted values at a given time from nAnimation object
    bool Sample(float time,nAnimation* animSource,nVariableContext* varContext,vector4* keyArray,vector4* scratchKeyArray, nAnimation::Group::LoopType loopType, float velocityFactor, int keyArraySize);
    /// emit animation events for a given time range
    void EmitAnimEvents(float fromTime, float toTime, nAnimation* animSource, nAnimEventHandler* handler);

    /// set bounding box
    void SetBoundingBox(const vector3& center, const vector3& extents);
    /// get bounding box
    void GetBoundingBox(bbox3 &box);

    /// set joint group
    void SetJointGroup(int jointGroup);
    /// get joint group
    int GetJointGroup();

    /// is looped
    bool IsLooped();

    /// set perception offset
    void SetPerceptionOffset(vector3 perceptionOffset);
    /// Get perception offset
    const vector3& GetPerceptionOffset() const;

    /// SetSubmissiveLeaderState
    void SetLeaderState(int leaderAnimStateIdx);
    /// Get leader state
    int GetLeaderState() const;

    /// Set follow joint
    void SetFollowJoint(int jointIdx);
    /// Get leader state
    int GetFollowJoint() const;

    /// get animation displacement
    const vector3 GetMotionAt(int key[2], float inbetween) const;
    /// get total displacement
    const vector3 GetTotalMotionMotion() const;

private:
    /// begin defining blended animation events
    void BeginEmitEvents();
    /// add a blended animation event
    void AddEmitEvent(const nAnimEventTrack& track, const nAnimEvent& event, float weight);
    /// finish defining blended anim events, emit the events
    void EndEmitEvents(nAnimEventHandler* handler);
        
    nArray<nAnimClip> clipArray;
    nString name;

    nString animFile;
    nRef<nAnimation> refAnim;

    nString motionFile;
    nRef<nAnimation> refMotion;

    int animGroupIndex;
    float fadeInTime;
    bbox3 boundingBox;
    int jointGroupIndex;
    vector3 perceptionOffset;
    int leaderState;

    int followedJointIdx;

    nArray<nAnimEventTrack> outAnimEventTracks;
};

//------------------------------------------------------------------------------
/**
*/
inline
nAnimState::nAnimState() :
    clipArray(0, 0),
    animGroupIndex(0),
    fadeInTime(0.0f),
    jointGroupIndex(-1),
    leaderState(-1),
    followedJointIdx(-1)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nAnimState::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetAnimFile(const char *filename)
{
    n_assert(filename);
    this->animFile = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nAnimState::GetAnimFile() const
{
    return this->animFile.IsEmpty() ? 0 : this->animFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetAnim(nAnimation *anim)
{
    this->refAnim = anim;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation *
nAnimState::GetAnim()
{
    return this->refAnim.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAnimState::IsAnimValid() const
{
    return this->refAnim.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::InvalidateAnim()
{
    this->refAnim.invalidate();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetAnimGroupIndex(int index)
{
    this->animGroupIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimState::GetAnimGroupIndex() const
{
    return this->animGroupIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetFadeInTime(float t)
{
    this->fadeInTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nAnimState::GetFadeInTime() const
{
    return this->fadeInTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::BeginClips(int num)
{
    this->clipArray.SetFixedSize(num);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetClip(int index, const nAnimClip& clip)
{
    // ensure identical number of curves in clips
    if (index > 0)
    {
        n_assert(this->clipArray[0].GetNumCurves() == clip.GetNumCurves());
    }
    this->clipArray[index] = clip;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::EndClips()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimState::GetNumClips() const
{
    return this->clipArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimClip&
nAnimState::GetClipAt(int index) const
{
    return this->clipArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetBoundingBox(const vector3& center, const vector3& extents)
{
    this->boundingBox.set(center, extents);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::GetBoundingBox(bbox3 &box)
{
    box.vmin = this->boundingBox.vmin;
    box.vmax = this->boundingBox.vmax;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetJointGroup(int jointGroup)
{
    this->jointGroupIndex = jointGroup;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nAnimState::GetJointGroup()
{
    return this->jointGroupIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAnimState::IsLooped()
{
    /// @todo ma.garcias - with group index!!
    return (this->GetAnim()->GetGroupAt(0).GetLoopType() == nAnimation::Group::Repeat);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetPerceptionOffset(vector3 perceptionOffset)
{
    this->perceptionOffset = perceptionOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nAnimState::GetPerceptionOffset() const
{
    return this->perceptionOffset;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetMotionFile(const char *filename)
{
    n_assert(filename);
    this->motionFile = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nAnimState::GetMotionFile() const
{
    return this->motionFile.IsEmpty() ? 0 : this->motionFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::SetMotion(nAnimation *anim)
{
    this->refMotion = anim;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation *
nAnimState::GetMotion()
{
    if (this->IsMotionValid())
    {
        return this->refMotion.get();
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nAnimState::IsMotionValid() const
{
    return this->refMotion.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nAnimState::InvalidateMotion()
{
    this->refMotion.invalidate();
}
//------------------------------------------------------------------------------
#endif
