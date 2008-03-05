#include "precompiled/pchncsound.h"
/*-----------------------------------------------------------------------------
    @file nsoundscheduler_cmds.cc

    @author Juan Jose Luna

    @brief nSoundScheduler persistence and scripting

    (C) 2005 Conjurer Services, S.A.

*/
//------------------------------------------------------------------------------
#include "nsoundscheduler/nsoundscheduler.h"
#include "kernel/npersistserver.h"

//------------------------------------------------------------------------------
/**
    Nebula class scripting initialization
*/
NSCRIPT_INITCMDS_BEGIN( nSoundScheduler )
    NSCRIPT_ADDCMD('JRST', void, Reset, 0, (), 0, ());
    NSCRIPT_ADDCMD('JLSL', void, LoadSoundLibrary, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('JLAS', void, LoadAllSamples, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('JSAS', void, StopAllSounds , 0, (), 0, ());
    NSCRIPT_ADDCMD('JSHC', void, SetHearingViewport, 1, (nAppViewport *), 0, ());
    NSCRIPT_ADDCMD('JGHC', nAppViewport *, GetHearingViewport , 0, (), 0, ());
    NSCRIPT_ADDCMD('FPEE', void, PlayEntity, 5, (nEntityObject*, int, int, nTime, bool), 0, ());
    NSCRIPT_ADDCMD('FPLS', void, Play, 5, (vector3, int, int, float, bool), 0, ());
    #ifndef NGAME
    NSCRIPT_ADDCMD('JSNS', void, ScanNewSounds , 3, (nString&, bool, bool), 0, ());
    #endif NGAME
NSCRIPT_INITCMDS_END()

//------------------------------------------------------------------------------
/**
    SaveCmds
*/
bool
nSoundScheduler::SaveCmds (nPersistServer* /*ps*/)
{
    return true;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
