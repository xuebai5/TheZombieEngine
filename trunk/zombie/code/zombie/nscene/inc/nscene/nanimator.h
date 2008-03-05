#ifndef N_ANIMATOR_H
#define N_ANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nAnimator
    @ingroup Scene

    @brief Animator nodes manipulate properties of other scene objects.
    
    They are not attached to the scene, instead they are called back by scene
    objects which wish to be manipulated.

    See also @ref N2ScriptInterface_nanimator

    (C) 2003 RadonLabs GmbH
*/
#include "nscene/nscenenode.h"
#include "variable/nvariable.h"
#include "util/nanimlooptype.h"

class nVariableServer;

//------------------------------------------------------------------------------
class nAnimator : public nSceneNode
{
public:
    enum Type
    {
        InvalidType,    // an invalid type
        Transform,      // a transform animator
        Shader,         // a shader animator
        BlendShape,     // a blend shape animator
        Deformer,       // a deformer animator
    };

    /// constructor
    nAnimator();
    /// destructor
    virtual ~nAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);    
    /// return the type of this animator object
    virtual Type GetAnimatorType() const;
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);
    /// reset all keys (override in subclasses, for persistence only)
    virtual void ResetKeys();

    /// set the variable handle which drives this animator object (e.g. time)
    void SetChannel(const char* name);
    /// get the variable which drives this animator object
    const char* GetChannel();
    /// set the loop type
    void SetLoopType(nAnimLoopType::Type t);
    /// get the loop type
    nAnimLoopType::Type GetLoopType() const;

    /// set fixed key offset (eg. time increments)
    void SetFixedTimeOffset(float offset);
    /// get fixed key offset
    float GetFixedTimeOffset() const;

    /// get time
    float GetTime(nEntityObject* entityObject) const;

protected:
    nAnimLoopType::Type loopType;
    nVariable::Handle channelVarHandle;
    nVariable::Handle channelOffsetVarHandle;

    float fixedTimeOffset;
};

//------------------------------------------------------------------------------
/**
    Set the loop type for this animation.
*/
inline
void
nAnimator::SetLoopType(nAnimLoopType::Type t)
{
    this->loopType = t;
}

//------------------------------------------------------------------------------
/**
    Get the loop type for this animation.
*/
inline
nAnimLoopType::Type
nAnimator::GetLoopType() const
{
    return this->loopType;
}

//------------------------------------------------------------------------------
#endif
