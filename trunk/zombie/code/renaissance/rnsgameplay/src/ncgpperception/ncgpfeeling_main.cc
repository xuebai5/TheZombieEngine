#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncgpfeeling_main.cc
//  (C) Conjurer Services, S.A. 2006
//------------------------------------------------------------------------------

#include "entity/nentityobjectserver.h"
#include "ncgpperception/ncgpfeeling.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#include "zombieentity/nctransform.h"
#include "mathlib/sphere.h"
#include "nspatial/ncspatial.h"
#include "ntrigger/ngameevent.h"
#include "ncfsm/ncfsm.h"
#include "ncagentmemory/ncagentmemory.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGPFeeling,nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPFeeling)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGPFeeling::ncGPFeeling():
feelingRadius(-1)
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGPFeeling::~ncGPFeeling()
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Init instance
*/
void
ncGPFeeling::InitInstance(nObject::InitInstanceMsg /*initType*/)
{
    // update the information that comes from the entity class
    this->UpdateClassInfo();
}

//------------------------------------------------------------------------------
/**
    Update the information comming from the entity class.
*/
void
ncGPFeeling::UpdateClassInfo()
{
    nEntityObject *entity = this->GetEntityObject();
    n_assert(entity);

    // get the feeling radius from the entity's gameplay living class
    ncGameplayLivingClass *livingClass = entity->GetClassComponent<ncGameplayLivingClass>();
    n_assert(livingClass);
    this->feelingRadius = livingClass->GetFeelingRadius();
}

//------------------------------------------------------------------------------
/**
    Get the transform applied to the feeling field to set it in the world
*/
void
ncGPFeeling::GetFeelingTransform( matrix44& m ) const
{
    // Locate the origin of the feeling field at the center of the entity
    m.ident();
    const bbox3& bbox = const_cast<ncSpatial*>( this->GetComponentSafe<ncSpatial>() )->GetBBox();
    m.set_translation( bbox.center() );
}

//------------------------------------------------------------------------------
/**
    Do a full perception process on the given event

    @see ncGPSight::SeeEvent for the steps involved in a perception process
*/
bool
ncGPFeeling::FeelEvent( nGameEvent* event )
{
    // Finish if the event's source is myself
    if ( event->GetSourceEntity() == this->GetEntityObject()->GetId() )
    {
        return false;
    }

    // Build the feeling field
    matrix44 m;
    this->GetFeelingTransform( m );
    sphere feelingSphere( m.pos_component(), this->feelingRadius );

    // Finish if the event doesn't fall within the feeling field.
    // Use a random point within the bounding of the event's emitter to do the intersection check.
    // A random point is used instead of a fixed point to avoid allowing greater entities to approach
    // closer than smaller ones (the alternative is a more expensive aabb-sphere intersection test).
    // It also makes the distance to which one can get close to an entity less predictable.
    nEntityObject* emitter = nEntityObjectServer::Instance()->GetEntityObject( event->GetEmitterEntity() );
    n_assert2( emitter, "The entity that emits the event doesn't exist anymore" );
#ifndef NGAME
    if ( !emitter )
    {
        return false;
    }
#endif
    const bbox3& emitterBBox = const_cast<ncSpatial*>( emitter->GetComponentSafe<ncSpatial>() )->GetBBox();
    if ( !feelingSphere.contains( emitterBBox.center() ) )
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
        NLOG( perception, (0, "Entity %d feels event %d", this->GetEntityObject()->GetId(), event->GetId()) );

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
