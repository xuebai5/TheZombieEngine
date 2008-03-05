#ifndef NCGAMEPLAYVEHICLE_H
#define NCGAMEPLAYVEHICLE_H
//-----------------------------------------------------------------------------
/**
    @class ncGameplayVehicle
    @ingroup GamePlay
    @brief Car's gameplay component.

    (C) 2004 Conjurer Services, S.A.
*/

#include "rnsgameplay/ncgameplay.h"
#include "ncgameplayvehicle/ncgameplayvehicleclass.h"
#include "ncgameplayvehicleseat/ncgameplayvehicleseat.h"

//-----------------------------------------------------------------------------

class ncGameplayVehicle : public ncGameplay
{

    NCOMPONENT_DECLARE(ncGameplayVehicle,ncGameplay);

public:
    /// Constructor
    ncGameplayVehicle();
    /// Destructor
    ~ncGameplayVehicle();

    // Sets/gets the current health of the vehicle
    void SetHealth(int);
    int GetHealth() const;

    /// sets a seat
    void SetSeat(const unsigned int,const nString&);
    /// gets a seat
    nEntityObject* GetSeat(const unsigned int) const;

    /// sets if the vehicle has battery
    void SetBattery(const bool);
    /// returns if the vehicle has battery
    const bool GetBattery() const;

    /// Persistency
    bool SaveCmds (nPersistServer* ps);
     
    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// returns the number of vehicles available
    static unsigned int GetNumVehiclesAvailable();

    /// return a vehicles by index
    static nEntityObject* GetVehicle( const unsigned int index );

    /// returns the clossest vehicle
    static nEntityObject* GetClossestVehicle( const vector3& position );

    /// returns if there's a seat available
    const bool IsASeatAvailable() const;

    /// reserves the seat
    const bool ReserveSeat( nEntityObject* driver );

    /// runs the gameplay logic 
    void Run( const float deltaTime );

    /// kicks in vehicle
    void SwitchOnVehicle();

    /// switches off the vehicle
    void SwitchOffVehicle();

private:

    /// stores the vehicles available
    static nKeyArray<nEntityObject*> vehiclesAvailable;

    // car hit points
    int health;

    // stores the car seats
    ncGameplayVehicleSeat* seats[ncGameplayVehicleClass::NumberOfSeats];

    nArray<nEntityObject*> inventory;

    // creates the car sets
    void CreateSeats();

    // destroys object information
    void Destroy();

    // vehicle sound
    void UpdateSound();

    /// stores if the engine it's been started
    bool started;

    /// stores if the car it's been driven
    bool driven;

    /// stores if the car has battery
    bool battery;

};

//------------------------------------------------------------------------------
/**
    SetHealth
*/
inline
void
ncGameplayVehicle::SetHealth (int health)
{
    this->health = health;
}

//------------------------------------------------------------------------------
/**
    GetHealth
*/
inline
int
ncGameplayVehicle::GetHealth() const
{
    return this->health;
}

#endif
