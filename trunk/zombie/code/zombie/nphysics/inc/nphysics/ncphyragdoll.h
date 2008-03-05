#ifndef NC_PHYRAGDOLL_H
#define NC_PHYRAGDOLL_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyRagDoll
    @ingroup NebulaPhysicsSystem
    @brief Base behaviour for rag-doll models.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Basic Rag-doll
    
    @cppclass ncPhyRagDoll
    
    @superclass ncPhyCompositeObj

    @classinfo Base behaviour for rag-doll models.

*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphycompositeobj.h"
#include "nphysics/ncphyragdolllimb.h"

//-----------------------------------------------------------------------------

class nPhysicsJoint;
class nPhyGeomSpace;
class nPhySpace;

//-----------------------------------------------------------------------------
class ncPhyRagDoll : public ncPhyCompositeObj
{

    NCOMPONENT_DECLARE(ncPhyRagDoll,ncPhyCompositeObj);

public:
    typedef enum {
        human = 0,
        alienA,
        alienB, // etc
        End,
    } ragdolltype;

    /// constructor
    ncPhyRagDoll();

    /// destructor
    virtual ~ncPhyRagDoll();

    /// process of creating of the object
    virtual void Create( nPhysicsWorld* world );

    /// add geometry to the body
    void AddGeometryToMainBody( nPhysicsGeom* geometry );

    /// create limb joint
    void AddLimbJoint( nPhysicsJoint* limbjoint, nPhysicsJoint* parent = 0 );

    /// Adds limb to the hierarchy
    void AddLimb( ncPhyRagDollLimb* limb );

    /// Removes a limb to the hierarchy
    void RemoveLimb( ncPhyRagDollLimb* limb );

    /// breaks a limb joint
    void BreakLimbJoint( const ncPhyRagDollLimb::rdId limb );

    /// enables the physic object
    void Enable();

    /// disables the physic object
    void Disable();

    /// sets the position of the rigid body
    void SetPosition( const vector3& newposition );

    /// sets the rotation of the rag-doll
    void SetRotation( const matrix33& newrotation );

    /// sets the rotation of the rag-doll
    void SetRotation( phyreal ax, phyreal ay, phyreal az );

    /// returns the rag-doll space
    nPhySpace* GetSpace() const;

    /// type joints container
    typedef nArray< nPhysicsJoint* > tContainerJoints;

    /// accessor to the joints container
    const tContainerJoints& GetJointsContainer() const;

    /// returns the parent joint
    nPhysicsJoint* GetParentJoint( const nPhysicsJoint* joint );

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);
    
    /// scales the object
    void Scale( const phyreal factor );

#ifndef NGAME

    /// moves the object to limbo
    virtual void YouShallDwellIntoTheLimbo();

    /// recovers an object from the limbo
    virtual void YourSoulMayComeBackFromLimbo();

#endif // !NGAME

protected:

    /// creates the rag-doll space
    void CreateSpace();

private:
    
    /// container of parent joints
    nKeyArray< nPhysicsJoint* > parentJoints;

    /// rag-doll space
    nPhyGeomSpace* space;

    /// number of initial geometries space
    static const int NumInitialLimbs = 10;

    /// growth pace
    static const int NumGrowthLimbs = 2;

    /// number of initial geometries space
    static const int NumInitialJointLimbs = 10;

    /// growth pace
    static const int NumGrowthJointLimbs = 2;

    /// type limbs container
    typedef nKeyArray< ncPhyRagDollLimb* > tContainerLimbs;

    /// limbs container
    tContainerLimbs containerLimbs;

    /// joints container
    tContainerJoints containerJoints;

};

#endif 
