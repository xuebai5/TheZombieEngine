#ifndef N_DSOUND3_H
#define N_DSOUND3_H
//------------------------------------------------------------------------------
/**
    @class nDSound3
    @ingroup Audio3
    @brief DirectSound based implementation of nSound3. Works together with
    nDSoundServer3 and nDSoundResource.

    (C) 2003 RadonLabs GmbH
*/
#include "audio3/nsound3.h"
#include "audio3/ndsoundresource.h"

class nDSoundServer3;
class CSound;

//------------------------------------------------------------------------------
class nDSound3 : public nSound3
{
public:
    /// constructor
    nDSound3();
    /// destructor
    virtual ~nDSound3();
    /// start the sound
    virtual void Start();
    /// stop the sound
    virtual void Stop();
    /// stop the sound in a given buffer index
    virtual void Stop( int index );
    /// update the sound
    virtual void Update();
    /// update the sound for a given buffer index
    virtual void Update( int index );
    /// update all 3d properties for a given buffer index
    virtual void Update3DProperties( int index );
    /// return duration of sound in seconds
    virtual float GetDuration();
    ///return last buffered used for play
    virtual int GetLastBufferUsed();
    /// return true if any buffer of sound currently playing
    virtual bool IsPlaying();
    /// return true if sound buffer is currently playing
    virtual bool IsPlaying(int index);
    /// get sound buffer status
    virtual int GetStatus(int index);
    /// set sound playing cursor in seconds
    virtual void SetTrackPosition( int index, float seconds );
    /// get pointer to embedded nDSoundResource object
    nDSoundResource* GetDSoundResource();
    /// get pointer to CSound object owned by sound resource
    CSound* GetCSoundPtr();
    /// get the volume converted to DirectSound conventions
    LONG GetDSVolume();
    /// get DirectSound 3D-Soundbuffer properties
    DS3DBUFFER* GetDS3DProps();
    /// convert a linear volume into a DirectSound volume value
    static LONG AsDirectSoundVolume(float vol);
    /// clamp a frequency (Hz) to limits of DSound
    float ClampFrequency( float f );

protected:
    /// load the resource 
    virtual bool LoadResource();
    /// unload the resource
    virtual void UnloadResource();

private:
    nAutoRef<nDSoundServer3> refSoundServer;
    nRef<nDSoundResource> refSoundResource;
    LONG dsVolume;
    DWORD baseFrequency;
    DWORD soundIndex;
    DS3DBUFFER ds3DProps;
};

//------------------------------------------------------------------------------
/**
    Convert a linear volume between 0.0f and 1.0f into a Dezibel-based
    DirectSound-Volume
*/
inline
LONG
nDSound3::AsDirectSoundVolume(float vol)
{
    float scaledVolume = 0.0f;
    if (vol > 0.0f)
    {
        const float minVol = 0.4f;
        const float maxVol = 1.0f;
        scaledVolume = minVol + (vol * (maxVol - minVol));
    }
    float dsVol = DSBVOLUME_MIN + ((DSBVOLUME_MAX - DSBVOLUME_MIN) * scaledVolume);
    return (LONG) dsVol;
}

//------------------------------------------------------------------------------
/**
    Get volume as DirectSound volume value.
*/
inline
LONG
nDSound3::GetDSVolume()
{
    return AsDirectSoundVolume(this->volume * nAudioServer3::Instance()->GetMasterVolume(this->category));
}

//------------------------------------------------------------------------------
/**
    Get pointer to DirectSound 3D-SoundBuffer properties.
*/
inline
DS3DBUFFER*
nDSound3::GetDS3DProps()
{
    if (this->props3DDirty)
    {
        this->props3DDirty = false;
        this->ds3DProps.vPosition.x = this->transform.M41;
        this->ds3DProps.vPosition.y = this->transform.M42;
        this->ds3DProps.vPosition.z = this->transform.M43;
        this->ds3DProps.vVelocity.x = this->velocity.x;
        this->ds3DProps.vVelocity.y = this->velocity.y;
        this->ds3DProps.vVelocity.z = this->velocity.z;
        this->ds3DProps.dwInsideConeAngle = (DWORD) this->insideConeAngle;
        this->ds3DProps.dwOutsideConeAngle = (DWORD) this->outsideConeAngle;
        this->ds3DProps.vConeOrientation.x = this->transform.M31;
        this->ds3DProps.vConeOrientation.y = this->transform.M32;
        this->ds3DProps.vConeOrientation.z = this->transform.M33;
        this->ds3DProps.lConeOutsideVolume = AsDirectSoundVolume(this->coneOutsideVolume);
        this->ds3DProps.flMinDistance = this->minDist;
        this->ds3DProps.flMaxDistance = this->maxDist;
        this->ds3DProps.dwMode = DS3DMODE_NORMAL;
    }
    return &(this->ds3DProps);
}

//------------------------------------------------------------------------------
/**
*/
inline
nDSoundResource* 
nDSound3::GetDSoundResource()
{
    return this->refSoundResource.get();
}

//------------------------------------------------------------------------------
/**
*/
inline
CSound*
nDSound3::GetCSoundPtr()
{
    if ( this->refSoundResource.isvalid() )
    {
        return this->refSoundResource->GetCSoundPtr();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
#endif    
