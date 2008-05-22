#ifndef N_PHYUNIVERSALJOINT_H
#define N_PHYUNIVERSALJOINT_H

//-----------------------------------------------------------------------------
/**
    @class nPhyUniversalJoint
    @ingroup NebulaPhysicsSystem
    @brief Representation of a universal joint

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Universal Joint.
    
    @cppclass nPhyUniversalJoint
    
    @superclass nPhysicsJoint

    @classinfo Representation of a universal joint.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsjoint.h"

//-----------------------------------------------------------------------------

class nPhysicsWorld;
class nPhyJointGroup;

//-----------------------------------------------------------------------------

class nPhyUniversalJoint : public nPhysicsJoint
{
public:
    /// constructor
    nPhyUniversalJoint();

    /// destructor
    ~nPhyUniversalJoint();

    /// creates the join
    void CreateIn( nPhysicsWorld* world, nPhyJointGroup* group );

    /// sets the universal joint anchor
    void SetAnchor( const vector3& anchor );

    /// sets the first axis
    void SetFirstAxis( const vector3& axis );

    /// sets the second axis
    void SetSecondAxis( const vector3& axis );

    /// returns the universal joint anchor
    void GetAnchor( vector3& anchor ) const;

    /// returns the universal joint first axis
    void GetFirstAxis( vector3& axis ) const;

    /// returns the universal joint second axis
    void GetSecondAxis( vector3& axis ) const;

    /// returns the angle for the first axis
    phyreal GetAngleFirstAxis() const;

    /// returns the angle for the second axis
    phyreal GetAngleSecondAxis() const;

    /// sets a parameters of the joint
    void SetParam( const phy::jointparameter, const phy::jointaxis, const phyreal );

    /// scales the object
    void Scale( const phyreal factor );

#ifndef NGAME
    /// draws the joint
    void Draw( nGfxServer2* server );
#endif
};

//-----------------------------------------------------------------------------
/**
    Sets the universal joint anchor.

    @param anchor   anchor point

    history:
        - 03-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyUniversalJoint::SetAnchor( const vector3& anchor )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetUniversalAnchor( this->Id(), anchor );
}

//-----------------------------------------------------------------------------
/**
    Sets the first axis.

    @param axis   first direction axis

    history:
        - 03-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyUniversalJoint::SetFirstAxis( const vector3& axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetUniversalFirstAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Sets the second axis.

    @param axis   second direction axis

    history:
        - 03-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyUniversalJoint::SetSecondAxis( const vector3& axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phySetUniversalSecondAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Returns the universal joint anchor.

    @param anchor   hoint anchor

    history:
        - 16-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyUniversalJoint::GetAnchor( vector3& anchor ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phyGetUniversalAnchor( this->Id(), anchor );
}

//-----------------------------------------------------------------------------
/**
    Returns the universal joint first axis.

    @param axis   hoint axis

    history:
        - 16-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyUniversalJoint::GetFirstAxis( vector3& axis ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phyGetUniversalFirstAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Returns the universal joint second axis.

    @param axis   hoint axis

    history:
        - 16-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyUniversalJoint::GetSecondAxis( vector3& axis ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phyGetUniversalSecondAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Sets a parameters of the joint

    @param parameter a join's parameter
    @param axis joint axis
    @param value new parameter's value

    history:
        - 07-Mar-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyUniversalJoint::SetParam( const phy::jointparameter parameter, const phy::jointaxis axis, const phyreal value )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phyUniversalSetParameter( this->Id(), parameter, axis, value );    
}

//-----------------------------------------------------------------------------
/**
    Returns the angle for the first axis.

    @return angle

    history:
        - 06-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
phyreal nPhyUniversalJoint::GetAngleFirstAxis() const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
 
    return phyGetUniversalAngleA( this->Id() );
}

//-----------------------------------------------------------------------------
/**
    Returns the angle for the second axis.

    @return angle

    history:
        - 06-Apr-2005   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
phyreal nPhyUniversalJoint::GetAngleSecondAxis() const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    return phyGetUniversalAngleB( this->Id() );
}

#endif 