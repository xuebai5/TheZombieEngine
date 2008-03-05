#include "precompiled/pchconjurerapp.h"
//------------------------------------------------------------------------------
//  nconjurersceneserver_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "conjurer/nconjurersceneserver.h"

//------------------------------------------------------------------------------
/**
    @scriptclass
    nconjurersceneserver

    @cppclass
    nConjurerSceneServer

    @superclass
    nstdsceneserver

    @classinfo
    A custom scene server for conjurer with embedded debug options.
*/
NSCRIPT_INITCMDS_BEGIN(nConjurerSceneServer)
    NSCRIPT_ADDCMD('SDBE', void, SetDebugEnabled, 1, (bool), 0, ());
    NSCRIPT_ADDCMD('GDBE', bool, GetDebugEnabled, 0, (), 0, ());
    NSCRIPT_ADDCMD('SRPE', void, SetPassEnabled, 2, (const char *, bool), 0, ());
    NSCRIPT_ADDCMD('GRPE', bool, GetPassEnabled, 1, (const char *), 0, ());
NSCRIPT_INITCMDS_END()
