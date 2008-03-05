#include "precompiled/pchrnsgameplay.h"
//------------------------------------------------------------------------------
//  ncgpsight_main.cc
//------------------------------------------------------------------------------

#include "entity/nentityobjectserver.h"
#include "ncgpperception/ncgpsight.h"
#include "ncgameplayliving/ncgameplaylivingclass.h"
#include "zombieentity/nctransform.h"
#include "mathlib/sphere.h"
#include "mathlib/cone.h"
#include "nspatial/ncspatial.h"
#include "ntrigger/ngameevent.h"
#include "ncfsm/ncfsm.h"
#include "ncagentmemory/ncagentmemory.h"
#include "nphysics/nphysicsserver.h"
#include "nphysics/nphysicsgeom.h"
#include "util/nrandomlogic.h"
#include "animcomp/nccharacter.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncGPSight,nComponentObject);

//------------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncGPSight)
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    Constructor
*/
ncGPSight::ncGPSight():
    sightRadius(-1),
    sightCone(0),
    trComp(0),
    trCompId(nEntityObjectServer::IDINVALID)
#ifndef NGAME
    , sightRay( vector3(0,0,0), vector3(0,0,0) )
#endif
{
    // Empty
}

//------------------------------------------------------------------------------
/**
    Destructor
*/
ncGPSight::~ncGPSight()
{
    n_delete(this->sightCone);

    if (this->trCompId != nEntityObjectServer::IDINVALID)
    {
        nEntityObjectServer::Instance()->RemoveEntityObjectById(this->trCompId);
        this->trComp = 0;
        this->trCompId = nEntityObjectServer::IDINVALID;
    }
}

//------------------------------------------------------------------------------
/**
Return the transform of the entity neck, so cone follows character real sight
*/
ncTransform*
ncGPSight::GetNeckTransform( nEntityObject* entity )
{
    // hooking
    nEntityObject* hookedEntity = nEntityObjectServer::Instance()->NewLocalEntityObject( "nefakeentity" );

    n_assert2( hookedEntity, "Failed to create a hooked entity." );

    ncCharacter* character( entity->GetComponent<ncCharacter>() );

    n_assert2( character, "In orther to hook the entity must have the component ncCharacter." );

    character->Attach( "tgr_camera", hookedEntity->GetId() );

    return hookedEntity->GetComponent<ncTransform>();
}

//------------------------------------------------------------------------------
/**
    Init instance
*/
void
ncGPSight::InitInstance(nObject::InitInstanceMsg initType)
{
    nEntityObject* entity = this->GetEntityObject();
    n_assert(entity);

    if (initType != nPersistServer::ReloadedInstance)
    {
        // get the entity's transform component
        n_assert(!this->trComp);
        ncTransform *trComp = this->GetNeckTransform( entity );
        n_assert(trComp);
        this->trComp = trComp;
        this->trCompId = trComp->GetEntityObject()->GetId();
    }

    // update the information that comes from the entity class
    this->UpdateClassInfo();
}

//------------------------------------------------------------------------------
/**
    Update the information comming from the entity class.
*/
void
ncGPSight::UpdateClassInfo()
{
    nEntityObject* entity = this->GetEntityObject();
    n_assert(entity);

    // get the sight radius from the entity's gameplay living class
    ncGameplayLivingClass* livingClass = entity->GetClassComponent<ncGameplayLivingClass>();
    n_assert(livingClass);
    this->sightRadius = livingClass->GetSightRadius();

    // update cone's information
    n_assert(this->trComp);
    const matrix44 &m = this->trComp->GetTransform();
    float coneRadius = this->sightRadius * sin(n_deg2rad(livingClass->GetFOVAngle()/2.0f));
    float coneHeight = this->sightRadius * cos(n_deg2rad(livingClass->GetFOVAngle()/2.0f));
    
    if (this->sightCone)
    {
        this->sightCone->set(coneHeight, coneRadius, m);
    }
    else
    {
        this->sightCone = n_new(cone(coneHeight, coneRadius, m));
    }
}

//------------------------------------------------------------------------------
/**
    Get the transform applied to the sight field to set it in the world
*/
void
ncGPSight::GetSightTransform( matrix44& m ) const
{
    // Set same orientation as the character
    m = this->trComp->GetTransform();

    // Locate the origin of the sight field at 90% of the height of the character,
    // aproximating the usual eyes location
    const bbox3& bbox = const_cast<ncSpatial*>( this->GetComponentSafe<ncSpatial>() )->GetBBox();
    vector3 eyes = bbox.center();
    eyes.y += bbox.extents().y * 0.8f;
    m.set_translation( eyes );
}

//------------------------------------------------------------------------------
/**
    Do a full perception process on the given event

    The perception process involves:

      -# Reject the event if not within the perception field
      -# Update the perception memory
      -# Perhaps send an event (the same or another one) to the FSM component
      -# Return true if an event has been send to the FSM, false otherwise
*/
bool
ncGPSight::SeeEvent( nGameEvent* event )
{
    // Finish if the event's source is myself
    if ( event->GetSourceEntity() == this->GetEntityObject()->GetId() )
    {
        return false;
    }

    // Build the sight field (intersection of a sphere and an infinite cone)
    matrix44 m;
    this->GetSightTransform( m );
    const vector3 eyesPos( m.pos_component() );
    sphere sightSphere( eyesPos, this->sightRadius );
    n_assert( this->sightCone );
    this->sightCone->setTransform( m );

    // Finish if the event doesn't fall within the sight field.
    // Use a random point within the bounding of the event's emitter to do the intersection check.
    // The random point is usefull in case both entities don't move but are partially visible,
    // so there's a chance to see the target soon or later. By using a random point the time needed
    // to see the target is proportional (in probability terms) to the target's occluded fragment.
    nEntityObject* emitter = nEntityObjectServer::Instance()->GetEntityObject( event->GetEmitterEntity() );
    n_assert2( emitter, "The entity that emits the event doesn't exist anymore" );
#ifndef NGAME
    if ( !emitter )
    {
        return false;
    }
#endif
    const bbox3& emitterBBox = const_cast<ncSpatial*>( emitter->GetComponentSafe<ncSpatial>() )->GetBBox();
    vector3 emitterPoint = n_rand_point_in_aabb( emitterBBox.vmin, emitterBBox.vmax );
    if ( !sightSphere.contains( emitterPoint ) )
    {
        return false;
    }
    if ( !this->sightCone->containsInAngle( emitterPoint ) )
    {
        return false;
    }

#ifndef NGAME
    // Store the sight ray for debug purposes
    this->sightRay.set( eyesPos, emitterPoint );
#endif

    // Check if there's a clear line of sight between the agent's eyes and the event's emitter
    vector3 direction( emitterPoint - eyesPos );
    float distance( direction.len() );
    if ( distance < TINY )
    {
        return false;
    }
    direction.norm();
    nPhyCollide::nContact contact;
    if ( nPhysicsServer::Collide( eyesPos, direction, distance, 1, &contact, nPhysicsGeom::Static ) > 0 )
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
        NLOG( perception, (0, "Entity %d sees event %d", this->GetEntityObject()->GetId(), event->GetId()) );

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

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Get the ray used to check for LoS
*/
const line3 &
ncGPSight::GetSightRay() const
{
    return this->sightRay;
}
#endif
