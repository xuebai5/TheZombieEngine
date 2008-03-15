#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpcover.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ngpcover/ngpcover.h"
#include "entity/nentityobject.h"

nNebulaScriptClass(nGPCover, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPCover)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPCover::nGPCover() : 
    nGPBasicAction() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPCover::~nGPCover()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPCover::Init (nEntityObject* entity)
{
    // @TODO:
    this->entity = entity;
    this->init = bool (entity != 0);

    return this->init;;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPCover::IsDone() const
{
    // @TODO:
    return nGPBasicAction::IsDone();
}