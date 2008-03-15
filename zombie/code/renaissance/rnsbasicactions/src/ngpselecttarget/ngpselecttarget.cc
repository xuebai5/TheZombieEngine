#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpselecttarget.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "ngpselecttarget/ngpselecttarget.h"
#include "ncaistate/ncaistate.h"
#include "zombieentity/nctransform.h"
#include "nspatial/nspatialserver.h"

nNebulaScriptClass(nGPSelectTarget, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPSelectTarget)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor 
*/
nGPSelectTarget::nGPSelectTarget() : 
    nGPBasicAction()    
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
nGPSelectTarget::~nGPSelectTarget()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Init

    @params the entity to select a target
*/
bool
nGPSelectTarget::Init (nEntityObject* entity, nEntityObject* target)
{
    n_assert(entity);

    bool init = true;
    ncAIState* state = entity->GetComponent <ncAIState>();
    n_assert(state);

    if ( state )
    {
        state->SetTarget (target);
        this->entity = entity;
    }
    else
    {
        init = false;
    }

    this->init = init;

    return init;
}
