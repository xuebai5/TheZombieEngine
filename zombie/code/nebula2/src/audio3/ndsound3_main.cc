//------------------------------------------------------------------------------
//  ndsound3_main.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "precompiled/pchndsound.h"
#include "audio3/ndsound3.h"
#include "audio3/ndsoundserver3.h"
#include "audio3/ndsoundresource.h"

nNebulaClass(nDSound3, "nsound3");

//------------------------------------------------------------------------------
/**
*/
nDSound3::nDSound3() :
    refSoundServer("/sys/servers/audio"),
    dsVolume(0),
    soundIndex( DWORD(~0) ),
    baseFrequency( DWORD(~0) )
{
    memset(&(this->ds3DProps), 0, sizeof(this->ds3DProps));
    this->ds3DProps.dwSize = sizeof(this->ds3DProps);
}

//------------------------------------------------------------------------------
/**
*/
nDSound3::~nDSound3()
{
    if (this->IsLoaded())
    {
        this->Unload();
    }
}

//------------------------------------------------------------------------------
/**
    Creates a shared sound resource, and initializes it.
*/
bool
nDSound3::LoadResource()
{
    n_assert(!this->IsLoaded());
    n_assert(!this->refSoundResource.isvalid());

    // create a sound resource object
    nDSoundResource* rsrc = (nDSoundResource*) this->refSoundServer->NewSoundResource(this->GetFilename().Get());
    n_assert(rsrc);

    // if sound resource not opened yet, do it
    if (!rsrc->IsLoaded())
    {
        rsrc->SetFilename(this->GetFilename());
        rsrc->SetNumTracks(this->GetNumTracks());
        rsrc->SetAmbient(this->GetAmbient());
        rsrc->SetStreaming(this->GetStreaming());
        rsrc->SetLooping(this->GetLooping());
        rsrc->SetFreqControl(this->GetFreqControl());
        bool success = rsrc->Load();
        if (!success)
        {
            rsrc->Release();
            return false;
        }
    }
    this->refSoundResource = rsrc;

    CSound * cSound = this->GetCSoundPtr();
    n_assert( cSound );
    LPDIRECTSOUNDBUFFER buffer = cSound->GetBuffer(0);
    n_assert(buffer);
    buffer->GetFrequency( &this->baseFrequency );
    this->frequency = 1.0f;

    this->SetState(Valid);
    return true;
}

//------------------------------------------------------------------------------
/**
    Unloads the sound resource object.
*/
void
nDSound3::UnloadResource()
{
    n_assert(this->IsLoaded());
    n_assert(this->refSoundResource.isvalid());
    this->refSoundResource->Release();
    this->refSoundResource.invalidate();
    this->SetState(Unloaded);
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::Start()
{
    HRESULT hr; 
    CSound* snd = this->GetCSoundPtr();
#ifdef NGAME
    n_assert( snd);
#else
    if ( ! snd )
    {
        return;
    }
#endif
    LONG dsVolume = this->GetDSVolume();
    DS3DBUFFER* ds3DProps = this->GetDS3DProps();

    // a streaming sound?
    if (this->GetStreaming())
    {
        // reset buffer and fill with new data
        CStreamingSound* streamingSound = (CStreamingSound*) snd;
        streamingSound->Reset();
        LPDIRECTSOUNDBUFFER dsBuffer = streamingSound->GetBuffer(0);
        streamingSound->FillBufferWithSound(dsBuffer, this->GetLooping());
        streamingSound->SetResetPositon( 0L );
        this->soundIndex = 0;
    }

    // playback flags
    int flags = 0;
    if (this->GetLooping() || this->GetStreaming())
    {
        flags |= DSBPLAY_LOOPING;
    }
    if (this->GetAmbient())
    {
        // play as 2D sound
        hr = snd->Play(this->GetPriority(), flags, dsVolume, 0, 0, this->soundIndex);
        if (FAILED(hr))
        {
            n_printf("nDSoundServer3: failed to start 2D sound '%s'\n", this->GetFilename());
        }
    }
    else
    {
        // play as 3D sound
        flags |= DSBPLAY_TERMINATEBY_PRIORITY | DSBPLAY_TERMINATEBY_DISTANCE;
        hr = snd->Play3D(ds3DProps, this->GetPriority(), flags, dsVolume, 0, this->soundIndex);
        if (FAILED(hr))
        {
            n_printf("nDSoundServer3: failed to start 3D sound '%s'\n", this->GetFilename());
        }        
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::Stop()
{
    CSound* snd = this->GetCSoundPtr();
//#ifdef NGAME
//    n_assert_return(snd, );
//#endif
    if ( !snd )
    {
        return;
    }
//#endif

    if ( ! this->streaming )
    {
        if (this->soundIndex != -1)
        {
            n_assert(this->soundIndex >= 0);
            snd->Stop(this->soundIndex);
            this->soundIndex = DWORD( ~0 );
        }
    }
    else
    {
        snd->Stop();
    }
}

//------------------------------------------------------------------------------
/**
    @brief stop the sound in a given buffer index
    @todo temporary function
*/
void
nDSound3::Stop( int index )
{
    CSound* snd = this->GetCSoundPtr();
#ifdef NGAME
    n_assert( snd);
#else
    if ( ! snd )
    {
        return;
    }
#endif

    if ( index != -1 )
    {
        n_assert( index >= 0);
        snd->Stop( index );
    }
}

//------------------------------------------------------------------------------
/**
    @brief clamp a frequency (Hz) to limits of DSound
*/
float
nDSound3::ClampFrequency( float f )
{
    return max(100.0f, min( 100000.0f, f ) );
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::Update()
{
    // Test: only update last buffer played. Works ok.
    if (this->soundIndex != -1)
    {
        if (this->GetStreaming())
        {
            CStreamingSound* streamingSound = (CStreamingSound*) this->GetCSoundPtr();
            n_assert(streamingSound);
            if (streamingSound->IsSoundPlaying())
            {
                // update the volume and frequency
                LPDIRECTSOUNDBUFFER dsBuffer = streamingSound->GetBuffer(0);
                dsBuffer->SetVolume(this->GetDSVolume());
                if ( this->freqControl )
                {
                    dsBuffer->SetFrequency( (DWORD) this->ClampFrequency( this->frequency * this->baseFrequency ) );
                }
            }
        }
        else
        {
            CSound* snd = this->GetCSoundPtr();
            n_assert(snd);
            if (snd->IsSoundPlaying())
            {
                // update the volume and frequency
                LPDIRECTSOUNDBUFFER dsBuffer = snd->GetBuffer(this->soundIndex);
                dsBuffer->SetVolume( this->GetDSVolume() );
                dsBuffer->SetFrequency( (DWORD) this->ClampFrequency( this->frequency * this->baseFrequency ) );
            }
        }
    }
}


//------------------------------------------------------------------------------
/**
    @brief update volume
    @param index buffer index
    @todo temp function, same as Stop( int index )
*/
void
nDSound3::Update( int index )
{
    n_assert( index >= 0 );

    if (index < 0)
    {
        return;
    }

    if (this->GetStreaming())
    {
        CStreamingSound* streamingSound = (CStreamingSound*) this->GetCSoundPtr();
        n_assert(streamingSound);
        if (streamingSound->IsSoundPlaying())
        {
            // update the volume and frequency
            LPDIRECTSOUNDBUFFER dsBuffer = streamingSound->GetBuffer(0);
            dsBuffer->SetVolume(this->GetDSVolume());
            if ( this->freqControl )
            {
                dsBuffer->SetFrequency( (DWORD) this->ClampFrequency( this->frequency * this->baseFrequency ) );
            }
        }
    }
    else
    {
        CSound* snd = this->GetCSoundPtr();
        n_assert(snd);
        if (snd->IsSoundPlaying())
        {
            // update the volume and frequency
            LPDIRECTSOUNDBUFFER dsBuffer = snd->GetBuffer( index );
            dsBuffer->SetVolume( this->GetDSVolume() );
            if ( this->freqControl )
            {
                dsBuffer->SetFrequency( (DWORD) this->ClampFrequency( this->frequency * this->baseFrequency ) );
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nDSound3::IsPlaying()
{   
    if (this->soundIndex != -1)
    {
        if (this->GetStreaming())
        {
            CStreamingSound* snd = (CStreamingSound*) this->GetCSoundPtr();
            return (snd && 0 != snd->IsSoundPlaying());
        }
        else
        {
            CSound* snd = this->GetCSoundPtr();
            return (snd && 0 != snd->IsSoundPlaying());
        }
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nDSound3::IsPlaying(int index)
{   
    if (this->GetStreaming())
    {
        CStreamingSound* snd = (CStreamingSound*) this->GetCSoundPtr();
        return (snd && 0 != snd->IsSoundPlaying());
    }
    else
    {
        CSound* snd = this->GetCSoundPtr();
        return snd && 0 != snd->IsSoundPlaying(index);
    }
}

//------------------------------------------------------------------------------
/**
*/
int
nDSound3::GetStatus(int index)
{
    if (this->GetStreaming())
    {
        if ( this->IsPlaying() )
        {
            return SOUNDBUFFER_PLAYING;
        }
        else
        {
            return SOUNDBUFFER_NONE;
        }
    }
    else
    {
        CSound* snd = this->GetCSoundPtr();
        n_assert(snd);
        DWORD status = snd->GetStatus(index);

        int sbStatus = SOUNDBUFFER_NONE;
        if ( status & DSBSTATUS_PLAYING )
        {
            sbStatus = SOUNDBUFFER_PLAYING;
        }
        if ( status & DSBSTATUS_TERMINATED )
        {
            sbStatus |= SOUNDBUFFER_TERMINATED;
        }
        if ( status & DSBSTATUS_BUFFERLOST )
        {
            sbStatus |= SOUNDBUFFER_LOST;
        }

		return sbStatus;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nDSound3::SetTrackPosition(int index, float seconds)
{
    if ( this->GetStreaming() || this->soundIndex != -1 )
    {
        CSound* snd = this->GetCSoundPtr();
        n_assert(snd);
        snd->SetTrackPosition(index, seconds);
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get duration of sound in seconds
    @author Juan Jose Luna

    @todo JJ  Can be accelerated with a internal variable, initialized on load
*/
float
nDSound3::GetDuration()
{
    CSound* snd = this->GetCSoundPtr();
    n_assert(snd);

    return snd->GetDuration();
}

//------------------------------------------------------------------------------
/**
    @brief Get last buffered used for playing, depends on implementation
    @author Juan Jose Luna
*/
int
nDSound3::GetLastBufferUsed()
{
    return this->soundIndex;
}

//------------------------------------------------------------------------------
/**
    @brief Update all 3d properties
    @param index Underlying sound buffer index
    @author Juan Jose Luna
*/
void
nDSound3::Update3DProperties( int index )
{
    if ( ! this->GetStreaming() && ! this->GetAmbient() )
    {
        CSound* snd = this->GetCSoundPtr();   
        n_assert(snd);
        if ( snd->IsSoundPlaying() )
        {
            // update position and velocity
            LPDIRECTSOUND3DBUFFER buf3d;
            snd->Get3DBufferInterface( index , &buf3d);

            // The last parameter means that IDirectSound3DListener8::CommitDeferredSettings must be called
            // (possibly in nAudioServer3::EndScene() to update all sounds at once, this is best.
            N_IFDEF_ASSERTS(HRESULT hr =)
            buf3d->SetAllParameters( GetDS3DProps(), DS3D_DEFERRED /*DS3D_IMMEDIATE*/ );
            n_assert(!FAILED(hr));
        }
    }
}