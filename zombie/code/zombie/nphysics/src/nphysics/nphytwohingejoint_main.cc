//-----------------------------------------------------------------------------
//  nphycontactjoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphytwohingejoint.h"
#include "nphysics/nphyjointgroup.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyTwoHingeJoint, "nphysicsjoint");

//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 03-Nov-2004   David Reyes    created
*/
nPhyTwoHingeJoint::nPhyTwoHingeJoint()
{   
    this->type = TwoHinge;
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 03-Nov-2004   David Reyes    created
*/
nPhyTwoHingeJoint::~nPhyTwoHingeJoint()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the join.

    @param world world where the joint will exists
    @param group joints group

    history:
        - 03-Nov-2004   David Reyes    created
*/
void nPhyTwoHingeJoint::CreateIn( nPhysicsWorld* world, nPhyJointGroup* group )
{
    n_assert2( world, "Null pointer" );
    n_assert2( group, "Null pointer" );
    n_assert2( this->Id() == NoValidID, "It's already created" );

    group->Add( this );

    this->jointID = phyCreateTwoHingeJoint( world->Id(), group->Id() );

    n_assert2( this->Id() != NoValidID, "Failed to create a contact joint" );

    nPhysicsJoint::CreateIn( world, group );
}


//------------------------------------------------------------------------------
/**
	Scales the object.

    @param factor scale factor
    
    history:
     - 12-May-2005   David Reyes    created
*/
void nPhyTwoHingeJoint::Scale( const phyreal factor )
{
    vector3 anchor;

    this->GetFirstAnchor( anchor );

    anchor *= factor;

    this->SetAnchor( anchor );
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Draws the joint.

    history:
     - 18-Aug-2005   David Reyes    created
*/
void nPhyTwoHingeJoint::Draw( nGfxServer2* server )
{
    nPhysicsJoint::Draw( server );

    // Drawing anchors
    matrix44 model;

    vector3 position;

    model.scale( vector3( phy_radius_joint_anchor, phy_radius_joint_anchor, phy_radius_joint_anchor ) );

    this->GetFirstAnchor( position );

    model.translate( position );

    server->BeginShapes();

    server->DrawShape( nGfxServer2::Sphere, model, phy_color_joint_anchorA );

    server->EndShapes();

    this->GetSecondAnchor( position );

    model.translate( position );

    server->BeginShapes();

    server->DrawShape( nGfxServer2::Sphere, model, phy_color_joint_anchorB );

    server->EndShapes();

    vector3 line[2];

    this->GetFirstAnchor( line[0] );

    this->GetFirstAxis( line[1] );

    line[1] *= phy_length_joint_axis;

    line[1] += position;

    server->BeginLines();

    server->DrawLines3d( line, 2, phy_color_joint_axisA );

    server->EndLines();
    
    this->GetSecondAxis( line[1] );

    line[1] *= phy_length_joint_axis;

    line[1] += position;

    server->BeginLines();

    server->DrawLines3d( line, 2, phy_color_joint_axisB );

    server->EndLines();
}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------