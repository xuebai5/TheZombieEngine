#include "precompiled/pchncsound.h"
//------------------------------------------------------------------------------
//  ncsound_main.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "zombieentity/nctransform.h"
#include "audio3/nsound3.h"
#include "audio3/naudioserver3.h"
#include "ncsound/ncsound.h"
#include "ncsound/ncsoundclass.h"
#include "nsoundscheduler/nsoundscheduler.h"
#include "nphysics/ncphysicsobj.h"
#include "ntrigger/ncareatrigger.h"
#include "ntrigger/nctriggershape.h"
#include "ntrigger/ntriggerserver.h"
#include "zombieentity/ncdictionary.h"
#include "kernel/nobject.h"


// includes required for network support
#include "nnetworkmanager/nnetworkmanager.h"
#include "util/nstream.h"
#include "rnsgameplay/nrnsentitymanager.h"

//------------------------------------------------------------------------------
struct nGameEvent;

//------------------------------------------------------------------------------
nNebulaComponentObject(ncSound,nComponentObject);

//-----------------------------------------------------------------------------
NSCRIPT_INITCMDS_BEGIN(ncSound)
    NSCRIPT_ADDCMD_COMPOBJECT('JPLA', void, Play, 2, (const nString&, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JPLM', void, PlayMaterial, 3, (const nString&, const nString&, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JAPL', void, PlayAlone, 2, (const nString&, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JAPM', void, PlayAloneMaterial, 3, (const nString&, const nString&, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSTP', void, StopSound, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSVO', void, SetSoundVolume, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGVO', float, GetSoundVolume , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSFR', void, SetSoundFrequency, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGFR', float, GetSoundFrequency , 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JSDV', void, SetSoundDefaultVolume, 1, (float), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JGDV', float, GetSoundDefaultVolume , 0, (), 0, ());
    /*NSCRIPT_ADDCMD_COMPOBJECT('EENA', void, EnableSoundTrigger, 1, (nGameEvent*), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EDIS', void, DisableSoundTrigger, 1, (nGameEvent*), 0, ());*/
    NSCRIPT_ADDCMD_COMPOBJECT('ESTS', void, SynchronizeTriggerShape, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESSP', void, SendEventToPlay, 2, (const char*, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('EPSE', void, PlaySoundEvent, 2, (const char*, int), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESSS', void, SendStopSound, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('ESLS', void, StopLastSound, 0, (), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JAPP', void, PlayAloneAtPosition, 3, (const nString&, int, const vector3 &), 0, ());
    NSCRIPT_ADDCMD_COMPOBJECT('JAMP', void, PlayAloneMaterialAtPosition, 4, (const nString&, const nString&, int, const vector3 &), 0, ());

NSCRIPT_INITCMDS_END()


//------------------------------------------------------------------------------
/**
    constructor
*/
ncSound::ncSound()
{
    this->soundInfo.soundComponent = this;
    this->defaultVolume = 1.0f;
}

//------------------------------------------------------------------------------
/**
    destructor
*/
ncSound::~ncSound()
{
    if ( this->soundInfo.GetState() > nSoundScheduler::Idle )
    {
        nSoundScheduler::Instance()->AbortSoundPlayback( &this->soundInfo );
    }
}

//------------------------------------------------------------------------------
/**
    instance initialization
*/
void
ncSound::InitInstance(nObject::InitInstanceMsg initType)
{
    if ( this->GetComponent<ncAreaTrigger>() && this->GetComponent<ncTriggerShape>() )
    {
        if( initType != nObject::ReloadedInstance )
        {
            // Listen to signals coming from the area trigger component
            this->GetEntityObject()->BindSignal( ncAreaTrigger::SignalOnEnterArea, this, &ncSound::EnableSoundTrigger, 0 );
            this->GetEntityObject()->BindSignal( ncAreaTrigger::SignalOnExitArea, this, &ncSound::DisableSoundTrigger, 0 );
        }

#ifndef NGAME
        // Activate by default the 'player' output in the sound source
        this->GetComponent<ncTrigger>()->SetEventFlag( nTriggerServer::Instance()->GetEventTransientId("player"), true );
#endif
    }
#ifndef NGAME
    if( initType != nObject::ReloadedInstance )
    {
        // Register to EnterLimbo and ExitLimbo signals
        this->GetEntityObject()->BindSignal( nEntityObject::SignalEnterLimbo, this, &ncSound::DoEnterLimbo, 0 );
        this->GetEntityObject()->BindSignal( nEntityObject::SignalExitLimbo, this, &ncSound::DoExitLimbo, 0 );
    }
#endif

}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @brief Enter limbo 
*/
void
ncSound::DoEnterLimbo()
{
    if ( this->GetComponent<ncAreaTrigger>() && this->GetComponent<ncTriggerShape>() )
    {
        // stop sound if playing
        this->StopSound();

        // Unbind from signals coming from the area trigger component
        this->GetEntityObject()->UnbindTargetObject( ncAreaTrigger::SignalOnExitArea.GetId(), this->entityObject );
        this->GetEntityObject()->UnbindTargetObject( ncAreaTrigger::SignalOnEnterArea.GetId(), this->entityObject );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Exit limbo 
*/
void
ncSound::DoExitLimbo()
{
    if ( this->GetComponent<ncAreaTrigger>() && this->GetComponent<ncTriggerShape>() )
    {
        // Listen to signals coming from the area trigger component
        this->GetEntityObject()->BindSignal( ncAreaTrigger::SignalOnEnterArea, this, &ncSound::EnableSoundTrigger, 0 );
        this->GetEntityObject()->BindSignal( ncAreaTrigger::SignalOnExitArea, this, &ncSound::DisableSoundTrigger, 0 );
    }
}
#endif

//------------------------------------------------------------------------------
/**
*/
bool
ncSound::SaveCmds(nPersistServer* ps)
{
    ps->Put( this->GetEntityObject(), 'JSDV', this->GetSoundDefaultVolume() );

    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Setup just before a play
    @return The sound index, or InvalidSoundIndex if some error ocurred (the play will fail silently)
*/
int
ncSound::PrePlay( const char* event, const char* material, bool standAlone )
{
    ncSoundClass* soundClass = this->GetClassComponent<ncSoundClass>();
    n_assert(soundClass);
    nSoundEventTable* soundEvents = soundClass->GetSoundTable();
    if ( !soundEvents )
    {
        return InvalidSoundIndex;
    }

    // Get sound index
    int soundIndex = InvalidSoundIndex;
    if ( material == 0 || *material == 0 )
    {
        soundIndex = soundEvents->GetSoundIndex( event );
    }
    else
    {
        soundIndex = soundEvents->GetSoundIndex( event, material );
    }

    // Sound already playing (this ncSound only supports one playing instance of a sound), abort playing to free the entity sound
    // @todo JJ When SoundComponentInfo is detached from ncSound, this will not be needed, unless the sound is infinite looping. In that
    // case, the previous sound must be stopped.
    if ( ! standAlone && soundIndex != InvalidSoundIndex && this->soundInfo.GetState() > nSoundScheduler::Idle )
    {
        nSoundScheduler::Instance()->AbortSoundPlayback( &this->soundInfo );
    }

    return soundIndex;
}

//------------------------------------------------------------------------------
/**
    @brief Play a sound
    @param event The sound event to play
    @param repeat Number of playback repetitions (0 = infinite looping)
    @todo The play methods could be passed a class that encapsulates event and material string, and the sound index already calculated, to be used
    externally
*/
void
ncSound::Play( const nString& event, int repeat )
{
    n_assert( repeat >= 0 );

    int soundIndex = this->PrePlay( event.Get() );

    if ( soundIndex != InvalidSoundIndex )
    {
        nSoundScheduler::Instance()->PlayEntity(this->GetEntityObject(), soundIndex, repeat);
    }
}

//------------------------------------------------------------------------------
/**
    @brief Play a sound with a material
    @param event The sound event to play
    @param repeat Number of playback repetitions (0 = infinite looping)
    @param material The material id to modify the played sound
*/
void
ncSound::PlayMaterial( const nString& event, const nString& material, int repeat )
{
    n_assert( repeat >= 0 );

    int soundIndex = this->PrePlay( event.Get(), material.Get() );

    if ( soundIndex != InvalidSoundIndex )
	{
		nSoundScheduler::Instance()->PlayEntity(this->GetEntityObject(), soundIndex, repeat);
	}
}

//------------------------------------------------------------------------------
/**
    @brief Play a sound with all possible parameters
    @param event The sound event to play
    @param event Material involved in the sound
    @param duration Duration of the sound
    @param repeat Number of playback repetitions (0 = infinite looping)

    See method nSoundScheduler::PlayQuery
*/
void
ncSound::PlayExtended( const nString& event, const nString& material, int repeat, nTime duration, bool calcVar )
{
    n_assert( duration >= nTime(0.0f) );
    n_assert( repeat >= 0 );

    int soundIndex = this->PrePlay( event.Get(), material.Get() );

    if ( soundIndex != InvalidSoundIndex )
    {
        nSoundScheduler::Instance()->PlayEntity( this->GetEntityObject(), soundIndex, repeat, duration, calcVar );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Play a sound without attaching it to the entity
    @param event The sound event to play
    @param repeat Number of playback repetitions (0 = infinite looping)
*/
void
ncSound::PlayAlone( const nString& event, int repeat )
{
    n_assert( repeat >= 0 );

    int soundIndex = this->PrePlay( event.Get(), false );

    if ( soundIndex != InvalidSoundIndex )
    {
        nSoundScheduler::Instance()->Play(this->GetComponentSafe<ncTransform>()->GetPosition(), soundIndex, repeat);
    }
}

//------------------------------------------------------------------------------
/**
    @brief Play a sound with a material without attaching it to the entity
    @param event The sound event to play
    @param repeat Number of playback repetitions (0 = infinite looping)
    @param material The material id to modify the played sound
*/
void
ncSound::PlayAloneMaterial( const nString& event, const nString& material, int repeat )
{
    n_assert( repeat >= 0 );

    int soundIndex = this->PrePlay( event.Get(), material.Get(), false );

    if ( soundIndex != InvalidSoundIndex )
	{
		nSoundScheduler::Instance()->Play(this->GetComponentSafe<ncTransform>()->GetPosition(), soundIndex, repeat);
	}
}

//------------------------------------------------------------------------------
/**
    @brief Play a sound without attaching it to the entity, at given position
    @param event The sound event to play
    @param repeat Number of playback repetitions (0 = infinite looping)
    @param position The position to play the sound at
*/
void
ncSound::PlayAloneAtPosition( const nString& event, int repeat, const vector3 & position )
{
    n_assert( repeat >= 0 );

    int soundIndex = this->PrePlay( event.Get(), false );

    if ( soundIndex != InvalidSoundIndex )
    {
        nSoundScheduler::Instance()->Play(position, soundIndex, repeat);
    }
}

//------------------------------------------------------------------------------
/**
    @brief Play a sound with a material without attaching it to the entity, at given position
    @param event The sound event to play
    @param repeat Number of playback repetitions (0 = infinite looping)
    @param material The material id to modify the played sound
    @param position The position to play the sound at
*/
void
ncSound::PlayAloneMaterialAtPosition( const nString& event, const nString& material, int repeat, const vector3 & position )
{
    n_assert( repeat >= 0 );

    int soundIndex = this->PrePlay( event.Get(), material.Get(), false );

    if ( soundIndex != InvalidSoundIndex )
	{
		nSoundScheduler::Instance()->Play(position, soundIndex, repeat);
	}
}

//------------------------------------------------------------------------------
/**
    @brief Set playing volume
    @param volume volume

    Volume range is 0..1
*/
void
ncSound::SetSoundVolume( float volume )
{
    if ( this->soundInfo.GetState() != nSoundScheduler::Idle )
    {
        this->soundInfo.playVol = volume;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get playing volume
    @return volume

    Volume range is 0..1
*/
float
ncSound::GetSoundVolume() const
{
    if ( this->soundInfo.GetState() != nSoundScheduler::Idle )
    {
        return this->soundInfo.playVol;
    }
    return 0.0f;
}

//------------------------------------------------------------------------------
/**
    @brief Get playing frequency
    @return volume

    Volume range is 0..1
*/
float
ncSound::GetSoundFrequency() const
{
    if ( this->soundInfo.GetState() != nSoundScheduler::Idle )
    {
        n_assert(this->soundInfo.sound);
        return this->soundInfo.sound->GetFrequency();
    }
    return 0.0f;
}

//------------------------------------------------------------------------------
/**
    @brief Set playing frequency
    @param volume volume

    Volume range is 0..1
    Only infinite looping sounds can vary frequency
*/
void
ncSound::SetSoundFrequency( float frequency )
{
    if ( this->soundInfo.GetState() != nSoundScheduler::Idle && this->soundInfo.endTime == 0.0f )
    {
        n_assert( this->soundInfo.sound );
        this->soundInfo.flags |= nSoundScheduler::DynamicVolFreq;
        this->soundInfo.sound->SetFrequency( frequency );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set volume setting (default modulating volume for all sounds)
    @param volume volume

    Volume range is 0..1
*/
void
ncSound::SetSoundDefaultVolume( float volume )
{
    this->defaultVolume = volume;
}

//------------------------------------------------------------------------------
/**
    @brief Get volume setting (default modulating volume for all sounds)
    @return volume

    Volume range is 0..1
*/
float
ncSound::GetSoundDefaultVolume() const
{
    return this->defaultVolume;
}
//------------------------------------------------------------------------------
/**
    @brief Stop sound
*/
void
ncSound::StopSound()
{
    this->soundInfo.Stop();
}

//------------------------------------------------------------------------------
/**
    @brief Set current nsound3, sound times (start, end) and play volume.
    @param scInfo Sound parameters

    Note that only sound, startTime, endTime and playVol of the struct are set by this funtion.
*/
void
ncSound::SetSoundParams( const nSoundScheduler::SoundComponentInfo& scInfo )
{
    n_assert( scInfo.sound );
    this->soundInfo.sound = scInfo.sound;
    this->soundInfo.startTime = scInfo.startTime;
    this->soundInfo.endTime = scInfo.endTime;
    this->soundInfo.playVol = scInfo.playVol;
    this->soundInfo.flags = scInfo.flags;
#ifndef NGAME
    this->soundInfo.soundIndex = scInfo.soundIndex;
#endif
}

//------------------------------------------------------------------------------
/**
    @brief Get a reference to the runtime (playing) sound parameters of the ncSound
    @return Reference to the SoundComponentInfo structure
*/
nSoundScheduler::SoundComponentInfo*
ncSound::GetSoundParams()
{
    return &this->soundInfo;
}

//------------------------------------------------------------------------------
/**
    @brief Get a reference to the runtime (playing) sound parameters of the ncSound, read only version
    @return Reference to the SoundComponentInfo structure
*/
const nSoundScheduler::SoundComponentInfo*
ncSound::GetSoundParams() const
{
    return &this->soundInfo;
}

//------------------------------------------------------------------------------
/**
    Set buffer index currently used
*/
void
ncSound::SetBufferIndex( int index )
{
    this->soundInfo.bufferIndex = index;
}

//------------------------------------------------------------------------------
/**
    @brief Refresh position from entity object.

    It gets position from ncTransform and also velocity from physic component if present
*/
void
ncSound::GetPositionFromEntity()
{
    nEntityObject *entObj = this->GetEntityObject();
    vector3 pos;
    pos = entObj->GetComponentSafe<ncTransform>()->GetPosition();

    if ( this->GetSoundParams() && this->soundInfo.sound && this->soundInfo.hasOffset )
    {
        vector3 v = this->GetComponentSafe<ncTransform>()->GetTransform().transform_norm( this->GetSoundParams()->positionOffset );
        pos += v;
    }
    ncPhysicsObj* phyComp = static_cast<ncPhysicsObj*>( entObj->GetComponent<ncPhysicsObj>() );
    if ( phyComp && phyComp->GetBody() )
    {
        vector3 vel;
        phyComp->GetLinearVelocity( vel );
        this->soundInfo.SetSoundPositionAndVelocity( pos, vel );
    }
    else
    {
        // rotation and scale changes should not make this call...
        this->soundInfo.SetSoundPosition( pos );
    }
}

//------------------------------------------------------------------------------
/**
    Set a fixed offset for sound position from entity position
*/
void
ncSound::SetSoundPositionOffset(vector3 offset)
{
    if ( this->GetSoundParams() )
    {
        this->GetSoundParams()->positionOffset = offset;
        this->GetSoundParams()->hasOffset = true;
    }
}

//------------------------------------------------------------------------------
/**
    Enable sound trigger
*/
void
ncSound::EnableSoundTrigger( nGameEvent* /*event*/ )
{
    // Increment enable vs disable counter
    int timesEnabled;
    ncDictionary* dictionary = this->GetComponentSafe<ncDictionary>();
    timesEnabled = dictionary->GetLocalIntVariable( "play_sound_counter" ) + 1;
    n_assert( timesEnabled >= 1 );
    dictionary->SetLocalIntVariable( "play_sound_counter", timesEnabled );

    // If the counter is 1, then it means that the sound was off, so turn it on
    if ( timesEnabled == 1 )
    {
        this->SendEventToPlay("trigger", 0);
    }
}

//------------------------------------------------------------------------------
/**
    Disable sound trigger
*/
void
ncSound::DisableSoundTrigger( nGameEvent* /*event*/ )
{
    // Decrement enable vs disable counter
    int timesEnabled;
    ncDictionary* dictionary = this->GetComponentSafe<ncDictionary>();
    timesEnabled = max(0, dictionary->GetLocalIntVariable( "play_sound_counter" ) - 1);
    n_assert( timesEnabled >= 0 );
    dictionary->SetLocalIntVariable( "play_sound_counter", timesEnabled );

    // If the counter is 0, then it means that all who has asked to enable the sound
    // has also asked to disable it afterwards, so turn it off
    if ( timesEnabled == 0 )
    {
        this->SendStopSound();
    }
}

//------------------------------------------------------------------------------
/**
    Synchronize the trigger shape radius with the greatest maximum distance among all the table's sounds
*/
void
ncSound::SynchronizeTriggerShape()
{
    ncTriggerShape* shape = this->GetComponent<ncTriggerShape>();
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
    @brief Get the flags
    @return The flags
*/
int
ncSound::GetFlags() const
{
    return this->soundInfo.flags;
}

//------------------------------------------------------------------------------
/**
    @brief Set the flags
    @param The flags to be set
*/
void
ncSound::SetFlags(int flags)
{
    this->soundInfo.flags = flags;
}

//-----------------------------------------------------------------------------
/**
    Play a sound on all the clients
*/
void
ncSound::SendEventToPlay( const char* event, int repeat )
{
    if ( nNetworkManager::Instance() && nNetworkManager::Instance()->IsServer() )
    {
        n_assert( event );
        int entityId( this->GetEntityObject()->GetId() );

        nstream data;
        data.SetWrite( true );
        data.UpdateInt( entityId );
        data.UpdateString( event ); // @todo Send the sound index instead of the event name
        data.UpdateInt( repeat );

        nNetworkManager::Instance()->CallRPCAllServer(
            nRnsEntityManager::PLAY_SOUND_EVENT,
            data.GetBufferSize(), data.GetBuffer() );
    }
#ifndef NGAME
    else
    {
        // In edition, allow to play sounds without entering game mode
        this->Play( event, repeat );
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Play the sound specified by the server
*/
void
ncSound::ReceiveEventToPlay( nstream* data )
{
    n_assert( data );

    if ( data )
    {
        char event[100]; // @todo Remove this ugly thingy when sending the sound index instead of the event name
        int repeat;
        data->UpdateString( event );
        data->UpdateInt( repeat );
        this->Play( event, repeat );
    }
}

//-----------------------------------------------------------------------------
/**
    Play a sound on all the clients (nicer name to be used by designers)
*/
void
ncSound::PlaySoundEvent( const char* event, int repeat )
{
    this->SendEventToPlay( event, repeat );
}

//-----------------------------------------------------------------------------
/**
    Stop playing sound on all the clients
*/
void
ncSound::SendStopSound()
{
    if ( nNetworkManager::Instance() && nNetworkManager::Instance()->IsServer() )
    {
        int entityId( this->GetEntityObject()->GetId() );

        nstream data;
        data.SetWrite( true );
        data.UpdateInt( entityId );

        nNetworkManager::Instance()->CallRPCAllServer(
            nRnsEntityManager::STOP_SOUND,
            data.GetBufferSize(), data.GetBuffer() );
    }
#ifndef NGAME
    else
    {
        // In edition, allow to stop sounds without entering game mode
        this->StopSound();
    }
#endif
}

//-----------------------------------------------------------------------------
/**
    Stop playing sound on all the clients (nicer name to be used by designers)
*/
void
ncSound::StopLastSound()
{
    this->SendStopSound();
}

//-----------------------------------------------------------------------------
/**
	Is playing?
*/
bool
ncSound::IsPlaying() const
{
	if ( this->GetSoundParams() && this->GetSoundParams()->IsPlaying() )
	{
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
    @param event name of the event
    @returns index of sound
*/
int
ncSound::GetSoundIndex( const char * event )
{
    int soundIndex = InvalidSoundIndex;

    ncSoundClass* soundClass = this->GetClassComponentSafe<ncSoundClass>();
    if( soundClass )
    {
        nSoundEventTable* soundEvents = soundClass->GetSoundTable();
        if( soundEvents )
        {
            soundIndex = soundEvents->GetSoundIndex( event );
        }
    }

    return soundIndex;
}

//------------------------------------------------------------------------------
