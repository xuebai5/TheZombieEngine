//-----------------------------------------------------------------------------
//  nphysliderjoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphysliderjoint.h"
#include "nphysics/nphyjointgroup.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhySliderJoint, "nphysicsjoint");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 15-Nov-2004   Zombie         created
*/
nPhySliderJoint::nPhySliderJoint()
{
    this->type = Slider;    
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 15-Nov-2004   Zombie         created
*/
nPhySliderJoint::~nPhySliderJoint()
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
void nPhySliderJoint::CreateIn( nPhysicsWorld* world, nPhyJointGroup* group )
{
    n_assert2( world, "Null pointer" );
    n_assert2( group, "Null pointer" );
    n_assert2( this->Id() == NoValidID, "It's already created" );

    group->Add( this );

    this->jointID = phyCreateSliderJoint( world->Id(), group->Id() );

    n_assert2( this->Id() != NoValidID, "Failed to create a contact joint" );

    nPhysicsJoint::CreateIn( world, group );
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Draws the joint.

    history:
     - 18-Aug-2005   Zombie         created
*/
void nPhySliderJoint::Draw( nGfxServer2* server )
{
    nPhysicsJoint::Draw( server );

    vector3 line[2];

    nPhyRigidBody* bodyA(this->GetBodyA());
    nPhyRigidBody* bodyB(this->GetBodyB());

    bodyA->GetPosition( line[0] );
    bodyB->GetPosition( line[1] );

    server->BeginLines();

    server->DrawLines3d( line, 2, phy_color_joint_axisA );

    server->EndLines();
}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
