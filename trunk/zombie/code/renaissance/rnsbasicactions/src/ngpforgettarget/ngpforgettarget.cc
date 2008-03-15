#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpforgettarget.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ngpforgettarget/ngpforgettarget.h"
#include "ncaistate/ncaistate.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "ncfsm/ncfsm.h"
#include "ntrigger/ngameevent.h"

nNebulaScriptClass(nGPForgetTarget, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPForgetTarget)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPForgetTarget::nGPForgetTarget() : 
    nGPBasicAction() 
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPForgetTarget::~nGPForgetTarget()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init
*/
bool
nGPForgetTarget::Init (nEntityObject* entity)
{
    bool valid = entity != 0;
    ncAIState* state = 0;

    if ( entity )
    {
        state = entity->GetComponent <ncAIState>();
        this->entity = entity;
    }
    else
    {
        valid = false;
    }

    if ( state )
    {
        ncGameplayLiving* living = this->entity->GetComponentSafe<ncGameplayLiving>();

        state->SetTarget (0);
        valid = valid && (living != 0);
    }

    this->init = valid;

    return valid;
}