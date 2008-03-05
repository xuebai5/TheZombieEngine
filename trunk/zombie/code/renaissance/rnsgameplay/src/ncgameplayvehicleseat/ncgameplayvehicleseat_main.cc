#include "precompiled/pchrnsgameplay.h"

/**
    @file ncgameplayvehicleseat_main.cc

    (C) Conjurer Services, S.A. 2005
*/

#include "ncgameplayvehicleseat/ncgameplayvehicleseat.h"
#include "ncgameplayvehicleseat/ncgameplayvehicleseatclass.h"
#include "nphysics/ncphyvehicle.h"
#include "input/ninputserver.h"
#include "nnetworkmanager/nnetworkmanager.h"
#include "ncgameplayvehicle/ncgameplayvehicle.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGameplayVehicleSeat,ncGameplay);

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGameplayVehicleSeat::ncGameplayVehicleSeat() : 
    passenger(0),
    weaponMounted(0),
    numberseat(0),
    vehicle(0),
    acceleration(0),
    turning(0),
    free(true),
    exitPositionOffset(0,0,0),
    refClass(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGameplayVehicleSeat::~ncGameplayVehicleSeat()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Sets the seat number.

    @param number assigned number

    history:
        - 25-Jan-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::SetSeatNumber( const int number )
{
    this->numberseat = number;
}

//-----------------------------------------------------------------------------
/**
    Gets the seat number.

    @return seat's assigned number

    history:
        - 25-Jan-2006   David Reyes    created
*/
const int ncGameplayVehicleSeat::GetSeatNumber() const
{
    return this->numberseat;
}

//-----------------------------------------------------------------------------
/**
    Gets seat's offset.

    @return seat's assigned number

    history:
        - 25-Jan-2006   David Reyes    created
*/
const vector3& ncGameplayVehicleSeat::GetSeatOffset() const
{
    return this->seatOffset;
}

//-----------------------------------------------------------------------------
/**
    Sets seat's offset.

    @param newoffset new seat's offset

    history:
        - 25-Jan-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::SetSeatOffset(const vector3 newoffset )
{
    this->seatOffset = newoffset;
}

//-----------------------------------------------------------------------------
/**
    Gets the vehicle where this seat belongs.

    @return vehicle

    history:
        - 25-Jan-2006   David Reyes    created
*/
nEntityObject* ncGameplayVehicleSeat::GetVehicle() const
{
    return this->vehicle;
}

//-----------------------------------------------------------------------------
/**
    Sets the vehicle where this seat belongs.

    @param newvehicle seat's vehicle

    history:
        - 25-Jan-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::SetVehicle( nEntityObject* newvehicle )
{
    this->vehicle = newvehicle;
}

//-----------------------------------------------------------------------------
/**
    Manage player.

    @param frameTime

    history:
        - 25-Jan-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::ManagePlayer( const float frameTime )
{
    switch( this->GetSeatType() )
    {
    case ncGameplayVehicleSeatClass::driver:
        this->ManageDriver( frameTime );
        break;
    case ncGameplayVehicleSeatClass::normal:
        this->ManageNormal( frameTime );
        break;
    }
}

//-----------------------------------------------------------------------------
/**
    Manages driver.

    @param frameTime

    history:
        - 25-Jan-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::ManageDriver( const float /*frameTime*/ )
{
    if( nInputServer::Instance()->GetButton( "Activate" ) )
    {
        // leaving the car
        nArg args[3];
        args[0].Delete(); args[0].SetO( this->GetPassenger() );
        n_assert2( this->GetPassenger(), "Missing Passenger Entity." );
        args[1].Delete(); args[1].SetO( this->GetVehicle() );
        n_assert2( this->GetVehicle(), "Missing Vehicle Entity." );
        args[2].Delete(); args[2].SetI( this->GetSeatNumber() ); 
        nNetworkManager::Instance()->SendAction( "ngpleavevehicle", 3, args, true );

        args[0].Delete(); args[0].SetO( this->GetVehicle() );
        args[1].Delete(); args[1].SetF( 0 );
        nNetworkManager::Instance()->SendAction( "ngpmovevehicle", 2, args, true );         

        args[0].Delete(); args[0].SetO( this->GetVehicle() );
        args[1].Delete(); args[1].SetF( 0 );
        nNetworkManager::Instance()->SendAction( "ngpturnvehicle", 2, args, true );         
        return;
    }

    if( !this->GetVehicle()->GetComponent<ncGameplayVehicle>()->GetBattery() )
    {
        return;
    }

    float accel(0);

    float turn(0);

    if( nInputServer::Instance()->GetButton( "Forward" ) )
    {
        accel += float(-1);

    } else if( nInputServer::Instance()->GetButton( "Backward" ) )
    {
        accel += float(1);
    } 

    if( nInputServer::Instance()->GetButton( "StrafeLeft" ) )
    {
        turn += float(-1);
    }
    else if( nInputServer::Instance()->GetButton( "StrafeRight" ) )
    {
        turn += float(1);
    }        

    if( accel != this->acceleration )
    {
        this->acceleration = accel;

        nArg args[2];
        args[0].Delete(); args[0].SetO( this->GetVehicle() );
        args[1].Delete(); args[1].SetF( this->acceleration );
        nNetworkManager::Instance()->SendAction( "ngpmovevehicle", 2, args, true );         
    }

    if( turn != this->turning )
    {
        this->turning = turn;

        nArg args[2];
        args[0].Delete(); args[0].SetO( this->GetVehicle() );
        args[1].Delete(); args[1].SetF( this->turning );
        nNetworkManager::Instance()->SendAction( "ngpturnvehicle", 2, args, true );         
    }
}

//-----------------------------------------------------------------------------
/**
    Get exit position.

    @return exit position

    history:
        - 25-Jan-2006   David Reyes    created
*/
const vector3& ncGameplayVehicleSeat::GetExitPosition() const
{
    return this->exitPositionOffset;
}

//-----------------------------------------------------------------------------
/**
    Sets the exit position for this seat.

    @param newposition exit position

    history:
        - 02-Feb-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::SetExitPosition( const vector3& newposition )
{
    this->exitPositionOffset = newposition;
}

//-----------------------------------------------------------------------------
/**
    User init instance code.

    @param initType type of initialization

    history:
        - 06-Feb-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    if( !this->refClass )
    {
        this->refClass = this->GetClassComponent<ncGameplayVehicleSeatClass>();
    }
}

//-----------------------------------------------------------------------------
/**
    Returns the type of seat.

    @return seat's type

    history:
        - 06-Feb-2006   David Reyes    created
*/
const int ncGameplayVehicleSeat::GetSeatType() const
{
    return this->refClass->GetSeatType();
}


//-----------------------------------------------------------------------------
/**
    Sets if the seat it's free.

    @param is true/false

    history:
        - 06-Feb-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::SetSeatFree( const bool is )
{
    this->free = is;
}

//-----------------------------------------------------------------------------
/**
    Gets if the seat it's free.

    @return true/false

    history:
        - 06-Feb-2006   David Reyes    created
*/
const bool ncGameplayVehicleSeat::GetSeatFree() const
{
    return this->free;
}

//-----------------------------------------------------------------------------
/**
    Set the current passenger.

    @param newpassanger passenger object

    history:
        - 06-Feb-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::SetPassenger (nEntityObject* newpassenger)
{
    this->passenger = newpassenger;
}

//-----------------------------------------------------------------------------
/**
    Get the current passenger.

    @return passenger object

    history:
        - 06-Feb-2006   David Reyes    created
*/
nEntityObject* ncGameplayVehicleSeat::GetPassenger() const
{
    return this->passenger;
}

//-----------------------------------------------------------------------------
/**
    Manages normal.

    @param frameTime elapsed time between frames

    history:
        - 06-Feb-2006   David Reyes    created
*/
void ncGameplayVehicleSeat::ManageNormal( const float /*frameTime*/ )
{
    if( nInputServer::Instance()->GetButton( "Activate" ) )
    {
        // leaving the car
        nArg args[3];
        args[0].Delete(); args[0].SetO( this->GetPassenger() );
        n_assert2( this->GetPassenger(), "Missing Passenger Entity." );
        args[1].Delete(); args[1].SetO( this->GetVehicle() );
        n_assert2( this->GetVehicle(), "Missing Vehicle Entity." );
        args[2].Delete(); args[2].SetI( this->GetSeatNumber() ); 
        nNetworkManager::Instance()->SendAction( "ngpleavevehicle", 3, args, true );
    }
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
