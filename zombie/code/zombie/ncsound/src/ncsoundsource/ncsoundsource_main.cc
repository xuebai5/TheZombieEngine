#include "precompiled/pchncsound.h"
//------------------------------------------------------------------------------
//  ncSoundSource_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncsoundsource/ncsoundsource.h"
#include "zombieentity/nctransform.h"
#include "ncsound/ncsound.h"
#include "ncsound/ncsoundclass.h"
#include "nsoundscheduler/nsoundscheduler.h"
#include "nphysics/ncphysicsobj.h"
#include "ntrigger/ncareatrigger.h"
#include "ntrigger/nctriggershape.h"
#include "ntrigger/ntriggerserver.h"

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSoundSource,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSoundSource)
    NSCRIPT_ADDCMD_COMPOBJECT('JSTI', void, SetTriggerId, 1, (nEntityObjectId), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGTI', nEntityObjectId, GetTriggerId, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSFD', void, SetFadeDistance, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGFD', float, GetFadeDistance, 0, (), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    constructor
*/
ncSoundSource::ncSoundSource():
    fadeDistance( 0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncSoundSource::~ncSoundSource()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    instance initialization
*/
void
ncSoundSource::InitInstance(nObject::InitInstanceMsg initType)
{
    if (initType != nObject::ReloadedInstance)
    {
        nEntityObject * trigger = nEntityObjectServer::Instance()->GetEntityObject( this->triggerEntityId );
        this->SetTrigger( trigger );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set trigger that controls the sound source
*/
void
ncSoundSource::SetTrigger(nEntityObject *trigger)
{
    if ( trigger )
    {
        ncAreaTrigger* areaTrigger( trigger->GetComponent<ncAreaTrigger>() );
        ncTriggerShape* triggerShape( trigger->GetComponent<ncTriggerShape>() );
        if ( areaTrigger && triggerShape )
        {
            // Listen to signals coming from the area trigger component
            trigger->BindSignal( ncAreaTrigger::SignalOnEnterArea, this, &ncSoundSource::EnterArea, 0 );
            trigger->BindSignal( ncAreaTrigger::SignalOnExitArea, this, &ncSoundSource::ExitArea, 0 );

            // Synchronize the trigger shape radius with the greatest maximum distance among all the table's sounds
            this->SynchronizeTriggerShape( trigger );

            // Activate by default the 'player' output in the sound source
#ifndef NGAME
            trigger->GetComponentSafe<ncTrigger>()->SetEventFlag( nTriggerServer::Instance()->GetEventTransientId("player"), true );
#endif

            this->triggerEntityId = trigger->GetId();
        }
    }

    // @todo unbind if triggerId not valid and currently binded
}

//------------------------------------------------------------------------------
/**
    @brief Synchronize the trigger shape radius with the greatest maximum distance among all the table's sounds
*/
void
ncSoundSource::SynchronizeTriggerShape( nEntityObject * trigger )
{
    n_assert( this->GetComponent<ncSound>() );

    ncTriggerShape* shape = trigger->GetComponent<ncTriggerShape>();
    if ( shape )
    {
        ncSoundClass* clazz = this->GetClassComponent<ncSoundClass>();
        n_assert( clazz );
        nSoundEventTable* table = clazz->GetSoundTable();
        float radius = 1.0f;
        if ( table )
        {
            radius = table->GetGreatestMaxDistance();
        }
        shape->SetCircle( radius );

        // Constraint the trigger culling radius to the radius circle plus a border
        // to allow to detect events exiting the area
        ncTrigger* trigger = this->GetComponent<ncTrigger>();
        if ( trigger )
        {
            const float SecurityMargin( 5 );
            trigger->SetCullingRadius( radius + SecurityMargin );
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
ncSoundSource::SaveCmds(nPersistServer* ps)
{
    if ( this->GetFadeDistance() != 0.0f )
    {
        ps->Put( this->entityObject, 'JSFD', this->GetFadeDistance() );
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Set trigger that controls the sound source
    @param triggerId trigger entity id
*/
void
ncSoundSource::SetTriggerId(nEntityObjectId triggerId)
{
    this->triggerEntityId = triggerId;
}

//------------------------------------------------------------------------------
/**
    @brief Set trigger that controls the sound source
    @param triggerId trigger entity id
*/
nEntityObjectId
ncSoundSource::GetTriggerId()
{
    return this->triggerEntityId;
}

//------------------------------------------------------------------------------
/**
    @brief Set fade border distance of sound
*/
void
ncSoundSource::SetFadeDistance(float distance)
{
    this->fadeDistance = distance;
}

//------------------------------------------------------------------------------
/**
    @brief Get fade border distance of sound
*/
float
ncSoundSource::GetFadeDistance()
{
    return this->fadeDistance;
}

//------------------------------------------------------------------------------
/**
    Enable sound trigger
*/
void
ncSoundSource::EnterArea( nGameEvent* /*event*/ )
{
    n_assert( this->GetComponent<ncSound>() );
    this->GetComponentSafe<ncSound>()->Play("trigger", 0);
}

//------------------------------------------------------------------------------
/**
    Disable sound trigger
*/
void
ncSoundSource::ExitArea( nGameEvent* /*event*/ )
{
    n_assert( this->GetComponent<ncSound>() );
   this->GetComponentSafe<ncSound>()->StopSound();
}
