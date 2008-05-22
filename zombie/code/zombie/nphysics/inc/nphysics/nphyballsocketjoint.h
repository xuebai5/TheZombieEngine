#ifndef N_PHYBALLSOCKETJOINT_H
#define N_PHYBALLSOCKETJOINT_H

//-----------------------------------------------------------------------------
/**
    @class nPhyBallSocketJoint
    @ingroup NebulaPhysicsSystem
    @brief Representation of ball and socket joint

    (C) 2004 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
/**
    @scriptclass Physics Socket Joint
    
    @cppclass nPhyBallSocketJoint
    
    @superclass nPhysicsJoint

    @classinfo Representation of ball and socket joint.
*/    

//-----------------------------------------------------------------------------

#include "nphysics/nphysicsjoint.h"

//-----------------------------------------------------------------------------

class nPhysicsWorld;
class nPhyJointGroup;

//-----------------------------------------------------------------------------

class nPhyBallSocketJoint : public nPhysicsJoint
{
public:
    /// constructor
    nPhyBallSocketJoint();

    /// destructor
    virtual ~nPhyBallSocketJoint();

    /// creates the join
    virtual void CreateIn( nPhysicsWorld* world, nPhyJointGroup* group );

    /// sets the ball anchor
    void SetAnchor( const vector3& anchor );

    /// returns the anchor
    void GetAnchor( vector3& anchor ) const;

    /// scales the object
    void Scale( const phyreal factor );

#ifndef NGAME
    /// draws the joint
    void Draw( nGfxServer2* server );
#endif
};

//-----------------------------------------------------------------------------
/**
    Sets the ball anchor.

    @param anchor ball anchor

    history:
        - 15-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyBallSocketJoint::SetAnchor( const vector3& anchor )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phySetBallSocketAnchor( this->Id(), anchor );
}

//-----------------------------------------------------------------------------
/**
    Returns the anchor.

    @param anchor ball anchor

    history:
        - 16-Nov-2004   Zombie         created
        - 12-May-2005   Zombie         inlined
*/
inline
void nPhyBallSocketJoint::GetAnchor( vector3& anchor ) const
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    phyGetBallSocketAnchor( this->Id(), anchor );
}

#endif 