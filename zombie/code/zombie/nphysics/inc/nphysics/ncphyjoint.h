#ifndef NC_PHYJOINT_H
#define NC_PHYJOINT_H
//-----------------------------------------------------------------------------
/**
    @class ncPhyJoint
    @ingroup NebulaPhysicsSystem
    @brief Component for entities that represents any type of joint.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Joint
    
    @cppclass ncPhyJoint
    
    @superclass ncPhyPickableObj

    @classinfo Component for entities that represents any type of joint.

*/

//-----------------------------------------------------------------------------
#include "entity/nentity.h"
#include "nphysics/nphysicsjoint.h"
#ifndef NGAME
#include "nphysics/ncphypickableobj.h"
#endif

//-----------------------------------------------------------------------------

class nPhyRigidBody;

//-----------------------------------------------------------------------------

N_CMDARGTYPE_NEW_TYPE(nPhysicsJoint::JointTypes, "i", (value = (nPhysicsJoint::JointTypes) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );

//-----------------------------------------------------------------------------
#ifndef NGAME
class ncPhyJoint : public ncPhyPickableObj
{

    NCOMPONENT_DECLARE(ncPhyJoint,ncPhyPickableObj);

#else
class ncPhyJoint : public nComponentObject
{

    NCOMPONENT_DECLARE(ncPhyJoint,nComponentObject);

#endif

public:
    /// constructor
    ncPhyJoint();

    /// destructor
    ~ncPhyJoint();

    /// object persistency.
    bool SaveCmds(nPersistServer* ps);

    /// stores the body A
    void SetBodyA( nPhyRigidBody* body );

    /// stores the body B
    void SetBodyB( nPhyRigidBody* body );

    /// begin:scripting
    
    /// mounts the joint
    void Mount();

    /// unmounts the joint
    void UnMount();

    /// sets the id of the object if any
    void SetObjectA(const nEntityObjectId);
    /// gets the id of the object if any
    const nEntityObjectId GetObjectA() const;

    /// sets the id of the object if any
    void SetObjectB(const nEntityObjectId);
    /// gets the id of the object if any
    const nEntityObjectId GetObjectB() const;

    /// sets the type of joint
    void SetType(const nPhysicsJoint::JointTypes);
    /// gets the type of joint
    nPhysicsJoint::JointTypes GetType() const;

    /// sets the joint's whereabouts
    void SetAnchorPoint(const vector3&);

    /// sets the first axis
    void SetFirstAxis(const vector3&);

    /// sets low stop angle or position
    void SetLowStopAnglePosition(const phy::jointaxis, const phyreal);

    /// sets high stop angle or position
    void SetHiStopAnglePosition(const phy::jointaxis, const phyreal);

    /// sets desired motor velocity (this will be an angular or linear velocity)
    void SetVelocity(const phy::jointaxis, const phyreal);

    /// the maximum force or torque that the motor will use to achieve the desired velocity
    void SetMaximunForce(const phy::jointaxis, const phyreal);

    /// sets the fudge factor
    void SetFudgeFactor(const phy::jointaxis, const phyreal);

    /// sets the bouncyness of the stops
    void SetBouncynessStop(const phy::jointaxis, const phyreal);

    /// sets the constraint force mixing (CFM) value used when not at a stop
    void SetCFM(const phy::jointaxis, const phyreal);

    /// sets the error reduction parameter (ERP) used by the stops
    void SetStopERP(const phy::jointaxis, const phyreal);

    /// sets the constraint force mixing (CFM) value used by the stops
    void SetStopCFM(const phy::jointaxis, const phyreal);

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    /// mounts the selected object(s)
    void MountSelected();
#endif
#endif

    /// end:scripting

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
#endif
#endif

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// returns the anchor point if any
    bool GetAnchorPoint( vector3& point ) const;

    /// returns the first axis if any
    bool GetFirstAxis( vector3& axis ) const;

#ifndef NGAME
    /// sets the object's position
    virtual void SetPosition( const vector3& newposition );
    /// function to be process after running the simulation
    virtual void PostProcess();
#endif

private:
    /// stores the type of joint
    nPhysicsJoint::JointTypes type;

    /// stores the body A
    nPhyRigidBody* bodyA;

    /// stores the body B
    nPhyRigidBody* bodyB;

    /// stores the object A Id
    nEntityObjectId idA;
    /// stores the object B Id
    nEntityObjectId idB;

    /// stores if the object is mounted
    bool mounted;

    /// stores the physics joint
    nPhysicsJoint* joint;

    /// creates the physics joint
    void CreateJoint();

};

#endif // NC_PHYJOINT_H
