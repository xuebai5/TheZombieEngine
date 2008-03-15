#ifndef NCGAMEPLAYVEHICLESEAT_H
#define NCGAMEPLAYVEHICLESEAT_H

//------------------------------------------------------------------------------
/**
    @class ncGameplayVehicleSeat

    (C) 2005 Conjurer Services, S.A.
*/

#include "rnsgameplay/ncgameplay.h"

class ncGameplayVehicleSeatClass;

class ncGameplayVehicleSeat : public ncGameplay
{

    NCOMPONENT_DECLARE(ncGameplayVehicleSeat,ncGameplay);

public:

    /// Constructor
    ncGameplayVehicleSeat();
    /// Destructor
    ~ncGameplayVehicleSeat();

    /// Persistency
    bool SaveCmds (nPersistServer* ps); 

    /// returns the exit position for this seat
    const vector3& GetExitPosition() const;

    /// returns the exit position for this seat
    void SetExitPosition( const vector3& newposition );

    /// set the current passenger
    void SetPassenger (nEntityObject* newpassenger);

    /// get the current passenger
    nEntityObject* GetPassenger() const;

    /// set the current weapon mounted
    void SetWeapon (nEntityObject* weapon);

    /// get the current weapon mounted
    nEntityObject* GetWeapon() const;

    /// sets the seat number
    void SetSeatNumber( const int number );

    /// gets the seat number
    const int GetSeatNumber() const;

    /// gets seat's offset
    const vector3& GetSeatOffset() const;

    /// sets seat's offset
    void SetSeatOffset(const vector3 newoffset );

    /// gets the vehicle where this seat belongs
    nEntityObject* GetVehicle() const;

    /// sets the vehicle where this seat belongs
    void SetVehicle( nEntityObject* newvehicle );

    /// manage player
    void ManagePlayer( const float frameTime );

    /// user init instance code
    void InitInstance(nObject::InitInstanceMsg initType);

    /// returns the type of seat
    const int GetSeatType() const;

    /// sets if the seat it's free
    void SetSeatFree( const bool is );

    /// gets if the seat it's free
    const bool GetSeatFree() const;

private:
    /// stores the passanger entity
    nEntityObject* passenger;
    /// stores the weapon entity
    nEntityObject* weaponMounted;
    /// stores the seat number
    int numberseat;
    /// seat offset
    vector3 seatOffset;
    /// stores the vehicle
    nEntityObject* vehicle;
    /// manages driver
    void ManageDriver( const float frameTime );
    /// manages normal
    void ManageNormal( const float frameTime );
    /// stores the values of the input for acceleration
    float acceleration;
    /// stores the values of the input for turning
    float turning;
    /// stores if the seat it's ocupied
    bool free;
    /// class ref
    ncGameplayVehicleSeatClass* refClass;
    /// stores the exit position offset
    vector3 exitPositionOffset;

};

#endif
