#ifndef NCGAMEPLAYVEHICLESEATCLASS_H
#define NCGAMEPLAYVEHICLESEATCLASS_H

//------------------------------------------------------------------------------
/**
    @class ncGameplayVehicleSeatClass

    (C) 2005 Conjurer Services, S.A.
*/

#include "rnsgameplay/ncgameplayclass.h"

//-----------------------------------------------------------------------------

class ncGameplayVehicleSeatClass : public ncGameplayClass
{

    NCOMPONENT_DECLARE(ncGameplayVehicleSeatClass,ncGameplayClass);

public:

    /// types of seat
    typedef enum {
        unknow = 0,
        driver,
        normal,
        withweapon
    } type;

    /// Constructor
    ncGameplayVehicleSeatClass();
    /// Destructor
    ~ncGameplayVehicleSeatClass();

    /// sets a seat type
    void SetSeatType(const type);

    /// gets a seat type
    const type GetSeatType() const;

    /// sets the max movement angle for yaw
    void SetMaxYaw(const float);

    /// gets the max movement angle for yaw
    const float GetMaxYaw() const;

    /// sets the min movement angle for yaw
    void SetMinYaw(const float);

    /// gets the min movement angle for yaw
    const float GetMinYaw() const;

    /// sets the max movement angle for pitch
    void SetMaxPitch(const float);

    /// gets the max movement angle for pitch
    const float GetMaxPitch() const;

    /// sets the min movement angle for pitch
    void SetMinPitch(const float);

    /// gets the min movement angle for pitch
    const float GetMinPitch() const;

    /// Persistency
    bool SaveCmds (nPersistServer* ps); 

private:

    /// stores the type of seat
    type seatType;

    /// stores the max yaw
    float maxYaw;

    /// stores the min yaw
    float minYaw;

    /// stores the max pitch
    float maxPitch;

    /// stores the min pitch
    float minPitch;
    
};

//-----------------------------------------------------------------------------

N_CMDARGTYPE_NEW_TYPE(ncGameplayVehicleSeatClass::type, "i", (value = (ncGameplayVehicleSeatClass::type)(size_t)cmd->In()->GetI()), (cmd->Out()->SetI(int(size_t(value))))  );

//-----------------------------------------------------------------------------

#endif