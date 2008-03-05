#ifndef N_PHYTWOHINGEJOINT_H
#define N_PHYTWOHINGEJOINT_H

//-----------------------------------------------------------------------------
/**
    @class nPhyTwoHingeJoint
    @ingroup NebulaPhysicsSystem
    @brief Representation of two joints in serie

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Two Joints in serie.
    
    @cppclass nPhyTwoHingeJoint
    
    @superclass nPhysicsJoint

    @classinfo Representation of two joints in serie.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsjoint.h"

//-----------------------------------------------------------------------------

class nPhysicsWorld;
class nPhyJointGroup;

//-----------------------------------------------------------------------------

class nPhyTwoHingeJoint : public nPhysicsJoint
{
public:
    /// constructor
    nPhyTwoHingeJoint();

    /// destructor
    ~nPhyTwoHingeJoint();

    /// creates the join
    void CreateIn( nPhysicsWorld* world, nPhyJointGroup* group );

    /// sets the hinge anchor
    void SetAnchor( const vector3& anchor );

    /// sets the first axis
    void SetFirstAxis( const vector3& axis );

    /// sets the second axis
    void SetSecondAxis( const vector3& axis );

    /// sets a parameters of the joints
    void SetParam( const phy::jointparameter parameter, const phy::jointaxis axis, const phyreal value );

    /// gets the axis direction
    void GetAxisDirection( vector3& direction, const phy::jointaxis axis = phy::axisA );

    /// returns the first axis
    void GetFirstAxis( vector3& axis ) const;

    /// returns the second axis
    void GetSecondAxis( vector3& axis ) const;

    /// returns the anchor
    void GetFirstAnchor( vector3& anchor ) const;

    /// returns the anchor
    void GetSecondAnchor( vector3& anchor ) const;

    /// scales the object
    void Scale( const phyreal factor );

#ifndef NGAME
    /// draws the joint
    void Draw( nGfxServer2* server );
#endif
};

//-----------------------------------------------------------------------------
/**
    Sets the hinge anchor.

    @param anchor   anchor point

    history:
        - 03-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyTwoHingeJoint::SetAnchor( const vector3& anchor )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetTwoHingeAnchor( this->Id(), anchor );
}

//-----------------------------------------------------------------------------
/**
    Sets the first axis.

    @param axis   first direction axis

    history:
        - 03-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyTwoHingeJoint::SetFirstAxis( const vector3& axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetTwoHingeFirstAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Sets the second axis.

    @param axis   second direction axis

    history:
        - 03-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyTwoHingeJoint::SetSecondAxis( const vector3& axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phySetTwoHingeSecondAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Sets a parameters of the joint.

    @param parameter    affected parameter
    @param axis         wich axis affects if any
    @param value        new parameters value

    history:
        - 11-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyTwoHingeJoint::SetParam( const phy::jointparameter parameter, const phy::jointaxis axis, const phyreal value )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phyHinge2SetParameter( this->Id(), parameter, axis, value );
}

//-----------------------------------------------------------------------------
/**
    Gets the axis direction.

    @param direction    vector with the axis direction
    @param axis         wich axis

    history:
        - 11-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyTwoHingeJoint::GetAxisDirection( vector3& direction, const phy::jointaxis axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phyGetHinge2Axis( this->Id(), direction, axis );
}

//-----------------------------------------------------------------------------
/**
    Returns the first axis.

    @param axis         two hinge axis

    history:
        - 16-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyTwoHingeJoint::GetFirstAxis( vector3& axis ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phyGetTwoHingeFirstAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Returns the second axis.

    @param axis         two hinge axis

    history:
        - 16-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyTwoHingeJoint::GetSecondAxis( vector3& axis ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phyGetTwoHingeSecondAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Returns the anchor.

    @param anchor         two hinge anchor

    history:
        - 16-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyTwoHingeJoint::GetFirstAnchor( vector3& anchor ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phyGetTwoHingeAnchor( this->Id(), anchor );
}

//-----------------------------------------------------------------------------
/**
    Returns the anchor.

    @param anchor         two hinge anchor

    history:
        - 20-Apr-2005   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhyTwoHingeJoint::GetSecondAnchor( vector3& anchor ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phyGetTwoHingeAnchor2( this->Id(), anchor );
}

#endif 