//-----------------------------------------------------------------------------
//  nphyuniversaljoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#include "nphysics/nphyuniversaljoint.h"
#include "nphysics/nphyjointgroup.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyUniversalJoint, "nphysicsjoint");

//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 03-Nov-2004   Zombie         created
*/
nPhyUniversalJoint::nPhyUniversalJoint()
{   
    this->type = Universal;
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 03-Nov-2004   Zombie         created
*/
nPhyUniversalJoint::~nPhyUniversalJoint()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the join.

    @param world world where the joint will exists
    @param group joints group

    history:
        - 03-Nov-2004   Zombie         created
*/
void nPhyUniversalJoint::CreateIn( nPhysicsWorld* world, nPhyJointGroup* group )
{
    nPhysicsJoint::CreateIn( world, group );
 
    n_assert2( this->Id() == NoValidID, "It's already created" );

    this->groupJoint->Add( this );

    this->jointID = phyCreateUniversalJoint( world->Id(), this->groupJoint->Id() );

    n_assert2( this->Id() != NoValidID, "Failed to create a contact joint" );

}

//------------------------------------------------------------------------------
/**
	Scales the object.

    @param factor scale factor
    
    history:
     - 12-May-2005   Zombie         created
*/
void nPhyUniversalJoint::Scale( const phyreal factor )
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
void nPhyUniversalJoint::Draw( nGfxServer2* server )
{
    nPhysicsJoint::Draw( server );

    // Drawing anchors
    matrix44 model;

    vector3 position;

    model.scale( vector3( phy_radius_joint_anchor, phy_radius_joint_anchor, phy_radius_joint_anchor ) );

    this->GetAnchor( position );

    model.translate( position );

    server->BeginShapes();

    server->DrawShape( nGfxServer2::Sphere, model, phy_color_joint_anchorA );

    server->EndShapes();

    vector3 line[2];

    this->GetAnchor( line[0] );

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
