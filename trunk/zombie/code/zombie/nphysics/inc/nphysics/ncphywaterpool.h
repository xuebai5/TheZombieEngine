#ifndef NC_PHYWATERPOOL_H
#define NC_PHYWATERPOOL_H

//-----------------------------------------------------------------------------
/**
    @class ncPhySea
    @ingroup NebulaPhysicsSystem
    @brief Simulates a water pool behaviour.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Water Pool.
    
    @cppclass ncPhyWaterPool
    
    @superclass ncPhyAreaDensity

    @classinfo Simulates a water pool behaviour.

*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphyareadensity.h"

//-----------------------------------------------------------------------------
class ncPhyWaterPool : public ncPhyAreaDensity
{

    NCOMPONENT_DECLARE(ncPhyWaterPool,ncPhyAreaDensity);

public:
    /// constructor
    ncPhyWaterPool();

    /// destructor
    ~ncPhyWaterPool();

    /// process of creating of the object
    virtual void Create( nPhysicsWorld* world );

    /// check if the box is under the fluid
    bool IsUnderFluid( const nPhysicsAABB& box ) const;

    /// function to be procesed during the collision check
    bool CollisionProcess( int numContacts, nPhyCollide::nContact* contacts );

    /// sets the position of this physic object
    void SetPosition( const vector3& newposition );

    /// sets the rotation of this physic object
    void SetRotation( const matrix33& newrotation );

    /// begin:scripting
    
    /// sets the lengths of the pool
    void SetPoolsLength(const vector3&);
    /// gets the lengths of the pool
    const vector3& GetPoolsLength() const;

    /// end:scripting

    /// object persistency.
    bool SaveCmds(nPersistServer* ps);

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

private:

    /// stores the pool bounding box (not an AA).
    nPhysicsAABB bboxPool;    

    /// rotation matrix for the points
    matrix33 matrixPool;

    /// stores the pool lengths
    vector3 lengthsPool;

    /// computes the bounding box (not an AA)
    void UpdateBoundingBox();

    /// helper function to find out if a point belongs to the pool area
    bool ItBelongs( const vector3& worldpoint ) const;

    /// default resitance
    static const phyreal DefaultResistance;

};

#endif 