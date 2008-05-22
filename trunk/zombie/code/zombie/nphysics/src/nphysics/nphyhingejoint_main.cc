//-----------------------------------------------------------------------------
//  nphyhingejoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyhingejoint.h"
#include "nphysics/nphyjointgroup.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyHingeJoint, "nphysicsjoint");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 15-Nov-2004   Zombie         created
*/
nPhyHingeJoint::nPhyHingeJoint()
{
    this->type = Hinge;    
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 15-Nov-2004   Zombie         created
*/
nPhyHingeJoint::~nPhyHingeJoint()
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
void nPhyHingeJoint::CreateIn( nPhysicsWorld* world, nPhyJointGroup* group )
{
    nPhysicsJoint::CreateIn( world, group );

    n_assert2( this->Id() == NoValidID, "It's already created" );

    this->groupJoint->Add( this );

    this->jointID = phyCreateHingeJoint( world->Id(), this->groupJoint->Id() );

    n_assert2( this->Id() != NoValidID, "Failed to create a contact joint" );
}


//------------------------------------------------------------------------------
/**
	Scales the object.

    @param factor scale factor
    
    history:
     - 12-May-2005   Zombie         created
*/
void nPhyHingeJoint::Scale( const phyreal factor )
{
    n_assert2( factor != 0, "Can't scale with a factor of 0" );

    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    vector3 anchor;

    this->GetAnchor( anchor );

    anchor *= factor;

    phySetHingeAnchor( this->Id(), anchor );    
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Draws the joint.

    history:
     - 18-Aug-2005   Zombie         created
*/
void nPhyHingeJoint::Draw( nGfxServer2* server )
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

    this->GetAxis( line[1] );

    line[1] *= phy_length_joint_axis;

    line[1] += position;

    server->BeginLines();

    server->DrawLines3d( line, 2, phy_color_joint_axisA );

    server->EndLines();
}

#endif

//-----------------------------------------------------------------------------
/**
    Returns a parameter value.

    @param parameter which one
    @param axis which axis

    history:
        - 25-Aug-2005   Zombie         created
*/
const phyreal nPhyHingeJoint::GetParam( const phy::jointparameter parameter, const phy::jointaxis axis ) const
{
    return phyHingeGetParameter( this->Id(), parameter, axis );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
