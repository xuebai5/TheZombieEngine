#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngprepairvehicle.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngprepairvehicle/ngprepairvehicle.h"
#include "entity/nentityobject.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPRepairVehicle, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPRepairVehicle)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPRepairVehicle::nGPRepairVehicle() : 
    nGPBasicAction() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPRepairVehicle::~nGPRepairVehicle()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPRepairVehicle::Init (nEntityObject* entity, nEntityObject* vehicle)
{
    // @TODO
    this->vehicle = vehicle;
    this->entity = entity;
    this->init = true;

    return true;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPRepairVehicle::IsDone() const
{
    // @TODO:
    return nGPBasicAction::IsDone();
}