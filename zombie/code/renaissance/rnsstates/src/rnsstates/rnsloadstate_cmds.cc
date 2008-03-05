//------------------------------------------------------------------------------
//  rnsloadstate_cmds.cc
//  (C) 2006 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "precompiled/pchrnsstates.h"

#include "rnsstates/rnsloadstate.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    rnsloadstate

    @cppclass
    RnsLoadState

    @superclass
    ncommonstate

    @classinfo
    The Load State class of Renaissance Game
*/
NSCRIPT_INITCMDS_BEGIN(RnsLoadState);
    NSCRIPT_ADDCMD('LSNS', void, SetNextState, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('LIPC', bool, InsertPreloadClass, 1, (const char*), 0, ());
    NSCRIPT_ADDCMD('LGPR', float, GetPercent, 0, (), 0, () );
NSCRIPT_INITCMDS_END();

//------------------------------------------------------------------------------
