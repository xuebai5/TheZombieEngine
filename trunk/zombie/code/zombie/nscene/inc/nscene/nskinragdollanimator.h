#ifndef N_SKINRAGDOLLANIMATOR_H
#define N_SKINRAGDOLLANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nSkinRagdollAnimator
    @ingroup SceneAnimators

    @brief Provide a interface with ragdoll physics.

    On Animate() invocation, the nSkinRagdollAnimator will retrieve
    a ragdoll skeleton from physics server and build a proper character
    skeleton compatible with nSkinGeometryNode, then call SetCharSkeleton()
    on the calling node as usual. It requires that a character state 
    is specified for the ragdoll-animated state.

    See also @ref N2ScriptInterface_nskinragdollanimator
    
    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/nskingeometryanimator.h"
#include "kernel/ncmdprotonativecpp.h"
#include "util/nkeyarray.h"
#include "mathlib/vector.h"

class nCharAnimator;
class nScriptServer;
//------------------------------------------------------------------------------
class nSkinRagdollAnimator : public nSkinGeometryAnimator
{
public:
    /// constructor
    nSkinRagdollAnimator();
    /// destructor
    virtual ~nSkinRagdollAnimator();
    /// object persistency
    virtual bool SaveCmds(nPersistServer *ps);
    /// called by app when new render context has been created for this object
    virtual void EntityCreated(nEntityObject* entityObject);
    /// called by app when render context is going to be released
    virtual void EntityDestroyed(nEntityObject* entityObject);
    /// called by scene node objects which wish to be animated by this object
    virtual void Animate(nSceneNode* sceneNode, nEntityObject* entityObject);

    /// set ragdoll joint for the local joint
    void SetRagdollJoint(int, int);
    /// get ragdoll joint for the local joint
    int GetRagdollJoint(int);

private:
    int charAnimVarIndex;
    nAutoRef<nScriptServer> refScriptServer;
    nKeyArray<int> ragdollJoints;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSkinRagdollAnimator::SetRagdollJoint(int jointIndex, int ragdollJointIndex)
{
    this->ragdollJoints.Add(jointIndex, ragdollJointIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSkinRagdollAnimator::GetRagdollJoint(int jointIndex)
{
    int ragdollJointIndex;
    if (this->ragdollJoints.Find(jointIndex, ragdollJointIndex))
    {
        return ragdollJointIndex;
    }
    return -1;
}

//------------------------------------------------------------------------------
#endif
