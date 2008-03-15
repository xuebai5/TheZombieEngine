#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngwaitsometime.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngpwaitsometime/ngpwaitsometime.h"

#include "ncaimovengine/ncaimovengine.h"
#include "rnsgameplay/ngameplayutils.h"
#include "napplication/napplication.h"
#include "nclogicanimator/nclogicanimator.h"

nNebulaScriptClass(nGPWaitSomeTime, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN (nGPWaitSomeTime)
    NSCRIPT_ADDCMD('INIT', bool, Init, 2, (nEntityObject*, float), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPWaitSomeTime::nGPWaitSomeTime():
initTime   ( .0f ),
finishTime ( .0f )
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPWaitSomeTime::Init (nEntityObject * entity, float time)
{
    bool valid = entity != 0;   // Entity must exists
    ncAIMovEngine* moveEngine = 0;

    if ( valid )
    {
        this->finishTime = time;        
        moveEngine = entity->GetComponentSafe<ncAIMovEngine>();
        valid =  (moveEngine != 0) ;
        n_assert( valid );
    }

    if ( valid )
    {
        this->initTime = static_cast<float> ( nApplication::Instance()->GetTime() );
        entity->GetComponentSafe<ncLogicAnimator>()->SetIdle();
        moveEngine->Stop();
    }

    this->init = valid;

    return valid;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPWaitSomeTime::Run( )
{
    n_assert( this->init )

    return this->IsDone();
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPWaitSomeTime::IsDone() const
{
    n_assert(this->init);

    float runTime = static_cast< float > ( nApplication::Instance()->GetTime() ) - this->initTime;

    return runTime > this->finishTime;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
