#include "precompiled/pchrnsstates.h"
//------------------------------------------------------------------------------
//  rnsgamestate_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "rnsstates/rnsgamestate.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    rnsgamestate

    @cppclass
    RnsGameState

    @superclass
    rnsgamestate

    @classinfo
    The Game State class of Renaissance Game
*/
NSCRIPT_INITCMDS_BEGIN(RnsGameState);
    NSCRIPT_ADDCMD('LGMX', float, GetMouseXFactor , 0, (), 0, ());
    NSCRIPT_ADDCMD('LSMX', void, SetMouseXFactor, 1, (float), 0, ());
    NSCRIPT_ADDCMD('LGMY', float, GetMouseYFactor , 0, (), 0, ());
    NSCRIPT_ADDCMD('LSMY', void, SetMouseYFactor, 1, (float), 0, ());
    NSCRIPT_ADDCMD('LGYI', bool, GetMouseYInvert , 0, (), 0, ());
    NSCRIPT_ADDCMD('LSYI', void, SetMouseYInvert, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('SAMS', void, SaveMouseSettings, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('LAMS', void, LoadMouseSettings, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('LSCM', void, SetCrossMode, 1, (int), 0, ());
    NSCRIPT_ADDCMD('LGCM', int, GetCrossMode , 0, (), 0, ());
    NSCRIPT_ADDCMD('LSRT', void, SetFullReloadTime, 1, (float), 0, ());
    NSCRIPT_ADDCMD('LGRT', float, GetFullReloadTime , 0, (), 0, ());
    NSCRIPT_ADDCMD('LSNS', void, SetNextState, 1, (const nString&), 0, ());
    NSCRIPT_ADDCMD('LEXT', void, Exit, 0, (), 0, ());
    NSCRIPT_ADDCMD('LICW', void, InitConnectionWith, 2, (const nString&, const nString&), 0, ());
NSCRIPT_INITCMDS_END();