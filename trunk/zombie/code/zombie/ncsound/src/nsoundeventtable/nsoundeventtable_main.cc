#include "precompiled/pchncsound.h"
//------------------------------------------------------------------------------
//  nSoundEventTable_main.cc
//  (C) 2004 Conjurer Services, S.A.
//------------------------------------------------------------------------------

#include "stdlib.h"
#include "nsoundeventtable/nsoundeventtable.h"
#include "nsoundscheduler/nsoundscheduler.h"
#include "nsoundlibrary/nsoundlibrary.h"
#include "kernel/nlogclass.h"

//------------------------------------------------------------------------------
nNebulaScriptClass(nSoundEventTable, "nobject");

//------------------------------------------------------------------------------
/**
*/
nSoundEventTable::nSoundEventTable():
    soundMap(1)
{
    // empty
}
//------------------------------------------------------------------------------
/**
*/
nSoundEventTable::~nSoundEventTable()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    @brief Get sound id with event
    @param the event name
    Gets the sound id string associated with an event
*/
const char*
nSoundEventTable::GetSoundId(const char* event) const
{
    nString key = event;
    key.Append("%");
    SoundMapInfo* info = this->soundMap[ key ];
    if ( info )
    {
        return info->soundId.Get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    @brief Get sound id with event & material
    @param the event name
    @param the material name
    Gets the sound id string associated with an event and material
*/
const char*
nSoundEventTable::GetSoundId(const char* event, const char* material) const
{
    nString key = event;
    key.Append("%");
    key.Append( material);
    SoundMapInfo* info = this->soundMap[ key ];
    if ( info )
    {
        return info->soundId.Get();
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
    @brief Set sound id by event & material
*/
void
nSoundEventTable::SetSoundId( const char* event, const char* material, const char* soundId )
{
    nString key = this->MakeKey( event, material );
    SoundMapInfo* info = this->soundMap[ key ];
    if ( info )
    {
        info->soundId = soundId;
        info->soundIndex = nSoundLibrary::Instance()->GetSoundIndex( soundId );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Get sound global index with event
    @param the event name
*/
int
nSoundEventTable::GetSoundIndex(const char* event) const
{
    n_assert( event );
    nString key = event;
    key.Append("%");
    SoundMapInfo* info = this->soundMap[ key ];
    if ( info )
    {
#ifndef NGAME
        // In editor time, get index directly from sound library to avoid recalculating indexes
        return nSoundLibrary::Instance()->GetSoundIndex( info->soundId.Get() );
#else
        // In game time, return stored index
        return info->soundIndex;
#endif
    }
    return InvalidSoundIndex;
}

//------------------------------------------------------------------------------
/**
    @brief Get sound global index with event & material
    @param the event name
    @param the material name
*/
int
nSoundEventTable::GetSoundIndex(const char* event, const char* material) const
{
    n_assert( event );
    n_assert( material );

    nString key = event;
    key.Append("%");
    key.Append( material );
    SoundMapInfo* info = this->soundMap[ key ];
    if ( info )
    {
#ifndef NGAME
        // In editor time, get index directly from sound library to avoid recalculating indexes
        return nSoundLibrary::Instance()->GetSoundIndex( info->soundId.Get() );
#else
        // In game time, return stored index
        return info->soundIndex;
#endif
    }
    return InvalidSoundIndex;
}

//------------------------------------------------------------------------------
/**
    @brief Generate a key from an event and material
*/
nString
nSoundEventTable::MakeKey( const char* event, const char* material ) const
{
    n_assert( event && material );
    nString key = event;
    key.Append( "%" );
    key.Append( material );
    return key;
}

//------------------------------------------------------------------------------
/**
    @brief Extract the event and material from a key
*/
void
nSoundEventTable::UnmakeKey( const nString& key, nString& event, nString& material ) const
{
    int pos = key.FindChar( '%', 0 );
    n_assert( pos >= 0 );
    event.Set( key.Get(), pos );
    material.Set( &key.Get()[pos+1] );
}

//------------------------------------------------------------------------------
/**
    @brief Add a sound event
    @param event Event name
    @param material Material name
    @param soundId Sound id string
*/
void
nSoundEventTable::AddSoundEvent( const char* event, const char* material, const char* soundId )
{
    n_assert( event && material );
    nString key = event;
    key.Append( "%" );
    key.Append( material );
    SoundMapInfo info;
    info.soundId = soundId;
    nSoundLibrary* soundLibrary = nSoundScheduler::Instance()->GetSoundLibrary();
    n_assert( soundLibrary );
    info.soundIndex = soundLibrary->GetSoundIndex( soundId );
    if ( info.soundIndex == InvalidSoundIndex )
    {
        NLOG( soundLog, (0, "Error: Tried to add non-existing sound '%s' (event '%s', material '%s')", soundId, event, soundId ) );
        return;
    }

    SoundMapInfo* soundMInfo = this->soundMap[ key ];
    if ( ! soundMInfo )
    {
        this->soundMap.Add( key, &info );
    }
}

//------------------------------------------------------------------------------
/**
    @brief Remove a sound event by event & material
*/
void
nSoundEventTable::RemoveSoundEvent( const char* event, const char* material )
{
    nString key = this->MakeKey( event, material );
    this->soundMap.Remove( key );
}

//------------------------------------------------------------------------------
/**
    @brief Remove all event rows that refers to a sound name
*/
bool
nSoundEventTable::RemoveSoundRows( const char* soundId )
{
    bool result = false;
    this->soundMap.Begin();
    nString key;
    nSoundEventTable::SoundMapInfo* info;
    this->soundMap.Next( key, info );
    while ( info )
    {
        if ( strcmp( info->soundId.Get(), soundId ) == 0 )
        {
            this->soundMap.Remove( key );
            this->soundMap.Next( key, info );
            result = true;
        }
        else
        {
            this->soundMap.Next( key, info );
        }
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    @brief Get the greatest maximum distance among all the sounds
*/
float
nSoundEventTable::GetGreatestMaxDistance()
{
    float greatestMaxDist = 0;
    this->soundMap.Begin();
    for ( SoundMapInfo* info = this->soundMap.Next(); info; info = this->soundMap.Next() )
    {
        float maxDist = nSoundLibrary::Instance()->GetSoundInfo( info->soundIndex ).maxDist;
        if ( maxDist > greatestMaxDist )
        {
            greatestMaxDist = maxDist;
        }
    }
    return greatestMaxDist;
}

//------------------------------------------------------------------------------
/**
    @brief Get number of sound events
*/
int
nSoundEventTable::GetNumberOfSounds() const
{
    return const_cast<nMapTableTypes<SoundMapInfo>::NString&>(this->soundMap).GetCount();
}

//------------------------------------------------------------------------------
/**
    @brief Get full sound event by index
*/
void
nSoundEventTable::GetSoundEvent( int index, nString& event, nString& material, nString& soundId )
{
    this->soundMap.Begin();
    for ( int i = 0; ; ++i )
    {
        if ( i < index )
        {
            this->soundMap.Next();
        }
        else
        {
            nString key;
            SoundMapInfo* info;
            this->soundMap.Next( key, info );
            n_assert( info );
            this->UnmakeKey( key, event, material );
            soundId = info->soundId;
            break;
        }
    }
}

#ifndef NGAME
//------------------------------------------------------------------------------
/**
    @brief Get full sound event by index
*/
bool
nSoundEventTable::ImportSoundGroup( const char * groupName )
{
    nSoundLibrary* soundLibrary = nSoundScheduler::Instance()->GetSoundLibrary();
    return soundLibrary->ImportSoundGroup( this, groupName );
}
#endif

//------------------------------------------------------------------------------
/**
    @brief Duplicate a set of sound events changing only the event name
    @param eventName source event name
    @param destEvent Event name of new rows in the table
    @return number of events added
*/
int
nSoundEventTable::DuplicateSoundEvent( const char * eventName, const char* destEvent )
{
    if ( !eventName || !destEvent || !*eventName || !*destEvent )
    {
        return 0;
    }

    nString key;
    nString event;
    nString material;
    nArray<nString> newEvents;

    this->soundMap.Begin();
    for ( int i = 0; i < this->soundMap.GetCount(); ++i )
    {
        SoundMapInfo* info;
        this->soundMap.Next( key, info );
        n_assert( info );
        this->UnmakeKey( key, event, material );

        if ( event == eventName )
        {
            newEvents.Append( material );
            newEvents.Append( info->soundId );
        }
    }

    int added = 0;
    for ( int i = 0; i < newEvents.Size(); i+= 2 )
    {
        n_assert( i < newEvents.Size() - 1);
        this->AddSoundEvent( destEvent, newEvents[i].Get(), newEvents[i+1].Get() );
        added++;
    }
    return added;
}

//------------------------------------------------------------------------------
/**    
    LoadSounds

    @brief Load all sound in table
    @returns true if there is not problem
*/
bool
nSoundEventTable::LoadSounds()
{
    SoundMapInfo * soundInfo = 0;

    this->soundMap.Begin();

    while( 0 != ( soundInfo = this->soundMap.Next() ) )
    {
        if ( ! nSoundScheduler::Instance()->IsLoadedSound( soundInfo->soundIndex ) )
        {
            if ( ! nSoundScheduler::Instance()->LoadSound( soundInfo->soundIndex ) )
            {
                return false;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
    UnloadSounds

    @brief Unload all sounds in table
    @returns true if there is not problem
*/
bool
nSoundEventTable::UnloadSounds()
{
    SoundMapInfo * soundInfo = 0;

    this->soundMap.Begin();

    while( 0 != ( soundInfo = this->soundMap.Next() ) )
    {
        if ( nSoundScheduler::Instance()->IsLoadedSound( soundInfo->soundIndex ) )
        {
            if ( ! nSoundScheduler::Instance()->UnloadSound( soundInfo->soundIndex ) )
            {
                return false;
            }
        }
    }

    return true;
}

//------------------------------------------------------------------------------
