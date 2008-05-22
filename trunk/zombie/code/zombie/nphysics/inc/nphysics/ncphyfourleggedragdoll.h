#ifndef NC_PHYFOURLEGGEDRAGDOLL_H
#define NC_PHYFOURLEGGEDRAGDOLL_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyFourleggedRagDoll
    @ingroup NebulaPhysicsSystem
    @brief Base behaviour for humanoid rag-doll models.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Human Rag Doll
    
    @cppclass ncPhyFourleggedRagDoll
    
    @superclass ncPhyRagDoll

    @classinfo Base behaviour for humanoid rag-doll models.

*/

//-----------------------------------------------------------------------------
#include "nphysics/ncphyragdoll.h"

//-----------------------------------------------------------------------------

class nPhyUniversalJoint;
class nPhyHingeJoint;
class ncPhyRagDollLimb;
class ncPhyFourleggedRagDollClass;

//-----------------------------------------------------------------------------
class ncPhyFourleggedRagDoll : public ncPhyRagDoll
{

    NCOMPONENT_DECLARE(ncPhyFourleggedRagDoll,ncPhyRagDoll);

public:
    typedef enum {
        left = 0,
        right
    } which;

    /// constructor
    ncPhyFourleggedRagDoll();

    /// destructor
    virtual ~ncPhyFourleggedRagDoll();

    /// process of creating of the object
    void Create( nPhysicsWorld* world );

    /// function to be procesed during the collision check
    bool CollisionProcess( int numContacts, nPhyCollide::nContact* contacts );

    /// head 
    ncPhyRagDollLimb* GetHeadObj() const;

    /// left arm
    ncPhyRagDollLimb* GetLeftArm() const;

    /// right arm
    ncPhyRagDollLimb* GetRightArm() const;

    /// left forearm
    ncPhyRagDollLimb* GetLeftForeArm() const;

    /// right forearm
    ncPhyRagDollLimb* GetRightForeArm() const;

    /// left leg
    ncPhyRagDollLimb* GetLeftLeg() const;

    /// right leg
    ncPhyRagDollLimb* GetRightLeg() const;

    /// left foreLeg
    ncPhyRagDollLimb* GetForeLeftLeg() const;

    /// right foreLeg
    ncPhyRagDollLimb* GetForeRightLeg() const;

    /// begin:scripting
    
        /// sets the head object
        void SetHead(ncPhyRagDollLimb*);
        /// sets the arm object
        void SetArm(const which,ncPhyRagDollLimb*);
        /// sets the fore arm object
        void SetForeArm(const which,ncPhyRagDollLimb*);
        /// sets the leg object
        void SetLeg(const which,ncPhyRagDollLimb*);
        /// sets the fore leg object
        void SetForeLeg(const which,ncPhyRagDollLimb*);
        /// sets the neck in world position
        void SetNeckPoint(const vector3&);
        /// sets the shoulder point
        void SetShoulderPoint(const which,const vector3&);
        /// sets the elbow point
        void SetElbowPoint(const which,const vector3&);
        /// sets the wrist position
        void SetWristPoint(const which,const vector3&);
        /// sets the knee position
        void SetKneePoint(const which,const vector3&);

    /// end:scripting

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// assembles the body
    void Assemble();
    
private:

    /// neck position
    vector3 neckPosition;

    /// left shoulder position
    vector3 leftShoulderPosition;

    /// right shoulder position
    vector3 rightShoulderPosition;

    /// left elbow position
    vector3 leftElbowPosition;

    /// right elbow position
    vector3 rightElbowPosition;

    /// left wrist position
    vector3 leftWristPosition;

    /// right wrist position
    vector3 rightWristPosition;

    /// left knee position
    vector3 leftKneePosition;

    /// right knee position
    vector3 rightKneePosition;

    /// body object
    ncPhysicsObj *mainBody; // (itself)

    /// head 
    ncPhyRagDollLimb* head;

    /// left arm
    ncPhyRagDollLimb* leftArm;

    /// right arm
    ncPhyRagDollLimb* rightArm;

    /// left forearm
    ncPhyRagDollLimb* leftForeArm;

    /// right forearm
    ncPhyRagDollLimb* rightForeArm;

    /// left leg
    ncPhyRagDollLimb* leftLeg;

    /// right leg
    ncPhyRagDollLimb* rightLeg;

    /// left foreLeg
    ncPhyRagDollLimb* foreLeftLeg;

    /// right foreLeg
    ncPhyRagDollLimb* foreRightLeg;

    /// neck
    nPhyUniversalJoint* neck;

    /// left shoulder
    nPhyUniversalJoint* leftShoulder;

    /// right shoulder
    nPhyUniversalJoint* rightShoulder;

    /// left shoulder
    nPhyHingeJoint* leftElbow;

    /// right shoulder
    nPhyHingeJoint* rightElbow;

    /// left wrist
    nPhyUniversalJoint* leftWrist;

    /// right wrist
    nPhyUniversalJoint* rightWrist;

    /// left knee
    nPhyHingeJoint* leftKnee;

    /// right knee
    nPhyHingeJoint* rightKnee;

    /// stores a reference to the class of this component
    ncPhyFourleggedRagDollClass* refClass;

    /// attaches the head
    void AttachHead();

    /// attaches an arm
    void AttachArm( const which arm );

    /// attaches a forearm
    void AttachForeArm( const which foreArm );

    /// attaches a leg
    void AttachLeg( const which leg );

    /// attaches a Foreleg
    void AttachForeLeg( const which foreLef );

    /// creates the articulations
    void CreateArticulations();

    /// creates the neck
    void CreateNeck();

    /// creates the elbows
    void CreateElbows();

    /// creates the shoulders
    void CreateShoulders();

    /// creates the wrist
    void CreateWrist();

    /// creates the knees
    void CreateKnees();

    /// adds the limbs to the world
    void AddLimbs();

};

//-----------------------------------------------------------------------------
/**
    Head.

    @return the ragdoll head

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhyRagDollLimb* ncPhyFourleggedRagDoll::GetHeadObj() const
{
    return this->head;
}

//-----------------------------------------------------------------------------
/**
    Left Arm.

    @return the ragdoll left arm

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhyRagDollLimb* ncPhyFourleggedRagDoll::GetLeftArm() const
{
    return this->leftArm;
}

//-----------------------------------------------------------------------------
/**
    Right Arm.

    @return the ragdoll right arm

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhyRagDollLimb* ncPhyFourleggedRagDoll::GetRightArm() const
{
    return this->rightArm;
}

//-----------------------------------------------------------------------------
/**
    Left Fore Arm.

    @return the ragdoll left fore arm
    
    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhyRagDollLimb* ncPhyFourleggedRagDoll::GetLeftForeArm() const
{
    return this->leftForeArm;
}

//-----------------------------------------------------------------------------
/**
    Right Fore Arm.

    @return the ragdoll right fore arm

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhyRagDollLimb* ncPhyFourleggedRagDoll::GetRightForeArm() const
{
    return this->rightForeArm;
}

//-----------------------------------------------------------------------------
/**
    Left leg.

    @return the ragdoll left leg

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhyRagDollLimb* ncPhyFourleggedRagDoll::GetLeftLeg() const
{
    return this->leftLeg;
}

//-----------------------------------------------------------------------------
/**
    Right leg.

    @return the ragdoll right leg

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhyRagDollLimb* ncPhyFourleggedRagDoll::GetRightLeg() const
{
    return this->rightLeg;
}

//-----------------------------------------------------------------------------
/**
    Left foreLeg.

    @return the ragdoll fore left leg

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhyRagDollLimb* ncPhyFourleggedRagDoll::GetForeLeftLeg() const
{
    return this->foreLeftLeg;
}

//-----------------------------------------------------------------------------
/**
    Right foreLeg.

    @return the ragdoll fore right leg

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
ncPhyRagDollLimb* ncPhyFourleggedRagDoll::GetForeRightLeg() const
{
    return this->foreRightLeg;
}

N_CMDARGTYPE_NEW_TYPE(ncPhyFourleggedRagDoll::which, "i", (value = (ncPhyFourleggedRagDoll::which) cmd->In()->GetI()), (cmd->Out()->SetI(int(value)))  );

#endif 
