#ifndef N_RAGDOLLCHARANIMATOR_H
#define N_RAGDOLLCHARANIMATOR_H
//------------------------------------------------------------------------------
/**
    @class nRagdollCharAnimator
    @ingroup NebulaCharacterAnimationSystem

    @brief A class to provide an interface for physics animation.

    On calling Animate(), the nCharacterAnimator can update the character's
    skeleton from a physics animated ragdoll struct.

    (C) 2005 Conjurer Services, S.A.
*/
#include "nscene/ncharacteranimator.h"
#include "util/nkeyarray.h"
#include "mathlib/vector.h"
#include "mathlib/quaternion.h"
#include "nphysics/ncphyhumragdoll.h"
#include "nphysics/ncphyfourleggedragdoll.h"

#include "kernel/ncmdprotonativecpp.h"

//------------------------------------------------------------------------------
class quaternion;
class nPhysicsJoint;
class nScriptServer;

//------------------------------------------------------------------------------
class nRagdollCharAnimator : public nCharacterAnimator
{
public:
    /// constructor
    nRagdollCharAnimator();
    /// destructor
    ~nRagdollCharAnimator();
    /// update the character skeleton
    virtual bool Animate(nCharacter2* character, nVariableContext* varContext);

    /// set physics ragdoll
    void SetPhyRagdoll(nEntityObject * ragdoll);
    /// get physics ragdoll
    ncPhyHumRagDoll *GetPhyRagdoll();
    /// set ragdoll joints
    void SetRagdollJoints(nKeyArray<int> *joints);
    /// get ragdoll joints
    nKeyArray<int> *GetRagdollJoints();

    /// animate humanoid skeleton
    bool AnimateHuman(nCharacter2* character);
    /// animate four legged skeleton
    bool Animate4Legged(nCharacter2* character);

private:
    /// get joint world transformations
    void GetPhysicsWorldTransformations( int ragdollIndex, vector3 & position, quaternion & rotation, vector3 & scale );
    /// get physics position in world coordinates
    vector3 GetPhysicsWorldPosition( nPhysicsJoint* ragjoint );
    /// get physics scale in world coordinates
    vector3 GetPhysicsScale( nPhysicsJoint* ragjoint );

    nRef<nEntityObject> humragdoll;
    nRef<nEntityObject> fourleggedRagdoll;
    nKeyArray<int> *ragdollJoints;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nRagdollCharAnimator::SetPhyRagdoll(nEntityObject *ragdoll)
{
    if( ragdoll->GetComponent<ncPhyHumRagDoll>())
    {
        this->humragdoll = ragdoll;
    }
    if( ragdoll->GetComponent<ncPhyFourleggedRagDoll>())
    {
        this->fourleggedRagdoll = ragdoll;
    }
}

//------------------------------------------------------------------------------
/**
*/
inline
ncPhyHumRagDoll *
nRagdollCharAnimator::GetPhyRagdoll()
{
    return this->humragdoll.get()->GetComponentSafe<ncPhyHumRagDoll>();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nRagdollCharAnimator::SetRagdollJoints(nKeyArray<int> *joints)
{
    this->ragdollJoints = joints;
}

//------------------------------------------------------------------------------
/**
*/
inline
nKeyArray<int> *
nRagdollCharAnimator::GetRagdollJoints()
{
    return this->ragdollJoints;
}

//------------------------------------------------------------------------------
#endif
