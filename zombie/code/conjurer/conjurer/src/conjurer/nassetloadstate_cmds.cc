#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nassetloadstate_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nassetloadstate.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nassetloadstate

    @cppclass
    nAssetLoadState

    @superclass
    nroot

    @classinfo
    Editor state for loading a scene into conjurer.
*/
NSCRIPT_INITCMDS_BEGIN(nAssetLoadState)
    NSCRIPT_ADDCMD('SSFN', void, SetSceneFile, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GSFN', const char *, GetSceneFile, 0, (), 0, ());
    NSCRIPT_ADDCMD('SCRN', void, SetLoadClassName, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GCRN', const char *, GetLoadClassName, 0, (), 0, ());
    NSCRIPT_ADDCMD('SANS', void, SetAnimState, 1, (int), 0, ());
    NSCRIPT_ADDCMD('GANS', int, GetAnimState, 0, (), 0, ());
NSCRIPT_INITCMDS_END()
