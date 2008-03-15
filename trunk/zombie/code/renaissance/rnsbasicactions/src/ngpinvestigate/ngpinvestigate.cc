#include "precompiled/pchrnsbasicactions.h"
//------------------------------------------------------------------------------
//  ngpinvestigate.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ngpinvestigate/ngpinvestigate.h"

#include "nclogicanimator/nclogicanimator.h"
#include "ncaimovengine/ncaimovengine.h"
#include "ncagentmemory/ncagentmemory.h"
#include "ncaistate/ncaistate.h"
#include "ntrigger/ntriggerserver.h"
#include "zombieentity/nctransform.h"

#include "napplication/napplication.h"

nNebulaScriptClass(nGPInvestigate, "ngpbasicaction");

//------------------------------------------------------------------------------
/**
Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN(nGPInvestigate)
    NSCRIPT_ADDCMD('INIT', bool, Init, 3, (nEntityObject*, bool,nTime), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
*/
nGPInvestigate::nGPInvestigate()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPInvestigate::Init (nEntityObject * entity, bool m, nTime time )
{
    n_assert(entity);
    bool valid;
    ncAIState* state = entity->GetComponentSafe<ncAIState>();
    this->movEngine = entity->GetComponentSafe<ncAIMovEngine>();

    this->move = m;

    valid = ( state != 0 ) && ( this->movEngine != 0 );
    if ( valid )
    {
        vector3 position;
        if( this->FindInvestPoint( position ) )
        {
            this->movEngine->SetMovMode(ncAIMovEngine::MOV_RUN);
            this->movEngine->SetMovDirection(ncAIMovEngine::MD_FORWARD);
            if( move ) {
                this->movEngine->MoveTo( position );
            }
            else
            {
                this->movEngine->FaceTo( position );
            }
        }
        else
        {
            valid = false;
        }
    }
    
    if( valid )
    {
        this->applyTimeOut = false;
        this->totalTime = time;
    }

    this->init = valid;
    return valid;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPInvestigate::Run( )
{
    bool done = this->IsDone();

    if( done && ! this->applyTimeOut )
    {
        // get the animator
        ncLogicAnimator * animator = entity->GetComponentSafe<ncLogicAnimator>();
        if( animator )
        {
            animator->SetIdle();
        }

        this->applyTimeOut = true;

        this->initTime = nApplication::Instance()->GetTime();

        done = this->IsDone();
    }

    return done;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPInvestigate::IsDone() const
{
    bool done = false;

    if( this->applyTimeOut )
    {
        nTime runTime = nApplication::Instance()->GetTime() - this->initTime;
        done = ( runTime >= this->totalTime );
    }
    else
    {
        done = (this->move) ? this->movEngine->IsAtEnd() : !this->movEngine->IsFacing();
    }

    return done;
}

//------------------------------------------------------------------------------
/**
*/
bool
nGPInvestigate::FindInvestPoint( vector3& position )
{
    bool found = false;
    ncAgentMemory* memory = this->entity->GetComponentSafe<ncAgentMemory>();
    for ( ncAgentMemory::EventsIterator it( memory->GetEventsIterator() ); !it.IsEnd(); it.Next() )
    {
        nGameEvent* event = nTriggerServer::Instance()->GetEvent( it.Get() );
        Reminder& reminder = it.GetReminder();
        if( event && !reminder.HasProperties( Reminder::INVESTIGATED ) && !event->GetSourceEntity() )
        {
            nEntityObject* emitterEntity = nEntityObjectServer::Instance()->GetEntityObject( event->GetEmitterEntity() );
            position = emitterEntity->GetComponentSafe<ncTransform>()->GetPosition();                
            found = true;
            reminder.SetProperties( Reminder::INVESTIGATED );
            break;
        }
    }
    return found;
}

//------------------------------------------------------------------------------
//   EOF
//------------------------------------------------------------------------------
