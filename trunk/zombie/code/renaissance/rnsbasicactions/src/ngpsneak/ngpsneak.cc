#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpsneak.cc
//------------------------------------------------------------------------------

#include "ngpsneak/ngpsneak.h"
#include "entity/nentityobject.h"

nNebulaScriptClass(nGPSneak, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPSneak)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPSneak::nGPSneak() : 
    nGPBasicAction() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPSneak::~nGPSneak()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPSneak::Init (nEntityObject* entity)
{
    // @TODO:
    this->entity = entity;
    this->init = true;

    return true;
}

//------------------------------------------------------------------------------
/**
    IsDone
*/
bool
nGPSneak::IsDone() const
{
    // @TODO:
    return nGPBasicAction::IsDone();
}