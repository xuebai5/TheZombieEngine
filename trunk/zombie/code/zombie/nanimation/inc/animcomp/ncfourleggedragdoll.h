#ifndef NC_FOURLEGGEDRAGDOLL_H
#define NC_FOURLEGGEDRAGDOLL_H
//------------------------------------------------------------------------------
/**
    @class ncFourLeggedRagdoll
    @ingroup Scene
    @author

    @brief Ragdoll component object for entities.

    (C) 2005 Conjurer Services, S.A.
*/
#include "ncragdoll/ncragdoll.h"

class nRagdollCharAnimator;

//------------------------------------------------------------------------------
class ncFourLeggedRagdoll : public ncRagDoll
{

    NCOMPONENT_DECLARE(ncFourLeggedRagdoll,ncRagDoll);

public:
    /// limbs
    enum HumanLimbs
    {
        Head = 0,
        LeftArm,
        RightArm,
        LeftForeArm,
        RightForeArm,
        LeftLeg,
        RightLeg,
        LeftForeLeg,
        RightForeLeg,
        NumLimbs,
    };

    /// constructor
    ncFourLeggedRagdoll();
    /// destructor
    ~ncFourLeggedRagdoll();

    /// initialize entity
    void InitInstance(nObject::InitInstanceMsg initType);

    /// set joint data
    void SetJointData(int jointIndex, vector3& midPos, vector3& pos, const quaternion& rot, const vector3& scale);

    /// load
    bool Load(nEntityObject* parentObj);
    /// unload
    void Unload();

    /// animate
    void PhysicsAnimate();
    /// get physics bounding box
    void GetPhysicsBBox(bbox3& bbox);

    /// save state of the component
    bool SaveCmds(nPersistServer *);

    /// initialize ragdoll, setmass and add to world
    void InitializeRagdoll();

    // switchs off the ragdoll
    void SwitchOff();

    ///find mid pos
    void FindMidPosForExtremeJoints(vector3& midPos, const matrix33& rotmatrix);

private:
    nRef<nEntityObject> refLimbs[NumLimbs];

    bool active;

};

//------------------------------------------------------------------------------
#endif
