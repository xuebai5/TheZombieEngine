#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpentervehicle.cc
//------------------------------------------------------------------------------

#include "ngpmovevehicle/ngpmovevehicle.h"
#include "entity/nentityobject.h"
#include "nphysics/ncphyvehicle.h"

nNebulaScriptClass(nGPMoveVehicle, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPMoveVehicle)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*,const float), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPMoveVehicle::nGPMoveVehicle()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPMoveVehicle::~nGPMoveVehicle()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPMoveVehicle::Init (nEntityObject* eVehicle, const float value )
{
    eVehicle->GetComponent<ncPhyVehicle>()->SetAcceleration( value );

    return true;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPMoveVehicle::IsDone() const
{
    return true;
}

/// Main loop
bool nGPMoveVehicle::Run()
{
    return true; // done
}
