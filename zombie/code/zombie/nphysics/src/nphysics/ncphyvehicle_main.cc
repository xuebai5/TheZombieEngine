//-----------------------------------------------------------------------------
//  nphycontactjoint_main.cc
//  (C) 2004 Conjurer Services, S.A.
//-----------------------------------------------------------------------------

#include "precompiled/pchnphysics.h"

#include "nphysics/ncphyvehicle.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/ncphywheel.h"
#include "nphysics/ncphyvehicleclass.h"

#include "input/ninputserver.h"

#include "kernel/nlogclass.h"


#include "nlevel/nlevel.h"
#include "nlevel/nlevelmanager.h"

#include "zombieentity/nctransform.h"

#include "kernel/npersistserver.h"

#include "entity/nentityobjectserver.h"

#include "nscene/ncscene.h"

#include "entity/nentityclassserver.h"
#include "nscene/ncsceneclass.h"
#include "zombieentity/ncloaderclass.h"
#include "nspatial/ncspatialclass.h"

#ifndef NGAME
#include "ndebug/nceditor.h"
#include "nlayermanager/nlayermanager.h"
#endif

//-----------------------------------------------------------------------------
nNebulaComponentObject(ncPhyVehicle,ncPhyCompositeObj);

//-----------------------------------------------------------------------------
namespace 
{
    const char *plugNames[] = {
        "frontright",
        "frontleft",
        "backright",
        "backleft"
    };

    const char* const plugSteeringWheelName( "steering_wheel" );

    const float steeringWheelTurning( 3.1415f / 2.f );

    const float SpeedFactorToTriggerLoseOfPower( phyreal(.4) );
    
    const float EnginePowerLeftWhenLoseOfPower( phyreal(.1) );
}

//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 04-Nov-2004   Zombie         created
*/
ncPhyVehicle::ncPhyVehicle() :
    wheels(0),
    assembled(false),
    handBrake(true),
    braking(false),
    steerValue(0),
    accelerateValue(0),
    vehicleParts(NumInitialParts,NumGrowthParts),
    refClass(0),
    steeringWheelOffset(0,0,0),
    maintainedAcceleration(0),
    maintainedTurning(0),
    forcePhysicsAction(true)
{   
    this->jointGroup.Create();
}

//-----------------------------------------------------------------------------
/**
    Destructor.
 
    history:
        - 04-Nov-2004   Zombie         created
*/
ncPhyVehicle::~ncPhyVehicle()
{
    if( this->wheels )
    {
        n_delete_array( this->wheels );

        this->wheels = 0;
    }

    if( this->steeringWheel.isvalid() )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->steeringWheel.get() );
    }

}

//-----------------------------------------------------------------------------
/**
    Loader.

    history:
        - 22-Dec-2005   Zombie         created
*/
void ncPhyVehicle::Load()
{    
    ncPhysicsObj::Load();
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 06-May-2005   Zombie         created
*/
void ncPhyVehicle::InitInstance(nObject::InitInstanceMsg initType)
{
    ncPhyCompositeObj::InitInstance( initType );

    if( initType == nObject::ReloadedInstance )
    {
        for( unsigned index(0); index < this->refClass->GetNumWheels(); ++index )
        {
            this->wheels[index].GetWheelObject()->Reset();
            this->wheels[index].GetSuspension().Reset();
        }
        return;
    }

    ncTransform* transform( this->GetComponent<ncTransform>() );

    transform->DisableUpdate( ncTransform::cPhysics );

    // gets a reference to the component "class"
    this->refClass = this->GetClassComponent<ncPhyVehicleClass>();

    n_assert2( this->refClass, "Failed to obtain the component \"class\"" );

    if( this->wheels )
    {
        // has been already initializated
        return;
    }

    this->wheels = n_new_array( Wheel, this->refClass->GetNumWheels() );

    n_assert2( this->wheels, "Failed to allocate the vehicle wheels" );

    // begin: horrible hack
    nObject* clone(0);

    nPersistServer* pserver(nKernelServer::Instance()->GetPersistServer());

    if( pserver->GetSaveMode() == nPersistServer::SAVEMODE_CLONE )
    {
        clone = pserver->GetClone();

        pserver->SetSaveMode(nPersistServer::SAVEMODE_FOLD);
    }
    // end: horrible hack

    for( unsigned int indexWheel(0); indexWheel < this->refClass->GetNumWheels(); ++indexWheel )
    {
        nEntityObject* eObject(nEntityObjectServer::Instance()->NewLocalEntityObject( this->refClass->GetWheelObjectName(indexWheel).Get() ));

        n_assert2( eObject, "Failed to create the whell object entity." );

        this->wheels[ indexWheel ].SetWheelObject(eObject->GetComponent<ncPhyWheel>());

        this->wheels[ indexWheel ].SetPosition( this->GetWheelPosition( indexWheel ) );

        this->wheels[ indexWheel ].SetTraction( this->refClass->GetHasTraction( indexWheel ) );
        this->wheels[ indexWheel ].SetDirection( this->refClass->GetHasDirection( indexWheel ) );
        this->wheels[ indexWheel ].SetSteeringMax( this->refClass->GetSteeringMax( indexWheel ) );
        this->wheels[ indexWheel ].SetSteeringForce( this->refClass->GetSteeringForce( indexWheel ) );

        eObject->GetComponent<ncPhysicsObj>()->InsertInTheWorld();

#ifndef NGAME
        if( initType == nObject::LoadedInstance || 
            initType == nObject::ReloadedInstance || 
            initType == nObject::ClonedInstance || clone )
        {
            // begin:hardcoded

            // Set current layer to the object
            nLevel* level = nLevelManager::Instance()->GetCurrentLevel();
            n_assert( level );
            nLayerManager* layerManager = static_cast<nLayerManager*>( level->GetEntityLayerManager() );
            n_assert( layerManager );
            ncEditor* editorComp = eObject->GetComponent<ncEditor>();
            if ( editorComp )
            {
                editorComp->SetLayerId( layerManager->GetSelectedLayerId() );
            }
            level->AddEntity( eObject );
            // end:hardcoded 
        }
#endif NGAME
    }
    // begin: horrible hack
    if( clone )
    {
        pserver->SetClone( clone );

        pserver->SetSaveMode(nPersistServer::SAVEMODE_CLONE);
    }
    // end: horrible hack
}


//-----------------------------------------------------------------------------
/**
    Sets the acceleration (-1..0..1).

    @param acceleration grade of acceleration (push)

    history:
        - 04-Nov-2004   Zombie         created
        - 17-Nov-2004   Zombie         First Implementation
*/
void ncPhyVehicle::SetAcceleration( const phyreal acceleration )
{
    n_assert2( this->assembled, "The vehicle it's not ready to work yet" );

    n_assert2( acceleration >= -1, "Value out of range" );
    n_assert2( acceleration <= 1, "Value out of range" );

    this->maintainedAcceleration = acceleration; 
}

//-----------------------------------------------------------------------------
/**
    Sets the steering (-1..0..1).

    @param steering grade of steering (turn)

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::SetSteering( const phyreal steering )
{
    n_assert2( this->assembled, "The vehicle it's not ready to work yet" );
    
    n_assert2( steering >= phyreal(-1), "Value out of range" );
    n_assert2( steering <= phyreal(1), "Value out of range" );

    this->maintainedTurning = steering;
}

//-----------------------------------------------------------------------------
/**
    Get speed in km/h.

    @return speed in km/h

    history:
        - 04-Nov-2004   Zombie         created
*/
phyreal ncPhyVehicle::GetSpeed() const
{
    n_assert2( this->assembled, "The vehicle it's not ready to work yet" );

    vector3 velocity;

    this->GetBody()->GetLinearVelocity( velocity );

    return velocity.len() * phyreal(3.600);
}

//-----------------------------------------------------------------------------
/**
    Builds the vehicle.

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Assemble()
{
    this->assembled = true;

    Wheel* wheel(this->wheels);

    for( unsigned int i(0); i < this->refClass->GetNumWheels(); ++i, ++wheel )
    {
        wheel->SetJointGroup( &this->jointGroup );
    
        ncPhyWheel* wheelObj( wheel->GetWheelObject() );

        wheelObj->SetRotation( 
            this->GetWheelOrientation( i ).x,
            this->GetWheelOrientation( i ).y,
            this->GetWheelOrientation( i ).z );

        wheelObj->SetVehicle( this );

        wheelObj->GetBody()->SetContactWithOtherBodies( false );

        wheel->SetSuspensionRecover( this->refClass->GetSuspensionRecover(i) );

        wheel->SetSuspensionRecoverForce( this->refClass->GetSuspensionRecoverForce(i) );

        // attach the wheels to the vehicle
        wheel->Attach( this->GetBody() );

        wheel->GetWheelObject()->SetFastRotatingObject( true );

        /// setting suspension breakability
        if( this->refClass->GetBreakTreshold(i) != phyreal(-1) )
        {
            wheel->GetSuspension().SetBreakable(true);
            wheel->GetSuspension().SetMaxForceBodyA( this->refClass->GetBreakTreshold(i) );
            wheel->GetSuspension().SetMaxForceBodyB( this->refClass->GetBreakTreshold(i) );
            wheel->GetSuspension().SetMaxTorqueBodyA( this->refClass->GetBreakTreshold(i) );
            wheel->GetSuspension().SetMaxTorqueBodyB( this->refClass->GetBreakTreshold(i) );
        }

        wheelObj->RegisterForPreProcess();
        wheelObj->RegisterForPostProcess();
    }    


    /// registering the object to be processed b4 world runs
    this->RegisterForPreProcess();

    /// registering the object to be processed after world runs
    this->RegisterForPostProcess();

    /// setting impact treshold
    this->ShowImpactWhenForceLargeThan( 100 );

    /// obtains transform
    ncTransform* transform( this->GetComponent<ncTransform>() );

    this->SetPosition(transform->GetPosition());
    const vector3& orientation( transform->GetEuler() );
    this->SetRotation(orientation.x,orientation.y,orientation.z );

    transform->EnableUpdate( ncTransform::cPhysics );

    this->CreateSteeringWheel();
}

//-----------------------------------------------------------------------------
/**
    Set the hand brake set.

    @param isset if the hand brake it's been applied

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::SetHandBrake( const bool isset )
{
    this->handBrake = isset;

    if( !isset )
    {
        this->Enable();
    }
}

//-----------------------------------------------------------------------------
/**
    Process b4 run the world.

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::PreProcess()
{
    for( int index(0); index < this->GetNumGeometries(); ++index )
    {
        if( static_cast<nPhyGeomTrans*>(this->GetGeometry(index))->GetGeometry()->Type() == nPhysicsGeom::TriangleMesh )
        {
            this->GetGeometry(index)->Disable();
        }
        else
        {
            this->GetGeometry(index)->Enable();
        }
    }

#ifndef NGAME
    this->Process( 0, nGfxServer2::Instance() );
#endif
    if( !this->forcePhysicsAction )
    {
        if( this->IsSetHandBrake() )
        {
            if( this->GetSpeed() < phyreal(.001) )
            {
                // disable vehicle
                this->Disable();
                return;
            }
        }
    }
    else
    {
        this->forcePhysicsAction = false;
    }


    this->UpdateWheelsAxis();
    
    this->AntiRollBars();

    this->ApplyAcceleration();

    this->ApplySteer();
}

//-----------------------------------------------------------------------------
/**
    Applies dampening.

    history:
        - 21-Apr-2005   Zombie         created
        - 10-Feb-2006   Zombie         removed resistance on the wheels
*/
void ncPhyVehicle::Dampening()
{
    vector3 velocity;

    phyreal resistance(-this->refClass->GetAirForce()*this->refClass->GetAirResistance());

    // over the vehicle
    this->GetLinearVelocity( velocity );

    this->AddForce( velocity * resistance );
    
    this->GetAngularVelocity( velocity );

    this->AddTorque( velocity * resistance );    
}

//-----------------------------------------------------------------------------
/**
    Updates all vehicle wheels axis.

    history:
        - 01-Apr-2005   Zombie         created
*/
void ncPhyVehicle::UpdateWheelsAxis()
{
    for( unsigned int index(0); index < this->refClass->GetNumWheels(); ++index ) 
    {
        Wheel* wheel( &this->wheels[index] );
        wheel->AdjustRotationAxis();
    }    
}

//-----------------------------------------------------------------------------
/**
    Returns if the hand brake is set.

    @return if it's set

    history:
        - 04-Nov-2004   Zombie         created
*/
const bool ncPhyVehicle::IsSetHandBrake() const
{
    return this->handBrake;
}

//-----------------------------------------------------------------------------
/**
    Acces to the wheel.

    @param index index to the wheel
    @return a wheel

    history:
        - 04-Nov-2004   Zombie         created
*/
ncPhyVehicle::Wheel* ncPhyVehicle::operator [] (unsigned int index) const
{
    n_assert2( index < this->refClass->GetNumWheels(), "Index out of bounds" );

    return &this->wheels[ index ];
}

//-----------------------------------------------------------------------------
/**
    Acces to the steering wheel.

    @return steering wheel object

    history:
        - 05-Mar-2006   MA Garcias      created
*/
nEntityObject* ncPhyVehicle::GetSteeringWheel() const
{
    return this->steeringWheel.isvalid() ? this->steeringWheel.get() : 0;
}

//-----------------------------------------------------------------------------
/**
    Returns access to the vehicle joint group.

    @return the vehicles joint group

    history:
        - 04-Nov-2004   Zombie         created
*/
nPhyJointGroup* ncPhyVehicle::GetJointGroup()
{
    return &jointGroup;
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Process car.

    @param camera

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Process( matrix44* /*camera*/, nGfxServer2* server )
{
#ifndef NGAME
    char buffer[128];
    sprintf( buffer, "Desired Speed: %.2f", this->accelerateValue );
    server->Text( buffer, vector4(1,0,0,1),.7f,-.3f );
            
    sprintf( buffer, "Steer: %.2f", this->steerValue );
    server->Text( buffer, vector4(1,0,0,1),.7f,-.2f );

    sprintf( buffer, "Speed: %.2f (km/h)", this->GetSpeed() );
    server->Text( buffer, vector4(1,0,0,1),.7f,-.1f );

    sprintf( buffer, "Braking: %s", this->braking ? "True" : "False" );
    server->Text( buffer, vector4(1,0,0,1),.7f,.0f );
#endif
    // end: temporary code
}
#endif
//-----------------------------------------------------------------------------
/**
    Sets the position of the vehicle.

    @param newposition world position

    history:
        - 08-Nov-2004   Zombie         created
*/
void ncPhyVehicle::SetPosition( const vector3& newposition )
{
    ncPhyCompositeObj::SetPosition( newposition );

    if( this->refClass )
    {
        for( unsigned int i(0); i < this->refClass->GetNumWheels(); ++i )
        {
            this->wheels[i].SetWorldPosition( newposition );
        }
    }

    this->UpdateSteeringWheel();

    this->forcePhysicsAction = true;
}

//-----------------------------------------------------------------------------
/**
    Returns the bhps per wheel.

    history:
        - 04-Nov-2004   Zombie         created
*/
phyreal ncPhyVehicle::GetBHPsPerWheel() const
{
    int counterTractionWheels(0);

    for( unsigned int i(0); i < this->refClass->GetNumWheels(); ++i )
    {
        if( this->wheels[i].HasTraction() ) 
        {
            ++counterTractionWheels;
        }
    }

    return this->refClass->GetBHPs() / phyreal(counterTractionWheels);
}

#ifndef NGAME
//-----------------------------------------------------------------------------
/**
    Sets camera respect the vehicle position.

    @param camera camera to be used

    history:
        - 19-Nov-2004   Zombie         created
*/
void ncPhyVehicle::SetCamera( matrix44* camera )
{
    // begin: temporary code
    static phyreal cameraYIncrement(phyreal(3));
    static phyreal cameraRearIncrement(phyreal(10));
    static phyreal farPointOfView(phyreal(10));
    static phyreal ignoreSpeed(phyreal(0));

    if( !camera )
    {
        return;
    }

    vector3 vehiclePosition;

    this->GetPosition( vehiclePosition );

    vector3 velocity;

    this->GetBody()->GetLinearVelocity( velocity );

    if( velocity.len() < ignoreSpeed )
        return;

    velocity.norm();

    vector3 cameraPosition( velocity * -cameraRearIncrement );

    cameraPosition += vehiclePosition;

    cameraPosition.y += cameraYIncrement;

    //camera->set_translation( (cameraPosition + camera->pos_component()) / phyreal(2) );
    camera->set_translation( cameraPosition  );

    camera->lookatRh( vehiclePosition + velocity * farPointOfView, vector3( 0, 1, 0 ) );
    // end: temporary code
}

#endif // NGAME

//-----------------------------------------------------------------------------
/**
    Apply the acceleration (in engine time).

    history:
        - 17-Dec-2004   Zombie         created
*/
void ncPhyVehicle::ApplyAcceleration()
{
    this->ComputeInput();

    const int NumWheels( this->refClass->GetNumWheels() );

    Wheel* ptrWheel( wheels );

    if( this->GetAccelerateValue() > this->refClass->GetMaxRevolution() )
    {
        this->accelerateValue = this->refClass->GetMaxRevolution();
    }
    else
    {
        if( this->GetAccelerateValue() < -this->refClass->GetMaxRevolution() )
            this->accelerateValue = -this->refClass->GetMaxRevolution();
    }

    if( !this->braking )
    {
        phyreal bhps( this->accelerateValue*this->GetBHPsPerWheel()/this->refClass->GetMaxRevolution() + phyreal(2.0) );

        phyreal speed( this->accelerateValue );

        if( speed > 0 )
        {
            bhps *= this->refClass->GetBackwardsResistance();
        }

        if( speed < 0 )
        {
            speed = -speed;
            bhps = -bhps;
        }

        if( this->IsSetHandBrake() )
        {
            bhps = phyInfinity;
            speed = 0;
        }
        else
        {
            // hack: to avoid the vehicle going to stiff slopes
            vector3 velocity;
            
            this->GetLinearVelocity( velocity );

            velocity.norm();

            if( velocity.y > SpeedFactorToTriggerLoseOfPower )
            {
                bhps *= EnginePowerLeftWhenLoseOfPower;
            }
        }

        for( int i(0); i < NumWheels; ++i, ++ptrWheel )
        {
            ptrWheel->SetSpeed( this->accelerateValue, bhps );
        }
    }
    else
    {
        // braking
        for( int i(0); i < NumWheels; ++i, ++ptrWheel )
        {
            ptrWheel->SetSpeed( 0, this->refClass->GetBreakingForce() );
        }
    }

    if( abs(this->accelerateValue) < phyreal(.0001) )
    {
        this->accelerateValue = 0;
    }
    else
    {
        const phyreal resistance( nPhysicsServer::Proportion(this->refClass->GetEngineResistance() ) );

        /// motor deceleration
        if( this->accelerateValue > 0 )
        {            
            if( resistance > this->accelerateValue )
            {
                this->accelerateValue = 0;
            }
            else
            {
                this->accelerateValue -=  resistance;
            }
        }
        else
        {
            if( resistance > (phyreal(-1) * this->accelerateValue) )
            {
                this->accelerateValue = 0;
            }
            else
            {
                this->accelerateValue +=  resistance;
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Apply the steering (in engine time).

    history:
        - 17-Dec-2004   Zombie         created
*/
void ncPhyVehicle::ApplySteer() 
{
    Wheel* wheel(wheels); 

    // turn reduction/increse for speed

    phyreal factor( fabs(this->GetSpeed() / this->refClass->GetSpeedMaxSteeringForced()) );

    if( factor > phyreal(1) )
    {
        factor = phyreal(1);
    }

    factor = phyreal(1) - (this->refClass->GetPercentageMaxSteering() * factor);

    for( unsigned int i(0); i < this->refClass->GetNumWheels(); ++i, ++wheel )
    {
        if( wheel->IsDirection() )
        {
            wheel->SetSteering( wheel->GetMaxSteering() * this->steerValue * factor );
            
            if( abs(this->steerValue) < phyreal(.0001) )
            {
                this->steerValue = 0;
            }
            else
            {
                /// steer correction
                if( this->steerValue > 0 )
                {
                    this->steerValue -=  nPhysicsServer::Proportion(this->refClass->GetSteerRecoverRatio());
                }
                else
                {
                    this->steerValue +=  nPhysicsServer::Proportion(this->refClass->GetSteerRecoverRatio());
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Creates the object.

    @param world it's the world where the object will be come to existance

    history:
        - 20-Dec-2004   Zombie         created
*/
void ncPhyVehicle::Create( nPhysicsWorld* world )
{
    if( !this->refClass )
    {
        return;
    }

    this->AllowCreateSubSpace( false );

    n_assert2( world, "Null pointer" );

    ncPhysicsObj::Create( world );

    nPhyRigidBody *body(this->GetBody());

    if( body )
    {
        this->SetBody( body );

        vector3 position;

        this->GetPosition(position);

        this->GetBody()->SetPosition(position);

        /// assigns to each geometry de body
        body->Update( this->containerGeometries, GetMass(), GetDensity(), this->refClass->GetGravityCenterOffset() );

        body->SetPhysicsObj( this );

        body->SetIfAtmospheraAffected( false );
    }

    this->Assemble();

    this->ShowImpactWhenForceLargeThan(1);
}

//-----------------------------------------------------------------------------
/**
    Resets the physics.

    history:
        - 30-Mar-2005   Zombie         created
*/
void ncPhyVehicle::Reset()
{
    for( unsigned int i(0); i < this->refClass->GetNumWheels(); ++i )
    {
        this->wheels[i].GetWheelObject()->Reset();
    }

    ncPhysicsObj::Reset();
}

//-----------------------------------------------------------------------------
/**
    Function call when an impact ocurs.

    @param info impact information

    history:
        - 30-Mar-2005   Zombie         created
*/
void ncPhyVehicle::OnImpact( const tImpactInfo& info )
{
    const breakablePart* part(0);

    phyreal forceImpact(info.impact.len());

    for( int index(0); index < this->vehicleParts.Size(); ++index )
    {
        part = &this->vehicleParts[index];
        if( part->where == info.geom )
        {
            if( part->tresholdForce < forceImpact )
            {
                /// @todo: Trigger Impact sending a signal to the entity and ID of the part
                NLOG( physicsLog , (1, "# Impact in vehicle on: (%.2f,%.2f,%2.f) with a vector force of (%.2f,%.2f,%.2f) and a total force of: %.2f in piece %ld.", info.point.x, info.point.y,info.point.z, info.impact.x, info.impact.y, info.impact.z, info.impact.len(), index) );
            }
            return;
        }
    }

    /// @todo: Trigger any impact and position

    /// setting the engine revolutions to stop
    this->accelerateValue = 0;
}

//-----------------------------------------------------------------------------
/**
    Add a breakable part.

    @param where which geometric part
    @param force force threshold in newtons

    history:
        - 30-Mar-2005   Zombie         created
*/
void ncPhyVehicle::BreakableAt( nPhysicsGeom* where, phyreal force )
{
    n_assert2( where, "Null pointer." );

    n_assert2( force >= 0, "Force can't be negative." );

    // TODO: Check if the geometry belongs to the car body    
    breakablePart part;

    part.tresholdForce = force;
    part.where = where;

    // TODO: Check if the part it's already in the list
    this->vehicleParts.PushBack( part );

    // Updating impact treshold
    if( this->GetImpactTreshold() > force )
    {
        this->ShowImpactWhenForceLargeThan( force );
    }
}

//-----------------------------------------------------------------------------
/**
    Checks if the vehicle it's flipped.

    @return true/false

    history:
        - 30-Mar-2005   Zombie         created
*/
bool ncPhyVehicle::ItsFlipped() const
{
    n_assert2( this->assembled, "It can not be perform until the vehicle is assembled." );

    matrix33 vehicleOrientation;

    this->GetBody()->GetOrientation( vehicleOrientation );

    vector3 verticalOrientation( vehicleOrientation.y_component() );

    if( verticalOrientation.y < 0 )
        return true;

    // NOTE: May be, will need more information

    return false;
}

//-----------------------------------------------------------------------------
/**
    Try's to unflip the vehicle.

    history:
        - 30-Mar-2005   Zombie         created
*/
void ncPhyVehicle::TryUnflip()
{
    n_assert2( this->assembled, "It can not be perform until the vehicle is assembled." );

    /// @todo: do a better implementation this is just a temporary fix.
    this->SetRotation( 0,0,0 );
}


//-----------------------------------------------------------------------------
/**
    Applies anti-roll bars.

    history:
        - 20-Apr-2005   Zombie         created
*/
void ncPhyVehicle::AntiRollBars()
{
    /// @todo: Check out a better way, or at least improve computation speed.
    vector3 bodyPoint;
    vector3 hingePoint;
    vector3 axis;
    vector3 position;
    
    phyreal angle;

    for( unsigned int index(0); index < this->refClass->GetNumWheels(); ++index ) 
    {
        nPhyTwoHingeJoint& suspension(this->wheels[index].GetSuspension());

        suspension.GetSecondAnchor( bodyPoint );
        suspension.GetFirstAnchor( hingePoint );
        suspension.GetFirstAxis( axis );
      
        angle = vector3(hingePoint - bodyPoint).dot(axis);        
        
        phyreal amt(angle * this->refClass->GetAntiRollForce());

        if( angle > 0 ) 
        {
            if( amt > this->refClass->GetMaxAntiRollForce() ) 
            {
                amt = this->refClass->GetMaxAntiRollForce();
            }

            this->wheels[index].GetWheelObject()->AddForce( vector3(axis*-amt) );

            this->wheels[index].GetWheelObject()->GetPosition( position );

            this->AddForceAtPosition( vector3( axis*amt), position );

            this->wheels[index^1].GetWheelObject()->AddForce( vector3(axis * amt) );

            this->wheels[index].GetWheelObject()->GetPosition( position );
            
            this->AddForceAtPosition( vector3( axis * -amt ), position );
        }
        
    }
}


//-----------------------------------------------------------------------------
/**
    Process after run the world.

    history:
        - 21-Apr-2005   Zombie         created
*/
void ncPhyVehicle::PostProcess()
{
    this->Dampening();

    this->UpdateSteeringWheel();

    for( int index(0); index < this->GetNumGeometries(); ++index )
    {
        if( static_cast<nPhyGeomTrans*>(this->GetGeometry(index))->GetGeometry()->Type() == nPhysicsGeom::TriangleMesh )
        {
            this->GetGeometry(index)->Enable();
        }
        else
        {
            this->GetGeometry(index)->Disable();
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Sets a wheel object.

    @param wheelIndex which wheel
    @param wheelObject a wheel object

    history:
        - 06-May-2005   Zombie         created
*/
void ncPhyVehicle::SetWheelObject(const unsigned int wheelIndex, ncPhyWheel* wheelObject )
{
    n_assert2( wheelIndex < this->refClass->GetNumWheels(), "Index out of bounds." );
    n_assert2( wheelObject, "Null object" );

    this->wheels[ wheelIndex ].SetWheelObject( wheelObject );
}

//------------------------------------------------------------------------------
/**
	Scales the object.
    
    @param factor scale factor

    history:
     - 12-May-2005   Zombie         created
*/
void ncPhyVehicle::Scale( const phyreal factor )
{
    /// scaling chasis
    ncPhyCompositeObj::Scale( factor );

    if( !this->wheels )
    {
        return;
    }

    /// scaling wheels
    for( unsigned int index(0); index < this->refClass->GetNumWheels(); ++index )
    {
        this->wheels[ index ].Scale( factor );
    }
}

//------------------------------------------------------------------------------
/**
	Set piece treshold.

    @param index geometry index
    @param threshold force in newtons
    
    history:
     - 10-Aug-2005   Zombie         created
*/
void ncPhyVehicle::SetPieceImpact( int index,  phyreal treshold )
{
    nPhyGeomTrans* trans( static_cast<nPhyGeomTrans*>(this->GetGeometry( index )));

    for( int inner(0); inner < this->vehicleParts.Size(); ++inner )
    {
        if( this->vehicleParts[ inner ].where == trans->GetGeometry() )
        {
            this->vehicleParts[ inner ].tresholdForce = treshold;
            return;
        }
    }

    this->BreakableAt( trans->GetGeometry(), treshold );
}


#ifndef NGAME

//------------------------------------------------------------------------------
/**
	Returns the number of geometries.

    @return geometries number
    
    history:
     - 10-Aug-2005   Zombie         created
*/
int ncPhyVehicle::GetNumberPieces() const
{
    return  this->GetNumGeometries();
}

//------------------------------------------------------------------------------
/**
	Marks a geometry.
    
    @param index geometry index

    history:
     - 10-Aug-2005   Zombie         created
*/
void ncPhyVehicle::MarkPiece(int index)
{
    this->GetGeometry( index )->Mark();
}

//------------------------------------------------------------------------------
/**
	Unmarks a geometry.
    
    @param index geometry index

    history:
     - 10-Aug-2005   Zombie         created
*/
void ncPhyVehicle::UnMarkPiece(int index)
{
    this->GetGeometry( index )->UnMark();
}
#endif


//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 04-Nov-2004   Zombie         created
*/
ncPhyVehicle::Wheel::Wheel() :
    direction(false),
    traction(false),
    jointGroup(0),
    position(0,0,0),
    suspensionDirection(0,1,0),
    wheelAxis(1,0,0),
    attached(false),
    maxSteering(0),
    forceSteering(0),
    steeringVelocity(1),
    suspensionRecoverForce(phyreal(0.01)), //.04
    suspensionRecover(phyreal(0.25)), //.15
    suspension(0)
{
    this->suspension = static_cast<nPhyTwoHingeJoint*>(nKernelServer::Instance()->New( "nphytwohingejoint" ));

    n_assert2( this->suspension, "Failed to allocate memory." );
}

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 04-Nov-2004   Zombie         created
*/
ncPhyVehicle::Wheel::~Wheel()
{
    if( this->GetWheelObject() )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->GetWheelObject()->GetEntityObject() );
    }

    if( this->suspension )
    {
        this->suspension->Release();
    }

}

//-----------------------------------------------------------------------------
/**
    Sets if the wheel has direction.

    @param dir if it has direction

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetDirection( const bool dir )
{
    this->direction = dir;
}

//-----------------------------------------------------------------------------
/**
    Sets if the wheel has traction.

    @param tract if it has traction

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetTraction( const bool tract )
{
    this->traction = tract;
}

//-----------------------------------------------------------------------------
/**
    Set wheel position relative to the vehicle position.

    @param pos wheel position

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetPosition( const vector3& pos )
{
    this->position = pos;
}

//-----------------------------------------------------------------------------
/**
    Sets suspension direction.

    @param direction suspension direction

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetSpringDirection( const vector3& direction )
{
    n_assert2( this->attached == false, "Cannot be chanced once attached to the car" );

    this->suspensionDirection = direction;
}

//-----------------------------------------------------------------------------
/**
    Set wheel axis direction.

    @param direction wheel axis direction

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetWheelAxis( const vector3& direction )
{
    n_assert2( this->attached == false, "Cannot be chanced once attached to the car" );
    
    this->wheelAxis = direction;
}

//-----------------------------------------------------------------------------
/**
    Returns the max steering for this wheel.

    @return max steering (0..1)

    history:
        - 04-Nov-2004   Zombie         created
*/
const phyreal ncPhyVehicle::Wheel::GetMaxSteering() const
{
    return this->maxSteering;
}

//-----------------------------------------------------------------------------
/**
    Set wheel object.

    @param wheelobj physics object representing the wheel

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetWheelObject( ncPhyWheel* wheelobj )
{
    n_assert2( this->attached == false, "Cannot be chanced once attached to the car" );
    
    this->wheelObject = wheelobj->GetEntityObject();
}

//-----------------------------------------------------------------------------
/**
    Returns if it can be steer.

    @return if it can be steer

    history:
        - 04-Nov-2004   Zombie         created
*/
const bool ncPhyVehicle::Wheel::IsDirection() const
{
    return this->direction;
}

//-----------------------------------------------------------------------------
/**
    Returns if it has traction.

    @return if it has traction

    history:
        - 04-Nov-2004   Zombie         created
*/
const bool ncPhyVehicle::Wheel::HasTraction() const
{
    return this->traction;
}

//-----------------------------------------------------------------------------
/**
    Returns the wheel object.

    @return physics object representing the wheel

    history:
        - 04-Nov-2004   Zombie         created
*/
ncPhyWheel*  ncPhyVehicle::Wheel::GetWheelObject() const
{
    if( !this->wheelObject.isvalid() )
    {
        return 0;
    }
    return this->wheelObject->GetComponent<ncPhyWheel>();
}

//-----------------------------------------------------------------------------
/**
    Returns if the wheel it's slidding.

    @return if it's sliding

    history:
        - 04-Nov-2004   Zombie         created
*/
const bool ncPhyVehicle::Wheel::IsSliding() const
{
    // TODO: Implementation.
    return false;
}

//-----------------------------------------------------------------------------
/**
    Sets if the wheels are a set of track wheels.

    @param is if is a track wheel

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetTrackWheel( const bool /*is*/ )
{
    // TODO: Implement if needed
}

//-----------------------------------------------------------------------------
/**
    Sets the max steering value.

    @param max how much can be steered

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetSteeringMax( const phyreal max )
{
    n_assert2( max >= 0, "Max steering cannot be negative." );
    n_assert2( max <= 1, "Max steering cannot be bigger than 1." );

    this->maxSteering = max;
}

//-----------------------------------------------------------------------------
/**
    Attaches the wheel to the body.

    @param body where to be attached the wheel

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::Attach( nPhyRigidBody* body )
{
    // If null pointer, means unattach the wheel
    
    n_assert2( this->jointGroup != 0, "No joint available" );

    n_assert2( this->wheelObject != 0, "Wheel object not available" );

    n_assert2( this->jointGroup != 0, "Joint group missing" );

    if( !body )
    {
        this->attached = false;

        this->suspension->Attach( static_cast<nPhyRigidBody*>(0), 0 );

        return;
    }

    // setting the relative position of the object
    this->GetWheelObject()->SetPosition( this->position );

    if( this->suspension->Id() == NoValidID )
    {
        // creating the suspension
        this->suspension->CreateIn( body->GetWorld(), this->jointGroup );
    }

    // attaching with the suspension the body and the wheel object
    this->suspension->Attach( body, this->GetWheelObject()->GetBody() );

    // setting the anchor point
    this->suspension->SetAnchor( this->position );

    // setting the suspension direction
    this->suspension->SetFirstAxis( this->suspensionDirection );

    // setting the wheel axis
    this->suspension->SetSecondAxis( this->wheelAxis );

    // setting the suspension softness
    this->suspension->SetParam(phy::suspensionERP, phy::axisA, this->GetSuspensionRecover() );
    this->suspension->SetParam(phy::suspensionCFM, phy::axisA, this->GetSuspensionRecoverForce() );

    // blocking the direction of all the wheels by default
    this->suspension->SetParam(phy::lowerStop, phy::axisA, 0);
    this->suspension->SetParam(phy::higherStop, phy::axisA, 0);

    this->suspension->SetParam(phy::lowerStop, phy::axisA, 0);
    this->suspension->SetParam(phy::higherStop, phy::axisA, 0);

    // set suspension limits
    this->suspension->SetParam(phy::stopERP, phy::axisA, phyreal(0.99));
    this->suspension->SetParam(phy::stopCFM, phy::axisA, phyreal(.001));

    this->attached = true;
}

//-----------------------------------------------------------------------------
/**
    Sets the joint group to be used.

    @param jg jointgroup

    history:
        - 04-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetJointGroup( nPhyJointGroup *jg )
{
    this->jointGroup = jg;
}

//-----------------------------------------------------------------------------
/**
    Sets the torque speed.

    @param speed (angular speed)

    history:
        - 05-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetSpeed( phyreal speed, const phyreal bhps )
{
    if( !this->traction )
    {
        return;
    }
    
    this->suspension->SetParam( phy::maxForce, phy::axisB, bhps );
    this->suspension->SetParam( phy::velocity, phy::axisB, speed );
}

//-----------------------------------------------------------------------------
/**
    Set the steering.

    @param steering how much

    history:
        - 05-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetSteering( const phyreal steering )
{
    if( !this->direction )
    {
        return;
    }

    this->suspension->SetParam( phy::velocity, phy::axisA, this->GetSteeringVelocity() );
    this->suspension->SetParam( phy::maxForce, phy::axisA, this->GetSteeringForce() );
    this->suspension->SetParam( phy::lowerStop, phy::axisA, steering );
    this->suspension->SetParam( phy::higherStop, phy::axisA, steering );
    this->suspension->SetParam( phy::lowerStop, phy::axisA, steering );
    this->suspension->SetParam( phy::higherStop, phy::axisA, steering );
}

//-----------------------------------------------------------------------------
/**
    Sets the steering velocity.

    @param value    steering velocity

    history:
        - 20-Dec-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetSteeringVelocity( phyreal value )
{
    this->steeringVelocity = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the steering velocity.

    @return    steering velocity

    history:
        - 20-Dec-2004   Zombie         created
*/
phyreal ncPhyVehicle::Wheel::GetSteeringVelocity() const
{
    return this->steeringVelocity;
}

//-----------------------------------------------------------------------------
/**
    Sets the position given another.

    @param steering how much

    history:
        - 05-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetWorldPosition( const vector3& pos )
{
    n_assert2( this->wheelObject, "Null pointer" );


    this->GetWheelObject()->SetPosition( this->position + pos );
}

//-----------------------------------------------------------------------------
/**
    Sets steering force (0..1).

    @param value steering force

    history:
        - 22-Nov-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetSteeringForce( const phyreal value )
{
    n_assert2( value >= 0, "Steering force cannot be negative." );
    n_assert2( value <= 1, "Steering force cannot be bigger than one." );

    this->forceSteering = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the steering force.

    @return value steering force

    history:
        - 22-Nov-2004   Zombie         created
*/
phyreal ncPhyVehicle::Wheel::GetSteeringForce() const
{
    return this->forceSteering;
}

//-----------------------------------------------------------------------------
/**
    Sets the suspension recover force.

    @param value (0..1)  1...max recovery

    history:
        - 20-Dec-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetSuspensionRecoverForce( phyreal value )
{
    n_assert2( this->attached == false, "This operation can only be performed b4 the wheel attachment" );

    n_assert2( value > 0, "This value cannot be negative" );

    n_assert2( value <= 1, "This value cannot be bigger than 1" );

    this->suspensionRecoverForce = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the suspension recover force.

    @return (0..1)  1...max recovery

    history:
        - 20-Dec-2004   Zombie         created
*/
phyreal ncPhyVehicle::Wheel::GetSuspensionRecoverForce() const
{
    return this->suspensionRecoverForce;
}

//-----------------------------------------------------------------------------
/**
    Sets the suspension recover.

    @param value (0..1)  1...max recovery

    history:
        - 20-Dec-2004   Zombie         created
*/
void ncPhyVehicle::Wheel::SetSuspensionRecover( phyreal value )
{
    n_assert2( this->attached == false, "This operation can only be performed b4 the wheel attachment" );

    n_assert2( value > 0, "This value cannot be negative" );

    n_assert2( value <= 1, "This value cannot be bigger than 1" );

    this->suspensionRecover = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the suspension recover.

    @return (0..1)  1...max recovery

    history:
        - 20-Dec-2004   Zombie         created
*/
phyreal ncPhyVehicle::Wheel::GetSuspensionRecover() const
{
    return this->suspensionRecover;
}

//-----------------------------------------------------------------------------
/**
    Adjust rotation axis.

    history:
        - 01-Apr-2005   Zombie         created
*/
void ncPhyVehicle::Wheel::AdjustRotationAxis()
{
    vector3 axis;
    
    this->suspension->GetSecondAxis( axis );

    phySetFastRotationAxis( this->GetWheelObject()->GetBody()->Id(), axis );    
}

//-----------------------------------------------------------------------------
/**
    Returns the suspension.

    @return suspension joint

    history:
        - 21-Apr-2005   Zombie         created
*/
nPhyTwoHingeJoint& ncPhyVehicle::Wheel::GetSuspension()
{
    return *this->suspension;
}

//------------------------------------------------------------------------------
/**
	Scales the object.

    @param factor scale factor
    
    history:
     - 12-May-2005   Zombie         created
*/
void ncPhyVehicle::Wheel::Scale( const phyreal factor )
{
    this->GetWheelObject()->Scale( factor );
}


#ifndef NGAME
//------------------------------------------------------------------------------
/**
	Draws the physic object.

    history:
     - 18-Aug-2005   Zombie         created
*/
void ncPhyVehicle::Draw( nGfxServer2* server )
{
    ncPhysicsObj::Draw( server );    

    if( nPhysicsServer::Instance()->GetDraw() & nPhysicsServer::phyJoints )
    {
        for( int index(0); index < this->jointGroup.GetNumJoints(); ++index )
        {
            this->jointGroup.GetJoint( index )->Draw( server );
        }
    }
}

#endif // !NGAME

//------------------------------------------------------------------------------
/**
	Sets the orientation of the rigid body.

*/
void ncPhyVehicle::SetRotation( const matrix33& neworientation )
{
    vector3 euler(neworientation.to_euler());

    this->SetRotation( euler.x, euler.y, euler.z );
}

//------------------------------------------------------------------------------
/**
	Sets the euler orientation.

*/
void ncPhyVehicle::SetRotation( phyreal ax, phyreal ay, phyreal az )
{
    if( this->refClass )
    {
        vector3 positionVehicle;

        this->GetPosition( positionVehicle );

        matrix33 orientation;

        orientation.from_euler( vector3( ax, ay, az ) );

        // rotation the wheels
        for( unsigned index(0); index < this->refClass->GetNumWheels(); ++index )
        {
            ncPhyWheel* wheel(this->wheels[ index ].GetWheelObject());

            matrix33 matrixWheel;

            matrixWheel.from_euler( this->GetWheelOrientation(index) );

            matrixWheel = matrixWheel * orientation;

            vector3 angles( matrixWheel.to_euler() );

            wheel->SetRotation( angles.x, 
                angles.y, 
                angles.z );

            vector3 wheelPosition;

            wheel->GetPosition( wheelPosition );

            vector3 offset;

            offset = wheelPosition - positionVehicle;

            offset = orientation * offset;

            wheelPosition = offset + positionVehicle;

            wheel->SetPosition( wheelPosition );
        }
    }

    matrix33 neworientation;

    neworientation.from_euler( vector3(ax, ay, az) );

    ncPhyCompositeObj::SetRotation( neworientation );
}

//------------------------------------------------------------------------------
/**
	Sets the object it's been load or a clone.

*/
void ncPhyVehicle::SetLoadOrClone()
{
    n_assert2( this->refClass == 0, "It's already been initializated." );

    this->InitInstance( nObject::LoadedInstance );
}

//------------------------------------------------------------------------------
/**
	Sets the object position by scripting.

*/
void ncPhyVehicle::SetPositionPhyObj( const vector3& /*newpos*/ )
{
    // empty

}

//------------------------------------------------------------------------------
/**
	Checks if collision with other physics object.

    history:
     - 15-Sep-2005   Zombie         created
   
*/
int ncPhyVehicle::Collide( const ncPhysicsObj* obj,
    int numContacts, nPhyCollide::nContact* contact ) const
{
    int hits(ncPhyCompositeObj::Collide( obj, numContacts, contact ));

    Wheel *wheel(this->wheels);

    numContacts -= hits;

    for( unsigned index(0); index < this->refClass->GetNumWheels(); ++index, ++wheel )
    {
        if( !numContacts )
            break;

        hits += wheel->GetWheelObject()->Collide( obj, numContacts, &contact[hits] );
    }

    return hits;
}


//------------------------------------------------------------------------------
/**
	Checks if collision with a geometry.

    history:
     - 15-Sep-2005   Zombie         created
   
*/
int ncPhyVehicle::Collide( const nPhysicsGeom* geom,
    int numContacts, nPhyCollide::nContact* contact ) const
{
    int hits(ncPhyCompositeObj::Collide( geom, numContacts, contact ));

    Wheel *wheel(this->wheels);

    numContacts -= hits;

    for( unsigned index(0); index < this->refClass->GetNumWheels(); ++index, ++wheel )
    {
        if( !numContacts )
            break;

        hits += wheel->GetWheelObject()->Collide( geom, numContacts, &contact[hits] );
    }

    return hits;
}

//------------------------------------------------------------------------------
/**
	Checks if the object collides with anything in his own world.

    history:
     - 15-Sep-2005   Zombie         created
   
*/
int ncPhyVehicle::Collide( int numContacts, nPhyCollide::nContact* contact ) const
{
    int hits(ncPhyCompositeObj::Collide( numContacts, contact ));

    Wheel *wheel(this->wheels);

    numContacts -= hits;

    for( unsigned index(0); index < this->refClass->GetNumWheels(); ++index, ++wheel )
    {
        if( !numContacts )
            break;

        hits += wheel->GetWheelObject()->Collide( numContacts, &contact[hits] );
    }

    return hits;
}

#ifndef NGAME

//------------------------------------------------------------------------------
/**
	Moves the object to limbo.

    history:
     - 16-Sep-2005   Zombie         created
   
*/
void ncPhyVehicle::YouShallDwellIntoTheLimbo()
{
    ncPhysicsObj::YouShallDwellIntoTheLimbo();

    for( unsigned index(0); index < this->refClass->GetNumWheels(); ++index )
    {
        this->wheels[ index ].GetWheelObject()->GetEntityObject()->SendToLimbo( this->wheels[ index ].GetWheelObject()->GetEntityObject() );
    }

    // steering wheel
    this->steeringWheel->SendToLimbo(this->steeringWheel);
}

//------------------------------------------------------------------------------
/**
	Recovers an object from the limbo.

    history:
     - 16-Sep-2005   Zombie         created
   
*/
void ncPhyVehicle::YourSoulMayComeBackFromLimbo()
{
    this->steeringWheel->ReturnFromLimbo(this->steeringWheel);

    for( unsigned index(0); index < this->refClass->GetNumWheels(); ++index )
    {
        this->wheels[ index ].GetWheelObject()->GetEntityObject()->ReturnFromLimbo( this->wheels[ index ].GetWheelObject()->GetEntityObject() );
    }

    ncPhysicsObj::YourSoulMayComeBackFromLimbo();

}

#endif // !NGAME

//------------------------------------------------------------------------------
/**
	Helper function to get the wheel position.

    @param index wheel index
    @return the position

    history:
     - 22-Dec-2005   Zombie         created
   
*/
const vector3 ncPhyVehicle::GetWheelPosition( const unsigned int index ) const
{
    ncSceneClass* sclass(this->GetEntityObject()->GetClassComponent<ncSceneClass>());

    vector3 position;

    quaternion orientation;

    sclass->GetPlugData( plugNames[ index ], position, orientation );

    return position;
}

//------------------------------------------------------------------------------
/**
	Helper function to get the wheel orientation.

    @param index wheel index
    @return the orientation

    history:
     - 22-Dec-2005   Zombie         created
   
*/
const vector3 ncPhyVehicle::GetWheelOrientation( const unsigned int index ) const
{
    ncSceneClass* sclass(this->GetEntityObject()->GetClassComponent<ncSceneClass>());

    vector3 position;

    quaternion orientation;

    sclass->GetPlugData( plugNames[ index ], position, orientation );

    matrix33 orMatrix(orientation);

    return orMatrix.to_euler();
}

//------------------------------------------------------------------------------
/**
	Returns the brush class.

    @return class

    history:
     - 23-Dec-2005   Zombie         created
   
*/
nEntityClass* ncPhyVehicle::GetBrushClass() const
{
    const ncLoaderClass* lclass( this->GetClassComponent<ncLoaderClass>() );

    n_assert2( lclass, "Missing component." );

    const char* rfile(lclass->GetResourceFile());

    n_assert2( rfile, "Missing resource file." );

    nString srfile( rfile );

    srfile.StripTrailingSlash();
    
    nString className(srfile.ExtractFileName());

    return nEntityClassServer::Instance()->GetEntityClass( className.Get() );
}

//-----------------------------------------------------------------------------
/**
    Creates the steering wheel object if any.

    history:
        - 06-May-2005   Zombie         created
*/
void ncPhyVehicle::CreateSteeringWheel()
{
    const nString& objectName( this->refClass->GetSteeringWheelObjectName() );
    if( objectName == "None" )
    {
        return;
    }

    this->steeringWheel = nEntityObjectServer::Instance()->NewLocalEntityObject( objectName.Get() );

    /// getting the plug info
    ncSceneClass* sclass(this->GetEntityObject()->GetClassComponent<ncSceneClass>());

    quaternion orientation;

    sclass->GetPlugData( plugSteeringWheelName, this->steeringWheelOffset, orientation );

    this->steeringWheelOrientation = orientation;
}

//-----------------------------------------------------------------------------
/**
    Updates steering wheel.

    history:
        - 06-May-2005   Zombie         created
*/
void ncPhyVehicle::UpdateSteeringWheel()
{
    if( !this->steeringWheel.isvalid() )
    {
        return;
    }

    matrix33 first;

    first.from_euler( vector3(0, -this->steerValue * steeringWheelTurning, 0) );

    vector3 position;

    this->GetPosition( position );

    matrix33 orientation;

    this->GetOrientation( orientation );

    position += orientation * this->steeringWheelOffset;

    this->steeringWheel->GetComponent<ncTransform>()->SetPosition( position );

    orientation = first *this->steeringWheelOrientation * orientation;

    this->steeringWheel->GetComponent<ncTransform>()->SetEuler( orientation.to_euler() );
}

//-----------------------------------------------------------------------------
/**
    Computes the final acceleration and turning.

    history:
        - 30-Jan-2006   Zombie         created
*/
void ncPhyVehicle::ComputeInput()
{
    this->braking = false;

    /// compute acceleration
    if( fabs(this->maintainedAcceleration) > phyreal(.1) )
    {
        if( this->maintainedAcceleration > 0 )
        {
            if( this->GetAccelerateValue() <  0 )
            {
                this->braking = true;
            }
        }
        else
        {
            if( this->GetAccelerateValue() > 0 )
            {
                this->braking = true;
            }
        }
    }

    if( !this->braking )
    {
        this->accelerateValue += nPhysicsServer::Proportion(this->maintainedAcceleration * this->refClass->GetAccelerateRatio());
    }

    /// compute turnin
    this->steerValue += nPhysicsServer::Proportion( this->maintainedTurning ) * this->refClass->GetSteerRatio();

    if( this->steerValue >  phyreal(1) )
    {
        this->steerValue = phyreal(1);
    }
    else 
    {
        if( this->steerValue < phyreal(-1) )
        {
            this->steerValue = phyreal(-1);
        }
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the acceleration value.

    @return gas expected

    history:
        - 30-Jan-2006   Zombie         created
*/
const phyreal ncPhyVehicle::GetAccelerateValue() const
{
    return this->accelerateValue;
}

//-----------------------------------------------------------------------------
/**
    Enables the physic object.

    history:
        - 15-Feb-2006   Zombie         created
*/
void ncPhyVehicle::Enable()
{
    if( this->GetBody() )
    {
        this->GetBody()->Enable();
    }

    // disabling wheel
    for( unsigned index(0); index < this->refClass->GetNumWheels(); ++index )
    {
        this->wheels[ index ].Enable();
    }
}

//-----------------------------------------------------------------------------
/**
    Disables the physic object.

    history:
        - 15-Feb-2006   Zombie         created
*/
void ncPhyVehicle::Disable()
{
    if( this->GetBody() )
    {
        this->GetBody()->Disable();
    }

    for( unsigned index(0); index < this->refClass->GetNumWheels(); ++index )
    {
        this->wheels[ index ].Disable();
    }
}

//-----------------------------------------------------------------------------
/**
    Enables the physic object.

    history:
        - 15-Feb-2006   Zombie         created
*/
void ncPhyVehicle::Wheel::Enable()
{
    this->GetWheelObject()->GetBody()->Enable();
}

//-----------------------------------------------------------------------------
/**
    Disables the physic object.

    history:
        - 15-Feb-2006   Zombie         created
*/
void ncPhyVehicle::Wheel::Disable()
{
    this->GetWheelObject()->GetBody()->Disable();
}
//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
