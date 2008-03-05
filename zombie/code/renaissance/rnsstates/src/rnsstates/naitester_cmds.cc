//-----------------------------------------------------------------------------
//  naitester_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//-----------------------------------------------------------------------------
#include "precompiled/pchntrigger.h"
#include "rnsstates/naitester.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nAITester )
#ifndef NGAME

    NSCRIPT_ADDCMD('EEGU', void, EnableGameplayUpdating , 0, (), 0, ());
    NSCRIPT_ADDCMD('EDGU', void, DisableGameplayUpdating , 0, (), 0, ());
    NSCRIPT_ADDCMD('EIGE', bool, IsGameplayUpdatingEnabled , 0, (), 0, ());
    NSCRIPT_ADDCMD('EESS', void, EnableSoundSources , 0, (), 0, ());
    NSCRIPT_ADDCMD('EDSS', void, DisableSoundSources , 0, (), 0, ());
    NSCRIPT_ADDCMD('EASE', bool, AreSoundSourcesEnabled , 0, (), 0, ());
    NSCRIPT_ADDCMD('EGUF', int, GetUpdateFrequency , 0, (), 0, ());
    NSCRIPT_ADDCMD('ESUF', void, SetUpdateFrequency, 1, (int), 0, ());
    NSCRIPT_ADDCMD('EEGD', void, EnableGameplayDebugging , 0, (), 0, ());
    NSCRIPT_ADDCMD('EDGD', void, DisableGameplayDebugging , 0, (), 0, ());
    NSCRIPT_ADDCMD('EIDE', bool, IsGameplayDebuggingEnabled , 0, (), 0, ());
    NSCRIPT_ADDCMD('EAPE', void, ApplyPendingEvent , 0, (), 0, ());
    NSCRIPT_ADDCMD('EDPE', void, DiscardPendingEvent , 0, (), 0, ());
    NSCRIPT_ADDCMD('EGPE', void, GetPendingEvent, 0, (), 2, (nEntityObjectId&, nString&));


    cl->BeginSignals( 5 );
    N_INITCMDS_ADDSIGNAL( GameplayUpdatingEnabled );
    N_INITCMDS_ADDSIGNAL( GameplayUpdatingDisabled );
    N_INITCMDS_ADDSIGNAL( SoundSourcesEnabled );
    N_INITCMDS_ADDSIGNAL( SoundSourcesDisabled );
    N_INITCMDS_ADDSIGNAL( PendingEventChanged );
    cl->EndSignals();
#endif
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
