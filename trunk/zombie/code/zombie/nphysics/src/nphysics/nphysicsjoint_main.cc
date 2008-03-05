//-----------------------------------------------------------------------------
//  nphysicsjoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#include "nphysics/nphysicsjoint.h"
#include "nphysics/nphyjointgroup.h"

//-----------------------------------------------------------------------------

nNebulaScriptClass(nPhysicsJoint, "nobject");

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 25-Sep-2004   David Reyes    created
*/
nPhysicsJoint::nPhysicsJoint() :
    type(Invalid),
    jointID(NoValidID),
    breakability(0),
    world(0),
    groupJoint(0),
    rbodyA(0),
    rbodyB(0)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 25-Sep-2004   David Reyes    created
*/
nPhysicsJoint::~nPhysicsJoint()
{
    if( this->Id() != NoValidID )
    {
        this->groupJoint->Remove(this);
        this->SetBreakable( false );
        phyDestroyJoint( this->Id() );
        this->jointID = NoValidID;
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the joint type.

    @return returns the joins type

    history:
        - 25-Sep-2004   David Reyes    created
*/
nPhysicsJoint::JointTypes nPhysicsJoint::Type() const
{
    return type;
}

//-----------------------------------------------------------------------------
/**
    Attaches the joint type.

    @param  bodyA   a body to be attached
    @param  bodyB   a body to be attached

    history:
        - 25-Sep-2004   David Reyes    created
*/
void nPhysicsJoint::Attach( nPhyRigidBody* bodyA, nPhyRigidBody* bodyB )
{
    n_assert2( this->Id() != NoValidID, "No valid joint" );

    bodyid rbodyA(0);
    bodyid rbodyB(0);

    if( bodyA ) 
    {
        rbodyA = bodyA->Id();
    }

    if( bodyB ) 
    {
        rbodyB = bodyB->Id();
    }

    this->rbodyA = bodyA;
    this->rbodyB = bodyB;

    phyJointAttach( this->Id(), rbodyA, rbodyB );
}

//-----------------------------------------------------------------------------
/**
    Attaches the joint type.

    @param  bodyA   a body to be attached
    @param  bodyB   a body to be attached

    history:
        - 25-Sep-2004   David Reyes    created
*/
void nPhysicsJoint::Attach( bodyid bodyA, bodyid bodyB )
{
    n_assert2( this->Id() != NoValidID, "No valid joint" );

    phyJointAttach( this->Id(), bodyA, bodyB );
}


//-----------------------------------------------------------------------------
/**
    Returns if it's a breakable joint.

    @return a bool value setting if it's a breakable join

    history:
        - 25-Sep-2004   David Reyes    created
*/
bool nPhysicsJoint::IsBreakable() const
{
    return this->breakability ? true : false;
}

//-----------------------------------------------------------------------------
/**
    Sets if the join it's breakable.

    @param is   sets if the join it's breakable

    history:
        - 25-Sep-2004   David Reyes    created
        - 10-Nov-2004   David Reyes    added functionality for breakability
*/
void nPhysicsJoint::SetBreakable( bool is )
{
    n_assert2( this->Id() != NoValidID, "No valid joint" );  
    
    if( is )
    {
        if( !this->breakability )
        {
            this->breakability = n_new( Breakability( this->Id() ) );

            n_assert2( this->breakability, "Failed to allocate memory" );

            // registering the joint as breakable
            this->GetWorld()->Add( this );
        }

    }
    else
    {
        if( this->breakability )
        {
            n_delete( this->breakability );

            this->breakability = 0;
    
            // unregistering the joint as breakable
            this->GetWorld()->Remove( this );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the join.

    @param world world where the joint will exists
    @param group joints group

    history:
        - 25-Sep-2004   David Reyes    created
*/
void nPhysicsJoint::CreateIn( nPhysicsWorld* world, nPhyJointGroup* group )
{
    n_assert2( world, "Null pointer" );

    this->world = world;
    
    if( group )
    {
        this->groupJoint = group;
    }
    else
    {
        this->groupJoint = world->GetGeneralJointGroup();
    }
}

//-----------------------------------------------------------------------------
/**
    Returns body A Id (no valid if it's attached to the environment).

    @return body id

    history:
        - 10-Nov-2004   David Reyes    created
*/
nPhyRigidBody* nPhysicsJoint::GetBodyA() const
{
    n_assert2( this->Id() != NoValidID, "No valid joint" );  

    return this->rbodyA;
}

//-----------------------------------------------------------------------------
/**
    Returns body B Id (no valid if it's attached to the environment).

    @return body id

    history:
        - 10-Nov-2004   David Reyes    created
*/
nPhyRigidBody* nPhysicsJoint::GetBodyB() const
{
    n_assert2( this->Id() != NoValidID, "No valid joint" );  

    return this->rbodyB;
}

//-----------------------------------------------------------------------------
/**
    Sets the force limit of a breakable joint.

    @param  max force

    history:
        - 10-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::SetMaxForceBodyA( const phyreal force )
{
    n_assert2( this->Id() != NoValidID, "No valid joint" );  
    
    this->SetBreakable( true );

    this->breakability->SetMaxForceBodyA( force ); 
}

//-----------------------------------------------------------------------------
/**
    Sets the force limit of a breakable joint.

    @param  max force

    history:
        - 10-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::SetMaxForceBodyB( const phyreal force )
{
    n_assert2( this->Id() != NoValidID, "No valid joint" );  
    
    this->SetBreakable( true );

    this->breakability->SetMaxForceBodyB( force ); 
}
    
//-----------------------------------------------------------------------------
/**
    Set the torque limit of a breakable joint.

    @param  max torque

    history:
        - 10-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::SetMaxTorqueBodyA( const phyreal torque )
{
    n_assert2( this->Id() != NoValidID, "No valid joint" );  

    this->SetBreakable( true );

    this->breakability->SetMaxTorqueBodyA( torque ); 
}

//-----------------------------------------------------------------------------
/**
    Set the torque limit of a breakable joint.

    @param  max torque

    history:
        - 10-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::SetMaxTorqueBodyB( const phyreal torque )
{
    n_assert2( this->Id() != NoValidID, "No valid joint" );  

    this->SetBreakable( true );

    this->breakability->SetMaxTorqueBodyB( torque ); 
}

//-----------------------------------------------------------------------------
/**
    Returns the world id of this joint.

    @return a world

    history:
        - 10-Nov-2004   David Reyes    created
*/
nPhysicsWorld* nPhysicsJoint::GetWorld() const
{
    return this->world;
}


//-----------------------------------------------------------------------------
/**
    Breaks the joint inconditionally.

    history:
        - 28-Feb-2005   David Reyes    created
*/
void nPhysicsJoint::ForceBreak()
{
    this->Attach( static_cast<nPhyRigidBody*>(0), 0 );
}

//-----------------------------------------------------------------------------
/**
    Checks if the joint has to be broken, and break it.

    @return if a has been a break

    history:
        - 10-Nov-2004   David Reyes    created
*/
bool nPhysicsJoint::Break()
{
    n_assert2( this->IsBreakable(), "Flow corruption" );

    if( this->breakability->IsBreakable() )
    {
        // It has to be broken (detaching from the bodies)
        this->ForceBreak();

        // TODO: fix the problem of releasing

        //this->Release();

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Sets a parameters of the joints

    history:
        - 12-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::SetParam( const phy::jointparameter, const phy::jointaxis, const phyreal )
{
    n_assert2_always(  "Shouldn't be at this point" );
}

//-----------------------------------------------------------------------------
/**
    Adds torque to the motor joint

    history:
        - 12-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::AddTorque( const phyreal torque, const phy::jointaxis axis )
{
    nPhyRigidBody* body( phy::axisA ? this->GetBodyA() : this->GetBodyB() );

    n_assert2( body, "Invalid body" );

    vector3 direction;

    this->GetAxisDirection( direction, axis );

    body->SetTorque( direction * torque );
}

//-----------------------------------------------------------------------------
/**
    Adds force to the motor joint

    history:
        - 12-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::AddForce( const phyreal force, const phy::jointaxis axis )
{
    nPhyRigidBody* body( phy::axisA ? this->GetBodyA() : this->GetBodyB() );

    n_assert2( body, "Invalid body" );

    vector3 direction;

    this->GetAxisDirection( direction, axis );

    body->SetForce( direction * force );
}

/**
    Gets the axis direction

    history:
        - 12-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::GetAxisDirection( vector3&, const phy::jointaxis )
{
    n_assert2_always(  "It shouldn't be called" );
}

/**
    Attaches two physics objs

    history:
        - 21-Dec-2004   David Reyes    created
*/
void nPhysicsJoint::Attach( ncPhysicsObj* objectA, ncPhysicsObj* objectB )
{
    n_assert2( objectA || objectB, "Null pointer" );

    nPhyRigidBody* bodyA(0);
    nPhyRigidBody* bodyB(0);

    if( objectA )
    {
        n_assert2( objectA->GetBody(), "A object to be attached has to have a body.");
        bodyA = objectA->GetBody();
    }

    if( objectB )
    {
        n_assert2( objectB->GetBody(), "A object to be attached has to have a body.");
        bodyB = objectB->GetBody();
    }

    this->Attach( bodyA, bodyB );
}

//-----------------------------------------------------------------------------
/**
    Returns breakability info.

    @return breakability info if any

    history:
        - 30-Nov-2004   David Reyes    created
*/
nPhysicsJoint::Breakability* nPhysicsJoint::GetBreakabilityInfo() const
{
    return this->breakability;
}

//------------------------------------------------------------------------------
/**
	Scales the object.
    
    history:
     - 12-May-2005   David Reyes    created
*/
void nPhysicsJoint::Scale( const phyreal /*factor*/ )
{
    // empty
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Draws the joint.
    
    history:
     - 18-Aug-2005   David Reyes    created
*/
void nPhysicsJoint::Draw( nGfxServer2* /*server*/ )
{
    // Empty
}

#endif


//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 10-Nov-2004   David Reyes    created
*/
nPhysicsJoint::Breakability::Breakability( const jointid id  ) :
    jointData(0),
    jointID( id ),
    maxForceBodyA(phyInfinity),
    maxForceBodyB(phyInfinity),
    maxTorqueBodyA(phyInfinity),
    maxTorqueBodyB(phyInfinity)
{
    n_assert2( this->jointID != NoValidID, "Invalid joint id" );

    this->jointData = n_new( jointdata );

    n_assert2( this->jointData, "Failled to allocate memory" );

    phyJointExtraData( this->jointID, this->jointData );
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 10-Nov-2004   David Reyes    created
*/
nPhysicsJoint::Breakability::~Breakability()
{
    n_assert2( this->jointID != NoValidID, "Invalid joint id" );

    n_delete( this->jointData );

    this->jointData = 0;

    phyJointExtraData( this->jointID, this->jointData );

    this->jointID = NoValidID;
}

//-----------------------------------------------------------------------------
/**
    Returns if the conditions are for break

    @return true if it's breakable

    history:
        - 10-Nov-2004   David Reyes    created
*/
bool nPhysicsJoint::Breakability::IsBreakable() const
{
    n_assert2( this->jointData, "Data corruption" );

    if( phyGetForce( this->jointData, 0 ) > maxForceBodyA )
        return true;
    if( phyGetForce( this->jointData, 1 ) > maxForceBodyB )
        return true;

    if( phyGetTorque( this->jointData, 0 ) > maxTorqueBodyA )
        return true;
    if( phyGetTorque( this->jointData, 1 ) > maxTorqueBodyB )
        return true;

    return false;
}

//-----------------------------------------------------------------------------
/**
    Sets the force limit of a breakable joint.

    @param max force

    history:
        - 10-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::Breakability::SetMaxForceBodyA( const phyreal force )
{
    this->maxForceBodyA = force;
}

//-----------------------------------------------------------------------------
/**
    Sets the force limit of a breakable joint.

    @param max force

    history:
        - 10-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::Breakability::SetMaxForceBodyB( const phyreal force )
{
    this->maxForceBodyB = force;
}
    
//-----------------------------------------------------------------------------
/**
    Sets the torque limit of a breakable joint.

    @param max torque

    history:
        - 10-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::Breakability::SetMaxTorqueBodyA( const phyreal torque )
{
    this->maxTorqueBodyA = torque;
}

//-----------------------------------------------------------------------------
/**
    Sets the torque limit of a breakable joint.

    @param max torque

    history:
        - 10-Nov-2004   David Reyes    created
*/
void nPhysicsJoint::Breakability::SetMaxTorqueBodyB( const phyreal torque )
{
    this->maxTorqueBodyB = torque;
}

//-----------------------------------------------------------------------------
/**
    Returns a parameter value.

    @param parameter which one
    @param axis which axis

    history:
        - 25-Aug-2005   David Reyes    created
*/
const phyreal nPhysicsJoint::GetParam( const phy::jointparameter /*parameter*/, const phy::jointaxis /*axis*/ ) const
{
    return 0;
}

//-----------------------------------------------------------------------------
/**
    Resets the joint.

    history:
        - 30-Jan-2006   David Reyes    created
*/
void nPhysicsJoint::Reset()
{
    nPhyRigidBody* bodyA(this->GetBodyA()), *bodyB(this->GetBodyB());
    this->ForceBreak();   
    this->Attach( bodyA, bodyB );
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
 