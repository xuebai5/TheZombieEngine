#ifndef NC_PHYSEA_H
#define NC_PHYSEA_H

//-----------------------------------------------------------------------------
/**
    @class ncPhySea
    @ingroup NebulaPhysicsSystem
    @brief Simulates a sea behaviour.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @component Physics Sea.
    
    @cppclass ncPhySea
    
    @superclass ncPhyAreaDensity

    @classinfo Simulates a sea behaviour.
*/

//-----------------------------------------------------------------------------

#include "nphysics/ncphyareadensity.h"

//-----------------------------------------------------------------------------
class ncPhySea : public ncPhyAreaDensity
{

    NCOMPONENT_DECLARE(ncPhySea,ncPhyAreaDensity);

public:
    /// constructor
    ncPhySea();

    /// destructor
    ~ncPhySea();

    /// process of creating of the object
    virtual void Create( nPhysicsWorld* world );

    /// check if the box is under the fluid
    bool IsUnderFluid( const nPhysicsAABB& box ) const;

    /// gets the sea level
    phyreal GetSeaLeavel() const;

    /// function to be procesed during the collision check
    bool CollisionProcess( int numContacts, nPhyCollide::nContact* contacts );

    /// object persistency
    bool SaveCmds(nPersistServer* ps);

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// sets the position of the rigid body
    virtual void SetPosition( const vector3& newposition );

    /// sets the rotation of this physic object
    virtual void SetRotation( const matrix33& newrotation );

private:

    /// sets the sea level
    void SetSeaLeavel( phyreal level );

    /// set sea level
    phyreal sealevel;

    /// default sea level
    static const phyreal DefaultSeaLevel;

    /// default resitance
    static const phyreal DefaultResistance;

};

//-----------------------------------------------------------------------------
/**
    Gets the sea level.

    @return height value

    history:
        - 14-Jan-2005   David Reyes    created
        - 27-Jan-2005   David Reyes    inlined
*/
inline
phyreal ncPhySea::GetSeaLeavel() const
{
    return this->sealevel;
}

#endif 