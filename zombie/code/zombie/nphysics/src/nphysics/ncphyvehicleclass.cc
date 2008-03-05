//------------------------------------------------------------------------------
//  ncphyvehicleclass.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

#include "nphysics/ncphyvehicleclass.h"

//-----------------------------------------------------------------------------
nNebulaComponentClass(ncPhyVehicleClass, ncPhysicsObjClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyVehicleClass)
    NSCRIPT_ADDCMD_COMPCLASS('D001', void, SetFrontLeftWheelTraction, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D002', const bool, GetFrontLeftWheelTraction, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D003', void, SetFrontRightWheelTraction, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D004', const bool, GetFrontRightWheelTraction, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D005', void, SetRearLeftWheelTraction, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D006', const bool, GetRearLeftWheelTraction, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D007', void, SetRearRightWheelTraction, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D008', const bool, GetRearRightWheelTraction, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D009', void, SetFrontLeftWheelDirection, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D010', const bool, GetFrontLeftWheelDirection, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D011', void, SetFrontRightWheelDirection, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D012', const bool, GetFrontRightWheelDirection, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D013', void, SetRearLeftWheelDirection, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D014', const bool, GetRearLeftWheelDirection, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D015', void, SetRearRightWheelDirection, 1, (const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D016', const bool, GetRearRightWheelDirection, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D017', void, SetFrontLeftWheelSteeringMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D018', const phyreal, GetFrontLeftWheelSteeringMax, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D019', void, SetFrontRightWheelSteeringMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D020', const phyreal, GetFrontRightWheelSteeringMax, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D021', void, SetRearLeftWheelSteeringMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D022', const phyreal, GetRearLeftWheelSteeringMax, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D023', void, SetRearRightWheelSteeringMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D024', const phyreal, GetRearRightWheelSteeringMax, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D025', void, SetFrontLeftWheelSteeringForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D026', const phyreal, GetFrontLeftWheelSteeringForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D027', void, SetFrontRightWheelSteeringForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D028', const phyreal, GetFrontRightWheelSteeringForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D029', void, SetRearLeftWheelSteeringForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D030', const phyreal, GetRearLeftWheelSteeringForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D031', void, SetRearRightWheelSteeringForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D032', const phyreal, GetRearRightWheelSteeringForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D033', const vector3&, GetGravityCenterOffset, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D034', void, SetGravityCenterOffset, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D035', void, SetBHPs, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D036', const phyreal, GetBHPs, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D037', void, SetMaxRevolution, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D038', const phyreal, GetMaxRevolution, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D039', void, SetMaxAntiRollForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D040', const phyreal, GetMaxAntiRollForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D041', void, SetAntiRollForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D042', const phyreal, GetAntiRollForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D043', void, SetAccelerateRatio, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D044', const phyreal, GetAccelerateRatio, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D045', void, SetEngineResistance, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D046', const phyreal, GetEngineResistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D047', void, SetSteerRatio, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D048', const phyreal, GetSteerRatio, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D049', void, SetSteerRecoverRatio, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D050', const phyreal, GetSteerRecoverRatio, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D051', void, SetFrontLeftWheelBreakTreshold, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D052', const phyreal, GetFrontLeftWheelBreakTreshold, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D053', void, SetFrontRightWheelBreakTreshold, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D054', const phyreal, GetFrontRightWheelBreakTreshold, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D055', void, SetRearLeftWheelBreakTreshold, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D056', const phyreal, GetRearLeftWheelBreakTreshold, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D057', void, SetRearRightWheelBreakTreshold, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D058', const phyreal, GetRearRightWheelBreakTreshold, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D059', void, SetAirResistance, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D060', const phyreal, GetAirResistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D061', void, SetFrontLeftWheelSuspensionRecovery, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D062', const phyreal, GetFrontLeftWheelSuspensionRecovery, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D063', void, SetFrontRightWheelSuspensionRecovery, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D064', const phyreal, GetFrontRightWheelSuspensionRecovery, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D065', void, SetRearLeftWheelSuspensionRecovery, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D066', const phyreal, GetRearLeftWheelSuspensionRecovery, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D067', void, SetRearRightWheelSuspensionRecovery, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D068', const phyreal, GetRearRightWheelSuspensionRecovery, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D069', void, SetFrontRightWheel, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D070', const nString&, GetFrontRightWheel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D071', void, SetFrontLeftWheel, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D072', const nString&, GetFrontLeftWheel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D073', void, SetRearLeftWheel, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D074', const nString&, GetRearLeftWheel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D075', void, SetRearRightWheel, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D076', const nString&, GetRearRightWheel, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D077', void, SetHasTraction, 2, (const unsigned int,const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D078', void, SetHasDirection, 2, (const unsigned int,const bool), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D079', void, SetSteeringMax, 2, (const unsigned int,const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D080', void, SetSteeringForce, 2, (const unsigned int,const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D081', const phyreal, GetSteeringForce, 1, (const unsigned int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D082', void, SetSuspensionRecoverForce, 2, (const unsigned int,const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D083', void, SetBreakTreshold, 2, (const unsigned int,const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D084', void, SetWheelObjectName, 2, (const unsigned int,const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D085', void, SetSteeringWheelObjectName, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D086', const nString&, GetSteeringWheelObjectName, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D087', void, SetBackwardsResistance, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D088', const phyreal, GetBackwardsResistance, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D089', const phyreal, GetBreakingForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D090', void, SetBreakingForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D091', const phyreal, GetSpeedMaxSteeringForced, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D092', void, SetSpeedMaxSteeringForced, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D093', const phyreal, GetPercentageMaxSteering, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D094', void, SetPercentageMaxSteering, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D095', void, SetSuspensionRecover, 2, (const unsigned int,const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D096', const phyreal, GetSuspensionRecover, 1, (const unsigned int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D097', const phyreal, GetSuspensionRecoverForce, 1, (const unsigned int), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D098', void, SetFrontLeftWheelSuspensionRecoverForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D099', const phyreal, GetFrontLeftWheelSuspensionRecoverForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D100', void, SetFrontRightWheelSuspensionRecoverForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D101', const phyreal, GetFrontRightWheelSuspensionRecoverForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D102', void, SetRearLeftWheelSuspensionRecoverForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D103', const phyreal, GetRearLeftWheelSuspensionRecoverForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D104', void, SetRearRightWheelSuspensionRecoverForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D105', const phyreal, GetRearRightWheelSuspensionRecoverForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D106', void, SetAirForce, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('D107', const phyreal, GetAirForce, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JSMP', void, SetMotorPosition, 1, (const vector3), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('JGMP', vector3, GetMotorPosition, 0, (), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 06-May-2005   David Reyes    created
*/

ncPhyVehicleClass::ncPhyVehicleClass():
    gravityCenterOffset(0,0,0),
    BHPs(40),
    maxRevolution(70),
    antiRollBarsMaxForce(15),
    antiRollBarsForce(300),
    accelerateRatio(50),
    engineResistance(phyreal(.5)),
    steerRatio(2),
    wheelRecover(phyreal(.005)),
    aerodynamicCoeficient(phyreal(0.3)),
    steeringWheelName( "None" ),
    backwardsResistance(phyreal(1)),
    breakingForce(60),
    maxSpeedForceSteering(50),
    percentageMaxSteering(phyreal(.56)),
    airForce(10),
    motorPosition(0,0,0)
{
    // Empty
};

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 06-May-2005   David Reyes    created
*/
ncPhyVehicleClass::~ncPhyVehicleClass()
{
    // Empty
}

//-----------------------------------------------------------------------------
/**
    Returns the number of wheels.

    @return number of vehicle wheels

    history:
        - 06-May-2005   David Reyes    created
*/
const unsigned int ncPhyVehicleClass::GetNumWheels() const
{
    return 4;
}


//-----------------------------------------------------------------------------
/**
    Sets wheel traction for the front left wheel.

    @param has traction

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontLeftWheelTraction( const bool has )
{
    this->wheelsInfo[ frontleft ].traction = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel traction from the front left wheel.

    @return traction

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetFrontLeftWheelTraction() const
{
    return this->wheelsInfo[ frontleft ].traction;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel traction for the front right wheel.

    @param has traction

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontRightWheelTraction( const bool has )
{
    this->wheelsInfo[ frontright ].traction = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel traction from the front right wheel.

    @return traction

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetFrontRightWheelTraction() const
{
    return this->wheelsInfo[ frontright ].traction;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel traction for the rear left wheel.

    @param has traction

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearLeftWheelTraction( const bool has )
{
    this->wheelsInfo[ rearleft ].traction = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel traction from the rear left wheel.

    @return traction

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetRearLeftWheelTraction() const
{
    return this->wheelsInfo[ rearleft ].traction;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel traction for the rear right wheel.

    @return traction

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearRightWheelTraction( const bool has )
{
    this->wheelsInfo[ rearright ].traction = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel traction from the rear right wheel.

    @return traction

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetRearRightWheelTraction() const
{
    return this->wheelsInfo[ rearright ].traction;
}

//-----------------------------------------------------------------------------
/**
    Set gravity center of the car (from the center of the car).

    @param offset vehicle's mass offset

    history:
        - 20-Dec-2004   David Reyes    created
        - 06-May-2005   David Reyes    moved to ncphyvehicleclass
*/
void ncPhyVehicleClass::SetGravityCenterOffset( const vector3& offset )
{
    this->gravityCenterOffset = offset;
}

//-----------------------------------------------------------------------------
/**
    Returns gravity center of the car (from the center of the car).

    @return vehicle's mass offset

    history:
        - 20-Dec-2004   David Reyes    created
        - 06-May-2005   David Reyes    moved to ncphyvehicleclass
*/
const vector3& ncPhyVehicleClass::GetGravityCenterOffset() const
{
    return this->gravityCenterOffset;
}

//-----------------------------------------------------------------------------
/**
    Sets how much BHPs (brake horse power) has.

    @param bhps BHPs

    history:
        - 04-Nov-2004   David Reyes    created
        - 06-May-2005   David Reyes    moved to ncphyvehicleclass
*/
void ncPhyVehicleClass::SetBHPs( const phyreal bhps )
{
    n_assert2( bhps >= 0, "The number of BHPs cannot be negative" );

    this->BHPs = bhps;
}

//-----------------------------------------------------------------------------
/**
    Returns the BHPs (brake hourse power).

    @return bhps

    history:
        - 06-May-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetBHPs() const
{
    return this->BHPs;
}


//-----------------------------------------------------------------------------
/**
    Sets the max revolutions of the engine.

    @param revolutions engine revolutions / 1000   

    history:
        - 22-Nov-2004   David Reyes    created
        - 06-May-2005   David Reyes    moved to ncphyvehicleclass
*/
void ncPhyVehicleClass::SetMaxRevolution( phyreal revolutions )
{
    n_assert2( revolutions > 0, "Revolutions of a vehicle's engine has to be bigger than zero." );

    this->maxRevolution = revolutions;
}



//-----------------------------------------------------------------------------
/**
    Returns the max revolutions.

    @return revolutions

    history:
        - 22-Nov-2004   David Reyes    created
        - 06-May-2005   David Reyes    moved to ncphyvehicleclass
*/
const phyreal ncPhyVehicleClass::GetMaxRevolution() const
{
    return this->maxRevolution;
}


//-----------------------------------------------------------------------------
/**
    Sets the vehicle resistance to the air.

    @pararm resistance aerodynamic coeficient (0..1)

    history:
        - 11-Aug-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetAirResistance(const phyreal resistance)
{
    n_assert2( resistance >= 0, "The range of this value goes from zero to one." );
    n_assert2( resistance <= 1, "The range of this value goes from zero to one." );
    this->aerodynamicCoeficient = resistance;
}

//-----------------------------------------------------------------------------
/**
    Returns the air resistance coeficient.

    @return aerodynamic coeficient (0..1)

    history:
        - 11-Aug-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetAirResistance() const
{
    return this->aerodynamicCoeficient;
}

//-----------------------------------------------------------------------------
/**
    Save state of the component.

    history:
        - 06-May-2005   David Reyes    created
*/
bool ncPhyVehicleClass::SaveCmds(nPersistServer *server)
{
    if( !ncPhysicsObjClass::SaveCmds( server ) )
    {
        return false;
    }

    nCmd* cmd(0);

    /// persisting wheels information
    for( unsigned int wheelIndex(0); wheelIndex < this->GetNumWheels(); ++wheelIndex )
    {
        /// persisting wheel traction
        cmd = server->GetCmd( this->GetEntityClass(), 'D077');

        n_assert2( cmd, "Failed to find command." );

        cmd->In()->SetI( wheelIndex );
        cmd->In()->SetB( this->GetHasTraction( wheelIndex ) );

        server->PutCmd(cmd);

        /// persisting wheel direction
        cmd = server->GetCmd( this->GetEntityClass(), 'D078');

        n_assert2( cmd, "Failed to find command." );

        cmd->In()->SetI( wheelIndex );
        cmd->In()->SetB( this->GetHasDirection( wheelIndex ) );

        server->PutCmd(cmd);

        /// persisting wheel steering maximun
        cmd = server->GetCmd( this->GetEntityClass(), 'D079');

        n_assert2( cmd, "Failed to find command." );

        cmd->In()->SetI( wheelIndex );
        cmd->In()->SetF( this->GetSteeringMax( wheelIndex ) );

        server->PutCmd(cmd);

        /// persisting wheel steering force
        cmd = server->GetCmd( this->GetEntityClass(), 'D080');

        n_assert2( cmd, "Failed to find command." );

        cmd->In()->SetI( wheelIndex );
        cmd->In()->SetF( this->GetSteeringForce( wheelIndex ) );

        server->PutCmd(cmd);

        /// persisting wheel object
        cmd = server->GetCmd( this->GetEntityClass(), 'D084');

        n_assert2( cmd, "Failed to find command." );

        cmd->In()->SetI( wheelIndex );
        cmd->In()->SetS( this->GetWheelObjectName( wheelIndex ).Get() );

        server->PutCmd(cmd);

        /// persisting wheel break
        cmd = server->GetCmd( this->GetEntityClass(), 'D083');

        n_assert2( cmd, "Failed to find command." );

        cmd->In()->SetI( wheelIndex );
        cmd->In()->SetF( this->GetBreakTreshold( wheelIndex ) );

        server->PutCmd(cmd);

        /// persisting suspension recovery
        cmd = server->GetCmd( this->GetEntityClass(), 'D095');

        n_assert2( cmd, "Failed to find command." );

        cmd->In()->SetI( wheelIndex );
        cmd->In()->SetF( this->GetSuspensionRecover( wheelIndex ) );

        server->PutCmd(cmd);

        /// persisting suspension recovery force
        cmd = server->GetCmd( this->GetEntityClass(), 'D082');

        n_assert2( cmd, "Failed to find command." );

        cmd->In()->SetI( wheelIndex );
        cmd->In()->SetF( this->GetSuspensionRecoverForce( wheelIndex ) );

        server->PutCmd(cmd);

    } // wheels info persisting end

    /// persisting vehicle centre of gravity
    cmd = server->GetCmd( this->GetEntityClass(), 'D034');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetGravityCenterOffset().x );
    cmd->In()->SetF( this->GetGravityCenterOffset().y );
    cmd->In()->SetF( this->GetGravityCenterOffset().z );

    server->PutCmd(cmd);

    /// persisting vehicle BHPs
    cmd = server->GetCmd( this->GetEntityClass(), 'D035');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetBHPs() );

    server->PutCmd(cmd);

    /// persisting vehicle max speed
    cmd = server->GetCmd( this->GetEntityClass(), 'D037');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetMaxRevolution() );

    server->PutCmd(cmd);

    /// persisting vehicle max force for the anti-roll bars
    cmd = server->GetCmd( this->GetEntityClass(), 'D039');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetMaxAntiRollForce() );

    server->PutCmd(cmd);

    /// persisting vehicle force for the anti-roll bars
    cmd = server->GetCmd( this->GetEntityClass(), 'D041');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetAntiRollForce() );

    server->PutCmd(cmd);

    /// persisting vehicle accelerate ratio
    cmd = server->GetCmd( this->GetEntityClass(), 'D043');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetAccelerateRatio() );

    server->PutCmd(cmd);

    /// persisting vehicle engine resistance
    cmd = server->GetCmd( this->GetEntityClass(), 'D045');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetEngineResistance() );

    server->PutCmd(cmd);

    /// persisting vehicle steer ratio
    cmd = server->GetCmd( this->GetEntityClass(), 'D047');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetSteerRatio() );

    server->PutCmd(cmd);

    /// persisting vehicle steer ratio recovery
    cmd = server->GetCmd( this->GetEntityClass(), 'D049');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetSteerRecoverRatio() );

    server->PutCmd(cmd);

    /// persists the aerodynamic coeficient
    cmd = server->GetCmd( this->GetEntityClass(), 'D059');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetAirResistance() );

    server->PutCmd(cmd);

    /// persists the steering wheel object name
    cmd = server->GetCmd( this->GetEntityClass(), 'D085');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetS( GetSteeringWheelObjectName().Get() );

    server->PutCmd(cmd);

    /// persists the backward resistance
    cmd = server->GetCmd( this->GetEntityClass(), 'D087');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetBackwardsResistance() );

    server->PutCmd(cmd);

    /// persists the braking force
    cmd = server->GetCmd( this->GetEntityClass(), 'D090');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetBreakingForce() );

    server->PutCmd(cmd);

    /// persists the max speed to enforce steering
    cmd = server->GetCmd( this->GetEntityClass(), 'D092');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetSpeedMaxSteeringForced() );

    server->PutCmd(cmd);

    /// persists percentage of max steering
    cmd = server->GetCmd( this->GetEntityClass(), 'D094');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetPercentageMaxSteering() );

    server->PutCmd(cmd);

    /// persists air force
    cmd = server->GetCmd( this->GetEntityClass(), 'D106');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetAirForce() );

    server->PutCmd(cmd);

    /// persists motor position
    cmd = server->GetCmd( this->GetEntityClass(), 'JSMP');

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetMotorPosition().x );
    cmd->In()->SetF( this->GetMotorPosition().y );
    cmd->In()->SetF( this->GetMotorPosition().z );

    server->PutCmd(cmd);

    return true;
}

//-----------------------------------------------------------------------------
/**
    Sets the front right wheel.

    @param objectName wheel class name

    history:
        - 21-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontRightWheel( const nString& objectName )
{
    this->wheelsInfo[ frontright ].objectName = objectName;
}

//-----------------------------------------------------------------------------
/**
    Gets the front right wheel.

    @return wheel class name

    history:
        - 21-Dec-2005   David Reyes    created
*/
const nString& ncPhyVehicleClass::GetFrontRightWheel() const
{
    return this->wheelsInfo[ frontright ].objectName;
}

//-----------------------------------------------------------------------------
/**
    Sets the front left wheel.

    @param objectName wheel class name

    history:
        - 21-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontLeftWheel( const nString& objectName )
{
    this->wheelsInfo[ frontleft ].objectName = objectName;    
}

//-----------------------------------------------------------------------------
/**
    Gets the front left wheel.

    @return wheel class name

    history:
        - 21-Dec-2005   David Reyes    created
*/
const nString& ncPhyVehicleClass::GetFrontLeftWheel() const
{
    return this->wheelsInfo[ frontleft ].objectName;
}

//-----------------------------------------------------------------------------
/**
    Sets the rear left wheel.

    @param objectName wheel class name

    history:
        - 21-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearLeftWheel( const nString& objectName )
{
    this->wheelsInfo[ rearleft ].objectName = objectName;    
}

//-----------------------------------------------------------------------------
/**
    Gets the rear left wheel.

    @return wheel class name

    history:
        - 21-Dec-2005   David Reyes    created
*/
const nString& ncPhyVehicleClass::GetRearLeftWheel() const
{
    return this->wheelsInfo[ rearleft ].objectName;
}

//-----------------------------------------------------------------------------
/**
    Sets the rear right wheel.

    @param objectName wheel class name

    history:
        - 21-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearRightWheel( const nString& objectName )
{
    this->wheelsInfo[ rearright ].objectName = objectName;    
}

//-----------------------------------------------------------------------------
/**
    Gets the rear right wheel.

    @return wheel class name

    history:
        - 21-Dec-2005   David Reyes    created
*/
const nString& ncPhyVehicleClass::GetRearRightWheel() const
{
    return this->wheelsInfo[ rearright ].objectName;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel direction for the front left wheel.

    @param has direction

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontLeftWheelDirection( const bool has )
{
    this->wheelsInfo[ frontleft ].direction = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel direction from the front left wheel.

    @param has direction

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetFrontLeftWheelDirection() const
{
    return this->wheelsInfo[ frontleft ].direction;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel direction for the front right wheel.

    @param has direction

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontRightWheelDirection( const bool has )
{
    this->wheelsInfo[ frontright ].direction = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel direction from the front right wheel.

    @param has direction

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetFrontRightWheelDirection() const
{
    return this->wheelsInfo[ frontright ].direction;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel direction for the rear left wheel.

    @param has direction

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearLeftWheelDirection( const bool has )
{
    this->wheelsInfo[ rearleft ].direction = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel direction from the rear left wheel.

    @param has direction

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetRearLeftWheelDirection() const
{
    return this->wheelsInfo[ rearleft ].direction;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel direction for the rear right wheel.

    @param has direction

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearRightWheelDirection( const bool has )
{
    this->wheelsInfo[ rearright ].direction = has;
}


//-----------------------------------------------------------------------------
/**
    Gets wheel direction from the rear right wheel.

    @param has direction

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetRearRightWheelDirection() const
{
    return this->wheelsInfo[ rearright ].direction;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel steering max for the front left wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontLeftWheelSteeringMax( const phyreal has )
{
    this->wheelsInfo[ frontleft ].steeringMax = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel steering max from the front left wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontLeftWheelSteeringMax() const
{
    return this->wheelsInfo[ frontleft ].steeringMax;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel steering max for the front right wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontRightWheelSteeringMax( const phyreal has )
{
    this->wheelsInfo[ frontright ].steeringMax = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel steering max from the front right wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontRightWheelSteeringMax() const
{
    return this->wheelsInfo[ frontright ].steeringMax;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel steering max for the rear left wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearLeftWheelSteeringMax( const phyreal has )
{
    this->wheelsInfo[ rearleft ].steeringMax = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel steering max from the rear left wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearLeftWheelSteeringMax() const
{
    return this->wheelsInfo[ rearleft ].steeringMax;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel steering max for the rear right wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearRightWheelSteeringMax( const phyreal has )
{
    this->wheelsInfo[ rearright ].steeringMax = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel steering max for the rear right wheel.

    @return steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearRightWheelSteeringMax() const
{
    return this->wheelsInfo[ rearright ].steeringMax;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel steering max for the front left wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontLeftWheelSteeringForce( const phyreal has )
{
    this->wheelsInfo[ frontleft ].steeringForce = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel steering max from the front left wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontLeftWheelSteeringForce() const
{
    return this->wheelsInfo[ frontleft ].steeringForce;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel steering max for the front right wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontRightWheelSteeringForce( const phyreal has )
{
    this->wheelsInfo[ frontright ].steeringForce = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel steering max from the front right wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontRightWheelSteeringForce() const
{
    return this->wheelsInfo[ frontright ].steeringForce;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel steering max for the rear left wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearLeftWheelSteeringForce( const phyreal has )
{
    this->wheelsInfo[ rearleft ].steeringForce = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel steering max from the rear left wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearLeftWheelSteeringForce() const
{
    return this->wheelsInfo[ rearleft ].steeringForce;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel steering max for the rear right wheel.

    @return steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearRightWheelSteeringForce( const phyreal has )
{
    this->wheelsInfo[ rearright ].steeringForce = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel steering max for the rear right wheel.

    @param has steering max

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearRightWheelSteeringForce() const
{
    return this->wheelsInfo[ rearright ].steeringForce;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel break treshold for the front left wheel.

    @param has break treshold

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontLeftWheelBreakTreshold( const phyreal has )
{
    this->wheelsInfo[ frontleft ].suspensionResistance = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel break treshold from the front left wheel.

    @param has break treshold

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontLeftWheelBreakTreshold() const
{
    return this->wheelsInfo[ frontleft ].suspensionResistance;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel break treshold for the front right wheel.

    @param has break treshold

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontRightWheelBreakTreshold( const phyreal has )
{
    this->wheelsInfo[ frontright ].suspensionResistance = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel break treshold from the front right wheel.

    @param has break treshold

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontRightWheelBreakTreshold() const
{
    return this->wheelsInfo[ frontright ].suspensionResistance;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel break treshold for the rear left wheel.

    @param has break treshold

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearLeftWheelBreakTreshold( const phyreal has )
{
    this->wheelsInfo[ rearleft ].suspensionResistance = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel break treshold from the rear left wheel.

    @param has break treshold

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearLeftWheelBreakTreshold() const
{
    return this->wheelsInfo[ rearleft ].suspensionResistance;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel break treshold for the rear right wheel.

    @return break treshold

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearRightWheelBreakTreshold( const phyreal has )
{
    this->wheelsInfo[ rearright ].suspensionResistance = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel break treshold for the rear right wheel.

    @param has break treshold

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearRightWheelBreakTreshold() const
{
    return this->wheelsInfo[ rearright ].suspensionResistance;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel suspension recovery for the front left wheel.

    @param has suspension recovery

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontLeftWheelSuspensionRecovery( const phyreal has )
{
    this->wheelsInfo[ frontleft ].suspensionRecovery = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel suspension recovery from the front left wheel.

    @param has suspension recovery

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontLeftWheelSuspensionRecovery() const
{
    return this->wheelsInfo[ frontleft ].suspensionRecovery;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel suspension recovery for the front right wheel.

    @param has suspension recovery

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontRightWheelSuspensionRecovery( const phyreal has )
{
    this->wheelsInfo[ frontright ].suspensionRecovery = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel suspension recovery from the front right wheel.

    @param has suspension recovery

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontRightWheelSuspensionRecovery() const
{
    return this->wheelsInfo[ frontright ].suspensionRecovery;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel suspension recovery for the rear left wheel.

    @param has suspension recovery

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearLeftWheelSuspensionRecovery( const phyreal has )
{
    this->wheelsInfo[ rearleft ].suspensionRecovery = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel suspension recovery from the rear left wheel.

    @param has suspension recovery

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearLeftWheelSuspensionRecovery() const
{
    return this->wheelsInfo[ rearleft ].suspensionRecovery;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel suspension recovery for the rear right wheel.

    @return suspension recovery

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetRearRightWheelSuspensionRecovery( const phyreal has )
{
    this->wheelsInfo[ rearright ].suspensionRecovery = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel suspension recovery for the rear right wheel.

    @param has suspension recovery

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearRightWheelSuspensionRecovery() const
{
    return this->wheelsInfo[ rearright ].suspensionRecovery;
}

//-----------------------------------------------------------------------------
/**
    Sets if a wheel has traction.

    @param index wheel index
    @param has true/false

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetHasTraction(const unsigned int index, const bool has )
{
    n_assert2( index < this->GetNumWheels(), "Index out of bounds." );

    this->wheelsInfo[ index ].traction = has;
}

//-----------------------------------------------------------------------------
/**
    Gets if a wheel has traction.

    @param index wheel index
    @return true/false

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetHasTraction( const unsigned int wheel ) const
{
    n_assert2( wheel < this->GetNumWheels(), "Index out of bounds." );

    return this->wheelsInfo[ wheel ].traction;
}

//-----------------------------------------------------------------------------
/**
    Sets if a wheel has direction.

    @param index wheel index
    @param has true/false

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetHasDirection(const unsigned int index, const bool has )
{
    n_assert2( index < this->GetNumWheels(), "Index out of bounds." );

    this->wheelsInfo[ index ].direction = has;
}

//-----------------------------------------------------------------------------
/**
    Gets if a wheel has direction.

    @param index wheel index
    @return true/false

    history:
        - 22-Dec-2005   David Reyes    created
*/
const bool ncPhyVehicleClass::GetHasDirection( const unsigned int wheel ) const
{
    n_assert2( wheel < this->GetNumWheels(), "Index out of bounds." );

    return this->wheelsInfo[ wheel ].direction;
}

//-----------------------------------------------------------------------------
/**
    Sets a wheel steering max.

    @param index wheel index
    @param value value

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetSteeringMax(const unsigned int index, const phyreal value )
{
    n_assert2( index < this->GetNumWheels(), "Index out of bounds." );

    this->wheelsInfo[ index ].steeringMax = value;
}

//-----------------------------------------------------------------------------
/**
    Gets a wheel steering max.

    @param index wheel index
    @return value

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetSteeringMax( const unsigned int wheel ) const
{
    n_assert2( wheel < this->GetNumWheels(), "Index out of bounds." );

    return this->wheelsInfo[ wheel ].steeringMax;
}

//-----------------------------------------------------------------------------
/**
    Sets a wheel steering force.

    @param index wheel index
    @param value value

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetSteeringForce(const unsigned int index, const phyreal value )
{
    n_assert2( index < this->GetNumWheels(), "Index out of bounds." );

    this->wheelsInfo[ index ].steeringForce = value;
}

//-----------------------------------------------------------------------------
/**
    Gets a wheel steering force.

    @param index wheel index
    @return value

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetSteeringForce( const unsigned int wheel ) const
{
    n_assert2( wheel < this->GetNumWheels(), "Index out of bounds." );

    return this->wheelsInfo[ wheel ].steeringForce;
}

//-----------------------------------------------------------------------------
/**
    Sets a wheel suspension recover.

    @param index wheel index
    @param value value

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetSuspensionRecover(const unsigned int index, const phyreal value )
{
    n_assert2( index < this->GetNumWheels(), "Index out of bounds." );

    this->wheelsInfo[ index ].suspensionRecovery = value;
}

//-----------------------------------------------------------------------------
/**
    Gets a wheel suspension recover.

    @param index wheel index
    @return value

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetSuspensionRecover( const unsigned int wheel ) const
{
    n_assert2( wheel < this->GetNumWheels(), "Index out of bounds." );

    return this->wheelsInfo[ wheel ].suspensionRecovery;
}

//-----------------------------------------------------------------------------
/**
    Sets a wheel suspension recover force.

    @param index wheel index
    @param value value

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetSuspensionRecoverForce(const unsigned int index, const phyreal value )
{
    n_assert2( index < this->GetNumWheels(), "Index out of bounds." );

    this->wheelsInfo[ index ].suspensionRecoveryForce = value;
}

//-----------------------------------------------------------------------------
/**
    Gets a wheel suspension recover force.

    @param index wheel index
    @return value

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetSuspensionRecoverForce( const unsigned int wheel ) const
{
    n_assert2( wheel < this->GetNumWheels(), "Index out of bounds." );

    return this->wheelsInfo[ wheel ].suspensionRecoveryForce;
}

//-----------------------------------------------------------------------------
/**
    Sets a wheel break treshold.

    @param index wheel index
    @param value value

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetBreakTreshold(const unsigned int index, const phyreal value )
{
    n_assert2( index < this->GetNumWheels(), "Index out of bounds." );

    this->wheelsInfo[ index ].suspensionResistance = value;
}

//-----------------------------------------------------------------------------
/**
    Gets a wheel break treshold.

    @param index wheel index
    @return value

    history:
        - 22-Dec-2005   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetBreakTreshold( const unsigned int wheel ) const
{
    n_assert2( wheel < this->GetNumWheels(), "Index out of bounds." );

    return this->wheelsInfo[ wheel ].suspensionResistance;
}

//-----------------------------------------------------------------------------
/**
    Sets a wheel object.

    @param wheel wheel index
    @param name wheel object name

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetWheelObjectName( const unsigned int wheel, const nString& name )
{
    n_assert2( wheel < this->GetNumWheels(), "Index out of bounds." );

    this->wheelsInfo[ wheel ].objectName = name;
}

//-----------------------------------------------------------------------------
/**
    Gets a wheel break treshold.

    @param wheel wheel index
    @return wheel object name

    history:
        - 22-Dec-2005   David Reyes    created
*/
const nString& ncPhyVehicleClass::GetWheelObjectName( const unsigned int wheel ) const
{
    n_assert2( wheel < this->GetNumWheels(), "Index out of bounds." );

    return this->wheelsInfo[ wheel ].objectName;
}

//-----------------------------------------------------------------------------
/**
    Sets the steering wheel object.

    @param wheel wheel index
    @return wheel object name

    history:
        - 22-Dec-2005   David Reyes    created
*/
void ncPhyVehicleClass::SetSteeringWheelObjectName( const nString& objectName )
{
    this->steeringWheelName = objectName;
}

//-----------------------------------------------------------------------------
/**
    Gets the steering wheel object.

    @param wheel wheel index
    @return wheel object name

    history:
        - 22-Dec-2005   David Reyes    created
*/
const nString& ncPhyVehicleClass::GetSteeringWheelObjectName() const
{
    return this->steeringWheelName;
}


//-----------------------------------------------------------------------------
/**
    Sets the backwards engine resistance.

    @param newvalue resistance in percentage where 0% completely resistant

    history:
        - 06-Feb-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetBackwardsResistance( const phyreal newvalue )
{
    this->backwardsResistance = newvalue;
}

//-----------------------------------------------------------------------------
/**
    Gets the backwards engine resistance.

    @return resistance in percentage where 0% completely resistant

    history:
        - 06-Feb-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetBackwardsResistance() const
{
    return this->backwardsResistance;
}

//-----------------------------------------------------------------------------
/**
    Gets the breaking force.

    @return force

    history:
        - 27-Feb-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetBreakingForce() const
{
    return this->breakingForce;
}

//-----------------------------------------------------------------------------
/**
    Sets the breaking force.

    @param newforce braking force

    history:
        - 27-Feb-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetBreakingForce( const phyreal newforce )
{
    this->breakingForce = newforce;
}

//-----------------------------------------------------------------------------
/**
    Gets the max speed where the steering it's forced.

    @return max speed

    history:
        - 27-Feb-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetSpeedMaxSteeringForced() const
{
    return this->maxSpeedForceSteering;
}

//-----------------------------------------------------------------------------
/**
    Sets the max speed where the steering it's forced.

    @param newspeed max speed to force steering

    history:
        - 27-Feb-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetSpeedMaxSteeringForced( const phyreal newspeed )
{
    this->maxSpeedForceSteering = newspeed;
}

//-----------------------------------------------------------------------------
/**
    Gets the percentage of max steering.

    @return percentage 

    history:
        - 27-Feb-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetPercentageMaxSteering() const
{
    return this->percentageMaxSteering;
}
//-----------------------------------------------------------------------------
/**
    Sets the percentage of max steering.

    @param newpercentage percentage 

    history:
        - 27-Feb-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetPercentageMaxSteering( const phyreal newpercentage )
{
    n_assert2( newpercentage >= 0, "The percentage has to be between 1 and 0." );
    n_assert2( newpercentage <= 1, "The percentage has to be between 1 and 0." );
#ifndef NGAME
    if( newpercentage < 0 )
    {
        this->percentageMaxSteering = 0;
        return;
    }
    if( newpercentage > 1 )
    {
        this->percentageMaxSteering = 1;
        return;
    }
#endif
    this->percentageMaxSteering = newpercentage;
}
//-----------------------------------------------------------------------------
/**
    Sets wheel suspension recovery force for the front left wheel.

    @param has suspension recovery

    history:
        - 03-Mar-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontLeftWheelSuspensionRecoverForce( const phyreal has )
{
    this->wheelsInfo[ frontleft ].suspensionRecoveryForce = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel suspension recovery force from the front left wheel.

    @param has suspension recovery

    history:
        - 03-Mar-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontLeftWheelSuspensionRecoverForce() const
{
    return this->wheelsInfo[ frontleft ].suspensionRecoveryForce;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel suspension recovery force for the front right wheel.

    @param has suspension recovery

    history:
        - 03-Mar-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetFrontRightWheelSuspensionRecoverForce( const phyreal has )
{
    this->wheelsInfo[ frontright ].suspensionRecoveryForce = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel suspension recovery force from the front right wheel.

    @param has suspension recovery

    history:
        - 03-Mar-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetFrontRightWheelSuspensionRecoverForce() const
{
    return this->wheelsInfo[ frontright ].suspensionRecoveryForce;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel suspension recovery force for the rear left wheel.

    @param has suspension recovery

    history:
        - 03-Mar-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetRearLeftWheelSuspensionRecoverForce( const phyreal has )
{
    this->wheelsInfo[ rearleft ].suspensionRecoveryForce = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel suspension recovery force from the rear left wheel.

    @param has suspension recovery

    history:
        - 03-Mar-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearLeftWheelSuspensionRecoverForce() const
{
    return this->wheelsInfo[ rearleft ].suspensionRecoveryForce;
}

//-----------------------------------------------------------------------------
/**
    Sets wheel suspension recovery force for the rear right wheel.

    @return suspension recovery

    history:
        - 03-Mar-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetRearRightWheelSuspensionRecoverForce( const phyreal has )
{
    this->wheelsInfo[ rearright ].suspensionRecoveryForce = has;
}

//-----------------------------------------------------------------------------
/**
    Gets wheel suspension recovery force for the rear right wheel.

    @param has suspension recovery

    history:
        - 03-Mar-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetRearRightWheelSuspensionRecoverForce() const
{
    return this->wheelsInfo[ rearright ].suspensionRecoveryForce;
}

//-----------------------------------------------------------------------------
/**
    Sets the air force.

    @param new air force in newtons

    history:
        - 03-Mar-2006   David Reyes    created
*/
void ncPhyVehicleClass::SetAirForce( const phyreal newvalue )
{
    this->airForce = newvalue;
}
//-----------------------------------------------------------------------------
/**
    Gets the air force.

    @return air force in newtons

    history:
        - 03-Mar-2006   David Reyes    created
*/
const phyreal ncPhyVehicleClass::GetAirForce() const
{
    return this->airForce;
}

//-----------------------------------------------------------------------------
/**
    Sets the motor position (used for sounds)

    @param relative motor position

    history:
        - 22-Mar-2006   Juanjo Luna    created
*/
void ncPhyVehicleClass::SetMotorPosition( const vector3 motorPos )
{
    this->motorPosition = motorPos;
}
//-----------------------------------------------------------------------------
/**
    Gets the motor position (used for sounds)

    @return air force in newtons

    history:
        - 22-Mar-2006   Juanjo Luna    created
*/
vector3 ncPhyVehicleClass::GetMotorPosition() const
{
    return this->motorPosition;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------

