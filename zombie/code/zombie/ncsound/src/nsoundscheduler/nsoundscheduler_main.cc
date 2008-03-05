#include "precompiled/pchncsound.h"
//------------------------------------------------------------------------------
//  nsoundscheduler_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "stdlib.h"
#include "nsoundscheduler/nsoundscheduler.h"
#include "nsoundlibrary/nsoundlibrary.h"
#include "ncsound/ncsound.h"
#include "audio3/nsound3.h"
#include "audio3/naudioserver3.h"
#include "napplication/napplication.h"
#include "napplication/nappviewport.h"
#include "audio3/nlistener3.h"
#include "zombieentity/nctransform.h"
#include "entity/nentityclassserver.h"
#include "kernel/nfileserver2.h"
#include "kernel/ndirectory.h"
#include "ncsound/ncsoundclass.h"
#include "nsoundeventtable/nsoundeventtable.h"
#include "ntrigger/ncareatrigger.h"
#include "ntrigger/nctriggershape.h"
#ifndef __ZOMBIE_EXPORTER__
#include "ncgameplayliving/ncgameplayliving.h"
#endif
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nSoundScheduler, "nroot");

//------------------------------------------------------------------------------
NCREATELOGLEVEL ( soundLog, "Sound", false, 0 )

//------------------------------------------------------------------------------
nSoundScheduler* nSoundScheduler::singleton = 0;

const char* soundLibraryPath = "wc:/export/sounds/library.n2";
const char* soundLibraryNOHPath = "/usr/soundlibrary";

const float fadeVolumeVelocity = 0.03f;

//------------------------------------------------------------------------------
/**
    @brief constructor
*/
nSoundScheduler::nSoundScheduler() :
    refAudioServer("/sys/servers/audio")
{
    singleton = this;
}

//------------------------------------------------------------------------------
/**
    @brief destructor
*/
nSoundScheduler::~nSoundScheduler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Get singleton
*/
nSoundScheduler*
nSoundScheduler::Instance()
{
    return singleton;
}

//------------------------------------------------------------------------------
/**
    @brief Get the sound library
    @return The sound library, if loaded. Fails otherwise.
*/
nSoundLibrary*
nSoundScheduler::GetSoundLibrary()
{
    if ( this->refSoundLibrary.isvalid() )
    {
        return this->refSoundLibrary.get();
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Initialize sound scheduler
    Must be called after all sound class data is loaded.
*/
void
nSoundScheduler::Init()
{
    n_assert( this->refSoundLibrary.isvalid() );

    // recalculate sound indexes and variation parameters in the library
    this->refSoundLibrary->RecalculateIndexes();
    this->refSoundLibrary->CalculateVariationsParameters();

    // Calc. max sound instances to size playing sounds array
    int totalMaxSoundInstances = 0;
    int n = this->refSoundLibrary->GetNumberOfSounds();

    for (int index = 0; index < n; index++ )
    {
        // Get sound info
        nSoundLibrary::SoundInfo& info = this->refSoundLibrary->GetSoundInfo( index );

        totalMaxSoundInstances += info.maxSoundInstances;
    }

    /// @todo mateu.batle removed NGAME for demo urgency
//#ifndef NGAME
    // Size the array of free sounds (not associated to entities)
    int maxFreeSounds = this->refSoundLibrary->GetMaxFreeSounds();
    this->freeSoundsPool.Reallocate( maxFreeSounds, 16 );
    this->freeSoundsPool.At( this->freeSoundsPool.AllocSize() );

    // Size the array of playing sounds references
    this->playingSounds.Reallocate( totalMaxSoundInstances + maxFreeSounds, 0 );
//#else
    //this->playingSounds.Reallocate( totalMaxSoundInstances , 0 );
//#endif

    // Grow the runtimeInfo array with null poniters if sounds hasn't been loaded
    if ( this->runtimeInfo.Size() == 0 )
    {
        for ( int i = 0; i < this->refSoundLibrary->GetNumberOfSounds(); i++ )
        {
            this->runtimeInfo.Append( 0 );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Clears all info and unload sound library

    No sound can be playing while calling this function
*/
void 
nSoundScheduler::Reset()
{
    if ( ! this->refSoundLibrary.isvalid() )
    {
        return;
    }

    // Delete runtime info
    int n = this->runtimeInfo.Size();
    for (int index = 0; index < n; index++ )
    {
        nSound3* sound = runtimeInfo[ index ];
        sound->Release();

        /*
        @todo commented code before trying one nSound3 for all entities
        RuntimeSoundInfo& rtInfo = this->runtimeInfo.At( index );
        for ( int sinst = 0; sinst < this->runtimeInfo.Size(); sinst++ )
        {
            SoundSlot& slot = rtInfo[ sinst ];
            n_assert( slot.sound );
            slot.sound->Release();
        }
        rtInfo.Reallocate(0, 0);*/
    }
    runtimeInfo.Reallocate(0, 0);

    // Unload library
    this->refSoundLibrary->Release();
}

//------------------------------------------------------------------------------
/**
    @brief Set the sound library path and load it
    
    No sound can be playing while calling this function
*/
void 
nSoundScheduler::LoadSoundLibrary(const char* path)
{
    this->Reset();

    if ( nFileServer2::Instance()->FileExists( path ) )
    {
        // Load the library info
        this->refSoundLibrary = static_cast<nSoundLibrary*>( nKernelServer::Instance()->LoadAs( path, soundLibraryNOHPath, true ) );
    }
    else
    {
        // Just to allow testing in temp wc
        this->refSoundLibrary = static_cast<nSoundLibrary*>( nKernelServer::Instance()->New( "nsoundlibrary", path ) );
    }
    n_assert( this->refSoundLibrary.isvalid() );
}


//------------------------------------------------------------------------------
/**
    @brief Load all samples contained in the library
    

    No sound can be playing while calling this function
*/
void 
nSoundScheduler::LoadAllSamples( bool scanNewFiles )
{

    n_assert( this->refSoundLibrary.isvalid() );

#ifndef NGAME
    if ( this->refSoundLibrary->GetPlayingCount() )
    {
        n_message( "Can't add sounds because there are sounds playing. Please stop all sounds before editing the library" );
        return;
    }
#endif

    // Unload all samples
    for ( int i = 0; i < this->refSoundLibrary->GetNumberOfSounds(); i++ )
    {
        this->UnloadSound( i );
    }

    if ( scanNewFiles )
    {
#ifndef NGAME

        // Scan for new sound files and generate library and table info when possible
        nString searchPath("wc:export/sounds");
        this->ScanNewSounds( searchPath );
#endif
    }


    // Update runtime info
    int n = this->refSoundLibrary->GetNumberOfSounds();
    for (int index = 0; index < n; index++ )
    {
        // Load the sound
        bool loaded = this->LoadSound( index );

        // Get sound info
        nSoundLibrary::SoundInfo& info = this->refSoundLibrary->GetSoundInfo( index );

        if ( !loaded )
        {
            this->refSoundLibrary->SignalFailedLoadingSound( this->refSoundLibrary.get(), info.soundId.Get(), info.resourceName.Get() );
        }
    }

    /*
        @todo commented code before trying one nSound3 for all entities

        RuntimeSoundInfo& rtInfo = this->runtimeInfo.At( index );
        nSoundLibrary::SoundInfo& info = this->refSoundLibrary->GetSoundInfo( index );

        int maxInstances = info.maxSoundInstances;
        if (true) //( info.mobile )
        {
            // If the sound is mobile, make 'maxInstances' different nSound3 instances with only one track each
            rtInfo.SetFixedSize( maxInstances );
            totalMaxSoundInstances += maxInstances;
            for ( int sinst = 0; sinst < maxInstances; sinst++ )
            {
                // See note at end of file
                SoundSlot& slot = rtInfo[ sinst ];
                slot.sound = this->refAudioServer.get()->NewSound();
                n_assert( slot.sound );
                slot.sound->SetFilename( info.resourceName );
                slot.sound->SetLooping( info.repeat != 1 );
                slot.sound->SetAmbient( false );
                slot.sound->SetNumTracks( 1 );
                slot.sound->SetStreaming( info.streamed );
                bool b = slot.sound->Load();
                if ( ! b )
                {
                    n_message( "Couldn't load sound '%s'");
                    slot.sound = 0;
                }

                slot.flags = 0;
                slot.startTime = 0.0f;
                slot.endTime = 0.0f;
                slot.soundComp = 0;
                slot.index = index;
            }
        }
        else
        {
            // If the sound is not mobile, create only one nSound3 with 'maxInstances' num of tracks
            rtInfo.SetFixedSize( 1 );
            SoundSlot& slot = rtInfo[ 0 ];
            slot.sound = this->refAudioServer.get()->NewSound();
            slot.sound->SetFilename( info.resourceName );
            slot.sound->SetLooping( info.repeat != 1 );
            slot.sound->SetAmbient( false );
            slot.sound->SetNumTracks( maxInstances );
            slot.sound->SetStreaming( info.streamed );
            bool b = slot.sound->Load();
            if ( ! b )
            {
                n_message( "Couldn't load sound '%s'");
                slot.sound = 0;
            }

            slot.flags = 0;
            slot.startTime = 0.0f;
            slot.endTime = 0.0f;
            slot.soundComp = 0;
            slot.index = index;
        }
    */
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @brief Scan for new sound files and generate library and table info when possible

    Sound files naming rules

    * Separator is '_'
    * Files Under /sources directory: Each new file creates a new entity subclass of SoundSource
    * Files begginning with "ENV" are 2d sounds
*/
void 
nSoundScheduler::ScanNewSounds( nString & path, bool createSoundSources, bool recursiveSearch )
{
    if ( this->refSoundLibrary->GetPlayingCount() )
    {
        n_message( "Can't add sounds because there are sounds playing. Please stop all sounds before editing the library" );
        return;
    }

    nFileServer2* fileserv = nFileServer2::Instance();
    nEntityClassServer *entityClassServer = nEntityClassServer::Instance();
    n_assert( entityClassServer );

    n_assert( fileserv );
    nDirectory *d = fileserv->NewDirectoryObject();
    d->Open( path );
    n_assert( d->IsOpen() );
    if ( ! d->IsOpen() )
    {
        n_delete(d);
        return;
    }

    nString filePath;
    nString fileName;
    nString classOrGroup;
    nString className;
    nString materialName;
    nString eventName;
    nString soundString;
    nString soundId;
    nString wcPath = nFileServer2::Instance()->ManglePath("wc:");
    nArray<nString> tokens;

    bool moreFiles = d->SetToFirstEntry();
    bool firstFile = true;

    while ( moreFiles )
    {
        if ( ! firstFile )
        {
            moreFiles = d->SetToNextEntry();
        }
        firstFile = false;

        filePath = d->GetEntryName();
        filePath = filePath.Substitute( (wcPath + "/").Get(), "wc:" );

        fileName = filePath;
        fileName.StripExtension();
        fileName = fileName.ExtractFileName();

        bool createSources = createSoundSources || filePath.ExtractLastDirName() == "sources";

        if ( d->GetEntryType() == nDirectory::DIRECTORY )
        {

            if ( !recursiveSearch || strcmp( filePath.ExtractFileName().Get(), ".svn" ) == 0 )
            {
                continue;
            }
        
            
            this->ScanNewSounds( filePath, createSoundSources );

        }
        else if ( d->GetEntryType() == nDirectory::FILE )
        {

            if ( ! this->IsSoundFile( filePath.Get() ) )
            {
                continue;
            }

            bool isGroupName = false;

            // Construct sound id
            if ( createSources )
            {
                soundId = fileName;
                className = fileName;
            }
            else
            {
                // Process file name string to obtain class, event, material and variation
                soundString = fileName;

                tokens.Clear();
                nString token;
                int tokStart = 0;
                int tokEnd = 0;
                while ( tokStart < soundString.Length() )
                {
                    if ( soundString[tokStart] != '_' )
                    {
                        tokEnd = tokStart + 1;
                        while ( tokEnd < soundString.Length() && soundString[tokEnd] != '_' )
                        {
                            tokEnd++;
                        }
                    }
                    else
                    {
                        tokEnd = tokStart;
                    }
                    token = soundString.ExtractRange(tokStart, tokEnd - tokStart);
                    if ( tokens.Size() < 6 )
                    {
                        tokens.Append( token );
                        tokStart = tokEnd + 1;
                    }
                    else
                    {
                        tokens.Append( soundString.ExtractRange(tokStart, soundString.Length() - tokStart ) );
                        tokStart = soundString.Length();
                    }
                }

                if ( tokens.Size() < 6 )
                {
                    continue;
                }

                classOrGroup = tokens[2];
                classOrGroup.ToUpper();
                isGroupName = classOrGroup[0] == 'G';
                className = tokens[3];
                eventName = tokens[4];
                materialName = tokens[5];
                soundId = tokens[0] + "_" + tokens[1] + "_" + classOrGroup + "_" + className + "_" + eventName;
                if ( ! materialName.IsEmpty() )
                {
                    soundId = soundId + "_" + materialName;
                }
            }

            // Make className correct
            className.ToLower();
            className.ConvertSpace();
            className[0] = char(toupper( className[0] ));

            // Make event name lower case
            eventName.ToLower();

            // Add file to library
            int soundIndex = this->refSoundLibrary->NewSound( soundId.Get(), filePath.Get(), false, false, 1, 10 );
            if ( soundIndex != InvalidSoundIndex )
            {
                this->refSoundLibrary->SetNewSound( soundIndex, true );
            }
            else
            {
                continue;
            }

            if ( ! isGroupName )
            {
                if ( createSources )
                {
                    nEntityClass * newClass = entityClassServer->GetEntityClass( className.Get() );
                    if ( ! newClass )
                    {
                        nEntityClass * soundSourceClass = entityClassServer->GetEntityClass( "nesoundsource" );
                        n_assert( soundSourceClass );

                        newClass = entityClassServer->NewEntityClass( soundSourceClass, className.Get() );
                        n_assert( newClass );
                    }

                    ncSoundClass * soundClassComp = newClass->GetComponentSafe<ncSoundClass>();
                    nSoundEventTable * soundTable = soundClassComp->SafeGetSoundTable();
                    n_assert( soundTable );
                    soundTable->AddSoundEvent( "trigger", "", soundId.Get() );
                    soundTable->SetObjectDirty( true );
                }
                else
                {
                    nEntityClass * newClass = entityClassServer->GetEntityClass( className.Get() );
                    if ( newClass )
                    {
                        ncSoundClass * soundClassComp = newClass->GetComponent<ncSoundClass>();
                        if ( soundClassComp )
                        {
                            nSoundEventTable * soundTable = soundClassComp->SafeGetSoundTable();
                            n_assert( soundTable );
                            if ( soundTable->GetSoundIndex( eventName.Get(), materialName.Get() ) == InvalidSoundIndex )
                            {
                                soundTable->AddSoundEvent( eventName.Get(), materialName.Get(), soundId.Get() );
                                nEntityClassServer::Instance()->SetEntityClassDirty( newClass, true );
                            }
                        }
                    }
                }
            }
            else
            {
                // Add group to library
                this->refSoundLibrary->AddSoundGroupName( className.Get(), eventName.Get(), materialName.Get(), soundId.Get() );
            }
        }
    }
    d->Close();
    n_delete(d);
}
#endif


//------------------------------------------------------------------------------
/**
    @brief tells if a file name is a sound resource
*/
bool
nSoundScheduler::IsSoundFile( const char * fileName )
{
    nString extStr = fileName;
    extStr = extStr.ExtractFileName();
    extStr.ToLower();
    const char* extension = extStr.GetExtension();
    if ( extension &&
         strcmp( extension, "wav" ) == 0 ||
         strcmp( extension, "ogg" ) == 0 )
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
nSoundScheduler::IsLoadedSound( int libraryIndex )
{
    return ( this->runtimeInfo[ libraryIndex ] != 0 );
}

//------------------------------------------------------------------------------
/**
    @brief Load a sound, taking the sound info from the library and creating the nSound3 object
    @param libraryIndex Sound index in the library

    No sound can be playing while calling this function
*/
bool
nSoundScheduler::LoadSound( int libraryIndex )
{
    n_assert( libraryIndex >= 0 && libraryIndex < this->refSoundLibrary->GetNumberOfSounds() );

    // Get sound info
    nSoundLibrary::SoundInfo& info = this->refSoundLibrary->GetSoundInfo( libraryIndex );

    if ( info.resourceName.IsEmpty() )
    {
        return false;
    }
    
    // Create sound
    nSound3* sound = this->refAudioServer.get()->NewSound();
    n_assert( sound );

    // Put sound main parameters
    sound->SetFilename( info.resourceName );
    sound->SetLooping( false );
    sound->SetFreqControl( info.pitchControl );

    sound->SetAmbient( info.is2d );
    sound->SetNumTracks( info.maxSoundInstances );
    sound->SetStreaming( info.streamed );

    bool loaded = sound->Load();
    if ( loaded )
    {
        // Put sound library parameters
        this->PutSoundParameters( sound, info );
    }
    else
    {
        // File exists?
        if ( nFileServer2::Instance()->FileExists( info.resourceName ) )
        {
            // Try to load as ambient sound
            sound->SetAmbient( ! info.is2d );
            loaded = sound->Load();
			if ( loaded )
			{
				// Update 2d flag
				info.is2d = ! info.is2d;
			}
        }
        if ( !loaded )
        {
            sound->Release();
            sound = 0;
        }
    }

    // Store sound pointer in runtime sound table
    runtimeInfo.At( libraryIndex );
    runtimeInfo[ libraryIndex ] = sound;

    return loaded;
}

//------------------------------------------------------------------------------
/**
    @brief Unload a sound, taking the sound info from the library and releasing the nSound3 object
    @param libraryIndex Sound index in the library

    No sound can be playing while calling this function
*/
bool
nSoundScheduler::UnloadSound( int libraryIndex )
{
    n_assert( libraryIndex >= 0 && libraryIndex < this->refSoundLibrary->GetNumberOfSounds() );

    if ( libraryIndex < this->runtimeInfo.Size() )
    {
        /*n_assert( this->runtimeInfo[ libraryIndex ]->GetSoundResource() );
        this->runtimeInfo[ libraryIndex ]->GetSoundResource()->Unload();*/

        if ( ! this->runtimeInfo[ libraryIndex ] )
        {
            return true;
        }

        this->runtimeInfo[ libraryIndex ]->Unload();

        this->runtimeInfo[ libraryIndex ]->Release();
        this->runtimeInfo[ libraryIndex ] = 0;

        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Reload a sound.
    @param libraryIndex Sound index in the library

    No sound can be playing while calling this function
*/
bool
nSoundScheduler::ReloadSound( int libraryIndex )
{
    n_assert( libraryIndex >= 0 && libraryIndex < this->refSoundLibrary->GetNumberOfSounds() );

    this->UnloadSound( libraryIndex );

    if ( ! this->LoadSound( libraryIndex ) )
    {
        return false;
    }

    return true;
}


//------------------------------------------------------------------------------
/**
    @brief Insert new sound
    @param libraryIndex Sound index in the library

    Used when adding a new sound during execution, i.e. from the editor. The sound is not loaded.
*/
bool
nSoundScheduler::InsertNewSound( int libraryIndex )
{
    // Insert runtime info only if sounds have been loaded
    if ( libraryIndex >= 0 && libraryIndex <= this->runtimeInfo.Size() )
    {
        this->runtimeInfo.Insert( libraryIndex, 0 );
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    @brief Process a playback query
    @param soundParams Sound playing info associated to the playback.
    @param soundIndex Sound index. If negative, no variation is calculated and the real index is ( -soundIndex - 1 )
    @param repeat # of repetitions (0=infinite loop)
    @param duration Limit of duration (seconds)
    @param calcVariation If true, sound is relaunched for each repetition, possibly with another variation
    @param volume Volume setting

    The 'duration' parameter is a limit to clamp the sample or clamp the repetition of a sample by time. If calcVariation is false,
    the sound is stopped when min('duration', repeat * 'sample duration') time has passed. If repeat is 0, i.e infinite looping, the time
    is simply 'duration'

    If calcVariation is true and the parameter 'duration' is greater than the sample duration, a silence is placed after the sample
    termination, to make it last exactly 'duration'. When the time 'duration' has passed since the beggining of the sample,
    the next 'repeat' is made.
    
    There is a exception: if repeat==1 and duration!=0, there must be no clamp-> endTime = startTime + soundDuration. This is to have
    the last footstep play entire.
*/
void
nSoundScheduler::PlayQuery( SoundComponentInfo* soundParams, int soundIndex, int repeat, nTime duration, bool calcVariation, float volume )
{
    // Check sound index in library range
    n_assert( soundIndex < this->runtimeInfo.Size() );
    if ( ! ( soundIndex < this->runtimeInfo.Size() ) )
    {
        return;
    }

    bool randomVariation = true;

    if ( soundIndex < 0 )
    {
        // If sound index parameter is negative, it means don't calculate a variation, the index is already the variation
        randomVariation = false;
        soundIndex = - soundIndex - 1;
    }

    // Get sound info from library
    nSoundLibrary::SoundInfo& soundInfoRef = this->refSoundLibrary->GetSoundInfo( soundIndex );
    nSoundLibrary::SoundInfo* soundInfo = & soundInfoRef;

    // Check max playing sounds
    if ( this->playingSounds.Size() >= this->playingSounds.AllocSize() )
    {
        NLOG( soundLog, (0, "PlayQuery: Discarded due max sound instances reached (%d). sound = '%s', ", this->playingSounds.Size(), soundInfo->soundId.Get() ) );
        return;
    }

    // Calculate a sound variation
    int storedSoundIndex = soundIndex;

    if ( randomVariation )
    {
        int variation = int( soundInfo->numberOfVariations  *  float( n_rand() ) );
        variation = max(0,min( variation, soundInfo->numberOfVariations - 1 ));
        soundIndex += variation;
        
        // Get variation sound info
        soundInfo = & this->refSoundLibrary->GetSoundInfo( soundIndex );
    }

#ifndef NGAME
    // Check mute / silenced flags
    if ( soundInfo->mute || soundInfo->silenced )
    {
        return;
    }
#endif

    // Get the sound
    nSound3* sound = this->runtimeInfo[ soundIndex ];
    if ( !sound )
    {
        if ( ! this->LoadSound( soundIndex ) )
        {
#ifdef NGAME
            n_error("Error playing sound: %s", soundInfo->resourceName);
#endif
            // Ignore playback
            return;
        }

        sound = this->runtimeInfo[ soundIndex ];
    }

    // Set queued state
    soundParams->flags = Queued;

    // Set sound
    soundParams->sound = sound;
    soundParams->soundIndex = storedSoundIndex;

    // Set initial volume, playing volume and current volume
    soundParams->initialVol = volume;
    soundParams->playVol = soundInfo->volume * volume;
    soundParams->currentVol = soundParams->playVol;

    // Set frequency
    soundParams->playFreq = 1.0f;

    // Calculate volume variation

    if ( soundInfo->volVar != 0.0f )
    {
        soundParams->playVol = max( 0.0f, soundParams->playVol - soundInfo->volVar * float( n_rand() ) );
    }

    // @todo JJ calculate and apply to the nSound3 the frequency shift.

    // @todo JJ: Test if starting sound from here rather from the scheduler works well (it should). Then it wouldn't be necessary to store
    // pitch, volume and property sets in the ncsound, they could be assigned to the nSound3 and then its Start method called, from here.

    // @todo: pitch variation (when done) should alter endTime

    nTime startTime = this->currentTime + soundInfo->delay;
    nTime endTime = 0.0f;

    float soundDuration = sound->GetDuration();

    // Calculate end time and fadeout flag

    // Sound plays a single variation, even if it's looped
    if ( repeat == 0 )
    {
        // Infinite looping sounds have endtime set to 0. Other value of duration will clamp the looping
        if ( duration != 0.0f )
        {
            endTime = startTime + duration;
        }
        else
        {
            endTime = 0.0f;
        }

        if ( ! calcVariation )
        {
            // If sound is stopped, it will need fadeout
            soundParams->flags |= StopFadeOut;

            // Set looping flag
            soundParams->flags |= Looping;
        }
    }
    else
    {
        if ( ! calcVariation )
        {
            // Calc. time of number of repeats * sound duration
            endTime = soundDuration * repeat;

            // Clamp by manual duration
            if ( duration != 0.0f )
            {
                endTime = min( endTime, duration );
            }
            // Calc. end time
            endTime += startTime;


            if ( repeat != 1 )
            {
                // If sound is stopped, it will need fadeout
                soundParams->flags |= StopFadeOut;
                // Set looping flag
                soundParams->flags |= Looping;
            }
        }
        else
        {
            if ( duration == 0.0f )
            {
                // Sound will play entire
                endTime = soundDuration;
            }
            else
            {
                if ( repeat == 1 )
                {
                    // Sound will play entire
                    endTime = soundDuration;
                }
                else
                {
                    // The current variation plays until 'duration', and a silence is played if duration is greater than sample duration
                    endTime = duration;
                }
            }
            endTime += startTime;
        }
    }

    if ( calcVariation )
    {
        // Set CalcVariation flag
        soundParams->flags |= CalcVariation;

        // Set number of repetitions
        soundParams->repeats = repeat;

        // Set duration
        soundParams->duration = duration;
    }

    // Set mobile flag
    if ( soundInfo->mobile )
    {
        soundParams->flags |= Mobile;
    }

    // Set area trigger flag
    if ( repeat == 0 && duration == 0.0f && soundParams->soundComponent && soundParams->soundComponent->GetComponent<ncAreaTrigger>() )
    {
        soundParams->flags |= Is2DAreaSound;
    }

    soundParams->startTime = startTime;
    soundParams->endTime = endTime;

#ifndef NGAME

    // Set all library parameters to the nSound3
    this->PutSoundParameters( soundParams->sound, *soundInfo );

#endif

    // @todo JJ this search can be avoided with a 'playing' flag in SoundComponentInfo
    //SoundComponentInfo* pSoundParams( soundParams->caller->GetSoundParams() );
    if ( this->playingSounds.FindIndex( soundParams ) == -1 )
    {
        // @todo JJ insert ordered by start  time - perhaps another list ordered by end time?
        this->playingSounds.Append( soundParams );
    }

    // Log play query
    NLOG( soundLog, (0, "PlayQuery: Sound = '%s', repeat = %d, start = %.3f, end = %.3f", soundInfo->soundId.Get(), repeat, startTime, endTime ) );
}

//------------------------------------------------------------------------------
/**
    @brief Play a 3d sound associating to an entity
    @param caller ncSound associated to the playback.
    @param repeat # of repetitions (0=infinite loop)
    @param duration Limit of duration (seconds)   
*/
void
nSoundScheduler::PlayEntity( nEntityObject* caller, int soundIndex, int repeat, nTime duration, bool calcVariation )
{
    n_assert( caller );
    n_assert( caller->GetComponentSafe<ncTransform>() );
    ncSound* sound = caller->GetComponentSafe<ncSound>();
    n_assert( sound );
    if ( sound )
    {
        this->PlayQuery( sound->GetSoundParams(), soundIndex, repeat, duration, calcVariation, sound->GetSoundDefaultVolume() );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Play a sound at a position given her sound name
    @param position Position where the sound is played.
    @param soundName Sound name in library
*/
void 
nSoundScheduler::PlayBySoundName( vector3 position, const char* soundName, int repeat )
{
    int soundIndex = this->GetSoundLibrary()->GetSoundIndex( soundName );
    if ( soundIndex > -1 )
    {
        this->Play(position, soundIndex, repeat);
    }   
}

//------------------------------------------------------------------------------
/**
    @brief Abort the sound playback immediatly
    @param soundInfo The sound component info to abort playback
*/
void
nSoundScheduler::AbortSoundPlayback( SoundComponentInfo* soundInfo )
{
    if ( soundInfo->sound )
    {
        nAudioServer3::Instance()->StopSound( soundInfo->sound );
    }

#ifndef NGAME
    if ( soundInfo->GetState() & ( Playing | Stopping ) )
    {
        // Update sound library's counter of playing sound to allow or not editing the library
        refSoundLibrary->DecPlayingCount();
    }
#endif

    soundInfo->flags = nSoundScheduler::Idle;

    int index = -1;
    if ( ( index = this->playingSounds.FindIndex( soundInfo ) ) != -1 )
    {
        this->playingSounds.EraseQuick( index );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Find a free sound slot
    @return The sound slot
*/
nSoundScheduler::SoundComponentInfo *
nSoundScheduler::FindFreeSound()
{
    SoundComponentInfo * soundInfo = 0;
    int slotIndex = 0;
    while ( slotIndex < this->freeSoundsPool.Size() && ! soundInfo )
    {
        soundInfo = &this->freeSoundsPool.At( slotIndex );
        if ( soundInfo->IsPlaying() || soundInfo->GetState() == Queued )
        {
            soundInfo = 0;
            slotIndex++;            
        }
    }

    // Not found, so take a new slot
    if ( !soundInfo && this->freeSoundsPool.Size() < this->freeSoundsPool.AllocSize() )
    {
        slotIndex = this->freeSoundsPool.Size();
        soundInfo = &this->freeSoundsPool.At( slotIndex );

        NLOG( soundLog, (0, "FindFreeSound: No free slot, array grown. Current size = %d, ", this->freeSoundsPool.Size() ) );
    }

    return soundInfo;
}

//------------------------------------------------------------------------------
/**
    @brief Play a 3d sound not entitled to an entity
    @param pos 3D position of sound
    @param repeat # of repetitions (0=infinite loop)
    @param duration Limit of duration (seconds)
    
*/
void 
nSoundScheduler::Play(vector3 pos, int soundIndex, int repeat, float duration, bool calcVariation)
{
    // Find a free sound 
    SoundComponentInfo* soundInfo = this->FindFreeSound();

    if ( soundInfo )
    {
        // Slot found, play the sound
        soundInfo->soundPos = pos;
        this->PlayQuery( soundInfo , soundIndex, repeat, duration, calcVariation, 1.0f );
    }

    // Sound slot not available, do nothing
}

//------------------------------------------------------------------------------
/**
    @brief Set static nsound3 parameters
    @param sound The nsound3 to put parameters on
    @param info Library parameters to set
*/
void
nSoundScheduler::StopAllSounds()
{
    for (int i=0; i < this->playingSounds.Size(); i++)
    {
        SoundComponentInfo* playingSound = this->playingSounds[i];
        playingSound->Stop();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set static nsound3 parameters
    @param sound The nsound3 to put parameters on
    @param info Library parameters to set
*/
void
nSoundScheduler::PutSoundParameters( nSound3* sound, const nSoundLibrary::SoundInfo& info ) const
{
    n_assert( sound );
    sound->SetMinDist( info.minDist );
    sound->SetMaxDist( info.maxDist );
    sound->SetPriority( info.priority );
}

//------------------------------------------------------------------------------

//@todo commented code before trying one nSound3 for all entities
#if 0
//------------------------------------------------------------------------------
/**
    @brief Search a unused sound slot
*/
int
nSoundScheduler::SearchUnusedSlot( RuntimeSoundInfo& rtInfo, SoundSlot*& soundSlot )
{
    return 0;
/*
    int slotIdx = 0;
    while ( slotIdx < this->runtimeInfo.Size() && ( runtimeInfo[ slotIdx ].flags | Used ) )
    {
        slotIdx++;
    }
    if ( slotIdx < this->runtimeInfo.Size() )
    {
        return slotIdx;
    }
    else
    {
        return InvalidSoundIndex;
    }*/
}
#endif

//------------------------------------------------------------------------------
/**
    @brief Trigger scheduler
*/
void
nSoundScheduler::Trigger(nTime time)
{
#ifndef __ZOMBIE_EXPORTER__

#ifndef NGAME
    nSoundLibrary::SoundInfo* info;
#endif

    // Set time
    this->SetTime( time );

    // Update audio listener transform
    nListener3* listener = nApplication::Instance()->GetAudioListener();
    vector3 listenerPos( 0.0f, 0.0f, 0.0f );
    if ( this->hearingViewport.isvalid() )
    {
        n_assert( listener );

        matrix44 view = this->hearingViewport->GetViewMatrix();
        listenerPos = view.pos_component();

        listener->SetTransform( view );
    }
    else
    {
        listenerPos = listener->GetTransform().pos_component();
    }

    // Process playing sounds. Stopped sounds are tested for startTime and playing sounds are tested for endTime
    // @todo Make two lists, ordered by startTime and endTime (ordered at insertion)
    int i = 0;
    bool advanceToNextPlayingSound = true;
    while ( i < this->playingSounds.Size() )
    {
        SoundComponentInfo* soundParams = this->playingSounds[i];
        n_assert( soundParams );

        ncSound* soundComp = soundParams->soundComponent;
        float v = 0.0f;

        if ( soundParams->sound )
        {
            // Check sound status
            if ( soundParams->bufferIndex != InvalidSoundIndex )
            {
                int soundStatus = soundParams->sound->GetStatus( soundParams->bufferIndex );
                if ( soundStatus & nSound3::SOUNDBUFFER_TERMINATED )
                {
                    // Log that this sound was terminated by another higher priority / nearer sound
                    int otherPlayingIndex = 0;
                    SoundComponentInfo* otherSoundParams = 0;
                    while ( otherPlayingIndex < this->playingSounds.Size() )
                    {
                        otherSoundParams = this->playingSounds[otherPlayingIndex];
                        if ( otherSoundParams && otherSoundParams->bufferIndex != InvalidSoundIndex &&
                             otherSoundParams->bufferIndex == soundParams->bufferIndex)
                        {

                            break;
                        }
                        else
                        {
                            otherPlayingIndex++;
                        }
                    }
                    if ( otherPlayingIndex < this->playingSounds.Size() )
                    {
                        n_assert( otherSoundParams  );
                        if ( otherSoundParams->soundComponent )
                        {
                            NLOG( soundLog, (0, "Sound terminated: sound = '%s', buffer = %d, replacing sound = '%s', entity id = %d", this->refSoundLibrary->GetSoundInfo( soundParams->soundIndex ).soundId.Get(), soundParams->bufferIndex, this->refSoundLibrary->GetSoundInfo( otherSoundParams->soundIndex ).soundId.Get(), otherSoundParams->soundComponent->GetEntityObject()->GetId() ) );
                        }
                        else
                        {
                            NLOG( soundLog, (0, "Sound terminated: sound = '%s', buffer = %d, replacing sound = '%s'.", this->refSoundLibrary->GetSoundInfo( soundParams->soundIndex ).soundId.Get(), soundParams->bufferIndex, this->refSoundLibrary->GetSoundInfo( otherSoundParams->soundIndex ).soundId.Get() ) );
                        }
                    }
                    else
                    {
                        NLOG( soundLog, (0, "Sound terminated: sound = '%s', buffer = %d, replacing sound unknown.",  this->refSoundLibrary->GetSoundInfo( soundParams->soundIndex ).soundId.Get(), soundParams->bufferIndex ) );
                    }

                    if ( soundComp )
                    {
                        // Set invalid index to the sound component
                        soundComp->SetBufferIndex( InvalidSoundIndex );
                    }
                    soundParams->sound = 0;
                    continue;
                }
            }

            switch ( soundParams->GetState() )
            {
            case Queued:
                // Sound is queued, check if it must be played now
                if ( soundParams->startTime > this->currentTime )
                {
                    break;
                }

#ifndef NGAME
                // Set static parameters to the sound
                info = & this->refSoundLibrary->GetSoundInfo( soundParams->soundIndex );
                this->PutSoundParameters( soundParams->sound, *info );
#endif

                // Set play parameters to the nSound3 (volume, frequency, property sets,...) that must be stored/referenced in ncSound
                // Volume
                if ( ! soundParams->Get2DAreaSound() )
                {
                    soundParams->currentVol = soundParams->playVol;
                    soundParams->sound->SetVolume( soundParams->playVol );
                }
                else
                {
                    // 2d ambient sounds are played with initial volume 0 if they have fade distance
                    float v = soundParams->playVol;
                    if ( soundComp )
                    {
                        ncTriggerShape * triggerShape = soundComp->GetComponentSafe<ncTriggerShape>();
                        if ( triggerShape->GetFadeDistance() > TINY || triggerShape->GetFadeVelocity() < 1.0f )
                        {
                            v = 0.0f;
                        }
                    }
                    soundParams->currentVol = v;
                    soundParams->sound->SetVolume( v );
                }

                // Looping
                soundParams->sound->SetLooping( soundParams->GetLooping() );

                // Play sound
                this->refAudioServer.get()->StartSound( soundParams->sound );

                // Update volume
                soundParams->sound->Update();

                // Get the buffer used from the nSound3
                soundParams->bufferIndex = soundParams->sound->GetLastBufferUsed();

                n_assert( soundParams->bufferIndex != InvalidSoundIndex );

                // Set playing state
                soundParams->SetState( Playing );

                // If sound playback is associated to an entity
                if ( soundComp )
                {
                    // Set sound position
                    soundComp->GetPositionFromEntity();
                }
                // Else, the sound is played freely
                else
                {
                    // @todo JJ: when sounds are started from nSoundScheduler::Play, they can be assigned position directly
                    // rather that storing it in SoundComponentInfo
                    soundParams->SetSoundPosition( soundParams->soundPos );
                }

#ifndef NGAME
                // Update sound library's counter of playing sound to allow or not editing the library
                refSoundLibrary->IncPlayingCount();
#endif
                
                // Log sound play
                NLOG( soundLog, (0, "Sound started: Sound '%s', buffer index %d",  this->refSoundLibrary->GetSoundInfo( soundParams->soundIndex ).soundId.Get(), soundParams->bufferIndex) );
                break;

            case Playing:

                // Sound is playing

                // Infinite looping sounds have endTime set to 0
                if ( soundParams->endTime != 0.0f && soundParams->endTime <= this->currentTime )
                {
                    // Stop sound
                    if ( soundParams->GetStopFadeOut() )
                    {
                        // If it has been stopped, enter Stopping state for fadeout
                        soundParams->endTime += FadeoutDuration;

                        soundParams->currentVol = soundParams->currentVol * 0.5f;
                        soundParams->sound->SetVolume( soundParams->currentVol );
                        soundParams->sound->Update();

                        // Set playing state
                        soundParams->SetState( Stopping );
                    }
                    else
                    {
                        // Get repeat state before changing it
                        bool calcVariation = soundParams->GetCalcVariation();

                        // If the sample just ended, set state to idle
                        soundParams->SetState( Idle );

                        // Remove from playing sounds
                        this->playingSounds.EraseQuick( i );
                        advanceToNextPlayingSound = false;
#ifndef NGAME
                        // Update sound library's counter of playing sound to allow or not editing the library
                        refSoundLibrary->DecPlayingCount();
#endif

                        if ( soundComp )
                        {
                            // Set invalid index to the sound component
                            soundComp->SetBufferIndex( InvalidSoundIndex );
                        }
                        soundParams->sound = 0;

                        // Sound is relaunched to play a different variation, if repeats is not 1
                        if ( calcVariation && soundParams->repeats != 1 )
                        {
                            if ( soundParams->repeats != 0 )
                            {
                                soundParams->repeats --;
                            }

                            this->PlayQuery( soundParams, soundParams->soundIndex, soundParams->repeats, soundParams->duration, true, soundParams->initialVol );
                        }
                    }
                }
                else
                {
                    // Process area trigger sounds. Variate volume depending on player position

                    if ( soundParams->Get2DAreaSound() && soundComp )
                    {
                        ncAreaTrigger * areaTrigger = soundComp->GetComponentSafe<ncAreaTrigger>();
                        if ( areaTrigger->GetTriggerEnabled() )
                        {
                            // Calculate volume based on position of hearing entity
                            ncTriggerShape * triggerShape = soundComp->GetComponentSafe<ncTriggerShape>();
                            if ( triggerShape->GetFadeDistance() > TINY || triggerShape->GetFadeVelocity() < 1.0f )
                            {
                                float relDistance = triggerShape->GetFadedDistance( listenerPos );
                                float volume;
                                volume = soundParams->playVol * min ( 1.0f, relDistance );

                                // Adjust volume lineary rather than setting it instantly
                                float difVol = volume - soundParams->currentVol;
                                if ( difVol >= 0.0f )
                                {
                                    volume = soundParams->currentVol + min( abs( difVol ), triggerShape->GetFadeVelocity() );
                                }
                                else
                                {
                                    volume = soundParams->currentVol - min( abs( difVol ), triggerShape->GetFadeVelocity() );
                                }
                                soundParams->currentVol = volume;
                                soundParams->sound->SetVolume( volume );
                                soundParams->sound->Update();
                            }
                        }
                    }

                    // Update volume and frequency variations
                    if ( soundParams->GetDynamicVolFreq() )
                    {
                        soundParams->sound->Update();
                    }
                }
                break;

            case Stopping:

                // Make fadeout
                n_assert( soundParams->sound );
                if ( soundParams->bufferIndex != InvalidSoundIndex )
                {
                    v = soundParams->currentVol;
                    v = max( 0.0f, v * StopDecay );
                    //v = max( 0.0f, v - (min(0.9f, 1.0f - v)));
                    soundParams->currentVol = v;
                    soundParams->sound->SetVolume( v );
                    soundParams->sound->Update( soundParams->bufferIndex );
                }

                if ( v < 0.001f || ( soundParams->endTime != 0.0f && soundParams->endTime <= this->currentTime ) ||
                     soundParams->bufferIndex == InvalidSoundIndex )
                {
                    // Fadeout finished
                    if ( soundParams->GetStopFadeOut() && soundParams->bufferIndex != InvalidSoundIndex )
                    {
                        soundParams->sound->Stop( soundParams->bufferIndex );
                    }
                    soundParams->SetState( Idle );
                    advanceToNextPlayingSound = false;
                    this->playingSounds.EraseQuick( i );

#ifndef NGAME
                    // Update sound library's counter of playing sound to allow or not editing the library
                    refSoundLibrary->DecPlayingCount();
#endif
                    // Set invalid index to the sound component
                    if ( soundComp )
                    {
                        soundComp->SetBufferIndex( InvalidSoundIndex );
                    }
                    soundParams->sound = 0;
                }
                break;
            
            case Idle:
                // This sound has been 'aborted' from a call to ncSound::PrePlay that didn't succeed, probably because the new sound
                // that was going to be played didn't exist in the sound table. So remove the sound from 
                this->playingSounds.EraseQuick( i );
                advanceToNextPlayingSound = false;
                break;
            
            default:
                // This line shouldn't be reached
                n_assert_always();
                break;
            }
        }

        if ( advanceToNextPlayingSound )
        {
            i++;
        }
        else
        {
            advanceToNextPlayingSound = true;
        }
    }
#endif
}

//------------------------------------------------------------------------------
/**
    @brief Set current time
    @param time The time
*/
void
nSoundScheduler::SetTime(nTime time)
{
    this->currentTime = time;
}

//------------------------------------------------------------------------------
/**
    @brief Get current time
    @return the Time
*/
nTime
nSoundScheduler::GetTime()
{
    return this->currentTime;
}

//------------------------------------------------------------------------------
/**
    @brief Set position and velocity to the nSound3
*/
void
nSoundScheduler::SoundComponentInfo::SetSoundPositionAndVelocity(const vector3& pos, const vector3& vel)
{
    if ( this->sound && this->GetMobile() )
    {
        this->sound->SetPositionAndVelocity( pos, vel );

        // Set position to the buffer, the index is stored from play in the sound scheduler
        if ( this->bufferIndex != InvalidSoundIndex )
        {
            this->sound->Update3DProperties( this->bufferIndex );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set position to the nSound3
*/
void
nSoundScheduler::SoundComponentInfo::SetSoundPosition(const vector3& pos)
{
    if ( this->sound && this->GetMobile() )
    {
        this->sound->SetPosition( pos );

        // Set 3D position to the buffer, the buffer index is stored from play in the sound scheduler
        if ( this->bufferIndex != InvalidSoundIndex )
        {
            this->sound->Update3DProperties( this->bufferIndex );
        }
    }
}

//------------------------------------------------------------------------------
/**
    @brief Queue stop of sound
*/
void
nSoundScheduler::SoundComponentInfo::Stop()
{
    if ( this->sound && ( this->GetState() == nSoundScheduler::Playing || this->GetState() == nSoundScheduler::Queued ) )
    {
        if ( ! this->Get2DAreaSound() )
        {
            this->endTime = nSoundScheduler::Instance()->GetTime() + FadeoutDuration;
        }
        this->flags = nSoundScheduler::StopFadeOut | nSoundScheduler::Stopping;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Set hearing viewport
*/
void
nSoundScheduler::SetHearingViewport( nAppViewport * viewport)
{
    if ( ! viewport )
    {
        this->hearingViewport.invalidate();
    }
    else
    {
        this->hearingViewport = viewport;
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get hearing viewport
*/
nAppViewport *
nSoundScheduler::GetHearingViewport()
{
    return this->hearingViewport;
}

//------------------------------------------------------------------------------
/**
    @brief Return a sound
    @params soundIndex The sound is looking for.

*/
nSoundScheduler::SoundComponentInfo *
nSoundScheduler::FindPlayingSound( int soundIndex )
{
    SoundComponentInfo * soundInfo = 0;
    int index = 0;
    while ( index < this->playingSounds.Size() && ! soundInfo )
    {
        soundInfo = this->playingSounds.At( index );
        if ( soundInfo->soundIndex != soundIndex )
        {
            soundInfo = 0;
            index++;
        }
    }

    return soundInfo;
}

//------------------------------------------------------------------------------

// @todo / **NOTE** This code block is neccesary when using OpenAL Server, but must not be when using DS server. DS server does not handle
// not-shared sound resources, so you never create a nSoundResource, that's done internally.
// Solution: make OpenAL server (and resources) behave this way is simplest, though DS server could alse be modified.

// Load sound resource if not loaded
//nSoundResource* soundRes = refAudioServer->NewSoundResource( info.soundFileName.Get() );
//n_assert( soundRes );
//if ( !soundRes->IsLoaded() )
//{
//    soundRes->SetFilename( info.soundFileName );
//    soundRes->SetLooping( info.repeat != 1 );
//    soundRes->SetAmbient( info.ambient );
//    soundRes->SetStreaming( false );
//
//    if ( !soundRes->Load() )
//    {
//        // Print log message 'Sound file not found...'
//        
//        // Should be called soundRes->Release() or something?
//    }
//}
