#ifndef NC_PHYVEHICLECLASS_H
#define NC_PHYVEHICLECLASS_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyVehicleClass
    @ingroup NebulaPhysicsSystem
    @brief Base physic object to create a vehicle.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
#include "entity/nentity.h"

#include "nphysics/ncphysicsobjclass.h"

//-----------------------------------------------------------------------------
class ncPhyVehicleClass : public ncPhysicsObjClass
{

    NCOMPONENT_DECLARE(ncPhyVehicleClass,ncPhysicsObjClass);

public:

    enum {
        frontright = 0,
        frontleft = 1,
        rearright = 2,
        rearleft = 3
    };

    /// constructor
    ncPhyVehicleClass();

    /// destructor
    virtual ~ncPhyVehicleClass();

    /// returns the number of wheels
    const unsigned int GetNumWheels() const;

    /// sets wheel traction for the front left wheel
    void SetFrontLeftWheelTraction(const bool);
    /// gets wheel traction from the front left wheel
    const bool GetFrontLeftWheelTraction() const;
    /// sets wheel traction for the front right wheel
    void SetFrontRightWheelTraction(const bool);
    /// gets wheel traction from the front right wheel
    const bool GetFrontRightWheelTraction() const;

    /// sets wheel traction for the rear left wheel
    void SetRearLeftWheelTraction(const bool);
    /// gets wheel traction from the rear left wheel
    const bool GetRearLeftWheelTraction() const;
    /// sets wheel traction for the rear right wheel
    void SetRearRightWheelTraction(const bool);
    /// gets wheel traction from the rear right wheel
    const bool GetRearRightWheelTraction() const;

    /// sets wheel direction for the front left wheel
    void SetFrontLeftWheelDirection(const bool);
    /// gets wheel direction from the front left wheel
    const bool GetFrontLeftWheelDirection() const;
    /// sets wheel direction for the front right wheel
    void SetFrontRightWheelDirection(const bool);
    /// gets wheel direction from the front right wheel
    const bool GetFrontRightWheelDirection() const;

    /// sets wheel direction for the rear left wheel
    void SetRearLeftWheelDirection(const bool);
    /// gets wheel direction from the rear left wheel
    const bool GetRearLeftWheelDirection() const;
    /// sets wheel direction for the rear right wheel
    void SetRearRightWheelDirection(const bool);
    /// gets wheel direction from the rear right wheel
    const bool GetRearRightWheelDirection() const;
    
    /// sets wheel steering max for the front left wheel
    void SetFrontLeftWheelSteeringMax(const phyreal);
    /// gets wheel steering max from the front left wheel
    const phyreal GetFrontLeftWheelSteeringMax() const;
    /// sets wheel steering max for the front right wheel
    void SetFrontRightWheelSteeringMax(const phyreal);
    /// gets wheel steering max from the front right wheel
    const phyreal GetFrontRightWheelSteeringMax() const;

    /// sets wheel steering max for the rear left wheel
    void SetRearLeftWheelSteeringMax(const phyreal);
    /// gets wheel steering max from the rear left wheel
    const phyreal GetRearLeftWheelSteeringMax() const;
    /// sets wheel steering max for the rear right wheel
    void SetRearRightWheelSteeringMax(const phyreal);
    /// gets wheel steering max from the rear right wheel
    const phyreal GetRearRightWheelSteeringMax() const;

    /// sets wheel steering forcea for the front left wheel
    void SetFrontLeftWheelSteeringForce(const phyreal);
    /// gets wheel steering forcea from the front left wheel
    const phyreal GetFrontLeftWheelSteeringForce() const;
    /// sets wheel steering forcea for the front right wheel
    void SetFrontRightWheelSteeringForce(const phyreal);
    /// gets wheel steering forcea from the front right wheel
    const phyreal GetFrontRightWheelSteeringForce() const;

    /// sets wheel steering forcea for the rear left wheel
    void SetRearLeftWheelSteeringForce(const phyreal);
    /// gets wheel steering forcea from the rear left wheel
    const phyreal GetRearLeftWheelSteeringForce() const;
    /// sets wheel steering forcea for the rear right wheel
    void SetRearRightWheelSteeringForce(const phyreal);
    /// gets wheel steering forcea from the rear right wheel
    const phyreal GetRearRightWheelSteeringForce() const;

    /// returns gravity center of the car (from the center of the car)
    const vector3& GetGravityCenterOffset() const;
    /// set gravity center of the car (from the center of the car)
    void SetGravityCenterOffset(const vector3&);

    /// sets how much BHPs (brake horse power) has
    void SetBHPs(const phyreal);
    /// gets how much BHPs (brake horse power) has
    const phyreal GetBHPs() const;

    /// sets the max revolutions of the engine
    void SetMaxRevolution(const phyreal);
    /// gets the max revolutions of the engine
    const phyreal GetMaxRevolution() const;

    /// sets the max force for the anti-roll bars
    void SetMaxAntiRollForce(const phyreal);
    /// gets the max force for the anti-roll bars
    const phyreal GetMaxAntiRollForce() const;

    /// sets the force for the anti-roll bars
    void SetAntiRollForce(const phyreal);
    /// gets the force for the anti-roll bars
    const phyreal GetAntiRollForce() const;

    /// sets the accelerate ratio
    void SetAccelerateRatio(const phyreal);
    /// gets the accelerate ratio
    const phyreal GetAccelerateRatio() const;

    /// sets the engine resistance
    void SetEngineResistance(const phyreal);
    /// gets the engine resistance
    const phyreal GetEngineResistance() const;

    /// sets the steer ratio
    void SetSteerRatio(const phyreal);
    /// gets the steer ratio
    const phyreal GetSteerRatio() const;
    
    /// sets the steer recover ratio
    void SetSteerRecoverRatio(const phyreal);
    /// gets the steer recover ratio
    const phyreal GetSteerRecoverRatio() const;

    /// sets wheel break treshold for the front left wheel
    void SetFrontLeftWheelBreakTreshold(const phyreal);
    /// gets wheel break treshold from the front left wheel
    const phyreal GetFrontLeftWheelBreakTreshold() const;
    /// sets wheel break treshold for the front right wheel
    void SetFrontRightWheelBreakTreshold(const phyreal);
    /// gets wheel break treshold from the front right wheel
    const phyreal GetFrontRightWheelBreakTreshold() const;

    /// sets wheel break treshold for the rear left wheel
    void SetRearLeftWheelBreakTreshold(const phyreal);
    /// gets wheel break treshold from the rear left wheel
    const phyreal GetRearLeftWheelBreakTreshold() const;
    /// sets wheel break treshold for the rear right wheel
    void SetRearRightWheelBreakTreshold(const phyreal);
    /// gets wheel break treshold from the rear right wheel
    const phyreal GetRearRightWheelBreakTreshold() const;
    
    /// sets the vehicle resistance to the air
    void SetAirResistance(const phyreal);
    /// gets the vehicle resistance to the air
    const phyreal GetAirResistance() const;

    /// sets wheel suspension recovery for the front left wheel
    void SetFrontLeftWheelSuspensionRecovery(const phyreal);
    /// gets wheel suspension recovery from the front left wheel
    const phyreal GetFrontLeftWheelSuspensionRecovery() const;
    /// sets wheel suspension recovery for the front right wheel
    void SetFrontRightWheelSuspensionRecovery(const phyreal);
    /// gets wheel suspension recovery from the front right wheel
    const phyreal GetFrontRightWheelSuspensionRecovery() const;

    /// sets wheel suspension recovery for the rear left wheel
    void SetRearLeftWheelSuspensionRecovery(const phyreal);
    /// gets wheel suspension recovery from the rear left wheel
    const phyreal GetRearLeftWheelSuspensionRecovery() const;
    /// sets wheel suspension recovery for the rear right wheel
    void SetRearRightWheelSuspensionRecovery(const phyreal);
    /// gets wheel suspension recovery from the rear right wheel
    const phyreal GetRearRightWheelSuspensionRecovery() const;

    /// sets the front right wheel
    void SetFrontRightWheel(const nString&);
    /// gets the front right wheel
    const nString& GetFrontRightWheel() const;
    
    /// sets the front left wheel
    void SetFrontLeftWheel(const nString&);
    /// gets the front left wheel
    const nString& GetFrontLeftWheel() const;

    /// sets the rear left wheel
    void SetRearLeftWheel(const nString&);
    /// gets the rear left wheel
    const nString& GetRearLeftWheel() const;

    /// sets the rear right wheel
    void SetRearRightWheel(const nString&);
    /// gets the rear right wheel
    const nString& GetRearRightWheel() const;

    /// begin: Helper functions to persist the vehicle
    
    /// sets if a wheel has traction
    void SetHasTraction(const unsigned int,const bool);
    /// gets if a wheel has traction
    const bool GetHasTraction( const unsigned int wheel ) const;

    /// sets if a wheel has direction
    void SetHasDirection(const unsigned int,const bool);
    /// gets if a wheel has direction
    const bool GetHasDirection( const unsigned int wheel ) const;

    /// sets a wheel steering max
    void SetSteeringMax(const unsigned int,const phyreal);
    /// gets a wheel steering max
    const phyreal GetSteeringMax( const unsigned int wheel ) const;

    /// sets a wheel steering force
    void SetSteeringForce(const unsigned int,const phyreal);
    /// gets a wheel steering force
    const phyreal GetSteeringForce(const unsigned int) const;

    /// sets a wheel suspension recover force
    void SetSuspensionRecoverForce(const unsigned int,const phyreal);

    /// sets a wheel break treshold
    void SetBreakTreshold(const unsigned int,const phyreal);
    /// gets a wheel break treshold
    const phyreal GetBreakTreshold( const unsigned int wheel ) const;

    /// sets a wheel object
    void SetWheelObjectName(const unsigned int,const nString&);
    /// gets a wheel break treshold
    const nString& GetWheelObjectName( const unsigned int wheel ) const;

    /// end: Helper functions to persist the vehicle

    /// sets the steering wheel object
    void SetSteeringWheelObjectName(const nString&);
    /// gets the steering wheel object
    const nString& GetSteeringWheelObjectName() const;

    /// sets the backwards engine resistance
    void SetBackwardsResistance(const phyreal);
    /// gets the backwards engine resistance
    const phyreal GetBackwardsResistance() const;

    /// gets the breaking force
    const phyreal GetBreakingForce() const;
    /// sets the breaking force
    void SetBreakingForce(const phyreal);

    /// gets the max speed where the steering it's forced
    const phyreal GetSpeedMaxSteeringForced() const;
    /// sets the max speed where the steering it's forced
    void SetSpeedMaxSteeringForced(const phyreal);

    /// gets the percentage of max steering
    const phyreal GetPercentageMaxSteering() const;
    /// sets the percentage of max steering
    void SetPercentageMaxSteering(const phyreal);

    /// sets a wheel suspension recover
    void SetSuspensionRecover(const unsigned int,const phyreal);
    /// gets a wheel suspension recover
    const phyreal GetSuspensionRecover(const unsigned int) const;

    /// gets a wheel suspension recover force
    const phyreal GetSuspensionRecoverForce(const unsigned int) const;

    /// sets wheel suspension recovery force for the front left wheel
    void SetFrontLeftWheelSuspensionRecoverForce(const phyreal);
    /// gets wheel suspension recovery force from the front left wheel
    const phyreal GetFrontLeftWheelSuspensionRecoverForce() const;
    /// sets wheel suspension recovery force for the front right wheel
    void SetFrontRightWheelSuspensionRecoverForce(const phyreal);
    /// gets wheel suspension recovery from the front right wheel
    const phyreal GetFrontRightWheelSuspensionRecoverForce() const;

    /// sets wheel suspension recovery force for the rear left wheel
    void SetRearLeftWheelSuspensionRecoverForce(const phyreal);
    /// gets wheel suspension recovery force from the rear left wheel
    const phyreal GetRearLeftWheelSuspensionRecoverForce() const;
    /// sets wheel suspension recovery force for the rear right wheel
    void SetRearRightWheelSuspensionRecoverForce(const phyreal);
    /// gets wheel suspension recovery force from the rear right wheel
    const phyreal GetRearRightWheelSuspensionRecoverForce() const;

    /// sets the air force
    void SetAirForce(const phyreal);
    /// gets the air force
    const phyreal GetAirForce() const;

    /// sets motor position
    void SetMotorPosition(const vector3);
    /// gets motor position
    vector3 GetMotorPosition() const;

        /// sets wheel traction for the front left wheel
        /// gets wheel traction from the front left wheel
        /// sets wheel traction for the front right wheel
        /// gets wheel traction from the front right wheel

        /// sets wheel traction for the rear left wheel
        /// gets wheel traction from the rear left wheel
        /// sets wheel traction for the rear right wheel
        /// gets wheel traction from the rear right wheel

        /// sets wheel direction for the front left wheel
        /// gets wheel direction from the front left wheel
        /// sets wheel direction for the front right wheel
        /// gets wheel direction from the front right wheel

        /// sets wheel direction for the rear left wheel
        /// gets wheel direction from the rear left wheel
        /// sets wheel direction for the rear right wheel
        /// gets wheel direction from the rear right wheel
        
        /// sets wheel steering max for the front left wheel
        /// gets wheel steering max from the front left wheel
        /// sets wheel steering max for the front right wheel
        /// gets wheel steering max from the front right wheel

        /// sets wheel steering max for the rear left wheel
        /// gets wheel steering max from the rear left wheel
        /// sets wheel steering max for the rear right wheel
        /// gets wheel steering max from the rear right wheel

        /// sets wheel steering forcea for the front left wheel
        /// gets wheel steering forcea from the front left wheel
        /// sets wheel steering forcea for the front right wheel
        /// gets wheel steering forcea from the front right wheel

        /// sets wheel steering forcea for the rear left wheel
        /// gets wheel steering forcea from the rear left wheel
        /// sets wheel steering forcea for the rear right wheel
        /// gets wheel steering forcea from the rear right wheel

        /// returns gravity center of the car (from the center of the car)
        /// set gravity center of the car (from the center of the car)

        /// sets how much BHPs (brake horse power) has
        /// gets how much BHPs (brake horse power) has

        /// sets the max revolutions of the engine
        /// gets the max revolutions of the engine

        /// sets the max force for the anti-roll bars
        /// gets the max force for the anti-roll bars

        /// sets the force for the anti-roll bars
        /// gets the force for the anti-roll bars

        /// sets the accelerate ratio
        /// gets the accelerate ratio

        /// sets the engine resistance
        /// gets the engine resistance

        /// sets the steer ratio
        /// gets the steer ratio
        
        /// sets the steer recover ratio
        /// gets the steer recover ratio

        /// sets wheel break treshold for the front left wheel
        /// gets wheel break treshold from the front left wheel
        /// sets wheel break treshold for the front right wheel
        /// gets wheel break treshold from the front right wheel

        /// sets wheel break treshold for the rear left wheel
        /// gets wheel break treshold from the rear left wheel
        /// sets wheel break treshold for the rear right wheel
        /// gets wheel break treshold from the rear right wheel
        
        /// sets the vehicle resistance to the air
        /// gets the vehicle resistance to the air

        /// sets wheel suspension recovery for the front left wheel
        /// gets wheel suspension recovery from the front left wheel
        /// sets wheel suspension recovery for the front right wheel
        /// gets wheel suspension recovery from the front right wheel

        /// sets wheel suspension recovery for the rear left wheel
        /// gets wheel suspension recovery from the rear left wheel
        /// sets wheel suspension recovery for the rear right wheel
        /// gets wheel suspension recovery from the rear right wheel

        /// sets the front right wheel
        /// gets the front right wheel
        
        /// sets the front left wheel
        /// gets the front left wheel

        /// sets the rear left wheel
        /// gets the rear left wheel

        /// sets the rear right wheel
        /// gets the rear right wheel
        /// sets if a wheel has traction
        /// sets if a wheel has direction
        /// sets the wheel steering max
        /// sets the wheel steering force
        /// gets a wheel steering force
        /// sets the wheel suspension recover
        /// gets the wheel suspension recover
        /// sets the wheel suspension recover force
        /// sets the wheel break treshold
        /// sets the wheel object name
        /// sets the steering wheel object
        /// gets the steering wheel object
        /// sets the backwards engine resistance
        /// gets the backwards engine resistance
        /// gets the breaking force
        /// sets the breaking force
        /// gets the max speed where the steering it's forced
        /// sets the max speed where the steering it's forced
        /// gets the percentage of max steering
        /// sets the percentage of max steering
        /// gets a wheel suspension recover force
        /// sets wheel suspension recovery force for the front left wheel
        /// gets wheel suspension recovery force from the front left wheel
        /// sets wheel suspension recovery force for the front right wheel
        /// gets wheel suspension recovery from the front right wheel
        /// sets wheel suspension recovery force for the rear left wheel
        /// gets wheel suspension recovery force from the rear left wheel
        /// sets wheel suspension recovery force for the rear right wheel
        /// gets wheel suspension recovery force from the rear right wheel
        /// sets the air force
        /// gets the air force

        /// sets motor position
        /// gets motor position

    // save state of the component
    bool SaveCmds(nPersistServer *);

private:
    /// defines wheel information
    struct InfoWheelPOD {
        
        InfoWheelPOD() : 
            traction(false), 
            direction(false), 
            objectName("newheel"),
            steeringMax(0),
            steeringForce(0),
            suspensionResistance(-1),
            suspensionRecovery(phyreal(0.25)),
            suspensionRecoveryForce(phyreal(0.01))
            {}

        bool traction;
        bool direction;
        
        phyreal steeringMax;

        phyreal steeringForce;

        nString objectName;

        phyreal suspensionResistance;

        phyreal suspensionRecovery;
        
        phyreal suspensionRecoveryForce;
    };

    /// stores wheels information
    InfoWheelPOD wheelsInfo[4];

    /// stores the offset of the gravity center
    vector3 gravityCenterOffset;

    /// stores the breaking force
    phyreal breakingForce;
    
    /// BHP
    phyreal BHPs;

    /// vehicle max revolutions
    phyreal maxRevolution;

    /// stores the max speed to force the steering
    phyreal maxSpeedForceSteering;

    /// stores the percentage of max steering at the max speed
    phyreal percentageMaxSteering;

    /// stores anti-roll bars max force
    phyreal antiRollBarsMaxForce;

    /// stores anti-roll bars force
    phyreal antiRollBarsForce;

    /// stores the ratio of acceleration
    phyreal accelerateRatio;

    /// stores the ratio of turning
    phyreal steerRatio;

    /// stores the engine resistance
    phyreal engineResistance;

    /// stores the wheel recover position ratio
    phyreal wheelRecover;

    /// stores the aerodynamic coeficient
    phyreal aerodynamicCoeficient;

    /// stores the steering wheel object name
    nString steeringWheelName;

    /// stores percentage reduction going backwards
    phyreal backwardsResistance;

    /// stores the air force
    phyreal airForce;

    /// stores relative motor position (for sound)
    vector3 motorPosition;

};

//-----------------------------------------------------------------------------
/**
    Returns the max force for the anti-roll bars.

    @param force a positive force magnitude

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
void ncPhyVehicleClass::SetMaxAntiRollForce( const phyreal force )
{
    n_assert2( force >= 0, "Error anti-roll bars max force can not be negative." );

    this->antiRollBarsMaxForce = force;
}

//-----------------------------------------------------------------------------
/**
    Returns the max force for the anti-roll bars.

    @return a positive force magnitude

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
const phyreal ncPhyVehicleClass::GetMaxAntiRollForce() const
{
    return this->antiRollBarsMaxForce;
}

//-----------------------------------------------------------------------------
/**
    Returns the force for the anti-roll bars.

    @return a positive force magnitude

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
const phyreal ncPhyVehicleClass::GetAntiRollForce() const
{
    return this->antiRollBarsForce;
}

//-----------------------------------------------------------------------------
/**
    Returns the force for the anti-roll bars.

    @param force a positive force magnitude

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
void ncPhyVehicleClass::SetAntiRollForce( const phyreal force )
{
    n_assert2( force >= 0, "Error anti-roll bars force can not be negavtive." );

    this->antiRollBarsForce = force;
}

//-----------------------------------------------------------------------------
/**
    Sets the accelerate ratio.

    @param ratio revolutions per second

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
void ncPhyVehicleClass::SetAccelerateRatio( const phyreal ratio )
{
    this->accelerateRatio = ratio;
}

//-----------------------------------------------------------------------------
/**
    Gets the accelerate ratio.

    @return revolutions per second

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
const phyreal ncPhyVehicleClass::GetAccelerateRatio() const
{
    return this->accelerateRatio;
}

//-----------------------------------------------------------------------------
/**
    Sets the engine resistance.

    @param resistance engine friction

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
void ncPhyVehicleClass::SetEngineResistance( const phyreal resistance )
{
    this->engineResistance = resistance;
}

//-----------------------------------------------------------------------------
/**
    Gets the engine resistance.

    @return engine friction

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
const phyreal ncPhyVehicleClass::GetEngineResistance() const
{
    return this->engineResistance;
}

//-----------------------------------------------------------------------------
/**
    Sets the steer ratio.

    @param ratio steering ratio per unit of time

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
void ncPhyVehicleClass::SetSteerRatio( const phyreal ratio )
{
    this->steerRatio = ratio;
}

//-----------------------------------------------------------------------------
/**
    Gets the steer ratio.

    @return steering ratio per unit of time

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
const phyreal ncPhyVehicleClass::GetSteerRatio() const
{
    return this->steerRatio;
}

//-----------------------------------------------------------------------------
/**
    Sets the steer recover ratio.

    @param ratio steering recovery ratio per unit of time

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
void ncPhyVehicleClass::SetSteerRecoverRatio( const phyreal ratio )
{
    this->wheelRecover = ratio;
}

//-----------------------------------------------------------------------------
/**
    Gets the steer recover ratio.

    @return steering recovery ratio per unit of time

    history:
        - 26-Apr-2005   Zombie         created
        - 06-May-2005   Zombie         moved to ncphyvehicleclass
*/
inline
const phyreal ncPhyVehicleClass::GetSteerRecoverRatio() const
{
    return this->wheelRecover;
}

#endif