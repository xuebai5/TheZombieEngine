#ifndef N_SOUNDSCHEDULER_H
#define N_SOUNDSCHEDULER_H
//------------------------------------------------------------------------------
/**
    @file nsoundscheduler.h
    @class nSoundScheduler

    @author Juan Jose Luna Espinosa

    @brief Singleton to keep track of playing sounds

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "kernel/nobject.h"
#include "kernel/ncmdprotonativecpp.h"
#include "entity/nentityobject.h"
#include "nsoundlibrary/nsoundlibrary.h"

//------------------------------------------------------------------------------
class nAudioServer3;
class nSound3;
class nSoundLibrary;
class ncSound;
class nAppViewport;

/// Fixed path of sound library
extern const char* soundLibraryPath;

/// NOH path of sound library
extern const char* soundLibraryNOHPath;

//------------------------------------------------------------------------------
class nSoundScheduler: public nRoot
{
public:

    /// Invalid sound index on any array
    #define InvalidSoundIndex -1

    /// Playing sound state's flags. State can be changed with the Set/GetState functions, but flags may be set only at initialization
    enum PlayingSoundFlags
    {
        /// State bits
        Idle = 0,
        Queued,
        Playing,
        Stopping,

        /// State mask
        StateMask = 3,

        /// Playing mask
        PlayingMask = 2,

        /// Looping bit
        Looping = 8,

        /// Stop fadeout bit
        StopFadeOut = 16,

        /// Mobile bit
        Mobile = 32,

        /// Calculate variation for each repetition
        CalcVariation = 64,

        /// Sound is part of a 2D area trigger
        Is2DAreaSound = 128,

        /// Sound has varying volume or frequency
        DynamicVolFreq = 256,
    };

    /// Info for tracking a playing sound in the component
    struct SoundComponentInfo
    {
        /// State
        int flags;

        /// nSound3 pointer
        nSound3* sound;

        /// ncSound pointer
        ncSound* soundComponent;

        /// Index of underlying sound buffer used by this entity
        int bufferIndex;

        /// Start time
        nTime startTime;

        /// Stoptime
        nTime endTime;

        /// Library sound index
        int soundIndex;

        /// Repetition counter
        int repeats;

        /// Duration stored for succcesive repeats
        nTime duration;

        //@todo JJ playing parameters could by avoided with AcquireResources()

        /// Initial (requested) playing volume
        float initialVol;

        /// Playing volume (affected by group volumes and other settings)
        float playVol;

        // Current volume (used in fading, etc)
        float currentVol;

        /// Playing frequency, multiples of base frequency
        float playFreq;

        /// Playing pitch
        // @todo JJ

        /// position offset
        vector3 positionOffset;

        /// position offset-set flag
        bool hasOffset;

        /// Editor time info

        /// Temp info
        // position
        vector3 soundPos;

        /// Constructor
        SoundComponentInfo()
        {
            sound = 0;
            soundComponent = 0;
            bufferIndex = InvalidSoundIndex;
            startTime = 0.0f;
            endTime = 0.0f;
            flags = Idle;
            repeats = 0;
            hasOffset = false;
            positionOffset = vector3(0.0f,0.0f,0.0f);
        }

        /// Set state
        void SetState(int state)
        {
            n_assert( ! (state & ~StateMask) );
            this->flags = (this->flags & ~StateMask ) | state;
        }

        /// Get state
        int GetState() const
        {
            return this->flags & StateMask;
        }

        /// IsPlaying
        bool IsPlaying() const
        {
            return ( this->GetState() & PlayingMask ) != 0;
        }

        /// Get looping flag
        bool GetLooping() const
        {
            return ( this->flags & Looping ) != 0;
        }

        /// Get stop fadeout flag
        bool GetStopFadeOut() const
        {
            return ( this->flags & StopFadeOut ) != 0;
        }

        /// Get mobile flag
        bool GetMobile() const
        {
            return ( this->flags & Mobile ) != 0;
        }

        /// Get CalcVariation flag
        bool GetCalcVariation() const
        {
            return ( this->flags & CalcVariation ) != 0;
        }

        /// Get Is2DAreaSound flag
        bool Get2DAreaSound() const
        {
            return ( this->flags & Is2DAreaSound ) != 0;
        }

        /// Get if sound has varying volume or frequency
        bool GetDynamicVolFreq() const
        {
            return ( this->flags & DynamicVolFreq ) != 0;
        }

        // Set position and velocity to the nSound3
        void SetSoundPositionAndVelocity(const vector3& pos, const vector3& vel);
        
        // Set position
        void SetSoundPosition(const vector3& pos);

        // Stop sound
        void Stop();

    };

    /// Slot flags
    /*enum SlotFlags
    {
        SlotUsed =      1 << 0,
        SlotPlaying =   1 << 1
    };*/

    ///// Sound instance slot
    //struct SoundSlot 
    //{
    //    /// Index of this slot in the array
    //    int index;

    //    /// Slot flags
    //    nuint8 flags;

    //    /// nSound3 instance
    //    nSound3* sound;

    //    /// Index of underlying sound buffer used by this slot
    //    int bufferIndex;

    //    /// Reference to entity sound component
    //    ncSound* soundComp;
    //};

    // Constructor
    nSoundScheduler();

    // Destructor
    virtual ~nSoundScheduler();

    /// Get singleton
    static nSoundScheduler* Instance();

    /// Get the sound library
    nSoundLibrary* GetSoundLibrary();

    /// Initialize sound scheduler
    void Init();

    /// Play a sound associating it to an entity
    void PlayEntity( nEntityObject* caller, int soundIndex, int repeat, nTime duration = nTime(0.0f), bool calcVariation = false );
    
    /// Play a sound not entitled to an entity
    void Play(vector3 pos, int soundIndex, int repeat, float duration = nTime(0.0f), bool calcVariation = false );

    /// Play a sound given her sound name
    void PlayBySoundName( vector3 position, const char* soundName, int repeat );

    /// Abort a sound playback
    void AbortSoundPlayback( SoundComponentInfo* soundInfo );

    /// Find a free sound slot
    SoundComponentInfo * nSoundScheduler::FindFreeSound();

    /// Find a free sound slot
    SoundComponentInfo * FindPlayingSound( int soundIndex );
    
    /// Trigger scheduler
    void Trigger(nTime time);

    /// Set current time
    void SetTime(nTime time);

    /// Get current time
    nTime GetTime();

    /// say if a sound is loaded
    bool IsLoadedSound( int libraryIndex );

    /// Load a sound
    bool LoadSound( int libraryIndex );

    /// Unload a sound
    bool UnloadSound( int libraryIndex );

    /// Reload a sound
    bool ReloadSound( int libraryIndex );

    /// Insert new sound
    bool InsertNewSound( int libraryIndex );

    /// Reset library
    void Reset();

    /// Load library from fixed path
    void LoadSoundLibrary(const char*);

    /// Load library samples
    void LoadAllSamples(bool);

    /// Stop all sounds
    void StopAllSounds ();

    /// Set hearing viewport
    void SetHearingViewport(nAppViewport *);

    /// Get hearing viewport
    nAppViewport * GetHearingViewport ();

    /// Persist info
    bool SaveCmds (nPersistServer* ps);
    
    #ifndef NGAME
    /// Scan for new sounds
    void ScanNewSounds( nString & path, bool createSoundSources = false, bool recursiveSearch = true );
    #endif

private:

    /// Process a playback query
    void PlayQuery( SoundComponentInfo* soundParams, int soundIndex, int repeat, nTime duration = 0.0f, bool calcVariation = true, float volume = 1.0f );

    /// Set library sound parameters
    void PutSoundParameters( nSound3* sound, const nSoundLibrary::SoundInfo& info ) const;

    /// Check if a file is a sound resource
    bool IsSoundFile( const char * fileName );

    /// Singleton
    static nSoundScheduler* singleton;

    /// Reference to audio server
    nAutoRef<nAudioServer3> refAudioServer;

    /// Reference to sound library
    nRef<nSoundLibrary> refSoundLibrary;

    /// Current time
    nTime currentTime;

    ///Runtime info for all sounds
    nArray<nSound3*> runtimeInfo;

    /// List of playing sounds
    nArray<SoundComponentInfo*> playingSounds;

    /// Pool of sounds not associated to entities @todo this will be the soundcomponentinfo pool
    nArray<SoundComponentInfo> freeSoundsPool;

    /// Hearing viewport
    nRef<nAppViewport> hearingViewport;

};
//------------------------------------------------------------------------------
#endif

