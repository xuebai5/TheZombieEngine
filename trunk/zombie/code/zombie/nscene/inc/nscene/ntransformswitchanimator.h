#ifndef N_TRANSFORMSWITCHANIMATOR_H
#define N_TRANSFORMSWITCHANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nSwitchOverrideAnimator
    @ingroup Scene
    @author MA Garcias <ma.garcias@yahoo.es>

    @brief Select a position, rotation or scaling for an index
    from a context variable.
    
    (C) 2006 Conjurer Services, S.A.
*/
#include "nscene/nanimator.h"

//------------------------------------------------------------------------------
class nTransformSwitchAnimator : public nAnimator
{
public:
    /// constructor
    nTransformSwitchAnimator();
    /// destructor
    virtual ~nTransformSwitchAnimator();
    /// save object to persistent stream
    virtual bool SaveCmds(nPersistServer* ps);
    /// return the type of this animator object (TRANSFORM)
    virtual Type GetAnimatorType() const;
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);
    /// reset all keys
    virtual void ResetKeys();

    /// set variable channel to override position
    void AddPosition(const vector3&);
    /// set variable channel to override rotation
    void AddEuler(const vector3&);

private:
    nArray<vector3> positionArray;
    nArray<vector3> rotationArray;
};

//------------------------------------------------------------------------------
#endif

