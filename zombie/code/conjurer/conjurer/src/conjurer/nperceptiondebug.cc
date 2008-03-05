#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nperceptiondebug.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nperceptiondebug.h"

#ifndef NGAME

nNebulaClass(nPerceptionDebug, "ndebugmodule");

//------------------------------------------------------------------------------
/**
    constructor
*/
nPerceptionDebug::nPerceptionDebug():
optionFlags(0x00000000)
{

}

//------------------------------------------------------------------------------
/**
    destructor
*/
nPerceptionDebug::~nPerceptionDebug()
{

}

//------------------------------------------------------------------------------
/**
    Called when the module is created through the debug server.
    Here debug options should be registered into the server.
*/
void
nPerceptionDebug::OnDebugModuleCreate()
{
    this->RegisterDebugFlag("perception",   "Activate/Deactivate Perception",         "Pereception");
    this->RegisterDebugFlag("perchearing",  "Activate/Deactivate Hearing Perception", "Pereception");
    this->RegisterDebugFlag("percfeeling",  "Activate/Deactivate Feeling Perception", "Pereception");
    this->RegisterDebugFlag("percsight",    "Activate/Deactivate Sight Perception",   "Pereception");
}

//------------------------------------------------------------------------------
/**
    Called when an option is checked in.
    The module should retrieve debug options to optimize access.
*/
void
nPerceptionDebug::OnDebugOptionUpdated()
{
    if (this->GetFlagEnabled("perception"))   this->optionFlags |= Perception;
    if (this->GetFlagEnabled("perchearing"))  this->optionFlags |= PerceptionHearing;
    if (this->GetFlagEnabled("percfeeling"))  this->optionFlags |= PerceptionFeeling;
    if (this->GetFlagEnabled("percsight"))    this->optionFlags |= PerceptionSight;
}

#endif // NGAME
