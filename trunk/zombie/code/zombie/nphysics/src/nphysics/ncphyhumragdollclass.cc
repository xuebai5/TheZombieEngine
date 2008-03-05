//------------------------------------------------------------------------------
//  ncphyhumragdollclass.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchnphysics.h"
#include "nphysics/ncphyhumragdollclass.h"

//-----------------------------------------------------------------------------
nNebulaComponentClass(ncPhyHumRagDollClass,ncPhysicsObjClass);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncPhyHumRagDollClass)
    NSCRIPT_ADDCMD_COMPCLASS('DC00', void, SetNeckFrontAngleMin, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC01', void, SetNeckFrontAngleMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC02', void, SetNeckSidesAngleMin, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC03', void, SetNeckSidesAngleMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC04', void, SetShoulderUpDownAngleMin, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC05', void, SetShoulderUpDownAngleMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC06', void, SetShoulderFrontAngleMin, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC07', void, SetShoulderFrontAngleMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC08', void, SetElbowUpDownAngleMin, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC09', void, SetElbowUpDownAngleMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC10', void, SetWristUpDownAngleMin, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC11', void, SetWristUpDownAngleMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC12', void, SetWristSidesAngleMin, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC13', void, SetWristSidesAngleMax, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC14', void, SetKneeUpDownAngleMin, 1, (const phyreal), 0, ());
    NSCRIPT_ADDCMD_COMPCLASS('DC15', void, SetKneeUpDownAngleMax, 1, (const phyreal), 0, ());
NSCRIPT_INITCMDS_END()


//-----------------------------------------------------------------------------

namespace {
    const phyreal pi_value( phyreal(M_PI) ); // NOTE: if pi is too exact can overflow
}

//-----------------------------------------------------------------------------
/**
    Constructor.

    history:
        - 06-May-2005   David Reyes    created
*/

ncPhyHumRagDollClass::ncPhyHumRagDollClass() : 
    neckFrontAngleMin(phyreal(-.3)), // over x-axis
    neckFrontAngleMax(phyreal(.3)), // over x-axis
    neckSidesAngleMin(phyreal(-.3)), // over z-axis
    neckSidesAngleMax(phyreal(.3)), // over z-axis
    shoulderUpDownAngleMin(-pi_value*phyreal(.75)*phyreal(.5)), // over z-axis
    shoulderUpDownAngleMax(phyreal(.1)*phyreal(.5)), // over z-axis
    shoulderFrontAngleMin(phyreal(-.2)*phyreal(.5)), // over x-axis
    shoulderFrontAngleMax(pi_value*phyreal(.75)*phyreal(.5)), // over x-axis
    elbowUpDownAngleMin(phyreal(pi_value/2.)*phyreal(.5)), // over x-axis
    elbowUpDownAngleMax(phyreal(0)), // over x-axis
    wristUpDownAngleMin(phyreal(-pi_value/2.5)*phyreal(.5)), // over x-axis
    wristUpDownAngleMax(phyreal(pi_value/2.5)*phyreal(.5)), // over x-axis
    wristSidesAngleMin(phyreal(-.6)*phyreal(.5)), // over z-axis
    wristSidesAngleMax(phyreal(.6)*phyreal(.5)), // over z-axis
    kneeUpDownAngleMin(phyreal(0)), // over x-axis
    kneeUpDownAngleMax(phyreal(-pi_value/2.)*phyreal(.5)) // over x-axis
{
};

//-----------------------------------------------------------------------------
/**
    Destructor.

    history:
        - 06-May-2005   David Reyes    created
*/
ncPhyHumRagDollClass::~ncPhyHumRagDollClass()
{
}

//-----------------------------------------------------------------------------
/**
    Returns the neck front minimun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetNeckFrontAngleMin() const
{
    return this->neckFrontAngleMin;
}

//-----------------------------------------------------------------------------
/**
    Returns the neck front maximun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetNeckFrontAngleMax() const
{
    return this->neckFrontAngleMax;
}

//-----------------------------------------------------------------------------
/**
    Returns the neck sides minumun angle (z-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetNeckSidesAngleMin() const
{
    return this->neckSidesAngleMin;
}

//-----------------------------------------------------------------------------
/**
    Returns the neck sides maximun angle (z-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetNeckSidesAngleMax() const
{
    return this->neckSidesAngleMax;
}

//-----------------------------------------------------------------------------
/**
    Returns the shoulder up-down minimun angle (z-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetShoulderUpDownAngleMin() const
{
    return this->shoulderUpDownAngleMin;
}

//-----------------------------------------------------------------------------
/**
    Returns the shoulder up-down maximun angle (z-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetShoulderUpDownAngleMax() const
{
    return this->shoulderUpDownAngleMax;
}

//-----------------------------------------------------------------------------
/**
    Returns the shoulder front minimun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetShoulderFrontAngleMin() const
{
    return this->shoulderFrontAngleMin;
}

//-----------------------------------------------------------------------------
/**
    Returns the shoulder front maximun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetShoulderFrontAngleMax() const
{
    return this->shoulderFrontAngleMax;
}

//-----------------------------------------------------------------------------
/**
    Returns the elbow up-down minimun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetElbowUpDownAngleMin() const
{
    return this->elbowUpDownAngleMin;
}

//-----------------------------------------------------------------------------
/**
    Returns the elbow up-down maximun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetElbowUpDownAngleMax() const
{
    return this->elbowUpDownAngleMax;
}

//-----------------------------------------------------------------------------
/**
    Returns the wrist up-down minimun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetWristUpDownAngleMin() const
{
    return this->wristUpDownAngleMin;
}

//-----------------------------------------------------------------------------
/**
    Returns the wrist up-down maximun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetWristUpDownAngleMax() const
{
    return this->wristUpDownAngleMax;
}

//-----------------------------------------------------------------------------
/**
    Returns the wrist sides minimun angle (y-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetWristSidesAngleMin() const
{
    return this->wristSidesAngleMin;
}

//-----------------------------------------------------------------------------
/**
    Returns the wrist sides maximun angle (y-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetWristSidesAngleMax() const
{
    return this->wristSidesAngleMax;
}

//-----------------------------------------------------------------------------
/**
    Returns the knee up-down minimun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetKneeUpDownAngleMin() const
{
    return this->kneeUpDownAngleMin;
}

//-----------------------------------------------------------------------------
/**
    Returns the knee up-down maximun angle (x-axis).

    @return angle

    history:
        - 09-May-2005   David Reyes    created
*/
phyreal ncPhyHumRagDollClass::GetKneeUpDownAngleMax() const
{
    return this->kneeUpDownAngleMax;
}


//-----------------------------------------------------------------------------
/**
    Save state of the component.

    history:
        - 06-May-2005   David Reyes    created
*/
bool ncPhyHumRagDollClass::SaveCmds(nPersistServer *server)
{
    if( !nComponentClass::SaveCmds( server ) )
    {
        return false;
    }

    nCmd *cmd(0);

    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC00' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetNeckFrontAngleMin() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC01' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetNeckFrontAngleMax() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC02' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetNeckSidesAngleMin() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC03' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetNeckSidesAngleMax() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC04' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetShoulderUpDownAngleMin() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC05' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetShoulderUpDownAngleMax() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC06' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetShoulderFrontAngleMin() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC07' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetShoulderFrontAngleMax() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC08' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetElbowUpDownAngleMin() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC09' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetElbowUpDownAngleMax() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC10' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetWristUpDownAngleMin() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC11' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetWristUpDownAngleMax() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC12' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetWristSidesAngleMin() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC13' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetWristSidesAngleMax() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC14' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetKneeUpDownAngleMin() );

    server->PutCmd(cmd);        
    ///------
    cmd = server->GetCmd( this->GetEntityClass(), 'DC15' );

    n_assert2( cmd, "Failed to find command." );

    cmd->In()->SetF( this->GetKneeUpDownAngleMax() );

    server->PutCmd(cmd);        

    return true;
}

//-----------------------------------------------------------------------------
/**
    Returns the neck front minimun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetNeckFrontAngleMin( const phyreal value )
{
    this->neckFrontAngleMin = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the neck front maximun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetNeckFrontAngleMax( const phyreal value )
{
    this->neckFrontAngleMax = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the neck sides minumun angle (z-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetNeckSidesAngleMin( const phyreal value )
{
    this->neckSidesAngleMin = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the neck sides maximun angle (z-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetNeckSidesAngleMax( const phyreal value )
{
    this->neckSidesAngleMax = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the shoulder up-down minimun angle (z-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetShoulderUpDownAngleMin( const phyreal value )
{
    this->shoulderUpDownAngleMin = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the shoulder up-down maximun angle (z-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetShoulderUpDownAngleMax( const phyreal value )
{
    this->shoulderUpDownAngleMax = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the shoulder front minimun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetShoulderFrontAngleMin( const phyreal value )
{
    this->shoulderFrontAngleMin = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the shoulder front maximun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetShoulderFrontAngleMax( const phyreal value )
{
    this->shoulderFrontAngleMax = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the elbow up-down minimun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetElbowUpDownAngleMin( const phyreal value )
{
    this->elbowUpDownAngleMin = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the elbow up-down maximun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetElbowUpDownAngleMax( const phyreal value )
{
    this->elbowUpDownAngleMax = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the wrist up-down minimun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetWristUpDownAngleMin( const phyreal value )
{
    this->wristUpDownAngleMin = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the wrist up-down maximun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetWristUpDownAngleMax( const phyreal value )
{
    this->wristUpDownAngleMax = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the wrist sides minimun angle (y-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetWristSidesAngleMin( const phyreal value )
{
    this->wristSidesAngleMin = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the wrist sides maximun angle (y-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetWristSidesAngleMax( const phyreal value )
{
    this->wristSidesAngleMax = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the knee up-down minimun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetKneeUpDownAngleMin( const phyreal value )
{
    this->kneeUpDownAngleMin = value;
}

//-----------------------------------------------------------------------------
/**
    Returns the knee up-down maximun angle (x-axis).

    @param value angle

    history:
        - 09-May-2005   David Reyes    created
*/
void ncPhyHumRagDollClass::SetKneeUpDownAngleMax( const phyreal value )
{
    this->kneeUpDownAngleMax = value;
}

#ifdef __ZOMBIE_EXPORTER__
//-----------------------------------------------------------------------------
/**
*/
void 
ncPhyHumRagDollClass::SetHumanAngles()
{
    this->neckFrontAngleMin = phyreal(-.3); // over x-axis
    this->neckFrontAngleMax = phyreal(.3); // over x-axis
    this->neckSidesAngleMin = phyreal(-.3); // over z-axis
    this->neckSidesAngleMax = phyreal(.3); // over z-axis
    this->shoulderUpDownAngleMin = -pi_value*phyreal(.75)*phyreal(.5); // over z-axis
    this->shoulderUpDownAngleMax = phyreal(.1)*phyreal(.5); // over z-axis
    this->shoulderFrontAngleMin = phyreal(-.2)*phyreal(.5); // over x-axis
    this->shoulderFrontAngleMax = pi_value*phyreal(.75)*phyreal(.5); // over x-axis
    this->elbowUpDownAngleMin = phyreal(0); // over x-axis
    this->elbowUpDownAngleMax = phyreal(pi_value/2.)*phyreal(.5); // over x-axis
    this->wristUpDownAngleMin = phyreal(-pi_value/2.5)*phyreal(.5); // over x-axis
    this->wristUpDownAngleMax = phyreal(pi_value/2.5)*phyreal(.5); // over x-axis
    this->wristSidesAngleMin = phyreal(-.6)*phyreal(.5); // over z-axis
    this->wristSidesAngleMax = phyreal(.6)*phyreal(.5); // over z-axis
    this->kneeUpDownAngleMin = phyreal(-pi_value/2.)*phyreal(.5); // over x-axis
    this->kneeUpDownAngleMax = phyreal(0); // over x-axis
}

//-----------------------------------------------------------------------------
/**
*/
/** ZOMBIE REMOVE
void 
ncPhyHumRagDollClass::SetStriderAngles()
{
    this->neckFrontAngleMin = phyreal(-.3); // over x-axis
    this->neckFrontAngleMax = phyreal(.3); // over x-axis
    this->neckSidesAngleMin = phyreal(-.3); // over z-axis
    this->neckSidesAngleMax = phyreal(.3); // over z-axis
    this->shoulderUpDownAngleMin = -pi_value*phyreal(.75)*phyreal(.5); // over z-axis
    this->shoulderUpDownAngleMax = phyreal(.1)*phyreal(.5); // over z-axis
    this->shoulderFrontAngleMin = phyreal(-.2)*phyreal(.5); // over x-axis
    this->shoulderFrontAngleMax = pi_value*phyreal(.75)*phyreal(.5); // over x-axis
    this->elbowUpDownAngleMin = phyreal(0); // over x-axis
    this->elbowUpDownAngleMax = phyreal(pi_value/2.)*phyreal(.5); // over x-axis
    this->wristUpDownAngleMin = phyreal(-pi_value/2.5)*phyreal(.5); // over x-axis
    this->wristUpDownAngleMax = phyreal(pi_value/2.5)*phyreal(.5); // over x-axis
    this->wristSidesAngleMin = phyreal(-.6)*phyreal(.5); // over z-axis
    this->wristSidesAngleMax = phyreal(.6)*phyreal(.5); // over z-axis
    this->kneeUpDownAngleMin = phyreal(-pi_value/2.)*phyreal(.5); // over x-axis
    this->kneeUpDownAngleMax = phyreal(0); // over x-axis
}
*/
#endif
