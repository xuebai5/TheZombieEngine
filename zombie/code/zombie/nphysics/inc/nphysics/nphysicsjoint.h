#ifndef N_PHYSICSJOINT_H
#define N_PHYSICSJOINT_H

//-----------------------------------------------------------------------------
/**
    @class nPhysicsJoint
    @ingroup NebulaPhysicsSystem
    @brief An abstract representation of a joint

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Joint
    
    @cppclass nPhysicsJoint
    
    @superclass nObject

    @classinfo An abstract representation of a joint.
*/    

//-----------------------------------------------------------------------------

#include "kernel/nobject.h"
#include "nphysics/nphysicsconfig.h"

//-----------------------------------------------------------------------------

class nPhyRigidBody;
class nPhysicsWorld;
class nPhyJointGroup;
class ncPhysicsObj;

//-----------------------------------------------------------------------------
N_CMDARGTYPE_NEW_TYPE(phy::jointparameter, "i", (value = (phy::jointparameter) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );
N_CMDARGTYPE_NEW_TYPE(phy::jointaxis, "i", (value = (phy::jointaxis) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );
//-----------------------------------------------------------------------------

class nPhysicsJoint : public nObject {
public:
    typedef enum {
        Contact = 0,
        TwoHinge,
        BallSocket,
        Hinge,
        Slider,
        Universal,
        AngularMotor,
        Num,
        Invalid
    } JointTypes;

    /// constructor
    nPhysicsJoint();

    /// destructor
    virtual ~nPhysicsJoint();

    /// returns the joint type
    JointTypes Type() const;

    /// attaches the joint
    virtual void Attach( nPhyRigidBody* bodyA, nPhyRigidBody* bodyB );

    /// attaches the joint
    virtual void Attach( bodyid bodyA, bodyid bodyB );
    
    /// returns if it's a breakable joint
    bool IsBreakable() const;

    /// set breakable
    void SetBreakable( bool is );

    /// Sets the force limit of a breakable joint
    void SetMaxForceBodyA( const phyreal force );
    /// Sets the force limit of a breakable joint
    void SetMaxForceBodyB( const phyreal force );
        
    /// Set the torque limit of a breakable joint
    void SetMaxTorqueBodyA( const phyreal torque );
    /// Set the torque limit of a breakable joint
    void SetMaxTorqueBodyB( const phyreal torque );

    /// returns the id of this joint
    jointid Id() const;

    /// returns body A Id (no valid if it's attached to the environment)
    nPhyRigidBody* GetBodyA() const;
    
    /// returns body B Id (no valid if it's attached to the environment)
    nPhyRigidBody* GetBodyB() const;

    /// returns the world id of this joint
    nPhysicsWorld* GetWorld() const;

    /// breakability information
    class Breakability {
        public:
            /// constructor
            explicit Breakability( const jointid id );

            /// destructor
            ~Breakability();

            /// returns if the conditions are for break
            bool IsBreakable() const;

            /// sets the force limit of a breakable joint
            void SetMaxForceBodyA( const phyreal force );
            /// sets the force limit of a breakable joint
            void SetMaxForceBodyB( const phyreal force );
                
            /// set the torque limit of a breakable joint
            void SetMaxTorqueBodyA( const phyreal torque );
            /// set the torque limit of a breakable joint
            void SetMaxTorqueBodyB( const phyreal torque );

        private:
            /// structure of the maximun values of force suported
            phyreal maxForceBodyA;
            phyreal maxForceBodyB;
            phyreal maxTorqueBodyA;
            phyreal maxTorqueBodyB;

            /// structure holding extra data information
            jointdata* jointData;

            /// joint id
            jointid jointID;
    };

    /// checks if the joint has to be broken, and break it
    bool Break();

    /// breaks the joint inconditionally
    void ForceBreak();

    /// adds torque to the motor joint
    void AddTorque( const phyreal torque, const phy::jointaxis axis = phy::axisA );

    /// adds force to the motor joint
    void AddForce( const phyreal torque, const phy::jointaxis axis = phy::axisA );

    /// gets the axis direction
    virtual void GetAxisDirection( vector3& direction, const phy::jointaxis axis = phy::axisA );

    /// creates the join
    virtual void CreateIn(nPhysicsWorld*, nPhyJointGroup*);

    /// returns breakability info
    Breakability* GetBreakabilityInfo() const;

    /// begin:scripting
    
    /// attaches two physics objs
   virtual void Attach(ncPhysicsObj*, ncPhysicsObj*);

    /// sets a parameters of the joints
   virtual void SetParam(const phy::jointparameter, const phy::jointaxis, const phyreal);
    
    /// creates the join
    void Create(nPhysicsWorld*);

    /// end:scripting

    /// scales the object
    virtual void Scale( const phyreal factor );

#ifndef NGAME
    /// draws the joint
    virtual void Draw( nGfxServer2* server );
#endif

    /// returns a parameter value
    virtual const phyreal GetParam( const phy::jointparameter, const phy::jointaxis ) const;

    /// resets the joint
    void Reset();

protected:

    /// stores the joint type
    JointTypes type; 

    /// stores the joint ID
    jointid jointID;

    /// stores the information for breakable joints
    Breakability* breakability;

    /// stores the world where this joint exists
    nPhysicsWorld* world;

    /// joint group where the joint belongs
    nPhyJointGroup* groupJoint;

    /// stores the bodies where the joint it's attached
    nPhyRigidBody* rbodyA; 
    nPhyRigidBody* rbodyB; 
};

//-----------------------------------------------------------------------------
/**
    Returns the id of this joint.

    history:
        - 25-Sep-2004   David Reyes    created
        - 12-Dec-2004   David Reyes    inlined
*/
inline
jointid nPhysicsJoint::Id() const
{
    return this->jointID;
}

//-----------------------------------------------------------------------------
/**
    Creates the joint in the default jointgroup

    history:
        - 05-May-2005   David Reyes    inlined
*/
inline
void nPhysicsJoint::Create( nPhysicsWorld* world )
{
    this->CreateIn( world, 0 );
}

#endif