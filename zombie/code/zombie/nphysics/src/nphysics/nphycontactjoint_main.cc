//-----------------------------------------------------------------------------
//  nphycontactjoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"

#include "nphysics/nphycontactjoint.h"
#include "nphysics/nphyjointgroup.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhyContactJoint, "nphysicsjoint");

//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 18-Oct-2004   Zombie         created
*/
nPhyContactJoint::nPhyContactJoint()
{   
    this->type = Contact;
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 18-Oct-2004   Zombie         created
*/
nPhyContactJoint::~nPhyContactJoint()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Creates the join.

    @param world world where the joint will exists
    @param group joints group

    history:
        - 25-Oct-2004   Zombie         created
*/
void nPhyContactJoint::CreateIn( nPhysicsWorld* world, nPhyJointGroup* group )
{
    n_assert2( world, "Null pointer" );
    n_assert2( group, "Null pointer" );

    group->Add( this );

    this->jointID = phyCreateContactJoint( world->Id(), group->Id(), this->data );

    n_assert2( this->Id() != NoValidID, "Failed to create a contact joint" );

    nPhysicsJoint::CreateIn( world, group );
}



//-----------------------------------------------------------------------------
/**
    Sets a parameters of the joints.

    @param parameter a join's parameter
    @param axis joint axis
    @param value new parameter's value   

    history:
        - 12-Nov-2004   Zombie         created
*/
void nPhyContactJoint::SetParam( const phy::jointparameter, const phy::jointaxis, const phyreal )
{
    // Empty
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
