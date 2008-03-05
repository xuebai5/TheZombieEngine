#ifndef NC_SOUND_H
#define NC_SOUND_H
//------------------------------------------------------------------------------
/**
    @class ncSound
    @ingroup Entities
    @author Juan Jose Luna Espinosa

    @brief Entity component to keep track of played sounds on an entity

    (C) 2005 Conjurer Services, S.A.
*/

#include "entity/nentity.h"
#include "nsoundeventtable/nsoundeventtable.h"
#include "nsoundscheduler/nsoundscheduler.h"

class ncTransform;
struct nGameEvent;
class nstream;

//------------------------------------------------------------------------------
class ncSound : public nComponentObject
{

    NCOMPONENT_DECLARE(ncSound,nComponentObject);

public:

    /// Duration of hardcoded fadeout to prevent stopping clicks
    #define FadeoutDuration 0.1f

    /// Decay of volume in that fadeout
    #define StopDecay 0.9f

    /// constructor
    ncSound();
    /// destructor
    virtual ~ncSound();
    /// component persistency
    bool SaveCmds(nPersistServer* ps);
	
    /// Set position
    void SetSoundPosition(const vector3&);
    /// Set position and velocity
    void SetSoundPositionAndVelocity(const vector3&, const vector3&);

    /// Play a sound
    void Play(const nString&, int);
    /// Play a sound given a material
    void PlayMaterial(const nString&, const nString&, int);

    /// Play a sound without attaching it to an entity
    void PlayAlone(const nString&, int);
    /// Play a sound given a material without attaching it to an entity
    void PlayAloneMaterial(const nString&, const nString&, int);

    /// Play a sound at a position without attaching it to an entity
    void PlayAloneAtPosition(const nString&, int, const vector3 &);
    /// Play a sound at a position given a material without attaching it to an entity
    void PlayAloneMaterialAtPosition(const nString&, const nString&, int, const vector3 &);

    /// Play a sound with all possible parameters
    void PlayExtended( const nString& event, const nString& material, int repeat, nTime duration = nTime(0.0f), bool calcVar = false);
    /// Stop last sound
    virtual void StopSound();

    /// Set volume
    void SetSoundVolume(float);
    /// Get volume
    float GetSoundVolume () const;
    /// Set frequency
    void SetSoundFrequency(float);
    /// Get frequency
    float GetSoundFrequency () const;

    /// Set volume setting (default modulating volume for all sounds)
    void SetSoundDefaultVolume(float);
    /// Get volume setting (default modulating volume for all sounds)
    float GetSoundDefaultVolume () const;

    /// Enable sound trigger
    void EnableSoundTrigger(nGameEvent*);
    /// Disable sound trigger
    void DisableSoundTrigger(nGameEvent*);
    /// Synchronize the trigger shape radius with the greatest maximum distance among all the table's sounds
    void SynchronizeTriggerShape();

    /// Play a sound on all the clients
    void SendEventToPlay(const char*, int);
    /// Play a sound on all the clients (nicer name to be used by designers)
    void PlaySoundEvent(const char*, int);
    /// Stop playing sound on all the clients
    void SendStopSound();
    /// Stop playing sound on all the clients (nicer name to be used by designers)
    void StopLastSound();

    /// set a fixed offset for sound position from entity position
    void SetSoundPositionOffset(vector3 offset);

    /// initialize component pointers from entity object
    void InitInstance( nObject::InitInstanceMsg /*initType*/ );

    /// Set current sound and sound times
    void SetSoundParams( const nSoundScheduler::SoundComponentInfo& scInfo );

    /// Get current sound and sound times
    nSoundScheduler::SoundComponentInfo* GetSoundParams();

	/// Get current sound and sound times, read only
    const nSoundScheduler::SoundComponentInfo* GetSoundParams() const;

    /// Set buffer index currently used
    void SetBufferIndex( int index );
   
    /// Get position from entity
    void GetPositionFromEntity();

    /// Get the flags
    int GetFlags() const;

    /// Sets the flags
    void SetFlags(int flags);

	/// Is playing?
	bool IsPlaying() const;

    /// Play the sound specified by the server
    void ReceiveEventToPlay( nstream* data );

    /// get a sound index
    int GetSoundIndex( const char * event );

#ifndef NGAME
    /// Enter limbo 
    void DoEnterLimbo();
    /// Exit limbo 
    void DoExitLimbo();
#endif

private:

    /// Setup called from all play methods
    int PrePlay( const char* event, const char* material = 0, bool standAlone = false );

    /// Info for keeping track of one sound
    nSoundScheduler::SoundComponentInfo soundInfo;

    /// Default volume setting for all sounds
    float defaultVolume;

};

//------------------------------------------------------------------------------
#endif
