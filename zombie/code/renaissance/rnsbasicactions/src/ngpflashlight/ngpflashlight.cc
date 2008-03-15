#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpflashlight.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngpflashlight/ngpflashlight.h"

#include "ncaimovengine/ncaimovengine.h"
#include "rnsgameplay/ngameplayutils.h"
#include "nclogicanimator/nclogicanimator.h"
#include "ncgpperception/ncgpsight.h"
#include "mathlib/cone.h"


nNebulaScriptClass(nGPFlashlight, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPFlashlight)
    NSCRIPT_ADDCMD('INIT', bool, Init, 1, (nEntityObject*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPFlashlight::nGPFlashlight():
animator( 0 )
{
    // empty
}


//------------------------------------------------------------------------------
/**
*/
bool
nGPFlashlight::Init (nEntityObject * entity)
{
    bool valid = entity != 0;   // Entity must exists

    if ( valid )
    {
        this->animator = entity->GetComponentSafe<ncLogicAnimator>();        
        valid =  this->animator != 0;
        n_assert( valid );
    }

    if ( valid )
    {        
        this->entity = entity;

        this->animIndex = this->animator->SetFlashlight();
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPFlashlight::IsDone() const
{
    n_assert(this->init);

    return !this->animator || this->animator->HasFinished( this->animIndex );
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
