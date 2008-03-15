#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpjump.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngpjump/ngpjump.h"
#include "entity/nentityobject.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPJump, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPJump)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, const vector3&), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPJump::nGPJump() : 
    nGPBasicAction() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPJump::~nGPJump()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPJump::Init (nEntityObject* entity, const vector3& goal)
{
    // @TODO:
    this->goal = goal;
    this->entity = entity;
    this->init = true;

    return true;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPJump::IsDone() const
{
    // @TODO:
    return nGPBasicAction::IsDone();
}