#ifndef N_BLENDSHAPEANIMATOR_H
#define N_BLENDSHAPEANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nBlendShapeAnimator
    @ingroup Scene
 
    (C) 2004 RadonLabs GmbH
*/
#include "nscene/nanimator.h"
#include "nscene/nblendstatearray.h"
#include "anim2/nanimation.h"
#include "anim2/nanimationserver.h"

//------------------------------------------------------------------------------
class nBlendShapeAnimator : public nAnimator
{
public:
    /// constructor
    nBlendShapeAnimator();
    /// destructor
    virtual ~nBlendShapeAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// load resources
    virtual bool LoadResources();
    /// unload resources
    virtual void UnloadResources();
    /// return the type of this animator object
    virtual Type GetAnimatorType() const;

    /// begin anim state definition
    void BeginStates(int num);
    /// add an animation state
    void SetState(int stateIndex, int animGroupIndex);
    /// set animation file for state
    void SetStateAnim(int stateIndex, const char* filename);
    /// get animation file for state
    const char* GetStateAnim(int stateIndex);
    /// set optional state name
    void SetStateName(int stateIndex, const nString& name);
    /// finish adding states
    void EndStates();
    /// get number of states
    int GetNumStates() const;
    /// get state attributes
    const nBlendState& GetStateAt(int stateIndex);
    /// set target change
    void SetTargetChange( int stateIndex, int keyIndex, int curveIndex, int targetIndex );
    /// add state
    void AddState( int animGroupIndex );

    //<OBSOLETE>
    /// set the animation resource name
    void SetAnimation(const char* name);
    /// get the animation resource name
    const char* GetAnimation();
    /// set the animation group to use
    void SetAnimationGroup(int group);
    /// get which animation group is used
    int GetAnimationGroup();
    //</OBSOLETE>

    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);

private:
    /// load animation resources
    bool LoadAnims();
    /// unload animation resources
    void UnloadAnims();

    nBlendStateArray blendStateArray;

    //<OBSOLETE>
    /// load animation resource
    bool LoadAnimation();
    /// unload animation resource
    void UnloadAnimation();

    int animationGroup;
    nString animationName;

    nRef<nAnimation> refAnimation;
    //</OBSOLETE>
};

//------------------------------------------------------------------------------
/**
    <OBSOLETE>
    Set the resource name. The animation resource name consists of the
    filename of the animation.
*/
inline
void
nBlendShapeAnimator::SetAnimation(const char* name)
{
    n_assert(name);
    this->UnloadAnimation();
    this->animationName = name;
}

//------------------------------------------------------------------------------
/**
    <OBSOLETE>
    Get the animation resource name.
*/
inline
const char*
nBlendShapeAnimator::GetAnimation()
{
    return this->animationName.Get();
}

//------------------------------------------------------------------------------
/**
    <OBSOLETE>
*/
inline
void
nBlendShapeAnimator::SetAnimationGroup(int group)
{
    this->animationGroup = group;
}

//------------------------------------------------------------------------------
/**
    <OBSOLETE>
*/
inline
int
nBlendShapeAnimator::GetAnimationGroup()
{
    return this->animationGroup;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nBlendShapeAnimator::AddState( int animGroupIndex )
{
    nBlendState newState;
    newState.SetAnimGroupIndex(animGroupIndex);
    this->blendStateArray.AddState( newState );
}

//------------------------------------------------------------------------------
#endif

