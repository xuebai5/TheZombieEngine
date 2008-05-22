#ifndef N_PHYANGULARMOTORJOINT_H
#define N_PHYANGULARMOTORJOINT_H

//-----------------------------------------------------------------------------
/**
    @class nPhyAngularMotorJoint
    @ingroup NebulaPhysicsSystem
    @brief Representation of a Angular Motor joint

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Angular Motor
    
    @cppclass nPhyAngularMotorJoint
    
    @superclass nPhysicsJoint

    @classinfo Representation of a Angular Motor joint.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsjoint.h"

//-----------------------------------------------------------------------------

class nPhysicsWorld;
class nPhyJointGroup;

//-----------------------------------------------------------------------------

class nPhyAngularMotorJoint : public nPhysicsJoint
{
public:
    /// constructor
    nPhyAngularMotorJoint();

    /// destructor
    ~nPhyAngularMotorJoint();

    /// creates the join
    void CreateIn( nPhysicsWorld* world, nPhyJointGroup* group );

    /// sets the first axis
    void SetFirstAxis( const vector3& axis );

    /// sets the second axis
    void SetSecondAxis( const vector3& axis );

    /// sets the second axis
    void SetThirdAxis( const vector3& axis );

    /// returns the universal joint first axis
    void GetFirstAxis( vector3& axis ) const;

    /// returns the universal joint second axis
    void GetSecondAxis( vector3& axis ) const;

    /// returns the universal joint second axis
    void GetThirdAxis( vector3& axis ) const;

    /// sets a parameters of the joint
    void SetParam( const phy::jointparameter, const phy::jointaxis, const phyreal );

    /// sets the number of axis controled by the motor
    void SetNumberOfAxis( const int numAxis );

#ifndef NGAME
    /// draws the joint
    void Draw( nGfxServer2* server );
#endif
};

//-----------------------------------------------------------------------------
/**
    Sets a parameters of the joint.

    @param parameter a join's parameter
    @param axis joint axis
    @param value new parameter's value

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyAngularMotorJoint::SetParam( const phy::jointparameter parameter, const phy::jointaxis axis, const phyreal value )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phyAngularMotorSetParameter( this->Id(), parameter, axis, value );    
}

//-----------------------------------------------------------------------------
/**
    Sets the first axis.

    @param axis axis direction

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyAngularMotorJoint::SetFirstAxis( const vector3& axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetAngularMotorAxis( this->Id(), phy::axisA, axis );

    dJointSetAMotorAngle( this->Id(), phy::axisA, 0 );
}

//-----------------------------------------------------------------------------
/**
    Sets the second axis.

    @param axis axis direction

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyAngularMotorJoint::SetSecondAxis( const vector3& axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetAngularMotorAxis( this->Id(), phy::axisB, axis );
}

//-----------------------------------------------------------------------------
/**
    Sets the second axis.

    @param axis axis direction

    history:
        - 09-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyAngularMotorJoint::SetThirdAxis( const vector3& axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetAngularMotorAxis( this->Id(), phy::axisC, axis );
}

#endif 