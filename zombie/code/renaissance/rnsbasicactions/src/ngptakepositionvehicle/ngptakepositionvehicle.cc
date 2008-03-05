#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngptakepositionvehicle.cc
//------------------------------------------------------------------------------

#include "ngptakepositionvehicle/ngptakepositionvehicle.h"
#include "entity/nentityobject.h"

nNebulaScriptClass(nGPTakePositionVehicle, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPTakePositionVehicle)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPTakePositionVehicle::nGPTakePositionVehicle() : 
    nGPBasicAction() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPTakePositionVehicle::~nGPTakePositionVehicle()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPTakePositionVehicle::Init (nEntityObject* entity, nEntityObject* vehicle)
{
    // @TODO:
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
nGPTakePositionVehicle::IsDone() const
{
    // @TODO:
    return nGPBasicAction::IsDone();
}