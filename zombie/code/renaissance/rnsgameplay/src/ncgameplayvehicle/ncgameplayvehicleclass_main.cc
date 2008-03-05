#include "precompiled/pchrnsgameplay.h"

/**
    @file ncgameplayvehicleclass_main.cc

    (C) Conjurer Services, S.A. 2005
*/

#include "ncgameplayvehicle/ncgameplayvehicleclass.h"

//------------------------------------------------------------------------------
nNebulaComponentClass(ncGameplayVehicleClass,ncGameplayClass);

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGameplayVehicleClass::ncGameplayVehicleClass() :
    maxHealth(100)
{
    for( int index(0); index < NumberOfSeats; ++index )
    {
        this->seatPriority[ index ] = index;
        this->seats[ index ] = "None";
    }
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGameplayVehicleClass::~ncGameplayVehicleClass()
{
    // empty
}

//-----------------------------------------------------------------------------
/**
    Sets a seat.

    @param index which seat
    @param entityname entityobject to use

    history:
        - 18-Jan-2006   David Reyes    created
*/
void ncGameplayVehicleClass::SetSeat( const unsigned int index, const nString& entityname )
{
    n_assert2( index < NumberOfSeats, "Index out of bounds." );

    this->seats[ index ] = entityname;
}

//-----------------------------------------------------------------------------
/**
    Gets a seat.

    @param index which seat
    
    @return name of the seat class

    history:
        - 18-Jan-2006   David Reyes    created
*/
const nString& ncGameplayVehicleClass::GetSeat(const unsigned int index) const
{
    n_assert2( index < NumberOfSeats, "Index out of bounds." );

    return this->seats[ index ];
}

//-----------------------------------------------------------------------------
/**
    Returns the priority seat.

    @param priority seats priority
    
    @return priority lvl    

    history:
        - 18-Jan-2006   David Reyes    created
*/
const unsigned int ncGameplayVehicleClass::GetSeatByPriority( const unsigned int priority ) const
{
    return this->seatPriority[ priority ];
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
