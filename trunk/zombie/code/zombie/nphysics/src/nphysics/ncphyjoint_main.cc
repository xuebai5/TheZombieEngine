//-----------------------------------------------------------------------------
//  ncphyjoint_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"

#include "nphysics/ncphyjoint.h"
#include "nphysics/nphyhingejoint.h"
#include "nphysics/nphyballsocketjoint.h"
#include "nphysics/nphyrigidbody.h"
#include "nphysics/ncphyjointclass.h"

#include "kernel/nlogclass.h"

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    #include "conjurer/nobjecteditorstate.h"
    #include "conjurer/nconjurerapp.h"
#endif
#endif

//-----------------------------------------------------------------------------
#ifndef NGAME
nNebulaComponentObject(ncPhyJoint,ncPhyPickableObj);
#else
nNebulaComponentObject(ncPhyJoint,nComponentObject);
#endif

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 18-Aug-2005   David Reyes    created
*/
ncPhyJoint::ncPhyJoint() :
    type( nPhysicsJoint::Invalid ),
    bodyA(0),
    bodyB(0),
    idA(nEntityObjectId(-1)),
    idB(nEntityObjectId(-1)),
    mounted(false),
    joint(0)

{    
    // empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 18-Aug-2005   David Reyes    created
*/
ncPhyJoint::~ncPhyJoint()
{
    n_delete(this->joint);
}

//-----------------------------------------------------------------------------
/**
    Returns joint type

    @return joint type

    history:
        - 18-Aug-2005   David Reyes    created
*/
nPhysicsJoint::JointTypes ncPhyJoint::GetType() const
{
    return this->type;
}

//-----------------------------------------------------------------------------
/**
    Stores the body A

    @param rigid body

    history:
        - 18-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetBodyA( nPhyRigidBody* body )
{
    this->bodyA = body;
}

//-----------------------------------------------------------------------------
/**
    Stores the body B

    @param rigid body

    history:
        - 18-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetBodyB( nPhyRigidBody* body )
{
    this->bodyB = body;
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 19-Aug-2005   David Reyes    created
*/
void ncPhyJoint::InitInstance(nObject::InitInstanceMsg initType)
{
#ifndef NGAME
    if( initType == nObject::NewInstance )
    {
        ncPhyPickableObj::InitInstance(initType);
    }
#endif
    
    if( initType == nObject::NewInstance )
    {
        this->type = nPhysicsJoint::JointTypes(this->GetClassComponent<ncPhyJointClass>()->GetTypeJoint());
        return;
    }
}

//-----------------------------------------------------------------------------
/**
    Mounts the joint

    history:
        - 18-Aug-2005   David Reyes    created
*/
void ncPhyJoint::Mount()
{
#ifndef NGAME
    if( this->idA == -1 && this->idB == -1 )
    {
        // failed to mount
        NLOG( physicsLog , (1, "# Failed to mount the joint 'cos no bodies set." ) );
        return;
    }

    if( this->GetType() == nPhysicsJoint::Invalid )
    {
        // failed to mount
        NLOG( physicsLog , (1, "# Failed to mount the joint 'cos no joint type set." ) );
        return;
    }

    n_assert2( !this->mounted, "The joint it's already mounted." );

    if( this->mounted )
    {
        return;
    }

    if( !this->GetWorld() )
    {
        ncPhyPickableObj::InitInstance(nObject::LoadedInstance);
    }
#endif

    this->CreateJoint();
}

//-----------------------------------------------------------------------------
/**
    Creates the physics joint.

    history:
        - 18-Aug-2005   David Reyes    created
*/
void ncPhyJoint::CreateJoint()
{
    switch( this->type )
    {
    case nPhysicsJoint::Hinge:
        {        
            nPhyHingeJoint* hjoint(0);

            if( !this->joint )
            {
                hjoint = n_new( nPhyHingeJoint );

                n_assert2( hjoint, "Failed to create a hinge joint." );

                hjoint->CreateIn( nPhysicsServer::Instance()->GetDefaultWorld(),0 );

                this->joint = hjoint;
            }
            else
            {
                hjoint = static_cast<nPhyHingeJoint*>(this->joint);
            }

            nEntityObject* obj(nEntityObjectServer::Instance()->GetEntityObject( this->idA ));

            if( obj )
            {
                ncPhysicsObj* phyObj(obj->GetComponent<ncPhysicsObj>());

                n_assert2( phyObj, "It's requiered to have a physics object component." );

                this->bodyA = phyObj->GetBody();
            }

            obj = nEntityObjectServer::Instance()->GetEntityObject( this->idB );

            if( obj )
            {
                ncPhysicsObj* phyObj(obj->GetComponent<ncPhysicsObj>());

                n_assert2( phyObj, "It's requiered to have a physics object component." );

                this->bodyB = phyObj->GetBody();
            }

            hjoint->Attach( this->bodyA, this->bodyB ); 
#ifndef NGAME
            vector3 position;

            this->GetPosition( position );

            hjoint->SetAnchor( position );

            hjoint->SetAxis( vector3(0,1,0) );
#endif

        }
        break;

    case nPhysicsJoint::BallSocket:
        {        
            nPhyBallSocketJoint* hjoint(0);

            if( !this->joint )
            {
                hjoint = n_new( nPhyBallSocketJoint );

                n_assert2( hjoint, "Failed to create a hinge joint." );

                hjoint->CreateIn( nPhysicsServer::Instance()->GetDefaultWorld(),0 );

                this->joint = hjoint;
            }
            else
            {
                hjoint = static_cast<nPhyBallSocketJoint*>(this->joint);
            }

            nEntityObject* obj(nEntityObjectServer::Instance()->GetEntityObject( this->idA ));

            if( obj )
            {
                ncPhysicsObj* phyObj(obj->GetComponent<ncPhysicsObj>());

                n_assert2( phyObj, "It's requiered to have a physics object component." );

                this->bodyA = phyObj->GetBody();
            }

            obj = nEntityObjectServer::Instance()->GetEntityObject( this->idB );

            if( obj )
            {
                ncPhysicsObj* phyObj(obj->GetComponent<ncPhysicsObj>());

                n_assert2( phyObj, "It's requiered to have a physics object component." );

                this->bodyB = phyObj->GetBody();
            }

#ifndef NGAME
            hjoint->Attach( this->bodyA, this->bodyB ); 

            vector3 position;

            this->GetPosition( position );

            hjoint->SetAnchor( position );
#endif
        }
        break;
    default:
        n_assert_always();
        return;
    }

#ifndef NGAME
    this->RegisterForPostProcess();
#endif

    this->mounted = true;
}

//-----------------------------------------------------------------------------
/**
    Sets the id of the object if any.

    history:
        - 22-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetObjectA( const nEntityObjectId id )
{
    this->idA = id;
}

//-----------------------------------------------------------------------------
/**
    Sets the id of the object if any.

    history:
        - 22-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetObjectB( const nEntityObjectId id )
{
    this->idB = id;
}

#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
//-----------------------------------------------------------------------------
/**
    Mounts the selected object(s).

    history:
        - 22-Aug-2005   David Reyes    created
*/
void ncPhyJoint::MountSelected()
{
    nObjectEditorState* oes(static_cast<nObjectEditorState*>(nConjurerApp::Instance()->FindState( "object" )));

    int numOfSelectedObjects(oes->GetSelectionCount());

    if( !numOfSelectedObjects )
    {
        NLOG( physicsLog , (1, "# Max two selected objects, min one." ) );
        return;
    }

    if( numOfSelectedObjects > 2 )
    {
        NLOG( physicsLog , (1, "# Max two selected objects, min one." ) );
        return;
    }

    nEntityObject* entity(oes->GetSelectedEntity(0));

    n_assert2( entity, "Data corruption." );

    this->SetObjectA(entity->GetId());

    if( numOfSelectedObjects == 2 )
    {
        entity = oes->GetSelectedEntity(1);
        n_assert2( entity, "Data corruption." );
        this->SetObjectB(entity->GetId());
    }

    this->Mount();
}
#endif
//-----------------------------------------------------------------------------
/**
    Sets the object's position.

    history:
        - 23-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetPosition( const vector3& newposition )
{
    vector3 position;

    this->GetPosition( position );

    ncPhyPickableObj::SetPosition( newposition );

    if( !this->mounted )
    {
        return;
    }

    if( !(this->bodyA && this->bodyB) )
    {
        // means it's attached to the wall
        return;
    }

    // update the objects been attached
    position = newposition - position;

    vector3 bodyPosition;

    if( this->bodyA )
    {
        this->bodyA->GetPosition( bodyPosition );        

        this->bodyA->SetPosition( bodyPosition + position );

        this->bodyA->GetPhysicsObj()->UpdateTransformInfo();
    }

    if( this->bodyB )
    {
        this->bodyB->GetPosition( bodyPosition );        

        this->bodyB->SetPosition( bodyPosition + position );

        this->bodyB->GetPhysicsObj()->UpdateTransformInfo();
    }
}

//-----------------------------------------------------------------------------
/**
    Function to be process after running the simulation.

    history:
        - 23-Aug-2005   David Reyes    created
*/
void ncPhyJoint::PostProcess()
{
    vector3 position;

    this->GetAnchorPoint( position );

    ncPhyPickableObj::SetPosition( position );
}

#endif


//-----------------------------------------------------------------------------
/**
    Sets the type of joint.

    @param typeJoint nPhysicsJoint::JointTypes

    history:
        - 24-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetType( const nPhysicsJoint::JointTypes typeJoint ) 
{
    this->type = typeJoint;
}

//-----------------------------------------------------------------------------
/**
    Sets the joint's whereabouts.

    @param point 3d world coordinates

    history:
        - 24-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetAnchorPoint( const vector3& point )
{
#ifndef NGAME
    this->SetPosition( point );
#endif

    if( !this->joint )
    {
        return;
    }

    if( this->joint->Id() == NoValidID )
    {
        return;
    }

    switch( this->type )
    {
        case nPhysicsJoint::Hinge:
            {        
                nPhyHingeJoint* hjoint(static_cast<nPhyHingeJoint*>(this->joint));

                hjoint->SetAnchor( point );
            }
            break;
        case nPhysicsJoint::BallSocket:
            {        
                nPhyBallSocketJoint* hjoint(static_cast<nPhyBallSocketJoint*>(this->joint));

                hjoint->SetAnchor( point );
            }
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the anchor point if any.

    @param point 3d world coordinates

    @return if the feature is supported

    history:
        - 24-Aug-2005   David Reyes    created
*/
bool ncPhyJoint::GetAnchorPoint( vector3& point ) const
{
#ifndef NGAME
    if( !this->mounted )
    {
        this->GetPosition( point );
        return false;
    }
#endif

    if( !this->mounted )
        return false;

    switch( this->type )
    {
        case nPhysicsJoint::Hinge:
            {        
                nPhyHingeJoint* hjoint(static_cast<nPhyHingeJoint*>(this->joint));

                hjoint->GetAnchor( point );
            }
            return true;
        case nPhysicsJoint::BallSocket:
            {        
                nPhyBallSocketJoint* hjoint(static_cast<nPhyBallSocketJoint*>(this->joint));

                hjoint->GetAnchor( point );
            }
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Returns the first axis if any.

    @param axis orientation vector

    @return if the feature is supported

    history:
        - 24-Aug-2005   David Reyes    created
*/
bool ncPhyJoint::GetFirstAxis( vector3& axis ) const
{
    if( !this->mounted )
        return false;

    switch( this->type )
    {
        case nPhysicsJoint::Hinge:
            {        
                nPhyHingeJoint* hjoint(static_cast<nPhyHingeJoint*>(this->joint));

                hjoint->GetAxis( axis );
            }
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Sets the first axis.

    @param axis orientation vector

    history:
        - 24-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetFirstAxis( const vector3& axis )
{
    switch( this->type )
    {
        case nPhysicsJoint::Hinge:
            {        
                nPhyHingeJoint* hjoint(static_cast<nPhyHingeJoint*>(this->joint));

                hjoint->SetAxis( axis );
            }
            break;
    }
}

//-----------------------------------------------------------------------------
/**
    Sets low stop angle or position.

    Setting this to -dInfinity (the default value) turns off the low stop. 
    For rotational joints, this stop must be greater than - pi to be effective.
 
    @param axis which one
    @param value parameter value

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetLowStopAnglePosition( const phy::jointaxis axis, const phyreal value )
{
    this->joint->SetParam( phy::lowerStop, axis, value );
}


//-----------------------------------------------------------------------------
/**
    Sets high stop angle or position.

    Setting this to dInfinity (the default value) turns off the high stop. 
    For rotational joints, this stop must be less than pi to be effective. 
    If the high stop is less than the low stop then both stops will be ineffective.
 
    @param axis which one
    @param value parameter value

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetHiStopAnglePosition( const phy::jointaxis axis, const phyreal value )
{
    this->joint->SetParam( phy::higherStop, axis, value );
}

//-----------------------------------------------------------------------------
/**
    Sets desired motor velocity (this will be an angular or linear velocity).

    @param axis which one
    @param value parameter value

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetVelocity( const phy::jointaxis axis, const phyreal value )
{
    this->joint->SetParam( phy::velocity, axis, value );
}

//-----------------------------------------------------------------------------
/**
    The maximum force or torque that the motor will use to achieve the desired velocity.

    This must always be greater than or equal to zero. 
    Setting this to zero (the default value) turns off the motor.

    @param axis which one
    @param value parameter value

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetMaximunForce( const phy::jointaxis axis, const phyreal value )
{
    this->joint->SetParam( phy::maxForce, axis, value );
}

//-----------------------------------------------------------------------------
/**
    Sets the fudge factor.

    The current joint stop/motor implementation has a small problem: 
    when the joint is at one stop and the motor is set to move it away from the stop, 
    too much force may be applied for one time step, causing a ``jumping'' motion. 
    This fudge factor is used to scale this excess force. 
    It should have a value between zero and one (the default value). 
    If the jumping motion is too visible in a joint, the value can be reduced. 
    Making this value too small can prevent the motor from being able to move 
    the joint away from a stop

    @param axis which one
    @param value parameter value

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetFudgeFactor( const phy::jointaxis axis, const phyreal value )
{
    this->joint->SetParam( phy::fudgeFactor, axis, value );
}

//-----------------------------------------------------------------------------
/**
    Sets the bouncyness of the stops.

    This is a restitution parameter in the range 0..1. 0 means the stops are 
    not bouncy at all, 1 means maximum bouncyness.
    
    @param axis which one
    @param value parameter value

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetBouncynessStop( const phy::jointaxis axis, const phyreal value )
{
    this->joint->SetParam( phy::bounce, axis, value );
}

//-----------------------------------------------------------------------------
/**
    Sets the constraint force mixing (CFM) value used when not at a stop.

    @param axis which one
    @param value parameter value

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetCFM( const phy::jointaxis axis, const phyreal value )
{
    this->joint->SetParam( phy::CFM, axis, value );
}

//-----------------------------------------------------------------------------
/**
    Sets the error reduction parameter (ERP) used by the stops.

    @param axis which one
    @param value parameter value

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetStopERP( const phy::jointaxis axis, const phyreal value )
{
    this->joint->SetParam( phy::stopERP, axis, value );
}

//-----------------------------------------------------------------------------
/**
    Sets the constraint force mixing (CFM) value used by the stops.

    Together with the ERP value this can be used to get spongy or soft stops. 
    Note that this is intended for unpowered joints, 
    it does not really work as expected when a powered joint reaches its limit.

    @param axis which one
    @param value parameter value

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::SetStopCFM( const phy::jointaxis axis, const phyreal value )
{
    this->joint->SetParam( phy::stopCFM, axis, value );
}

//-----------------------------------------------------------------------------
/**
    Unmounts the joint.

    history:
        - 25-Aug-2005   David Reyes    created
*/
void ncPhyJoint::UnMount()
{
    this->joint->ForceBreak();

    this->mounted = false;
}

//-----------------------------------------------------------------------------
/**
    Gets the id of the object if any.

    @return object id

    history:
        - 26-Sep-2005   David Reyes    created
*/
const nEntityObjectId ncPhyJoint::GetObjectA() const 
{
    return this->idA;
}

//-----------------------------------------------------------------------------
/**
    Gets the id of the object if any.

    @return object id

    history:
        - 26-Sep-2005   David Reyes    created
*/
const nEntityObjectId ncPhyJoint::GetObjectB() const
{
    return this->idB;
}
