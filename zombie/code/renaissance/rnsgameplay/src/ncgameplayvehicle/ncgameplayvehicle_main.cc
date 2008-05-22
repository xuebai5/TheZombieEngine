#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncgameplayvehicle_main.cc
//  (C) Conjurer Services, S.A. 2005
//------------------------------------------------------------------------------

#include "ncgameplayvehicle/ncgameplayvehicle.h"
#include "rnsgameplay/ngameplayutils.h"
#include "entity/nentityobjectserver.h"
#include "zombieentity/nctransform.h"
#include "nscene/ncsceneclass.h"
#include "ncgameplayplayer/ncgameplayplayer.h"
#include "nphysics/ncphyvehicle.h"
#include "ncsound/ncsound.h"
#include "nphysics/ncphyvehicleclass.h"
#include "ncgameplayvehicleseat/ncgameplayvehicleseatclass.h"
#include "ntrigger/ngameevent.h"
#include "ntrigger/ntriggerserver.h"
#include "ntrigger/ncareaevent.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGameplayVehicle,ncGameplay);

//------------------------------------------------------------------------------

namespace 
{
    const float HowCloseToBeFromTheVehicleToEnterSquared(float(25));

    const char *plugSeatNames[] = {
        "seat01",
        "seat02",
        "seat03",
        "seat04",
        "seat05",
        "seat06",
    };
    const char *plugExitNames[] = {
        "exit_seat01",
        "exit_seat02",
        "exit_seat03",
        "exit_seat04",
        "exit_seat05",
        "exit_seat06",
    };
}

//------------------------------------------------------------------------------

/// stores the cars availables
nKeyArray<nEntityObject*> ncGameplayVehicle::vehiclesAvailable(1,1);

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGameplayVehicle::ncGameplayVehicle() :
    started( false ),
    driven( false ),
    battery( true )
{
    memset( this->seats, 0, sizeof( ncGameplayVehicleSeat* ) * ncGameplayVehicleClass::NumberOfSeats );

}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGameplayVehicle::~ncGameplayVehicle()
{
    this->Destroy();

    // unregistering car
    vehiclesAvailable.Rem( int(size_t(this)) );
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param loaded indicates if the instance is bare new of loaded

    history:
        - 18-Jan-2006   Zombie         created
*/
void ncGameplayVehicle::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType != nObject::ReloadedInstance)
    {
        // registering the vehicle
        vehiclesAvailable.Add( int(size_t(this)), this->GetEntityObject() );
        
        this->CreateSeats();
    }
}

//-----------------------------------------------------------------------------
/**
    Sets a seat.

    @param index which seat
    @param entityname entityobject to use

    history:
        - 18-Jan-2006   Zombie         created
*/
void ncGameplayVehicle::SetSeat( const unsigned int index, const nString& entityname )
{
    n_assert2( index < ncGameplayVehicleClass::NumberOfSeats, "Index out of bounds." );

    if( this->seats[ index ] )
    {
        nEntityObjectServer::Instance()->RemoveEntityObject( this->seats[ index ]->GetEntityObject() );
    }

    this->seats[ index ] = static_cast<ncGameplayVehicleSeat*>(
        nEntityObjectServer::Instance()->NewLocalEntityObject( entityname.Get())
        ->GetComponent<ncGameplayVehicleSeat>());

    n_assert2( this->seats[ index ], "Failed to create an entity seat." );

    this->seats[ index ]->SetSeatNumber( index );
}

//-----------------------------------------------------------------------------
/**
    Gets a seat.

    @param index which seat
    @return seat entity

    history:
        - 18-Jan-2006   Zombie         created
*/
nEntityObject* ncGameplayVehicle::GetSeat( const unsigned int index ) const
{
    n_assert2( index < ncGameplayVehicleClass::NumberOfSeats, "Index out of bounds." );

    return this->seats[ index ]->GetEntityObject();
}

//-----------------------------------------------------------------------------
/**
    Creates the car sets.

    history:
        - 18-Jan-2006   Zombie         created
*/
void ncGameplayVehicle::CreateSeats()
{
    // getting class reference
    ncGameplayVehicleClass* refClass( this->GetEntityObject()->GetClassComponent<ncGameplayVehicleClass>() );

    n_assert2( refClass, "Failed to get the right component." );

    for( int index(0); index < ncGameplayVehicleClass::NumberOfSeats; ++index )
    {
        if( refClass->GetSeat( index ) == "None" )
        {
            continue;
        }
        this->SetSeat( index, refClass->GetSeat( index ) );

        ncSceneClass* sclass(this->GetEntityObject()->GetClassComponent<ncSceneClass>());

        quaternion orientation;

        vector3 position;

        sclass->GetPlugData( plugSeatNames[ index ], position, orientation );

        this->seats[ index ]->SetSeatOffset( position );

        sclass->GetPlugData( plugExitNames[ index ], position, orientation );

        this->seats[ index ]->SetExitPosition( position );
    }
}

//-----------------------------------------------------------------------------
/**
    
    Destroys object information.

    history:
        - 18-Jan-2006   Zombie         created
*/
void ncGameplayVehicle::Destroy()
{
    // destroyin seats
    for( int index(0); index < ncGameplayVehicleClass::NumberOfSeats; ++index )
    {
        if( this->seats[ index ] )
        {
            nEntityObjectServer::Instance()->RemoveEntityObject( this->seats[ index ]->GetEntityObject() );
            this->seats[ index ] = 0;
        }
    }
}

//-----------------------------------------------------------------------------
/**
    
    Returns the number of vehicles available.

    @return number of vehicles

    history:
        - 19-Jan-2006   Zombie         created
*/
unsigned int ncGameplayVehicle::GetNumVehiclesAvailable()
{
    return vehiclesAvailable.Size();
}

//-----------------------------------------------------------------------------
/**
    
    Returns the clossest vehicle.

    @param position world's position

    @return vehicle

    history:
        - 19-Jan-2006   Zombie         created
*/
nEntityObject* ncGameplayVehicle::GetClossestVehicle( const vector3& position )
{
    float closest(1e10);

    nEntityObject *clossestVehicle(0);

    for( int index(0); index < vehiclesAvailable.Size(); ++index )
    {
        nEntityObject* vehicle(GetVehicle( index ));

        vector3 distance( vehicle->GetComponent<ncTransform>()->GetPosition() - position );

        float lenght( distance.lensquared() );

        if( lenght > HowCloseToBeFromTheVehicleToEnterSquared )
        {
            continue;
        }

        if( lenght < closest )
        {
            closest = lenght;
            clossestVehicle = vehicle;
        }

    }

    return clossestVehicle;
}

//------------------------------------------------------------------------------
/**
	Return a vehicle by index.

    @param index vehicle index
    @return vehicle entity

    history:
     - 27-Jan-2006   Zombie         created
   
*/
nEntityObject* ncGameplayVehicle::GetVehicle( const unsigned int index )
{
    return vehiclesAvailable.GetElementAt(index);
}

//------------------------------------------------------------------------------
/**
	Returns if there's a seat available.

    @return true/false

    history:
     - 27-Jan-2006   Zombie         created
   
*/
const bool ncGameplayVehicle::IsASeatAvailable() const
{
    /// check if any seed it's available
    for( int index(0); index < ncGameplayVehicleClass::NumberOfSeats; ++index )
    {
        if( this->seats[ this->GetClassComponent<ncGameplayVehicleClass>()->GetSeatByPriority( index ) ]->GetSeatFree() )
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/**
	Reserves the seat.

    @param driver who will seat there
    @return if succesful

    history:
     - 27-Jan-2006   Zombie         created
   
*/
const bool ncGameplayVehicle::ReserveSeat( nEntityObject* driver )
{
    n_assert2( driver, "Null pointer." );

    /// check if any seed it's available
    for( int index(0); index < ncGameplayVehicleClass::NumberOfSeats; ++index )
    {
        if( this->seats[ this->GetClassComponent<ncGameplayVehicleClass>()->GetSeatByPriority( index ) ]->GetSeatFree() )
        {
            ncGameplayVehicleSeat* seat(this->seats[ this->GetClassComponent<ncGameplayVehicleClass>()->GetSeatByPriority( index ) ]);

            /// now it's ocupied (no place for old ladies)
            seat->SetSeatFree( false );

            seat->SetPassenger( driver );

            seat->SetVehicle( this->GetEntityObject() );

            driver->GetComponent<ncGameplayPlayer>()->SetSeat( seat->GetEntityObject() );

            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
/**
	Runs the gameplay logic.

    @param deltaTime time transcurred since last call in seconds

    history:
     - 27-Jan-2006   Zombie         created
   
*/
void ncGameplayVehicle::Run( const float /*deltaTime*/ )
{
    // seats the passangers in their seats
    for( int index(0); index < ncGameplayVehicleClass::NumberOfSeats; ++index )
    {
        if( !this->seats[ index ] )
        {
            continue;
        }

        if( !this->seats[ index ]->GetSeatFree() )
        {
            ncGameplayVehicleSeat* seat(this->seats[ index ]);
            ncGameplayVehicleSeatClass* seatClass(seat->GetClassComponent<ncGameplayVehicleSeatClass>());

            vector3 positionOffset( seat->GetSeatOffset() );

            matrix33 orientation;
            
            orientation.from_euler(this->GetComponent<ncTransform>()->GetEuler());

            positionOffset = orientation * positionOffset;

            positionOffset += this->GetComponent<ncTransform>()->GetPosition();
            
            ncTransform * passengerTransform = seat->GetPassenger()->GetComponentSafe<ncTransform>();

            ncGameplayPlayer * gpPassenger = seat->GetPassenger()->GetComponentSafe<ncGameplayPlayer>();

            switch( seat->GetSeatType() )
            {
                case ncGameplayVehicleSeatClass::driver:
                    gpPassenger->SetPhysicPosition( positionOffset );
                    passengerTransform->SetEuler( this->GetComponent<ncTransform>()->GetEuler() );
                    break;
                    
                case ncGameplayVehicleSeatClass::normal:
                    gpPassenger->SetPhysicPosition( positionOffset );
                    gpPassenger->SetElevationLimits( vector2( seatClass->GetMinPitch(), seatClass->GetMaxPitch() ) );
                    gpPassenger->SetTurnLimits( vector2( seatClass->GetMinYaw(), seatClass->GetMaxYaw() ) );

                    //calculate angle of direction
                    vector3 vehicleDirection = orientation * vector3( 0, 0, -1 );

                    polar2 angles;
                    // calculate pitch
                    vector3 projection = vehicleDirection;
                    projection.x = 0.0f;
                    if( projection.z > 0 )
                    {
                        projection.z = -projection.z;
                    }

                    float cosa = projection.dot( nGameplayUtils::AngleBase ) / projection.len();

                    float angle = acos( cosa );
                    if( vehicleDirection.y < 0.0f )
                    {
                        angle = -angle;
                    }
                        
                    angles.theta = angle;

                    // calculate yaw
                    projection = vehicleDirection;
                    projection.y = 0.0f;

                    cosa = projection.dot( nGameplayUtils::AngleBase ) / projection.len();
                    angle = acos( cosa );
                    if( vehicleDirection.x > 0.0f )
                    {
                        angle = -angle;
                    }
                    angles.rho = angle;

                    gpPassenger->SetAngleCenter( vector2( angles.rho + N_PI, -angles.theta ) );
                    break;
            }
        }
    }

    this->UpdateSound();
}

//------------------------------------------------------------------------------
/**
	Vehicle sound.

    history:
     - 08-Feb-2006   Zombie         created
   
*/
void ncGameplayVehicle::UpdateSound()
{
    ncSound* sound( this->GetComponent<ncSound>() );

    n_assert2( sound, "Missing sound component." );

    if( !this->driven || !this->GetBattery() )
    {
        ncAreaEvent* areaEvent = this->GetComponentSafe<ncAreaEvent>();
        n_assert( areaEvent );
        if ( areaEvent )
        {
            nGameEvent::Id eventId = areaEvent->GetEventByType( nGameEvent::VEHICLE_SOUND );
            if ( eventId != nGameEvent::INVALID_ID )
            {
                areaEvent->StopGameEvent( eventId );
            }
        }    
        return;
    }

    if( this->started )
    {
        ncPhyVehicle* phyVehicle( this->GetComponent<ncPhyVehicle>() );

        phyreal revolutions(fabs(phyVehicle->GetAccelerateValue()));

        ncPhyVehicleClass* refPhyClass( this->GetEntityObject()->GetClassComponent<ncPhyVehicleClass>() );

        if( revolutions > refPhyClass->GetMaxRevolution() )
        {
            revolutions = refPhyClass->GetMaxRevolution();
        }

        revolutions /= refPhyClass->GetMaxRevolution();

        sound->SetSoundFrequency( .5f + ( .5f * revolutions) );

        ncAreaEvent* areaEvent = this->GetComponentSafe<ncAreaEvent>();
        n_assert( areaEvent );
        if ( areaEvent )
        {
            nGameEvent::Id eventId = areaEvent->GetEventByType( nGameEvent::VEHICLE_SOUND );
            if ( eventId == nGameEvent::INVALID_ID)
            {
                this->GetComponentSafe<ncAreaEvent>()->EmitGameEvent(nGameEvent::VEHICLE_SOUND, this->GetEntityObject(), 0, 0);
            }
        }    
    }
}

//------------------------------------------------------------------------------
/**
	Kicks in vehicle.

    history:
     - 08-Feb-2006   Zombie         created
   
*/
// workaround to compiler error, see MS Knowledge Base 320004
#pragma optimize("g",off)
void ncGameplayVehicle::SwitchOnVehicle()
{
    ncSound* sound( this->GetComponent<ncSound>() );

    this->started = false;

    if( this->GetBattery() )
    {
        sound->SetSoundPositionOffset( this->GetClassComponentSafe<ncPhyVehicleClass>()->GetMotorPosition() );
        sound->PlayAlone( "MotorOn", 1 );
        sound->Play( "Engine", 0 );
        sound->SetSoundFrequency( 1 );
        this->started = true;
    }
    else
    {
        sound->Play( "NoBatteryStart", 1 );
    }

    this->driven = true;

    this->GetComponent<ncPhyVehicle>()->SetHandBrake( false );
}
#pragma optimize("",on)

//------------------------------------------------------------------------------
/**
	Switches off the vehicle.

    history:
     - 08-Feb-2006   Zombie         created
   
*/
void ncGameplayVehicle::SwitchOffVehicle()
{
    ncSound* sound( this->GetComponent<ncSound>() );
    
    if( this->GetBattery() )
    {
        sound->Play( "MotorOff", 1 );
    }

    this->driven = false;

    this->started = false;

    this->GetComponent<ncPhyVehicle>()->SetHandBrake( true );
}

//------------------------------------------------------------------------------
/**
	Sets if the vehicle has battery.

    @param has true/false

    history:
     - 17-Feb-2006   Zombie         created
   
*/
void ncGameplayVehicle::SetBattery( const bool has )
{
    this->battery = has;
}

//------------------------------------------------------------------------------
/**
	Returns if the vehicle has battery.

    @return true/false

    history:
     - 17-Feb-2006   Zombie         created
   
*/
const bool ncGameplayVehicle::GetBattery() const
{
    return this->battery;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
