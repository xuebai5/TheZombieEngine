#ifndef NCGAMEPLAYVEHICLECLASS_H
#define NCGAMEPLAYVEHICLECLASS_H

#include "rnsgameplay/ncgameplayclass.h"

class ncGameplayVehicleClass : public ncGameplayClass
{

    NCOMPONENT_DECLARE(ncGameplayVehicleClass,ncGameplayClass);

public:
    /// Constructor
    ncGameplayVehicleClass();
    /// Destructor
    ~ncGameplayVehicleClass();

    /// Persistency
    bool SaveCmds (nPersistServer* ps);
    
    /// Sets/gets the max health of the vehicle
    void SetMaxHealth(int);
    int GetMaxHealth() const;

    /// Sets/gets the seat for a given position
    void SetSeat(const unsigned int, const nString&);
    const nString& GetSeat(const unsigned int) const;

    static const int NumberOfSeats = 6;

    /// returns the priority seat
    const unsigned int GetSeatByPriority( const unsigned int priority ) const;

private:
    /// sets the car's hit points
    int maxHealth;

    /// seats priority
    int seatPriority[ NumberOfSeats ];

    /// stores the inventory size
    int inventorySize;

    /// stores the car sets
    nString seats[ NumberOfSeats ];

};

//------------------------------------------------------------------------------
/** 
    SetMaxHealth
*/
inline
void
ncGameplayVehicleClass::SetMaxHealth (int maxHealth)
{
    this->maxHealth = maxHealth;
}

//------------------------------------------------------------------------------
/**
    GetMaxHealth
*/
inline
int
ncGameplayVehicleClass::GetMaxHealth() const
{
    return this->maxHealth;
}

#endif