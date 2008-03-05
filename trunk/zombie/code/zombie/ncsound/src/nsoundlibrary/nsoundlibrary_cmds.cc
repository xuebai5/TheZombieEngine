#include "precompiled/pchncsound.h"
/*-----------------------------------------------------------------------------
    @file nsoundlibrary_cmds.cc

    @author Juan Jose Luna

    @brief nSoundLibrary persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "nsoundlibrary/nsoundlibrary.h"
#include "kernel/npersistserver.h"
#include "entity/nentityobjectserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nSoundLibrary )
    NSCRIPT_ADDCMD('JNWS', int, NewSound, 6, (const char*, const char*, bool, bool, int, int ), 0, ());
    NSCRIPT_ADDCMD('JADS', int, AddSound, 6, (const char*, const char*, bool, bool, int, int ), 0, ());
    NSCRIPT_ADDCMD('JSSP', void, SetSoundParams, 5, (const char*, bool, float, float, float), 0, ());
    NSCRIPT_ADDCMD('JSTP', void, SetSoundTimeParams, 4, (const char*, float, float, float), 0, ());
    NSCRIPT_ADDCMD('JSS2', void, SetSoundParams2, 6, (const char*, int, bool, float, float, float), 0, ());
    NSCRIPT_ADDCMD('JST2', void, SetSoundTimeParams2, 5, (const char*, int, float, float, float), 0, ());
    NSCRIPT_ADDCMD('EADD', int, AddDefaultSound, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('EADR', int, AddDefaultSoundWithResource, 2, (const char*, const char*), 0, ());
    NSCRIPT_ADDCMD('EREM', bool, RemoveSound, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ENUM', int, GetNumberOfSounds , 0, (), 0, ());
    NSCRIPT_ADDCMD('JGSI', int, GetSoundIndex, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('EG01', nString, GetSoundId, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EG02', nString, GetResource, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES02', void, SetResource, 2, (int, const char*), 0, ());
    NSCRIPT_ADDCMD('EG03', bool, Get2dFlag, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES03', void, Set2dFlag, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('EG04', float, GetMinDist, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES04', void, SetMinDist, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('EG05', float, GetMaxDist, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES05', void, SetMaxDist, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('EG12', float, GetVolume, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES12', void, SetVolume, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('EG06', float, GetPitchVar, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES06', void, SetPitchVar, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('EG07', float, GetVolumeVar, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES07', void, SetVolumeVar, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('EG08', bool, GetMobile, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES08', void, SetMobile, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('JGPC', bool, GetPitchControl, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JSPC', void, SetPitchControl, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('JSPI', void, SetPitchControlById, 2, (const char *, bool), 0, ());
    NSCRIPT_ADDCMD('EG09', int, GetMaxInstances, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES09', void, SetMaxInstances, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('EG10', bool, GetStreamed, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES10', void, SetStreamed, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('EG11', int, GetPriority, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES11', void, SetPriority, 2, (int, int), 0, ());
    NSCRIPT_ADDCMD('EG13', float, GetDelay, 1, (int), 0, ());
    NSCRIPT_ADDCMD('ES13', void, SetDelay, 2, (int, float), 0, ());
    NSCRIPT_ADDCMD('JPLS', void, Play, 1, (int), 0, ());
    NSCRIPT_ADDCMD('LPLR', void, PlayRepeat, 2, (int,int), 0, ());
    NSCRIPT_ADDCMD('JSAS', void, StopAllSounds , 0, (), 0, ());
    NSCRIPT_ADDCMD('EGDI', bool, GetDirty , 0, (), 0, ());
    NSCRIPT_ADDCMD('ESDI', void, SetDirty, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('EGNV', int, GetSoundNumberOfVariations, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JSMN', void, SetMagicNumber, 1, (unsigned int), 0, ());
#ifndef NGAME
    NSCRIPT_ADDCMD('JGMF', int, GetMaxFreeSounds , 0, (), 0, ());
    NSCRIPT_ADDCMD('JSMF', void, SetMaxFreeSounds, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JINS', bool, IsNewSound, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JSMU', void, SetMuteFlag, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('JSSO', void, SetSoloFlag, 2, (int, bool), 0, ());
    NSCRIPT_ADDCMD('JGMU', bool, GetMuteFlag, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JGSO', bool, GetSoloFlag, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JRMS', void, ResetAllMuteSolo , 0, (), 0, ());
    NSCRIPT_ADDCMD('JGNG', int, GetNumberOfSoundGroups, 0, (), 0, ());
    NSCRIPT_ADDCMD('JGGN', nString, GetSoundGroupName, 1, (int), 0, ());
    NSCRIPT_ADDCMD('JAGN', void, AddSoundGroupName, 4, (const char *, const char *, const char *, const char *), 0, ());
#endif
    NSCRIPT_ADDCMD('JGSS', nObject *, GetSoundScheduler , 0, (), 0, ());

    cl->BeginSignals( 2 );
    N_INITCMDS_ADDSIGNAL( SoundAddedToLibrary );
    N_INITCMDS_ADDSIGNAL( FailedLoadingSound );
    cl->EndSignals();
    
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool 
nSoundLibrary::SaveCmds (nPersistServer* N_IFNDEF_NGAME(ps))
{
#ifndef NGAME
    for(nEntityObjectId hid = 0;hid < (1 << nEntityObjectServer::IDHIGHBITS);hid++)
    {
        // put magic number command
        nCmd * cmd = ps->GetCmd(this, 'JSMN');
        cmd->Rewind();
        cmd->In()->SetI( hid << nEntityObjectServer::IDLOWBITS );
        ps->PutCmd( cmd );

        for (int row = 0; row < this->soundLibrary.Size(); row++)
        {
            SoundInfo info = this->soundLibrary[ row ];
            if ( ( hid << nEntityObjectServer::IDLOWBITS ) == info.magicNumber )
            {
                ps->Put(this, 'JADS', info.soundId.Get(), info.resourceName.Get(), info.is2d, info.streamed, info.priority, info.maxSoundInstances );
                ps->Put(this, 'JSS2', info.soundId.Get(), info.variationNumber, info.mobile, info.minDist, info.maxDist, info.volume );
                ps->Put(this, 'JST2', info.soundId.Get(), info.variationNumber, info.delay, info.pitchVar, info.volVar );
                if ( info.pitchControl )
                {
                    ps->Put(this, 'JSPI', info.soundId.Get(), true );
                }
            }
        }
    }
#endif
    return true;
}

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
