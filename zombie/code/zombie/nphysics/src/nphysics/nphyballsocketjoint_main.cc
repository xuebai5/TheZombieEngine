//-----------------------------------------------------------------------------
//  nphyballsocketjoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyballsocketjoint.h"
#include "nphysics/nphyjointgroup.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyBallSocketJoint, "nphysicsjoint");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 15-Nov-2004   Zombie         created
*/
nPhyBallSocketJoint::nPhyBallSocketJoint()
{
    this->type = BallSocket;    
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 15-Nov-2004   Zombie         created
*/
nPhyBallSocketJoint::~nPhyBallSocketJoint()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the join

    @param world world where the joint will exists
    @param group joints group

    history:
        - 15-Nov-2004   Zombie         created
*/
void nPhyBallSocketJoint::CreateIn( nPhysicsWorld* world, nPhyJointGroup* group )
{
    nPhysicsJoint::CreateIn( world, group );

    n_assert2( this->Id() == NoValidID, "It's already created" );

    this->groupJoint->Add( this );

    this->jointID = phyCreateBallSocketJoint( world->Id(), this->groupJoint->Id() );

    n_assert2( this->Id() != NoValidID, "Failed to create a contact joint" );
}


//------------------------------------------------------------------------------
/**
	Scales the object.

    @param factor scale factor
    
    history:
     - 12-May-2005   Zombie         created
*/
void nPhyBallSocketJoint::Scale( const phyreal factor )
{
    n_assert2( factor != 0, "Can't scale with a factor of 0" );

    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    vector3 anchor;

    this->GetAnchor( anchor );

    anchor *= factor;

    this->SetAnchor( anchor );
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Draws the joint.

    history:
     - 18-Aug-2005   Zombie         created
*/
void nPhyBallSocketJoint::Draw( nGfxServer2* server )
{
    nPhysicsJoint::Draw( server );

    // Drawing anchors
    matrix44 model;

    vector3 position;

    model.scale( vector3( phy_radius_joint_anchor, phy_radius_joint_anchor, phy_radius_joint_anchor ) );

    this->GetAnchor( position );

    model.translate( position );

    vector4 colour(phy_color_joint_anchorA);

    if( !(this->GetBodyA() && this->GetBodyB()) )
    {
        colour  = phy_color_joint_anchorWall;
    }

    server->BeginShapes();

    server->DrawShape( nGfxServer2::Sphere, model, colour );

    server->EndShapes();
}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
