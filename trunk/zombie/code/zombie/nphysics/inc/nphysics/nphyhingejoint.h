#ifndef N_PHYHINGEJOINT_H
#define N_PHYHINGEJOINT_H

//-----------------------------------------------------------------------------
/**
    @class nPhyHingeJoint
    @ingroup NebulaPhysicsSystem
    @brief Representation of a hinge joint

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Hinge Joint
    
    @cppclass nPhyHingeJoint
    
    @superclass nPhysicsJoint

    @classinfo Representation of a hinge joint.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsjoint.h"

//-----------------------------------------------------------------------------

class nPhysicsWorld;
class nPhyJointGroup;

//-----------------------------------------------------------------------------

class nPhyHingeJoint : public nPhysicsJoint
{
public:
    /// constructor
    nPhyHingeJoint();

    /// destructor
    ~nPhyHingeJoint();

    /// creates the join
    void CreateIn( nPhysicsWorld* world, nPhyJointGroup* group );

    /// returns the hinge anchor
    void GetAnchor( vector3& anchor ) const;

    /// returns the hinge axis
    void GetAxis( vector3& axis ) const;

    /// gets the hinge angle (-pi..pi)
    phyreal GetAngle() const;

    /// sets a parameters of the joint
    void SetParam( const phy::jointparameter, const phy::jointaxis, const phyreal );

    /// begin:scripting
    
    /// sets the hinge anchor
    void SetAnchor(const vector3&);
    /// sets the hinge axis
    void SetAxis(const vector3&);

    /// end:scripting

    /// scales the object
    void Scale( const phyreal factor );

    /// returns a parameter value
    const phyreal GetParam( const phy::jointparameter, const phy::jointaxis ) const;

#ifndef NGAME
    /// draws the joint
    void Draw( nGfxServer2* server );
#endif
};

//-----------------------------------------------------------------------------
/**
    Sets the hinge anchor.

    @param anchor hinge anchor

    history:
        - 15-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyHingeJoint::SetAnchor( const vector3& anchor )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetHingeAnchor( this->Id(), anchor );
}

//-----------------------------------------------------------------------------
/**
    Sets the hinge axis.

    @param axis     hinge axis

    history:
        - 15-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyHingeJoint::SetAxis( const vector3& axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetHingeAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Returns the hinge anchor.

    @param anchor     hinge anchor

    history:
        - 16-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyHingeJoint::GetAnchor( vector3& anchor ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phyGetHingeAnchor( this->Id(), anchor );
}

//-----------------------------------------------------------------------------
/**
    Returns the hinge axis.

    @param axis     hinge axis

    history:
        - 16-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyHingeJoint::GetAxis( vector3& axis ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phyGetHingeAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Gets the hinge angle (-pi..pi).

    @return angle between the two bodies

    history:
        - 16-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
phyreal nPhyHingeJoint::GetAngle() const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
 
    return phyGetHingeAngle( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Sets a parameters of the joint

    @param parameter a join's parameter
    @param axis joint axis
    @param value new parameter's value

    history:
        - 21-Dec-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyHingeJoint::SetParam( const phy::jointparameter parameter, const phy::jointaxis axis, const phyreal value )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phyHingeSetParameter( this->Id(), parameter, axis, value );    
}

#endif 