#include "precompiled/pchrnsbasicactions.h"
/**
    @file ngpidle.cc
    @author Marc Ordinas i Llopis

    (C) 2006 Conjurer Services, S.A.
*/
#include "ngpidle/ngpidle.h"

#include "nclogicanimator/nclogicanimator.h"
#include "ncaimovengine/ncaimovengine.h"
#include "rnsgameplay/ngameplayutils.h"
#include "napplication/napplication.h"

nNebulaScriptClass(nGPIdle, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPIdle)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, float), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPIdle::nGPIdle():
    initTime   ( .0f ),
    finishTime ( .0f ),
    animator(0),
    idleSet(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPIdle::Init (nEntityObject * entity, float time)
{
    n_assert(entity);
    bool valid;

    this->finishTime = time;        
    this->animator = entity->GetComponentSafe<ncLogicAnimator>();
    valid = this->animator != 0;
    n_assert( valid );

    if ( valid )
    {
        this->initTime = static_cast<float> ( nApplication::Instance()->GetTime() );
        if( this->animator->HasFinishedAllAnims() )
        {
            this->animator->SetIdle();
            this->idleSet = true;
        }
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPIdle::Run( )
{
    bool done = false;
    n_assert( this->init );

    done = this->IsDone();

    if( !this->idleSet && (this->animator->HasFinishedAllAnims() || done) )
    {
        this->animator->SetIdle();
        this->idleSet = true;
    }

    return done;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPIdle::IsDone() const
{
    n_assert(this->init);

    float runTime = static_cast< float > ( nApplication::Instance()->GetTime() ) - this->initTime;

    return runTime > this->finishTime;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
