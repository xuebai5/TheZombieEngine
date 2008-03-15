#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncgphearing_main.cc
//  (C) Conjurer Services, S.A. 2006
//------------------------------------------------------------------------------

#include "entity/nentityobjectserver.h"
#include "ncgpperception/ncgphearing.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#include "zombieentity/nctransform.h"
#include "mathlib/sphere.h"
#include "ntrigger/ngameevent.h"
#include "ncfsm/ncfsm.h"
#include "ncagentmemory/ncagentmemory.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGPHearing,nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPHearing)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGPHearing::ncGPHearing():
hearingRadius(-1),
trComp(0)
{
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGPHearing::~ncGPHearing()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Init
*/
void
ncGPHearing::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    nEntityObject *entity = this->GetEntityObject();
    n_assert(entity);

    // get the entity's transform component
    ncTransform *trComp = entity->GetComponent<ncTransform>();
    n_assert(trComp);
    this->trComp = trComp;

    // update the information that comes from the class
    this->UpdateClassInfo();
}

//------------------------------------------------------------------------------
/**
    Update the information comming from the entity class.
*/
void
ncGPHearing::UpdateClassInfo()
{
    nEntityObject *entity = this->GetEntityObject();
    n_assert(entity);

    // get the hearing radius from the entity's gameplay living class
    ncGameplayLivingClass *livingClass = entity->GetClassComponent<ncGameplayLivingClass>();
    n_assert(livingClass);
    this->hearingRadius = livingClass->GetHearingRadius();
}

//------------------------------------------------------------------------------
/**
    Do a full perception process on the given event

    @see ncGPSight::SeeEvent for the steps involved in a perception process
*/
bool
ncGPHearing::HearEvent( nGameEvent* event )
{
    // Finish if the event's source is myself
    if ( event->GetSourceEntity() == this->GetEntityObject()->GetId() )
    {
        return false;
    }

    // Build the hearing field
    sphere hearingSphere( this->trComp->GetPosition(), this->hearingRadius );

    // Finish if the event doesn't fall within the feeling field
    nEntityObject* emitter = nEntityObjectServer::Instance()->GetEntityObject( event->GetEmitterEntity() );
    n_assert2( emitter, "The entity that emits the event doesn't exist anymore" );
#ifndef NGAME
    if ( !emitter )
    {
        return false;
    }
#endif
    const vector3& pos = emitter->GetComponentSafe<ncTransform>()->GetPosition();
    if ( !hearingSphere.contains( pos ) )
    {
        return false;
    }

    // Update the agent's memory
    ncAgentMemory* memory = this->GetComponent<ncAgentMemory>();
    n_assert2( memory, "The entity doesn't have the agent memory component" );
#ifndef NGAME
    if ( !memory )
    {
        return false;
    }
#endif
    if ( memory->AddEvent( event ) )
    {
        NLOG( perception, (0, "Entity %d hears event %d", this->GetEntityObject()->GetId(), event->GetId()) );

        // Notify the FSM about a new perceived event
        ncFSM* fsm = this->GetComponent<ncFSM>();
        n_assert2( fsm, "The entity doesn't have the FSM component" );
#ifndef NGAME
        if ( !fsm )
        {
            return false;
        }
#endif
        fsm->OnTransitionEvent( event->GetType() );
        return true;
    }

    // Event not given to the FSM
    return false;
}
