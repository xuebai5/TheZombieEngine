#ifndef N_PHYSLIDERJOINT_H
#define N_PHYSLIDERJOINT_H

//-----------------------------------------------------------------------------
/**
    @class nPhySliderJoint
    @ingroup NebulaPhysicsSystem
    @brief Representation of a slider joint

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Slider Joint
    
    @cppclass nPhySliderJoint
    
    @superclass nPhysicsJoint

    @classinfo Representation of a slider joint.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsjoint.h"

//-----------------------------------------------------------------------------

class nPhysicsWorld;
class nPhyJointGroup;

//-----------------------------------------------------------------------------

class nPhySliderJoint : public nPhysicsJoint
{
public:
    /// constructor
    nPhySliderJoint();

    /// destructor
    ~nPhySliderJoint();

    /// creates the join
    void CreateIn( nPhysicsWorld* world, nPhyJointGroup* group );

    /// sets the slider axis
    void SetAxis( const vector3& axis );

    /// returns the slider axis
    void GetAxis( vector3& axis ) const;

    /// sets a parameters of the joint
    void SetParam( const phy::jointparameter, const phy::jointaxis, const phyreal );

#ifndef NGAME
    /// draws the joint
    void Draw( nGfxServer2* server );
#endif
};

//-----------------------------------------------------------------------------
/**
    Sets the slider axis.

    @param axis     slider axis

    history:
        - 15-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhySliderJoint::SetAxis( const vector3& axis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetSliderAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Returns the slider axis.

    @param axis     slider axis

    history:
        - 16-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhySliderJoint::GetAxis( vector3& axis ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phyGetSliderAxis( this->Id(), axis );
}

//-----------------------------------------------------------------------------
/**
    Sets a parameters of the joint.

    @param parameter a join's parameter
    @param axis joint axis
    @param value new parameter's value

    history:
        - 16-Nov-2004   David Reyes    created
        - 12-May-2005   David Reyes    inlined
*/
inline
void nPhySliderJoint::SetParam( const phy::jointparameter parameter, const phy::jointaxis axis, const phyreal value )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );
    
    phySliderSetParameter( this->Id(), parameter, axis, value );    
}

#endif 