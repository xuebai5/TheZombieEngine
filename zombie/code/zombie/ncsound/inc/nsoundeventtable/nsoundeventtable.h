#ifndef N_SOUNDEVENTTABLE_H
#define N_SOUNDEVENTTABLE_H
//------------------------------------------------------------------------------
/**
    @file nsoundeventtable.h
    @class nSoundEventTable

    @author Juan Jose Luna Espinosa

    @brief Table of sounds for an entity class

    (C) 2004 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "kernel/nroot.h"
#include "kernel/ncmdprotonativecpp.h"
#include "util/nmaptabletypes.h"

//------------------------------------------------------------------------------
class nSoundEventTable: public nObject
{
public:

    struct SoundMapInfo
    {
        /// Sound id string in the global Sound Library
        nString soundId;

        /// Index of the sound in the global Sound Library
        int soundIndex;
    };

    // Constructor
    nSoundEventTable();

    // Destructor
    virtual ~nSoundEventTable();

    /// Get sound id with event
    const char* GetSoundId(const char* event) const;

    /// Get sound global index with event
    int GetSoundIndex(const char* event) const;

    /// Get sound global index with event & material
    int GetSoundIndex(const char* event, const char* material) const;

    /// Get the greatest maximum distance among all the sounds
    float GetGreatestMaxDistance();

    /// Generate a key from an event and material
    nString MakeKey( const char* event, const char* material ) const;

    /// Extract the event and material from a key
    void UnmakeKey( const nString& key, nString& event, nString& material ) const;

    /// Remove all event rows that refers to a sound name
    bool RemoveSoundRows( const char*  soundId );

    /// Add a sound event to the table
    void AddSoundEvent(const char*, const char*, const char*);
    /// Remove a sound event by event & material
    void RemoveSoundEvent(const char*, const char*);
    /// Get number of sound events
    int GetNumberOfSounds () const;
    /// Get full sound event by index
    void GetSoundEvent(int, nString&, nString&, nString&);
    /// Get sound id by event & material
    const char* GetSoundId(const char*, const char*) const;
    /// Set sound id by event & material
    void SetSoundId(const char*, const char*, const char*);

    /// load the sounds
    bool LoadSounds();

    /// load the sounds
    bool UnloadSounds();

#ifndef NGAME
    /// Import a set of sound events from a group of the sound library
    bool ImportSoundGroup(const char*);
#endif

    /// Duplicate a set of sound events
    int DuplicateSoundEvent(const char*, const char*);

    /// Persist info
    bool SaveCmds (nPersistServer* ps);

protected: 
private:

    // Map table from event string identifiers to nSound3*
    nMapTableTypes<SoundMapInfo>::NString soundMap;

};
//------------------------------------------------------------------------------
#endif

