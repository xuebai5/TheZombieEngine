//-----------------------------------------------------------------------------
//  ncphyfourleggedragdoll_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyfourleggedragdoll.h"
#include "nphysics/nphyuniversaljoint.h"
#include "nphysics/nphyhingejoint.h"
#include "nphysics/ncphyfourleggedragdollclass.h"

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyFourleggedRagDoll,ncPhyRagDoll);

//-----------------------------------------------------------------------------

// x < 0 ----> to the right
// x > 0 ----> to the left

// y > 0 ----> going up
// y < 0 ----> going down

// z > 0 ----> going forward
// z < 0 ----> going backwards

// angle > 0 ---> counterclockwise
// angle < 0 ---> clockwise

//-----------------------------------------------------------------------------
namespace {
    const phyreal fudgeFactor( phyreal(1));
}
//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 28-Feb-2005   Zombie         created
*/
ncPhyFourleggedRagDoll::ncPhyFourleggedRagDoll() :
    mainBody(0),
    head(0),
    leftArm(0),
    rightArm(0),
    leftForeArm(0),
    rightForeArm(0),
    leftLeg(0),
    rightLeg(0),
    foreLeftLeg(0),
    foreRightLeg(0),
    neck(0),
    rightShoulder(0),
    leftWrist(0),
    rightWrist(0),
    leftKnee(0),
    rightKnee(0),
    leftElbow(0),
    rightElbow(0),
    refClass(0)
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 28-Feb-2005   Zombie         created
*/
ncPhyFourleggedRagDoll::~ncPhyFourleggedRagDoll()
{
    // Emtpy
}

//-----------------------------------------------------------------------------
/**
    Process of creating of the object.

    history:
        - 28-Feb-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::Create( nPhysicsWorld* world )
{
    this->AllowCreateSubSpace( false );

    this->CreateSpace();

    this->GetSpace()->MoveToSpace( world->GetSpace() );

    this->MoveToSpace( this->GetSpace() );

    n_assert2( world, "Null pointer" );

    ncPhyCompositeObj::Create( world );

    this->mainBody = this;

    this->AddLimbs();
}

//-----------------------------------------------------------------------------
/**
    Assembles the body.

    history:
        - 28-Feb-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::Assemble()
{
    n_assert2( this->Id() != NoValidID, "It can not be assembled until the object it's in the world." );

    this->CreateArticulations();

    this->AttachHead();
    this->AttachArm( left );
    this->AttachArm( right);
    this->AttachForeArm( right );
    this->AttachForeArm( left );
    this->AttachLeg( left );
    this->AttachLeg( right );
    this->AttachForeLeg( left );
    this->AttachForeLeg( right );
}

//-----------------------------------------------------------------------------
/**
    Attaches the head.

    history:
        - 02-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::AttachHead()
{
    if( !this->neck )
    {
         // no neck in this rag-doll (but head).
        return;
    }

    /// setting neck parameters
    this->neck->Attach( this, this->head );

    this->neck->SetAnchor( this->neckPosition );

    this->neck->SetParam( phy::stopERP, phy::axisA, phyreal(1.));
    this->neck->SetParam( phy::stopCFM, phy::axisA, phyreal(0));

    this->neck->SetParam( phy::lowerStop, phy::axisA, this->refClass->GetNeckFrontAngleMin() );
    this->neck->SetParam( phy::higherStop, phy::axisA, this->refClass->GetNeckFrontAngleMax() );
    this->neck->SetParam( phy::fudgeFactor, phy::axisA, fudgeFactor );
    this->neck->SetParam( phy::bounce, phy::axisB, 0 );


    this->neck->SetParam( phy::stopERP, phy::axisB, phyreal(1.));
    this->neck->SetParam( phy::stopCFM, phy::axisB, phyreal(0));

    this->neck->SetParam( phy::lowerStop, phy::axisB, this->refClass->GetNeckSidesAngleMin() );
    this->neck->SetParam( phy::higherStop, phy::axisB, this->refClass->GetNeckSidesAngleMax() );
    this->neck->SetParam( phy::fudgeFactor, phy::axisB, fudgeFactor );
    this->neck->SetParam( phy::bounce, phy::axisB, 0 );

    // get body's orientation
    matrix33 orientation;

    this->GetOrientation( orientation );

    vector3 axis;

    axis = orientation * vector3(1,0,0);

    this->neck->SetFirstAxis( axis );

    axis = orientation * vector3(0,0,1);

    this->neck->SetSecondAxis( axis );
}

//-----------------------------------------------------------------------------
/**
    Attaches an arm.

    @param arm (left/right)

    history:
        - 02-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::AttachArm( const which arm )
{
    ncPhyRagDollLimb* larm(0);
    nPhyUniversalJoint* larticulation(0);

    const vector3& position( arm == left ? this->leftShoulderPosition : this->rightShoulderPosition );
    
    phyreal angleZMin(this->refClass->GetShoulderUpDownAngleMin());
    phyreal angleZMax(this->refClass->GetShoulderUpDownAngleMax());

    switch( arm )
    {
    case left:
        larm = this->leftArm;
        larticulation = this->leftShoulder;
        break;
    case right:
        larm = this->rightArm;
        larticulation = this->rightShoulder;
        angleZMin = -this->refClass->GetShoulderUpDownAngleMax();
        angleZMax = -this->refClass->GetShoulderUpDownAngleMin();
        break;
    default:
        n_assert2_always( "The IP it's in the wrong place" );
    }

    if( !larm )
    {
        return; // no arm present
    }

    if( !larticulation )
    {
        // no articulation present
        return;
    }

    larticulation->Attach( this, larm ); 

    larticulation->SetAnchor( position );

    larticulation->SetParam( phy::stopERP, phy::axisA, phyreal(0.2));
    larticulation->SetParam( phy::stopCFM, phy::axisA, phyreal(0));

    larticulation->SetParam( phy::lowerStop, phy::axisA, angleZMin );
    larticulation->SetParam( phy::higherStop, phy::axisA, angleZMax );
    larticulation->SetParam( phy::fudgeFactor, phy::axisA, fudgeFactor );
    larticulation->SetParam( phy::bounce, phy::axisA, 0 );

    larticulation->SetParam( phy::stopERP, phy::axisB, phyreal(0.2));
    larticulation->SetParam( phy::stopCFM, phy::axisB, phyreal(0));

    larticulation->SetParam( phy::lowerStop, phy::axisB, this->refClass->GetShoulderFrontAngleMin() );
    larticulation->SetParam( phy::higherStop, phy::axisB, this->refClass->GetShoulderFrontAngleMax()  );
    larticulation->SetParam( phy::fudgeFactor, phy::axisB, fudgeFactor );
    larticulation->SetParam( phy::bounce, phy::axisB, 0 );

    // get body's orientation
    matrix33 orientation;

    this->GetOrientation( orientation );

    vector3 axis;

    axis = orientation * vector3(0,0,1);

    larticulation->SetFirstAxis( axis );

    axis = orientation * vector3(1,0,0);

    larticulation->SetSecondAxis( vector3(1,0,0) );
}

//-----------------------------------------------------------------------------
/**
    Attaches a fore arm.

    @param arm (left/right)

    history:
        - 02-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::AttachForeArm( const which arm )
{
    ncPhyRagDollLimb* lforearm(0);
    ncPhyRagDollLimb* larm(0);
    nPhyHingeJoint* larticulation(0);
    
    const vector3& position( arm == left ? this->leftElbowPosition : this->rightElbowPosition );

    switch( arm )
    {
    case left:
        larm = this->leftArm;
        lforearm = this->leftForeArm;
        larticulation = this->leftElbow;
        break;
    case right:
        larm = this->rightArm;
        lforearm = this->rightForeArm;
        larticulation = this->rightElbow;
        break;
    default:
        n_assert2_always( "The IP it's in the wrong place" );
    }

    if( !larm )
    {
        // no arm present
        return;
    }

    if( !lforearm )
    {
        return; // no fore arm present
    }

    if( !larticulation )
    {
        // no articulation present
        return;
    }

    larticulation->Attach( larm, lforearm ); 

    larticulation->SetAnchor( position );

    larticulation->SetParam( phy::stopERP, phy::axisA, phyreal(1.));
    larticulation->SetParam( phy::stopCFM, phy::axisA, phyreal(0));

    larticulation->SetParam( phy::lowerStop, phy::axisA, this->refClass->GetElbowUpDownAngleMin() );
    larticulation->SetParam( phy::higherStop, phy::axisA, this->refClass->GetElbowUpDownAngleMax() );
    larticulation->SetParam( phy::fudgeFactor, phy::axisA, fudgeFactor );
    larticulation->SetParam( phy::bounce, phy::axisA, 0 );

    // get body's orientation
    matrix33 orientation;

    this->GetOrientation( orientation );

    vector3 axis;

    axis = orientation * vector3(1,0,0);

    larticulation->SetAxis( axis );
}

//-----------------------------------------------------------------------------
/**
    Attaches a leg.

    @param leg (left/right)

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::AttachLeg( const which leg )
{
    ncPhyRagDollLimb* lleg(0);
    nPhyUniversalJoint* larticulation(0);
    
    const vector3& position( leg == left ? this->leftWristPosition : this->rightWristPosition );

    switch( leg )
    {
    case left:
        lleg = this->leftLeg;
        larticulation = this->leftWrist;
        break;
    case right:
        lleg = this->rightLeg;
        larticulation = this->rightWrist;
        break;
    default:
        n_assert2_always( "The IP it's in the wrong place" );
    }

    if( !lleg )
    {
        return; // no leg present
    }

    if( !larticulation )
    {
        // no articulation present
        return;
    }

    larticulation->Attach( this, lleg ); 

    larticulation->SetAnchor( position );

    larticulation->SetParam( phy::stopERP, phy::axisA, phyreal(0.2));
    larticulation->SetParam( phy::stopCFM, phy::axisA, phyreal(0));

    larticulation->SetParam( phy::lowerStop, phy::axisA, this->refClass->GetWristUpDownAngleMin() );
    larticulation->SetParam( phy::higherStop, phy::axisA, this->refClass->GetWristUpDownAngleMax() );
    larticulation->SetParam( phy::fudgeFactor, phy::axisA, fudgeFactor );
    larticulation->SetParam( phy::bounce, phy::axisA, 0 );

    larticulation->SetParam( phy::stopERP, phy::axisB, phyreal(0.2));
    larticulation->SetParam( phy::stopCFM, phy::axisB, phyreal(0));

    larticulation->SetParam( phy::lowerStop, phy::axisB, this->refClass->GetWristSidesAngleMin() );
    larticulation->SetParam( phy::higherStop, phy::axisB,this->refClass->GetWristSidesAngleMax() );
    larticulation->SetParam( phy::fudgeFactor, phy::axisB, fudgeFactor );
    larticulation->SetParam( phy::bounce, phy::axisB, 0 );

    // get body's orientation
    matrix33 orientation;

    this->GetOrientation( orientation );

    vector3 axis;

    axis = orientation * vector3(1,0,0);

    larticulation->SetFirstAxis( axis );

    axis = orientation * vector3(0,0,1);

    larticulation->SetSecondAxis( axis );
}

//-----------------------------------------------------------------------------
/**
    Attaches a fore leg.

    @param leg (left/right)

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::AttachForeLeg( const which leg )
{
    ncPhyRagDollLimb* lforeleg(0);
    ncPhyRagDollLimb* lleg(0);
    nPhyHingeJoint* larticulation(0);
    
    const vector3& position( leg == left ? this->leftKneePosition : this->rightKneePosition );

    switch( leg )
    {
    case left:
        lleg = this->leftLeg;
        lforeleg = this->foreLeftLeg;
        larticulation = this->leftKnee;
        break;
    case right:
        lleg = this->rightLeg;
        lforeleg = this->foreRightLeg;
        larticulation = this->rightKnee;
        break;
    default:
        n_assert2_always( "The IP it's in the wrong place" );
    }

    if( !lleg )
    {
        // no leg present
        return;
    }

    if( !lforeleg )
    {
        return; // no fore leg present
    }


    if( !larticulation )
    {
        // no articulation present
        return;
    }

    larticulation->Attach( lleg, lforeleg ); 

    larticulation->SetAnchor( position );

    larticulation->SetParam( phy::stopERP, phy::axisA, phyreal(1.));
    larticulation->SetParam( phy::stopCFM, phy::axisA, phyreal(0));

    larticulation->SetParam( phy::lowerStop, phy::axisA, this->refClass->GetKneeUpDownAngleMin() );
    larticulation->SetParam( phy::higherStop, phy::axisA, this->refClass->GetKneeUpDownAngleMax() );
    larticulation->SetParam( phy::fudgeFactor, phy::axisA, fudgeFactor );
    larticulation->SetParam( phy::bounce, phy::axisA, 0 );

    // get body's orientation
    matrix33 orientation;

    this->GetOrientation( orientation );

    vector3 axis;

    axis = orientation * vector3(0,1,0);

    larticulation->SetAxis( axis );
}

//-----------------------------------------------------------------------------
/**
    Sets the head object.

    @param head head limb

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetHead( ncPhyRagDollLimb* head )
{
    this->head = head;
}

//-----------------------------------------------------------------------------
/**
    Sets the arm object.

    @param arm (left/right)
    @param armobj arm object

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetArm( const which arm, ncPhyRagDollLimb* armobj )
{
    switch( arm )
    {
    case left:
        this->leftArm = armobj;
        break;
    case right:
        this->rightArm = armobj;
        break;
    default:
        n_assert2_always( "Bad option, human only have two arms." );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the fore arm object.

    @param arm (left/right)
    @param armobj arm object

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetForeArm( const which arm, ncPhyRagDollLimb* armobj )
{
    switch( arm )
    {
    case left:
        this->leftForeArm = armobj;
        break;
    case right:
        this->rightForeArm = armobj;
        break;
    default:
        n_assert2_always( "Bad option, human only have two fore arms." );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the leg object.

    @param leg (left/right)
    @param legobj leg object

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetLeg( const which leg, ncPhyRagDollLimb* legobj )
{
    switch( leg )
    {
    case left:
        this->leftLeg = legobj;
        break;
    case right:
        this->rightLeg = legobj;
        break;
    default:
        n_assert2_always( "Bad option, human only have two legs." );
    }    
}

//-----------------------------------------------------------------------------
/**
    Sets the fore leg object.

    @param leg (left/right)
    @param legobj leg object

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetForeLeg( const which leg, ncPhyRagDollLimb* legobj )
{
    switch( leg )
    {
    case left:
        this->foreLeftLeg = legobj;
        break;
    case right:
        this->foreRightLeg = legobj;
        break;
    default:
        n_assert2_always( "Bad option, human only have two fore legs." );
    }    
}

//-----------------------------------------------------------------------------
/**
    Creates the articulations.

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::CreateArticulations()
{
    this->CreateNeck();
    this->CreateShoulders();
    this->CreateElbows();
    this->CreateWrist();
    this->CreateKnees();
}

//-----------------------------------------------------------------------------
/**
    Sets the neck in world position.

    @param position neck pivot point

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetNeckPoint( const vector3& position )
{
    this->neckPosition = position;
}

//-----------------------------------------------------------------------------
/**
    Sets the shoulder point.

    @param shoulder (left/right)
    @param position shoulder pivot point
    
    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetShoulderPoint( const which shoulder, const vector3& position )
{
    switch( shoulder )
    {
    case left:
        this->leftShoulderPosition = position;
        break;
    case right:
        this->rightShoulderPosition = position;
        break;
    default:
        n_assert2_always( "Bad option, human only have two shoulders." );
    }
}


//-----------------------------------------------------------------------------
/**
    Sets the elbow point.

    @param elbow (left/right)
    @param position elbow pivot point

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetElbowPoint( const which elbow, const vector3& position )
{
    switch( elbow )
    {
    case left:
        this->leftElbowPosition = position;
        break;
    case right:
        this->rightElbowPosition = position;
        break;
    default:
        n_assert2_always( "Bad option, human only have two elbows." );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the wrist position.

    @param leg (left/right)
    @param position leg pivot point

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetWristPoint( const which leg, const vector3& position )
{
    switch( leg )
    {
    case left:
        this->leftWristPosition = position;
        break;
    case right:
        this->rightWristPosition = position;
        break;
    default:
        n_assert2_always( "Bad option, human only have two legs." );
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the knee position.

    @param knee (left/right)
    @param position knee pivot point

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::SetKneePoint( const which knee, const vector3& position )
{
    switch( knee )
    {
    case left:
        this->leftKneePosition = position;
        break;
    case right:
        this->rightKneePosition = position;
        break;
    default:
        n_assert2_always( "Bad option, human only have two knees." );
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the neck.

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::CreateNeck()
{   
    this->neck = static_cast<nPhyUniversalJoint*>(nKernelServer::Instance()->New( "nphyuniversaljoint" ));

    n_assert2( this->neck, "Failed to create the neck" );

    this->neck->CreateIn( this->GetWorld(), 0 );

    this->AddLimbJoint( this->neck );
}

//-----------------------------------------------------------------------------
/**
    Creates the elbows.

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::CreateElbows()
{
    this->leftElbow = static_cast<nPhyHingeJoint*>(nKernelServer::Instance()->New( "nphyhingejoint" ));

    n_assert2( this->leftElbow, "Failed to create the elbows" );


    this->rightElbow = static_cast<nPhyHingeJoint*>(nKernelServer::Instance()->New( "nphyhingejoint" ));

    n_assert2( this->rightElbow, "Failed to create the elbows" );

    this->leftElbow->CreateIn( this->GetWorld(), 0 );
    this->rightElbow->CreateIn( this->GetWorld(), 0 );

    this->AddLimbJoint( this->leftElbow, this->leftShoulder );
    this->AddLimbJoint( this->rightElbow, this->rightShoulder );

}

//-----------------------------------------------------------------------------
/**
    Creates the shoulders.

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::CreateShoulders()
{
    this->leftShoulder = static_cast<nPhyUniversalJoint*>(nKernelServer::Instance()->New( "nphyuniversaljoint" ));

    n_assert2( this->leftShoulder, "Failed to create the shoulders" );
    
    this->rightShoulder = static_cast<nPhyUniversalJoint*>(nKernelServer::Instance()->New( "nphyuniversaljoint" ));

    n_assert2( this->rightShoulder, "Failed to create the shoulders" );

    this->leftShoulder->CreateIn( this->GetWorld(), 0 );
    this->rightShoulder->CreateIn( this->GetWorld(), 0 );

    this->AddLimbJoint( this->leftShoulder );
    this->AddLimbJoint( this->rightShoulder );
}

//-----------------------------------------------------------------------------
/**
    Creates the wrist.

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::CreateWrist()
{
    this->leftWrist = static_cast<nPhyUniversalJoint*>(nKernelServer::Instance()->New( "nphyuniversaljoint" ));

    n_assert2( this->leftWrist, "Failed to create the wrist" );

    this->rightWrist = static_cast<nPhyUniversalJoint*>(nKernelServer::Instance()->New( "nphyuniversaljoint" ));

    n_assert2( this->rightWrist, "Failed to create the wrist" );

    this->leftWrist->CreateIn( this->GetWorld(), 0 );
    this->rightWrist->CreateIn( this->GetWorld(), 0 );

    this->AddLimbJoint( this->leftWrist );
    this->AddLimbJoint( this->rightWrist );
}

//-----------------------------------------------------------------------------
/**
    Creates the knees.

    history:
        - 08-Mar-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::CreateKnees()
{
    this->leftKnee = static_cast<nPhyHingeJoint*>(nKernelServer::Instance()->New( "nphyhingejoint" ));

    n_assert2( this->leftKnee, "Failed to create the knees" );

    this->rightKnee = static_cast<nPhyHingeJoint*>(nKernelServer::Instance()->New( "nphyhingejoint" ));

    n_assert2( this->rightKnee, "Failed to create the knees" );

    this->leftKnee->CreateIn( this->GetWorld(), 0 );
    this->rightKnee->CreateIn( this->GetWorld(), 0 );

    this->AddLimbJoint( this->leftKnee, this->leftWrist );
    this->AddLimbJoint( this->rightKnee, this->rightWrist );
}

//-----------------------------------------------------------------------------
/**
    Function to be procesed during the collision check.

    @param numContacts maximun number of contacts expected
    @param contacts container of contacts

    @return if any collision was processed

    history:
        - 09-Mar-2005   Zombie         created
*/
bool ncPhyFourleggedRagDoll::CollisionProcess( int numContacts, nPhyCollide::nContact* contacts )
{
#ifndef PHY_JOINED_BODIES_DONT_COLLIDE
    if( contacts->GetPhysicsObjB()->GetParentSpace() == contacts->GetPhysicsObjA()->GetParentSpace() )
    {
        
        nPhyRigidBody* bodyA(contacts->GetGeometryA()->GetBody()); 
        nPhyRigidBody* bodyB(contacts->GetGeometryB()->GetBody());

        if( bodyA && bodyB )
        {
            if( phyAreConnected( bodyA->Id(), bodyB->Id() ) )
            {
                return true;
            }
        }
    }
#endif // !PHY_JOINED_BODIES_DONT_COLLIDE

    ncPhysicsObj::CollisionProcess( numContacts, contacts );

    return false;
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 09-May-2005   Zombie         created
*/
void ncPhyFourleggedRagDoll::InitInstance(nObject::InitInstanceMsg initType)
{
    // gets a reference to the component "class"
    this->refClass = this->GetClassComponent<ncPhyFourleggedRagDollClass>();

    n_assert2( this->refClass, "Failed to obtain the component \"class\"" );

    ncPhyRagDoll::InitInstance( initType );
}

/// adds the limbs to the world
void ncPhyFourleggedRagDoll::AddLimbs()
{

    if( this->head )
    {
        this->head->MoveToSpace( this->GetSpace() );

        this->GetWorld()->Add( this->head->GetEntityObject() );

        this->head->SetRagDoll( this );

        this->AddLimb( this->head );

    }

    /////////////////////////////////////////////////////////////////

    if( this->leftArm )
    {

        this->leftArm->MoveToSpace( this->GetSpace() );

        this->GetWorld()->Add( this->leftArm->GetEntityObject() );

        this->leftArm->SetRagDoll( this );

        this->AddLimb( this->leftArm );
    }

    /////////////////////////////////////////////////////////////////

    if( this->rightArm )
    {

        this->rightArm->MoveToSpace( this->GetSpace() );

        this->GetWorld()->Add( this->rightArm->GetEntityObject() );

        this->rightArm->SetRagDoll( this );

        this->AddLimb( this->rightArm );
    }


    /////////////////////////////////////////////////////////////////
    
    if( this->leftForeArm )
    {

        this->leftForeArm->MoveToSpace( this->GetSpace() );

        this->GetWorld()->Add( this->leftForeArm->GetEntityObject() );

        this->leftForeArm->SetRagDoll( this );

        this->AddLimb( this->leftForeArm );
    
    }

    /////////////////////////////////////////////////////////////////
    
    if( this->rightForeArm )
    {

        this->rightForeArm->MoveToSpace( this->GetSpace() );

        this->GetWorld()->Add( this->rightForeArm->GetEntityObject() );

        this->rightForeArm->SetRagDoll( this );

        this->AddLimb( this->rightForeArm );
    
    }

    /////////////////////////////////////////////////////////////////
    
    if( this->leftLeg )
    {

        this->leftLeg->MoveToSpace( this->GetSpace() );

        this->GetWorld()->Add( this->leftLeg->GetEntityObject() );

        this->leftLeg->SetRagDoll( this );

        this->AddLimb( this->leftLeg );
    
    }

    /////////////////////////////////////////////////////////////////
    
    if( this->rightLeg )
    {

        this->rightLeg->MoveToSpace( this->GetSpace() );

        this->GetWorld()->Add( this->rightLeg->GetEntityObject() );

        this->rightLeg->SetRagDoll( this );

        this->AddLimb( this->rightLeg );
    
    }


    /////////////////////////////////////////////////////////////////
    
    if( this->foreLeftLeg )
    {

        this->foreLeftLeg->MoveToSpace( this->GetSpace() );

        this->GetWorld()->Add( this->foreLeftLeg->GetEntityObject() );

        this->foreLeftLeg->SetRagDoll( this );

        this->AddLimb( this->foreLeftLeg );
    
    }

    /////////////////////////////////////////////////////////////////
    
    if( this->foreRightLeg )
    {

        this->foreRightLeg->MoveToSpace( this->GetSpace() );

        this->GetWorld()->Add( this->foreRightLeg->GetEntityObject() );

        this->foreRightLeg->SetRagDoll( this );

        this->AddLimb( this->foreRightLeg );
    
    }
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
