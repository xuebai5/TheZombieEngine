#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncgameplayvehicleclass_main.cc
//  (C) Conjurer Services, S.A. 2005
//------------------------------------------------------------------------------

#include "ncgameplayvehicleseat/ncgameplayvehicleseatclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncGameplayVehicleSeatClass,ncGameplayClass);

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGameplayVehicleSeatClass::ncGameplayVehicleSeatClass() : 
    seatType( unknow ),
    maxYaw(0),
    minYaw(0),
    maxPitch(0),
    minPitch(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGameplayVehicleSeatClass::~ncGameplayVehicleSeatClass()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Sets a seat type.

    @param type which type of seat

    history:
        - 18-Jan-2006   Zombie         created
*/
void ncGameplayVehicleSeatClass::SetSeatType( const type seattype )
{
    this->seatType = seattype;
}

//-----------------------------------------------------------------------------
/**
    Gets a seat type.

    @return which type of seat

    history:
        - 18-Jan-2006   Zombie         created
*/
const ncGameplayVehicleSeatClass::type ncGameplayVehicleSeatClass::GetSeatType() const
{
    return this->seatType;
}

//-----------------------------------------------------------------------------
/**
    Sets the max movement angle for yaw.

    @param newvalue angle

    history:
        - 06-Feb-2006   Zombie         created
*/
void ncGameplayVehicleSeatClass::SetMaxYaw( const float newvalue )
{
    this->maxYaw = newvalue;
}

//-----------------------------------------------------------------------------
/**
    Gets the max movement angle for yaw.

    @return angle

    history:
        - 06-Feb-2006   Zombie         created
*/
const float ncGameplayVehicleSeatClass::GetMaxYaw() const
{
    return this->maxYaw;
}

//-----------------------------------------------------------------------------
/**
    Sets the min movement angle for yaw.

    @param newvalue angle

    history:
        - 06-Feb-2006   Zombie         created
*/
void ncGameplayVehicleSeatClass::SetMinYaw(const float newvalue )
{
    this->minYaw = newvalue;
}

//-----------------------------------------------------------------------------
/**
    Gets the min movement angle for yaw.

    @return angle

    history:
        - 06-Feb-2006   Zombie         created
*/
const float ncGameplayVehicleSeatClass::GetMinYaw() const
{
    return this->minYaw;
}

//-----------------------------------------------------------------------------
/**
    Sets the max movement angle for pitch.

    @param newvalue angle

    history:
        - 06-Feb-2006   Zombie         created
*/
void ncGameplayVehicleSeatClass::SetMaxPitch( const float newvalue )
{
    this->maxPitch = newvalue;
}

//-----------------------------------------------------------------------------
/**
    Gets the max movement angle for pitch.

    @return angle

    history:
        - 06-Feb-2006   Zombie         created
*/
const float ncGameplayVehicleSeatClass::GetMaxPitch() const
{
    return this->maxPitch;
}

//-----------------------------------------------------------------------------
/**
    Sets the min movement angle for pitch.

    @param newvalue angle

    history:
        - 06-Feb-2006   Zombie         created
*/
void ncGameplayVehicleSeatClass::SetMinPitch( const float newvalue )
{
    this->minPitch = newvalue;
}

//-----------------------------------------------------------------------------
/**
    Gets the min movement angle for pitch.

    @return angle

    history:
        - 06-Feb-2006   Zombie         created
*/
const float ncGameplayVehicleSeatClass::GetMinPitch() const
{
    return this->minPitch;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
 