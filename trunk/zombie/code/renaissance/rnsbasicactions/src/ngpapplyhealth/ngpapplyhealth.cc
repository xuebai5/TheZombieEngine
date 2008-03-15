#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngapplyhealth.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngpApplyHealth/ngpApplyHealth.h"

#include "rnsgameplay/ngameplayutils.h"
#include "ncgameplayliving/ncgameplayliving.h"
#include "nclogicanimator/nclogicanimator.h"
#include "mathlib/nmath.h"
#include "nnetworkmanager/nnetworkmanager.h"

nNebulaScriptClass(nGPApplyHealth, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPApplyHealth)
	    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPApplyHealth::nGPApplyHealth():
	livingEntity( 0 ),
		animator(0),
	finalHealth(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPApplyHealth::Init (nEntityObject * entity, int finalHealth)
{
    bool valid = entity != 0;   // Entity must exists

    if ( valid )
    {        
        this->livingEntity = entity->GetComponent<ncGameplayLiving>();
        this->animator = entity->GetComponent<ncLogicAnimator>();
        valid = (this->livingEntity != 0) && (this->animator != 0);
        n_assert( valid );
    }

    if ( valid )
    {
        this->finalHealth = finalHealth;
        this->entity = entity;        
        this->animator->SetInjection();
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPApplyHealth::Run( )
{
    n_assert( this->init );
    bool done = this->IsDone();

    if ( done )
    {
        if (nNetworkManager::Instance()->IsServer())
        {
            this->livingEntity->SetRecoverHealth(this->finalHealth);
        }
		this->livingEntity->SetDizzy(true);
    }   
    
    return done;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPApplyHealth::IsDone() const
{
    n_assert(this->init);

    return !this->animator || this->animator->HasFinished( -1 );
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
