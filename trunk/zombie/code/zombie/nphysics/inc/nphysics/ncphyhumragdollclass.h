#ifndef NC_PHYHUMRAGDOLLCLASS_H
#define NC_PHYHUMRAGDOLLCLASS_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyHumRagDollClass
    @ingroup NebulaPhysicsSystem
    @brief Base behaviour for humanoid rag-doll models.

    (C) 2005 Conjurer Services, S.A.
*/

//-----------------------------------------------------------------------------
#include "entity/nentity.h"

#include "nphysics/ncphysicsobjclass.h"

//-----------------------------------------------------------------------------
class ncPhyHumRagDollClass : public ncPhysicsObjClass
{

    NCOMPONENT_DECLARE(ncPhyHumRagDollClass,ncPhysicsObjClass);

public:
    /// constructor
    ncPhyHumRagDollClass();

    /// destructor
    ~ncPhyHumRagDollClass();

    /// sets the neck front minimun angle (x-axis)
    void SetNeckFrontAngleMin(const phyreal);
    /// sets the neck front maximun angle (x-axis)
    void SetNeckFrontAngleMax(const phyreal);
    
    /// sets the neck sides minumun angle (z-axis)
    void SetNeckSidesAngleMin(const phyreal);
    /// sets the neck sides maximun angle (z-axis)
    void SetNeckSidesAngleMax(const phyreal);

    /// sets the shoulder up-down minimun angle (z-axis)
    void SetShoulderUpDownAngleMin(const phyreal);
    /// sets the shoulder up-down maximun angle (z-axis)
    void SetShoulderUpDownAngleMax(const phyreal);

    /// sets the shoulder front minimun angle (x-axis)
    void SetShoulderFrontAngleMin(const phyreal);
    /// sets the shoulder front maximun angle (x-axis)
    void SetShoulderFrontAngleMax(const phyreal);

    /// sets the elbow up-down minimun angle (x-axis)
    void SetElbowUpDownAngleMin(const phyreal);
    /// sets the elbow up-down maximun angle (x-axis)
    void SetElbowUpDownAngleMax(const phyreal);

    /// sets the wrist up-down minimun angle (x-axis)
    void SetWristUpDownAngleMin(const phyreal);
    /// sets the wrist up-down maximun angle (x-axis)
    void SetWristUpDownAngleMax(const phyreal);
    
    /// sets the wrist sides minimun angle (y-axis)
    void SetWristSidesAngleMin(const phyreal);
    /// sets the wrist sides maximun angle (y-axis)
    void SetWristSidesAngleMax(const phyreal);

    /// sets the knee up-down minimun angle (x-axis)
    void SetKneeUpDownAngleMin(const phyreal);
    /// sets the knee up-down minimun angle (x-axis)
    void SetKneeUpDownAngleMax(const phyreal);

    // save state of the component
    bool SaveCmds(nPersistServer *);

#ifdef __ZOMBIE_EXPORTER__
    /// set human min-max angles
    void SetHumanAngles();
/** ZOMBIE REMOVE
    /// set strider min-max angles
    void SetStriderAngles();
*/
#endif

    // returns the neck front minimun angle (x-axis)
    phyreal GetNeckFrontAngleMin() const;
    // returns the neck front maximun angle (x-axis)
    phyreal GetNeckFrontAngleMax() const;
    
    // returns the neck sides minumun angle (z-axis)
    phyreal GetNeckSidesAngleMin() const;
    // returns the neck sides maximun angle (z-axis)
    phyreal GetNeckSidesAngleMax() const;

    // returns the shoulder up-down minimun angle (z-axis)
    phyreal GetShoulderUpDownAngleMin() const;
    // returns the shoulder up-down maximun angle (z-axis)
    phyreal GetShoulderUpDownAngleMax() const;

    // returns the shoulder front minimun angle (x-axis)
    phyreal GetShoulderFrontAngleMin() const;
    // returns the shoulder front maximun angle (x-axis)
    phyreal GetShoulderFrontAngleMax() const;

    // returns the elbow up-down minimun angle (x-axis)
    phyreal GetElbowUpDownAngleMin() const;
    // returns the elbow up-down maximun angle (x-axis)
    phyreal GetElbowUpDownAngleMax() const;

    // returns the wrist up-down minimun angle (x-axis)
    phyreal GetWristUpDownAngleMin() const;
    // returns the wrist up-down maximun angle (x-axis)
    phyreal GetWristUpDownAngleMax() const;
    
    // returns the wrist sides minimun angle (y-axis)
    phyreal GetWristSidesAngleMin() const;
    // returns the wrist sides maximun angle (y-axis)
    phyreal GetWristSidesAngleMax() const;

    // returns the knee up-down minimun angle (x-axis)
    phyreal GetKneeUpDownAngleMin() const;
    // returns the knee up-down minimun angle (x-axis)
    phyreal GetKneeUpDownAngleMax() const;

private:

    /// neck constants
    phyreal neckFrontAngleMin; // over x-axis
    phyreal neckFrontAngleMax; // over x-axis
    
    phyreal neckSidesAngleMin; // over z-axis
    phyreal neckSidesAngleMax; // over z-axis

    /// shoulder constants
    phyreal shoulderUpDownAngleMin; // over z-axis
    phyreal shoulderUpDownAngleMax; // over z-axis

    phyreal shoulderFrontAngleMin; // over x-axis
    phyreal shoulderFrontAngleMax; // over x-axis

    /// elbow constants
    phyreal elbowUpDownAngleMin; // over x-axis
    phyreal elbowUpDownAngleMax; // over x-axis

    /// wrist constants
    phyreal wristUpDownAngleMin; // over x-axis
    phyreal wristUpDownAngleMax; // over x-axis
    
    phyreal wristSidesAngleMin; // over y-axis
    phyreal wristSidesAngleMax; // over y-axis

    /// knee constants
    phyreal kneeUpDownAngleMin; // over x-axis
    phyreal kneeUpDownAngleMax; // over x-axis

};

//-----------------------------------------------------------------------------
#endif