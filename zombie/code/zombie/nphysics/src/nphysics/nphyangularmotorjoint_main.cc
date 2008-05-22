//-----------------------------------------------------------------------------
//  nphyuniversaljoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/nphyangularmotorjoint.h"
#include "nphysics/nphyjointgroup.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyAngularMotorJoint, "nphysicsjoint");

//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 09-Mar-2004   Zombie         created
*/
nPhyAngularMotorJoint::nPhyAngularMotorJoint()
{   
    this->type = AngularMotor;
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 09-Mar-2004   Zombie         created
*/
nPhyAngularMotorJoint::~nPhyAngularMotorJoint()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the join.

    @param world world where the joint will exists
    @param group joints group

    history:
        - 09-Mar-2004   Zombie         created
*/
void nPhyAngularMotorJoint::CreateIn( nPhysicsWorld* world, nPhyJointGroup* group )
{
    nPhysicsJoint::CreateIn( world, group );
 
    n_assert2( this->Id() == NoValidID, "It's already created" );

    this->groupJoint->Add( this );

    this->jointID = phyCreateAngularMotorJoint( world->Id(), this->groupJoint->Id() );

    n_assert2( this->Id() != NoValidID, "Failed to create a contact joint" );
}



//-----------------------------------------------------------------------------
/**
    Returns the universal joint first axis.

    @param axis axis direction

    history:
        - 09-Mar-2005   Zombie         created
*/
void nPhyAngularMotorJoint::GetFirstAxis( vector3& /*axis*/ ) const
{
}

//-----------------------------------------------------------------------------
/**
    Returns the universal joint second axis.

    @param axis axis direction

    history:
        - 09-Mar-2005   Zombie         created
*/
void nPhyAngularMotorJoint::GetSecondAxis( vector3& /*axis*/ ) const
{
}

//-----------------------------------------------------------------------------
/**
    Returns the universal joint second axis.

    @param axis axis direction

    history:
        - 09-Mar-2005   Zombie         created
*/
void nPhyAngularMotorJoint::GetThirdAxis( vector3& /*axis*/ ) const
{
}

//-----------------------------------------------------------------------------
/**
    Sets the number of axis controled by the motor.

    @param numAxis number of axis

    history:
        - 09-Mar-2005   Zombie         created
*/
void nPhyAngularMotorJoint::SetNumberOfAxis( const int numAxis )
{
    n_assert2( this->Id() != NoValidID, "It's a not valid joint" );

    n_assert2( numAxis > 0, "Error number of axis can not be negative." );
    n_assert2( numAxis < 4, "Error the maximun number of axis are 3." );

    phySetNumberOfAxisAngularMotor( this->Id(), numAxis );
}


#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Draws the joint.

    history:
     - 18-Aug-2005   Zombie         created
*/
void nPhyAngularMotorJoint::Draw( nGfxServer2* server )
{
    nPhysicsJoint::Draw( server );
}

#endif

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
