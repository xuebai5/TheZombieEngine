//-----------------------------------------------------------------------------
//  ncphyjoint_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"

#include "nphysics/ncphyjoint.h"

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyJoint)
    NSCRIPT_ADDCMD_COMPOBJECT('DMNT', void, Mount, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DUMT', void, UnMount, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSOA', void, SetObjectA, 1, (const nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGOA', const nEntityObjectId, GetObjectA, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSOB', void, SetObjectB, 1, (const nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGOB', const nEntityObjectId, GetObjectB, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSTJ', void, SetType, 1, (const nPhysicsJoint::JointTypes), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DGTJ', nPhysicsJoint::JointTypes, GetType, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSAP', void, SetAnchorPoint, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSFA', void, SetFirstAxis, 1, (const vector3&), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DTLA', void, SetLowStopAnglePosition, 2, (const phy::jointaxis, const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DTHA', void, SetHiStopAnglePosition, 2, (const phy::jointaxis, const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSDV', void, SetVelocity, 2, (const phy::jointaxis, const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSMF', void, SetMaximunForce, 2, (const phy::jointaxis, const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSFF', void, SetFudgeFactor, 2, (const phy::jointaxis, const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSBS', void, SetBouncynessStop, 2, (const phy::jointaxis, const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSCF', void, SetCFM, 2, (const phy::jointaxis, const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSSE', void, SetStopERP, 2, (const phy::jointaxis, const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('DSSC', void, SetStopCFM, 2, (const phy::jointaxis, const phyreal), 0, ());
#ifndef NGAME
#ifndef __ZOMBIE_EXPORTER__
    NSCRIPT_ADDCMD_COMPOBJECT('DMTS', void, MountSelected, 0, (), 0, ());
#endif
#endif
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------
namespace
{
inline
void HelperPersistantParametersJoint( nPhysicsJoint* joint, nEntityObject* obj, nPersistServer* server, nFourCC id, phy::jointparameter parameter, phy::jointaxis axis )
{  
    phyreal value(joint->GetParam( parameter, axis ));

    nCmd* cmd(server->GetCmd( obj, id ));

    n_assert2( cmd, "Error command not found" );
    
    cmd->In()->SetI( axis );
    cmd->In()->SetF( value );

    server->PutCmd(cmd);
}
}

//------------------------------------------------------------------------------
/**
    Object persistency.
*/
bool ncPhyJoint::SaveCmds( nPersistServer* server )
{
    if( server->GetSaveType() != nPersistServer::SAVETYPE_PERSIST )
    {
        return true;
    }

    /// persisting object A's id
    nCmd* cmd(server->GetCmd( this->entityObject, 'DSOA'));

    n_assert2( cmd, "Error command not found" );
    
    cmd->In()->SetI( this->idA );
   
    server->PutCmd(cmd);        

    /// persisting object B's id
    cmd = server->GetCmd( this->entityObject, 'DSOB');

    n_assert2( cmd, "Error command not found" );
    
    cmd->In()->SetI( this->idB );
   
    server->PutCmd(cmd);        
    
    /// persist the type of joint
    cmd = server->GetCmd( this->entityObject, 'DSTJ');

    n_assert2( cmd, "Error command not found" );
    
    cmd->In()->SetI( this->type );
   
    server->PutCmd(cmd);

    /// persist if mounted the mounting action
    if( this->mounted )
    {
        cmd = server->GetCmd( this->entityObject, 'DMNT');
    
        n_assert2( cmd, "Error command not found" );

        server->PutCmd(cmd);
    }

    /// getting anchor point if any
    vector3 anchor;

    if( this->GetAnchorPoint( anchor ) )
    {
        cmd = server->GetCmd( this->entityObject, 'DSAP');
    
        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetF( anchor.x );
        cmd->In()->SetF( anchor.y );
        cmd->In()->SetF( anchor.z );

        server->PutCmd(cmd);
    }

    /// getting first axis if any
    vector3 axis;

    if( this->GetFirstAxis( axis ) )
    {
        cmd = server->GetCmd( this->entityObject, 'DSFA');
    
        n_assert2( cmd, "Error command not found" );

        cmd->In()->SetF( axis.x );
        cmd->In()->SetF( axis.y );
        cmd->In()->SetF( axis.z );

        server->PutCmd(cmd);
    }

    if( !this->mounted )
        return true;

    /// persisting parameters

    // Low stop angle or position
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DTLA', phy::lowerStop, phy::axisA );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DTLA', phy::lowerStop, phy::axisB );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DTLA', phy::lowerStop, phy::axisC );

    // High stop angle or position
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DTHA', phy::higherStop, phy::axisA );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DTHA', phy::higherStop, phy::axisB );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DTHA', phy::higherStop, phy::axisC );

    // Desired motor velocity
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSDV', phy::velocity, phy::axisA );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSDV', phy::velocity, phy::axisB );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSDV', phy::velocity, phy::axisC );

    // The maximum force or torque that the motor will use to achieve the desired velocity
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSMF', phy::maxForce, phy::axisA );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSMF', phy::maxForce, phy::axisB );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSMF', phy::maxForce, phy::axisC );

    // Sets the fudge factor
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSFF', phy::fudgeFactor, phy::axisA );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSFF', phy::fudgeFactor, phy::axisB );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSFF', phy::fudgeFactor, phy::axisC );

    // Sets the bouncyness of the stops
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSBS', phy::bounce, phy::axisA );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSBS', phy::bounce, phy::axisB );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSBS', phy::bounce, phy::axisC );

    // Sets the constraint force mixing (CFM) value used when not at a stop
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSCF', phy::CFM, phy::axisA );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSCF', phy::CFM, phy::axisB );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSCF', phy::CFM, phy::axisC );

    // Sets the error reduction parameter (ERP) used by the stops
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSSE', phy::stopERP, phy::axisA );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSSE', phy::stopERP, phy::axisB );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSSE', phy::stopERP, phy::axisC );

    // Sets the constraint force mixing (CFM) value used by the stops
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSSC', phy::stopCFM, phy::axisA );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSSC', phy::stopCFM, phy::axisB );
    HelperPersistantParametersJoint( this->joint, this->entityObject, server, 'DSSC', phy::stopCFM, phy::axisC );

    return true;
}

//-----------------------------------------------------------------------------
// EOF
//-----------------------------------------------------------------------------
