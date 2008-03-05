#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpclimbladder.cc
//------------------------------------------------------------------------------

#include "ngpclimbladder/ngpclimbladder.h"
#include "entity/nentityobject.h"

nNebulaScriptClass(nGPClimbLadder, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPClimbLadder)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPClimbLadder::nGPClimbLadder() : 
    nGPBasicAction() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPClimbLadder::~nGPClimbLadder()
{
    this->ladder = 0;
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPClimbLadder::Init (nEntityObject* entity, nEntityObject* ladder)
{
    // @TODO:
    this->ladder = ladder;
    this->entity = entity;
    this->init = true;

    return true;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPClimbLadder::IsDone() const
{
    // @TODO:
    return nGPBasicAction::IsDone();
}