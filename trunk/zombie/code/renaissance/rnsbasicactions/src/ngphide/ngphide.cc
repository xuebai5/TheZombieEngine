#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngphide.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngphide/ngphide.h"
#include "entity/nentityobject.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nGPHide, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPHide)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, const vector3&), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPHide::nGPHide() : 
    nGPBasicAction() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPHide::~nGPHide()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPHide::Init (nEntityObject* entity, const vector3& point)
{
    // @TODO:
    this->orign = point;
    this->entity = entity;
    this->init = true;

    return true;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPHide::IsDone() const
{
    // @TODO:
    return nGPBasicAction::IsDone();
}