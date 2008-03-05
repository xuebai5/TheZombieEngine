#include "precompiled/pchncommonapp.h"
//------------------------------------------------------------------------------
//  ncommonstate_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "ncommonapp/ncommonstate.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    ncommonstate

    @cppclass
    nCommonState

    @superclass
    ncommonstate

    @classinfo
    Base class for application states with nebula gui
*/
NSCRIPT_INITCMDS_BEGIN(nCommonState)
    NSCRIPT_ADDCMD('JCSL', void, SetOnScreenLogUpdateInterval, 1, (float), 0, ());
    NSCRIPT_ADDCMD('JUSL', void, UpdateOnScreenLog, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('JTLO', void, TestLog, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END()
