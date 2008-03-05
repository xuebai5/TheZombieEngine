#ifndef N_SOUNDLIBRARY_H
#define N_SOUNDLIBRARY_H
//------------------------------------------------------------------------------
/**
    @file nsoundlibrary.h
    @class nSoundLibrary

    @author Juan Jose Luna Espinosa

    @brief Global table of all positional sound resources

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "util/nmaptabletypes.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
class ncSound;
class nSoundEventTable;

//------------------------------------------------------------------------------
class nSoundLibrary: public nRoot
{
friend class nSoundScheduler;
public:

    struct SoundInfo
    {
        /// Sound string unique id
        nString soundId;

        /// Sound file name
        nString resourceName;

        /// Min distance
        float minDist;

        /// Max distance
        float maxDist;

        /// Volume setting
        float volume;

        /// Pitch variation
        float pitchVar;

        /// Volume variation
        float volVar;

        /// Delay between event and playback
        float delay;

        /// 2D sound flag (environmental stereo sound rather than mono, 3d sound)
        bool is2d;

        /// Mobile sound flag (false means that the sound doesn't need position update)
        bool mobile;

        /// Pitch control flag (sound is pitch-shiftable)
        bool pitchControl;

        /// Max number of simultaneous sound instances 
        int maxSoundInstances;

        /// Sound is streamed?
        bool streamed;

        /// Priority
        int priority;

        /// Number of sound variations (sound rows with same soundId)
        int numberOfVariations;

#ifndef NGAME

        /// Number of this variation
        int variationNumber;

        /// Is a new record, added automatically
        bool isNew;

        /// Mute flag
        bool mute;

        /// Solo flag
        bool solo;

        /// Internal flag used when other sound is solo'ed
        bool silenced;

        /// Sound group index
        int soundGroupIndex;

        /// Event associated in the resource file name
        nString eventName;

        /// Material associated in the resource file name
        nString materialName;

        /// Magic number
        nEntityObjectId magicNumber;
#endif
    };

    struct SoundIdTableInfo {
        /// Sound index in the library table, of the first sound variation
        int soundIndex;
    };

    // Constructor
    nSoundLibrary();

    // Destructor
    virtual ~nSoundLibrary();

    /// Get singleton
    static nSoundLibrary* Instance();

    /// Get info for a sound by index
    SoundInfo& GetSoundInfo( int index );
    /// Get info for a sound by index, for read only
    const SoundInfo& GetSoundInfo( int index ) const;

#ifndef NGAME
    /// Increment playing sound counter
    void IncPlayingCount();

    /// Decrement playing sound counter
    void DecPlayingCount();

    /// Get playing sound counter
    int GetPlayingCount();

    /// Import a sound group to a entity sound table
    bool ImportSoundGroup( nSoundEventTable * table, const char * groupName );
#endif

    /// Add a new sound
    int NewSound(const char*, const char*, bool, bool, int, int );
    /// Add a new sound, command for persistency
    int AddSound(const char*, const char*, bool, bool, int, int );
    /// Set sound parameters - obsolete
    void SetSoundParams(const char*, bool, float, float, float);
    /// Set time sound parameters - obsolete
    void SetSoundTimeParams(const char*, float, float, float);

    /// Set sound parameters
    void SetSoundParams2(const char*, int, bool, float, float, float);
    /// Set time sound parameters
    void SetSoundTimeParams2(const char*, int, float, float, float);

    /// Add a new sound with default parameters
    int AddDefaultSound(const char*);
    /// Add a new sound with default parameters and the given sound resource
    int AddDefaultSoundWithResource(const char*, const char* soundRsrc);
    /// Remove a sound by index
    bool RemoveSound(int);
    /// Get number of sounds
    int GetNumberOfSounds () const;
    /// Get sound start index
    int GetSoundIndex(const char *) const;
    /// Get the sound id
    nString GetSoundId(int) const;
    /// Get the resource
    nString GetResource(int) const;
    /// Set the resource
    void SetResource(int, const char*);
    /// Get the 2d flag
    bool Get2dFlag(int) const;
    /// Set the 2d flag
    void Set2dFlag(int, bool);
    /// Get the minimum distance
    float GetMinDist(int) const;
    /// Set the minimum distance
    void SetMinDist(int, float);
    /// Get the maximum distance
    float GetMaxDist(int) const;
    /// Set the maximum distance
    void SetMaxDist(int, float);
    /// Get the volume setting
    float GetVolume(int) const;
    /// Set the volume setting
    void SetVolume(int, float);
    /// Get the pitch variation
    float GetPitchVar(int) const;
    /// Set the pitch variation
    void SetPitchVar(int, float);
    /// Get the volume variation
    float GetVolumeVar(int) const;
    /// Set the volume variation
    void SetVolumeVar(int, float);
    /// Get the mobile flag value
    bool GetMobile(int) const;
    /// Set the mobile flag value
    void SetMobile(int, bool);
    /// Get the pitch control flag value
    bool GetPitchControl(int) const;
    /// Set the pitch control flag value
    void SetPitchControl(int, bool);
    /// Set the pitch control bt sound id
    void SetPitchControlById(const char *, bool);
    /// Get the maximum number of instances
    int GetMaxInstances(int) const;
    /// Set the maximum number of instances
    void SetMaxInstances(int, int);
    /// Get the streamed flag value
    bool GetStreamed(int) const;
    /// Set the streamed flag value
    void SetStreamed(int, bool);
    /// Get the priority
    int GetPriority(int) const;
    /// Set the priority
    void SetPriority(int, int);
    /// Get the delay setting
    float GetDelay(int) const;
    /// Set the delay setting
    void SetDelay(int, float);

    /// Play a standalone sound
    void Play(int);
    /// Play a standalone sound looped
    void PlayRepeat(int,int);
    /// Stop all sounds
    void StopAllSounds ();

    /// Get the dirty flag value
    bool GetDirty () const;
    /// Set the dirty flag value
    void SetDirty(bool);

    /// Get sound number of variations
    int GetSoundNumberOfVariations(int) const;

    /// Set magic number
    void SetMagicNumber(unsigned int);

    /// Get max number of free sounds playing
    int GetMaxFreeSounds () const;
#ifndef NGAME
    /// Set max number of free sounds playing
    void SetMaxFreeSounds(int);
    /// Get 'new sound' flag
    bool IsNewSound(int) const;
    /// Set mute flag to a sound
    void SetMuteFlag(int, bool);
    /// Set solo flag to a sound
    void SetSoloFlag(int, bool);
    /// Get mute flag for a sound
    bool GetMuteFlag(int) const;
    /// Get solo flag for a sound
    bool GetSoloFlag(int) const;
    /// Reset all mute and solo flags
    void ResetAllMuteSolo ();

    /// Get number of sound event groups
    int GetNumberOfSoundGroups() const;

    /// Get a sound event group name
    nString GetSoundGroupName(int) const;

    /// Add a sound to a group name
    void AddSoundGroupName(const char *, const char *, const char *, const char *);

#endif NGAME

    /// Get sound scheduler
    nObject * GetSoundScheduler ();

    /// @name Signals interface
    //@{
    /// Added new sound (soundId, file path)
    NSIGNAL_DECLARE('JSAL', void, SoundAddedToLibrary, 2, (const char *, const char *), 0, ());
    /// A sound form the library couldn't be loaded (soundId, file path)
    NSIGNAL_DECLARE('JFLS', void, FailedLoadingSound, 2, (const char *, const char *), 0, ());
    //@}

    /// Set 'new sound' flag
    void SetNewSound( int index, bool isNew );

    /// Persist info
    bool SaveCmds (nPersistServer* ps);

protected:
private:

    ///Singleton
    static nSoundLibrary* singleton;

    /// Recalculate sound indexes stored in the mapping table
    void RecalculateIndexes();

    /// Recalculate parameters for variations of each sound
    void CalculateVariationsParameters();

    /// Remove a sound event recursively from class tables
    void RemoveSoundRowsInClass( const char * className, const char* soundId ) const;

    /// Map table from sound id strings to global table index
    nMapTableTypes<SoundIdTableInfo>::NString soundIdsMapping;

    /// Global sound table
    nArray<SoundInfo> soundLibrary;

    /// Path of this object's file
    nString filePath;

    /// Dirty flag
    bool isDirty;

    /// Max number of sounds playing for free (i.e. not associated to an entity)
    int maxFreeSounds;

#ifndef NGAME
    /// Playing sounds counter
    int playingSoundsCounter;

    /// List of sound event groups for assigning to the entity classes
    nArray<nString> soundEventGroups;
#endif

    /// Current magic number
    nEntityObjectId magicNumber;

};
//------------------------------------------------------------------------------
#endif

