#include "precompiled/pchframework.h"
//------------------------------------------------------------------------------
//  nlevelmanager_cmds.cc
//  (C) 2005 Conjurer Services, S.A.
//------------------------------------------------------------------------------
#include "nlevel/nlevelmanager.h"

NSCRIPT_INITCMDS_BEGIN(nLevelManager)

    NSCRIPT_ADDCMD('NLEV', void, NewLevel, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('SLEV', bool, SaveLevel, 0, (), 0, ());
    NSCRIPT_ADDCMD('LLEV', bool, LoadLevel, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('DLEV', bool, DeleteLevel, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('FLEV', nObject*, FindLevel, 1, (const char *), 0, ());
    NSCRIPT_ADDCMD('GCLO', nObject*, GetCurrentLevelObject, 0, (), 0, ());
    NSCRIPT_ADDCMD('SLVR', void, SetLevelResource, 2, (const char *, bool), 0, ());
    NSCRIPT_ADDCMD('GLVR', const char *, GetLevelResource, 0, (), 0, ());


    cl->BeginSignals(4);
    N_INITCMDS_ADDSIGNAL(LevelCreated)
    N_INITCMDS_ADDSIGNAL(LevelLoaded)
    N_INITCMDS_ADDSIGNAL(LevelSaved)
    N_INITCMDS_ADDSIGNAL(LevelUnload)
    cl->EndSignals();

NSCRIPT_INITCMDS_END()
