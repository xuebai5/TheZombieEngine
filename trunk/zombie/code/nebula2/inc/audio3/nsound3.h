#ifndef N_SOUND3_H
#define N_SOUND3_H
//------------------------------------------------------------------------------
/**
    @class nSound3
    @ingroup Audio3

    Hold parameters for a sound instance. Usually a "game object" holds
    one or more nSound3 objects for all the sounds it has to play.
    Although nSound3 is derived from nResource, nSound3 objects should
    never be shared (this is enforced by the factory method
    nAudioServer3::NewSound()).

    When opened, a nSound3 object will create a shared static or streaming
    sound resource, based on the settings of nSound3.
    
    (C) 2003 RadonLabs GmbH
*/
#include "resource/nresource.h"
#include "audio3/naudioserver3.h"

//------------------------------------------------------------------------------
class nSound3 : public nResource
{
public:

    /// sound buffer status enum
    enum SoundBufferStatus
    {
        // none
        SOUNDBUFFER_NONE = 0,
        // playing
        SOUNDBUFFER_PLAYING = 1 << 0,
        // terminated, buffer is used for other sound
        SOUNDBUFFER_TERMINATED = 1 << 1,
        // lost, buffer is not in memory
        SOUNDBUFFER_LOST = 1 << 2,
    };

    /// constructor
    nSound3();
    /// destructor
    virtual ~nSound3();
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
    /// return true if any buffer of sound currently playing
    virtual bool IsPlaying();
    /// return true if sound buffer is currently playing
    virtual bool IsPlaying(int index);
    /// get sound buffer status
    virtual int GetStatus(int index);
    /// return duration of sound in seconds
    virtual float GetDuration();
    ///return last buffered used for play
    virtual int GetLastBufferUsed();
    /// set sound playing cursor in seconds
    virtual void SetTrackPosition( int index, float seconds );
    /// set the number of simultaneous "playback tracks" this sound should support
    void SetNumTracks(int t);
    /// get number of tracks
    int GetNumTracks() const;
    /// set the ambient flag (ambient sounds are non-positional)
    void SetAmbient(bool b);
    /// get the ambient flag
    bool GetAmbient() const;
    /// set static/streaming type
    void SetStreaming(bool b);
    /// get static/streaming type
    bool GetStreaming() const;
    /// set the looping behaviour
    void SetLooping(bool b);
    /// get the looping behaviour
    bool GetLooping() const;
    /// set the freq. control flag
    void SetFreqControl(bool b);
    /// get the freq. control flag
    bool GetFreqControl() const;
    /// set the sound priority for voice management
    void SetPriority(int pri);
    /// get the sound priority for voice management
    int GetPriority() const;
    /// set the playback volume
    void SetVolume(float v);
    /// get the playback volume
    float GetVolume() const;
    /// set the playback frequency
    void SetFrequency(float v);
    /// get the playback frequency
    float GetFrequency() const;
    /// set position
    void SetPosition(const vector3& pos);	
    /// set position and velocity
    void SetPositionAndVelocity(const vector3& pos, const vector3& vel);
    /// set world transform
    void SetTransform(const matrix44& m);
    /// get world transform
    const matrix44& GetTransform() const;
    /// set the velocity
    void SetVelocity(const vector3& v);
    /// get the velocity
    const vector3& GetVelocity() const;
    /// set the minimum distance (sound doesn't get louder when closer)
    void SetMinDist(float d);
    /// get the mimimum distance
    float GetMinDist() const;
    /// set the maximum distance (sound will not be audible beyond that distance
    void SetMaxDist(float d);
    /// get the maximum distance
    float GetMaxDist() const;
    /// set the inside cone angle in degrees
    void SetInsideConeAngle(int a);
    /// get the inside cone angle
    int GetInsideConeAngle() const;
    /// set the outside cone angle in degrees
    void SetOutsideConeAngle(int a);
    /// get the outside cone angle
    int GetOutsideConeAngle() const;
    /// set the cone outside volume (0.0 .. 1.0)
    void SetConeOutsideVolume(float v);
    /// get the cone outside volume
    float GetConeOutsideVolume() const;
    /// copy audio attributes from another sound object
    void CopySoundAttrsFrom(const nSound3* other);
    /// set category of sound
    void SetCategory(nAudioServer3::Category category);
    /// get category of sound
    nAudioServer3::Category GetCategory() const;

protected:
    int numTracks;
    bool ambient;
    bool streaming;
    bool looping;
    bool freqControl;
    int priority;
    float volume;
    float frequency;
    matrix44 transform;
    vector3 velocity;
    float minDist;
    float maxDist;
    int insideConeAngle;
    int outsideConeAngle;
    float coneOutsideVolume;
    nAudioServer3::Category category;
    bool volumeDirty;
    bool props3DDirty;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::CopySoundAttrsFrom(const nSound3* other)
{
    n_assert(other);
    this->SetNumTracks(other->GetNumTracks());
    this->SetAmbient(other->GetAmbient());
    this->SetStreaming(other->GetStreaming());
    this->SetLooping(other->GetLooping());
    this->SetFreqControl(other->GetFreqControl());
    this->SetPriority(other->GetPriority());
    this->SetVolume(other->GetVolume());
    this->SetFrequency(other->GetFrequency());
    this->SetTransform(other->GetTransform());
    this->SetVelocity(other->GetVelocity());
    this->SetMinDist(other->GetMinDist());
    this->SetMaxDist(other->GetMaxDist());
    this->SetInsideConeAngle(other->GetInsideConeAngle());
    this->SetOutsideConeAngle(other->GetOutsideConeAngle());
    this->SetConeOutsideVolume(other->GetConeOutsideVolume());
    this->SetCategory(other->GetCategory());
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetNumTracks(int t)
{
    this->numTracks = t;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSound3::GetNumTracks() const
{
    return this->numTracks;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetAmbient(bool b)
{
    this->ambient = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSound3::GetAmbient() const
{
    return this->ambient;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetStreaming(bool b)
{
    this->streaming = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSound3::GetStreaming() const
{
    return this->streaming;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetLooping(bool b)
{
    this->looping = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSound3::GetLooping() const
{
    return this->looping;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
nSound3::GetFreqControl() const
{
    return this->freqControl;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetFreqControl(bool b)
{
    this->freqControl = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetPriority(int p)
{
    this->priority = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSound3::GetPriority() const
{
    return this->priority;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetVolume(float v)
{
    this->volume = v;
    this->volumeDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound3::GetVolume() const
{
    return this->volume;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetFrequency(float f)
{
    this->frequency = f;
    this->volumeDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound3::GetFrequency() const
{
    return this->frequency;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetPosition(const vector3& pos)
{
    this->transform.set_translation( pos );
    this->props3DDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetPositionAndVelocity(const vector3& pos, const vector3& vel)
{
    this->transform.set_translation( pos );
    this->velocity = vel;
    this->props3DDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetTransform(const matrix44& m)
{
    this->transform = m;
    this->props3DDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
nSound3::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetVelocity(const vector3& v)
{
    this->velocity = v;
    this->props3DDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
const vector3&
nSound3::GetVelocity() const
{
    return this->velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetMinDist(float d)
{
    this->minDist = d;
    this->props3DDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound3::GetMinDist() const
{
    return this->minDist;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetMaxDist(float d)
{
    this->maxDist = d;
    this->props3DDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound3::GetMaxDist() const
{
    return this->maxDist;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetInsideConeAngle(int a)
{
    this->insideConeAngle = a;
    this->props3DDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSound3::GetInsideConeAngle() const
{
    return this->insideConeAngle;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetOutsideConeAngle(int a)
{
    this->outsideConeAngle = a;
    this->props3DDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
int
nSound3::GetOutsideConeAngle() const
{
    return this->outsideConeAngle;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetConeOutsideVolume(float v)
{
    this->coneOutsideVolume = v;
    this->props3DDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
nSound3::GetConeOutsideVolume() const
{
    return this->coneOutsideVolume;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nSound3::SetCategory(nAudioServer3::Category c)
{
    n_assert(c >= 0 && c < nAudioServer3::NumCategorys);
    this->category = c;
}

//------------------------------------------------------------------------------
/**
*/
inline
nAudioServer3::Category
nSound3::GetCategory() const
{
    return this->category;
}

//------------------------------------------------------------------------------
#endif

