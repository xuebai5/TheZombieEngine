#include "precompiled/pchncsound.h"
//------------------------------------------------------------------------------
//  nsoundeventtable_cmds.cc
// (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nsoundeventtable/nsoundeventtable.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nSoundEventTable )
    NSCRIPT_ADDCMD('JASE', void, AddSoundEvent, 3, (const char*, const char*, const char*), 0, ());
    NSCRIPT_ADDCMD('EREM', void, RemoveSoundEvent, 2, (const char*, const char*), 0, ());
    NSCRIPT_ADDCMD('ENUM', int, GetNumberOfSounds , 0, (), 0, ());
    NSCRIPT_ADDCMD('EGET', void, GetSoundEvent, 1, (int), 3, (nString&, nString&, nString&));
    NSCRIPT_ADDCMD('EGID', const char*, GetSoundId, 2, (const char*, const char*), 0, ());
    NSCRIPT_ADDCMD('ESID', void, SetSoundId, 3, (const char*, const char*, const char*), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('JISG', bool, ImportSoundGroup, 1, (const char*), 0, ());
#endif
    NSCRIPT_ADDCMD('JDSE', int, DuplicateSoundEvent, 2, (const char*, const char*), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
nSoundEventTable::SaveCmds (nPersistServer* ps)
{
    // Save sound events mapping

    this->soundMap.Begin();

    nString key;
    SoundMapInfo* info;
    this->soundMap.Next(key, info);

    while ( info )
    {
        int separatorIdx = key.FindChar( '%', 0 );
        n_assert( separatorIdx != -1 );
        // Save addsound command (event, material, sound id)
        nString eventString = key.ExtractRange(0, separatorIdx );
        nString materialString = key.ExtractRange(separatorIdx + 1, key.Length() - separatorIdx - 1);
        ps->Put( this, 'JASE', eventString.Get(), materialString.Get(), info->soundId.Get() );

        this->soundMap.Next(key, info); 
    }
    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
