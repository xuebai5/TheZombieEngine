#include "precompiled/pchncsound.h"
//------------------------------------------------------------------------------
//  nsoundlibrary_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "stdlib.h"
#include "nsoundlibrary/nsoundlibrary.h"
#include "nsoundscheduler/nsoundscheduler.h"
#include "entity/nentityobjectserver.h"
#include "ncsound/ncsoundclass.h"
#include "nsoundeventtable/nsoundeventtable.h"
#include "entity/nentityclassserver.h"
#include "napplication/napplication.h"
#include "audio3/nlistener3.h"
#include "kernel/nfileserver2.h"

#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nSoundLibrary, "nroot");

//------------------------------------------------------------------------------
nSoundLibrary* nSoundLibrary::singleton = 0;

//------------------------------------------------------------------------------
NSIGNAL_DEFINE( nSoundLibrary, SoundAddedToLibrary );
NSIGNAL_DEFINE( nSoundLibrary, FailedLoadingSound );

//------------------------------------------------------------------------------
const char * EditStringError = "Can't change the sound file because there are sounds playing. Please stop all sounds before changing sound files";

//------------------------------------------------------------------------------
/**
*/
nSoundLibrary::nSoundLibrary():
    soundIdsMapping(101),
    isDirty(false),
    maxFreeSounds( 10 )
#ifndef NGAME
    ,playingSoundsCounter( 0 )
#endif
{
    singleton = this;
}
//------------------------------------------------------------------------------
/**
*/
nSoundLibrary::~nSoundLibrary()
{
#ifndef NGAME
    this->soundEventGroups.Reset();
#endif
}

//------------------------------------------------------------------------------
/**
    @brief Get singleton
*/
nSoundLibrary*
nSoundLibrary::Instance()
{
    return singleton;
}

//------------------------------------------------------------------------------
/**
    @brief Add a sound to the library, this version is for a new sound created in this machine
    @param soundId Unique string identifier for the sound
    @param soundRsrc Full path of sound
    @param is2d Is a 2d environmental sound (or a 3d mono sound)
    @param streamed Is the sound streamed
    @param priority Sound priority
    @param maxSoundInstances Max number of total sounds playing

    @return Index of added sound
*/
int
nSoundLibrary::NewSound( const char* soundId, const char* soundRsrc, bool is2d, bool streamed, int priority, int maxSoundInstances )
{
    this->SetMagicNumber( nEntityObjectServer::Instance()->GetHighId() );

    // Check that resource file is not used yet in the library
    if ( *soundRsrc )
    {
        for ( int i = 0; i < this->soundLibrary.Size(); i++ )
        {
            if ( strcmp( nFileServer2::Instance()->ManglePath( this->soundLibrary[ i ].resourceName ).Get(),
                        nFileServer2::Instance()->ManglePath( soundRsrc ).Get() ) == 0 )
            {
                return InvalidSoundIndex;
            }
        }
    }

    int retValue = this->AddSound(soundId, soundRsrc, is2d, streamed, priority, maxSoundInstances );

    if ( retValue != InvalidSoundIndex )
    {
        nSoundScheduler::Instance()->InsertNewSound( retValue );
    }
    return retValue;
}

//------------------------------------------------------------------------------
/**
    @brief Add a sound to the library, command for persistency
    @param soundId Unique string identifier for the sound
    @param soundRsrc Full path of sound
    @param is2d Is a 2d environmental sound (or a 3d mono sound)
    @param streamed Is the sound streamed
    @param priority Sound priority
    @param maxSoundInstances Max number of total sounds playing

    @return Index of added sound
*/
int
nSoundLibrary::AddSound( const char* soundId, const char* soundRsrc, bool is2d, bool streamed, int priority, int maxSoundInstances )
{
    n_assert( soundId && soundRsrc );

#ifndef NGAME
    if ( this->playingSoundsCounter )
    {
        NLOG(resource, (NLOGUSER, "Can't add sound because there are sounds playing. Please stop all sounds before editing the library" ));
        return InvalidSoundIndex;
    }
#endif

    SoundInfo soundInfo;
    soundInfo.soundId = soundId;
    soundInfo.resourceName = soundRsrc;
    soundInfo.minDist = 1.0f;
    soundInfo.maxDist = 100.0f;
    soundInfo.volume = 1.0f;
    soundInfo.pitchVar = 0.0f;
    soundInfo.volVar = 0.0f;
    soundInfo.delay = 0.0f;
    soundInfo.is2d = is2d;
    soundInfo.streamed = streamed;
    soundInfo.priority = priority;
    soundInfo.maxSoundInstances = maxSoundInstances;
    soundInfo.mobile = true;
    soundInfo.pitchControl = false;

#ifndef NGAME
    soundInfo.isNew = false;
    soundInfo.mute = false;
    soundInfo.solo = false;
    soundInfo.silenced = false;
    
    soundInfo.soundGroupIndex = -1;

    soundInfo.magicNumber = this->magicNumber;
#endif

    // Insert info in id map table, or increment variations number if it existed
    SoundIdTableInfo* idInfo = this->soundIdsMapping[ soundId ];
    int index = 0;
    if ( idInfo )
    {
        // Insert sound just after the variations of the same sound
        index = 0;
        while ( index < this->soundLibrary.Size() && this->soundLibrary[ index ].soundId != soundId )
        {
            index++;
        }
        int varNumber = 0;

        int numberOfVar = 1;
        if( index < this->soundLibrary.Size() )
        {
            numberOfVar = this->soundLibrary[ index ].numberOfVariations + 1;
        }

        while ( index < this->soundLibrary.Size() && this->soundLibrary[ index ].soundId == soundId )
        {
            this->soundLibrary[ index ].numberOfVariations = numberOfVar;

            index++;
            varNumber++;
        }
        n_assert( index <= this->soundLibrary.Size() );

        soundInfo.numberOfVariations = numberOfVar;
#ifndef NGAME
        soundInfo.variationNumber = varNumber;
#endif
        // Insert sound info in library
        this->soundLibrary.Insert( index, soundInfo );

    }
    else
    {
        SoundIdTableInfo newInfo;
        idInfo = &newInfo;
       
        soundInfo.numberOfVariations = 1;

#ifndef NGAME

        soundInfo.variationNumber = 0;

        // Insert alfabetically
        while ( index < this->soundLibrary.Size() && strcmp( this->soundLibrary[ index ].soundId.Get(),  soundId ) < 0 )
        {
            index++;
        }
#else
        // Insert at end of library
        index = this->soundLibrary.Size();
#endif

        // Store index in mapping info
        idInfo->soundIndex = index;

        // Insert in mapping info
        this->soundIdsMapping.Add( soundId, &newInfo );

        // Insert sound info in library
        this->soundLibrary.Insert( index, soundInfo );

    }
    return index;
}

//------------------------------------------------------------------------------
/**
    @brief Set sound params
    @param soundId Unique string identifier for the sound
    @param mobile Flag that tells if the sound should track entity position
    @param min Distance at wich sound begins to attenuate
    @param maxDist Distance at wich sound is fully attenuated
    @param volume Volume setting for the sound
   
*/
void
nSoundLibrary::SetSoundParams( const char* soundId, bool mobile, float minDist, float maxDist, float volume )
{
    n_assert( minDist >= 0.0f && maxDist > 0.0f );
    SoundIdTableInfo* info = this->soundIdsMapping[ soundId ];
    if ( ! info )
    {
        NLOG(resource, (NLOGUSER, "Error: Called SetSoundParams for non-existent sound ('%s')", soundId ));
        return;
    }
    SoundInfo* soundInfo = 0;

    int sIndex = 0;
    while ( sIndex < this->soundLibrary.Size() && this->soundLibrary[sIndex].soundId != soundId )
    {
        sIndex ++;
    }

    if ( sIndex < this->soundLibrary.Size() )
    {
        soundInfo = &this->soundLibrary[ sIndex ];
    }
    else
    {
        NLOG(resource, (NLOGUSER, "Error: Called SetSoundParams for non-existent sound ('%s')", soundId ));
        return;
    }

    n_assert( soundInfo );
    soundInfo->minDist = minDist;
    soundInfo->maxDist = maxDist;
    soundInfo->volume = volume;
    soundInfo->mobile = mobile;

}

//------------------------------------------------------------------------------
/**
    @brief Set sound timing parameters
    @param soundId Unique string identifier for the sound
    @param delay Delay from event to playback of the sound
    @param pitchVar Relative random variation of pitch. e.g. 0.1 = +-5%
    @param volVar Relative random variation of volume e.g. 0.1 = +-5%
*/
void
nSoundLibrary::SetSoundTimeParams( const char* soundId, float delay, float pitchVar, float volVar )
{
    n_assert( delay >= 0.0f && pitchVar >= 0.0f && volVar >= 0.0f );

    SoundIdTableInfo* info = this->soundIdsMapping[ soundId ];
    if ( ! info )
    {
        NLOG(resource, (NLOGUSER, "Error: Called SetSoundParams for non-existent sound ('%s')", soundId ));
        return;
    }

    SoundInfo* soundInfo = 0;

    int sIndex = 0;
    while ( sIndex < this->soundLibrary.Size() && this->soundLibrary[sIndex].soundId != soundId )
    {
        sIndex ++;
    }

    if ( sIndex < this->soundLibrary.Size() )
    {
        soundInfo = &this->soundLibrary[ sIndex ];
    }
    else
    {
        NLOG(resource, (NLOGUSER, "Error: Called SetSoundParams for non-existent sound ('%s')", soundId ));
        return;
    }

    n_assert( soundInfo );
    soundInfo->delay = delay;
    soundInfo->pitchVar = pitchVar;
    soundInfo->volVar = volVar;
}

//------------------------------------------------------------------------------
/**
    @brief Set sound params
    @param soundId Unique string identifier for the sound
    @param varNumber Variation index
    @param mobile Flag that tells if the sound should track entity position
    @param min Distance at wich sound begins to attenuate
    @param maxDist Distance at wich sound is fully attenuated
    @param volume Volume setting for the sound
   
*/
void
nSoundLibrary::SetSoundParams2( const char* soundId, int varNumber, bool mobile, float minDist, float maxDist, float volume )
{
    n_assert( minDist >= 0.0f && maxDist > 0.0f );
    SoundIdTableInfo* info = this->soundIdsMapping[ soundId ];
    if ( ! info )
    {
        NLOG(resource, (NLOGUSER, "Error: Called SetSoundParams for non-existent sound ('%s')", soundId ));
        return;
    }
    SoundInfo* soundInfo = 0;

    int sIndex = 0;
    while ( sIndex < this->soundLibrary.Size() && this->soundLibrary[sIndex].soundId != soundId )
    {
        sIndex ++;
    }

    int i = varNumber;
    while ( sIndex < this->soundLibrary.Size() && i > 0 )
    {
        sIndex ++;
        i --;
    }

    if ( sIndex < this->soundLibrary.Size() )
    {
        soundInfo = &this->soundLibrary[ sIndex ];
    }
    else
    {
        NLOG(resource, (NLOGUSER, "Error: Called SetSoundParams for non-existent sound ('%s')", soundId ));
        return;
    }

    n_assert( soundInfo );
    soundInfo->minDist = minDist;
    soundInfo->maxDist = maxDist;
    soundInfo->volume = volume;
    soundInfo->mobile = mobile;
}

//------------------------------------------------------------------------------
/**
    @brief Set sound timing parameters
    @param soundId Unique string identifier for the sound
    @param varNumber Variation index
    @param delay Delay from event to playback of the sound
    @param pitchVar Relative random variation of pitch. e.g. 0.1 = +-5%
    @param volVar Relative random variation of volume e.g. 0.1 = +-5%
*/
void
nSoundLibrary::SetSoundTimeParams2( const char* soundId, int varNumber, float delay, float pitchVar, float volVar )
{
    n_assert( delay >= 0.0f && pitchVar >= 0.0f && volVar >= 0.0f );

    SoundIdTableInfo* info = this->soundIdsMapping[ soundId ];
    if ( ! info )
    {
        NLOG(resource, (NLOGUSER, "Error: Called SetSoundParams for non-existent sound ('%s')", soundId ));
        return;
    }

    SoundInfo* soundInfo = 0;

    int sIndex = 0;
    while ( sIndex < this->soundLibrary.Size() && this->soundLibrary[sIndex].soundId != soundId )
    {
        sIndex ++;
    }

    int i = varNumber;
    while ( sIndex < this->soundLibrary.Size() && i > 0 )
    {
        sIndex ++;
        i --;
    }

    if ( sIndex < this->soundLibrary.Size() )
    {
        soundInfo = &this->soundLibrary[ sIndex ];
    }
    else
    {
        NLOG(resource, (NLOGUSER, "Error: Called SetSoundParams for non-existent sound ('%s')", soundId ));
        return;
    }

    n_assert( soundInfo );
    soundInfo->delay = delay;
    soundInfo->pitchVar = pitchVar;
    soundInfo->volVar = volVar;
}

//------------------------------------------------------------------------------
/**
    @brief Recalculate sound indexes stored in the mapping table
*/
void
nSoundLibrary::RecalculateIndexes()
{
    this->soundIdsMapping.Begin();
    nString key;
    SoundIdTableInfo* info;
    this->soundIdsMapping.Next(key, info);
    while ( info )
    {
        int i = 0;
        while ( i < this->soundLibrary.Size() && this->soundLibrary[ i ].soundId != key )
        {
            i++;
        }
        n_assert( i < this->soundLibrary.Size() );

        info->soundIndex = i;

        this->soundIdsMapping.Next(key, info);
    }
}

//------------------------------------------------------------------------------
/**
    @brief Calculate parameters relative to variations of each sound
    Variations are consecutive rows in the library wich have the same id. The rest of parameters can vary to do the variations.
    Also, maxDistance must be the same in all sound variations.
*/
void
nSoundLibrary::CalculateVariationsParameters()
{
    this->soundIdsMapping.Begin();
    SoundIdTableInfo* info = this->soundIdsMapping.Next();
    while ( info )
    {
        int numVariations = 1;
        int i = info->soundIndex + 1;
        while ( i < this->soundLibrary.Size() && this->soundLibrary[ i ].soundId == this->soundLibrary[ info->soundIndex ].soundId )
        {
            numVariations++;
            i++;
        }

        i = info->soundIndex;
        float maxD = this->soundLibrary[ i ].maxDist;
        while ( i < this->soundLibrary.Size() && this->soundLibrary[ i ].soundId == this->soundLibrary[ info->soundIndex ].soundId )
        {
            if ( i == info->soundIndex )
            {
                this->soundLibrary[ i ].numberOfVariations = numVariations;
            }
            else
            {
                this->soundLibrary[ i ].numberOfVariations = 0;
            }

#ifndef NGAME
            this->soundLibrary[ i ].variationNumber = i - info->soundIndex;
#endif

            this->soundLibrary[ i ].maxDist = maxD;
            i++;
        }

        info = this->soundIdsMapping.Next();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get sound start index
    @param soundId Sound string id
    @param index Returned sound index, InvalidSoundIndex if not found
*/
int
nSoundLibrary::GetSoundIndex( const char* soundId ) const
{
    SoundIdTableInfo* info = this->soundIdsMapping[ soundId ];
    if ( ! info )
    {
        return InvalidSoundIndex;
    }
    return info->soundIndex;
}


//------------------------------------------------------------------------------
/**
    @brief Get sound number of variations
    @param soundId Sound string id
    @return Number of variations of the sound
*/
int
nSoundLibrary::GetSoundNumberOfVariations( int index ) const
{
    n_assert( index >= 0 && index <= this->soundLibrary.Size() );
    return this->soundLibrary[ index ].numberOfVariations;
}

//------------------------------------------------------------------------------
/**
    @brief Get number of different sounds in the library array (each variation is a different sound)
    return The number of sounds
*/
int
nSoundLibrary::GetNumberOfSounds() const
{
    return this->soundLibrary.Size();
}

/// @todo mateu.batle removed NGAME for demo urgency
//#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @brief Get max number of free playing sounds
    @return The max number of free playing sounds
*/
int
nSoundLibrary::GetMaxFreeSounds() const
{
    return this->maxFreeSounds;
}
//#endif // NGAME


#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @brief Set max number of free playing sounds
    @param The max number of free playing sounds
*/
void
nSoundLibrary::SetMaxFreeSounds(int maxFreeSounds)
{
    this->maxFreeSounds = maxFreeSounds;
}
#endif 

//------------------------------------------------------------------------------
/**
    @brief Get info for a sound by index
    @param index Global index of sound
    @return Sound info
*/
nSoundLibrary::SoundInfo&
nSoundLibrary::GetSoundInfo( int index )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );
    return this->soundLibrary[ index ];
}

//------------------------------------------------------------------------------
/**
    Get info for a sound by index, for read only
*/
const nSoundLibrary::SoundInfo&
nSoundLibrary::GetSoundInfo( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->soundLibrary[ index ];
}

//------------------------------------------------------------------------------
/**
    Add a new sound with default parameters
*/
int
nSoundLibrary::AddDefaultSound( const char* soundId )
{
    return this->AddDefaultSoundWithResource( soundId, "" );
}

//------------------------------------------------------------------------------
/**
    Add a new sound with the soundId and resource given, and other values as for default
*/
int
nSoundLibrary::AddDefaultSoundWithResource( const char* soundId, const char* soundRsrc )
{
    return this->NewSound( soundId, soundRsrc, false, false, 1, 10 );
}

//------------------------------------------------------------------------------
/**
    Remove a sound by index
*/
bool
nSoundLibrary::RemoveSound( int index )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

#ifndef NGAME
    if ( this->playingSoundsCounter )
    {
        NLOG(resource, (NLOGUSER, "Can't remove the sound because there are sounds playing. Please stop all sounds before editing the library" ));
        return false;
    }
#endif

    this->RemoveSoundRowsInClass( "nentityobject", this->soundLibrary[ index ].soundId.Get() );

    this->soundIdsMapping.Remove( this->soundLibrary[ index ].soundId );

    this->soundLibrary.Erase( index );

    this->RecalculateIndexes();

    this->CalculateVariationsParameters();


    return true;
}

//------------------------------------------------------------------------------
/**
    @brief Remove all rows related to the sound with a given soundId, recursively in all classes starting at 'className'
    @param className Root class to start removing sound event rows
    @param soundId the sound to remove from all class tables
*/
void
nSoundLibrary::RemoveSoundRowsInClass( const char* className,const char* soundId ) const
{
    nEntityClassServer* entServer = nEntityClassServer::Instance();
    nArray<nString> subclasses;
    entServer->GetEntitySubClassNames( className, subclasses);
    for (int i = 0; i < subclasses.Size(); i++ )
    {
        nEntityClass* entClass = entServer->GetEntityClass( subclasses[ i ].Get() );
        n_assert( entClass );
        ncSoundClass* soundClassComp = entClass->GetComponent<ncSoundClass>();
        if ( soundClassComp )
        {
            nSoundEventTable* eventTable = soundClassComp->GetSoundTable();
            if ( eventTable )
            {
                eventTable->RemoveSoundRows( soundId );
            }
        }
        this->RemoveSoundRowsInClass( subclasses[ i ].Get(), soundId );
    }
}

//------------------------------------------------------------------------------
/**
    Get the sound id
*/
nString
nSoundLibrary::GetSoundId( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).soundId;
}

//------------------------------------------------------------------------------
/**
    Get the resource
*/
nString
nSoundLibrary::GetResource( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).resourceName;
}

//------------------------------------------------------------------------------
/**
    Set the resource
*/
void
nSoundLibrary::SetResource( int index, const char* resourceName )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

#ifndef NGAME
    if ( this->playingSoundsCounter )
    {
        NLOG(resource, (NLOGUSER, "Can'change the sound file because there are sounds playing. Please stop all sounds before changing sound files" ));
        return;
    }
#endif

    this->GetSoundInfo( index ).resourceName = resourceName;

#ifndef NGAME
    nSoundScheduler::Instance()->ReloadSound( index );
#endif
}

//------------------------------------------------------------------------------
/**
    Get the 2d flag
*/
bool
nSoundLibrary::Get2dFlag( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).is2d;
}

//------------------------------------------------------------------------------
/**
    Set the 2d flag
*/
void
nSoundLibrary::Set2dFlag( int index, bool is2d )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

#ifndef NGAME
    if ( this->playingSoundsCounter )
    {
        n_message( EditStringError );
        return;
    }
#endif

    if ( this->GetSoundInfo( index ).is2d != is2d )
    {
        this->GetSoundInfo( index ).is2d = is2d;

#ifndef NGAME
        nSoundScheduler::Instance()->ReloadSound( index );
#endif
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Get 'new sound' flag
*/
bool
nSoundLibrary::IsNewSound( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).isNew;
}

//------------------------------------------------------------------------------
/**
    Set 'new sound' flag
*/
void
nSoundLibrary::SetNewSound(int index, bool isNew )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    SoundInfo & info = this->GetSoundInfo( index );
    info.isNew = isNew;
    if ( isNew )
    {
        this->SetDirty( true );
        this->SignalSoundAddedToLibrary( this, info.soundId.Get(), info.resourceName.Get() );
    }
}

//------------------------------------------------------------------------------
/**
    Set 'mute' flag
*/
void
nSoundLibrary::SetMuteFlag(int index, bool mute )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).mute = mute;
}

//------------------------------------------------------------------------------
/**
    Get 'mute' flag
*/
bool
nSoundLibrary::GetMuteFlag(int index) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).mute;
}

//------------------------------------------------------------------------------
/**
    Set 'solo' flag
*/
void
nSoundLibrary::SetSoloFlag(int index, bool solo )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    SoundInfo & info = this->GetSoundInfo( index );
    
    info.solo = solo;

    if ( solo )
    {
        info.silenced = false;
        for ( int i = 0; i < this->soundLibrary.Size(); i++ )
        {
            SoundInfo & infoOther = this->GetSoundInfo( i );
            if ( !infoOther.solo )
            {
                infoOther.silenced = true;
            }
        }
    }
    else
    {
        int numSolos = 0;
        for ( int i = 0; i < this->soundLibrary.Size(); i++ )
        {
            SoundInfo & infoOther = this->GetSoundInfo( i );
            if ( infoOther.solo )
            {
                numSolos ++;
            }
        }
        if ( numSolos == 0 )
        {
            for ( int i = 0; i < this->soundLibrary.Size(); i++ )
            {
                SoundInfo & infoOther = this->GetSoundInfo( i );
                infoOther.silenced = false;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
    Reset all mute/solo flags
*/
void
nSoundLibrary::ResetAllMuteSolo()
{
    for ( int i = 0; i < this->soundLibrary.Size(); i++ )
    {
        SoundInfo & infoOther = this->GetSoundInfo( i );
        infoOther.mute = false;
        infoOther.solo = false;
        infoOther.silenced = false;
    }
}
//------------------------------------------------------------------------------
/**
    Get 'solo' flag
*/
bool
nSoundLibrary::GetSoloFlag(int index) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).solo;
}

#endif

//------------------------------------------------------------------------------
/**
    Get the minimum distance
*/
float
nSoundLibrary::GetMinDist( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).minDist;
}

//------------------------------------------------------------------------------
/**
    Set the minimum distance
*/
void
nSoundLibrary::SetMinDist( int index, float minDist )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).minDist = minDist;
}

//------------------------------------------------------------------------------
/**
    Get the maximum distance
*/
float
nSoundLibrary::GetMaxDist( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).maxDist;
}

//------------------------------------------------------------------------------
/**
    Set the maximum distance
*/
void
nSoundLibrary::SetMaxDist( int index, float maxDist )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).maxDist = maxDist;
}

//------------------------------------------------------------------------------
/**
    Get the volume setting
*/
float
nSoundLibrary::GetVolume( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return ( this->GetSoundInfo( index ).volume ) * 100.0f;
}

//------------------------------------------------------------------------------
/**
    Set the volume setting
*/
void
nSoundLibrary::SetVolume( int index, float volume )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).volume = volume / 100.0f;
}

//------------------------------------------------------------------------------
/**
    Get the pitch variation
*/
float
nSoundLibrary::GetPitchVar( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).pitchVar;
}

//------------------------------------------------------------------------------
/**
    Set the pitch variation
*/
void
nSoundLibrary::SetPitchVar( int index, float pitchVar )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).pitchVar = pitchVar;
}

//------------------------------------------------------------------------------
/**
    Get the volume variation
*/
float
nSoundLibrary::GetVolumeVar( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).volVar;
}

//------------------------------------------------------------------------------
/**
    Set the volume variation
*/
void
nSoundLibrary::SetVolumeVar( int index, float volVar )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).volVar = volVar;
}

//------------------------------------------------------------------------------
/**
    Get the mobile flag value
*/
bool
nSoundLibrary::GetMobile( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).mobile;
}

//------------------------------------------------------------------------------
/**
    Set the mobile flag value
*/
void
nSoundLibrary::SetMobile( int index, bool mobile )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).mobile = mobile;
}

//------------------------------------------------------------------------------
/**
    Get the pitch control flag value
*/
bool
nSoundLibrary::GetPitchControl( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).pitchControl;
}

//------------------------------------------------------------------------------
/**
    Set the pitch control flag value
*/
void
nSoundLibrary::SetPitchControl( int index, bool pitchControl )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

#ifndef NGAME
    if ( this->playingSoundsCounter )
    {
        n_message( EditStringError );
        return;
    }
#endif

    this->GetSoundInfo( index ).pitchControl = pitchControl;

#ifndef NGAME
    nSoundScheduler::Instance()->ReloadSound( index );
#endif

}

//------------------------------------------------------------------------------
/**
    @brief Set the pitch control flag value
    @param soundId Unique string identifier for the sound
    @param pitchControl pitch control flag
   
*/
void
nSoundLibrary::SetPitchControlById( const char* soundId, bool pitchControl )
{
#ifndef NGAME
    if ( this->playingSoundsCounter )
    {
        n_message( EditStringError );
        return;
    }
#endif

    SoundIdTableInfo* info = this->soundIdsMapping[ soundId ];
    if ( ! info )
    {
        NLOG(resource, (NLOGUSER, "Error: Called SetPitchControlById for non-existent sound ('%s')", soundId ));
        return;
    }
    SoundInfo* soundInfo = &this->soundLibrary[ info->soundIndex ];
    n_assert( soundInfo );
    soundInfo->pitchControl = pitchControl;

#ifndef NGAME
    if ( nSoundScheduler::Instance()->GetSoundLibrary() )
    {
        nSoundScheduler::Instance()->ReloadSound( info->soundIndex );
    }
#endif

}

//------------------------------------------------------------------------------
/**
    Get the maximum number of instances
*/
int
nSoundLibrary::GetMaxInstances( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).maxSoundInstances;
}

//------------------------------------------------------------------------------
/**
    Set the maximum number of instances
*/
void
nSoundLibrary::SetMaxInstances( int index, int maxSoundInstances )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).maxSoundInstances = maxSoundInstances;

#ifndef NGAME
    nSoundScheduler::Instance()->ReloadSound( index );
#endif
}

//------------------------------------------------------------------------------
/**
    Get the streamed flag value
*/
bool
nSoundLibrary::GetStreamed( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).streamed;
}

//------------------------------------------------------------------------------
/**
    Set the streamed flag value
*/
void
nSoundLibrary::SetStreamed( int index, bool streamed )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).streamed = streamed;
}

//------------------------------------------------------------------------------
/**
    Get the priority
*/
int
nSoundLibrary::GetPriority( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).priority;
}

//------------------------------------------------------------------------------
/**
    Set the priority
*/
void
nSoundLibrary::SetPriority( int index, int priority )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).priority = priority;
}

//------------------------------------------------------------------------------
/**
    Get the delay
*/
float
nSoundLibrary::GetDelay( int index ) const
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    return this->GetSoundInfo( index ).delay;
}

//------------------------------------------------------------------------------
/**
    Set the delay
*/
void
nSoundLibrary::SetDelay( int index, float delayVar )
{
    n_assert( index >= 0 && index < this->soundLibrary.Size() );

    this->GetSoundInfo( index ).delay = delayVar;
}

//------------------------------------------------------------------------------
/**
    @brief Play a standalone sound
    @param soundIndex Index of sound in the library
    The sound is played in the camera position
*/
void
nSoundLibrary::Play( int soundIndex )
{
    // Get camera position
    nListener3* listener = nApplication::Instance()->GetAudioListener();
    const matrix44 transf = listener->GetTransform();

    // Play the sound in the scheduler. Negative index means that no variation is calculated, the index is already a variation
    nSoundScheduler::Instance()->Play( transf.pos_component(), soundIndex, 1, 0.0f, false );
}

//------------------------------------------------------------------------------
/**
    @brief Play a standalone sound
    @param soundIndex Index of sound in the library
    @param num number of repeats
    The sound is played in the camera position
*/
void
nSoundLibrary::PlayRepeat( int soundIndex, int num )
{
    // Get camera position
    nListener3* listener = nApplication::Instance()->GetAudioListener();
    const matrix44 transf = listener->GetTransform();

    // Play the sound in the scheduler. Negative index means that no variation is calculated, the index is already a variation
    nSoundScheduler::Instance()->Play( transf.pos_component(), soundIndex, num, 0.0f, false );
}

//------------------------------------------------------------------------------
/**
    @brief Stop all sounds
*/
void
nSoundLibrary::StopAllSounds()
{
    nSoundScheduler::Instance()->StopAllSounds();
}

//------------------------------------------------------------------------------
/**
    Get the dirty flag value
*/
bool
nSoundLibrary::GetDirty() const
{
    return this->isDirty;
}

//------------------------------------------------------------------------------
/**
    Set the dirty flag value
*/
void
nSoundLibrary::SetDirty( bool dirty )
{
    this->isDirty = dirty;
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    Increment playing sound counter
*/
void
nSoundLibrary::IncPlayingCount()
{
    this->playingSoundsCounter++;
}

//------------------------------------------------------------------------------
/**
    Decrement playing sound counter
*/
void
nSoundLibrary::DecPlayingCount()
{
    this->playingSoundsCounter--;
}

//------------------------------------------------------------------------------
/**
    Get playing sound counter
*/
int
nSoundLibrary::GetPlayingCount()
{
    return this->playingSoundsCounter;
}

//------------------------------------------------------------------------------
/**
*/
int
nSoundLibrary::GetNumberOfSoundGroups() const
{
    return this->soundEventGroups.Size();
}

//------------------------------------------------------------------------------
/**
*/
nString
nSoundLibrary::GetSoundGroupName(int index) const
{
    n_assert( index >=0 && index < this->soundEventGroups.Size() );
    return this->soundEventGroups[index];
}

//------------------------------------------------------------------------------
/**
*/
void
nSoundLibrary::AddSoundGroupName(const char * groupName, const char * eventName, const char * materialName, const char * soundId)
{
    n_assert( groupName && *groupName );
    n_assert( eventName && *eventName );
    n_assert( materialName );

    n_assert( soundId );

    int groupIndex = this->soundEventGroups.FindIndex( nString(groupName) );
    if ( groupIndex == InvalidSoundIndex )
    {
        groupIndex = this->soundEventGroups.Size();
        this->soundEventGroups.Append( groupName );
    }

    int soundIndex = this->GetSoundIndex( soundId );
    if ( soundIndex != InvalidSoundIndex )
    {
        SoundInfo & info = this->GetSoundInfo( soundIndex );
        info.eventName = eventName;
        info.materialName = materialName;
        info.soundGroupIndex = groupIndex;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
nSoundLibrary::ImportSoundGroup( nSoundEventTable * table, const char * groupName )
{
    n_assert( groupName && table);
    int groupIndex = this->soundEventGroups.FindIndex( nString(groupName) );
    if ( groupIndex == InvalidSoundIndex )
    {
        return false;
    }

    for ( int soundIndex = 0; soundIndex  < this->soundLibrary.Size(); soundIndex++)
    {
        SoundInfo & soundInfo = this->GetSoundInfo(soundIndex);
        if ( soundInfo.soundGroupIndex == groupIndex )
        {
            table->AddSoundEvent( soundInfo.eventName.Get(), soundInfo.materialName.Get(), soundInfo.soundId.Get() );
            NLOG( soundLog, (0, "Imported sound event: sound = '%s', group name = '%s', event = '%s', material = '%s'", soundInfo.soundId, groupName, soundInfo.eventName.Get(), soundInfo.materialName.Get() ) );
        }
    }
    return true;
}

#endif

//------------------------------------------------------------------------------
/**
    @brief get sound scheduler
*/
nObject *
nSoundLibrary::GetSoundScheduler()
{
    nObject * scheduler = static_cast<nObject*>( nSoundScheduler::Instance() );
    n_assert( scheduler );
    return scheduler;
}

//------------------------------------------------------------------------------
/**
    @brief set magic number (machine id)
    @param magic The magic number
*/
void
nSoundLibrary::SetMagicNumber(unsigned int magic)
{
    this->magicNumber = magic;
}
//------------------------------------------------------------------------------
