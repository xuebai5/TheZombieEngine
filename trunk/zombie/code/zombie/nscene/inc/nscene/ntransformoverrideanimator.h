#ifndef N_TRANSFORMOVERRIDEANIMATOR_H
#define N_TRANSFORMOVERRIDEANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nTransformOverrideAnimator
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Override position, orientation and scaling of a nTransformNode from
    a context variable, in both local or global coordinates.

    (C) 2006 Conjurer Services, S.A.
*/
#include "nscene/nanimator.h"
#include "util/nanimkeyarray.h"

//------------------------------------------------------------------------------
class nTransformOverrideAnimator : public nAnimator
{
public:
    /// constructor
    nTransformOverrideAnimator();
    /// destructor
    virtual ~nTransformOverrideAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// return the type of this animator object (TRANSFORM)
    virtual Type GetAnimatorType() const;
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);

    /// set variable channel to override position
    void SetPositionChannel(const char *);
    /// set variable channel to override rotation
    void SetRotationChannel(const char *);

private:
    nVariable::Handle positionChannel;
    nVariable::Handle rotationChannel;
};

//------------------------------------------------------------------------------
#endif

