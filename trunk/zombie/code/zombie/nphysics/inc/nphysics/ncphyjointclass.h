#ifndef NC_PHYJOINTCLASS_H
#define NC_PHYJOINTCLASS_H

//-----------------------------------------------------------------------------
/**
    @class ncPhyJointClass
    @ingroup NebulaPhysicsSystem
    @brief Base behaviour for entities that represents any type of joint.

    (C) 2005 Conjurer Services, S.A.
*/


//-----------------------------------------------------------------------------
#include "entity/nentity.h"

#include "nphysics/ncphysicsobjclass.h"
//-----------------------------------------------------------------------------

#ifdef NGAME
class ncPhyJointClass : public nComponentClass
{

    NCOMPONENT_DECLARE(ncPhyJointClass,nComponentClass);

#else
class ncPhyJointClass : public ncPhysicsObjClass
{

    NCOMPONENT_DECLARE(ncPhyJointClass,ncPhysicsObjClass);

#endif

public:
    /// constructor
    ncPhyJointClass();

    /// destructor
    ~ncPhyJointClass();

    // sets the kind of the joint
    void SetTypeJoint(int);
    // gets the kind of the joint
    int GetTypeJoint() const;

    // save state of the component
    bool SaveCmds(nPersistServer *);

private:

    // stores the joint's type
    int jointType;

};

#endif