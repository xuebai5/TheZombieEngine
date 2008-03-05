#ifndef N_BLENDSTATE_H
#define N_BLENDSTATE_H
//------------------------------------------------------------------------------
/**
    @class nBlendState
    @ingroup NebulaAnimationSystem
    
    @brief 
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "kernel/ntypes.h"
#include "anim2/nanimation.h"
#include "nscene/nBlendTargetKeyArray.h"

//------------------------------------------------------------------------------
class nBlendState
{
public:
    /// constructor
    nBlendState();
    /// destructor
    ~nBlendState();

    /// set state name
    void SetName(const nString& n);
    /// get state name
    const nString& GetName() const;
    
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
    void Invalidate();
    
    /// set index of animation group in animation resource file
    void SetAnimGroupIndex(int index);
    /// get animation group index
    int GetAnimGroupIndex() const;
    /// set the fadein time
    void SetFadeInTime(float t);
    /// get the fadein time
    float GetFadeInTime() const;
    /// set num curves
    void SetNumCurves(int numCurves);
    /// get num Curves
    int GetNumCurves() const;
    /// set the start time
    void SetStartTime(float t);
    /// get the start time
    float GetStartTime() const;

    /// set target change
    void SetTargetChange(int keyIndex, int curveIndex, int targetIndex);
    /// get target key array
    nBlendTargetKeyArray& GetTargetKeyArray();
    /// update targets
    void UpdateTargets( float curTime, nArray<int>& curveIndices, nArray<int>& targetIndices );
    
private:
    nString name;

    nString animFile;
    nRef<nAnimation> refAnim;
    int numCurves;
    
    nBlendTargetKeyArray targetKeyArray;

    int animGroupIndex;
    float fadeInTime;

    float startTime;
};

//------------------------------------------------------------------------------
/**
*/
inline
nBlendState::nBlendState() :
    animGroupIndex(0),
    fadeInTime(0.0f),
    numCurves(0),
    startTime(0.0f),
    targetKeyArray(0,2)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nBlendState::~nBlendState()
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::SetName(const nString& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const nString&
nBlendState::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::SetAnimFile(const char *filename)
{
    n_assert(filename);
    this->animFile = filename;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char *
nBlendState::GetAnimFile() const
{
    return this->animFile.IsEmpty() ? 0 : this->animFile.Get();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::SetAnim(nAnimation *anim)
{
    this->refAnim = anim;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAnimation *
nBlendState::GetAnim()
{
    return this->refAnim.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nBlendState::IsAnimValid() const
{
    return this->refAnim.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::Invalidate()
{
    this->refAnim.invalidate();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::SetAnimGroupIndex(int index)
{
    this->animGroupIndex = index;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBlendState::GetAnimGroupIndex() const
{
    return this->animGroupIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::SetFadeInTime(float t)
{
    this->fadeInTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nBlendState::GetFadeInTime() const
{
    return this->fadeInTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::SetStartTime(float t)
{
    this->startTime = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nBlendState::GetStartTime() const
{
    return this->startTime;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::SetNumCurves(int numCurves)
{
    this->numCurves = numCurves;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nBlendState::GetNumCurves() const
{
    return this->numCurves;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::SetTargetChange(int kI, int curveIndex, int targetIndex)
{
    this->targetKeyArray.AddBlendTargetKey( kI, nBlendTargetKey(kI, curveIndex, targetIndex));
}

//------------------------------------------------------------------------------
/**
*/
inline
nBlendTargetKeyArray&
nBlendState::GetTargetKeyArray()
{
    return this->targetKeyArray;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendState::UpdateTargets( float curTime, nArray<int>& curveIndices, nArray<int>& targetIndices )
{
    // given curTime, find keys
    int keyIndex[2];
    float inbetween;
    this->GetAnim()->GetGroupAt(0).TimeToIndex( curTime, keyIndex[0], keyIndex[1], inbetween, this->GetAnim()->GetGroupAt(0).GetLoopType());

    // update targets
    this->targetKeyArray.UpdateTargets( keyIndex[0], curveIndices, targetIndices );
}

//------------------------------------------------------------------------------
#endif
