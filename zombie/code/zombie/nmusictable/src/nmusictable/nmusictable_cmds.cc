#include "precompiled/pchnmusictable.h"
/*-----------------------------------------------------------------------------
    @file nmusictable_cmds.cc

    @author Juan Jose Luna

    @brief nMusicTable persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "nmusictable/nmusictable.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nMusicTable )
    NSCRIPT_ADDCMD('JAMS', void, AddMusicSample, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('JATS', void, AddTransitionSample, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('JSTT', void, AddTransition, 5, (const char *, const char *, const char *, float, float), 0, ());
    NSCRIPT_ADDCMD('JEPI', void, SetMusicSampleExitPointsInterval, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD('JMEP', void, AddMusicSampleExitPoint, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD('JPIN', bool, PlayMusicPartByIndex, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('JSTP', void, StopMusic, 0, (), 0, ());
    NSCRIPT_ADDCMD('JPLI', void, PlayMusicStinger, 2, (const char *, float), 0, ());
    NSCRIPT_ADDCMD('JPLS', void, PlayMusicStingerByIndex, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('JADM', void, AddMood, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('JAMM', void, AddMoodMusicSample, 2, (const char*, const char *), 0, ());
    NSCRIPT_ADDCMD('JSCM', void, SetCurrentMood, 2, (const char*, bool), 0, ());
    NSCRIPT_ADDCMD('JADS', void, AddStyle, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('JASM', void, AddMoodToStyle, 3, (const char*, const char *, int), 0, ());
    NSCRIPT_ADDCMD('JSCS', void, SetCurrentStyle, 3, (const char *, int, bool), 0, ());
    NSCRIPT_ADDCMD('JSSI', void, SetCurrentStyleByIndex, 3, (int, int, bool), 0, ());
    NSCRIPT_ADDCMD('SCSI', void, SetCurrentStyleIntensity, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('JPLY', void, SendPlayMusicPart, 2, (const char *, nObject *), 0, ());
    NSCRIPT_ADDCMD('JSTS', void, SendStopMusic, 1, (nObject *), 0, ());
    NSCRIPT_ADDCMD('JPLT', void, SendPlayMusicStinger, 3, (const char *, float, nObject *), 0, ());
    NSCRIPT_ADDCMD('ESMR', void, SendMoodToPlay, 3, (const char*, nObject *, bool), 0, ());
    NSCRIPT_ADDCMD('JNCS', void, SendSetCurrentStyle, 4, (const char *, int, nObject *, bool), 0, ());
    NSCRIPT_ADDCMD('GNMS', int, GetNumberOfMusicSamples , 0, (), 0, ());
    NSCRIPT_ADDCMD('GMSN', const nString&, GetMusicSampleName, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GNMD', int, GetNumberOfMoods , 0, (), 0, ());
    NSCRIPT_ADDCMD('GMDN', const nString&, GetMoodName, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GNST', int, GetNumberOfStyles , 0, (), 0, ());
    NSCRIPT_ADDCMD('GSTN', const char*, GetStyleName, 1, (int), 0, ());
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
nMusicTable::SaveCmds (nPersistServer* ps)
{
    
    // Music samples info
    for ( int i=0; i < this->musicSamples.Size(); i++)
    {
        ps->Put( this, 'JAMS', this->musicSamples[i].name.Get() );
        ps->Put( this, 'JEPI', this->musicSamples[i].name.Get(), this->musicSamples[i].exitPointInterval );
        for ( int s = 0; s < this->musicSamples[i].exitPoints->Size(); s++ )
        {
            float t = (*this->musicSamples[i].exitPoints)[s];
            ps->Put( this, 'JMEP', this->musicSamples[i].name.Get(), t );
        }
    }
    // Transition samples info
    for ( int i=0; i < this->transitionSamples.Size(); i++)
    {
        ps->Put( this, 'JATS', this->transitionSamples[i].name.Get() );
    }
    // Transitions info
    for ( int i=0; i < this->transitions.Size(); i++)
    {
        const char * transitionSampleName = 0;
        if ( this->transitions[i].transitionSample != -1 )
        {
            transitionSampleName = this->transitionSamples[ this->transitions[i].transitionSample ].name.Get();
        }

        ps->Put( this, 'JSTT', this->musicSamples[ this->transitions[i].fromSample ].name.Get(),
                               this->musicSamples[ this->transitions[i].toSample ].name.Get(),
                               transitionSampleName,
                               this->transitions[i].fadeInDuration,
                               this->transitions[i].fadeOutDuration);
    }

    // Mood info
    for ( int mood=0; mood < this->moodInfo.Size(); mood++)
    {
        ps->Put( this, 'JADM', this->moodInfo[mood].name);
        for ( int moodSample=0; moodSample < this->moodInfo[mood].musicSamples.Size(); moodSample++)
        {
            ps->Put( this, 'JAMM', this->moodInfo[mood].name.Get(), this->musicSamples[ this->moodInfo[mood].musicSamples[ moodSample ] ].name.Get() );
        }
    }

    // Style info
    StyleInfo * sInfo = 0;
    nString styleName;
    this->styleInfo.Begin();
    this->styleInfo.Next(styleName, sInfo);
    while ( sInfo )
    {
        n_assert( !styleName.IsEmpty() );
        ps->Put( this, 'JADS', styleName);
        for ( int styleMood=0; styleMood < sInfo->moods.Size(); styleMood++)
        {
            int moodIndex = this->FindMood( sInfo->moods[styleMood].Get() );
            n_assert( moodIndex >= 0 );
            ps->Put( this, 'JASM', sInfo->moods[styleMood].Get(), styleName.Get(), this->moodInfo[moodIndex].intensity );
        }
        this->styleInfo.Next(styleName, sInfo);
    }

    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
