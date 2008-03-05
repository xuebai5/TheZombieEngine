#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpentervehicle.cc
//------------------------------------------------------------------------------

#include "ngpturnvehicle/ngpturnvehicle.h"
#include "entity/nentityobject.h"
#include "nphysics/ncphyvehicle.h"

nNebulaScriptClass(nGPTurnVehicle, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPTurnVehicle)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*,const float), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPTurnVehicle::nGPTurnVehicle()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPTurnVehicle::~nGPTurnVehicle()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPTurnVehicle::Init (nEntityObject* eVehicle, const float value )
{
    eVehicle->GetComponent<ncPhyVehicle>()->SetSteering( value );

    return true;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPTurnVehicle::IsDone() const
{
    return true;
}

/// Main loop
bool nGPTurnVehicle::Run()
{
    return true; // done
}
