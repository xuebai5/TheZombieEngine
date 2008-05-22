//-----------------------------------------------------------------------------
//  ncgameplaydoor_main.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchrnsgameplay.h"

#include "ncgameplaydoor/ncgameplaydoor.h"

#include "nscene/ncscene.h"

#include "entity/nentityobjectserver.h"

#include "zombieentity/nctransform.h"

#include "nphysics/nphyhingejoint.h"

#include "nphysics/nphysicsserver.h"

#include "nphysics/ncphysicsobj.h"

#include "zombieentity/ncsubentity.h"

#include "zombieentity/ncsuperentity.h"

#include "entity/nobjectinstancer.h"

#include "nspatial/ncspatialportal.h"

#include "nspatial/nspatialserver.h"

//-----------------------------------------------------------------------------
namespace
{
    const char* plugName( "DoorHinge" );
}

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncGamePlayDoor,ncGameplay);

//-----------------------------------------------------------------------------
/**
    Constructor

    history:
        - 29-Nov-2005   Zombie         created
*/
ncGamePlayDoor::ncGamePlayDoor() :
    doorType( ncGamePlayDoorClass::hinge ),
    attributes(ncGamePlayDoorClass::closesalone),
    objectDoorA(0),
    objectDoorB(0),
    jointA(0),
    jointB(0),
    portal(0),
    doorAngles( float(M_PI/2.), 0 ),
    mounted( false ),
    slidingSpeed( float(.1) ),
    originalPositionA(0,0,0),
    originalPositionB(0,0,0),
    timeToDoAction(float(2000)),
    doorState(close),
    waitingTime(1),
    slidingSpace(0),
    sliddingMax(5),
    angleDone(0),
    angleSpeed(float(.1))
{
}

//-----------------------------------------------------------------------------
/**
    Destructor

    history:
        - 29-Nov-2005   Zombie         created
*/
ncGamePlayDoor::~ncGamePlayDoor()
{
    if( this->objectDoorA )
    {
        this->objectDoorA = 0;
    }


    if( this->objectDoorB )
    {
        this->objectDoorB = 0;
    }


    this->Destroy();
}

//-----------------------------------------------------------------------------
/**
    Returns the door type

    @return door type

    history:
        - 29-Nov-2005   Zombie         created
*/
const ncGamePlayDoorClass::type ncGamePlayDoor::GetType() const
{
    return this->doorType;
}

//-----------------------------------------------------------------------------
/**
    Sets the door type

    @param newtype new door's type

    history:
        - 29-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::SetType( const ncGamePlayDoorClass::type newtype )
{
    this->doorType = newtype;
}

//-----------------------------------------------------------------------------
/**
    Gets doors attributes

    @return door's attributes

    history:
        - 29-Nov-2005   Zombie         created
*/
const int ncGamePlayDoor::GetAttributes() const
{
    return this->attributes;
}

//-----------------------------------------------------------------------------
/**
    Sets attribs to the door

    @param newattributes new door's attributes set

    history:
        - 29-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::SetAttributes( const int newattributes )
{
    this->attributes = newattributes;
}

//-----------------------------------------------------------------------------
/**
    Returns if the door is locked

    @return true/false

    history:
        - 29-Nov-2005   Zombie         created
*/
const bool ncGamePlayDoor::IsLocked() const
{
    return this->doorState == locked;
}

//-----------------------------------------------------------------------------
/**
    Locks the door

    history:
        - 29-Nov-2005   Zombie         created
*/
const bool ncGamePlayDoor::Lock()
{
    if( this->doorState != close )
    {
        return false;
    }

    this->SetNewState( locked );

    return true;
}

//-----------------------------------------------------------------------------
/**
    Unlocks the door

    history:
        - 29-Nov-2005   Zombie         created
*/
const bool ncGamePlayDoor::Unlock()
{
    if( this->doorState != locked )
    {
        return false;
    }

    this->SetNewState( close );    

    return true;
}

//-----------------------------------------------------------------------------
/**
    Opens the door

    history:
        - 29-Nov-2005   Zombie         created  
*/
bool ncGamePlayDoor::Open() 
{
    if( this->IsLocked() )
    {
        return false;
    }

    if( doorState == open )
    {
        return false;
    }

    if( doorState == openning )
    {
        return false;
    }

    this->SetNewState( openning );

    return true;
}

//-----------------------------------------------------------------------------
/**
    Close door

    history:
        - 29-Nov-2005   Zombie         created
*/
bool ncGamePlayDoor::Close()
{
    if( this->IsLocked() )
    {
        return false;
    }

    if( doorState == close )
    {
        return false;
    }

    if( doorState == clossing )
    {
        return false;
    }

    this->SetNewState( clossing );

    return true;
}

//-----------------------------------------------------------------------------
/**
    Returns if the door is broken

    @param true/false

    history:
        - 29-Nov-2005   Zombie         created
*/
const bool ncGamePlayDoor::IsBroken() const
{
    return this->doorState == broken;
}

//-----------------------------------------------------------------------------
/**
    Sets the door entity A

    @param object door object representing

    history:
        - 29-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::SetDoorEntityA( nEntityObject* object )
{
    this->objectDoorA = object;
}

//-----------------------------------------------------------------------------
/**
    Gets the door entity A

    @return door object representing

    history:
        - 29-Nov-2005   Zombie         created
*/
nEntityObject* ncGamePlayDoor::GetDoorEntityA() const
{
    return this->objectDoorA;
}

//-----------------------------------------------------------------------------
/**
    Sets the door entity B

    @param object door object representing

    history:
        - 29-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::SetDoorEntityB( nEntityObject* object )
{
    this->objectDoorB = object;
}

//-----------------------------------------------------------------------------
/**
    Gets the door entity B

    @return door object representing

    history:
        - 29-Nov-2005   Zombie         created
*/
nEntityObject* ncGamePlayDoor::GetDoorEntityB() const
{
    return this->objectDoorB;
}

//-----------------------------------------------------------------------------
/**
    Mounts the door

    history:
        - 29-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::Mount()
{
    this->UnMount();

    this->SetType( this->GetClassComponentSafe<ncGamePlayDoorClass>()->GetType() );

    this->RetrieveDoors();

    this->PlugFakes();

    if( this->GetType() == ncGamePlayDoorClass::slidding )
    {
        this->MountSlidding();
    }
    else
    {
        this->MountHinge();
    }

    this->PreCompute();

    this->OriginalState();

    if (this->GetClassComponent<ncGamePlayDoorClass>()->GetConnectToPortal() && 
        (this->GetAttributes() & ~ncGamePlayDoorClass::free))
    {
        n_verify(this->ConnectToPortal());
    }

    this->UpdatePortal();

    this->mounted = true;
}

//-----------------------------------------------------------------------------
/**
    Mounts slidding doors

    history:
        - 29-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::MountSlidding()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Mounts hinge doors

    history:
        - 29-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::MountHinge()
{

    if( this->GetAttributes() == ncGamePlayDoorClass::free )
    {
        this->MountFreeDoor();
    }
    else
    {
        this->MountHingeDoor();
    }
}

//-----------------------------------------------------------------------------
/**
    User init instance code

    history:
        - 29-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType != nObject::ReloadedInstance)
    {
        // bind to level creation and loading
        this->GetEntityObject()->BindSignal( 
            ncSubentity::SignalSubEntitiesCreated, 
            this,
            &ncGamePlayDoor::ProcessDoor,
            0);
    } 
#ifndef NGAME
    else
    {
        this->UnMount();
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Do action

    @return if the action has finished

    history:
        - 29-Nov-2005   Zombie         created
*/
const bool ncGamePlayDoor::DoAction( const float deltaTime )
{
#ifndef NGAME
    // checks if the door it's mounted
    if( !this->FakeA.isvalid() )
    {
        this->Mount(); // forcing to mount the door
        return true;
    }
#endif 
    this->lastTimeIncrement = deltaTime;

    this->frameProportion = this->lastTimeIncrement / (float(1) / float(60));

    if( this->GetType() == ncGamePlayDoorClass::slidding )
    {
        return this->DoSliddingAction( deltaTime );
    }
    else
    {
        return this->DoHingeAction( deltaTime );
    }
}

//-----------------------------------------------------------------------------
/**
    Do action for slidding

    @return if the action has finished

    history:
        - 29-Nov-2005   Zombie         created
*/
const bool ncGamePlayDoor::DoSliddingAction( const nTime& time )
{
    this->timeElapsed += time;

    switch( this->doorState )
    {
    case none:
        break;
    case openning:
        return this->DoSliddingOpenningAction();
    case waiting:
        return this->DoWaiting();
    case clossing:
        return this->DoSliddingClosenningAction();
    case open:
        return true;
    case close:
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Do action for hinge

    @return if the action has finished

    history:
        - 29-Nov-2005   Zombie         created
*/
const bool ncGamePlayDoor::DoHingeAction( const nTime& time )
{
    if( this->GetAttributes() == ncGamePlayDoorClass::free )
    {
        // nothing to do
        return true;
    }

    this->timeElapsed += time;

    switch( this->doorState )
    {
    case none:
        break;
    case openning:
        return this->DoHingeOpenningAction();
    case waiting:
        return this->DoWaiting();
    case clossing:
        return this->DoHingeClosenningAction();
    case open:
        break;
    case close:
        break;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Restores the door stat after loading

    history:
        - 30-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::Restore()
{
    // TODO: Implement
}

//-----------------------------------------------------------------------------
/**
    Plugs the fakes

    history:
        - 30-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::PlugFakes()
{
    if( !this->FakeA.isvalid() )
    {
        nEntityObject *entA = 0;
        this->PlugFake( entA, objectDoorA );
        this->FakeA = entA;
    }

    if( !this->FakeB.isvalid() )
    {
        nEntityObject *entB = 0;
        this->PlugFake( entB, objectDoorB );
        this->FakeB = entB;
    }
}

//-----------------------------------------------------------------------------
/**
    Plugs a fake

    @param fake fake object to be created
    @param to where to plug

    history:
        - 30-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::PlugFake( nEntityObject*& fake, nEntityObject* to )
{
    if( !to )
    {
        // no object, no plug
        return;
    }

    fake = nEntityObjectServer::Instance()->NewLocalEntityObject( "nefakeentity" );

    n_assert2( fake, "Failed to create the instance of the fake entity." );

    ncScene* scene(to->GetComponent<ncScene>());

    n_assert2( scene, "The object has a missing component." );

    scene->Plug( plugName, fake->GetId() );
}

//-----------------------------------------------------------------------------
/**
    Destroys dynamic data

    history:
        - 30-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::Destroy()
{
    // destroys the objects related to the door

    if( this->FakeA.isvalid() )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->FakeA );
        this->FakeA.invalidate();
    }

    if( this->FakeB.isvalid() )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->FakeB );
        this->FakeB.invalidate();
    }

    if( this->jointA )
    {
        this->jointA->Release();
        this->jointA = 0;
    }

    if( this->jointB )
    {
        this->jointA->Release();
        this->jointB = 0;
    }

    // not longer mounted
    this->mounted = false;
}

//-----------------------------------------------------------------------------
/**
    Sets the the door max angle

    @param value angle

    history:
        - 30-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::SetMaxAngle( const float value )
{
    this->doorAngles.theta = value;

    if( this->mounted )
    {
        if( this->jointA )
        {
            this->jointA->SetParam( phy::lowerStop, phy::axisA, -this->GetMaxAngle() );
        }
        if( this->jointB )
        {
            this->jointB->SetParam( phy::higherStop, phy::axisA, this->GetMaxAngle() );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Sets the the door min angle

    @param value angle

    history:
        - 30-Nov-2005   Zombie         created
*/
void ncGamePlayDoor::SetMinAngle( const float value )
{
    this->doorAngles.rho = value;

    if( this->mounted )
    {
        if( this->jointA )
        {
            this->jointA->SetParam( phy::higherStop, phy::axisA, -this->GetMinAngle() );
        }
        if( this->jointB )
        {
            this->jointB->SetParam( phy::lowerStop, phy::axisA, this->GetMinAngle() );
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Gets the the door max angle

    @return angle

    history:
        - 30-Nov-2005   Zombie         created
*/
const float ncGamePlayDoor::GetMaxAngle() const
{
    return this->doorAngles.theta;
}

//-----------------------------------------------------------------------------
/**
    Gets the the door min angle

    @return angle

    history:
        - 30-Nov-2005   Zombie         created
*/
const float ncGamePlayDoor::GetMinAngle() const
{
    return this->doorAngles.rho;
}

//-----------------------------------------------------------------------------
/**
    Returns if the door is close

    @return angle

    history:
        - 01-Dec-2005   Zombie         created
*/
const bool ncGamePlayDoor::IsClosed() const
{
    return this->doorState == close || this->doorState == locked;
}

//-----------------------------------------------------------------------------
/**
    Returns if the door is open

    @return true if is open
*/
const bool ncGamePlayDoor::IsOpened() const
{
    return this->doorState == open;
}

//-----------------------------------------------------------------------------
/**
    Stores the original state

    history:
        - 01-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::OriginalState()
{
    if( this->objectDoorA )
    {
        this->originalPositionA = this->objectDoorA->GetComponent<ncTransform>()->GetPosition();
        
        matrix44 orientation( this->objectDoorA->GetComponent<ncTransform>()->GetTransform() );

        this->originalMatrixA.set( 
            orientation.x_component(),
            orientation.y_component(),
            orientation.z_component() );

        if( this->FakeA )
        {           
           const vector3& pos(this->FakeA->GetComponent<ncTransform>()->GetPosition());
           
           this->pivotA = this->originalPositionA - pos;
        }

    }

    if( this->objectDoorB )
    {
        this->originalPositionB = this->objectDoorB->GetComponent<ncTransform>()->GetPosition();
        
        matrix44 orientation( this->objectDoorB->GetComponent<ncTransform>()->GetTransform() );

        this->originalMatrixB.set( 
            orientation.x_component(),
            orientation.y_component(),
            orientation.z_component() );

        if( this->FakeB )
        {           
           const vector3& pos(this->FakeB->GetComponent<ncTransform>()->GetPosition());
           
           this->pivotB = this->originalPositionB - pos;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Does some precomputation

    history:
        - 01-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::PreCompute()
{
    if( this->GetType() == ncGamePlayDoorClass::hinge )
    {
        this->openingAngle = this->GetMaxAngle() == 0 ? this->GetMinAngle() : this->GetMaxAngle();
    }

    if( openingAngle < 0 )
    {
        this->angleSpeed *= -1.f;
    }
}

//-----------------------------------------------------------------------------
/**
    Searches the corresponding spatial portal to this door

    history:
        -16-Mar-2006    Miquel Angel Rujula    created
*/
bool ncGamePlayDoor::ConnectToPortal()
{
    static float radius = 100.f; // meters
    sphere sph(this->GetComponentSafe<ncTransform>()->GetPosition(), radius);

    int flags(0);
    flags |= nSpatialTypes::SPF_ALL_INDOORS;
    flags |= nSpatialTypes::SPF_ALL_CELLS;
    flags |= nSpatialTypes::SPF_CONTAINING;
    flags |= nSpatialTypes::SPF_USE_POSITION;

    nArray<nEntityObject*> portals;

    // search all the portals in a certain radius
    nSpatialServer::Instance()->GetEntitiesCategory(sph, nSpatialTypes::CAT_PORTALS, flags, portals);
    
    if (portals.Empty())
    {
        // there's no portal in this radius to connect to
        return false;
    }

    ncSpatialPortal* currPortal = 0;
    float minDist = FLT_MAX;
    float currDist = -1;
    const vector3& doorPos(this->GetComponentSafe<ncTransform>()->GetPosition());
    for (int i(0); i < portals.Size(); ++i)
    {
        currPortal = portals[i]->GetComponentSafe<ncSpatialPortal>();
        currDist = (currPortal->GetClipCenter() - doorPos).lensquared();
        if ( currDist < minDist )
        {
            this->portal = currPortal;
            minDist = currDist;
        }
    }

    // we've found the portal and the door is now connected to it.
    // let's pass the door brushes to the portal and to its twin (if exists)
    ncSpatialPortal *twinPortal = this->portal->GetTwinPortal();
    if (this->GetDoorEntityA())
    {
        this->portal->AddBrush(this->GetDoorEntityA());
        if (twinPortal)
        {
            twinPortal->AddBrush(this->GetDoorEntityA());
            this->GetDoorEntityA()->GetComponentSafe<ncSpatial>()->SetAssociatedToPortal(true);
        }
    }

    if (this->GetDoorEntityB())
    {
        this->portal->AddBrush(this->GetDoorEntityB());
        if (twinPortal)
        {
            twinPortal->AddBrush(this->GetDoorEntityB());
            this->GetDoorEntityB()->GetComponentSafe<ncSpatial>()->SetAssociatedToPortal(true);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Changes the state

    @param newstate new state

    history:
        - 01-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::SetNewState( const int newstate )
{
    this->doorState = static_cast<state>(newstate);

    this->UpdatePortal();

    this->timeElapsed = 0; // resets timer
}

//-----------------------------------------------------------------------------
/**
    Update the portal state

    history:
        -20-Mar-2006    Miquel Angel Rujula     created
*/
void ncGamePlayDoor::UpdatePortal()
{
    if (this->portal)
    {
        // portals info transfer
        switch( this->doorState )
        {
            case openning:
                this->portal->SetActiveAndTwin(true);
                break;
            case close:
                this->portal->SetActiveAndTwin(false);
                break;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Hinge openning action

    history:
        - 01-Dec-2005   Zombie         created
*/
const bool ncGamePlayDoor::DoHingeOpenningAction()
{
    // stores the angle already done
    this->angleDone += this->angleSpeed * this->frameProportion;

    if( fabs(this->angleDone) > fabs(openingAngle) )
    {
        this->angleDone = this->openingAngle;
    }

    // sets the door orientation
    matrix33 neworientation(this->originalMatrixA);

    neworientation.rotate_y( this->angleDone );

    matrix33 oldorientation;

    this->objectDoorA->GetComponent<ncPhysicsObj>()->GetOrientation( oldorientation );

    this->objectDoorA->GetComponent<ncPhysicsObj>()->SetRotation( neworientation );

    matrix33 m33;

    m33.rotate_y( this->angleDone );

    const vector3 newpivot( m33 *  this->pivotA );

    vector3 oldposition;

    this->objectDoorA->GetComponent<ncPhysicsObj>()->GetPosition( oldposition );

    this->objectDoorA->GetComponent<ncPhysicsObj>()->SetPosition( 
        this->originalPositionA - this->pivotA + newpivot );

    // checks if collision in the new position
    nPhyCollide::nContact contact;

    ncPhysicsObj* physicsObject( this->objectDoorA->GetComponent<ncPhysicsObj>() );

    n_assert2( physicsObject, "The object mus have a ncPhysicsObj component." );

    if( physicsObject->Collide( 1, &contact ) )
    {
        // restoring old position
        this->objectDoorA->GetComponent<ncPhysicsObj>()->SetPosition( oldposition );
        this->objectDoorA->GetComponent<ncPhysicsObj>()->SetRotation( oldorientation );
        this->SetNewState( clossing );
        return false;
    }

    if( this->angleDone == this->openingAngle )
    {
        if( this->GetAttributes() == ncGamePlayDoorClass::closesalone )
        {
            this->SetNewState( waiting );
            return false;
        }
        else
        {
            this->SetNewState( open );
            return true;
        }
    }

    // check not run out of time
    if( this->timeElapsed > this->timeToDoAction )
    {
        this->SetNewState( clossing );
        return false;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Waits for a time

    history:
        - 01-Dec-2005   Zombie         created
*/
const bool ncGamePlayDoor::DoWaiting()
{
    // @todo: active gameplay object
    if( this->timeElapsed > this->waitingTime )
    {
        // @todo: disable gameplay object
        this->SetNewState( clossing );
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Hinge openning action

    history:
        - 01-Dec-2005   Zombie         created
*/
const bool ncGamePlayDoor::DoHingeClosenningAction()
{
    // stores the angle already done
    this->angleDone -= this->angleSpeed * this->frameProportion;

    if( this->angleSpeed < 0 )
    {
        if( this->angleDone >= 0 )
        {
            this->angleDone = 0;
        }
    }
    else
    {
        if( this->angleDone <= 0 )
        {
            this->angleDone = 0;
        }
    }

    // sets the door orientation
    matrix33 neworientation(this->originalMatrixA);

    neworientation.rotate_y( this->angleDone );

    this->objectDoorA->GetComponent<ncPhysicsObj>()->SetRotation( neworientation );

    matrix33 m33;

    m33.rotate_y( this->angleDone );

    matrix33 oldorientation;

    this->objectDoorA->GetComponent<ncPhysicsObj>()->GetOrientation( oldorientation );

    const vector3 newpivot( m33 *  this->pivotA );

    vector3 oldposition;

    this->objectDoorA->GetComponent<ncPhysicsObj>()->GetPosition( oldposition );

    this->objectDoorA->GetComponent<ncPhysicsObj>()->SetPosition( 
        this->originalPositionA - this->pivotA + newpivot );

    // checks if collision in the new position
    nPhyCollide::nContact contact;

    ncPhysicsObj* physicsObject( this->objectDoorA->GetComponent<ncPhysicsObj>() );

    n_assert2( physicsObject, "The object mus have a ncPhysicsObj component." );

    if( physicsObject->Collide( 1, &contact ) )
    {
        // restoring old position
        this->objectDoorA->GetComponent<ncPhysicsObj>()->SetPosition( oldposition );
        this->objectDoorA->GetComponent<ncPhysicsObj>()->SetRotation( oldorientation );
        this->SetNewState( openning );
        return false;
    }

    if( this->angleDone == 0 )
    {
        this->SetNewState( close );
        return true;
    }

    // check not run out of time
    if( this->timeElapsed > this->timeToDoAction )
    {
        this->SetNewState( openning );
        return false;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Mounts a free door

    history:
        - 01-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::MountFreeDoor()
{
    ncTransform *plugA(0),*plugB(0);

    if( this->FakeA )
    {
        plugA = this->FakeA->GetComponent<ncTransform>();

        jointA = static_cast<nPhyHingeJoint*>(
            nKernelServer::Instance()->New( "nphyhingejoint" ));

        n_assert2( jointA, "Failed to create joint." );

        jointA->CreateIn( nPhysicsServer::Instance()->GetDefaultWorld(), 0 );

        jointA->Attach( this->GetDoorEntityA()->GetComponent<ncPhysicsObj>(), 0 );

        jointA->SetAnchor( plugA->GetPosition() );
    
        jointA->SetAxis( plugA->GetTransform().y_component() );

        jointA->SetParam( phy::stopERP, phy::axisA, phyreal(0.2));
        jointA->SetParam( phy::stopCFM, phy::axisA, phyreal(0));

        if( this->GetAttributes() == ncGamePlayDoorClass::free )
        {
            jointA->SetParam( phy::lowerStop, phy::axisA, -this->GetMinAngle() );
            jointA->SetParam( phy::higherStop, phy::axisA, this->GetMaxAngle() );
        }

        jointA->SetParam( phy::bounce, phy::axisA, 0 );
    }
    
    if( this->FakeB )
    {
        plugB = this->FakeB->GetComponent<ncTransform>();

        jointB = static_cast<nPhyHingeJoint*>(
            nKernelServer::Instance()->New( "nphyhingejoint" ));

        n_assert2( jointB, "Failed to create joint." );

        jointB->CreateIn( nPhysicsServer::Instance()->GetDefaultWorld(), 0 );

        jointB->Attach( this->GetDoorEntityB()->GetComponent<ncPhysicsObj>(), 0 );

        jointB->SetAnchor( plugB->GetPosition() );

        jointB->SetAxis( plugB->GetTransform().y_component() );

        jointB->SetParam( phy::stopERP, phy::axisA, phyreal(0.2));
        jointB->SetParam( phy::stopCFM, phy::axisA, phyreal(0));

        if( this->GetAttributes() == ncGamePlayDoorClass::free )
        {
            jointB->SetParam( phy::lowerStop, phy::axisA, this->GetMaxAngle() );
            jointB->SetParam( phy::higherStop, phy::axisA, -this->GetMinAngle() );
        }

        jointB->SetParam( phy::bounce, phy::axisA, 0 );
    }    
}

//-----------------------------------------------------------------------------
/**
    Slidding openning action

    history:
        - 01-Dec-2005   Zombie         created
*/
const bool ncGamePlayDoor::DoSliddingOpenningAction()
{
    this->slidingSpace += this->lastTimeIncrement * this->GetSliddingSpeed();

    vector3 axis( this->FakeA->GetComponent<ncTransform>()->GetTransform().y_component());

    if( this->slidingSpace > this->GetSliddingLenght() )
    {
        this->slidingSpace = this->GetSliddingLenght();
    }

    if( !this->Slide( this->objectDoorA, axis, this->originalPositionA ) )
    {
        this->SetNewState( clossing );
        return false;
    }

    if( this->FakeB )
    {
        axis = this->FakeB->GetComponent<ncTransform>()->GetTransform().y_component();

        if( !this->Slide( this->objectDoorB, axis, this->originalPositionB ) )
        {
            this->SetNewState( clossing );
            return false;
        }
    }

    // checking if done
    if( this->slidingSpace == this->GetSliddingLenght() )
    {
        if( this->GetAttributes() == ncGamePlayDoorClass::closesalone )
        {
            this->SetNewState( waiting );
            return false;
        }
        else
        {
            this->SetNewState( open );
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Slidding closenning action

    history:
        - 01-Dec-2005   Zombie         created
*/
const bool ncGamePlayDoor::DoSliddingClosenningAction()
{
    this->slidingSpace -= this->lastTimeIncrement * this->GetSliddingSpeed();

    vector3 axis( this->FakeA->GetComponent<ncTransform>()->GetTransform().y_component());

    if( this->slidingSpace < 0 )
    {
        this->slidingSpace = 0;
    }

    if( !this->Slide( this->objectDoorA, axis, this->originalPositionA ) )
    {
        this->SetNewState( clossing );
        return false;
    }

    if( this->FakeB )
    {
        axis = this->FakeB->GetComponent<ncTransform>()->GetTransform().y_component();

        if( !this->Slide( this->objectDoorB, axis, this->originalPositionB ) )
        {
            this->SetNewState( clossing );
            return false;
        }
    }

    // checking if done
    if( this->slidingSpace == 0 )
    {
        this->SetNewState( close );
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
    Slides an object with collision

    history:
        - 02-Dec-2005   Zombie         created
*/
const bool ncGamePlayDoor::Slide( nEntityObject* object, const vector3& axis, const vector3& from )
{
    if( !object )
    {
        return true;
    }

    vector3 offset( axis * this->slidingSpace );

    vector3 newposition( offset +  from );

    ncTransform* transformComponent( object->GetComponent<ncTransform>() );

    n_assert2( transformComponent, "The object must have a ncTransform component." );

    vector3 oldposition( transformComponent->GetPosition() );

    transformComponent->SetPosition( newposition );

    // checks if collision in the new position
    nPhyCollide::nContact contact;

    ncPhysicsObj* physicsObject( object->GetComponent<ncPhysicsObj>() );

    n_assert2( physicsObject, "The object mus have a ncPhysicsObj component." );

    if( physicsObject->Collide( 1, &contact ) )
    {
        // restoring old position
        transformComponent->SetPosition( oldposition );
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
/**
    Runs the gameplay logic

    @param time elapsed

    history:
        - 02-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::Run (const float deltaTime)
{
    // @todo: to agree about the time policy

    this->DoAction( deltaTime );

    ncGameplay::Run( deltaTime );
}

//-----------------------------------------------------------------------------
/**
    Unmonts the door

    history:
        - 02-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::UnMount()
{
    this->doorState = close;
    if (this->portal)
    {
        this->portal->SetActiveAndTwin(true);
        ncSpatialPortal *twinPortal = this->portal->GetTwinPortal();
        if (this->GetDoorEntityA())
        {
            this->portal->RemoveBrush(this->GetDoorEntityA());
            if (twinPortal)
            {
                twinPortal->RemoveBrush(this->GetDoorEntityA());
            }
            this->GetDoorEntityA()->GetComponentSafe<ncSpatial>()->SetAssociatedToPortal(false);
        }

        if (this->GetDoorEntityB())
        {
            this->portal->RemoveBrush(this->GetDoorEntityB());
            if (twinPortal)
            {
                twinPortal->RemoveBrush(this->GetDoorEntityB());
            }
            this->GetDoorEntityB()->GetComponentSafe<ncSpatial>()->SetAssociatedToPortal(false);
        }
        this->portal = 0;
    }
    this->Destroy();
}

//-----------------------------------------------------------------------------
/**
    Mounts a hinge door.

    history:
        - 05-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::MountHingeDoor()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Gets the slidding speed

    @return speed in space game units per second

    history:
        - 06-Dec-2005   Zombie         created
*/
const float ncGamePlayDoor::GetSliddingSpeed() const
{
    return this->slidingSpeed;
}

//-----------------------------------------------------------------------------
/**
    Sets the slidding speed

    @param speed speed in space game units per second

    history:
        - 06-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::SetSliddingSpeed( const float speed )
{
    n_assert2( speed != 0, "Speed has to be different of zero." );

    this->slidingSpeed = speed;
}

//-----------------------------------------------------------------------------
/**
    Gets the slidding lenght

    @return lenght in game units

    history:
        - 06-Dec-2005   Zombie         created
*/
const float ncGamePlayDoor::GetSliddingLenght() const
{
    return this->sliddingMax;
}

//-----------------------------------------------------------------------------
/**
    Sets the slidding lenght

    @param lenght lenght in game units

    history:
        - 06-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::SetSliddingLenght( const float length )
{
    this->sliddingMax = length;
}

//-----------------------------------------------------------------------------
/**
    Retrieves the doors from the superentity component

    history:
        - 06-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::RetrieveDoors()
{
    this->SetDoorEntityA(0);

    this->SetDoorEntityB(0);

    ncSuperentity* csentity( this->GetComponentSafe<ncSuperentity>() );
    nObjectInstancer* entitieslist( csentity->GetSubentities() );

    n_assert2( entitieslist, "Data corruption." );

    for( int index(0),counter(0); index < entitieslist->Size(); ++index )
    {
        n_assert(entitieslist->At( index ) != this->GetEntityObject());

        if( !counter )
        {
            this->SetDoorEntityA( static_cast<nEntityObject*>(entitieslist->At( index )) );
            ++counter;
        }
        else
        {
            this->SetDoorEntityB( static_cast<nEntityObject*>(entitieslist->At( index )) );
            break;
        }
    }    

    n_assert2( this->GetDoorEntityA(), "At least one entity was expected." );
}

//-----------------------------------------------------------------------------
/**
    Gets the angle speed

    @return angle speed, degrees per second

    history:
        - 12-Dec-2005   Zombie         created
*/
const float ncGamePlayDoor::GetAngleSpeed() const
{
    return this->angleSpeed;
}

//-----------------------------------------------------------------------------
/**
    Sets the angle speed

    @param speed angle speed, degrees per second

    history:
        - 12-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::SetAngleSpeed( const float speed )
{
    n_assert2( speed != 0, "Speed can't be zero." );

    this->angleSpeed = speed;
}

//-----------------------------------------------------------------------------
/**
    Gets the openning angle

    @return angle in degrees

    history:
        - 12-Dec-2005   Zombie         created
*/
const float ncGamePlayDoor::GetOpeningAngle() const
{
    float angle( this->GetMaxAngle() == 0 ? this->GetMinAngle() : this->GetMaxAngle() );

    // convert from radians to degree
    return angle * (float(360)/(float(2) * float(M_PI)));
}

//-----------------------------------------------------------------------------
/**
    Sets the openning angle

    @param angle angle in degrees

    history:
        - 12-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::SetOpeningAngle( const float angle )
{
    n_assert2( angle != 0, "Angle has to different of zero." );

    float radians( (angle / float(360)) * (float(2) * float(M_PI)) );

    if( radians > 0 )
    {
        this->SetMaxAngle( radians );
        this->SetMinAngle( 0 );
    }
    else
    {
        this->SetMinAngle( radians );
        this->SetMaxAngle( 0 );
    }
}

//-----------------------------------------------------------------------------
/**
    Gets the waiting time b4 closing

    @return time in seconds

    history:
        - 12-Dec-2005   Zombie         created
*/
const float ncGamePlayDoor::GetWaitingTime() const
{
    return this->waitingTime;
}

//-----------------------------------------------------------------------------
/**
    Sets the waiting time b4 closing

    @param time time in seconds

    history:
        - 12-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::SetWaitingTime( const float time )
{
    this->waitingTime = time;
}

//-----------------------------------------------------------------------------
/**
    Decides what to do with the door

    @param time time in seconds

    history:
        - 12-Dec-2005   Zombie         created
*/
void ncGamePlayDoor::ProcessDoor()
{
#ifndef NGAME
    ncSuperentity* csentity( this->GetComponent<ncSuperentity>() );

    bool loaded(false);

    if( csentity )
    {
        loaded = csentity->IsLoaded();
    }
    if( loaded )
    {
#endif
        this->Mount();
        this->Restore();
#ifndef NGAME
    }
    else
    {
        ncGamePlayDoorClass *doorClass( this->GetClassComponent<ncGamePlayDoorClass>() );

        n_assert2( doorClass, "Failed to get the component class." );
        
        this->SetType( doorClass->GetType() );

        this->SetAttributes( doorClass->GetAttributes() );

        this->SetSliddingSpeed( doorClass->GetSliddingSpeed() );

        this->SetSliddingLenght( doorClass->GetSliddingLenght() );

        this->SetAngleSpeed( doorClass->GetAngleSpeed() );
        
        this->SetOpeningAngle( doorClass->GetOpeningAngle() );

        this->SetWaitingTime( doorClass->GetWaitingTime() );
    }
#endif
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
